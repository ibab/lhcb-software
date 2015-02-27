// $Header: $
// ====================================================================
// Elf.cpp
// --------------------------------------------------------------------
//
// Author    : Markus Frank
//
// ====================================================================
#ifndef _WIN32
#include "RTL/Elf.h"
#include <cerrno>
#include <cstdio>
#include <sstream>
#include <cstring>
#include <climits>
#include <stdexcept>
// Linux include files
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <cxxabi.h>

//#define MMAP_IMAGES 1

namespace Elf {
  unsigned long ElfOutputStyle = ~0x2UL;
}
using namespace Elf;
using namespace std;

unsigned long pg_size = 4096;

static bool elf_test_memory(const void* start, const void* end)  {
#if 0
  const char* s = (const char*)start;
  const char* e = (const char*)end;
  const char* f = e+pg_size-1;
  s = (const char*)( (((unsigned long)s)/pg_size) * pg_size);
  e = (const char*)( (((unsigned long)f)/pg_size) * pg_size);

  size_t len = e-s;
  size_t ll = (len+pg_size-1)/pg_size;
  unsigned char* buff = new char unsigned [ll];
  int ret = mincore((void*)s,len,buff);
  for(size_t i=0; i<ll;++i)  {
    if ( (buff[i]&0x1) == 0 ) {
      char text[256];
      ::snprintf(text,sizeof(text),"+++ Memory range %p -> %p not mapped!\n",s,e);
      throw runtime_error(text);
    }
  }
  ::printf("+++ Memory range %p -> %p mapped!\n",s,e);
#endif
  if ( start && end ) {}
  return true;
}

/// Modify output style
unsigned long Elf::elf_set_output_style(unsigned long int flags)   {
  unsigned long tmp = ElfOutputStyle;
  ElfOutputStyle = flags;
  return tmp;
}

#define STR_DEF(x) case x: return #x

const char* Elf::elf_image_header_type(int typ)   {
  /* Legal values for e_type (object file type).  */
  switch(typ)  {
    STR_DEF(ET_NONE);//		0		/* No file type */
    STR_DEF(ET_REL);//		1		/* Relocatable file */
    STR_DEF(ET_EXEC);//		2		/* Executable file */
    STR_DEF(ET_DYN);//		3		/* Shared object file */
    STR_DEF(ET_CORE);//		4		/* Core file */
    STR_DEF(ET_NUM);//		5		/* Number of defined types */
    STR_DEF(ET_LOOS);//		0xfe00		/* OS-specific range start */
    STR_DEF(ET_HIOS);//		0xfeff		/* OS-specific range end */
    STR_DEF(ET_LOPROC);//	0xff00		/* Processor-specific range start */
    STR_DEF(ET_HIPROC);//	0xffff		/* Processor-specific range end */
  default:    return "UNKNOWN";
  }
}

const char* Elf::elf_section_header_type(int typ)   {
  switch(typ)  {
    STR_DEF(SHT_NULL);//	  0		/* Section header table entry unused */
    STR_DEF(SHT_PROGBITS);//	  1		/* Program data */
    STR_DEF(SHT_SYMTAB);//	  2		/* Symbol table */
    STR_DEF(SHT_STRTAB);//	  3		/* String table */
    STR_DEF(SHT_RELA);//	  4		/* Relocation entries with addends */
    STR_DEF(SHT_HASH);//	  5		/* Symbol hash table */
    STR_DEF(SHT_DYNAMIC);//	  6		/* Dynamic linking information */
    STR_DEF(SHT_NOTE);//	  7		/* Notes */
    STR_DEF(SHT_NOBITS);//	  8		/* Program space with no data (bss) */
    STR_DEF(SHT_REL);//		  9		/* Relocation entries, no addends */
    STR_DEF(SHT_SHLIB);//	  10		/* Reserved */
    STR_DEF(SHT_DYNSYM);//	  11		/* Dynamic linker symbol table */
    STR_DEF(SHT_INIT_ARRAY);//	  14		/* Array of constructors */
    STR_DEF(SHT_FINI_ARRAY);//	  15		/* Array of destructors */
    STR_DEF(SHT_PREINIT_ARRAY);// 16		/* Array of pre-constructors */
    STR_DEF(SHT_GROUP);//	  17		/* Section group */
    STR_DEF(SHT_SYMTAB_SHNDX);//  18		/* Extended section indeces */
    STR_DEF(SHT_NUM);//		  19		/* Number of defined types.  */
    STR_DEF(SHT_LOOS);//	  0x60000000	/* Start OS-specific.  */
    STR_DEF(SHT_GNU_ATTRIBUTES);// 0x6ffffff5	/* Object attributes.  */
    STR_DEF(SHT_GNU_HASH);//	  0x6ffffff6	/* GNU-style hash table.  */
    STR_DEF(SHT_GNU_LIBLIST);//	  0x6ffffff7	/* Prelink library list */
    STR_DEF(SHT_CHECKSUM);//	  0x6ffffff8	/* Checksum for DSO content.  */
    STR_DEF(SHT_LOSUNW);//	  0x6ffffffa	/* Sun-specific low bound.  */
    STR_DEF(SHT_SUNW_COMDAT);//   0x6ffffffb
    STR_DEF(SHT_SUNW_syminfo);//  0x6ffffffc
    STR_DEF(SHT_GNU_verdef);//	  0x6ffffffd	/* Version definition section.  */
    STR_DEF(SHT_GNU_verneed);//	  0x6ffffffe	/* Version needs section.  */
    STR_DEF(SHT_GNU_versym);//	  0x6fffffff	/* Version symbol table.  */
    STR_DEF(SHT_LOPROC);//	  0x70000000	/* Start of processor-specific */
    STR_DEF(SHT_HIPROC);//	  0x7fffffff	/* End of processor-specific */
  default:    return "UNKNOWN";
  }
}

