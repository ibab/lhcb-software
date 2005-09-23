/*-----------------------------------------------------------------------*/
/*                                                                       */
/*                       OS-9 BUFFER MANAGER  	                         */
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
#include "RTL/conioex.h"
#include "bm_struct.h"
#include "Manager.h"

#ifdef _WIN32
#define strdate _strdate
#define strtime _strtime
#endif

#define writeln(a,b,c) printf(b)
#define E_MNF	221
#define double          printf("\033#6")
#define single          printf("\033#5")
#define	bold		printf("\033[1m")
#define	flashing	printf("\033[5m")
#define	underline	printf("\033[4m")
#define	inverse		printf("\033[7m")
#define	plain		printf("\033[0m")
#define clear_scr	printf("\033[2J")
#define cursor(x,y)	printf("\033[%d;%dH",x,y)
#define scroll(top,bot)	printf("\033[%d;%dr",top,bot)
#define beep		printf("\007")
#define cr		printf("\n")
#define ascii		printf("\033(B")
#define graphics	printf("\033(0")
#define checker printf("\141");

#define writeln(a,b,c) printf(b)
#define _CHECK( x )  { int sc = x ; if ( !(sc&1) ) { printf ( "Error in:%s, status=%d\n", #x , sc ); return sc; } }

static int USER_next_off;
static char    *sstat[13] = {" nl", "   ", "*SL","*EV","*SP","WSL","WEV","WSP","wsl","wev","wsp"," ps"," ac"};
static char    *spy[2]  = {" ","s"};

int cont = 1;
int end  = 1;

