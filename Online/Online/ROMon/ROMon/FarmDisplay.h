// $Id: FarmDisplay.h,v 1.2 2008-06-24 15:13:19 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/FarmDisplay.h,v 1.2 2008-06-24 15:13:19 frankb Exp $
#ifndef ROMON_FARMDISPLAY_H
#define ROMON_FARMDISPLAY_H 1

// Framework includes
#include "ROMon/ROMonDisplay.h"
#include "CPP/Interactor.h"

// C++ include files
#include <map>

class Interactor;
namespace SCR {
  struct Display;
  struct Pasteboard;
}
/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  class SubfarmDisplay;

  enum { 
    CMD_ADD = 100,
    CMD_CONNECT,
    CMD_CHECK,
    CMD_DELETE,
    CMD_UPDATE
  };

  class FarmSubDisplay   {
    SCR::Pasteboard*  m_pasteboard;
    SCR::Display*     m_display;
    Interactor*       m_parent;
    std::string       m_name;
    std::string       m_title;
    int               m_svc;
    int               m_evtBuilt;
    int               m_evtMoore;
    int               m_evtSent;
    int               m_totBuilt;
    int               m_totMoore;
    int               m_totSent;
    time_t            m_lastUpdate;
    bool              m_hasProblems;
  public:
    /// Access to SCR display structure
    SCR::Display* display() const { return m_display; }
    /// Access to last update time stamp
    time_t lastUpdate() const { return m_lastUpdate; }
    /// Access to problem flag
    bool hasProblems() const { return m_hasProblems; }
    /// Access subfarm name
    const std::string& name() const { return m_name; }
    /// Set timeout error
    void setTimeoutError();
    /// Initializing constructor
    FarmSubDisplay(SCR::Pasteboard* paste, Interactor* parent, const std::string& title, bool bad=false);
    /// Standard destructor
    virtual ~FarmSubDisplay();
    /// Initialize default display text
    void init(bool bad);
    /// Show the display on the main panel
    void show(int row, int col);
    /// Hide the display from the panel
    void hide();
    /// DIM command service callback
    static void dataHandler(void* tag, void* address, int* size);
    /// Update display content
    virtual void update(const Nodeset& ns);
  };

  class PartitionListener {
  protected:
    Interactor* m_parent;
    std::string m_name;
    int         m_subFarmDP;
  public:
    /// Standard constructor with object setup through parameters
    PartitionListener(Interactor* parent,const std::string& name);
    /// Standard destructor
    virtual ~PartitionListener();
    /// DIM command service callback
    static void subFarmHandler(void* tag, void* address, int* size);
  };


  /**@class FarmDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Monitoring display for the LHCb storage system.
   *
   *   @author M.Frank
   */
  class FarmDisplay : public Interactor  {
  protected:
    typedef std::map<std::string, FarmSubDisplay*> SubDisplays;
    typedef std::vector<std::string> Farms;
    /// vector with all farm displays
    SCR::Pasteboard*   m_pasteboard;
    SCR::Display*      m_display;
    SubDisplays        m_farmDisplays;
    PartitionListener* m_listener;
    SubfarmDisplay*    m_subfarmDisplay;
    std::string        m_partition;
    Farms              m_farms;
    int                m_height;
    int                m_width;
    int                m_info;
    int                m_dense;
    /// Cursor position in sub display array
    size_t             m_posCursor;
    size_t             m_subPosCursor;
    SCR::Display*      m_mbmDisplay;
    SCR::Display*      m_helpDisplay;

    /// Keyboard rearm action
    static int key_rearm (unsigned int fac, void* param);
    /// Keyboard action
    static int key_action(unsigned int fac, void* param);

    /// Show MBM subdisplay of a given node
    void showMBM(int which);

    void updateMBM(int which);

    size_t draw_bar(int x, int y, float f1, int scale);

    /// Invoke help window
    void showHelp();

public:
    /// Standard constructor
    FarmDisplay(int argc, char** argv);

    /// Standard destructor
    virtual ~FarmDisplay();

    /// Show subfarm display
    int showSubfarm();

    /// Handle keyboard interrupts
    int handleKeyboard(int key);

    /// Access to SCR pasteboard structure
    SCR::Pasteboard* pasteboard() const { return m_pasteboard; }

    /// Access to SCR display structure
    SCR::Display* display() const { return m_display; }

    /// Get farm display from cursor position
    FarmSubDisplay* currentDisplay();

    SubDisplays& subDisplays() {  return m_farmDisplays; }

    /// Set cursor to position
    void set_cursor();

    /// Interactor overload: Display callback handler
    virtual void handle(const Event& ev);

    /// Show the display header information (title, time, ...)
    void showHeader(const Nodeset& ns);

    /// Connect to data sources
    void connect(const std::vector<std::string>& farms);

    /// DIM command service callback
    static void infoHandler(void* tag, void* address, int* size);
  };
}      // End namespace ROMon
#endif /* ROMON_FARMDISPLAY_H */

