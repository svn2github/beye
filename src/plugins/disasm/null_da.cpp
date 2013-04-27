#include "config.h"
#include "libbeye/libbeye.h"
using namespace beye;
/**
 * @namespace   beye_plugins_II
 * @file        plugins/disasm/null_da.c
 * @brief       This file contains implementation of Data disassembler.
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
#include <stdlib.h>
#include <stdio.h>

#include "beye.h"
#include "reg_form.h"
#include "plugins/disasm.h"
#include "bconsole.h"
#include "beyehelp.h"
#include "beyeutil.h"
#include "reg_form.h"
#include "libbeye/file_ini.h"

namespace beye {
static int nulWidth = 1;

static const char *width_names[] =
{
   "~Byte",
   "~Word",
   "~Double word",
   "~Quad word"
};

static bool __FASTCALL__ nulSelect_width( void )
{
  unsigned nModes;
  int i;
  nModes = sizeof(width_names)/sizeof(char *);
  i = SelBoxA(const_cast<char**>(width_names),nModes," Select bitness mode: ",nulWidth);
  if(i != -1)
  {
    nulWidth = i;
    return true;
  }
  return false;
}

static char *outstr;

static DisasmRet __FASTCALL__ nulDisassembler(__filesize_t ulShift,
					      MBuffer buffer,
					      unsigned flags)
{
  DisasmRet ret;
  int type,cl;
  const char *preface;
  if(!((flags & __DISF_SIZEONLY) == __DISF_SIZEONLY))
  {
    memset(&ret,0,sizeof(ret));
    ret.str = outstr;
    switch(nulWidth)
    {
      case 0: preface = "db ";
	      type = DISARG_BYTE;
	      cl = 1;
	      break;
      default:
      case 1: preface = "dw ";
	      type = DISARG_WORD;
	      cl = 2;
	      break;
      case 2: preface = "dd ";
	      type = DISARG_DWORD;
	      cl = 4;
	      break;
      case 3: preface = "dq ";
	      type = DISARG_QWORD;
	      cl = 8;
	      break;
    }
    ret.codelen = cl;
    strcpy(outstr,preface);
    disAppendDigits(outstr,ulShift,APREF_USE_TYPE,cl,buffer,type);
  }
  else
    if(flags & __DISF_GETTYPE) ret.pro_clone = __INSNT_ORDINAL;
    else
    switch(nulWidth)
    {
      case 0: ret.codelen = 1; break;
      default:
      case 1: ret.codelen = 2; break;
      case 2: ret.codelen = 4; break;
      case 3: ret.codelen = 8; break;
    }
  return ret;
}

static void  __FASTCALL__ nulHelpAsm( void )
{
  hlpDisplay(20010);
}

static int    __FASTCALL__ nulMaxInsnLen( void ) { return 8; }
static ColorAttr __FASTCALL__ nulGetAsmColor( unsigned long clone )
{
  UNUSED(clone);
  return disasm_cset.cpu_cset[0].clone[0];
}
static int       __FASTCALL__ nulGetBitness( void ) { return DAB_USE16; }
static char      __FASTCALL__ nulGetClone( unsigned long clone )
{
  UNUSED(clone);
  return ' ';
}
static void      __FASTCALL__ nulInit( void )
{
  outstr = new char [1000];
  if(!outstr)
  {
    MemOutBox("Data disassembler initialization");
    exit(EXIT_FAILURE);
  }
}

static void  __FASTCALL__ nulTerm( void )
{
   delete outstr;
}

static void __FASTCALL__ nulReadIni( hIniProfile *ini )
{
  char tmps[10];
  if(beye_context().is_valid_ini_args())
  {
    beye_context().read_profile_string(ini,"Beye","Browser","SubSubMode3","1",tmps,sizeof(tmps));
    nulWidth = (int)strtoul(tmps,NULL,10);
    if(nulWidth > 3) nulWidth = 0;
  }
}

static void __FASTCALL__ nulWriteIni( hIniProfile *ini )
{
  char tmps[10];
  sprintf(tmps,"%i",nulWidth);
  beye_context().write_profile_string(ini,"Beye","Browser","SubSubMode3",tmps);
}

extern const REGISTRY_DISASM Null_Disasm =
{
  DISASM_DATA,
  "~Data",
  { NULL, "Width ", NULL, NULL },
  { NULL, nulSelect_width, NULL, NULL },
  nulDisassembler,
  NULL,
  nulHelpAsm,
  nulMaxInsnLen,
  nulGetAsmColor,
  NULL,
  nulGetAsmColor,
  NULL,
  nulGetBitness,
  nulGetClone,
  nulInit,
  nulTerm,
  nulReadIni,
  nulWriteIni
};
} // namespace beye
