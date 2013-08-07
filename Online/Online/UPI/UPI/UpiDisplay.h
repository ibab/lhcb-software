/*-----------------------------------------------------------------------*/
/*                                                                       */
/*                  ASCII GRAPHICS DISPLAY IMPLEMEMENTATION              */
/*                                                                       */
/*-----------------------------------------------------------------------*/
#ifndef UPI_UPIDISPLAY_H
#define UPI_UPIDISPLAY_H

#include <cstdarg>
#include "CPP/MonitorDisplay.h"

/*
 * UPI namespace
 */
namespace UPI {

  class ReallyClose : public Interactor {
  protected:
    int m_menuID;
    char m_buff[32];
  public:
    ReallyClose(int parent_id,int cmd_id);
    virtual ~ReallyClose();
    void invoke();
    virtual void handle(const Event& ev);
  };

  /**@class UpiDisplay UpiDisplay.h UPI/UpiDisplay.h
   *
   * Simple ASCII display implementation for monitoring applications
   *
   * @author Markus Frank
   */
  class UpiDisplay : public Interactor, public MonitorDisplay   {
    protected:
      int          m_menuID, m_lastCMD;
      char**       m_lines;
      ReallyClose* m_quit;
      Area         m_area;
      const Area& area() const                    { return m_area;                       }
      void print_char(int x, int y, int val);
    public:
      enum { CMD_CLOSE=1 };

    private:
      /// No copy constructor allowed
      UpiDisplay(const UpiDisplay& d);
      /// No assignment allowed
      UpiDisplay& operator=(const UpiDisplay& d);
    public:
      /// Access to the display width in fixed size characters
      virtual size_t width() const                { return m_area.width;                 }
      /// Access to the display height in fixed size characters
      virtual size_t height() const               { return m_area.height;                }
      /// Setup display window
      virtual void setup_window();
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
      /// Handle UPI interrupts from menu
      virtual void handle(const Event& ev);
      /// Default Constructor
      UpiDisplay(size_t width, size_t height);
      /// Default destructor
      virtual ~UpiDisplay();
    };
}
#endif // UPI_UPIDISPLAY_H