const char* Elf::elf_symbol_type(int typ)   {
  switch(typ)  {
    STR_DEF(STT_NOTYPE);//	0		/* Symbol type is unspecified */
    STR_DEF(STT_OBJECT);//	1		/* Symbol is a data object */
    STR_DEF(STT_FUNC);//	2		/* Symbol is a code object */
    STR_DEF(STT_SECTION);//	3		/* Symbol associated with a section */
    STR_DEF(STT_FILE);//	4		/* Symbol's name is file name */
    STR_DEF(STT_COMMON);//	5		/* Symbol is a common data object */
    STR_DEF(STT_TLS);//		6		/* Symbol is thread-local data object*/
    STR_DEF(STT_NUM);//		7		/* Number of defined types.  */
    STR_DEF(STT_NUM+STT_OBJECT);//8
    //STR_DEF(STT_LOOS);//	10		/* Start of OS-specific */
    STR_DEF(STT_GNU_IFUNC);//	10		/* Symbol is indirect code object */
    STR_DEF(STT_HIOS);//	12		/* End of OS-specific */
    STR_DEF(STT_LOPROC);//	13		/* Start of processor-specific */
    STR_DEF(STT_HIPROC);//	15		/* End of processor-specific */
  default: return "UNKNOWN";
  }
}

const char* Elf::elf_symbol_bind(int typ)   {
  switch(typ)  {
    STR_DEF(STB_LOCAL);//	0		/* Local symbol */
    STR_DEF(STB_GLOBAL);//	1		/* Global symbol */
    STR_DEF(STB_WEAK);//	2		/* Weak symbol */
    STR_DEF(STB_NUM);//		3		/* Number of defined types.  */
    //STR_DEF(STB_LOOS);//	10		/* Start of OS-specific */
    STR_DEF(STB_GNU_UNIQUE);//	10		/* Unique symbol.  */
    STR_DEF(STB_HIOS);//	12		/* End of OS-specific */
    STR_DEF(STB_LOPROC);//	13		/* Start of processor-specific */
    STR_DEF(STB_HIPROC);//	15		/* End of processor-specific */
  default: return "UNKNOWN";
  }
}

const char* Elf::elf_symbol_index_type(int typ)   {
  switch(typ)  {
    STR_DEF(SHN_UNDEF);//	0		/* Undefined section */
    // STR_DEF(SHN_LORESERVE);//	0xff00		/* Start of reserved indices */
    // STR_DEF(SHN_LOPROC);//	0xff00		/* Start of processor-specific */
    STR_DEF(SHN_BEFORE);//	0xff00		/* Order section before all others (Solaris).  */
    STR_DEF(SHN_AFTER);//	0xff01		/* Order section after all others  (Solaris).  */
    STR_DEF(SHN_HIPROC);//	0xff1f		/* End of processor-specific */
    STR_DEF(SHN_LOOS);//	0xff20		/* Start of OS-specific */
    STR_DEF(SHN_HIOS);//	0xff3f		/* End of OS-specific */
    STR_DEF(SHN_ABS);//		0xfff1		/* Associated symbol is absolute */
    STR_DEF(SHN_COMMON);//	0xfff2		/* Associated symbol is common */
    STR_DEF(SHN_XINDEX);//	0xffff		/* Index is in extra table.  */
    //STR_DEF(SHN_HIRESERVE);//	0xffff		/* End of reserved indices */
  default: return "";
  }
}
#undef STR_DEF

