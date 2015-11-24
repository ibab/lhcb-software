#ifndef ONLINE_ONLINEKERNEL_CPP_MONITORDISPLAY_H
#define ONLINE_ONLINEKERNEL_CPP_MONITORDISPLAY_H

// C++ Include files
#include <cstdarg>
#include <string>

// Forward declarations
namespace CPP { class Interactor;  }

class Position {
public:
  int x, y;
  Position() : x(0), y(0) {}
  Position(int xx, int yy) : x(xx), y(yy) {}
};
class Area {
public:
  int width, height;
  Area() : width(0), height(0) {}
  Area(int w, int h) : width(w), height(h) {}
};

/**@class MonitorDisplay MonitorDisplay.h CPP/MonitorDisplay.h
 *
 * Template to write simple monitoring applications with plugable display types.
 * @author M.Frank
 */
class MonitorDisplay {
 public:
  typedef CPP::Interactor Interactor;

protected:
  /// Display Client to handle interface callbacks
  Interactor*   m_client;
  /// Currentl ine when drawing display
  size_t             m_currLine;
  /// Display position in global window (if applicable)
  Position           m_position;
  /// Display title
  std::string        m_title;

public:
  /// Access to the display width in fixed size characters
  virtual size_t width() const  = 0;
  /// Access to the display height in fixed size characters
  virtual size_t height() const = 0;
  virtual size_t left() const                 { return m_position.x;                 }
  virtual size_t right() const                { return m_position.x + width();       }
  virtual size_t top() const                  { return m_position.y;                 }
  virtual size_t bottom() const               { return m_position.y + height();      }
  const Position& position() const            { return m_position;                   }

  /// Access to current line
  size_t currLine() const        {  return m_currLine; }
  /// Attach interactor client for callback handling
  void setClient(Interactor* cl) {   m_client = cl;    }
  /// Setup display window
  virtual void setup_window() = 0;
  /// Reset display window
  virtual void reset_window() = 0;
  /// Start update cycle
  virtual void begin_update() = 0;
  /// Finish update cycle
  virtual void end_update() = 0;
  /// Draw empty line to the display
  virtual size_t draw_line() = 0;
  /// Draw a line bar
  virtual size_t draw_bar() = 0;
  /// Draw formatted line in normal rendering
  virtual size_t draw_line_normal(const char* format,...) = 0;
  /// Draw formatted line in reverse rendering
  virtual size_t draw_line_reverse(const char* format,...) = 0;
  /// Draw formatted line in bold rendering
  virtual size_t draw_line_bold(const char* format,...) = 0;
  /// Draw a progress bar
  virtual size_t draw_bar(int x, int y,float ratio,int full_scale) = 0;
  /// Default Constructor
  MonitorDisplay() : m_client(0), m_currLine(0) {}
  /// Default destructor
  virtual ~MonitorDisplay() {}
};
#endif // ONLINE_ONLINEKERNEL_CPP_MONITORDISPLAY_H

