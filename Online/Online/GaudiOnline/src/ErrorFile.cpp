// C++ header files
#include <string>

namespace LHCb {

  class ErrorFile {
    std::string m_name;
    char*       m_memory;
    size_t      m_length;
    int         m_badBlock;
    int         m_badVsn;
    int         m_badHdr;
    int         m_numEvt;
    int         m_numBank;
    int         m_print;
  public:
    enum { PRINT_ERRORS = 1, PRINT_MDF_HEADERS = 2, PRINT_BANKS = 4 };
    ErrorFile(int prt, const std::string& nam);
    virtual ~ErrorFile();
    int read();
    int analyze();
    int summarize();
    void printEvt();
  };
}

#include <iostream>
#include <iomanip>
#include "RTL/rtl.h"


#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#ifdef _WIN32
#else
#include <unistd.h>
static const int O_BINARY = 0;
#endif

#include "MDF/MDFHeader.h"
#include "MDF/RawEventPrintout.h"
#include "Event/RawBank.h"

static const char* line="=====================================================";

using namespace std;
using namespace LHCb;

ErrorFile::ErrorFile(int prt, const string& nam) 
  : m_name(nam), m_memory(0), m_badBlock(0), m_badVsn(0), m_badHdr(0), m_numEvt(0), m_numBank(0), m_print(prt)
{
}


ErrorFile::~ErrorFile() {
  if ( m_memory ) ::free(m_memory);
  m_memory = 0;
}

void ErrorFile::printEvt() {
  cout << "Event:" << dec << setw(10) << left << m_numEvt << " ";
}

void printMDFHeader(int evt, MDFHeader* h) {
  cout << line << line << endl;
  cout << "MDFHeader [" << evt << "]: size" 
       << " [0]:" << left << dec << setw(10) << h->size0() 
       << " [1]:" << left << dec << setw(10) << h->size1() 
       << " [2]:" << left << dec << setw(10) << h->size2() 
       << endl
       << "       Checksum:" << left << hex << setw(10) << h->checkSum()  
       << "    Compression:" << left << hex << setw(10) << (int)h->compression()
       << endl
       << "        Version:" << left << hex << setw(10) << (int)h->headerVersion() 
       << "      Sublength:" << left << dec << setw(10) << (int)h->subheaderLength()
       << endl
       << "        HdrSize:" << left << dec << setw(10) << h->sizeOf(h->headerVersion()) 
       << "      Data type:" << left << dec << setw(10) << (int)h->dataType() 
       << endl;
}

int ErrorFile::analyze() {
  int num_jump = 0;
  int num_bank = 0;
  RawBank* last_bank = 0;
  MDFHeader* last_hdr = 0;
  const char* p = m_memory, *q = p;
  
  for(size_t i=0; i<1000 && q<p+m_length; ++i ) {
    const unsigned short* sp=(const unsigned short*)q;
    const unsigned int*   ip=(const unsigned int*)q;
    if ( *sp == 0xcbcb ) {
      RawBank* b = (RawBank*)sp;
      if ( num_jump > 0 ) {
	printEvt();
	cout << "!!!!!! Detected unreadable data of size " << num_jump*2 << " Bytes." << endl;
	num_jump = 0;
	++m_badBlock;
      }
      if ( m_print >= PRINT_BANKS ) {
	cout << "RawBank at offset:" << dec << left << setw(10) << size_t(q-p) << " "
	     << RawEventPrintout::bankHeader(b) << endl;
      }
      last_bank = b;
      q += b->totalSize();
      ++m_numBank;
      ++num_bank;
      continue;
    }
    if ( ip[0] == ip[1] && ip[0] == ip[2] ) {
      // Looks like we got an MDF header
      MDFHeader* hdr = (MDFHeader*)q;
      if ( num_jump > 0 ) {
	printEvt();
	cout << "!!!!!! Detected unreadable data of size " << num_jump*2 << " Bytes." << endl;
	num_jump = 0;
	++m_badBlock;
      }
      if ( last_hdr ) {
	const char* a = (const char*)last_hdr;
	size_t diff = q-a;
	cout << "=== Number of banks for event:" << m_numEvt << ": " << num_bank << endl;
	num_bank = 0;
	if ( last_hdr->size0() != diff ) {
	  printEvt();
	  cout << "!!!!!! Inconsistent MDF Header at " 
	       << dec << (size_t)(((char*)last_hdr)-((char*)m_memory))
	       << ": Event size of event " << m_numEvt << " is " 
	       << diff << " Bytes not " << last_hdr->size0() << " Bytes."
	       << endl;
	  ++m_badHdr;
	}
      }
      ++m_numEvt;
      if ( m_print >= PRINT_MDF_HEADERS ) printMDFHeader(m_numEvt,hdr);
      last_hdr = hdr;
      if ( hdr->headerVersion() == 3 ) {
	q += hdr->sizeOf(hdr->headerVersion());
	continue;
      }
      printEvt();
      cout << "!!!!!! Inconsistent MDF header version" << endl;
      ++m_badVsn;
    }
    ++num_jump;
    q += 2;
  }
  if ( num_jump > 0 ) {
    cout << "!!!!!! Detected unreadable data of size " << num_jump*2 << " Bytes." << endl;
    num_jump = 0;
    ++m_badBlock;
  }
  cout << "=== Number of banks for event:" << m_numEvt << ": " << num_bank << endl;
  return 1;
}

int ErrorFile::read() {
  struct stat buf;
  int ret = ::stat(m_name.c_str(),&buf);
  if ( 0 == ret ) {
    if ( m_memory ) ::free(m_memory);
    m_memory = (char*)::malloc(buf.st_size);
    int fd = ::open64(m_name.c_str(),O_RDONLY|O_BINARY);
    if ( fd > 0 ) {
      off_t tmp = 0;
      char* p = m_memory;
      while ( tmp < buf.st_size )  {
	int sc = ::read(fd,p+tmp,buf.st_size-tmp);
	if ( sc >  0 ) tmp += sc;
	else if ( sc == 0 ) break;
	else                break;
      }
      m_length = tmp;
      ::close(fd);
      return buf.st_size == tmp ? 1 : 0;
    }
  }
  cout << "The file " << m_name << " does not exist." << endl;
  return 0;
}

int ErrorFile::summarize() {
  cout << line << line << endl;
  cout << "=== Summary of file analysis:" << m_name << endl;
  cout << line << line << endl;
  cout << "=== Number of events: " << dec << m_numEvt << endl;
  cout << "=== Number of banks:  " << dec << m_numBank << endl;
  cout << "=== Number of MDF headers with bad size:   " << dec << m_badHdr << endl;
  cout << "=== Number of MDF headers with bad version:" << dec << m_badVsn << endl;
  cout << "=== Number of unreadable data junks:       " << dec << m_badBlock << endl;
  cout << line << line << endl;
  return 1;
}



static void help()  {
  ::lib_rtl_output(LIB_RTL_ALWAYS,"check_errfile -opt [-opt]\n");
  ::lib_rtl_output(LIB_RTL_ALWAYS,"    -name=<name>      file name\n");
}


extern "C" int check_errfile(int argc, char** argv) {
  RTL::CLI cli(argc, argv, help);
  int prt = ErrorFile::PRINT_ERRORS;
  std::string name="";

  cli.getopt("name",1,name);
  if ( cli.getopt("headers",1) ) prt += ErrorFile::PRINT_MDF_HEADERS;
  if ( cli.getopt("banks",1)   ) prt += ErrorFile::PRINT_BANKS;
  ErrorFile f(prt, name);
  if ( f.read() )  {
    f.analyze();
    f.summarize();
  }
  return 1;
}