static int raw_image_count = 0;
RawImage::RawImage() 
  : refCount(0), map_len(0), header(0), sectionNames(0), dynstrNames(0), strtabNames(0), name(), symbolSections()
{
  ++raw_image_count;
  ::printf("+++ Create raw image [%d]: %p!\n",raw_image_count,(void*)this);
}
RawImage::~RawImage() {
  if ( map_len > 0 )   {
#ifdef MMAP_IMAGES
    ::munmap(header,map_len);
#else
    ::free(header);
#endif
    map_len = 0;
  }
  --raw_image_count;
  ::printf("+++ Delete raw image [%d]: %p!\n",raw_image_count,(void*)this);
}

long RawImage::release()  {
  long tmp = --refCount;
  if ( tmp <= 0 )   {
    delete this;
  }
  return tmp;
}

/// Default destructor
Image::~Image()  {
}

/// Get header from image buffer
Image Image::fromImage(const void* img)   {
  Image hdr;
  hdr.setImage(img);
  return hdr;
}

#ifndef MMAP_IMAGES
namespace  {
  int file_read(int con, void* buff, int len)  {
    int tmp = 0;
    char* p = (char*)buff;
    while ( tmp < len )  {
      int sc = ::read(con,p+tmp,len-tmp);
      if ( sc >  0 ) tmp += sc;
      else if ( sc == 0 ) return 0;
      else                return -1;
    }
    return 1;
  }
}
#endif

Image Image::mapImage(const char* name)    {
  if ( name && *name )   {
    struct stat buff;
    if ( 0 == ::stat(name,&buff) )   {
      if ( (buff.st_mode&S_IFLNK) == S_IFLNK )  {
	char text[PATH_MAX];
	ssize_t len = ::readlink(name,text,sizeof(text)-1);
	if ( len > 0 )  {
	  text[len] = 0;
	  return mapImage(text);
	}
      }
      else if ( (buff.st_mode&S_IFREG) == S_IFREG )  {
	char text[PATH_MAX];
	int fd = ::open(name,O_RDONLY);
	if ( !fd )  {
	  ::snprintf(text,sizeof(text),"Failed to open file:%s Error:%s\n",
		     name, strerror(errno));
	  throw runtime_error(text);
	}
	size_t len = ((buff.st_size+pg_size-1)/pg_size) * pg_size;
#ifdef MMAP_IMAGES
	void* ptr = ::mmap64(0,len,PROT_READ,MAP_PRIVATE,fd,0);
#else
	void* ptr = ::malloc(len);
	if ( ptr ) ::file_read(fd,ptr,buff.st_size);
#endif
	if ( !ptr )   {
	  ::close(fd);
	  ::snprintf(text,sizeof(text),"Failed to open file:%s Error:%s\n",
		     name, strerror(errno));
	  throw runtime_error(text);
	}
	::printf("+++ Mapped %ld bytes of image %s\n",long(buff.st_size),name);
	Image hdr = fromImage(ptr);
	hdr.m_ptr->name = name;
	hdr.m_ptr->map_len = buff.st_size;
	return hdr;
      }
      throw runtime_error("Unknown file type of "+string(name)+"to memory map Elf image!");
    }
    throw runtime_error("Nonexisting file "+string(name)+"to memory map Elf image!");
  }
  throw runtime_error("Invalid (empty) name to memory map Elf image!");
}