namespace MBM {
  struct Monitor : public Manager {
    struct ManagerImp : public Manager {
      virtual int  optparse (const char* c) {
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
    int nbms;
    int m_currLine;
    void draw_frame() {}
    int monitor();
    int put_inf();
    int m_color;
    void setTextcolor(int col)  {
      m_color = col;
      //textcolor(col);
    }

    int draw_line(const char* format,...)  {
      va_list args;
      unsigned char buffer[1024];
      gotoxy(1, m_currLine++);
      va_start( args, format );
      buffer[0] = 0xba;
      int len = ::vsprintf((char*)&buffer[1], format, args)+1;
      if ( len < 130 )  {
        memset(buffer+len,' ',sizeof(buffer)-len);
      }
      buffer[131] = 0xba;
      buffer[132] = 0;
      ::printf((char*)buffer);
      printf("%c",0xba);
      return m_currLine;
    }
    int draw_bar()  {
      char line[256];
      ::memset(line,0xfe,sizeof(line));
      line[255] = 0;
      return draw_line(line);
    }
    int draw_buffer(int which, const char* name, CONTROL* ctrl);
    void draw_a_process(const char* line)  {
      draw_line(line);
    }
    void begin_update()  {
      unsigned char buffer[256];
      ::memset(buffer,0xcd,sizeof(buffer));
      buffer[0]   = 0xc9;
      buffer[131] = 0xbb;
      buffer[132] = 0;
      gotoxy(1,m_currLine=1);
      textcolor(YELLOW);
      ::printf((char*)buffer);
      gotoxy(1,m_currLine=2);
    }
    void end_update() {
      unsigned char buffer[256];
      ::memset(buffer,0xcd,sizeof(buffer));
      buffer[0]   = 0xc8;
      buffer[131] = 0xbc;
      buffer[132] = 0;
      gotoxy(1, m_currLine++);
      ::printf((char*)buffer);
    }
    virtual int  optparse (const char* c);
    int MBM::Monitor::get_bm_list();
    int draw_bar(float ratio,int full_scale);
    Monitor(int argc , char** argv) {
      getOptions(argc, argv);
      nbms = 1;
      bms[0].m_mgr.setup("0");
      m_color = YELLOW;
    }
  };
}

int MBM::Monitor::draw_bar(float ratio,int full_scale)    {
  int barlen  =  int(0.5+ratio*float(full_scale));
  graphics;
  printf("\164");
  for (int i=0;i<barlen;i++) checker;
  for (int j=barlen;j<full_scale;j++) printf("\161");
  printf("\165");
  ascii;
  return 0;
}

int MBM::Monitor::monitor() {
  byte_offset(USER,next,USER_next_off);

  //signal (SIGINT,handler);
  //signal (SIGABRT,handler);

  get_bm_list();    
  _setcursortype(_NOCURSOR);      // hide the cursor
  clrscr();                       // clear the screen
  _setcursortype(_NOCURSOR);      // hide the cursor
  textcolor(YELLOW);              // change textcolor to YELLOW
  textbackground(BLUE);           // change backgroundcolor to BLUE
  draw_frame();
  if( cont )    {
    while( end )    {
      begin_update();
      put_inf();
      lib_rtl_sleep(1000);
    }      
  }
  else  {
    put_inf();
  }
  _setcursortype(_NORMALCURSOR);
  return 1;
}

int MBM::Monitor::draw_buffer(int which, const char* name, CONTROL* ctr)  {
  int i, m;
  char txt[256];
  sprintf(txt," \"%s\"",name);
  draw_line("%-26s  Events: Produced:%d Actual:%d Seen:%d Pending:%d Max:%d",
    txt, ctr->tot_produced, ctr->tot_actual, ctr->tot_seen, ctr->i_events, ctr->p_emax);
  draw_line("%-26s  Space(kB):[Tot:%d Free:%d] Users:[Tot:%d Max:%d]",
    "",ctr->bm_size, ctr->i_space, ctr->i_users, ctr->p_umax);

  sprintf(txt,"  Occupancy [Events]:            ");
  for (i=0, m=125; i<m; ++i)  {
    txt[i+28] = int(float(ctr->i_events)/float(ctr->p_emax)*float(m)) > (i) ? 0xdb : 0xb0;
  }
  txt[i] = 0;
  draw_line(txt);
  sprintf(txt,"            [Space]:             ");
  for (i=0, m=125; i<m; ++i)  {
    txt[i+28] = int(float(ctr->bm_size-ctr->i_space)/float(ctr->bm_size)*float(m)) > (i) ? 0xdb : 0xb0;
  }
  txt[i] = 0;
  draw_line(txt);
  return 1;
}

int MBM::Monitor::put_inf()   {
  static int old_i,new_i;
  USER *us;
  static int count = 0;
  int i, j;
  float perc;
  const char* head=" Name    Partition  Pid Type State Produced    #seen seen Buffer";
  char line[256], tim[32], dat[32];

  ::strtime(tim);
  ::strdate(dat);
  draw_line("");
  draw_line("                               Buffer Manager Monitor [%s %s]", dat, tim);
  draw_line("");
  for (i=0;i<nbms;i++)  {
    BMDESCRIPT* dsc = bms[i].m_mgr.m_bm;
    draw_buffer(i,dsc->bm_name, dsc->ctrl);
    draw_line("");
  }
  if ( nbms <= 0 )  {
    draw_line("");
    draw_line("               No active buffers present");
    draw_line("");
  }
  draw_bar();
  draw_line(head);
  draw_bar();

  new_i	= 0;
  for (i=0;i<nbms;i++)  {
    BMDESCRIPT* dsc = bms[i].m_mgr.m_bm;
    for (j=0,us = dsc->user;j<dsc->ctrl->p_umax;j++,us++)    {
      if (us->busy == 0) continue;
      if (us->ev_produced>0)      {
        sprintf(line," %-13s%4x%5X%5s%6s%9d%21s",
          us->name,us->partid,us->pid,"P",sstat[us->p_state+1],us->ev_produced,dsc->bm_name);    
      }
      else if (us->ev_actual>0)        {
        perc	= ((float)us->ev_seen/(float)us->ev_actual)*100;
        sprintf(line," %-13s%4x%5X%5s%6s         %9d  %3.0f%7s",
          us->name,us->partid,us->pid,"C",sstat[us->c_state+1],
          us->ev_seen,perc+0.1,dsc->bm_name);    
      }
      else        {
        sprintf(line," %-13s%4x%5X%5s    %32s",us->name,us->partid,us->pid,"?",dsc->bm_name);    
      }
      draw_line(line);
      new_i++;
    }
  }
  j = new_i;
  while( j < old_i )   {
    draw_line("");
    j++;
  }
  old_i = new_i;    
  end_update();
  return 1;
}

int MBM::Monitor::optparse (const char* c)  {
  char buff_id[32];
  register int iret;
  switch (*c | 0x20)  {
  case 's':				/*      Single Update*/	
    cont = 0;
    break;
  case 'i':				/*      buffer_id        */	
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
    bms[i].m_bm_row	= 4+i*4;
    bms[i].m_stat_row	= bms[i].m_bm_row+2;
  }
  return 1;
}

int mbm_mon(int argc , char** argv) {
  MBM::Monitor mon(argc, argv);
  return mon.monitor();
}
extern "C" int mbm_ascii(int argc , char** argv) {
  for ( unsigned char i=0; i < 255; ++i)  {
    printf("%3d  %03X   \"%c\"\n", i,i,i);
  }
  return 1;
}
