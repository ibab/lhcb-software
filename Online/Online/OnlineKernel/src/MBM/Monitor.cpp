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

#define writeln(a,b,c) printf(b)
#define E_MNF  221

#define writeln(a,b,c) printf(b)
#define _CHECK( x )  { int sc = x ; if ( !(sc&1) ) { printf ( "Error in:%s, status=%d\n", #x , sc ); return sc; } }

static int USER_next_off;
static char    *sstat[13] = {" nl", "   ", "*SL","*EV","*SP","WSL","WEV","WSP","wsl","wev","wsp"," ps"," ac"};
// static char    *spy[2]  = {" ","s"};

int cont = 1;
int end  = 1;

namespace MBM {
  struct Monitor : public Manager {
    struct ManagerImp : public Manager {
      virtual int  optparse (const char*) {
        return 1;
      }
      ManagerImp() {}
      virtual ~ManagerImp() {}
    };
    struct DISP_BMDES   {
      ManagerImp m_mgr;
      int m_bm_row;
      int m_stat_row;
    };
    DISP_BMDES bms[10];
    int  nbms;
    size_t m_currLine;

    int monitor();
    int put_inf();
    int m_color;
    WINDOW* m_window;
    void setTextcolor(int col)  {
      m_color = col;
      textcolor(col);
    }

    int draw_line()  {
      print_char(1, m_currLine, TEE_LEFT);
      for(size_t i=1; i < term_width()-1; ++i)
        print_char(i+1, m_currLine, HORZ_BAR);
      print_char(term_width(), m_currLine, TEE_RIGHT);
      return ++m_currLine;
    }
    int draw_bar()  {
      print_char(1, m_currLine, VERT_BAR);
      for(size_t i=1; i < term_width()-1; ++i)
        print_char(i+1, m_currLine, FAT_VERT_BAR);
      print_char(term_width(), m_currLine, VERT_BAR);      
      return ++m_currLine;
    }
    int draw_line(int flags, const char* format,...)  {
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

    int draw_buffer(const char* name, CONTROL* ctrl);

    virtual int  optparse (const char* c);
    int MBM::Monitor::get_bm_list();
    int draw_bar(float ratio,int full_scale);
    Monitor(int argc , char** argv) : m_window(0)  {
      getOptions(argc, argv);
      nbms = 1;
      bms[0].m_mgr.setup("0");
      m_color = YELLOW;
    }
  };
}

int MBM::Monitor::draw_bar(float ratio,int full_scale)    {
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

  get_bm_list();    
  m_window = initscreen();
  _setcursortype(_NOCURSOR);      // hide the cursor
  textcolor(YELLOW);              // change textcolor to YELLOW
  textbackground(BLUE);           // change backgroundcolor to BLUE
  if( cont )    {
    while( end )    {
      begin_update();
      put_inf();
      end_update();
      lib_rtl_sleep(1000);
    }      
  }
  else  {
    put_inf();
  }
  _setcursortype(_NORMALCURSOR);
  return 1;
}

int MBM::Monitor::draw_buffer(const char* name, CONTROL* ctr)  {
  size_t i, m;
  char txt[256];
  sprintf(txt," Buffer \"%s\"",name);
  draw_line(NORMAL,"%-26s  Events: Produced:%d Actual:%d Seen:%d Pending:%d Max:%d",
    txt, ctr->tot_produced, ctr->tot_actual, ctr->tot_seen, ctr->i_events, ctr->p_emax);
  draw_line(NORMAL,"%-26s  Space(kB):[Tot:%d Free:%d] Users:[Tot:%d Max:%d]",
    "",ctr->bm_size, ctr->i_space, ctr->i_users, ctr->p_umax);

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
  int i, j;
  time_t nowt;
  const char* head=" Name    Partition  Pid Type State Produced    #seen seen Buffer";
  char line[256], tim[64];
  ::time(&nowt);
  struct tm *now = ::localtime(&nowt);
  ::strftime(tim,sizeof(tim),"%a %d %b %Y  %H:%M:%S",now);
  draw_line();
  draw_line(REVERSE,  "                               Buffer Manager Monitor [%s]",tim);
  draw_line();
  draw_line(NORMAL,"");
  for (i=0;i<nbms;i++)  {
    BMDESCRIPT* dsc = bms[i].m_mgr.m_bm;
    draw_buffer(dsc->bm_name, dsc->ctrl);
    draw_line(NORMAL,"");
  }
  draw_line();
  draw_line(NORMAL,nbms<=0 ? "               No active buffers present" : head);
  draw_line();

  for (i=0;i<nbms;i++)  {
    USER *us;
    BMDESCRIPT* dsc = bms[i].m_mgr.m_bm;
    for (j=0,us = dsc->user;j<dsc->ctrl->p_umax;j++,us++)    {
      if (us->busy == 0) continue;
      if (us->ev_produced>0)      {
        sprintf(line," %-13s%4x%5X%5s%6s%9d%21s",
          us->name,us->partid,us->pid,"P",sstat[us->p_state+1],us->ev_produced,dsc->bm_name);    
      }
      else if (us->ev_actual>0)        {
        float perc = ((float)us->ev_seen/(float)us->ev_actual)*100;
        sprintf(line," %-13s%4x%5X%5s%6s         %9d  %3.0f%7s",
          us->name,us->partid,us->pid,"C",sstat[us->c_state+1],
          us->ev_seen,perc+0.1,dsc->bm_name);    
      }
      else        {
        sprintf(line," %-13s%4x%5X%5s    %32s",us->name,us->partid,us->pid,"?",dsc->bm_name);    
      }
      draw_line(NORMAL,line);
    }
  }
  while(m_currLine<term_height()-1) 
    draw_line(NORMAL,"");
  return 1;
}

int MBM::Monitor::optparse (const char* c)  {
  char buff_id[32];
  register int iret;
  switch (*c | 0x20)  {
  case 's':        /*      Single Update*/  
    cont = 0;
    break;
  case 'i':        /*      buffer_id        */  
    iret = sscanf(c+1,"=%s",buff_id);
    if( iret != 1 )      {
      writeln(2,"Error reading Buffer identifier parameter\n",80);
      exit(0);
    }
    bm_id = (char*)c+2;
    break;
  case '?':
  case 'h':
  default:
    writeln(2,"bm - Buffer Manager Monitor\n",80);
    writeln(2,"Options:\n",80);
    writeln(2,"    -i=<bm_name>   Select Buffer Identifier\n",80);
    writeln(2,"    -s             Single update \n",80);
    exit(0);
  }
  return 1;
}

int MBM::Monitor::get_bm_list()   {
  for (int i = 0; i < nbms; ++i)  {
    int sc = mapSections(&bms[i].m_mgr);
    if ( !lib_rtl_is_success(sc) ) exit(sc);
    bms[i].m_bm_row  = 4+i*4;
    bms[i].m_stat_row  = bms[i].m_bm_row+2;
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
