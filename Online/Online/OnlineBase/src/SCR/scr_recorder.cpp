#include <ctime>
#include <cstdio>
#include <cstring>
#include "SCR/scr.h"
#include "CPP/Event.h"
#include "CPP/Interactor.h"
#include "SCR/MouseSensor.h"

using namespace SCR;

namespace {
  struct ScrRecorder : public Interactor {
    Pasteboard* pasteboard;
    Display* display;
    int key, pasteboard_rows, pasteboard_cols, posx, posy;

    ScrRecorder() : pasteboard_rows(0), pasteboard_cols(80), posx(1), posy(1) {
      ::scrc_create_pasteboard (&pasteboard, 0, &pasteboard_rows, &pasteboard_cols);
      ::scrc_create_display (&display, pasteboard_rows-2, pasteboard_cols-2, BOLD, ON, " Key stroke recorder: CTRL-E to exit");
      ::scrc_begin_pasteboard_update (pasteboard);
      ::scrc_paste_display (display, pasteboard, 2, 2);
      ::scrc_end_pasteboard_update (pasteboard);
      ::scrc_fflush (pasteboard);
      MouseSensor::instance().start(pasteboard);
      MouseSensor::instance().add(this,display);
    }
    virtual ~ScrRecorder() {
      MouseSensor::instance().remove(this,display);
      ::scrc_unpaste_display(display,pasteboard);
      ::scrc_delete_display(display);
      ::scrc_clear_screen(pasteboard);
      ::scrc_delete_pasteboard(pasteboard);
      ::scrc_resetANSI();
    }
    virtual void handle(const Event& ev) {
      char txt[132];
      const MouseEvent* m = ev.get<MouseEvent>();
      sprintf(txt,"((%s-Mouse event: %d  %d x:%d y:%d  %u))",
              m->msec != (unsigned int)~0x0 ? "DBLE" : "SNGL",m->button,m->modifier,m->x,m->y,m->msec);
      addText(txt);
    }

    void addText(const char* txt) {
      int l = ::strlen(txt);
      if ( posx+l > pasteboard_cols-2 ) { 
        posx = 1;
        ++posy;
        if ( posy >= pasteboard_rows-2) {
          posy = 1;
        }
      }
      ::scrc_begin_pasteboard_update (pasteboard);
      ::scrc_put_chars (display, txt, NORMAL, posy, posx, 1);
      ::scrc_end_pasteboard_update (pasteboard);
      ::scrc_fflush (pasteboard);
      posx += l;
    }
  };
}

extern "C" int scr_recorder(int, char**)  {
  char txt[128];
  ScrRecorder rec;
  while(1) {
    int key = ::scrc_wait(rec.display);
    ::sprintf(txt," B'%X/%c' ",(unsigned char)key,(unsigned char)key);
    rec.addText(txt);
    if (key==CTRL_E) break;
  }
  return 0;
}

