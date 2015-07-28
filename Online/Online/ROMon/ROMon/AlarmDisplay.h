// $Id: AlarmDisplay.h,v 1.2 2010-09-03 14:47:45 frankb Exp $
//====================================================================
//  ROMon
//--------------------------------------------------------------------
//
//  Package    : ROMon
//
//  Description: Base class for displays for readout monitoring 
//               in the LHCb experiment
//
//  Author     : M.Frank
//  Created    : 29/1/2008
//
//====================================================================
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/AlarmDisplay.h,v 1.2 2010-09-03 14:47:45 frankb Exp $
#ifndef ROMON_ALARMDISPLAY_H
#define ROMON_ALARMDISPLAY_H 1

// Framework includes
#include "ROMon/InternalDisplay.h"
#include "ROMon/Constants.h"

class Interactor;
namespace SCR {
  struct Display;
  struct Pasteboard;
}
/*
 *   ROMon namespace declaration
 */
namespace ROMon {
  class Alarm;
  class MessageWindow;
  class AlarmDisplay;

  /**@class MessageWindow ROMon.h GaudiOnline/AlarmDisplay.h
   *
   *   @author M.Frank
   */
  class MessageWindow : public InternalDisplay {
  protected:
    int m_level;

  private:
    /// Private copy constructor
    MessageWindow(const MessageWindow&) : InternalDisplay() {}
    /// Private assignment operator
    MessageWindow& operator=(const MessageWindow&) { return *this; }
  public:
    /// Initializing constructor
    MessageWindow(AlarmDisplay* parent, const std::string& title, int height, int width, int lvl);
    /// Standard destructor
    virtual ~MessageWindow() {}
    /// Update display content
    virtual void update(const void*);
    /// Update display content
    virtual void update(const void* data, size_t len)  { this->InternalDisplay::update(data,len); }
  };

  /**@class AlarmPrinter ROMon.h GaudiOnline/AlarmPrinter.h
   *
   *   Monitoring display for the LHCb storage system.
   *
   *   @author M.Frank
   */
  class AlarmPrinter : public Interactor  {
  public:
    typedef std::map<int,Alarm*>     DisplayAlarms;

  protected:
    DisplayAlarms                    m_alarms;
    std::string                      m_name;
    int                              m_svc;
  public:
    /// Standard constructor
    AlarmPrinter(int argc, char** argv);
    /// Standard destructor
    virtual ~AlarmPrinter();
    /// Interactor overload: Display callback handler
    virtual void handle(const Event& ev);
    /// Handle new incoming alarm
    void handleAlarm(const std::string& alarm);

    /// DIM command service callback
    static void dataHandler(void* tag, void* address, int* size);
  };

  /**@class AlarmDisplay ROMon.h GaudiOnline/AlarmDisplay.h
   *
   *   Monitoring display for the LHCb storage system.
   *
   *   @author M.Frank
   */
  class AlarmDisplay : public InternalDisplay  {
  public:
    typedef std::map<int,Alarm*>     DisplayAlarms;

  protected:
    int                              m_height;
    int                              m_width;
    int                              m_dense;
    /// Cursor position in sub display array
    size_t                           m_posCursor;
    size_t                           m_subPosCursor;
    int                              m_anchorX, m_anchorY;
    InternalDisplay*                 m_alarmDisplay;
    InternalDisplay*                 m_warnDisplay;
    InternalDisplay*                 m_monDisplay;
    DisplayAlarms                    m_alarms;

    /// Keyboard rearm action
    static int key_rearm (unsigned int fac, void* param);
    /// Keyboard action
    static int key_action(unsigned int fac, void* param);

  public:
    /// Standard constructor
    AlarmDisplay(int argc, char** argv);
    /// Standard destructor
    virtual ~AlarmDisplay();

    /// Handle new incoming alarm
    void handleAlarm(const std::string& alarm);

    /// Handle keyboard interrupts
    int handleKeyboard(int key);

    /// Set cursor to position
    virtual void set_cursor(InternalDisplay* updater);

    /// Show Help window
    int showHelpWindow();

    /// Interactor overload: Display callback handler
    virtual void handle(const Event& ev);

    /// InternalDisplay overload: update method: DIM command service callback
    virtual void update(const void* /* data */ ) {}

    /// Update display content
    virtual void update(const void* data, size_t len)  { this->InternalDisplay::update(data,len); }

    /// DIM command service callback
    static void dataHandler(void* tag, void* address, int* size);
  };

}      // End namespace ROMon
#endif /* ROMON_ALARMDISPLAY_H */
