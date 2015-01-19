// $Header: $
// ====================================================================
// Elf.h : Interface to handle Elf binary files
// --------------------------------------------------------------------
//
// Author    : Markus Frank
//
// ====================================================================
#ifndef _WIN32
#ifndef ONLINEBASE_CPP_ELF_H
#define ONLINEBASE_CPP_ELF_H

// C/C++ include files
#include <elf.h>
#include <string>
#include <vector>
#include <algorithm>


/// Elf namespace declaration: ELF image analysis helper classes
namespace Elf  {

  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   */
  template <typename O> class Handle  {
  public:
    typedef O object_t;
  protected:
    object_t* m_ptr;
  public:
    /// Default constructor
    Handle(object_t* ptr) : m_ptr(ptr)           {  }
    /// Copy constructor
    Handle(const Handle& c) : m_ptr(c.m_ptr)     {  }
    /// Assignment operator
    Handle& operator=(const Handle& copy)         {  m_ptr = copy.m_ptr; return *this; }
    /// Equivalence operator
    bool operator==(const Handle& test) const    {  return m_ptr == test.m_ptr;  }
    /// Access to the pointer
    object_t* ptr() const                        {  return m_ptr;  }
    /// Access to the underlying object
    object_t* operator->() const                 {  return m_ptr;  }
  };


  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   */
  struct RawImage  {
    typedef unsigned char byte_t;
    long refCount;
    long map_len;
    Elf64_Ehdr* header;
    const char* sectionNames;
    const char* dynstrNames;
    const char* strtabNames;
    Elf64_Word  sh_length;
    Elf64_Word  pgm_memLength;
    Elf64_Word  pgm_fileLength;
    std::string name;
    std::vector<std::pair<int,Elf64_Shdr*> >  symbolSections;
    RawImage();
    ~RawImage();
    long release();
    long addRef()  { return ++refCount; }
  };

  /// Class to access entities from the ELF image header structure
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   */
  class Header  {
  public:
    typedef RawImage object_t;
    typedef unsigned char byte_t;
  protected:
    object_t* m_ptr;
  public:
    /// Default constructor
    Header() : m_ptr(0) {}
    /// Initializing constructor
    Header(RawImage* img);
    /// Copy constructor
    Header(const Header& copy);
    /// Default destructor
    virtual ~Header();

    /// Assignment operator
    Header& operator=(const Header& copy);
    /// Equivalence operator
    bool operator==(const Header& test) const      {  return m_ptr == test.m_ptr;     }
    /// Access to the pointer
    Elf64_Ehdr* ptr() const                        {  return m_ptr->header;           }
    /// Access to the underlying object
    Elf64_Ehdr* operator->() const                 {  return m_ptr->header;           }

    /// Access to the raw image pointer
    RawImage* image() const                        {   return m_ptr;                  }
    /// IsValid the identification words 
    static bool isValid(const void* ptr);
    /// IsValid the identification words 
    bool isValid()  const;
    /// Image start as bytes
    byte_t* start() const                  {  return (byte_t*)m_ptr->header;          }
    /// ELF identification words
    const byte_t* identification()  const  {  return m_ptr->header->e_ident;          }
    /// ELF object type
    Elf64_Half type()  const               {  return m_ptr->header->e_type;           }
    /// Image type as string
    const char* typeStr() const;
    /// ELF Machine type
    Elf64_Half machine()  const            {  return m_ptr->header->e_machine;        }
    /// ELF image version
    Elf64_Word version()  const            {  return m_ptr->header->e_version;        }
    /// ELF Image flags
    Elf64_Word flags()  const              {  return m_ptr->header->e_flags;          }
    /// ELF Section name as index in string table
    Elf64_Half nameIndex()  const          {  return m_ptr->header->e_shstrndx;       }
    /// String table with section names
    const char* sectionNames()  const      {  return m_ptr->sectionNames;             }
    /// String table with symbol names
    const char* dynstrNames()  const       {  return m_ptr->dynstrNames;              }
    /// String table with symbol names
    const char* strtabNames()  const       {  return m_ptr->strtabNames;              }

