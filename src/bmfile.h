/**
 * @namespace   beye
 * @file        bmfile.h
 * @brief       This file contains prototypes of Buffering streams Manager.
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
#ifndef __BMFILE_INC
#define __BMFILE_INC

#ifndef __BBIO_H
#include "libbeye/bbio.h"
#endif

#define HA_LEN ((BMFileFlags&BMFF_USE64)?18:10)

#define BMFF_NONE	0x00000000
#define BMFF_USE64	0x00000001
extern unsigned BMFileFlags;

#if __WORDSIZE == 16
#define BBIO_CACHE_SIZE        0x4000  /* 16k */
#define BBIO_SMALL_CACHE_SIZE  0x1000  /* 4k */
#else
#define BBIO_CACHE_SIZE        0xFFFF  /* 64k */
#define BBIO_SMALL_CACHE_SIZE  0x4000  /* 16k */
#endif

BFile*        __FASTCALL__ beyeOpenRO(const char *fname,unsigned cache_size);
BFile*        __FASTCALL__ beyeOpenRW(const char *fname,unsigned cache_size);

#define BM_SEEK_SET BIO_SEEK_SET
#define BM_SEEK_CUR BIO_SEEK_CUR
#define BM_SEEK_END BIO_SEEK_END
extern BFile* bm_file_handle,*sc_bm_file_handle;

int            __FASTCALL__ BMOpen(const char * fname);
void           __FASTCALL__ BMClose( void );
static inline bhandle_t		__FASTCALL__ BMHandle( void ) { return bm_file_handle->handle(); }
static inline BFile*		__FASTCALL__ BMbioHandle( void ) { return bm_file_handle; }
static inline const char *		__FASTCALL__ BMName( void ) { return bm_file_handle->filename(); }
static inline __filesize_t	__FASTCALL__ BMGetCurrFilePos( void ) { return bm_file_handle->tell(); }
static inline __filesize_t	__FASTCALL__ BMGetFLength( void ) { return bm_file_handle->flength(); }
static inline bool	__FASTCALL__ BMEOF( void ) { return bm_file_handle->eof(); }
static inline void	__FASTCALL__ BMSeek(__fileoff_t pos,int RELATION) { bm_file_handle->seek(pos,RELATION); }
static inline void	__FASTCALL__ BMReRead( void )  { bm_file_handle->reread(); }
static inline uint8_t	__FASTCALL__ BMReadByte( void ) { return bm_file_handle->read_byte(); }
static inline uint16_t	__FASTCALL__ BMReadWord( void ) { return bm_file_handle->read_word(); }
static inline uint32_t	__FASTCALL__ BMReadDWord( void ) { return bm_file_handle->read_dword(); }
static inline uint64_t	__FASTCALL__ BMReadQWord( void ) { return bm_file_handle->read_qword(); }
static inline bool	__FASTCALL__ BMReadBuffer(any_t* buffer,unsigned len) { return bm_file_handle->read_buffer(buffer,len); }
uint8_t        __FASTCALL__ BMReadByteEx(__fileoff_t pos,int RELATION);
uint16_t       __FASTCALL__ BMReadWordEx(__fileoff_t pos,int RELATION);
uint32_t       __FASTCALL__ BMReadDWordEx(__fileoff_t pos,int RELATION);
uint64_t       __FASTCALL__ BMReadQWordEx(__fileoff_t pos,int RELATION);
bool         __FASTCALL__ BMReadBufferEx(any_t* buffer,unsigned len,__fileoff_t pos,int RELATION);
static inline bool	__FASTCALL__ BMWriteByte(uint8_t byte) { return bm_file_handle->write_byte(byte); }
static inline bool	__FASTCALL__ BMWriteWord(uint16_t word) { return bm_file_handle->write_word(word); }
static inline bool	__FASTCALL__ BMWriteDWord(uint32_t dword) { return bm_file_handle->write_dword(dword); }
static inline bool	__FASTCALL__ BMWriteQWord(uint64_t qword) { return bm_file_handle->write_qword(qword); }
bool		__FASTCALL__ BMWriteBuff(any_t* buff,unsigned len);
bool		__FASTCALL__ BMWriteByteEx(__fileoff_t pos,int RELATION,uint8_t byte);
bool		__FASTCALL__ BMWriteWordEx(__fileoff_t pos,int RELATION,uint16_t word);
bool		__FASTCALL__ BMWriteDWordEx(__fileoff_t pos,int RELATION,uint32_t dword);
bool		__FASTCALL__ BMWriteBuffEx(__fileoff_t pos,int RELATION,any_t* buff,unsigned len);

