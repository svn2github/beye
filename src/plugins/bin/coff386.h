/**
 * @namespace   beye_plugins_auto
 * @file        plugins/bin/coff386.h
 * @brief       This file contains coff-i386 file format definition.
 * @version     -
 * @remark      Copyright (C) 1995, 1996, 1997, 1998 Free Software Foundation,
 *              Inc. This file is part of the GNU C Library.
 *              The GNU C Library is free software; you can redistribute it and/or
 *              modify it under the terms of the GNU Library General Public License as
 *              published by the Free Software Foundation; either version 2 of the
 *              License, or (at your option) any later version.
 *              The GNU C Library is distributed in the hope that it will be useful,
 *              but WITHOUT ANY WARRANTY; without even the implied warranty of
 *              MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *              Library General Public License for more details.
 *              You should have received a copy of the GNU Library General Public
 *              License along with the GNU C Library; see the file COPYING.LIB.  If not,
 *              write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 *              Boston, MA 02111-1307, USA.
 * @note        Requires POSIX compatible development system
 *
 * @author      GNU FSF
 * @since       1995
**/
#ifndef __SYS_DEP_H
#include "_sys_dep.h"
#endif

#ifdef __HAVE_PRAGMA_PACK__
#pragma pack(1)
#endif

/********************** FILE HEADER **********************/

struct external_filehdr {
        uint8_t f_magic[2];       /**< magic number                 */
        uint8_t f_nscns[2];       /**< number of sections           */
        uint8_t f_timdat[4];      /**< time & date stamp            */
        uint8_t f_symptr[4];      /**< file pointer to symtab       */
        uint8_t f_nsyms[4];       /**< number of symtab entries     */
        uint8_t f_opthdr[2];      /**< sizeof(optional hdr)         */
        uint8_t f_flags[2];       /**< flags                        */
};

#define F_RELFLG        (0x0001) /**< relocation info stripped from file */
#define F_EXEC          (0x0002) /**< file is executable (no unresolved external references) */
#define F_LNNO          (0x0004) /**< line numbers stripped from file */
#define F_LSYMS         (0x0008) /**< local symbols stripped from file */
#define F_AR32WR        (0x0100) /**< file has byte ordering of an AR32WR machine (e.g. vax) */


#define I386MAGIC       0x14c
#define I386PTXMAGIC    0x154
#define I386AIXMAGIC    0x175

/** This is Lynx's all-platform magic number for executables. */

#define LYNXCOFFMAGIC   0415

#define I386BADMAG(x) (((x) != I386MAGIC) \
                       && (x) != I386AIXMAGIC \
                       && (x) != I386PTXMAGIC \
                       && (x) != LYNXCOFFMAGIC)

#define FILHDR  struct external_filehdr
#define FILHSZ  20


/********************** AOUT "OPTIONAL HEADER" **********************/


typedef struct
{
  uint8_t magic[2];               /**< type of file                         */
  uint8_t vstamp[2];              /**< version stamp                        */
  uint8_t tsize[4];               /**< text size in bytes, padded to FW bdry*/
  uint8_t dsize[4];               /**< initialized data "  "                */
  uint8_t bsize[4];               /**< uninitialized data "   "             */
  uint8_t entry[4];               /**< entry pt.                            */
  uint8_t text_start[4];          /**< base of text used for this file */
  uint8_t data_start[4];          /**< base of data used for this file */
}
AOUTHDR;

typedef struct gnu_aout {
	uint32_t info;
	uint32_t tsize;
	uint32_t dsize;
	uint32_t bsize;
	uint32_t symsize;
	uint32_t entry;
	uint32_t txrel;
	uint32_t dtrel;
	} GNU_AOUT;

#define AOUTSZ (sizeof(AOUTHDR))

#define OMAGIC          0404    /**< object files, eg as output */
#define ZMAGIC          0413    /**< demand load format, eg normal ld output */
#define STMAGIC         0401    /**< target shlib */
#define SHMAGIC         0443    /**< host   shlib */


/** define some NT default values */
/*  #define NT_IMAGE_BASE        0x400000 moved to internal.h */
#define NT_SECTION_ALIGNMENT 0x1000
#define NT_FILE_ALIGNMENT    0x200
#define NT_DEF_RESERVE       0x100000
#define NT_DEF_COMMIT        0x1000

/********************** SECTION HEADER **********************/