    /// Number of entries in the section header table
    Elf64_Half shNumber() const            {  return m_ptr->header->e_shnum;          }
    /// Section header table file offset
    Elf64_Off  shOffset()  const           {  return m_ptr->header->e_shoff;          }
    /// Section header table entry size
    Elf64_Half shEntrySize() const         {  return m_ptr->header->e_shentsize;      }
    /// Aggregated length of all section blocks (headers excluded)
    Elf64_Word shLength() const            {  return m_ptr->sh_length;                }
    /// The beginning of the section header blocks
    const byte_t* sectionHeaders()  const  {  return start() + shOffset();            }
    /// Access to the section headers one by one
    std::pair<RawImage*,Elf64_Shdr*> sectionHeader(int which) const  {
      const byte_t* p = sectionHeaders() + which*shEntrySize();
      return std::make_pair(m_ptr,(Elf64_Shdr*)p);
    }

    /// Number of entries in the program header table
    Elf64_Half pgmNumber() const           {  return m_ptr->header->e_phnum;          }
    /// Program header table file offset
    Elf64_Off  pgmOffset()  const          {  return m_ptr->header->e_phoff;          }
    /// Program header table entry size
    Elf64_Half pgmEntrySize() const        {  return m_ptr->header->e_phentsize;      }
    /// Aggregated length of all program sections when mapped to memory
    Elf64_Word pgmMemLength() const        {  return m_ptr->pgm_memLength;            }
    /// Aggregated length of all program sections in the file
    Elf64_Word pgmFileLength() const       {  return m_ptr->pgm_fileLength;           }
    /// The beginning of the section header blocks
    const byte_t* programHeaders()  const  {  return start() + pgmOffset();           }
    /// Access to the program headers one by one
    std::pair<RawImage*,Elf64_Phdr*> programHeader(int which) const  {
      const byte_t* p = programHeaders() + which*pgmEntrySize();
      return std::make_pair(m_ptr,(Elf64_Phdr*)p);
    }

    /// Print ELF image header
    void print(const char* prefix="")  const;
    /// Test the image memory up to the end of the section headers
    void testSectionHeaderMemory()  const;
  };

  /// Class to access entities from an ELF section header structure
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   */
  class Section : public Handle<Elf64_Shdr>   {
  protected:
    Header header;
  private:
    /// Default constructor is inhibited!
    Section() : Handle(0), header(0)  {}
  public:
    typedef Header::byte_t byte_t;
    
    /// Initializing constructor
    Section(const Header& hdr,int num_section);
    /// Copy constructor
    Section(const Section& s);
    /// Initializing constructor
    Section(const std::pair<RawImage*,Elf64_Shdr*>& h);
    /// Assignment operator
    Section& operator=(const Section& s);
    /// ELF object type
    Elf64_Word type()  const               {  return m_ptr->sh_type;          }
    /// Image type as string
    const char* typeStr() const;
    /// ELF Section name as index in string table
    Elf64_Word nameIndex()  const          {  return m_ptr->sh_name;          }
    /// Name of the section as string from the string table
    const char* name()  const;
    /// ELF Image flags
    Elf64_Xword flags()  const             {  return m_ptr->sh_flags;         }
    /// ELF Virtual address in memory
    Elf64_Word address()  const            {  return m_ptr->sh_addr;          }
    /// ELF Section offset in file
    Elf64_Off offset()  const              {  return m_ptr->sh_offset;        }
    /// ELF Section size
    Elf64_Xword size()  const              {  return m_ptr->sh_size;          }
    /// ELF Sizeof entries, if section has table
    Elf64_Xword entrySize()  const         {  return m_ptr->sh_entsize;       }
    /// ELF Link to another section 
    Elf64_Word link()  const               {  return m_ptr->sh_link;          }
    /// ELF Section alignment
    Elf64_Xword align()  const             {  return m_ptr->sh_addralign;     }

    /// Print section information
    void print(const char* prefix="") const;
    /// Test section data for accessibility
    void testSectionDataMemory();
    /// ELF data of the section      
    byte_t* dataBegin()  const             {  return header.start()+offset(); }
    /// ELF data of the section      
    byte_t* dataEnd()  const               {  return dataBegin() + size();    }
  };

