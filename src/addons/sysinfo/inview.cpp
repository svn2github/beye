#include "config.h"
#include "libbeye/libbeye.h"
#include "libbeye/osdep/tconsole.h"
using namespace	usr;
/**
 * @namespace	usr_addons
 * @file        addons/sys/kbdview.c
 * @brief       This file contains simple implementation console information.
 * @version     -
 * @remark      this source file is part of Binary EYE project (BEYE).
 *              The Binary EYE (BEYE) is copyright (C) 1995 Nickols_K.
 *              All rights reserved. This software is redistributable under the
 *              licence given in the file "Licence.en" ("Licence.ru" in russian
 *              translation) distributed in the BEYE archive.
 * @note        Requires POSIX compatible development system
 *
 * @author      Nickols_K
 * @since       2003
 * @note        Development, fixes and improvements
**/
#include <string.h>
#include <stddef.h>

#include "beye.h"

#include "addons/addon.h"

#include "colorset.h"
#include "bconsole.h"
#include "beyeutil.h"
#include "libbeye/libbeye.h"
#include "libbeye/kbd_code.h"

namespace	usr {
    class InputView_Addon : public Addon {
	public:
	    InputView_Addon(BeyeContext& bc);
	    virtual ~InputView_Addon();
	
	    virtual void	run();
	private:
	    BeyeContext&	bctx;
    };

InputView_Addon::InputView_Addon(BeyeContext& bc):Addon(bc),bctx(bc) {}
InputView_Addon::~InputView_Addon() {}

void InputView_Addon::run()
{
  TWindow * hwnd = CrtDlgWndnls(" Input viewer ",78,2);
  int rval, do_exit;
  char head[80], text[80];
  drawEmptyPrompt();
  hwnd->freeze();
  hwnd->set_footer(" [Escape] - quit ",TWindow::TMode_Right,dialog_cset.selfooter);
  hwnd->refresh();
  do_exit=0;
  do
  {
    rval = bctx.tconsole().input_raw_info(head,text);
    if(rval==-1)
    {
	bctx.ErrMessageBox("Not implemented yet!","");
	break;
    }
    hwnd->goto_xy(1,1);
    hwnd->puts(head);
    hwnd->clreol();
    hwnd->goto_xy(1,2);
    hwnd->puts(text);
    hwnd->clreol();
    if(!rval) do_exit++;
  }
  while(do_exit<2);
  delete hwnd;
}

static Addon* query_interface(BeyeContext& bc) { return new(zeromem) InputView_Addon(bc); }
extern const Addon_Info InputViewer = {
    "~Input viewer",
    query_interface
};
} // namespace	usr