/** Below analogs with using small cache size */

static inline bool	__FASTCALL__ bmEOF( void ) { return sc_bm_file_handle->eof(); }
static inline void	__FASTCALL__ bmSeek(__fileoff_t pos,int RELATION) { sc_bm_file_handle->seek(pos,RELATION); }
static inline void	__FASTCALL__ bmReRead( void ) { sc_bm_file_handle->reread(); }
static inline uint8_t	__FASTCALL__ bmReadByte( void ) { return sc_bm_file_handle->read_byte(); }
static inline uint16_t	__FASTCALL__ bmReadWord( void ) { return sc_bm_file_handle->read_word(); }
static inline uint32_t	__FASTCALL__ bmReadDWord( void ) { return sc_bm_file_handle->read_dword(); }
static inline uint64_t	__FASTCALL__ bmReadQWord( void ) { return sc_bm_file_handle->read_qword(); }
static inline bool	__FASTCALL__ bmReadBuffer(any_t* buffer,unsigned len) { return sc_bm_file_handle->read_buffer(buffer,len); }
uint8_t        __FASTCALL__ bmReadByteEx(__fileoff_t pos,int RELATION);
uint16_t       __FASTCALL__ bmReadWordEx(__fileoff_t pos,int RELATION);
uint32_t       __FASTCALL__ bmReadDWordEx(__fileoff_t pos,int RELATION);
uint64_t       __FASTCALL__ bmReadQWordEx(__fileoff_t pos,int RELATION);
bool         __FASTCALL__ bmReadBufferEx(any_t* buffer,unsigned len,__fileoff_t pos,int RELATION);
static inline bhandle_t	__FASTCALL__ bmHandle( void ) { return sc_bm_file_handle->handle(); }
static inline BFile*	__FASTCALL__ bmbioHandle( void ) { return sc_bm_file_handle; }
static inline const char *	__FASTCALL__ bmName( void ) { return sc_bm_file_handle->filename(); }

static inline __filesize_t	__FASTCALL__ bmGetCurrFilePos( void ) { return sc_bm_file_handle->tell(); }
static inline __filesize_t	__FASTCALL__ bmGetFLength( void ) {  return sc_bm_file_handle->flength(); }

static inline bool	__FASTCALL__ bmWriteByte(uint8_t byte) { return sc_bm_file_handle->write_byte(byte); }
static inline bool	__FASTCALL__ bmWriteWord(uint16_t word) { return sc_bm_file_handle->write_word(word); }
static inline bool	__FASTCALL__ bmWriteDWord(uint32_t dword) { return sc_bm_file_handle->write_dword(dword); }
static inline bool	__FASTCALL__ bmWriteQWord(uint64_t qword) { return sc_bm_file_handle->write_qword(qword); }
bool          __FASTCALL__ bmWriteBuff(any_t* buff,unsigned len);
bool          __FASTCALL__ bmWriteByteEx(__fileoff_t pos,int RELATION,uint8_t byte);
bool          __FASTCALL__ bmWriteWordEx(__fileoff_t pos,int RELATION,uint16_t word);
bool          __FASTCALL__ bmWriteDWordEx(__fileoff_t pos,int RELATION,uint32_t dword);
bool          __FASTCALL__ bmWriteQWordEx(__fileoff_t pos,int RELATION,uint64_t dword);
bool          __FASTCALL__ bmWriteBuffEx(__fileoff_t pos,int RELATION,any_t* buff,unsigned len);

#endif
