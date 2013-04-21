/**
 * @namespace   beye
 * @file        bin_util.h
 * @brief       This file contains prototypes of common functions of
 *              plugins\bin of BEYE project.
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
#ifndef __BIN_UTIL__H
#define __BIN_UTIL__H

#ifndef __BBIO_H
#include "libbeye/bbio.h"
#endif

#ifndef __BEYEUTIL__H
#include "beyeutil.h"
#endif

#if __BYTE_ORDER == __LITTLE_ENDIAN
#define  FMT_WORD(cval,is_big)\
 (uint16_t)((bool)is_big ? ByteSwapS(*(uint16_t *)(uint8_t *)cval) : *(uint16_t *)(uint8_t *)cval)
#define  FMT_DWORD(cval,is_big)\
 (uint32_t)((bool)is_big ? ByteSwapL(*(uint32_t *)(uint8_t *)cval) : *(uint32_t *)(uint8_t *)cval)
#define  FMT_QWORD(cval,is_big)\
 (uint64_t)((bool)is_big ? ByteSwapLL(*(uint64_t *)(uint8_t *)cval) : *(uint64_t *)(uint8_t *)cval)
#else
#define  FMT_WORD(cval,is_big)\
 (uint16_t)(!(bool)is_big ? ByteSwapS(*(uint16_t *)(uint8_t *)cval) : *(uint16_t *)(uint8_t *)cval)
#define  FMT_DWORD(cval,is_big)\
 (uint32_t)(!(bool)is_big ? ByteSwapL(*(uint32_t *)(uint8_t *)cval) : *(uint32_t *)(uint8_t *)cval)
#define  FMT_QWORD(cval,is_big)\
 (uint64_t)(!(bool)is_big ? ByteSwapLL(*(uint64_t *)(uint8_t *)cval) : *(uint64_t *)(uint8_t *)cval)
#endif

struct PubName
{
  __filesize_t pa;
  __filesize_t nameoff;
  __filesize_t addinfo;
  __filesize_t attr;
};

extern linearArray *PubNames;
extern unsigned fmtActiveState;

typedef void (__FASTCALL__ *ReadPubName)(BFile* b_cache,const struct PubName *it,
			    char *buff,unsigned cb_buff);
typedef void (__FASTCALL__ *ReadPubNameList)(BFile* fmt_chahe,void (__FASTCALL__ *mem_out)(const char *));

extern void  __FASTCALL__ fmtSetState(int state);
extern tCompare __FASTCALL__ fmtComparePubNames(const void __HUGE__ *v1,const void __HUGE__ *v2);
extern bool __FASTCALL__ fmtFindPubName(BFile* fmt_cache,char *buff,unsigned cb_buff,
					 __filesize_t pa,
					 ReadPubNameList fmtReadPubNameList,
					 ReadPubName fmtReadPubName);
extern __filesize_t __FASTCALL__ fmtGetPubSym(BFile* fmt_cache,char *str,unsigned cb_str,
				      unsigned *func_class,__filesize_t pa,
				      bool as_prev,
				      ReadPubNameList fmtReadPubNameList,
				      ReadPubName fmtReadPubName);

typedef unsigned      (__FASTCALL__ * GetNumItems)(BFile* handle);
typedef bool         (__FASTCALL__ * ReadItems)(BFile* handle,memArray * names,unsigned nnames);
typedef __filesize_t  (__FASTCALL__ * CalcEntry)(unsigned,int dispmsg);
extern  int           __FASTCALL__ fmtShowList( GetNumItems gni,ReadItems ri,const char * title,int flags,unsigned * ordinal);

/** Reads user defined name at given offset!
**/
extern bool __FASTCALL__ udnFindName(__filesize_t pa,char *buff, unsigned cb_buff);

/** Display select box to select user defined name and returns its offset.
**/
extern bool __FASTCALL__ udnSelectName(__filesize_t *off);

/** Shows menu with operations for user defined names!
**/
extern bool __FASTCALL__ udnUserNames( void );

extern void __FASTCALL__ udnInit( hIniProfile *ini );
extern void __FASTCALL__ udnTerm( hIniProfile *ini );

#endif
