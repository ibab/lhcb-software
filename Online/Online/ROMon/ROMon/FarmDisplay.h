// $Id: FarmDisplay.h,v 1.21 2010-09-20 18:59:49 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/FarmDisplay.h,v 1.21 2010-09-20 18:59:49 frankb Exp $
#ifndef ROMON_FARMDISPLAY_H
#define ROMON_FARMDISPLAY_H 1

// Framework includes
#include "ROMon/PartitionListener.h"
#include "ROMon/InternalDisplay.h"
#include "ROMon/HelpDisplay.h"
#include "ROMon/ClusterDisplay.h"
#include "ROMon/Constants.h"
#include "CPP/Interactor.h"

// C++ include files
#include <map>
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

  class ProcessDisplay;
  class RecSubfarmDisplay;
  class SubfarmDisplay;
  class ROMonDisplay;
  class FarmDisplay;
  class ProcFarm;
  class CPUfarm;


  /**@class ProcessDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Display showing all processes on a given node.
   *
   *   @author M.Frank
   */
  class ProcessDisplay : public InternalDisplay {
    /// Flag for various operation modes
    int m_flag;
  public:
    /// Initializing constructor
    ProcessDisplay(FarmDisplay* parent, const std::string& title, const std::string& cluster, int full=0, int height=55,int width=120);
    /// Standard destructor
    virtual ~ProcessDisplay();
    /// Update display content
    virtual void update(const void* data);
    /// Update display content
    virtual void update(const void* data, size_t len)  { this->InternalDisplay::update(data,len); }
    /// Update display content
    void updateContent(const ProcFarm& pf);
  };

  /**@class CPUDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Display showing all processes on a given node.
   *
   *   @author M.Frank
   */
  class CPUDisplay : public InternalDisplay {
    /// Node name
    std::string  m_node;
  public:
    /// Initializing constructor
    CPUDisplay(FarmDisplay* parent, const std::string& title, const std::string& node, int height=55,int width=120);
    /// Standard destructor
    virtual ~CPUDisplay();
    /// Update display content
    virtual void update(const void* data);
    /// Update display content
    virtual void update(const void* data, size_t len)  { this->InternalDisplay::update(data,len); }
    /// Update display content
    void updateContent(const CPUfarm& pf);
  };

  /**@class BufferDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Internal MBM monitor display, when spying on individual nodes.
   *
   *   @author M.Frank
   */
  class BufferDisplay : public InternalDisplay {
  protected:
    /// Node number in node set
    int m_node;
  public:
    /// Initializing constructor
    BufferDisplay(FarmDisplay* parent, const std::string& title);
    /// Standard destructor
    virtual ~BufferDisplay() {}
    /// Set the node number for the display
    void setNode(int which) { m_node = which; }
    /// Update display content
    virtual void update(const void* data);
    /// Update display content
    virtual void update(const void* data, size_t len)  { this->InternalDisplay::update(data,len); }
  };

  /**@class CtrlNodeDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Internal Task control display, when spying on individual nodes.
   *
   *   @author M.Frank
   */
  class CtrlNodeDisplay : public InternalDisplay {
  protected:
    /// Node number in node set
    int m_node;
  public:
    /// Initializing constructor
    CtrlNodeDisplay(FarmDisplay* parent, const std::string& title);
    /// Standard destructor
    virtual ~CtrlNodeDisplay() {}
    /// Set the node number for the display
    void setNode(int which) { m_node = which; }
    /// Update display content
    virtual void update(const void* data);
    /// Update display content
    virtual void update(const void* data, size_t len)  { this->InternalDisplay::update(data,len); }
  };

  /**@class FarmDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Monitoring display for the LHCb storage system.
   *
   *   @author M.Frank
   */
  class FarmDisplay : public InternalDisplay  {
  protected:
    enum { HLT_MODE, RECO_MODE, CTRL_MODE };
    typedef std::map<std::string, InternalDisplay*> SubDisplays;
    typedef std::vector<std::string> Farms;
    SubDisplays                      m_farmDisplays;
    std::auto_ptr<PartitionListener> m_listener;
    ClusterDisplay*                  m_subfarmDisplay;
    std::auto_ptr<ProcessDisplay>    m_procDisplay;
    std::auto_ptr<CtrlNodeDisplay>   m_ctrlDisplay;
    std::auto_ptr<BufferDisplay>     m_mbmDisplay;
    std::auto_ptr<HelpDisplay>       m_helpDisplay;
    std::auto_ptr<CPUDisplay>        m_cpuDisplay;
    ScrDisplay*                      m_nodeSelector;
    std::string                      m_partition;
    std::string                      m_match;
    /// vector with all farm displays
    Farms                            m_farms;
    int                              m_height;
    int                              m_width;
    int                              m_dense;
    /// Cursor position in sub display array
    size_t                           m_posCursor;
    size_t                           m_subPosCursor;
    int                              m_anchorX, m_anchorY;
    int                              m_mode;
    /// Keyboard rearm action
    static int key_rearm (unsigned int fac, void* param);
    /// Keyboard action
    static int key_action(unsigned int fac, void* param);

public:
    /// Standard constructor
    FarmDisplay(int argc, char** argv);
    /// Standard destructor
    virtual ~FarmDisplay();
    /// Show subfarm display
    int showSubfarm();
    /// Handle keyboard interrupts
    int handleKeyboard(int key);
    /// Get farm display from cursor position
    InternalDisplay* currentDisplay();
    /// Accessor to sub-displays of main panel
    SubDisplays& subDisplays() {  return m_farmDisplays; }
    /// Set cursor to position
    virtual void set_cursor();
    /// Set cursor to position
    virtual void set_cursor(InternalDisplay* updater);
    /// Interactor overload: Display callback handler
    virtual void handle(const Event& ev);
    /// Connect to data sources
    void connect(const std::vector<std::string>& farms);
    /// DIM command service callback
    virtual void update(const void* data);
    /// Update display content
    virtual void update(const void* data, size_t len)  { this->InternalDisplay::update(data,len); }
    /// Show context dependent help window
    int showHelpWindow();
    /// Show window with processes on a given node
    int showProcessWindow(int full=0);
    /// Show window with CPU information of a given subfarm
    int showCpuWindow();
    /// Show window with buffer information of a given node
    int showMbmWindow();
    /// Show window with node control information
    int showCtrlWindow();
    /// DIM command service callback
    static void dnsDataHandler(void* tag, void* address, int* size);

  };
}      // End namespace ROMon
#endif /* ROMON_FARMDISPLAY_H */
