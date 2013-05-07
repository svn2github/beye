#include "config.h"
#include "libbeye/libbeye.h"
using namespace beye;
/**
 * @namespace   beye_plugins_auto
 * @file        plugins/bin/realmdeia.c
 * @brief       This file contains implementation of decoder for jpeg
 *              file format.
 * @version     -
 * @remark      this source file is part of rmary vIEW project (BEYE).
 *              The rmary vIEW (BEYE) is copyright (C) 1995 Nickols_K.
 *              All rights reserved. This software is redistributable under the
 *              licence given in the file "Licence.en" ("Licence.ru" in russian
 *              translation) distributed in the BEYE archive.
 * @note        Requires POSIX compatible development system
 *
 * @author      Nickols_K
 * @since       1995
 * @note        Development, fixes and improvements
**/
#include <stddef.h>

#include "bconsole.h"
#include "beyehelp.h"
#include "colorset.h"
#include "beyeutil.h"
#include "reg_form.h"
#include "bmfile.h"
#include "libbeye/kbd_code.h"
#include "plugins/disasm.h"
#include "plugins/bin/mmio.h"

namespace beye {
#define MKTAG(a, b, c, d) (a | (b << 8) | (c << 16) | (d << 24))

static bool  __FASTCALL__ rm_check_fmt()
{
    if(bmReadDWordEx(0,BFile::Seek_Set)==MKTAG('.', 'R', 'M', 'F')) return true;
    return false;
}

static __filesize_t __FASTCALL__ Show_RM_Header()
{
    ErrMessageBox("Not implemented yet!","RM format");
    return BMGetCurrFilePos();
}

static void __FASTCALL__ rm_init_fmt(CodeGuider& code_guider) { UNUSED(code_guider); }
static void __FASTCALL__ rm_destroy_fmt() {}
static int  __FASTCALL__ rm_platform() { return DISASM_DEFAULT; }

extern const REGISTRY_BIN rmTable =
{
  "Real Media file format",
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
  { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL },
  rm_check_fmt,
  rm_init_fmt,
  rm_destroy_fmt,
  Show_RM_Header,
  NULL,
  rm_platform,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL
};
} // namespace beye