struct external_scnhdr {
        uint8_t   s_name[8];      /**< section name                 */
        uint8_t   s_paddr[4];     /**< physical address, aliased s_nlib */
        uint8_t   s_vaddr[4];     /**< virtual address              */
        uint8_t   s_size[4];      /**< section size                 */
        uint8_t   s_scnptr[4];    /**< file ptr to raw data for section */
        uint8_t   s_relptr[4];    /**< file ptr to relocation       */
        uint8_t   s_lnnoptr[4];   /**< file ptr to line numbers     */
        uint8_t   s_nreloc[2];    /**< number of relocation entries */
        uint8_t   s_nlnno[2];     /**< number of line number entries*/
        uint8_t   s_flags[4];     /**< flags                        */
};

#define STYP_TEXT      0x0020
#define STYP_DATA      0x0040
#define STYP_BSS       0x0080

#define SCNHDR  struct external_scnhdr
#define SCNHSZ  40

/**
 * names of "special" sections
 */
#define _TEXT   ".text"
#define _DATA   ".data"
#define _BSS    ".bss"
#define _COMMENT ".comment"
#define _LIB ".lib"

/********************** LINE NUMBERS **********************/

/** 1 line number entry for every "breakpointable" source line in a section.
 * Line numbers are grouped on a per function basis; first entry in a function
 * grouping will have l_lnno = 0 and in place of physical address will be the
 * symbol table index of the function name.
 */
struct external_lineno {
        union {
                uint8_t l_symndx[4];      /**< function name symbol index, iff l_lnno == 0*/
                uint8_t l_paddr[4];       /**< (physical) address of line number    */
        } l_addr;
        uint8_t l_lnno[2];        /**< line number          */
};


#define LINENO  struct external_lineno
#define LINESZ  6


/********************** SYMBOLS **********************/

#define E_SYMNMLEN      8       /**< # uint8_tacters in a symbol name       */
#define E_FILNMLEN      14      /**< # uint8_tacters in a file name         */
#define E_DIMNUM        4       /**< # array dimensions in auxiliary entry */

struct external_syment
{
  union {
    uint8_t e_name[E_SYMNMLEN];
    struct {
      uint8_t e_zeroes[4];
      uint8_t e_offset[4];
    } e;
  } e;
  uint8_t e_value[4];
  uint8_t e_scnum[2];
  uint8_t e_type[2];
  uint8_t e_sclass[1];
  uint8_t e_numaux[1];
};

#define __N_BTMASK        (0xf)
#define __N_TMASK         (0x30)
#define __N_BTSHFT        (4)
#define __N_TSHIFT        (2)

union external_auxent {
        struct {
                uint8_t x_tagndx[4];      /**< str, un, or enum tag indx */
                union {
                        struct {
                            uint8_t  x_lnno[2]; /**< declaration line number */
                            uint8_t  x_size[2]; /**< str/union/array size */
                        } x_lnsz;
                        uint8_t x_fsize[4];       /**< size of function */
                } x_misc;
                union {
                        struct {                /**< if ISFCN, tag, or .bb */
                            uint8_t x_lnnoptr[4]; /**< ptr to fcn line # */
                            uint8_t x_endndx[4];  /**< entry ndx past block end */
                        } x_fcn;
                        struct {                /**< if ISARY, up to 4 dimen. */
                            uint8_t x_dimen[E_DIMNUM][2];
                        } x_ary;
                } x_fcnary;
                uint8_t x_tvndx[2];               /**< tv index */
        } x_sym;

        union {
                uint8_t x_fname[E_FILNMLEN];
                struct {
                        uint8_t x_zeroes[4];
                        uint8_t x_offset[4];
                } x_n;
        } x_file;

        struct {
                uint8_t x_scnlen[4];      /**< section length */
                uint8_t x_nreloc[2];      /**< # relocation entries */
                uint8_t x_nlinno[2];      /**< # line numbers */
                uint8_t x_checksum[4];    /**< section COMDAT checksum */
                uint8_t x_associated[2];  /**< COMDAT associated section index */
                uint8_t x_comdat[1];      /**< COMDAT selection number */
        } x_scn;

        struct {
                uint8_t x_tvfill[4];      /**< tv fill value */
                uint8_t x_tvlen[2];       /**< length of .tv */
                uint8_t x_tvran[2][2];    /**< tv range */
        } x_tv;         /**< info about .tv section (in auxent of symbol .tv)) */


};

#define SYMENT  struct external_syment
#define SYMESZ  18
#define AUXENT  union external_auxent
#define AUXESZ  18


#       define _ETEXT   "etext"


/********************** RELOCATION DIRECTIVES **********************/

#define _ETEXT	"etext"


/** Relocatable symbols have number of the section in which they are defined,
   or one of the following: */

#define N_UNDEF	((int16_t)0)	/**< undefined symbol */
#define N_ABS	((int16_t)-1)	/**< value of symbol is absolute */
#define N_DEBUG	((int16_t)-2)	/**< debugging symbol -- value is meaningless */
#define N_TV	((int16_t)-3)	/**< indicates symbol needs preload transfer vector */
#define P_TV	((int16_t)-4)	/**< indicates symbol needs postload transfer vector*/

