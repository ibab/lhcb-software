// $Id: FarmDisplay.h,v 1.1 2008-06-10 18:20:19 frankb Exp $
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
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/ROMon/ROMon/FarmDisplay.h,v 1.1 2008-06-10 18:20:19 frankb Exp $
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

  enum { 
    CMD_ADD = 100,
    CMD_CONNECT,
    CMD_DELETE
  };

  class FarmSubDisplay   {
    SCR::Pasteboard*  m_pasteboard;
    SCR::Display*     m_display;
    std::string       m_title;
    int               m_svc;
    int               m_evtBuilt;
    int               m_evtMoore;
    int               m_evtSent;
    int               m_totBuilt;
    int               m_totMoore;
    int               m_totSent;
  public:
    SCR::Display* display() const { return m_display; }
    FarmSubDisplay(SCR::Pasteboard* paste, const std::string& title, bool bad=false);
    virtual ~FarmSubDisplay();
    void init(bool bad);
    void show(int row, int col);
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
    std::string        m_partition;
    Farms              m_farms;
    int                m_height;
    int                m_width;
    int                m_info;
  public:
    /// Standard constructor
    FarmDisplay(int argc, char** argv);

    /// Standard destructor
    virtual ~FarmDisplay();

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

