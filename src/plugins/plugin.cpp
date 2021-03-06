#include "plugin.h"
#include "codeguid.h"
#include "disasm.h"
#include "beyeutil.h"
#include "beye.h"
#include "libbeye/bstream.h"
#include "plugins/binary_parser.h"

namespace	usr {

bad_format_exception::bad_format_exception() throw() {}
bad_format_exception::~bad_format_exception() throw() {}
const char* bad_format_exception::what() const throw() { return "unknown format of file"; }

extern const Binary_Parser_Info bin_info;
extern const Binary_Parser_Info rm_info;
extern const Binary_Parser_Info mov_info;
extern const Binary_Parser_Info mp3_info;
extern const Binary_Parser_Info mpeg_info;
extern const Binary_Parser_Info jpeg_info;
extern const Binary_Parser_Info wav_info;
extern const Binary_Parser_Info avi_info;
extern const Binary_Parser_Info asf_info;
extern const Binary_Parser_Info bmp_info;
extern const Binary_Parser_Info ne_info;
extern const Binary_Parser_Info pe_info;
extern const Binary_Parser_Info le_info;
extern const Binary_Parser_Info lx_info;
extern const Binary_Parser_Info nlm_info;
extern const Binary_Parser_Info elf_info;
extern const Binary_Parser_Info jvm_info;
extern const Binary_Parser_Info coff_info;
extern const Binary_Parser_Info arch_info;
extern const Binary_Parser_Info aout_info;
extern const Binary_Parser_Info oldpharlap_info;
extern const Binary_Parser_Info pharlap_info;
extern const Binary_Parser_Info rdoff_info;
extern const Binary_Parser_Info rdoff2_info;
extern const Binary_Parser_Info sis_info;
extern const Binary_Parser_Info sisx_info;
extern const Binary_Parser_Info lmf_info;
extern const Binary_Parser_Info mz_info;
extern const Binary_Parser_Info dossys_info;

Bin_Format::Bin_Format(BeyeContext& bc,CodeGuider& _parent,udn& __udn)
	    :bctx(bc)
	    ,parent(_parent)
	    ,_udn(__udn)
{
    formats.push_back(&ne_info);
    formats.push_back(&pe_info);
    formats.push_back(&le_info);
    formats.push_back(&lx_info);
    formats.push_back(&nlm_info);
    formats.push_back(&elf_info);
    formats.push_back(&jvm_info);
    formats.push_back(&coff_info);
    formats.push_back(&arch_info);
    formats.push_back(&aout_info);
    formats.push_back(&oldpharlap_info);
    formats.push_back(&pharlap_info);
    formats.push_back(&rdoff_info);
    formats.push_back(&rdoff2_info);
    formats.push_back(&lmf_info);
    formats.push_back(&sis_info);
    formats.push_back(&sisx_info);
    formats.push_back(&avi_info);
    formats.push_back(&asf_info);
    formats.push_back(&bmp_info);
    formats.push_back(&mpeg_info);
    formats.push_back(&jpeg_info);
    formats.push_back(&wav_info);
    formats.push_back(&mov_info);
    formats.push_back(&rm_info);
    formats.push_back(&mp3_info);
    formats.push_back(&mz_info);
    formats.push_back(&dossys_info);
    formats.push_back(&bin_info);
}
Bin_Format::~Bin_Format() { delete detectedFormat; }

void Bin_Format::detect_format(binary_stream& handle)
{
    if(!handle.flength()) return;
    size_t i,sz=formats.size();
    for(i = 0;i < sz;i++) {
	try {
	    detectedFormat = formats[i]->query_interface(bctx,handle,parent,_udn);
	    active_format = i;
	    break;
	} catch (const bad_format_exception& ) {
	    delete detectedFormat;
	    continue;
	}
    }
}

const char* Bin_Format::name() const { return formats[active_format]->name; }

const char* Bin_Format::prompt(unsigned idx) const { return detectedFormat->prompt(idx); }	/**< on Alt-Fx selection */
__filesize_t Bin_Format::action_F1()  const { return detectedFormat->action_F1(); }
__filesize_t Bin_Format::action_F2()  const { return detectedFormat->action_F2(); }
__filesize_t Bin_Format::action_F3()  const { return detectedFormat->action_F3(); }
__filesize_t Bin_Format::action_F4()  const { return detectedFormat->action_F4(); }
__filesize_t Bin_Format::action_F5()  const { return detectedFormat->action_F5(); }
__filesize_t Bin_Format::action_F6()  const { return detectedFormat->action_F6(); }
__filesize_t Bin_Format::action_F7()  const { return detectedFormat->action_F7(); }
__filesize_t Bin_Format::action_F8()  const { return detectedFormat->action_F8(); }
__filesize_t Bin_Format::action_F9()  const { return detectedFormat->action_F9(); }
__filesize_t Bin_Format::action_F10() const { return detectedFormat->action_F10(); }

__filesize_t Bin_Format::show_header() const { return detectedFormat->show_header(); }
std::string	Bin_Format::bind(const DisMode& _parent,__filesize_t shift,bind_type flg,int codelen,__filesize_t r_shift) const
{
    return detectedFormat->bind(_parent,shift,flg,codelen,r_shift);
}

int	Bin_Format::query_platform() const { return detectedFormat->query_platform(); }
Bin_Format::bitness	Bin_Format::query_bitness(__filesize_t off) const { return detectedFormat->query_bitness(off); }
Bin_Format::endian	Bin_Format::query_endian(__filesize_t off) const { return detectedFormat->query_endian(off); }

std::string Bin_Format::address_resolving(__filesize_t off) const
{
    return detectedFormat->address_resolving(off);
}

__filesize_t Bin_Format::va2pa(__filesize_t va) const
{
    __filesize_t rc=detectedFormat->va2pa(va);
    if(!rc) rc=Plugin::Bad_Address;
    return rc;
}
__filesize_t Bin_Format::pa2va(__filesize_t pa) const
{
    __filesize_t rc=detectedFormat->pa2va(pa);
    if(!rc) rc=Plugin::Bad_Address;
    return rc;
}

Symbol_Info Bin_Format::get_public_symbol(__filesize_t pa,bool as_prev) const
{
    Symbol_Info rc=detectedFormat->get_public_symbol(pa,as_prev);
    if(!rc.pa) rc.pa = Plugin::Bad_Address;
    return rc;
}

Object_Info Bin_Format::get_object_attribute(__filesize_t pa) const { return detectedFormat->get_object_attribute(pa); }

Symbol_Info::Symbol_Info()
	:_class(Local)
	,pa(Plugin::Bad_Address)
{
}

} // namespace	usr
