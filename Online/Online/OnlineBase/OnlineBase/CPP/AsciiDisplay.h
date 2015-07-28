/*-----------------------------------------------------------------------*/
/*                                                                       */
/*                  ASCII GRAPHICS DISPLAY IMPLEMEMENTATION              */
/*                                                                       */
/*-----------------------------------------------------------------------*/
#ifndef ONLINEKERNEL_CPP_ASCIIDISPLAY_H
#define ONLINEKERNEL_CPP_ASCIIDISPLAY_H

#include <cstdlib>
#include <cstdarg>
#include <string>
#include "SCR/scr.h"
#include "CPP/MonitorDisplay.h"

/**@class AsciiDisplay AsciiDisplay.h CPP/AsciiDisplay.h
 *
 * Simple ASCII display implementation for monitoring applications
 *
 * @author Markus Frank
 */
class AsciiDisplay : public MonitorDisplay  {
protected:
  /// Pointer to WINDOW structure
  SCR::Pasteboard* m_pasteboard;
  SCR::Display*    m_display;
  //WINDOW*          m_window;
  /// Color setup
  int              m_color;
  /// Display width
  int              m_width;
  /// Display height
  int              m_height;
  /// Display title
  std::string      m_title;
public:
  /// Setup display window
  virtual void setup_window();
  /// Reset display window
  virtual void reset_window();
  /// Start update cycle
  virtual void begin_update();
  /// Finish update cycle
  virtual void end_update();
  /// Access to the display width in fixed size characters
  virtual size_t width() const;
  /// Access to the display height in fixed size characters
  virtual size_t height() const;
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
  /// Default Constructor
  AsciiDisplay();
  /// Default Constructor with title
  explicit AsciiDisplay(const std::string& title);
  /// Default destructor
  virtual ~AsciiDisplay() {}
};
#endif // ONLINEKERNEL_CPP_ASCIIDISPLAY_H