/// Default constructor
void Image::setImage(const void* image) {
  if ( !isValid(image) )   {
    stringstream str;
    str << "Image at address 0x" << image << " is not an ELF image!";
    throw runtime_error(str.str());
  }
  m_ptr = new RawImage;
  m_ptr->refCount       = 1;
  m_ptr->sh_length      = 0;
  m_ptr->pgm_memLength  = 0;
  m_ptr->pgm_fileLength = 0;
  m_ptr->header         = (Elf64_Ehdr*)image;
  m_ptr->sectionNames   = 0;
  m_ptr->dynstrNames    = 0;
  m_ptr->strtabNames    = 0;
  m_ptr->symbolSections.clear();
  testSectionHeaderMemory();
  Elf64_Shdr* shdr = sectionHeader(m_ptr->header->e_shstrndx).second;
  m_ptr->sectionNames = (char*)(start() + shdr->sh_offset);
  for(int i=0, n=shNumber(); i<n; ++i)   {
    shdr = sectionHeader(i).second;
    if ( shdr->sh_type == SHT_STRTAB )   {
      const byte_t* str_tab = start() + shdr->sh_offset;
      const char* sec_name = (char*)(m_ptr->sectionNames + shdr->sh_name);
      elf_test_memory(str_tab,str_tab+shdr->sh_size);
      m_ptr->sh_length += shdr->sh_size;
      ::printf(" String table:%s\n",sec_name);
      if ( 0 == ::strcmp(sec_name,".dynstr") )
	m_ptr->dynstrNames = (char*)str_tab;
      else if ( 0 == ::strcmp(sec_name,".strtab") )
	m_ptr->strtabNames = (char*)str_tab;
    }
    else if ( shdr->sh_type == SHT_SYMTAB || shdr->sh_type == SHT_DYNSYM )   {
      m_ptr->symbolSections.push_back(make_pair(i,shdr));
    }
  }

  // Calculating the program sections size
  for(int i=0, n=pgmNumber(); i<n; ++i)   {
    const Elf64_Phdr* phdr = (Elf64_Phdr*)programHeader(i).second;
    m_ptr->pgm_fileLength += phdr->p_filesz;
    m_ptr->pgm_memLength  += phdr->p_memsz;
  }
}

/// Name of image from header structure
string Image::name(const char* fmt)   const  {
  char text[PATH_MAX+1024];
  text[0] = 0;
  if ( !m_ptr->name.empty() )  {
    ::snprintf(text,sizeof(text),fmt,m_ptr->name.c_str());
  }
  return text;
}

/// Name of image from header structure
string Image::name(const Header& header, const char* fmt)  {
  return Image(header).name(fmt);
}

/// Print Elf image header
void Image::printAll()  const  {
  ::printf("++++++++++++++++++ Elf information%s:\n",name(" of image '%s'").c_str());
  printHeader();
  if ( ElfOutputStyle&ELF_SECTION_HEADERS ) printSections();
  if ( ElfOutputStyle&ELF_STRING_TABLES   ) printStringTables();
  if ( ElfOutputStyle&ELF_SYMBOL_TABLES   ) printSymbolTables();
  if ( ElfOutputStyle&ELF_PROGRAM_HEADERS ) printProgramHeaders();
}

/// Print Elf image header
void Image::printAll(const Header& header)   {
  Image(header).printAll();
}

/// Print Elf image header
void Image::printHeader()  const  {
  ::printf("++++++++++++++++++ Elf Header%s:\n",name(" of image '%s'").c_str());
  this->Header::print();
}

/// Print Elf image header
void Image::printHeader(const Header& header)  {
  Image(header).print();
}
 
/// Print Elf program headers
void Image::printProgramHeaders()  const  {
  printf("   Elf64_Ehdr: e_phnum:%d e_phoff:%ld e_phentsize:%d\n",
	 pgmNumber(), pgmOffset(), pgmEntrySize());
  for(int i=0, n=pgmNumber(); i<n; ++i)   {
    const Elf64_Phdr* phdr = (Elf64_Phdr*)programHeader(i).second;
    printf("   program_header %2d: address=%10p typ:%08X offset:%4ld \n",
	   i,(void*)phdr,phdr->p_type,phdr->p_offset);
  } 
}
 
/// Print Elf program headers
void Image::printProgramHeaders(const Header& header)  {
  Image(header).printProgramHeaders();
}
 
/// Print Elf section headers
void Image::printSections()  const  {
  ::printf("++++++++++++++++++ Section dump%s:\n",name(" of image '%s'").c_str());
  for(int isection=0; isection<shNumber(); ++isection)
    Section(sectionHeader(isection)).print();
}

/// Print Elf section headers
void Image::printSections(const Header& header)  {
  Image(header).printSections();
}

