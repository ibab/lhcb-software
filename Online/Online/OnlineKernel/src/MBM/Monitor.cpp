/*-----------------------------------------------------------------------*/
/*                                                                       */
/*                       OS-9 BUFFER MANAGER                             */
/*                  INSTALLATION PROGRAM FOR THE AEB                     */
/*                                                                       */
/* Edition History                                                       */
/*                                                                       */
/*  #   Date    Comments                                       By        */
/* -- -------- ----------------------------------------------- ---       */
/*  0  28/09/88  Initial version                               PM        */
/*  1  11/11/88  Released version 1.0                          PM        */
/*  2  29/11/88  Spy and Multibuffer included                  PM        */
/*  3  15/12/88  Released version 2.0                          PM        */
/*  4  21/02/90  Now it can allocate buffer across memories    PM        */
/*-----------------------------------------------------------------------*/
#define MBM_IMPLEMENTATION
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <ctime>
#include "RTL/screen.h"
#include "bm_struct.h"
#include "Manager.h"

#define E_MNF  221

namespace MBM {
  static int USER_next_off;
  static const char *sstat[17] = {" nl", "   ", "*SL","*EV","*SP","WSL","WEV","WSP","wsl","wev","wsp"," ps"," ac", "SPR", "WER", "   "};

  static int cont = 1;
  static int end  = 1;

  struct Monitor {
    struct ManagerImp : public Manager {
      virtual int  optparse (const char*) {
        return 1;
      }
      ManagerImp() {}
      virtual ~ManagerImp() {}
    };
    struct DISP_BMDES   {
      ManagerImp      m_mgr;
      BUFFERS::BUFF*  m_buff;
      DISP_BMDES() : m_buff(0) {}
    };
    DISP_BMDES* m_bms;
    int  m_numBM;
    size_t m_currLine;
    char m_buffID[32];
    char* m_bmid;
    lib_rtl_gbl_t m_bm_all;
    WINDOW* m_window;
    BUFFERS* m_buffers;
    int m_color;

    int monitor();
    int put_inf();
    void setTextcolor(int col)  {
      m_color = col;
      textcolor(col);
    }

    size_t draw_line()  {
      print_char(1, m_currLine, TEE_LEFT);
      for(size_t i=1; i < term_width()-1; ++i)
        print_char(i+1, m_currLine, HORZ_BAR);
      print_char(term_width(), m_currLine, TEE_RIGHT);
      return ++m_currLine;
    }
    size_t draw_bar()  {
      print_char(1, m_currLine, VERT_BAR);
      for(size_t i=1; i < term_width()-1; ++i)
        print_char(i+1, m_currLine, FAT_VERT_BAR);
      print_char(term_width(), m_currLine, VERT_BAR);      
      return ++m_currLine;
    }
    size_t draw_line(int flags, const char* format,...)  {
      va_list args;
      char buffer[1024];
      va_start( args, format );
      size_t len = ::vsprintf(buffer, format, args);
      print_char(1, m_currLine, VERT_BAR);
      for(size_t j=0; j<len && j<term_width()-1; ++j)
        print_char(j+2, m_currLine, flags|buffer[j]);
      for(size_t i=len; i < term_width()-1; ++i)
        print_char(i+2, m_currLine, ' '|flags);
      print_char(term_width(), m_currLine, VERT_BAR);
      return ++m_currLine;
    }
    void begin_update()  {
      ::textcolor(YELLOW);
      m_currLine = 1;
      print_char(1,m_currLine,LEFT_UP_EDGE);
      for(size_t i=1; i < term_width()-1; ++i)
        print_char(i+1,m_currLine,HORZ_BAR);
      print_char(term_width(),m_currLine,RIGHT_UP_EDGE);      
      m_currLine = 2;
    }
    void end_update() {
      print_char(1,m_currLine,LEFT_LOW_EDGE);
      for(size_t i=1; i < term_width()-1; ++i)
        print_char(i+1,m_currLine,HORZ_BAR);
      print_char(term_width(),m_currLine,RIGHT_LOW_EDGE);      
      m_currLine++;
      refresh();
    }

    size_t draw_buffer(const char* name, CONTROL* ctrl);

    void getOptions(int argc, char** argv);
    virtual int optparse (const char* c);
    int get_bm_list();
    int drop_bm_list();
    size_t draw_bar(float ratio,int full_scale);
    Monitor(int argc , char** argv) : m_window(0)  {
      m_bmid = 0;
      getOptions(argc, argv);
      m_color = YELLOW;
    }
  };
}

