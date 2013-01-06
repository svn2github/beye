/**
 * @namespace   beye
 * @file        bmfile.c
 * @brief       This file has developed as Buffered stream Manager and presents
 *              front end interface to bbio library.
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
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "bmfile.h"
#include "bconsole.h"
#include "tstrings.h"
#include "libbeye/bbio.h"

unsigned BMFileFlags=0;
extern bool fioUseMMF;
BGLOBAL bm_file_handle = NULL,sc_bm_file_handle = NULL;

BGLOBAL __FASTCALL__ beyeOpenRO(char *fname,unsigned cache_size)
{
  BGLOBAL fret;
  fret = bioOpen(fname,FO_READONLY | SO_DENYNONE,cache_size,fioUseMMF ? BIO_OPT_USEMMF : BIO_OPT_DB);
  if(fret == &bNull)
    fret = bioOpen(fname,FO_READONLY | SO_COMPAT,cache_size,fioUseMMF ? BIO_OPT_USEMMF : BIO_OPT_DB);
  return fret;
}

BGLOBAL __FASTCALL__ beyeOpenRW(char *fname,unsigned cache_size)
{
  BGLOBAL fret;
  fret = bioOpen(fname,FO_READWRITE | SO_DENYNONE,cache_size,fioUseMMF ? BIO_OPT_USEMMF : BIO_OPT_DB);
  if(fret == &bNull)
    fret = bioOpen(fname,FO_READWRITE | SO_COMPAT,cache_size,fioUseMMF ? BIO_OPT_USEMMF : BIO_OPT_DB);
  return fret;
}

int __FASTCALL__ BMOpen(char * fname)
{
  bm_file_handle = beyeOpenRO(fname,BBIO_CACHE_SIZE);
  if(bm_file_handle == &bNull)
  {
    errnoMessageBox(OPEN_FAIL,NULL,errno);
    return -1;
  }
  sc_bm_file_handle = bioDupEx(bm_file_handle,BBIO_SMALL_CACHE_SIZE);
  if(sc_bm_file_handle == &bNull)
  {
    errnoMessageBox(DUP_FAIL,NULL,errno);
    return -1;
  }
  bioSetOptimization(bm_file_handle,BIO_OPT_RANDOM);
  bioSetOptimization(sc_bm_file_handle,BIO_OPT_RANDOM);
  if(BMGetFLength() > ULONG_MAX) BMFileFlags |= BMFF_USE64;
  return 0;
}

void __FASTCALL__ BMClose( void )
{
  if(bm_file_handle != &bNull) bioClose(bm_file_handle);
  bm_file_handle = &bNull;
  if(sc_bm_file_handle != &bNull) bioClose(sc_bm_file_handle);
  sc_bm_file_handle = &bNull;
}

uint8_t  __FASTCALL__ BMReadByteEx(__fileoff_t pos,int RELATION)
{
 bioSeek(bm_file_handle,pos,RELATION);
 return bioReadByte(bm_file_handle);
}

uint16_t __FASTCALL__ BMReadWordEx(__fileoff_t pos,int RELATION)
{
 bioSeek(bm_file_handle,pos,RELATION);
 return bioReadWord(bm_file_handle);
}

uint32_t  __FASTCALL__ BMReadDWordEx(__fileoff_t pos,int RELATION)
{
 bioSeek(bm_file_handle,pos,RELATION);
 return bioReadDWord(bm_file_handle);
}

uint64_t  __FASTCALL__ BMReadQWordEx(__fileoff_t pos,int RELATION)
{
 bioSeek(bm_file_handle,pos,RELATION);
 return bioReadQWord(bm_file_handle);
}

bool __FASTCALL__ BMReadBufferEx(any_t* buffer,unsigned len,__fileoff_t pos,int RELATION)
{
 bioSeek(bm_file_handle,pos,RELATION);
 return bioReadBuffer(bm_file_handle,buffer,len);
}

bool __FASTCALL__ BMWriteBuff(any_t* buff,unsigned len)
{
  bool ret;
  ret = bioWriteBuffer(bm_file_handle,buff,len);
  bioFlush(bm_file_handle);
  return ret;
}

bool __FASTCALL__ BMWriteByteEx(__fileoff_t pos,int RELATION,uint8_t byte)
{
  bioSeek(bm_file_handle,pos,RELATION);
  return bioWriteByte(bm_file_handle,byte);
}

bool __FASTCALL__ BMWriteWordEx(__fileoff_t pos,int RELATION,uint16_t word)
{
  bioSeek(bm_file_handle,pos,RELATION);
  return bioWriteWord(bm_file_handle,word);
}

bool __FASTCALL__ BMWriteDWordEx(__fileoff_t pos,int RELATION,uint32_t dword)
{
  bioSeek(bm_file_handle,pos,RELATION);
  return bioWriteDWord(bm_file_handle,dword);
}

bool __FASTCALL__ BMWriteQWordEx(__fileoff_t pos,int RELATION,uint64_t dword)
{
  bioSeek(bm_file_handle,pos,RELATION);
  return bioWriteQWord(bm_file_handle,dword);
}

bool  __FASTCALL__ BMWriteBuffEx(__fileoff_t pos,int RELATION,any_t* buff,unsigned len)
{
  bioSeek(bm_file_handle,pos,RELATION);
  return bioWriteBuffer(bm_file_handle,buff,len);
}

uint8_t __FASTCALL__ bmReadByteEx(__fileoff_t pos,int RELATION)
{
 bioSeek(sc_bm_file_handle,pos,RELATION);
 return bioReadByte(sc_bm_file_handle);
}

uint16_t __FASTCALL__ bmReadWordEx(__fileoff_t pos,int RELATION)
{
 bioSeek(sc_bm_file_handle,pos,RELATION);
 return bioReadWord(sc_bm_file_handle);
}

uint32_t __FASTCALL__ bmReadDWordEx(__fileoff_t pos,int RELATION)
{
 bioSeek(sc_bm_file_handle,pos,RELATION);
 return bioReadDWord(sc_bm_file_handle);
}

uint64_t __FASTCALL__ bmReadQWordEx(__fileoff_t pos,int RELATION)
{
 bioSeek(sc_bm_file_handle,pos,RELATION);
 return bioReadQWord(sc_bm_file_handle);
}

bool __FASTCALL__ bmReadBufferEx(any_t* buffer,unsigned len,__fileoff_t pos,int RELATION)
{
 bioSeek(sc_bm_file_handle,pos,RELATION);
 return bioReadBuffer(sc_bm_file_handle,buffer,len);
}

bool __FASTCALL__ bmWriteBuff(any_t* buff,unsigned len)
{
  bool ret;
  ret = bioWriteBuffer(sc_bm_file_handle,buff,len);
  bioFlush(sc_bm_file_handle);
  return ret;
}

bool __FASTCALL__ bmWriteByteEx(__fileoff_t pos,int RELATION,uint8_t byte)
{
  bioSeek(sc_bm_file_handle,pos,RELATION);
  return bioWriteByte(sc_bm_file_handle,byte);
}

bool __FASTCALL__ bmWriteWordEx(__fileoff_t pos,int RELATION,uint16_t word)
{
  bioSeek(sc_bm_file_handle,pos,RELATION);
  return bioWriteWord(sc_bm_file_handle,word);
}

bool __FASTCALL__ bmWriteDWordEx(__fileoff_t pos,int RELATION,uint32_t dword)
{
  bioSeek(sc_bm_file_handle,pos,RELATION);
  return bioWriteDWord(sc_bm_file_handle,dword);
}

bool __FASTCALL__ bmWriteQWordEx(__fileoff_t pos,int RELATION,uint64_t dword)
{
  bioSeek(sc_bm_file_handle,pos,RELATION);
  return bioWriteQWord(sc_bm_file_handle,dword);
}

bool  __FASTCALL__ bmWriteBuffEx(__fileoff_t pos,int RELATION,any_t* buff,unsigned len)
{
  bioSeek(sc_bm_file_handle,pos,RELATION);
  return bioWriteBuffer(sc_bm_file_handle,buff,len);
}
