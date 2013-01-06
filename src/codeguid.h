/**
 * @namespace   beye
 * @file        codeguid.h
 * @brief       This file contains prototypes code navigator.
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
#ifndef __CODEGUID__H
#define __CODEGUID__H

#ifndef __SYS_DEP_H
#include "_sys_dep.h"
#endif

extern char codeguid_image[];

extern void              __FASTCALL__ GidResetGoAddress( int keycode );
extern void              __FASTCALL__ GidAddGoAddress(char *str,__filesize_t addr);
extern void              __FASTCALL__ GidAddBackAddress( void );
extern __filesize_t      __FASTCALL__ GidGetGoAddress(unsigned keycode);
extern char *            __FASTCALL__ GidEncodeAddress(__filesize_t cfpos,bool aresolv);

extern bool             __FASTCALL__ initCodeGuider( void );
extern void              __FASTCALL__ termCodeGuider( void );

#endif
