// $Id: FarmLineDisplay.h,v 1.24 2010-10-15 10:53:54 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/FarmLineDisplay.h,v 1.24 2010-10-15 10:53:54 frankb Exp $
#ifndef ROMON_FARMDISPLAYBASE_H
#define ROMON_FARMDISPLAYBASE_H 1

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

namespace SCR {
  struct Display;
  struct Pasteboard;
  struct MouseEvent;
}
/*
 *   ROMon namespace declaration
 */
namespace ROMon {

  class ProcessDisplay;
  class RecSubfarmDisplay;
  class SubfarmDisplay;
  class ROMonDisplay;
  class FarmLineDisplay;
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
    ProcessDisplay(InternalDisplay* parent, const std::string& title, const std::string& cluster, int full=0, int height=60,int width=132);
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
    CPUDisplay(InternalDisplay* parent, const std::string& title, const std::string& node, int height=75,int width=132);
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
    /// Partition name for filtering output
    std::string m_partition;

  private:
    /// Private copy constructor
    BufferDisplay(const BufferDisplay&) : InternalDisplay() {}
    /// Private assignment operator
    BufferDisplay& operator=(const BufferDisplay&) { return *this; }
  public:
    /// Initializing constructor
    BufferDisplay(InternalDisplay* parent, const std::string& title, const std::string& part_name="");
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
    /// Node name if running standalone
    std::string m_nodeName;

  private:
    /// Private copy constructor
    CtrlNodeDisplay(const CtrlNodeDisplay&) : InternalDisplay() {}
    /// Private assignment operator
    CtrlNodeDisplay& operator=(const CtrlNodeDisplay&) { return *this; }

  public:
    /// Initializing constructor
    CtrlNodeDisplay(InternalDisplay* parent, const std::string& title);
    /// Standard destructor
    virtual ~CtrlNodeDisplay() {}
    /// Set the node number for the display
    void setNode(int which) { m_node = which; }
    /// Connect to data resources
    virtual void connect()  {  InternalDisplay::connect(); }
    /// Connect display to data sources
    virtual void connect(const std::string& node);
    /// Update display content
    virtual void update(const void* data);
    /// Update display content
    virtual void update(const void* data, size_t len)  { this->InternalDisplay::update(data,len); }
    /// DIM command service callback
    static void tsDataHandler(void* tag, void* address, int* size);
  };

  /**@class FarmDisplayBase FarmDisplayBase.h GaudiOnline/FarmDisplayBase.h
   *
   *   Basic display class for farm monitoring applications.
   *
   *   @author M.Frank
   */
  class FarmDisplayBase : public InternalDisplay  {
  protected:
    enum { HLT_MODE, RECO_MODE, CTRL_MODE, TORRENT_MODE, HLTDEFER_MODE };
    ClusterDisplay*                  m_subfarmDisplay;
    ScrDisplay*                      m_nodeSelector;
    std::auto_ptr<ClusterDisplay>    m_roDisplay;
    std::auto_ptr<ClusterDisplay>    m_sysDisplay;
    std::auto_ptr<ClusterDisplay>    m_torrentDisplay;
    std::auto_ptr<ClusterDisplay>    m_deferHltDisplay;
    std::auto_ptr<ProcessDisplay>    m_procDisplay;
    std::auto_ptr<CtrlNodeDisplay>   m_ctrlDisplay;
    std::auto_ptr<BufferDisplay>     m_mbmDisplay;
    std::auto_ptr<HelpDisplay>       m_helpDisplay;
    std::auto_ptr<CPUDisplay>        m_cpuDisplay;
    std::auto_ptr<InternalDisplay>   m_torrentNodeDisplay;
    std::auto_ptr<InternalDisplay>   m_bootDisplay;
    std::auto_ptr<InternalDisplay>   m_statsDisplay;
    std::auto_ptr<InternalDisplay>   m_benchDisplay;
    int                              m_subDisplayHeight;
    int                              m_anchorX, m_anchorY;
    int                              m_mode;
    /// Main cursor position
    size_t                           m_posCursor;
    /// Cursor position in sub display array
    size_t                           m_subPosCursor;
    /// Flag to setup reverse colors in display
    bool                             m_reverse;
    /// Partition name if output filtering should be applied
    std::string                      m_partition;

    /// Standard constructor
    FarmDisplayBase();
    /// Standard destructor
    virtual ~FarmDisplayBase();

    /// Get farm <partition>/<display name> from cursor position
    virtual std::string currentCluster()  const = 0;
    /// Get farm display name from cursor position
    virtual std::string currentDisplayName()  const = 0;
    /// Get the name of the currently selected cluster
    virtual std::string selectedCluster() const = 0;
    /// Get the name of the currently selected cluster and node
    virtual std::pair<std::string,std::string> selectedNode() const = 0;
    /// Set cursor to appropriate position
    virtual void set_cursor() = 0;
    /// Set cursor position to specified display
    virtual void set_cursor(InternalDisplay* d) { this->InternalDisplay::set_cursor(d); }

    /// Show context dependent help window
    int showHelpWindow();
    /// Show subfarm display
    int showSubfarm();
    /// Show window with node control information
    int showCtrlWindow();
    /// Show window with deferred trigger information for a subfarm
    int showDeferHltWindow();
    /// Show window with processes on a given node
    int showProcessWindow(int full=0);
    /// Show window with CPU information of a given subfarm
    int showCpuWindow();
    /// Show window with buffer information of a given node
    int showMbmWindow();
    /// Show window with torrent information of a given node
    int showTorrentNodeWindow();
    /// Show window with SYSTEM information of a given subfarm
    int showSysWindow();
    /// Show window with boot information of the subfarm
    int showBootWindow();
    /// Show window with boot information of the subfarm
    int showStatsWindow();
    /// Show window to run Moore bench marks
    int showBenchmarkWindow();
    /// Show window to with the bittorrent data distribution status for this node
    int showTorrentWindow();
    /// Show readout window 
    int showReadoutWindow();

    /// Handle common Mouse interrupt
    virtual bool handleMouseEvent(const SCR::MouseEvent* m);
    /// Handle common IOC interrupt
    virtual bool handleIocEvent(const CPP::Event& ev);
    /// Handle common keyboard interrupts
    virtual int handleKeyboard(int key);
  };

}      // End namespace ROMon
#endif /* ROMON_FARMDISPLAYBASE_H */
