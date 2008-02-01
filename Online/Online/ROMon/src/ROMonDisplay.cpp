#include "SCR/scr.h"
#include "SCR/ScrDisplay.h"
#include "RTL/rtl.h"
#include <algorithm>

/** @class 
 *
 * @author  M.Frank
 * @version 1.0
 */
class ROMonDisplay : public ScrDisplay  {
public:
  MonitorDisplay* m_buffers;
  MonitorDisplay* m_streams;
  MonitorDisplay* m_logging;
  MonitorDisplay* m_files;
  MonitorDisplay* m_hltRec;
  ROMonDisplay();
  void waitInput();
};

using namespace SCR;
ROMonDisplay::ROMonDisplay()   {
  setup_window();
  m_hltRec  = createSubDisplay(Position(0,5), Area(27,m_area.height-5),"Farm receivers");
  int right = m_hltRec->right();
  int width = m_area.width-right;
  m_buffers = createSubDisplay(Position(right,5), Area(width,8),"Buffer Monitor");
  m_streams = createSubDisplay(Position(right,m_buffers->bottom()-1),Area(width,12),"Stream Information");
  m_logging = createSubDisplay(Position(right,m_streams->bottom()-1),Area(width,4), "Logger Summary");
  m_files   = createSubDisplay(Position(right,m_logging->bottom()-1),Area(width,m_area.height-m_logging->bottom()+1), "File Information");
  
  end_update();
  ::scrc_set_cursor(display(),2,2);
}

void ROMonDisplay::waitInput() {
  while(1) {
    //::scrc_wait(display());
    begin_update();
    draw_line();
    draw_line_normal("Storage Monitor");
    draw_line_reverse("Storage Monitor");
    m_buffers->begin_update();
    m_buffers->draw_line_bold("Buffer info:%s",::lib_rtl_timestr());
    m_buffers->draw_bar();
    m_streams->begin_update();
    m_streams->draw_bar();
    m_streams->draw_line_bold("Stream info:%s",::lib_rtl_timestr());    
    m_streams->draw_bar();
    m_streams->end_update();
    m_buffers->end_update();
    end_update();
    ::scrc_set_cursor(display(),2,2);
    lib_rtl_sleep(1000);
  }
}

extern "C" int scr_romon(int /* argc */, char** /* argv */) {
  ROMonDisplay disp;
  disp.waitInput();
  return 1;
}