/// Print Elf string tables of the image
void Image::printStringTables()  const {
  const char* img = (const char*)start();
  ::printf("++++++++++++++++++ String table dump%s:\n",name(" of image '%s'").c_str());
  for(int isection=0; isection<shNumber(); ++isection)   {
    Section s(sectionHeader(isection));
    if ( s.type() == SHT_STRTAB )  {
      StringTable strtab(sectionHeader(isection));
      strtab.testSectionDataMemory();
      s.print();
      StringTable::const_iterator sec_start = strtab.begin();
      for(StringTable::const_iterator i=strtab.begin(); i!=strtab.end(); i=strtab.next(i))
	::printf(" String table %-12s file-offset:%5ld section-offset:%5ld entry:'%s'\n",
		 s.name(),long(*i-img),long(*i-*sec_start),*i);
    }
  }
}

/// Print Elf string tables of the image
void Image::printStringTables(const Header& header)  {
  Image(header).printStringTables();
}

/// Print Elf symbols in this image
void Image::printSymbolTables()  const  {
  ::printf("++++++++++++++++++ Symbol dump%s:\n",name(" of image '%s'").c_str());
  for(int isection=0; isection<shNumber(); ++isection)   {
    Section s(sectionHeader(isection));
    if ( s.type() == SHT_SYMTAB || s.type() == SHT_DYNSYM || s.type() == SHT_DYNAMIC )   {
      SymbolTable symtab(sectionHeader(isection));
      symtab.testSectionDataMemory();
      s.print();
      for(SymbolTable::const_iterator i=symtab.begin(); i!=symtab.end(); i=symtab.next(i))
	(*i).print();
    }
  }
}

/// Print Elf symbols in this image
void Image::printSymbolTables(const Header& header)  {
  Image(header).printSymbolTables();
}

/// Initializing constructor
Header::Header(RawImage* img) : m_ptr(img)  {
  if ( m_ptr ) m_ptr->addRef();
}

/// Copy constructor
Header::Header(const Header& copy) : m_ptr(copy.m_ptr)  {
  m_ptr = copy.m_ptr;
  if ( m_ptr ) m_ptr->addRef();
}

/// Default destructor
Header::~Header()  {
  if ( m_ptr ) m_ptr->release();
  m_ptr = 0;
}

/// Print ELF image header
void Header::print(const char* prefix)  const  {
  const char* begin = (char*)start();
  const byte_t* ehdr_ident = identification();
  if ( !prefix ) prefix = "";
  ::printf("%s   ELF image address=%10p  magic:%2X %c%c%c type:%3d %-12s vsn:%1d Mach:%3d flags:%04X\n",
	   prefix, (void*)begin,
	   ehdr_ident[0], ehdr_ident[1], ehdr_ident[2], ehdr_ident[3],
	   type(), typeStr(), version(), machine(), flags() );
  ::printf("%s   String tables: .dynstr: %10p offset:%ld .strtab:%10p offset:%ld\n"
	   "%s   Section Headers: num:%4d offset:%6ld entry size:%3d length:%d bytes string table index:%d\n"
	   "%s   Program Headers: num:%4d offset:%6ld entry size:%3d length(memory):%d (file):%d bytes\n",
	   prefix, 
	   (void*)m_ptr->dynstrNames, m_ptr->dynstrNames-begin,
	   (void*)m_ptr->strtabNames, m_ptr->sectionNames-begin,
	   prefix, shNumber(), long(shOffset()), shEntrySize(), shLength(), nameIndex(),
	   prefix, pgmNumber(), long(pgmOffset()), pgmEntrySize(), pgmMemLength(), pgmFileLength()
	   );
}

/// Assignment operator
Header& Header::operator=(const Header& copy)  {
  if ( m_ptr != copy.m_ptr )   {
    if ( m_ptr ) m_ptr->release();
    m_ptr = copy.m_ptr;
    if ( m_ptr ) m_ptr->addRef();
  }
  return *this;
}

/// IsValid the identification words 
bool Header::isValid(const void* ptr)   {
  if ( ptr )  {
    const Elf64_Ehdr* ehdr = (const Elf64_Ehdr*)ptr;
    const unsigned char* e = ehdr->e_ident;
    return e[EI_MAG0] == ELFMAG0 && 
      e[EI_MAG1] == ELFMAG1 && 
      e[EI_MAG2] == ELFMAG2 && 
      e[EI_MAG3] == ELFMAG3;
  }
  return false;
}

/// IsValid the identification words 
bool Header::isValid()  const  {
  return m_ptr ? Header::isValid(m_ptr->header) : false;
}