void MBM::Monitor::getOptions(int argc, char** argv)    {
  while (--argc > 0) {                                  /* process options  */
    const char* cptr = *++argv;
    if ( *cptr == '-' || *cptr == '/' )
      optparse (cptr+1);
  }
}
size_t MBM::Monitor::draw_bar(float ratio,int full_scale)    {
  int barlen  =  int(0.5+ratio*float(full_scale));
  graphics();
  printf("\164");
  for (int j=barlen;j<full_scale;j++) ::printf("\161");
  ::printf("\165");
  ascii();
  return 0;
}

int MBM::Monitor::monitor() {
  byte_offset(USER,next,USER_next_off);

  //signal (SIGINT,handler);
  //signal (SIGABRT,handler);
  int status = mbm_map_global_buffer_info(&m_bm_all);
  if(!lib_rtl_is_success(status))   {   
    printf("Cannot map global buffer information....\n");
    exit(status);
  }
  m_buffers = (BUFFERS*)m_bm_all->address;
  m_bms = new DISP_BMDES[m_buffers->p_bmax];

  m_window = initscreen();
  _setcursortype(_NOCURSOR);      // hide the cursor
  textcolor(YELLOW);              // change textcolor to YELLOW
  textbackground(BLUE);           // change backgroundcolor to BLUE
  if( cont )    {
    while( 0 != end )    {
      try {
        get_bm_list();    
        begin_update();
        put_inf();
        end_update();
        drop_bm_list();
      }
      catch(...) {
        draw_line(NORMAL," Exception during buffer monitoring.");
      }
      lib_rtl_sleep(1000);
    }      
  }
  else  {
    put_inf();
  }
  _setcursortype(_NORMALCURSOR);
  return 1;
}

size_t MBM::Monitor::draw_buffer(const char* name, CONTROL* ctr)  {
  int i, m;
  char txt[256];
  sprintf(txt," Buffer \"%s\"",name);
  draw_line(NORMAL,"%-26s  Events: Produced:%d Actual:%d Seen:%d Pending:%d Max:%d",
    txt, ctr->tot_produced, ctr->tot_actual, ctr->tot_seen, ctr->i_events, ctr->p_emax);
  draw_line(NORMAL,"%-26s  Space(kB):[Tot:%d Free:%d] Users:[Tot:%d Max:%d]",
    "",ctr->bm_size/2, ctr->i_space/2, ctr->i_users, ctr->p_umax);

  draw_line(NORMAL,"  Occupancy [Events]:");
  float f1=float(ctr->i_events)/float(ctr->p_emax);
  for (i=0, m=term_width()-30; i<m; ++i)  {
    print_char(i+30,m_currLine-1,f1*m > i ? FAT_VERT_BAR : DIM_VERT_BAR);
  }
  draw_line(NORMAL,"            [Space]: ");
  f1 = float(ctr->bm_size-ctr->i_space)/float(ctr->bm_size);
  for (i=0, m=term_width()-30; i<m; ++i)  {
    print_char(i+30,m_currLine-1,f1*m > i ? FAT_VERT_BAR : DIM_VERT_BAR);
  }
  return 1;
}

