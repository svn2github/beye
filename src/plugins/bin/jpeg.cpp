#include "config.h"
#include "libbeye/libbeye.h"
using namespace	usr;
/**
 * @namespace	usr_plugins_auto
 * @file        plugins/bin/jpeg.c
 * @brief       This file contains implementation of decoder for jpeg
 *              file format.
 * @version     -
 * @remark      this source file is part of jpegary vIEW project (BEYE).
 *              The jpegary vIEW (BEYE) is copyright (C) 1995 Nickols_K.
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
#include <string.h>

#include "bconsole.h"
#include "beyehelp.h"
#include "colorset.h"
#include "reg_form.h"
#include "libbeye/kbd_code.h"
#include "plugins/disasm.h"
#include "plugins/bin/mmio.h"
#include "libbeye/bstream.h"
#include "plugins/binary_parser.h"
#include "beye.h"

namespace	usr {
    class Jpeg_Parser : public Binary_Parser {
	public:
	    Jpeg_Parser(binary_stream&,CodeGuider&,udn&);
	    virtual ~Jpeg_Parser();

	    virtual const char*		prompt(unsigned idx) const;

	    virtual __filesize_t	show_header();
	    virtual int			query_platform() const;
	private:
	    binary_stream&	main_handle;
	    udn&		_udn;
    };
static const char* txt[]={ "", "", "", "", "", "", "", "", "", "" };
const char* Jpeg_Parser::prompt(unsigned idx) const { return txt[idx]; }

Jpeg_Parser::Jpeg_Parser(binary_stream& h,CodeGuider& code_guider,udn& u)
	    :Binary_Parser(h,code_guider,u)
	    ,main_handle(h)
	    ,_udn(u)
{}
Jpeg_Parser::~Jpeg_Parser() {}
int Jpeg_Parser::query_platform() const { return DISASM_DEFAULT; }

__filesize_t Jpeg_Parser::show_header()
{
    beye_context().ErrMessageBox("Not implemented yet!","JPEG format");
    return beye_context().tell();
}

static bool probe(binary_stream& main_handle) {
    unsigned long val;
    unsigned char id[4];
    main_handle.seek(0,binary_stream::Seek_Set);
    val = main_handle.read(type_dword);
    main_handle.seek(6,binary_stream::Seek_Set);
    main_handle.read(id,4);
    if(val==0xE0FFD8FF && memcmp(id,"JFIF",4)==0) return true;
    return false;
}

static Binary_Parser* query_interface(binary_stream& h,CodeGuider& _parent,udn& u) { return new(zeromem) Jpeg_Parser(h,_parent,u); }
extern const Binary_Parser_Info jpeg_info = {
    "JPEG file format",	/**< plugin name */
    probe,
    query_interface
};
} // namespace	usr