  /// Class to access entities from an ELF section header structure
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   */
  class Symbol : public Handle<Elf64_Sym>   {
  protected:
    Header header;
    Elf64_Shdr* section;
  private:
    /// Default constructor is inhibited!
    Symbol() : Handle(0), header(0)  {}
  public:
    typedef Header::byte_t byte_t;
    
    /// Initializing constructor
    Symbol(const Header& hdr, Elf64_Shdr* sec, Elf64_Sym* sym);
    /// Copy constructor
    Symbol(const Symbol& s);
    /// Initializing constructor
    //Symbol(const std::pair<RawImage*,Elf64_Sym*>& h);    
    /// Assignment operator
    Symbol& operator=(const Symbol& s);
    /// Equality operator
    bool operator==(const Symbol& symbol)  const;
    /// ELF Symbol type and binding
    byte_t info()  const               {  return m_ptr->st_info;                }
    /// ELF Symbol visibility
    byte_t visibility()  const         {  return m_ptr->st_other;               }
    /// ELF Symbol type 
    byte_t type()  const               {  return ELF64_ST_TYPE(m_ptr->st_info); }
    /// Symbol type in string form
    const char* typeStr() const;
    /// ELF Symbol binding 
    byte_t binding()  const            {  return ELF64_ST_BIND(m_ptr->st_info); }
    /// Symbol binding in string form
    const char* bindingStr() const;
    /// ELF Symbol name as index in string table
    Elf64_Word nameIndex()  const      {  return m_ptr->st_name;                }
    /// Name of the symbol as string from the string table
    const char* name()  const;
    /// Name of the symbol as string after demangling
    std::string realname()  const;
    /// ELF Symbol size
    Elf64_Xword size()  const          {  return m_ptr->st_size;                }
    /// ELF Symbol value
    Elf64_Addr  value()  const         {  return m_ptr->st_value;               }
    /// ELF Symbol section header index
    Elf64_Section codeSectionIndex() const {  return m_ptr->st_shndx;           }
    /// ELF Symbol section index name
    const char* codeSectionIndexType() const;

    /** Derived symbol calls  */
    /// ELF Symbol Section header
    std::pair<RawImage*,Elf64_Shdr*> codeSectionHeader()  const;
    /// Access the data section with the symbol code
    Section codeSection()  const;
    /// Print section information
    void print(const char* prefix="") const;
    /// ELF data of the section      
    std::pair<byte_t*,Elf64_Xword> data()  const;
    /// Access to the function pointer
    byte_t* function()  const;
  };

  /// Class to access entities from an ELF symbol section or dynamic symbol section
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   */
  class SymbolTable : public Section  {
  public:

    /// Iterator class over ELF symbol entries in a section
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     */
    struct iterator  {
      RawImage*  image;
      Elf64_Shdr* section;
      Elf64_Sym* symbol;
      
      /// Initializing constructor
      iterator(RawImage* hdr, Elf64_Shdr* sec, Elf64_Sym* start) : image(hdr), section(sec), symbol(start) {}
      /// Copy constructor
      iterator(const iterator& iter) : image(iter.image), section(iter.section), symbol(iter.symbol) {}
      /// Deferencing operation
      Symbol operator*() const;
      /// Deferencing operation
      Symbol operator->() const;
      /// Equality operator
      bool operator==(const iterator& iter) const {
	return symbol == iter.symbol && section == iter.section && image == iter.image;
      }
      /// Unequivality operator
      bool operator!=(const iterator& iter) const {
	return !(symbol == iter.symbol && section == iter.section && image == iter.image);
      }
      /// Iterator equivalence
      iterator& operator=(const iterator iter)  {
	image  = iter.image;
	section = iter.section;
	symbol = iter.symbol;
	return *this;
      }
    };
    /// Const and non-const iterators are the same here...
    typedef iterator const_iterator;

    /// Initializing constructor
    SymbolTable(const std::pair<RawImage*,Elf64_Shdr*>& h);
    /// Elf Symbol Iteration start
    iterator begin() const;
    /// Elf Symbol Iteration end
    iterator end() const;
    /// Elf Symbol Iteration advance
    iterator next(const iterator& curr)  const;
  };


