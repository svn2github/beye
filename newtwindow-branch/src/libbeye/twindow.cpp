#include "config.h"
#include "libbeye/libbeye.h"
using namespace	usr;
/**
 * @namespace   libbeye
 * @file        libbeye/twin.c
 * @brief       This file contains implementation of Text Window manager.
 * @version     -
 * @remark      this source file is part of Binary EYE project (BEYE).
 *              The Binary EYE (BEYE) is copyright (C) 1995 Nickols_K.
 *              All rights reserved. This software is redistributable under the
 *              licence given in the file "Licence.en" ("Licence.ru" in russian
 *              translation) distributed in the BEYE archive.
 * @note        Requires POSIX compatible development system
 *
 * @author      Nickols_K
 * @since       1995
 * @note        Development, fixes and improvements
 * @warning     Program is destroyed, from printf misapplication
 * @bug         Limitation of printf using
 * @todo        Accelerate windows interaction algorithm
**/
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <sstream>

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#ifdef __GNUC__
#include <unistd.h>
#endif

#include "libbeye/osdep/system.h"
#include "libbeye/osdep/tconsole.h"
#include "libbeye/twindow.h"

namespace	usr {
bool TWindow::test_win() const
{
    bool ret;
    ret = TWidget::test_win() &&
	*((any_t**)(saved.get_chars() + wsize)) == saved.get_chars() &&
	*((any_t**)(saved.get_oempg() + wsize)) == saved.get_oempg() &&
	*((any_t**)(saved.get_attrs() + wsize)) == saved.get_attrs() ? true : false;
    return ret;
}

enum {
    IFLG_VISIBLE      =0x00000001UL,
    IFLG_ENABLED      =0x00000002UL,
    IFLG_CURSORBEENOFF=0x80000000UL
};

TWindow::TWindow(tAbsCoord x1, tAbsCoord y1, tAbsCoord _width, tAbsCoord _height, twc_flag _flags)
	:TWidget(x1,y1,_width,_height,_flags)
	,saved(_width*_height)
{
    TWidget::create(x1,y1,_width,_height,_flags);
}

TWindow::TWindow(tAbsCoord x1_, tAbsCoord y1_,
		 tAbsCoord _width, tAbsCoord _height,
		 twc_flag _flags, const std::string& classname)
	:TWidget(x1_,y1_,_width,_height,_flags,classname)
	,saved(_width*_height)
{
    TWidget::create(x1_, y1_, _width, _height, _flags);
}

TWindow::~TWindow()
{
}

/**
 *  Three basic functions for copying from buffer to screen:
 *  ========================================================
 *  updatescreencharfrombuff: low level implementation updatescreen family
 *  updatescreenchar:  correctly copyed user record from win to screen
 *  restorescreenchar: correctly copyed window memory from win to screen
 */

void TWindow::updatescreencharfrombuff(tRelCoord x,tRelCoord y,const tvideo_buffer& buff,tvideo_buffer* accel) const
{
    unsigned idx;
    if((iflags & IFLG_VISIBLE) == IFLG_VISIBLE) {
	idx = y*wwidth+x;
	TWindow* top=static_cast<TWindow*>(__find_over(x,y));
	if(top) {
	    unsigned tidx;
	    tAbsCoord tx,ty;
	    tx = X1 - top->X1 + x;
	    ty = Y1 - top->Y1 + y;
	    tidx = tx + ty*top->wwidth;
	    top->saved.assign_at(tidx,&buff.get_chars()[idx],&buff.get_oempg()[idx],&buff.get_attrs()[idx],1);
	}
    }
    TWidget::updatescreencharfrombuff(x,y,buff,accel);
}

/**
 *  Three basic functions for copying from buffer to screen:
 *  ========================================================
 *  updatewinmemcharfromscreen: correctly copied screen to window memory
 *  screen2win: quick implementation of copying screen to window memory
 *  snapshot:   snap shot of screen to win surface
 */
void TWindow::updatewinmemcharfromscreen(tRelCoord x,tRelCoord y,const tvideo_buffer& accel)
{
    unsigned idx,aidx;
    if((iflags & IFLG_VISIBLE) == IFLG_VISIBLE) {
	idx = y*wwidth+x;
	aidx = x;
	TWindow* top=static_cast<TWindow*>(__find_over(x,y));
	if(top) {
	    unsigned tidx;
	    tAbsCoord tx,ty;
	    tx = X1 - top->X1 + x;
	    ty = Y1 - top->Y1 + y;
	    tidx = tx + ty*top->wwidth;
	    saved.assign_at(idx,&top->saved.get_chars()[tidx],&top->saved.get_oempg()[tidx],&top->saved.get_attrs()[tidx],1);
	    top->saved.assign_at(tidx,&get_surface().get_chars()[idx],&get_surface().get_oempg()[idx],&get_surface().get_attrs()[idx],1);
	    top->check_win();
	} else {
	    saved.assign_at(idx,&accel.get_chars()[aidx],&accel.get_oempg()[aidx],&accel.get_attrs()[aidx],1);
	}
    }
}

void TWindow::screen2win()
{
    unsigned i,lwidth,idx;
    tAbsCoord inx;
    bool ms_vis;
    bool is_hidden = false;
    inx = X1;
    lwidth = wwidth;
    if(inx + lwidth > tconsole->vio_width()) lwidth = tconsole->vio_width() > inx ? tconsole->vio_width() - inx : 0;
    if(lwidth) {
	ms_vis = tconsole->mouse_get_state();
	if(ms_vis) {
	    tAbsCoord mx,my;
	    tconsole->mouse_get_pos(mx,my);
	    if(mx >= inx && mx <= X2 && my >= Y1 && my <= Y2) {
		is_hidden = true;
		tconsole->mouse_set_state(false);
	    }
	}
	if(wwidth == tconsole->vio_width() && !X1) {
	    tvideo_buffer tmp=tconsole->vio_read_buff(0, Y1, wwidth*wheight);
	    saved=tmp;
	} else {
	    for(i = 0;i < wheight;i++) {
		tAbsCoord iny;
		iny = Y1+i;
		if(iny <= tconsole->vio_height()) {
		    idx = i*wwidth;
		    tvideo_buffer tmp=tconsole->vio_read_buff(inx,iny,lwidth);
		    saved.assign_at(idx,tmp);
		} else break;
	    }
	}
	if(is_hidden) tconsole->mouse_set_state(true);
    }
    check_win();
}

/**
 *  Helpful functions:
 *  ==================
 *  savedwin2screen:  restore entire window memory to screen
 *  updatescreen:     restore entire user record of window to screen
 *  updatescreenpiece:restore one piece of line from user record of window to screen
 *  updatewinmem:     update entire window memory from screen
 */

void TWindow::savedwin2screen()
{
    unsigned i,j, tidx;
    tvideo_buffer accel(__TVIO_MAXSCREENWIDTH);
    bool ms_vis, is_hidden = false, is_top;
    if((iflags & IFLG_VISIBLE) == IFLG_VISIBLE) {
	tAbsCoord mx,my;
	ms_vis = tconsole->mouse_get_state();
	if(ms_vis) {
	    tconsole->mouse_get_pos(mx,my);
	    if(mx >= X1 && mx <= X2 && my >= Y1 && my <= Y2) {
		is_hidden = true;
		tconsole->mouse_set_state(false);
	    }
	}
	is_top = __topmost();
	if(is_top && wwidth == tconsole->vio_width() && !X1) {
	    /* Special case of redrawing window interior at one call */
	    tconsole->vio_write_buff(0, Y1, tvideo_buffer(saved.get_chars(),saved.get_oempg(),saved.get_attrs(),wwidth*wheight));
	} else {
	    for(i = 0;i < wheight;i++) {
		tAbsCoord outx,outy;
		unsigned nwidth;
		if(!is_top) for(j = 0;j < wwidth;j++) restorescreenchar(j+1,i+1,&accel);
		else {
		    tidx = i*wwidth;
		    accel.assign(&saved.get_chars()[tidx],&saved.get_oempg()[tidx],&saved.get_attrs()[tidx],saved.length()-tidx);
		}
		outx = X1;
		outy = Y1+i;
		nwidth = wwidth;
		if(outx + nwidth > tconsole->vio_width()) nwidth = tconsole->vio_width() > outx ? tconsole->vio_width() - outx : 0;
		if(outy <= tconsole->vio_height() && nwidth) tconsole->vio_write_buff(outx,outy,accel);
	    }
	}
	if(is_hidden) tconsole->mouse_set_state(true);
	check_win();
    }
}

void TWindow::updatewinmem()
{
    unsigned i,j,tidx;
    tvideo_buffer accel(__TVIO_MAXSCREENWIDTH);
    bool ms_vis, is_hidden = false, is_top;
    if((iflags & IFLG_VISIBLE) == IFLG_VISIBLE) {
	tAbsCoord mx,my;
	ms_vis = tconsole->mouse_get_state();
	if(ms_vis) {
	    tconsole->mouse_get_pos(mx,my);
	    if(mx >= X1 && mx <= X2 && my >= Y1 && my <= Y2) {
		is_hidden = true;
		tconsole->mouse_set_state(false);
	    }
	}
	is_top = __topmost();
	if(is_top && wwidth == tconsole->vio_width() && !X1) {
	    /* Special case of redrawing window interior at one call */
	    tvideo_buffer tmp=tconsole->vio_read_buff(0, Y1, wwidth*wheight);
	    saved=tmp;
	} else {
	    for(i = 0;i < wheight;i++) {
		tAbsCoord inx,iny;
		unsigned lwidth;
		lwidth = wwidth;
		inx = X1;
		iny = Y1+i;
		if(inx + lwidth > tconsole->vio_width()) lwidth = tconsole->vio_width() > inx ? tconsole->vio_width() - inx : 0;
		if(iny <= tconsole->vio_height() && lwidth) {
		    if(is_top) {
			tidx = i*wwidth;
			accel.assign(&saved.get_chars()[tidx],&saved.get_oempg()[tidx],&saved.get_attrs()[tidx],saved.length());
		    }
		    tvideo_buffer tmp=tconsole->vio_read_buff(inx,iny,lwidth);
		    accel=tmp;
		}
		if(!is_top)
		    for(j = 0;j < wwidth;j++)
			updatewinmemcharfromscreen(j,i,accel);
	    }
	}
	if(is_hidden) tconsole->mouse_set_state(true);
    }
}

void TWindow::show()
{
    if(!(iflags & IFLG_VISIBLE) == IFLG_VISIBLE) {
	updatewinmem();
	TWidget::show();
    }
}

void TWindow::show_on_top()
{
    screen2win();
    TWidget::show_on_top();
}

void TWindow::show_beneath(TWindow& prev)
{
    updatewinmem();
    TWidget::show_beneath(prev);
}

void TWindow::hide()
{
    TWidget::hide();
    savedwin2screen();
}

void TWindow::resize(tAbsCoord _width,tAbsCoord _height)
{
    size_t size=_width*_height;
    wsize = size;
    wwidth = _width;
    wheight = _height;
    saved.resize(wsize);
    TWidget::resize(_width,_height);
}
} // namespace	usr
