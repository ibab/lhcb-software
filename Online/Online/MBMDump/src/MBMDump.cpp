#include "MBMDump/MBMDump.h"
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <cstring>
#include <stdexcept>

#include "UPI/upidef.h"
#include "UPI/upi_proto.h"
#include "RTL/rtl.h"
#include <map>
#include "winsock.h"
#include "NET/IPHeader.h"
#include "MBM/mepdef.h"
#include "MDF/MEPEvent.h"
#include "MDF/RawEventPrintout.h"
using namespace LHCb;

/* Buffer id for mbm */
#define TOPTYP    10

/* used by bank_list() */
#define MAKE 0
#define GIME1 1
#define GIME2 2
#define UPDATE 3

/* used by functions operating on the display */

/* format information */

namespace {

  int page(int menu_number);
  int bank_list(int cmd, int n, unsigned int nam[4], struct DataBlock *bank);
  int conv(char* c, unsigned int word[4]);   /* convert int to char string */
  int rconv(char c[], int word);  /* ...reversed                */
  unsigned int* invconv(const char *c);           /* and inverse                */ 


  /* Which do i want conv or rconv ?? */
  int conv(char* c, unsigned int word[4])  {
    for(size_t j=0;j<4;j++) {
      for(size_t i=0;i<4;i++) {
        c[i] = (word[j]>>(i*8) & 0XFF);
        if(!isalnum(c[i]))c[i] = '.';
      }
    }
    return 1;
  }


  // takes the first four characters of c and converts then to
  // an integer, i, s.t. conv(c,i) returns c  ... obvious! no?
  unsigned int* invconv(const char *c)    {
    static unsigned int word[4];
    memset(word,0,sizeof(word));
    for(size_t j=0;j<4;j++){
      for(size_t i=0;i<4;i++){
        word[j] = word[j]<<8;
        word[j] += *(c+3-i);
      }
    }
    return word;
  }

}

int page(int m)   {
  static int p[10] = {0,0,0,0,0,0,0,0,0,0};
  if(m >= 0){
    int i;
    for(i=0; p[i] != m && p[i] != 0; i++)
      ;
    if(p[i] == 0)
      p[i] = m;
    return i;
  }
  else{    /* Horrible bodge...if m < 0 then it is a page number     */
    /* the menu number corresponding to this page is returned */
    return p[-m-1];
  }
}




#include "RTL/rtl.h"

const char* MBMDump::procName()  {  
  return RTL::processName().c_str();    
}

void MBMDump::rconv(char c[], int word)   {
  for(int i=0;i<4;i++){
    c[3-i] = (word>>(i*8) & 0XFF);
    if(!isalnum(c[3-i]))c[3-i] = '.';
  }
}

static void help()  {
    ::lib_rtl_printf("mbmdump -opt [-opt]\n");
    ::lib_rtl_printf("    -d(ebug)               Invoke debugger.\n");
}

static size_t print_msg(void* env, const char* fmt, va_list args)  {
  char buff[1024];
  size_t result = vsprintf(buff,fmt,args);
  ::upic_write_message(buff,"");
  return result;
}

extern "C" int mbmdump(int argc,char *argv[])   {
  using namespace MBMDump;
  RTL::CLI cli(argc, argv, help);
  bool debug = cli.getopt("debug",1) != 0;
  if ( debug ) lib_rtl_start_debugger();
  int status = ::upic_attach_terminal();
  if(status != UPI_NORMAL)
    exit(EXIT_FAILURE);

  ::lib_rtl_install_printer(print_msg,0);
  ::upic_change_pasteboard(132,0);

  MainMenu m;
  m.buildMenu();
  return m.run();
}
