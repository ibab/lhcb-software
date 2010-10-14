// $Id: InternalDisplay.h,v 1.3 2010-10-14 06:44:04 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/InternalDisplay.h,v 1.3 2010-10-14 06:44:04 frankb Exp $
#ifndef ROMON_INTERNALDISPLAY_H
#define ROMON_INTERNALDISPLAY_H 1

// Framework includes
#include "ROMon/InternalDisplay.h"
#include "CPP/Interactor.h"
#include "RTL/rtl.h"

// C++ include files
#include <ctime>

class Interactor;
namespace SCR {
  struct Display;
  struct Pasteboard;
}
/*
 *   ROMon namespace declaration
 */
namespace ROMon {
  class InternalDisplay;

  /**@class InternalDisplay ROMon.h GaudiOnline/InternalDisplay.h
   *
   *   Display base class handling standard actions.
   *
   *   @author M.Frank
   */
  class InternalDisplay : public Interactor   {
  protected:
    SCR::Pasteboard*  m_pasteboard;
    SCR::Display*     m_display;
    InternalDisplay*  m_parent;
    std::string       m_name;
    std::string       m_title;
    int               m_svc;
    time_t            m_lastUpdate;
  public:

    /// Access to pasteboard
    SCR::Pasteboard* pasteboard() const { return m_pasteboard; }
    /// Access to SCR display structure
    SCR::Display* display() const       { return m_display;    }
    /// Access to parent to send interrupts
    InternalDisplay* parent() const     { return m_parent;     }
    /// Access subfarm name
    const std::string& name() const     { return m_name;       }
    /// Access title string
    const std::string& title() const    { return m_title;      }
    /// Access to last update time
    time_t lastUpdate() const           { return m_lastUpdate; }

  public:

    /// Initializing constructor
    InternalDisplay(InternalDisplay* parent, const std::string& title);
    /// Standard destructor
    virtual ~InternalDisplay();
    /// Global screen lock shared by all displays
    static lib_rtl_lock_t screenLock();
    /// Return service prefix for usage of the bridge
    static const std::string& svcPrefix();
    /// Set service prefix for usage of the bridge
    static void setSvcPrefix(const std::string& pref);
    /// Draw bar to show occupancies
    size_t draw_bar(int x, int y, float f1, int scale);
    /// Show the display on the main panel
    void show(int row, int col);
    /// Hide the display from the panel
    void hide();
    /// Close the display
    void close();
    /// Disconnect from services: Only destructor may be called afterwards
    void disconnect();
    /// Update display content
    virtual void update(const void* data) = 0;
    /// Update display content
    virtual void update(const void* data, size_t len);
    /// Set cursor position
    virtual void set_cursor(InternalDisplay* /* updater */) {}
    /// Check display for errors
    virtual void check(time_t /* stamp */) {}
    /// Set the focus to this display
    virtual void setFocus() {}
    /// Release the focus of this display
    virtual void releaseFocus() {}
    /// DIM command service callback
    static void dataHandler(void* tag, void* address, int* size);
    /// Interactor overload: Display callback handler
    virtual void handle(const Event& ev);
  };
}      // End namespace ROMon
#endif /* ROMON_INTERNALDISPLAY_H */
