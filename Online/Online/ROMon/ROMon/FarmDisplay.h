// $Id: FarmDisplay.h,v 1.24 2010-10-15 10:53:54 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/FarmDisplay.h,v 1.24 2010-10-15 10:53:54 frankb Exp $
#ifndef ROMON_FARMDISPLAY_H
#define ROMON_FARMDISPLAY_H 1

// Framework includes
#include "ROMon/FarmDisplayBase.h"

/*
 *   ROMon namespace declaration
 */
namespace ROMon {

#if 0
  class FarmDisplayShow : public InternalDisplay  {
  protected:
    enum { HLT_MODE, RECO_MODE, CTRL_MODE };
    ClusterDisplay*                  m_subfarmDisplay;
    ScrDisplay*                      m_nodeSelector;
    std::auto_ptr<ClusterDisplay>    m_sysDisplay;
    std::auto_ptr<ProcessDisplay>    m_procDisplay;
    std::auto_ptr<CtrlNodeDisplay>   m_ctrlDisplay;
    std::auto_ptr<BufferDisplay>     m_mbmDisplay;
    std::auto_ptr<HelpDisplay>       m_helpDisplay;
    std::auto_ptr<CPUDisplay>        m_cpuDisplay;
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

    /// Standard constructor
    FarmDisplayShow();
    /// Standard destructor
    virtual ~FarmDisplayShow();

    /// Get farm display name from cursor position
    virtual std::string currentDisplayName()  const = 0;
    /// Get the name of the currently selected cluster
    virtual std::string selectedCluster() const = 0;
    /// Get the name of the currently selected cluster and node
    virtual std::pair<std::string,std::string> selectedNode() const = 0;
    /// Set cursor to appropriate position
    virtual void set_cursor() = 0;
    /// Set cursor position to specified display
    virtual void set_cursor(InternalDisplay* d) 
    { this->InternalDisplay::set_cursor(d); }

    /// Show context dependent help window
    int showHelpWindow();
    /// Show subfarm display
    int showSubfarm();
    /// Show window with node control information
    int showCtrlWindow();
    /// Show window with processes on a given node
    int showProcessWindow(int full=0);
    /// Show window with CPU information of a given subfarm
    int showCpuWindow();
    /// Show window with buffer information of a given node
    int showMbmWindow();
    /// Show window with SYSTEM information of a given subfarm
    int showSysWindow();
    /// Show window with boot information of the subfarm
    int showBootWindow();
    /// Show window with boot information of the subfarm
    int showStatsWindow();
    /// Show window to run Moore bench marks
    int showBenchmarkWindow();

    /// Handle common Mouse interrupt
    virtual bool handleMouseEvent(const SCR::MouseEvent* m);
    /// Handle common IOC interrupt
    virtual bool handleIocEvent(const CPP::Event& ev);
    /// Handle common keyboard interrupts
    virtual int handleKeyboard(int key);
  };
#endif

  /**@class FarmDisplay ROMon.h GaudiOnline/FarmDisplay.h
   *
   *   Monitoring display for the LHCb storage system.
   *
   *   @author M.Frank
   */
  class FarmDisplay : public FarmDisplayBase  {
  protected:
    typedef std::map<std::string, InternalDisplay*> SubDisplays;
    typedef std::vector<std::string> Farms;
    SubDisplays                      m_farmDisplays;
    std::auto_ptr<PartitionListener> m_listener;
    std::string                      m_match;
    int                              m_height;
    int                              m_width;
    int                              m_dense;
    /// Cursor position in sub display array
    int                              m_subDisplayHeight;

    /// Keyboard rearm action
    static int key_rearm (unsigned int fac, void* param);
    /// Keyboard action
    static int key_action(unsigned int fac, void* param);

  public:
    /// Standard constructor
    FarmDisplay(int argc, char** argv);

    /// Standard destructor
    virtual ~FarmDisplay();

    /// Get farm <partition>/<display name> from cursor position
    virtual std::string currentCluster()  const;

    /// Get farm display name from cursor position
    virtual std::string currentDisplayName()  const;

    /// Get the name of the currently selected cluster
    virtual std::string selectedCluster() const;

    /// Get the name of the currently selected cluster and node
    virtual std::pair<std::string,std::string> selectedNode() const;

    /// Number of sub-nodes in a cluster
    size_t selectedClusterSize() const;

    /// Handle keyboard interrupts
    int handleKeyboard(int key);

    /// Get farm display from cursor position
    InternalDisplay* currentDisplay()  const;

    /// Accessor to sub-displays of main panel
    SubDisplays& subDisplays() {  return m_farmDisplays; }

    /// Set cursor to position
    virtual void set_cursor();

    /// Set cursor to position
    virtual void set_cursor(InternalDisplay* updater);
    /// Interactor overload: Display callback handler
    virtual void handle(const CPP::Event& ev);
    /// Connect to data resources
    virtual void connect()  {  InternalDisplay::connect(); }
    /// Connect to data sources
    void connect(const std::string& section, const std::vector<std::string>& farms);
    /// DIM command service callback
    virtual void update(const void* /* data */ )       {                                          }
    /// Update display content
    virtual void update(const void* data, size_t len)  { this->InternalDisplay::update(data,len); }

    /// DIM command service callback
    static void dnsDataHandler(void* tag, void* address, int* size);

  };
}      // End namespace ROMon
#endif /* ROMON_FARMDISPLAY_H */