  /// Class to access entities from an ELF string section or dynamic string section
  /**
   *
   *  \author  M.Frank
   *  \version 1.0
   */
  class StringTable : public Section  {
  public:
    /// Iterator class over ELF string table entries in a section
    /**
     *
     *  \author  M.Frank
     *  \version 1.0
     */
    struct iterator  {
      RawImage*  image;
      char*      string;
      /// Initializing constructor
      iterator(RawImage* hdr, char* start) : image(hdr), string(start) {}
      /// Copy constructor
      iterator(const iterator& iter) : image(iter.image), string(iter.string) {}
      /// Deferencing operation
      const char* operator*() const;
      /// Deferencing operation
      const char* operator->() const;
      /// Equality operator
      bool operator==(const iterator& iter) const {
	return string == iter.string && image == iter.image;
      }
      /// Unequivality operator
      bool operator!=(const iterator& iter) const {
	return !(string == iter.string && image == iter.image);
      }
      /// Iterator equivalence
      iterator& operator=(const iterator iter)  {
	image = iter.image;
	string = iter.string;
	return *this;
      }
    };
    /// Const and non-const iterators are the same here...
    typedef iterator const_iterator;

    /// Initializing constructor
    StringTable(const std::pair<RawImage*,Elf64_Shdr*>& h);
    /// Elf String Iteration start
    iterator begin() const;
    /// Elf String Iteration end
    iterator end() const;
    /// Elf String Iteration advance
    iterator next(const iterator& curr)  const;
  };

  class Image : public Header {
  public:
    /// Default constructor
    Image() : Header() {}
    /// Initializing constructor
    Image(RawImage* img) : Header(img)  {}
    /// Copy constructor
    Image(const Header& copy) : Header(copy)  {}
    /// Copy constructor
    Image(const Image& copy) : Header(copy)  {}
    /// Default destructor
    virtual ~Image();

    /// Get header from image buffer
    static Image fromImage(const void* image);
    /// Get header from memory mapped file
    static Image mapImage(const char* name);
    /// Default constructor
    void setImage(const void* image);

    /// Name of image from header structure
    std::string name(const char* fmt="%s")  const;
    /// Name of image from header structure
    static std::string name(const Header& header, const char* fmt="%s");

    /// Print Elf image information
    void printAll()  const;
    /// Print Elf image information
    static void printAll(const Header& header);

    /// Print Elf image header
    void printHeader()  const;
    /// Print Elf image header
    static void printHeader(const Header& header);

    /// Print Elf section headers
    void printSections()  const;
    /// Print Elf section headers
    static void printSections(const Header& header);

    /// Print Elf section headers
    void printProgramHeaders()  const;
    /// Print Elf section headers
    static void printProgramHeaders(const Header& header);
    
    /// Print Elf string tables of the image
    void printStringTables()  const;
    /// Print Elf string tables of the image
    static void printStringTables(const Header& header);

    /// Print Elf symbols in this image
    void printSymbolTables()  const;
    /// Print Elf symbols in this image
    static void printSymbolTables(const Header& header);
  };

  /// Output styles
  enum _OutputStyle  {
    ELF_EXTENDED        = 1<<0,
    ELF_COMPACT         = 1<<1,
    ELF_DEMANGLE        = 1<<2,

    ELF_SYMBOL_TABLES   = 1<<28,
    ELF_STRING_TABLES   = 1<<29,
    ELF_SECTION_HEADERS = 1<<30,
    ELF_PROGRAM_HEADERS = 1<<31
  };
  /// Modify output style
  unsigned long elf_set_output_style(unsigned long flags);
  /// Retrieve string representation of image header type
  const char* elf_image_header_type(int typ);
  /// Retrieve string representation of section header type
  const char* elf_section_header_type(int typ);
  /// Retrieve string representation of symbol type
  const char* elf_symbol_type(int typ);
  /// Retrieve string representation of symbol binding
  const char* elf_symbol_bind(int typ);
  /// Retrieve string representation of symbol index type
  const char* elf_symbol_index_type(int typ);


}  // End namespace ELF
#endif /* ONLINEBASE_CPP_ELF_H  */
#endif /* _WIN32  */