int MBM::Monitor::put_inf()   {
  int i, j, k;
  time_t nowt;
  const char* head=" Name      Partition     Pid Type State   Produced %%prod     #seen     #freed %%seen Reqs Buffer    UTime  STime";
  char line[256], tim[64];
  ::time(&nowt);
  struct tm *now = ::localtime(&nowt);
  ::strftime(tim,sizeof(tim),"%a %d %b %Y  %H:%M:%S",now);
  draw_line();
  draw_line(REVERSE,  "                               Buffer Manager Monitor [%s]  pid:%d", tim, lib_rtl_pid());
  draw_line();
  draw_line(NORMAL,"");
  for (i=0;i<m_buffers->p_bmax;i++)  {
    if ( m_bms[i].m_buff != 0 )  {
      BMDESCRIPT* dsc = m_bms[i].m_mgr.m_bm;
      draw_buffer(dsc->bm_name, dsc->ctrl);
      draw_line(NORMAL,"");
    }
  }
  draw_line();
  draw_line(NORMAL,m_numBM<=0 ? "               No active buffers present" : head);
  draw_line();

  for (i=0;i<m_buffers->p_bmax;i++)  {
    if ( m_bms[i].m_buff != 0 )  {
      USER *us, *utst=(USER*)~0x0;
      BMDESCRIPT* dsc = m_bms[i].m_mgr.m_bm;
      CONTROL*    ctr = dsc->ctrl;
      for (j=0,us=dsc->user;j<ctr->p_umax;j++,us++)    {
        if ( us == utst || us == 0         ) break;
        if ( us->block_id != int(BID_USER) ) continue;
        if ( us->busy     == 0             ) continue;
        char spy_val[5] = {' ',' ',' ',' ',0};
        for (k=0; k<us->n_req; ++k )  {
          if      ( us->req[k].user_type == BM_REQ_ONE  ) spy_val[1] = '1';
          else if ( us->req[k].user_type == BM_REQ_USER ) spy_val[2] = 'U';
          else if ( us->req[k].user_type == BM_REQ_ALL  ) spy_val[3] = 'A';
          else if ( us->req[k].freq < 100.0             ) spy_val[0] = 'S';
        }
        if ( us->ev_produced>0 || us->get_sp_calls>0 )   {
          float perc = ((float)us->ev_produced/(float)ctr->tot_produced)*100;
          sprintf(line," %-15s%4x%8d%5s%6s%11d   %3.0f%32s%7s  %7.1e %7.1e %d %d",
            us->name,us->partid,us->pid,"P",sstat[us->p_state+1],us->ev_produced,
            perc+0.1, spy_val, dsc->bm_name, us->utime, us->stime, us->free_calls, us->alloc_calls);    
        }
        else if ( us->ev_actual>0 || us->get_ev_calls>0 || us->n_req>0 ) {
          float perc = ((float)us->ev_seen/(float)us->ev_actual)*100;
          sprintf(line," %-15s%4x%8d%5s%6s               %12d%11d   %3.0f%5s%7s  %7.1e %7.1e %d %d",
            us->name,us->partid,us->pid,"C",sstat[us->c_state+1],
            us->ev_seen, us->ev_freed, perc+0.1, spy_val, dsc->bm_name, us->utime, us->stime, us->free_calls, us->alloc_calls);
        }
        else        {
          sprintf(line," %-15s%4x%8d%5s          %40s%5s%7s",us->name,us->partid,us->pid,"?","",
            spy_val, dsc->bm_name);    
        }
        draw_line(NORMAL,line);
      }
    }
  }
  while(m_currLine<term_height()-1) 
    draw_line(NORMAL,"");
  return 1;
}

int MBM::Monitor::optparse (const char* c)  {
  int iret;
  switch (*c | 0x20)  {
  case 's':        /*      Single Update*/  
    cont = 0;
    break;
  case 'i':        /*      buffer_id        */  
    iret = sscanf(c+1,"=%s",m_buffID);
    if( iret != 1 )      {
      lib_rtl_printf("Error reading Buffer identifier parameter\n");
      exit(0);
    }
    m_bmid = m_buffID;
    break;
  case '?':
  case 'h':
  default:
    lib_rtl_printf("mbm_mon - Buffer Manager Monitor\n");
    lib_rtl_printf("Options:\n");
    lib_rtl_printf("    -i=<bm_name>   Select Buffer Identifier\n");
    lib_rtl_printf("    -s             Single update \n");
    exit(0);
  }
  return 1;
}

int MBM::Monitor::get_bm_list()   {
  m_numBM = 0;
  for (int i = 0; i < m_buffers->p_bmax; ++i)  {
    if ( m_buffers->buffers[i].used == 1 )  {
      if ( m_bmid != 0 && strcmp(m_bmid,m_buffers->buffers[i].name) != 0 )  {
        continue;
      }
      if ( m_bms[i].m_buff == 0 )  {
        m_bms[i].m_mgr.setup(m_buffers->buffers[i].name);
        int sc = m_bms[i].m_mgr.mapSections();
        if ( !lib_rtl_is_success(sc) ) exit(sc);
        m_bms[i].m_buff = &m_buffers->buffers[i];
      }
      m_numBM++;
    }
    else if ( m_bms[i].m_buff != 0 )  {
      m_bms[i].m_mgr.unmapSections();
      m_bms[i].m_buff = 0;
    }
  }
  return 1;
}

int MBM::Monitor::drop_bm_list()   {
  m_numBM = 0;
  for (int i = 0; i < m_buffers->p_bmax; ++i)  {
    if ( m_buffers->buffers[i].used != 1 && m_bms[i].m_buff != 0 )  {
      m_bms[i].m_mgr.unmapSections();
      m_bms[i].m_buff = 0;
    }
  }
  return 1;
}

int mbm_mon(int argc , char** argv) {
  MBM::Monitor mon(argc, argv);
  return mon.monitor();
}

extern "C" int mbm_ascii(int /* argc */, char** /* argv */) {
  for ( unsigned char i=0; i < 255; ++i)  {
    printf("%3d  %03X   \"%c\"\n", i,i,i);
  }
  return 1;
}