/**
 * Type of a symbol, in low N bits of the word
 */
#define T_NULL		0
#define T_VOID		1	/**< function argument (only used by compiler) */
#define T_CHAR		2	/**< character		*/
#define T_SHORT		3	/**< short integer	*/
#define T_INT		4	/**< integer		*/
#define T_LONG		5	/**< long integer		*/
#define T_FLOAT		6	/**< floating point	*/
#define T_DOUBLE	7	/**< double word		*/
#define T_STRUCT	8	/**< structure 		*/
#define T_UNION		9	/**< union 		*/
#define T_ENUM		10	/**< enumeration 		*/
#define T_MOE		11	/**< member of enumeration*/
#define T_UCHAR		12	/**< unsigned character	*/
#define T_USHORT	13	/**< unsigned short	*/
#define T_UINT		14	/**< unsigned integer	*/
#define T_ULONG		15	/**< uint32_t	*/
#define T_LNGDBL	16	/**< long double		*/

/**
 * derived types, in n_type
*/
#define DT_NON		(0)	/**< no derived type */
#define DT_PTR		(1)	/**< pointer */
#define DT_FCN		(2)	/**< function */
#define DT_ARY		(3)	/**< array */

#define BTYPE(x)	((x) & N_BTMASK)
#define ISPTR(x)	(((x) & N_TMASK) == (DT_PTR << N_BTSHFT))
#define ISFCN(x)	(((x) & N_TMASK) == (DT_FCN << N_BTSHFT))
#define ISARY(x)	(((x) & N_TMASK) == (DT_ARY << N_BTSHFT))
#define ISTAG(x)	((x)==C_STRTAG||(x)==C_UNTAG||(x)==C_ENTAG)
#define DECREF(x) ((((x)>>N_TSHIFT)&~N_BTMASK)|((x)&N_BTMASK))

/********************** STORAGE CLASSES **********************/

/** This used to be defined as -1, but now n_sclass is unsigned.  */
#define C_EFCN		0xff	/**< physical end of function	*/
#define C_NULL		0
#define C_AUTO		1	/**< automatic variable		*/
#define C_EXT		2	/**< external symbol		*/
#define C_STAT		3	/**< static			*/
#define C_REG		4	/**< register variable		*/
#define C_EXTDEF	5	/**< external definition	*/
#define C_LABEL		6	/**< label			*/
#define C_ULABEL	7	/**< undefined label		*/
#define C_MOS		8	/**< member of structure	*/
#define C_ARG		9	/**< function argument		*/
#define C_STRTAG	10	/**< structure tag		*/
#define C_MOU		11	/**< member of union		*/
#define C_UNTAG		12	/**< union tag			*/
#define C_TPDEF		13	/**< type definition		*/
#define C_USTATIC	14	/**< undefined static		*/
#define C_ENTAG		15	/**< enumeration tag		*/
#define C_MOE		16	/**< member of enumeration	*/
#define C_REGPARM	17	/**< register parameter		*/
#define C_FIELD		18	/**< bit field			*/
#define C_AUTOARG	19	/**< auto argument		*/
#define C_LASTENT	20	/**< dummy entry (end of block)	*/
#define C_BLOCK		100	/**< ".bb" or ".eb"		*/
#define C_FCN		101	/**< ".bf" or ".ef"		*/
#define C_EOS		102	/**< end of structure		*/
#define C_FILE		103	/**< file name			*/
#define C_LINE		104	/**< line # reformatted as symbol table entry */
#define C_ALIAS	 	105	/**< duplicate tag		*/
#define C_HIDDEN	106	/**< ext symbol in dmert public lib */

/********************** RELOCATION DIRECTIVES **********************/



struct external_reloc {
  uint32_t r_vaddr;
  uint32_t r_symndx;
  uint16_t r_type;
};


#define RELOC struct external_reloc
#define RELSZ sizeof(RELOC)

#define RELOC_REL32	20	/**< 32-bit PC-relative address */
#define RELOC_ADDR32	6	/**< 32-bit absolute address */

#define DEFAULT_DATA_SECTION_ALIGNMENT 4
#define DEFAULT_BSS_SECTION_ALIGNMENT 4
#define DEFAULT_TEXT_SECTION_ALIGNMENT 4
/** For new sections we havn't heard of before */
#define DEFAULT_SECTION_ALIGNMENT 4

#ifdef __HAVE_PRAGMA_PACK__
#pragma pack()
#endif

#define RELOC struct external_reloc
