#include "config.h"
#include "libbeye/libbeye.h"
using namespace beye;
/**
 * @namespace   beye_plugins_auto
 * @file        plugins/bin/lmf.c
 * @brief       This file contains implementation of lmf (QNX4 executable file)
 *              file format decoder.
 * @version     -
 * @remark      this source file is part of Binary EYE project (BEYE).
 *              The Binary EYE (BEYE) is copyright (C) 1995 Nickols_K.
 *              All rights reserved. This software is redistributable under the
 *              licence given in the file "Licence.en" ("Licence.ru" in russian
 *              translation) distributed in the BEYE archive.
 * @note        Requires POSIX compatible development system
 *
 * @author      Andrew Golovnia
 * @since       2001
 * @note        Development, fixes and improvements
 * @todo        wc 10.6 debug information support!!! (see lmf.tgz)
**/
#include <algorithm>

#include <stddef.h>
#include <string.h>
#include <stdio.h>

#include "reg_form.h"
#include "bin_util.h"
#include "bmfile.h"
#include "beyehelp.h"
#include "beyeutil.h"
#include "bconsole.h"
#include "plugins/disasm.h"
#include "plugins/bin/lmf.h"
#include "libbeye/libbeye.h"
#include "libbeye/kbd_code.h"

namespace beye {
enum {
    MAXREC		=200,
    MINREC		=20,
    MAXSEG		=50,
    MAXSEGFRAMES	=50
};
typedef struct		/* LMF file frame */
{
	lmf_header header;		/* Header of frame */
	lmf_data data;			/* Data info */
	lmf_resource res;		/* Resource info */
	uint32_t file_pos;		/* Frame header file position */
} lmf_headers_list;

typedef struct		/* Extra definition */
{
	lmf_definition def;		/* Standard definition */
	uint32_t seg[MAXSEG];	/* Segments lengthes list */
} lmf_xdef;

static lmf_headers_list *hl;
static lmf_xdef xdef;
static int xdef_len=0;
static unsigned seg_num=0;
static uint32_t reccnt;
static uint32_t recmax;
static uint32_t reclast;
static uint32_t segbase[MAXSEG];

const char *lmftypes[]={
	"definition",
	"comment",
	"text",
	"fixup seg",
	"fixup x87",
	"eof",
	"resource",
	"end data",
	"fixup linear",
	"ph resource",
	"unknown"};

static void __FASTCALL__ failed_lmf(void)
{
	/* lmf corruption message */
}

#define DEF xdef.def
#define DEFSIZE sizeof(lmf_definition)
#define DATSIZE sizeof(lmf_data)
#define HDRSIZE sizeof(lmf_header)

static bool __FASTCALL__ lmf_check_fmt(void)
{
	int32_t i,j,p=0;
/*	lmf_data d;*/
	lmf_header h;
	if(!bmReadBufferEx(&h,sizeof h,0,BM_SEEK_SET)) return false;
	/* Test a first heder */
	if(h.rec_type!=_LMF_DEFINITION_REC||h.zero1!=0||/*h.spare!=0||*/
		h.data_nbytes<DEFSIZE+2*sizeof(long)||
		(h.data_nbytes-DEFSIZE)%4!=0) return false;
	i=j=(h.data_nbytes-DEFSIZE)/4;
	xdef_len=h.data_nbytes;
	if(!bmReadBufferEx(&xdef,std::min(sizeof(lmf_xdef),size_t(h.data_nbytes)),6,BM_SEEK_SET)) return false;
	/* Test a definition record */
	if(DEF.version_no!=400||DEF.code_index>i||DEF.stack_index>i||
		DEF.heap_index>i||DEF.argv_index>i||DEF.zero2!=0)
		return false;
	if(DEF.cpu%100!=86||(DEF.fpu!=0&&DEF.fpu%100!=87))
		return false;
	if(DEF.cflags&_PCF_FLAT&&DEF.flat_offset==0) return false;
	if(DEF.stack_nbytes==0) return false;
	for(i=0;i<4;i++)
		if(DEF.zero1[i]!=0) return false;
	while(1)
	{
		/* Test other headers */
		p+=HDRSIZE+h.data_nbytes;
		if(!bmReadBufferEx(&h,sizeof h,p,BM_SEEK_SET)) return false;
		if(h.rec_type==_LMF_DEFINITION_REC||h.data_nbytes==0||
			h.zero1!=0/*||h.spare!=0*/) return false;
		if(h.rec_type==_LMF_EOF_REC) break;
	}
	return true;
}

#define failed_lmf {failed_lmf();return;}

static void __FASTCALL__ lmf_init_fmt(CodeGuider& code_guider)
{
    UNUSED(code_guider);
	uint32_t i,l;
	int32_t pos=0;
	hl=new lmf_headers_list[MINREC];
	if(hl==NULL) return;
	recmax=MINREC;
	reccnt=0;
	seg_num=(xdef_len-DEFSIZE)/4;
	for(i=0;i<seg_num;i++) segbase[i]=0;
	for(i=0;;i++)
	{
		if((unsigned)i==recmax)
		{
			hl=(lmf_headers_list*)mp_realloc(hl,(recmax+=MINREC)*sizeof(lmf_headers_list));
			if(hl==NULL) return;
		}
		if(!bmReadBufferEx(&hl[i].header,HDRSIZE,pos,BM_SEEK_SET))
			failed_lmf;
		hl[i].file_pos=pos;
		switch(hl[i].header.rec_type)
		{
		case _LMF_DATA_REC:
		case _LMF_FIXUP_SEG_REC:
		case _LMF_FIXUP_LINEAR_REC:
			if(!bmReadBufferEx(&hl[i].data,DATSIZE,pos+HDRSIZE,BM_SEEK_SET))
				failed_lmf;
			l=hl[i].data.index;
			if(l>=seg_num)
				failed_lmf;
			break;
		case _LMF_RESOURCE_REC:
			if(!bmReadBufferEx(&hl[i].res,sizeof(lmf_resource),pos+HDRSIZE,BM_SEEK_SET))
				failed_lmf;
			break;
		case _LMF_EOF_REC:
			reclast=i;
			goto outloop;
		case _LMF_DEFINITION_REC:
		case _LMF_COMMENT_REC:
		case _LMF_FIXUP_80X87_REC:
		case _LMF_ENDDATA_REC:	/* todo: decode license information (name) */
		case _LMF_PHRESOURCE:
			break;				/* Ignore this records */
		default:
			failed_lmf;
		}
		pos+=hl[i].header.data_nbytes+HDRSIZE;
	}
outloop:
	if(DEF.cflags&_PCF_FLAT)
	{
		segbase[0]=DEF.stack_nbytes;
		for(i=1;i<=seg_num;i++)
			segbase[i]=(segbase[i-1]+
				((xdef.seg[i-1]&0x0fffffff)+4095))&0xfffff000;
	}

	return;
}

#undef failed_lmf

static void __FASTCALL__ lmf_destroy_fmt(void)
{
	delete hl;
}

static int __FASTCALL__ lmf_platform(void)
{
	return DISASM_CPU_IX86;
}

static int __FASTCALL__ lmf_bitness(__filesize_t pa)
{
	UNUSED(pa);
	if(DEF.cflags&_PCF_32BIT) return DAB_USE32;
	else return DAB_USE16;
}

static bool __FASTCALL__ lmf_AddressResolv(char *addr,__filesize_t cfpos)
{
	unsigned i;
 /* Since this function is used in references resolving of disassembler
    it must be seriously optimized for speed. */
	for(i=0;i<=reclast;i++)
	{
		if(hl[i].file_pos<=cfpos&&
			cfpos<hl[i].file_pos+hl[i].header.data_nbytes+HDRSIZE)
		{
			if(cfpos<hl[i].file_pos+HDRSIZE)
				sprintf(addr,"H%s:%s",Get2Digit(i),Get4Digit(cfpos-hl[i].file_pos));
			else
				switch(hl[i].header.rec_type)
				{
				case _LMF_DEFINITION_REC:
					sprintf(addr,"Def:%s",
						Get4Digit(cfpos-hl[i].file_pos-HDRSIZE));
					break;
				case _LMF_COMMENT_REC:
					if(cfpos<hl[i].file_pos+HDRSIZE+DATSIZE)
						sprintf(addr,"Com:%s",
							Get4Digit(cfpos-hl[i].file_pos-HDRSIZE));
					break;
				case _LMF_DATA_REC:
				case _LMF_FIXUP_SEG_REC:
					if(cfpos<hl[i].file_pos+HDRSIZE+DATSIZE)
						sprintf(addr,(hl[i].header.rec_type==_LMF_DATA_REC)?
								"Dat:%s":"Fix:%s",
							Get4Digit(cfpos-hl[i].file_pos-HDRSIZE));
/*					else
						if(((xdef.seg[hl[i].data.index]>>28)&0xf)==_LMF_CODE)
							sprintf(addr,"C:%06X",(cfpos-hl[i].file_pos+
								hl[i].data.offset-HDRSIZE-
								DATSIZE));
						else
							sprintf(addr,"D:%06X",(cfpos-hl[i].file_pos+
								hl[i].data.offset-HDRSIZE-
								DATSIZE));*/
					return false;
					break;
				case _LMF_FIXUP_80X87_REC:
					sprintf(addr,"F87:%s",
						Get4Digit(cfpos-hl[i].file_pos-HDRSIZE));
					break;
				case _LMF_EOF_REC:
					sprintf(addr,"Eof:%s",
						Get4Digit(cfpos-hl[i].file_pos-HDRSIZE));
					break;
				case _LMF_RESOURCE_REC:
					sprintf(addr,"Res:%s",
						Get4Digit(cfpos-hl[i].file_pos-HDRSIZE));
					break;
				case _LMF_ENDDATA_REC:
					sprintf(addr,"EnD:%s",
						Get4Digit(cfpos-hl[i].file_pos-HDRSIZE));
					break;
				default:
					return false;
				}
			return true;
		}
	}
	return false;
}

static __filesize_t __FASTCALL__ lmf_va2pa(__filesize_t va)
{
	unsigned i,j;
	int seclen;
	__filesize_t addr=0;
	__filesize_t newva=0;
	if(DEF.cflags&_PCF_32BIT)
	{
		for(i=0;i<seg_num;i++)
			if(va>segbase[i]&&va<segbase[i+1])
			{
				newva=va-segbase[i];
				break;
			}
		if(newva>(xdef.seg[i]&0x0fffffff)) return 0;
	}
	else
	{
		i=va>>19;
		newva=va&0xffff;
		if(i>seg_num||newva>(xdef.seg[i]&0x0ffff)) return 0;
	}
	for(j=1;j<reclast;j++)
	{
		seclen=hl[j].header.data_nbytes-DATSIZE;
		if(hl[j].header.rec_type==_LMF_DATA_REC&&
			hl[j].data.index==i&&
			hl[j].data.offset<=newva&&
			hl[j].data.offset+seclen>newva) break;
	}
	if(i==reclast) return 0;
	addr=hl[j].file_pos+newva-hl[j].data.offset+HDRSIZE+DATSIZE;

	return addr;
}

static __filesize_t __FASTCALL__ lmf_pa2va(__filesize_t pa)
{
	unsigned i;
	int seclen;
	__filesize_t addr=0;
	for(i=1;i<reclast;i++)
	{
		seclen=hl[i].header.data_nbytes-DATSIZE;
		if(hl[i].file_pos<=pa&&
			hl[i].file_pos+seclen>pa)
		{
			if(hl[i].header.rec_type==_LMF_DATA_REC) break;
			else return 0;
		}
	}
	addr=hl[i].data.offset+pa-(hl[i].file_pos+HDRSIZE+DATSIZE);
	if(DEF.cflags&_PCF_32BIT)
		addr+=segbase[hl[i].data.index];
	else
		addr|=(hl[i].data.index<<19)|((DEF.cflags&_PCF_PRIVMASK)<<14);
	return addr;
}

static bool __FASTCALL__ lmf_ReadSecHdr(BFile& handle,memArray *obj,unsigned nnames)
{
	unsigned i;
	char tmp[30];
	char stmp[80];
	UNUSED(handle);
	UNUSED(nnames);
	for(i=0;i<=reclast;i++)
	{
		switch(hl[i].header.rec_type)
		{
		case _LMF_DEFINITION_REC:
			sprintf(tmp,"%s %s",
				(DEF.cflags&_PCF_32BIT)?"32-bit":"16-bit",
				(DEF.cflags&_PCF_FLAT)?"flat model":"");
			sprintf(stmp," %2d %-17s<%2d> %s",i+1,
				lmftypes[hl[i].header.rec_type],seg_num,tmp);
			break;
		case _LMF_DATA_REC:
		case _LMF_FIXUP_SEG_REC:
		case _LMF_FIXUP_LINEAR_REC:
			sprintf(tmp,"%s (%s)",lmftypes[hl[i].header.rec_type],
				((xdef.seg[hl[i].data.index]>>28)==_LMF_CODE)?
					"code":"data");
			if(DEF.cflags&_PCF_32BIT)
				sprintf(stmp," %2d %-18s %2d %08lX to %08lX",
					i+1,
					tmp,
					hl[i].data.index,
					(unsigned long)hl[i].data.offset,
					(unsigned long)hl[i].data.offset+
						hl[i].header.data_nbytes-HDRSIZE-DATSIZE);
			else
				sprintf(stmp," %2d %-18s %2d %04lX to %04lX        ",
					i+1,
					tmp,
					hl[i].data.index,
					(unsigned long)hl[i].data.offset,
					(unsigned long)hl[i].data.offset+
						hl[i].header.data_nbytes-HDRSIZE-DATSIZE);
			break;
		case _LMF_RESOURCE_REC:
			sprintf(tmp,"%s%s",lmftypes[hl[i].header.rec_type],
				(hl[i].res.resource_type==0)?"(usage)":"");
			sprintf(stmp," %2d %-18s",
				i+1,tmp);
			break;
		default:
			sprintf(stmp," %2d %-18s",i+1,
				(hl[i].header.rec_type<10)?
					lmftypes[hl[i].header.rec_type]:lmftypes[10]);
		}
		if(!ma_AddString(obj,stmp,true)) break;
	}
	return true;
}

static unsigned __FASTCALL__ lmf_SecHdrNumItems(BFile& handle)
{
	UNUSED(handle);
	return reclast+1;
}

static __filesize_t __FASTCALL__ lmf_ShowSecLst(void)
{
	__filesize_t fpos;
	int ret;
	fpos=BMGetCurrFilePos();
	ret=fmtShowList(lmf_SecHdrNumItems,lmf_ReadSecHdr,
		" Num Type              Seg Virtual addresses   ",
		LB_SELECTIVE,NULL);
	if(ret!=-1)
		fpos=hl[ret].file_pos;
	return fpos;
}

static __filesize_t __FASTCALL__ lmf_ShowHeader( void )
{
	unsigned i,j,k;
	__filesize_t fpos;
	TWindow *w;
	char hdr[81];
	char tmp[30];
	unsigned keycode;
/*	unsigned long entrya;*/
	fpos = BMGetCurrFilePos();
	sprintf(hdr," QNX%d Load Module Format Header ",DEF.version_no/100);
	sprintf(tmp,"%s%sPrivity=%d%s%s",
		(DEF.cflags&_PCF_LONG_LIVED)?"Long lived, ":"",
		(DEF.cflags&_PCF_32BIT)?"32-bit, ":"",
		(DEF.cflags&_PCF_PRIVMASK)>>2,
		(DEF.cflags&_PCF_FLAT)?", Flat model":"",
		(DEF.cflags&_PCF_NOSHARE)?", NoShare":"");
	if(strlen(tmp)>30) j=5;
	else j=1;
	k=seg_num+j+1;
	if(k<14) k=14;
	w=CrtDlgWndnls(hdr,64,k);
	twGotoXY(1,1);
	twPrintF(
		"Version       = %d.%02d\n"
		"Code flags    = %04XH\n"
		"(%s)\n"
		"CPU/FPU       = %d/%d\n",
		DEF.version_no/100,DEF.version_no%100,DEF.cflags,
		tmp,DEF.cpu,DEF.fpu);
	twPrintF(
		"Code index    = %d\n"
		"Stack index   = %d\n"
		"Heap index    = %d\n"
		"Argv index    = %d\n"
		"Code offset   = %08lXH\n"
		"Stack size    = %08lXH\n"
		"Heap size     = %08lXH\n"
		"Flat offset   = %08lXH\n"
		"Unmapped size = %08lXH\n",
		DEF.code_index,
		DEF.stack_index,
		DEF.heap_index,
		DEF.argv_index,
		DEF.code_offset,
		DEF.stack_nbytes,
		DEF.heap_nbytes,
		DEF.flat_offset,
		DEF.unmapped_size);
	twGotoXY(35,j++);
	twPrintF("Segments:");
	twGotoXY(35,j++);
	twPrintF("Num  Length     Type");
	for(i=0;i<seg_num;i++)
	{
		twGotoXY(35,j+i);
		twPrintF(" %2d  %08lXH  %s",i,xdef.seg[i]&0x0fffffff,
			((xdef.seg[i]>>28)==_LMF_CODE)?
				"code":"data");
	}
	twGotoXY(1,14);
	twSetColorAttr(dialog_cset.entry);
	twPrintF(
		"Entry point   = seg:%d, offset:%08lXH",
		DEF.code_index,DEF.code_offset);
	twClrEOL(); twPrintF("\n");
	twSetColorAttr(dialog_cset.main);
	while(1)
	{
		keycode=GetEvent(drawEmptyPrompt,NULL,w);
		if(keycode==KE_ENTER)
		{
			if(DEF.cflags&_PCF_32BIT)
				fpos=lmf_va2pa(segbase[DEF.code_index]+DEF.code_offset);
			else
				fpos=lmf_va2pa((DEF.code_index<<19)+DEF.code_offset);
			break;
		}
		else
			if(keycode==KE_ESCAPE||keycode==KE_F(10)) break;
	}
	CloseWnd(w);
	return fpos;
}

static __filesize_t __FASTCALL__ lmf_LMFHlp( void )
{
  hlpDisplay(10015);
  return BMGetCurrFilePos();
}

extern const REGISTRY_BIN lmfTable=
{
	"lmf (QNX4 executable file)",
	{"LMFHlp",NULL,NULL,NULL,NULL,NULL,NULL,NULL,"SecLst",NULL},
	{lmf_LMFHlp,NULL,NULL,NULL,NULL,NULL,NULL,NULL,lmf_ShowSecLst,NULL},
	lmf_check_fmt,
	lmf_init_fmt,
	lmf_destroy_fmt,
	lmf_ShowHeader,
	NULL,
	NULL,
	lmf_platform,
	lmf_bitness,
	NULL,
	lmf_AddressResolv,
	lmf_va2pa,
	lmf_pa2va,
	NULL,
	NULL,
	NULL,
	NULL
};
} // namespace beye
