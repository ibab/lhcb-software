/*-----------------------------------------------------------------------*/
/*                                                                       */
/*                  ASCII GRAPHICS DISPLAY IMPLEMEMENTATION              */
/*                                                                       */
/*-----------------------------------------------------------------------*/
#ifndef ONLINEKERNEL_SCR_SCRDISPLAY_H
#define ONLINEKERNEL_SCR_SCRDISPLAY_H

#include <cstdlib>
#include <cstdarg>
#include <string>
#include "CPP/MonitorDisplay.h"

namespace SCR { 
  struct Pasteboard;
  struct Display; 
}

/**@class ScrDisplay ScrDisplay.h SCR/ScrDisplay.h
 *
 * Simple ASCII display implementation for monitoring applications
 *
 * @author Markus Frank
 */
class ScrDisplay : public MonitorDisplay     {
protected:
  SCR::Pasteboard* m_pb;
  SCR::Display*    m_display;
  Area             m_area;
  void print_char(int x, int y, int attr, int val);
public:
  /// Set global pasteborard area
  static void setPasteboard(SCR::Pasteboard* pasteboard);
  /// Set global border types
  static void setBorder(int border);
  /// Access to SCR Pasteboard
  SCR::Pasteboard* pasteboard() const;
  /// Access to SCR display structure
  SCR::Display* display() const               { return m_display;                    }
  /// Display Area
  const Area& area() const                    { return m_area;                       }
  /// Access to the display width in fixed size characters
  virtual size_t width() const                { return m_area.width;                 }
  /// Access to the display height in fixed size characters
  virtual size_t height() const               { return m_area.height;                }
  /// Access to next line pointer
  size_t nextLine()                           { return ++m_currLine;                 }
  /// Access in write mode to display position
  Position& position()                        { return m_position;                   }

public:
  /// Setup display window
  virtual void setup_window();
  /// Default Constructor with display sizes
  virtual void setup_window(const Position& pos, const Area& a, const std::string& title="");
  /// Create subdisplay
  MonitorDisplay* createSubDisplay(const Position& pos, const Area& a, const std::string& title="");
  /// Reset display window
  virtual void reset_window();
  /// Start update cycle
  virtual void begin_update();
  /// Finish update cycle
  virtual void end_update();
  /// Draw empty line to the display
  virtual size_t draw_line();
  /// Draw a line bar
  virtual size_t draw_bar();
  /// Draw formatted line in normal rendering
  virtual size_t draw_line_normal(const char* format,...);
  /// Draw formatted line in reverse rendering
  virtual size_t draw_line_reverse(const char* format,...);
  /// Draw formatted line in bold rendering
  virtual size_t draw_line_bold(const char* format,...);
  /// Draw a progress bar
  virtual size_t draw_bar(int x, int y,float ratio,int full_scale);

  /// Default Constructor: will open in full window mode
  ScrDisplay();
  /// Default Constructor with display sizes
  ScrDisplay(int width, int height);
  /// Default destructor
  virtual ~ScrDisplay();
};
#endif // ONLINEKERNEL_SCR_SCRDISPLAY_H
