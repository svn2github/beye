/**
 * @namespace   beye_plugins_auto
 * @file        plugins/bin/rdoff2.h
 * @brief       This file contains RDOFF v2 file definitions.
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
#ifndef __RDOFF_INC
#define __RDOFF_INC

#ifndef __SYS_DEP_H
#include "_sys_dep.h"
#endif

#ifdef __HAVE_PRAGMA_PACK__
#pragma pack(1)
#endif

/** RDOFF v2 fixed header */
typedef struct tag_rdoff2_Header
{
  uint8_t  id[6]; /**< RDOFF2 or RDOFF\02 */
  uint32_t image_len;
  uint32_t header_len;
}rdoff2_Header;


#ifdef __HAVE_PRAGMA_PACK__
#pragma pack()
#endif

#endif