/// Image type as string
const char* Header::typeStr() const   {
  return elf_image_header_type(type());
}

/// Test the image memory up to the end of the section headers
void Header::testSectionHeaderMemory()  const   {
  elf_test_memory(start(),sectionHeaders()+shNumber()*shEntrySize());
}

/// Copy constructor
Section::Section(const Section& s) 
  : Handle<Elf64_Shdr>(s), header(s.header) 
{
}

/// Initializing constructor
Section::Section(const pair<RawImage*,Elf64_Shdr*>& h)
  : Handle<Elf64_Shdr>(h.second), header(h.first) 
{
}

/// Initializing constructor
Section::Section(const Header& h,int num_section) : Handle<Elf64_Shdr>(0), header(h)   {
  const byte_t* p = header.sectionHeaders() + num_section*header.shEntrySize();
  m_ptr = (Elf64_Shdr*)p;
}

/// Assignment operator
Section& Section::operator=(const Section& s)  {
  this->Handle<Elf64_Shdr>::operator=(s);
  header = s.header;
  return *this;
}

/// Image type as string
const char* Section::typeStr() const   {
  return Elf::elf_section_header_type(type());
}

/// Name of the section as string from the string table
const char* Section::name()  const   {
  return (const char*)header.sectionNames() + nameIndex();
}

/// Print section information
void Section::print(const char* prefix) const   {
  ::printf("%s+++ Section %-20s %10p addr:%10x name:%4d type:%8X %-16s\n"
	   "                off:%6ld size:%4ld link:%4d \n",
	   prefix, name(), (void*)ptr(), address(), nameIndex(), type(), typeStr(),
	   offset(), size(), link());
}

/// Test section data for accessibility
void Section::testSectionDataMemory()  {
  elf_test_memory(dataBegin(), dataEnd());
}

/// Initializing constructor
Symbol::Symbol(const Header& hdr, Elf64_Shdr* sec, Elf64_Sym* sym)
  : Handle<Elf64_Sym>(sym), header(hdr), section(sec)
{
}

/// Copy constructor
Symbol::Symbol(const Symbol& s)
  : Handle<Elf64_Sym>(s), header(s.header), section(s.section)
{
}

/// Assignment operator
Symbol& Symbol::operator=(const Symbol& symbol)   {
  m_ptr   = symbol.m_ptr;
  section = symbol.section;
  header  = symbol.header;
  return *this;
}

/// Equality operator
bool Symbol::operator==(const Symbol& symbol)  const  {
  return m_ptr == symbol.m_ptr && section == symbol.section && header == symbol.header;
}

/// Name of the symbol as string from the string table
const char* Symbol::name()  const   {
  int typ = section->sh_type;
  switch(typ)  {
  case SHT_DYNAMIC:
  case SHT_SYMTAB:
  case SHT_DYNSYM:  {
    Elf32_Word link = section->sh_link;
    Section strtab(header.sectionHeader(link));
    return (char*)(strtab.dataBegin() + nameIndex());
  }
  default:
    return "----";
  }
}

/// Name of the symbol as string after demangling
string Symbol::realname()  const   {
  const char* n = name();
  int status = 0;
  string buff;
  if ( n && *n )  {
    char* real = abi::__cxa_demangle(n, 0, 0, &status);
    if ( 0 == real ) return n;
    buff = real;
    ::free(real);
  }
  return buff;
}

/// Symbol type in string form
const char* Symbol::typeStr() const  {
  return elf_symbol_type(type());
}

/// Symbol binding in string form
const char* Symbol::bindingStr() const  {
  return elf_symbol_bind(binding());
}

/// ELF Symbol Section header
pair<RawImage*,Elf64_Shdr*> Symbol::codeSectionHeader()  const  {
  return header.sectionHeader(codeSectionIndex());
}

/// Access the data section with the symbol code
Section Symbol::codeSection()  const   {
  return Section(codeSectionHeader());
}

/// Access to the function pointer
Symbol::byte_t* Symbol::function()  const   {
  Elf64_Addr sym_offset = 0xFFF&value();
  byte_t* sym_pointer = header.start() + sym_offset;// + func_shdr->sh_offset;
  return sym_pointer;
}

/// ELF Symbol section index name
const char* Symbol::codeSectionIndexType() const   {
  return Elf::elf_symbol_index_type(codeSectionIndex());
}

