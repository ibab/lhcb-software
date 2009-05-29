#define MBM_IMPLEMENTATION
#include <ctime>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <vector>
#include "MBM/bmstruct.h"
#include "bm_internals.h"
#include "RTL/bits.h"
#include "RTL/ConsoleDisplay.h"

namespace {
  void help()  {
    ::printf("mep_dump_bitmap -opt [-opt]\n");
    ::printf("    -b=<name>      Buffer identifier \n");
    ::printf("    -c(continuous) Continues display\n");
  }
}
namespace MBM {
  class BitmapDump : public RTL::ConsoleDisplay {
    std::vector<BMID> m_bm;
  public:
    BitmapDump(const char* bm_name) : ConsoleDisplay(), m_bm(0) {
      if ( bm_name ) {
        addBuffer(bm_name);
      }
    }
    ~BitmapDump()  {
      if ( !m_bm.empty() )  {
        for(std::vector<BMID>::iterator i=m_bm.begin(); i != m_bm.end(); ++i)  {
          mbm_unmap_memory(*i);
        }
      }
    }
    void addBuffer(const char* name)  {
      BMID id = mbm_map_memory(name);
      if ( id ) m_bm.push_back(id);
      else  printf("Failed to map to buffer %s:\n", name);
    }
    int id() const  {  return m_bm.size(); }
    void put_info()  {
      int cnt = 1;
      std::vector<std::string> words;
      std::string s;
      for(std::vector<BMID>::iterator j=m_bm.begin(); j != m_bm.end(); ++j)  {
        BMID id = *j;
        Bits::dumpWords(id->bitmap, id->bitmap_size>>3, words);
        draw_line(REVERSE,  "                          %s Buffer Bitmap Monitor [%s]  pid:%d", 
          id->bm_name, ::lib_rtl_timestr("%a %d %b %Y  %H:%M:%S",0), lib_rtl_pid());
        if ( m_bm.size() == 1 )  {
          draw_line();
          draw_line(NORMAL,"");
        }
        s = " ";
        bool compress = words.size()/4 > 24;
        int line_mod = compress ? 32 : 4;
        for(std::vector<std::string>::iterator i=words.begin(); i < words.end(); ++i, cnt++)  {
          if ( compress )  {
            const char* c = (*i).c_str();
            s += strncmp(c,   "00000000",8) ==0 ? '.' : 'X';
            s += strncmp(c+8, "00000000",8) ==0 ? '.' : 'X';
            s += strncmp(c+16,"00000000",8) ==0 ? '.' : 'X';
            s += strncmp(c+24,"00000000",8) ==0 ? '.' : 'X';
          }
          else {
            s += *i;
          }
          if ( (cnt%line_mod)==0 )  {
            draw_line(NORMAL, s.c_str());
            s = " ";
          }
        }
        draw_line(NORMAL, s.c_str());
        if ( !s.empty() ) draw_line(NORMAL, "");
      }
    }
  };
}
extern "C" int mbm_dump_bitmap(int argc,char ** argv) {
  RTL::CLI cli(argc, argv, help);
  std::string buffer="0";
  cli.getopt("buffer",1,buffer);
  bool continuous = cli.getopt("continuous",1) != 0;
  MBM::BitmapDump dmp(buffer.c_str());
  if ( dmp.id()==1 )  {
    dmp.setContinuous(continuous);
    dmp.run(2000);
  }
  else  {
    printf("Failed to map to buffer %s:\n", buffer.c_str());
    return 0;
  }
  return 1;
}

extern "C" int mep_dump_bitmap(int argc,char ** argv) {
  RTL::CLI cli(argc, argv, help);
  std::string b1="MEP", b2="", b3="";
  cli.getopt("b1",2,b1);
  cli.getopt("b2",2,b2);
  cli.getopt("b3",2,b3);
  bool continuous = cli.getopt("continuous",1) != 0;
  MBM::BitmapDump dmp(b1.c_str());
  if ( dmp.id()==1 )  {
    if ( !b2.empty() ) dmp.addBuffer(b2.c_str());
    if ( !b3.empty() ) dmp.addBuffer(b3.c_str());
    dmp.setContinuous(continuous);
    dmp.run(2000);
  }
  else  {
    printf("Failed to map to buffer %s:\n", b1.c_str());
    return 0;
  }
  return 1;
}
