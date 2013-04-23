#include "config.h"
#include "libbeye/libbeye.h"
using namespace beye;
/**
 * @namespace   beye
 * @file        addendum.c
 * @brief       This file contains interface to Addons functions.
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
**/
#include <stddef.h>
#include "bconsole.h"
#include "beyeutil.h"
#include "reg_form.h"

extern REGISTRY_TOOL DigitalConvertor;
extern REGISTRY_TOOL Calculator;

namespace beye {
static REGISTRY_TOOL *toolTable[] =
{
  &DigitalConvertor,
  &Calculator
};

static size_t defToolSel = 0;

void SelectTool( void )
{
  const char *toolName[sizeof(toolTable)/sizeof(REGISTRY_TOOL *)];
  size_t i,nTools;
  int retval;

  nTools = sizeof(toolTable)/sizeof(REGISTRY_TOOL *);
  for(i = 0;i < nTools;i++) toolName[i] = toolTable[i]->name;
  retval = SelBoxA(const_cast<char**>(toolName),nTools," Select tool: ",defToolSel);
  if(retval != -1)
  {
    toolTable[retval]->tool();
    defToolSel = retval;
  }
}

void init_addons( void )
{
  size_t i;
  for(i = 0;i < sizeof(toolTable)/sizeof(REGISTRY_TOOL *);i++)
  {
    if(toolTable[i]->read_ini) toolTable[i]->read_ini();
  }
}

void term_addons( void )
{
  size_t i;
  for(i = 0;i < sizeof(toolTable)/sizeof(REGISTRY_TOOL *);i++)
  {
    if(toolTable[i]->save_ini) toolTable[i]->save_ini();
  }
}
} // namespace beye