/// Print symbol information
void Symbol::print(const char* prefix)  const  {
  const char* n = name();
  int style = ElfOutputStyle;
  if ( (style&ELF_COMPACT) == ELF_COMPACT )   {
    string rn  = (style&ELF_DEMANGLE) ? realname().c_str() : name();
    int    bnd = binding();
    int    st  = codeSectionIndex();
    char   b   = ' ';
    Elf64_Addr  sym_offset = 0;
    if ( st == SHN_UNDEF ) b = 'U';
    else if ( bnd == STB_GLOBAL ) b = 'T';
    else if ( bnd == STB_WEAK   ) b = 'W';
    else if ( bnd == STB_LOCAL  ) b = 't';

    if ( st == SHN_UNDEF ) sym_offset = 0;
    else if ( st == SHN_ABS ) sym_offset = value();
    else if ( st == SHN_COMMON ) sym_offset = 0xBADADDUL;
    else sym_offset = value();
    if ( st == SHN_UNDEF )
      ::printf("%s %16s %c %s\n", prefix, "", b, rn.c_str());
    else
      ::printf("%s %016lX %c %s\n", prefix, sym_offset, b, rn.c_str());
  }
  else if ( value() )  {
    Elf64_Addr  sym_offset = 0xFFF&value();
    Section     code_sec = codeSection();
    const char* cs_name = code_sec.name();
    ::printf("%s     Symbol:'%s'\n"
	     "%s            '%s'\n"
	     "%s          Header:%10p value:%10lx fptr:%10p name:%6d type:%1X %-12s Bind:%1X %-12s \n"
	     "%s          section_idx:%4X %s size:%4ld code:%s offset:%ld\n",
	     prefix, n, prefix, realname().c_str(), 
	     prefix, (void*)ptr(), value(), (void*)function(), nameIndex(), 
	     type(),  typeStr(), binding(), bindingStr(),
	     prefix, codeSectionIndex(), codeSectionIndexType(), size(), cs_name, sym_offset);
  }
  else  {
    ::printf("%s     Symbol:'%s'\n"
	     "%s            '%s'\n"
	     "%s          Header:%10p value:%10lx fptr:%10p name:%6d type:%1X/%-8s Bind:%1X/%-10s size:%4ld\n",
	     prefix, n, prefix, realname().c_str(), 
	     prefix, (void*)ptr(), value(), (void*)function(), nameIndex(), 
	     type(),  typeStr(), binding(), bindingStr(), size());
  }
}

/// ELF data of the section      
std::pair<Symbol::byte_t*,Elf64_Xword> Symbol::data()  const  {
  Elf64_Addr sym_offset = 0xFFF&value();
  byte_t*    sym_pointer = header.start() + sym_offset;// + func_shdr->sh_offset;
  return make_pair(sym_pointer,size());
}

/// Initializing constructor
SymbolTable::SymbolTable(const pair<RawImage*,Elf64_Shdr*>& h) : Section(h) 
{
}

/// Elf Symbol Iteration advance
SymbolTable::iterator SymbolTable::next(const iterator& curr)  const    {
  Elf64_Sym *sym = curr.symbol;
  Elf64_Sym *last = (Elf64_Sym*)(header.start()+offset()+size());
  while (++sym < last)   {
    if ( ELF64_ST_TYPE(sym->st_info) == STT_FUNC )
      return iterator(header.image(),ptr(),sym);
  }
  return end();
}

/// Elf Symbol Iteration start
SymbolTable::iterator SymbolTable::begin() const   {
  Elf64_Sym *sym = (Elf64_Sym*)(header.start()+offset());
  return iterator(header.image(),ptr(),sym);
}

/// Elf Symbol Iteration end
SymbolTable::iterator SymbolTable::end() const   {
  return iterator(header.image(),0,0);
}

/// Deferencing operation
Symbol SymbolTable::iterator::operator*() const   {
  if ( image && symbol )  {
    return Symbol(image,section,symbol);
  }
  throw runtime_error("Invalid symbol!");
}

/// Deferencing operation
Symbol SymbolTable::iterator::operator->() const  {
  if ( image && symbol )  {
    return Symbol(image,section,symbol);
  }
  throw runtime_error("Invalid symbol!");
}


/// Initializing constructor
StringTable::StringTable(const pair<RawImage*,Elf64_Shdr*>& h) : Section(h) 
{
}

