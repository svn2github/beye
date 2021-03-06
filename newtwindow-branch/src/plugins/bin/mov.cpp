#include "config.h"
#include "libbeye/libbeye.h"
using namespace	usr;
/**
 * @namespace	usr_plugins_auto
 * @file        plugins/bin/mov.c
 * @brief       This file contains implementation of decoder for MOV
 *              file format.
 * @version     -
 * @remark      this source file is part of movary vIEW project (BEYE).
 *              The movary vIEW (BEYE) is copyright (C) 1995 Nickols_K.
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
#include "libbeye/bswap.h"
#include "libbeye/kbd_code.h"
#include "plugins/disasm.h"
#include "plugins/bin/mmio.h"
#include "libbeye/bstream.h"
#include "plugins/binary_parser.h"
#include "beye.h"

namespace	usr {
#define MOV_FOURCC(a,b,c,d) ((a<<24)|(b<<16)|(c<<8)|(d))
    class MOV_Parser : public Binary_Parser {
	public:
	    MOV_Parser(BeyeContext& b,binary_stream&,CodeGuider&,udn&);
	    virtual ~MOV_Parser();

	    virtual const char*		prompt(unsigned idx) const;

	    virtual __filesize_t	show_header() const;
	    virtual int			query_platform() const;

	    __filesize_t		mov_find_chunk(__filesize_t off,unsigned long id);
	private:
	    BeyeContext&	bctx;
	    binary_stream&	main_handle;
	    udn&		_udn;
    };
static const char* txt[]={ "", "", "", "", "", "", "", "", "", "" };
const char* MOV_Parser::prompt(unsigned idx) const { return txt[idx]; }

__filesize_t MOV_Parser::mov_find_chunk(__filesize_t off,unsigned long id)
{
    unsigned long ids,size;
    main_handle.seek(off,binary_stream::Seek_Set);
    while(!main_handle.eof())
    {
	size=be2me_32(main_handle.read(type_dword));
	if(size < 8) return -1;
	ids=be2me_32(main_handle.read(type_dword));
	if(ids==id) return main_handle.tell()-8;
	main_handle.seek(size-8,binary_stream::Seek_Cur);
    }
    return -1;
}


MOV_Parser::MOV_Parser(BeyeContext& b,binary_stream& h,CodeGuider& code_guider,udn& u)
	    :Binary_Parser(b,h,code_guider,u)
	    ,bctx(b)
	    ,main_handle(h)
	    ,_udn(u)
{
    __filesize_t moov,mdat;
    moov=mov_find_chunk(0,MOV_FOURCC('m','o','o','v'));
    mdat=mov_find_chunk(0,MOV_FOURCC('m','d','a','t'));
    if(!(moov != __filesize_t(-1) && mdat != __filesize_t(-1))) throw bad_format_exception();
}
MOV_Parser::~MOV_Parser() {}
int MOV_Parser::query_platform() const { return DISASM_DEFAULT; }

__filesize_t MOV_Parser::show_header() const
{
    bctx.ErrMessageBox("Not implemented yet!","MOV format");
    return bctx.tell();
}

static Binary_Parser* query_interface(BeyeContext& b,binary_stream& h,CodeGuider& _parent,udn& u) { return new(zeromem) MOV_Parser(b,h,_parent,u); }
extern const Binary_Parser_Info mov_info = {
    "MOV file format",	/**< plugin name */
    query_interface
};
} // namespace	usr
