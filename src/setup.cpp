#include "config.h"
#include "libbeye/libbeye.h"
using namespace beye;
/**
 * @namespace   beye
 * @file        setup.c
 * @brief       This file contains BEYE setup.
 * @version     -
 * @remark      this source file is part of Binary EYE project (BEYE).
 *              The Binary EYE (BEYE) is copyright (C) 1995 Nickols_K.
 *              All rights reserved. This software is redistributable under the
 *              licence given in the file "Licence.en" ("Licence.ru" in russian
 *              translation) distributed in the BEYE archive.
 * @note        Requires POSIX compatible development system
 *
 * @author      Nickols_K
 * @since       1999
 * @note        Development, fixes and improvements
**/
#include <string.h>
#include <stdio.h>

#include "beyehelp.h"
#include "colorset.h"
#include "setup.h"
#include "bconsole.h"
#include "beyeutil.h"
#include "libbeye/twin.h"
#include "libbeye/kbd_code.h"

#ifdef __QNX4__
extern int photon,bit7;
#endif
namespace beye {
    extern char beye_help_name[];
    extern char beye_skin_name[];
    extern char beye_syntax_name[];
    extern char beye_codepage[];

    extern unsigned long beye_vioIniFlags;
    extern unsigned long beye_twinIniFlags;
    extern unsigned long beye_kbdFlags;
    extern bool iniSettingsAnywhere;
    extern bool fioUseMMF;
    extern bool iniPreserveTime;
    extern bool iniUseExtProgs;

char * beyeGetHelpName( void )
{
  if(!beye_help_name[0])
  {
    strcpy(beye_help_name,__get_rc_dir("beye"));
    strcat(beye_help_name,"beye.hlp");
  }
  return beye_help_name;
}

static char * __NEAR__ __FASTCALL__ beyeGetColorSetName( void )
{
  if(!beye_skin_name[0])
  {
    strcpy(beye_skin_name,__get_rc_dir("beye"));
    strcat(beye_skin_name,"skn/" "standard.skn"); /* [dBorca] in skn/ subdir */
  }
  return beye_skin_name;
}

static char * __NEAR__ __FASTCALL__ beyeGetSyntaxName( void )
{
  if(!beye_syntax_name[0])
  {
    strcpy(beye_syntax_name,__get_rc_dir("beye"));
    strcat(beye_syntax_name,"syntax/" "syntax.stx"); /* [dBorca] in syntax/ subdir */
  }
  return beye_syntax_name;
}


static const char * setuptxt[] =
{
  "Help  ",
  "Consol",
  "Color ",
  "Mouse ",
  "Bit   ",
  "Plugin",
  "MMF   ",
  "Time  ",
  "ExtPrg",
  "Escape"
};

static unsigned default_cp = 15;
static const char * cp_list[] =
{
  "CP437 - original IBM PC Latin US",
  "CP708 - Arabic language",
  "CP737 - Greek language",
  "CP775 - Baltic languages",
  "CP850 - IBM PC MSDOS Latin-1",
  "CP851 - IBM PC Greek-1",
  "CP852 - Central European languages (Latin-2)",
  "CP855 - Serbian language",
  "CP857 - Turkish language",
  "CP858 - Western European languages",
  "CP860 - Portuguese language",
  "CP861 - Icelandic language",
  "CP862 - Hebrew language",
  "CP863 - French language",
  "CP865 - Nordic languages",
  "CP866 - Cyrillic languages",
  "CP869 - IBM PC Greek-2",
};

static bool __FASTCALL__ select_codepage( void )
{
  unsigned nModes;
  int i;
  nModes = sizeof(cp_list)/sizeof(char *);
  i = SelBoxA(const_cast<char**>(cp_list),nModes," Select single-byte codepage: ",default_cp);
  if(i != -1)
  {
    unsigned len;
    const char *p;
    default_cp = i;
    p = strchr(cp_list[i],' ');
    len = p-cp_list[i];
    memcpy(beye_codepage,cp_list[i],len);
    beye_codepage[len] = '\0';
    return true;
  }
  return false;
}

static void drawSetupPrompt( void )
{
   __drawSinglePrompt(setuptxt);
}

static void __NEAR__ __FASTCALL__ setup_paint( TWindow *twin )
{
  TWindow *usd;
  usd = twUsedWin();
  twUseWin(twin);
  twSetColorAttr(dialog_cset.group.active);
  twGotoXY(2,9);
  twPrintF(" [%c] - Direct console access "
	   ,Gebool((beye_vioIniFlags & __TVIO_FLG_DIRECT_CONSOLE_ACCESS) == __TVIO_FLG_DIRECT_CONSOLE_ACCESS));
  twGotoXY(2,10);
  twPrintF(" [%c] - Mouse sensitivity     "
	   ,Gebool((beye_kbdFlags & KBD_NONSTOP_ON_MOUSE_PRESS) == KBD_NONSTOP_ON_MOUSE_PRESS));
  twGotoXY(2,11);
  twPrintF(" [%c] - Force mono            "
	   ,Gebool((beye_twinIniFlags & TWIF_FORCEMONO) == TWIF_FORCEMONO));
  twGotoXY(2,12);
#ifdef __QNX4__
  if(photon)
  {
    twSetColorAttr(dialog_cset.group.disabled);
    twPrintF(" [%c] - Force 7-bit output    "
	    ,Gebool(bit7));
    twSetColorAttr(dialog_cset.group.active);
  }
  else
#endif
  twPrintF(" [%c] - Force 7-bit output    "
	   ,Gebool((beye_vioIniFlags & __TVIO_FLG_USE_7BIT) == __TVIO_FLG_USE_7BIT));
  twGotoXY(32,9);
  twPrintF(" [%c] - Apply plugin settings to all files     "
	   ,Gebool(iniSettingsAnywhere));
  twGotoXY(32,10);
  if(!__mmfIsWorkable()) twSetColorAttr(dialog_cset.group.disabled);
  twPrintF(" [%c] - Use MMF                                "
	   ,Gebool(fioUseMMF));
  twSetColorAttr(dialog_cset.group.active);
  twGotoXY(32,11);
  twPrintF(" [%c] - Preserve timestamp                     "
	   ,Gebool(iniPreserveTime));
  twGotoXY(32,12);
  twPrintF(" [%c] - Enable usage of external programs      "
	   ,Gebool(iniUseExtProgs));
  twSetColorAttr(dialog_cset.main);
  twGotoXY(50,7); twPutS(beye_codepage);
  twUseWin(usd);
}

void Setup(void)
{
  tAbsCoord x1,y1,x2,y2;
  tRelCoord X1,Y1,X2,Y2;
  int ret;
  TWindow * wdlg,*ewnd[4];
  char estr[3][FILENAME_MAX+1];
  int active = 0;
  strcpy(estr[0],beyeGetHelpName());
  strcpy(estr[1],beyeGetColorSetName());
  strcpy(estr[2],beyeGetSyntaxName());
  wdlg = CrtDlgWndnls(" Setup ",78,13);
  twGetWinPos(wdlg,&x1,&y1,&x2,&y2);
  X1 = x1;
  Y1 = y1;
  X2 = x2;
  Y2 = y2;

  X1 += 2;
  X2 -= 1;
  Y1 += 2;
  Y2 = Y1;
  ewnd[0] = CreateEditor(X1,Y1,X2,Y2,TWS_CURSORABLE | TWS_NLSOEM);
  twShowWin(ewnd[0]);
  twUseWin(ewnd[0]);
  PostEvent(KE_ENTER);
  xeditstring(estr[0],NULL,sizeof(estr[0]), NULL);
  Y1 += 2;
  Y2 = Y1;
  ewnd[1] = CreateEditor(X1,Y1,X2,Y2,TWS_CURSORABLE | TWS_NLSOEM);
  twShowWin(ewnd[1]);
  twUseWin(ewnd[1]);
  PostEvent(KE_ENTER);
  xeditstring(estr[1],NULL,sizeof(estr[1]), NULL);

  Y1 += 2;
  Y2 = Y1;
  ewnd[2] = CreateEditor(X1,Y1,X2,Y2,TWS_CURSORABLE | TWS_NLSOEM);
  twShowWin(ewnd[2]);
  twUseWin(ewnd[2]);
  PostEvent(KE_ENTER);
  xeditstring(estr[2],NULL,sizeof(estr[2]), NULL);

  Y1 += 2;
  Y2 = Y1;
  ewnd[3] = WindowOpen(60,Y1,61,Y2,TWS_NLSOEM);

  twUseWin(wdlg);
  twGotoXY(2,1); twPutS("Enter help file name (including full path):");
  twGotoXY(2,3); twPutS("Enter color skin name (including full path):");
  twGotoXY(2,5); twPutS("Enter syntax name (including full path):");
  twGotoXY(2,7); twPutS("Enter OEM codepage (for utf-based terminals):");
  twSetFooterAttr(wdlg," [Enter] - Accept changes ",TW_TMODE_CENTER,dialog_cset.footer);
  twinDrawFrameAttr(1,8,78,13,TW_UP3D_FRAME,dialog_cset.main);

  setup_paint(wdlg);
  active = 0;
  twUseWin(ewnd[active]);
  while(1)
  {
   if(active==3) {
	if(select_codepage() == true) setup_paint(wdlg);
	ret = KE_TAB;
   }
   else ret = xeditstring(estr[active],NULL,sizeof(estr[active]),drawSetupPrompt);
   switch(ret)
   {
     case KE_F(10):
     case KE_ESCAPE: ret = 0; goto exit;
     case KE_ENTER: ret = 1; goto exit;
     case KE_SHIFT_TAB:
     case KE_TAB:   active++;
		    if(active>3) active=0;
		    twUseWin(ewnd[active]);
		    continue;
     case KE_F(1):  hlpDisplay(5);
		    break;
     case KE_F(2):  beye_vioIniFlags ^= __TVIO_FLG_DIRECT_CONSOLE_ACCESS;
		    break;
     case KE_F(3):  beye_twinIniFlags ^= TWIF_FORCEMONO ;
		    break;
     case KE_F(4):  beye_kbdFlags ^= KBD_NONSTOP_ON_MOUSE_PRESS;
		    break;
     case KE_F(5):  beye_vioIniFlags ^= __TVIO_FLG_USE_7BIT;
		    break;
     case KE_F(6):  iniSettingsAnywhere = iniSettingsAnywhere ? false : true;
		    break;
     case KE_F(7):  if(__mmfIsWorkable()) fioUseMMF = fioUseMMF ? false : true;
		    break;
     case KE_F(8):  iniPreserveTime = iniPreserveTime ? false : true;
		    break;
     case KE_F(9):  iniUseExtProgs = iniUseExtProgs ? false : true;
		    break;
     default: continue;
   }
   setup_paint(wdlg);
  }
  exit:
  if(ret)
  {
    strcpy(beye_help_name,estr[0]);
    strcpy(beye_skin_name,estr[1]);
    strcpy(beye_syntax_name,estr[2]);
  }
  CloseWnd(ewnd[0]);
  CloseWnd(ewnd[1]);
  CloseWnd(ewnd[2]);
  CloseWnd(wdlg);
}
} // namespace beye