/// Elf String Iteration advance
StringTable::iterator StringTable::next(const iterator& curr)  const    {
  char *sym = curr.string;
  char *last = (char*)(header.start()+offset()+size());
  if ( sym && sym < last )  {
    char* p = sym + strlen(sym) + 1;
    return iterator(header.image(),p < last ? p : 0);
  }
  return iterator(header.image(),0);
}

/// Elf String Iteration start
StringTable::iterator StringTable::begin() const   {
  char *sym = (char*)(header.start()+offset());
  return iterator(header.image(),sym);
}

/// Elf String Iteration end
StringTable::iterator StringTable::end() const   {
  return iterator(header.image(),0);
}

/// Deferencing operation
const char* StringTable::iterator::operator*() const   {
  if ( string )  {
    return string;
  }
  throw runtime_error("Invalid string!");
}

/// Deferencing operation
const char* StringTable::iterator::operator->() const  {
  if ( string )  {
    return string;
  }
  throw runtime_error("Invalid string!");
}
#endif

#include <link.h>

static int callback_throw(struct dl_phdr_info *info, size_t , void*)   {
  ::printf("Image name=%s (%d segments)\n", info->dlpi_name, info->dlpi_phnum);
  if ( info->dlpi_phnum>0 )  {
    Elf::Image ehdr;
    if ( info->dlpi_name && 0 != info->dlpi_name[0] )  {
      ehdr = Image::mapImage(info->dlpi_name);
    }
    else   {
      char* elf_image_addr = (char*) (info->dlpi_addr + info->dlpi_phdr[0].p_vaddr);
      ehdr = Image::fromImage(elf_image_addr);
    }
    ehdr.printAll();
  }
  return 0;
}

int callback(struct dl_phdr_info *info, size_t size, void *data)  {
  try  {
    return callback_throw(info,size,data);
  }
  catch(const exception& e)   {
    ::printf("Exception: %s\n",e.what());
  }
  catch(...)   {
    ::printf("UNKNOWN Exception!\n");
  }
  return 0;
}
#include "RTL/rtl.h"
namespace  {

  void help()  {
    ::printf(
	     "   elf_dump -opt [-opt]                                        \n"
	     "                                                               \n"
	     "    Options:                                                   \n"
	     "    -i(input=<file>    Enter shared image name to analyse.     \n"
	     "                       If option is not present all mapped     \n"
	     "                       section of the executable are dumped.   \n"
	     "                                                               \n"
	     "    -d(emangle)        Demangle symbol names.                  \n"
	     "    -sec(tions)        Print the section table in the image.   \n"
	     "    -sym(boltables)    Print symbols from the .dynsym and      \n"
	     "                       the .symtab sections.                   \n"
	     "    -pro(gramheaders)  Print the program headers of the image. \n"
	     "    -e(xtended)        Print extended symbol information.      \n"
	     "                       Attention: very lengthy and of limited  \n"
	     "                                  interest.                    \n"
	     "    -h(elp)            Print this help.                        \n"
	     "                                                               \n");
    ::exit(0);
  }
}

extern "C" int rtl_elf_dump(int argc, char ** argv)    {
  RTL::CLI cli(argc,argv,help);
  string input;
  cli.getopt("input",1,input);
  ElfOutputStyle = cli.getopt("extended",1) ? ELF_EXTENDED : ELF_COMPACT;
  if ( cli.getopt("demangle",1)       ) ElfOutputStyle |= ELF_DEMANGLE;
  if ( cli.getopt("sections",3)       ) ElfOutputStyle |= ELF_SECTION_HEADERS;
  if ( cli.getopt("programheaders",3) ) ElfOutputStyle |= ELF_PROGRAM_HEADERS;
  if ( cli.getopt("stringtables",3)   ) ElfOutputStyle |= ELF_STRING_TABLES;
  if ( cli.getopt("symboltables",3)   ) ElfOutputStyle |= ELF_SYMBOL_TABLES;
  if ( input.empty() )  {
    char text[64];
    sprintf(text,"cat /proc/%d/maps",getpid());
    system(text);
    dl_iterate_phdr(callback, NULL);
  }
  else  {
    try {
      Elf::Image ehdr = Image::mapImage(input.c_str());
      ehdr.printAll();
    }
    catch(const exception& e)   {
      ::printf("Exception: %s\n",e.what());
    }
    catch(...)   {
      ::printf("UNKNOWN Exception!\n");
    }
  }
  exit(EXIT_SUCCESS);
}

