#include "config.h"
#include "libbeye/libbeye.h"
using namespace	usr;
/**
 * @namespace	usr
 * @file        info_win.c
 * @brief       This file contains information interface of BEYE project.
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
 * @author      Mauro Giachero
 * @date        02.11.2007
 * @note        Added ASSEMBle option to casmtext
**/
#include <stdio.h>
#include <string.h>

#include "beye.h"
#include "colorset.h"
#include "bmfile.h"
#include "tstrings.h"
#include "reg_form.h"
#include "bconsole.h"
#include "beyeutil.h"
#include "beyehelp.h"
#include "libbeye/kbd_code.h"
#include "libbeye/osdep/tconsole.h"
#include "plugins/plugin.h"

namespace	usr {
static void  __FASTCALL__ ShowFunKey(TWindow* w,const char * key,const char * text)
{
 w->set_color(prompt_cset.digit);
 w->puts(key);
 w->set_color(prompt_cset.button);
 w->puts(text);
}

static const char * ftext[] = { "1"," 2"," 3"," 4"," 5"," 6"," 7"," 8"," 9","10" };

static void  __FASTCALL__ drawControlKeys(TWindow* w,int flg)
{
  char ckey;
  if(flg & KS_SHIFT) ckey = 'S';
  else
    if(flg & KS_ALT) ckey = 'A';
    else
      if(flg & KS_CTRL) ckey = 'C';
      else              ckey = ' ';
  w->goto_xy(1,1);
  w->set_color(prompt_cset.control);
  w->putch(ckey);
}

void __FASTCALL__ __drawMultiPrompt(const char * const norm[], const char *const shift[], const char * const alt[], const char * const ctrl[])
{
  TWindow *_using;
  int flg = beye_context().tconsole().kbd_get_shifts();
  int i;
  const char * cptr;
  HelpWnd->freeze();
  HelpWnd->goto_xy(2,1);
  for(i = 0;i < 10;i++)
  {
    /* todo: it might be better to ensure that if
       text!=NULL then text[i]!=NULL, rather than
       checking it all the time
     */
    if (flg & KS_SHIFT)
	cptr = shift && shift[i] && shift[i][0] ? shift[i] : "      ";
    else if (flg & KS_ALT)
	cptr = alt && alt[i] && alt[i][0] ? alt[i] : "      ";
    else if (flg & KS_CTRL)
	cptr = ctrl && ctrl[i] && ctrl[i][0] ? ctrl[i] : "      ";
    else cptr = norm && norm[i] && norm[i][0] ? norm[i] : "      ";

    ShowFunKey(HelpWnd,ftext[i],cptr);
  }
  drawControlKeys(HelpWnd,flg);
  HelpWnd->refresh();
}

void __FASTCALL__ __drawSinglePrompt(const char *prmt[])
{
  __drawMultiPrompt(prmt, NULL, NULL, NULL);
}


static const char * ShiftFxText[] =
{
  "ModHlp",
  "      ",
  "      ",
  "      ",
  "Where ",
  "SysInf",
  "NextSr",
  "Tools ",
  "      ",
  "FilUtl"
};

static const char * FxText[] =
{
  "Intro ",
  "ViMode",
  "More  ",
  "      ",
  "Goto  ",
  "ReRead",
  "Search",
  "      ",
  "Setup ",
  "Quit  "
};

static void  fillFxText()
{
  FxText[3] = beye_context().active_mode().misckey_name();
  FxText[7] = "Header";
}

void drawPrompt()
{
    fillFxText();
    const char* prmt[10];
    const char* fprmt[10];
    size_t i;
    for(i=0;i<10;i++) prmt[i]=beye_context().active_mode().prompt(i);
    for(i=0;i<10;i++) fprmt[i]=beye_context().bin_format().prompt(i);
    __drawMultiPrompt(FxText, ShiftFxText, fprmt, prmt);
}

static const char * amenu_names[] =
{
   "~Base",
   "~Alternative",
   "~Format-depended",
   "~Mode-depended"
};

int MainActionFromMenu()
{
    const char* prmt[10];
    size_t j;
  unsigned nModes;
  int i;
  nModes = sizeof(amenu_names)/sizeof(char *);
  i = SelBoxA(amenu_names,nModes," Select action: ",0);
  if(i != -1)
  {
    switch(i)
    {
	default:
	case 0:
		fillFxText();
		i = SelBoxA(FxText,10," Select base action: ",0);
		if(i!=-1) return KE_F(i+1);
		break;
	case 1:
		i = SelBoxA(ShiftFxText,10," Select alternative action: ",0);
		if(i!=-1) return KE_SHIFT_F(i+1);
		break;
	case 2:
		for(j=0;j<10;j++) prmt[j]=beye_context().bin_format().prompt(i);
		i = SelBoxA(prmt,10," Select format-depended action: ",0);
		if(i!=-1) return KE_ALT_F(i+1);
		break;
	case 3:
		for(j=0;j<10;j++) prmt[j]=beye_context().active_mode().prompt(i);
		i = SelBoxA(prmt,10," Select mode-depended action: ",0);
		if(i!=-1) return KE_CTL_F(i+1);
		break;
    }
  }
  return 0;
}

static const char * fetext[] =
{
  "Help  ",
  "Update",
  "InitXX",
  "Not   ",
  "Or  XX",
  "And XX",
  "Xor XX",
  "Put XX",
  "Undo  ",
  "Escape"
};

static const char * casmtext[] =
{
  "AsmRef",
  "SysInf",
  "Tools ",
  "Assemb",
  "      ",
  "      ",
  "      ",
  "      ",
  "      ",
  "      "
};

static const char * empttext[] =
{
  "      ",
  "      ",
  "      ",
  "      ",
  "      ",
  "      ",
  "      ",
  "      ",
  "      ",
  "Escape"
};
static const char * emptlsttext[] =
{
  "      ",
  "      ",
  "      ",
  "SaveAs",
  "      ",
  "      ",
  "      ",
  "      ",
  "      ",
  "      "
};

void drawEditPrompt()
{
  __drawSinglePrompt(fetext);
}

/*
int EditActionFromMenu()
{
  int i;
  i = SelBoxA(fetext,10," Select editor's action: ",0);
  if(i != -1) return KE_F(i+1);
  return 0;
}
*/

void drawEmptyPrompt()
{
  __drawSinglePrompt(empttext);
}

void drawEmptyListPrompt()
{
  __drawSinglePrompt(emptlsttext);
}

void drawAsmEdPrompt()
{
  __drawMultiPrompt(fetext, NULL, NULL, casmtext);
}

int EditAsmActionFromMenu()
{
  int i;
  i = SelBoxA(amenu_names,2," Select asm editor's action: ",0);
  if(i != -1)
  {
    switch(i)
    {
	default:
	case 0:
		fillFxText();
		i = SelBoxA(fetext,10," Select base action: ",0);
		if(i!=-1) return KE_F(i+1);
		break;
	case 1:
		i = SelBoxA(casmtext,10," Select alternative action: ",0);
		if(i!=-1) return KE_CTL_F(i+1);
		break;
    }
  }
  return 0;
}

static const char * ordlisttxt[] =
{
  "      ",
  "SrtNam",
  "SrtOrd",
  "SaveAs",
  "      ",
  "      ",
  "Search",
  "      ",
  "      ",
  "Escape"
};

static const char * listtxt[] =
{
  "      ",
  "Sort  ",
  "      ",
  "SaveAs",
  "      ",
  "      ",
  "Search",
  "      ",
  "      ",
  "Escape"
};

static const char * searchlisttxt[] =
{
  "      ",
  "      ",
  "      ",
  "SaveAs",
  "      ",
  "      ",
  "Search",
  "      ",
  "      ",
  "Escape"
};

static const char * shlisttxt[] =
{
  "      ",
  "      ",
  "      ",
  "      ",
  "      ",
  "      ",
  "NextSr",
  "      ",
  "      ",
  "      "
};

static const char * helptxt[] =
{
  "Licenc",
  "KeyHlp",
  "Credit",
  "      ",
  "      ",
  "      ",
  "      ",
  "      ",
  "      ",
  "Escape"
};

static const char * helplisttxt[] =
{
  "      ",
  "      ",
  "      ",
  "      ",
  "      ",
  "      ",
  "Search",
  "      ",
  "      ",
  "Escape"
};

void drawListPrompt()
{
  __drawMultiPrompt(listtxt, shlisttxt, NULL, NULL);
}

void drawOrdListPrompt()
{
  __drawMultiPrompt(ordlisttxt, shlisttxt, NULL, NULL);
}

void drawSearchListPrompt()
{
  __drawMultiPrompt(searchlisttxt, shlisttxt, NULL, NULL);
}

void drawHelpPrompt()
{
  __drawSinglePrompt(helptxt);
}

int HelpActionFromMenu()
{
  int i;
  i = SelBoxA(helptxt,10," Select help action: ",0);
  if(i != -1) return KE_F(i+1);
  return 0;
}

void drawHelpListPrompt()
{
  __drawMultiPrompt(helplisttxt, shlisttxt, NULL, NULL);
}

typedef struct tagvbyte
{
  char x;
  char y;
  char image;
}vbyte;

typedef struct tagcvbyte
{
  char x;
  char y;
  char image;
  Color color;
}cvbyte;

const vbyte stars[] = {
{ 51,8,'.' },
{ 48,6,'*' },
{ 50,11,'*' },
{ 49,9,'.' },
{ 69,6,'*' },
{ 72,11,'*' },
{ 68,10,'.' },
{ 71,8,'.' },
{ 70,12,'*' },
{ 48,12,'.' }
};

const cvbyte buttons[] = {
{ 65,12,0x07, LightRed },
{ 66,12,0x07, LightGreen },
{ 67,12,0x07, LightCyan },
{ 55,12,TWC_SH, Black },
{ 56,12,TWC_SH, Black }
};

void About()
{
 TWindow * hwnd;
 unsigned i,j,len;
 char str[2];
 const unsigned char core[8] = { TWC_LT_SHADE, TWC_LT_SHADE, TWC_LT_SHADE, TWC_LT_SHADE, TWC_LT_SHADE, TWC_LT_SHADE, TWC_LT_SHADE, 0x00 };
 hwnd = WindowOpen(0,0,74,14,TWindow::Flag_Has_Frame | TWindow::Flag_NLS);
 hwnd->into_center();
 hwnd->set_color(LightCyan,Black);
 hwnd->clear();
 hwnd->set_frame(TWindow::DOUBLE_FRAME,White,Black);
 hwnd->set_title(BEYE_VER_MSG,TWindow::TMode_Center,White,Black);
 hwnd->show();

 hwnd->freeze();
 hwnd->goto_xy(1,1); hwnd->puts(msgAboutText);
 hwnd->text_color(White);
 for(i = 0;i < 13;i++)  { hwnd->goto_xy(47,i + 1); hwnd->putch(TWC_SV); }
 for(i = 0;i < 47;i++) { hwnd->goto_xy(i + 1,6); hwnd->putch(TWC_SH);  }
 hwnd->goto_xy(47,6); hwnd->putch(TWC_SV_Sl);
 for(i = 0;i < 5;i++)
 {
   len=strlen(BeyeLogo[i]);
   for(j=0;j<len;j++) {
    hwnd->text_color(BeyeLogo[i][j]=='0'?Green:LightGreen);
    hwnd->goto_xy(49+j,i + 1);
    hwnd->putch(BeyeLogo[i][j]);
   }
 }
 hwnd->text_color(LightGreen); hwnd->text_bkgnd(Green);
 for(i = 0;i < 7;i++)
 {
   hwnd->goto_xy(1,i+7); hwnd->puts(MBoardPicture[i]);
 }
 hwnd->draw_frame(3,8,13,12,TWindow::UP3D_FRAME,White,LightGray);
 hwnd->draw_frame(4,9,12,11,TWindow::DN3D_FRAME,Black,LightGray);
 hwnd->goto_xy(5,10);
 hwnd->puts((const char*)core);
 hwnd->text_color(Brown); hwnd->text_bkgnd(Black);
 for(i = 0;i < 7;i++)
 {
   hwnd->goto_xy(17,i+7); hwnd->puts(ConnectorPicture[i]);
 }
 hwnd->text_color(Gray); hwnd->text_bkgnd(Black);
 for(i = 0;i < 7;i++)
 {
   hwnd->goto_xy(22,i+7); hwnd->puts(BitStreamPicture[i]);
 }
 hwnd->text_color(LightGray); hwnd->text_bkgnd(Black);
 for(i = 0;i < 7;i++)
 {
   hwnd->goto_xy(31,i+7); hwnd->puts(BeyePicture[i]);
 }
 hwnd->text_color(LightCyan); hwnd->text_bkgnd(Blue);
 for(i = 0;i < 5;i++)
 {
   hwnd->goto_xy(32,i+8); hwnd->puts(BeyeScreenPicture[i]);
 }
 hwnd->text_bkgnd(Black);   hwnd->text_color(LightCyan);
 for(i = 0;i < 10;i++) hwnd->direct_write(stars[i].x,stars[i].y,&stars[i].image,1);
 hwnd->text_color(LightGray);
 for(i = 0;i < 7;i++)
 {
   hwnd->goto_xy(52,i + 6);
   hwnd->puts(CompPicture[i]);
 }
 hwnd->text_bkgnd(LightGray);
 for(i = 0;i < 5;i++)
 {
   hwnd->text_color(buttons[i].color);
   str[0] = buttons[i].image;
   str[1] = 0;
   hwnd->direct_write(buttons[i].x,buttons[i].y,str,1);
 }
 hwnd->set_color(LightCyan,Black);
 for(i = 0;i < 4;i++)
 {
   hwnd->goto_xy(54,i + 7);
   hwnd->puts(CompScreenPicture[i]);
 }
 hwnd->refresh();
 while(1)
 {
   int ch;
   ch = GetEvent(drawHelpPrompt,HelpActionFromMenu,hwnd);
   switch(ch)
   {
      case KE_ESCAPE:
      case KE_F(10):  goto bye_help;
      case KE_F(1):   hlpDisplay(1); break;
      case KE_F(2):   hlpDisplay(3); break;
      case KE_F(3):   hlpDisplay(4); break;
      default:        break;
   }
 }
 bye_help:
 delete hwnd;
}

__filesize_t __FASTCALL__ WhereAMI(__filesize_t ctrl_pos)
{
  TWindow *hwnd,*wait_wnd;
  char vaddr[64],prev_func[61],next_func[61],oname[25];
  const char *btn;
  int obj_class,obj_bitness;
  unsigned obj_num,func_class;
  __filesize_t obj_start,obj_end;
  __filesize_t cfpos,ret_addr,va,prev_func_pa,next_func_pa;
  hwnd = CrtDlgWndnls(" Current position information ",78,5);
  hwnd->set_footer("[Enter] - Prev. entry [Ctrl-Enter | F5] - Next entry]",TWindow::TMode_Right,dialog_cset.selfooter);
  hwnd->goto_xy(1,1);
  wait_wnd = PleaseWaitWnd();
  cfpos = BMGetCurrFilePos();
  va = beye_context().bin_format().pa2va(ctrl_pos);
  if(va==Bin_Format::Bad_Address) va = ctrl_pos;
  vaddr[0] = '\0';
  sprintf(&vaddr[strlen(vaddr)],"%016llXH",va);
  prev_func_pa = next_func_pa = 0;
  prev_func[0] = next_func[0] = '\0';
  prev_func_pa = beye_context().bin_format().get_public_symbol(prev_func,sizeof(prev_func),
					      &func_class,ctrl_pos,true);
  next_func_pa = beye_context().bin_format().get_public_symbol(next_func,sizeof(next_func),
					      &func_class,ctrl_pos,false);
  prev_func[sizeof(prev_func)-1] = next_func[sizeof(next_func)-1] = '\0';
  obj_num = beye_context().bin_format().get_object_attribute(ctrl_pos,oname,sizeof(oname),
					  &obj_start,&obj_end,&obj_class,
					  &obj_bitness);
  oname[sizeof(oname)-1] = 0;
  if(!obj_num)
  {
    obj_num = 0;
    oname[0] = 0;
    obj_start = 0;
    obj_end = BMGetFLength();
    obj_class = OC_CODE;
    obj_bitness = DAB_USE16;
  }
  delete wait_wnd;
  switch(obj_bitness)
  {
    case DAB_USE16: btn = "USE16"; break;
    case DAB_USE32: btn = "USE32"; break;
    case DAB_USE64: btn = "USE64"; break;
    case DAB_USE128:btn = "USE128"; break;
    case DAB_USE256:btn = "USE256"; break;
    default: btn = "";
  }
  hwnd->printf(
	   "File  offset : %016llXH\n"
	   "Virt. address: %s\n"
	   "%s entry  : %s\n"
	   "Next  entry  : %s\n"
	   "Curr. object : #%u %s %s %016llXH=%016llXH %s"
	   ,ctrl_pos
	   ,vaddr
	   ,prev_func_pa == ctrl_pos ? "Curr." : "Prev."
	   ,prev_func
	   ,next_func
	   ,obj_num
	   ,obj_class == OC_CODE ? "CODE" : obj_class == OC_DATA ? "DATA" : "no obj."
	   ,btn
	   ,obj_start
	   ,obj_end
	   ,oname
	   );
  ret_addr = ctrl_pos;
  while(1)
  {
    int ch;
    ch = GetEvent(drawEmptyPrompt,NULL,hwnd);
    switch(ch)
    {
      case KE_F(10):
      case KE_ESCAPE: goto exit;
      case KE_ENTER:
		    {
		      if(prev_func_pa)
		      {
			ret_addr = prev_func_pa;
		      }
		      else ErrMessageBox(NOT_ENTRY,"");
		    }
		    goto exit;
      case KE_F(5):
      case KE_CTL_ENTER:
		    {
		      if(next_func_pa)
		      {
			ret_addr = next_func_pa;
		      }
		      else ErrMessageBox(NOT_ENTRY,"");
		    }
		    goto exit;
      default: break;
    }
  }
  exit:
  BMSeek(cfpos,binary_stream::Seek_Set);
  delete hwnd;
  return ret_addr;
}
} // namespace	usr
