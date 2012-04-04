// $Id: RootPerfMonSvc.h,v 1.7 2010-09-14 06:01:12 frankb Exp $
//====================================================================
//	RootPerfMonSvc definition
//--------------------------------------------------------------------
//
//	Author     : M.Frank
//====================================================================
#ifndef GAUDIROOTCNV_GAUDIROOTCNVSVC_H
#define GAUDIROOTCNV_GAUDIROOTCNVSVC_H
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/RootCnv/src/RootPerfMonSvc.h,v 1.7 2010-09-14 06:01:12 frankb Exp $

// Framework include files
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Service.h"

// C/C++ include files
#include <map>
#include <sys/time.h>

// Forward declarations
class IIncidentSvc;
class TFile;
class TTree;
class TBranch;
class TDirectory;
class TTreePerfStats;

/*
 * Gaudi namespace declaration
 */
namespace Gaudi {

  struct PerfMonCounters;

  /** @class RootPerfMonSvc RootPerfMonSvc.h src/RootPerfMonSvc.h
   *
   * Description:
   * 
   * RootPerfMonSvc class implementation definition.
   *
   *  @author  Markus Frank
   *  @version 1.0
   *  @date    20/12/2009
   */
  class GAUDI_API RootPerfMonSvc 
    : public Service, virtual public IIncidentListener 
  {
  protected:

    typedef std::map<std::string,TTreePerfStats*> RootTreeStats; 
      
    /// Property: Enable TTree IOperfStats if not empty; otherwise perf stat file name
    std::string     m_ioPerfStats;
    /// Property: output file name for the collected counters and I/O tree stats
    std::string     m_outputName;
    /// Property: Counter collection interval
    int             m_collectInterval;

    /// Reference to incident service
    IIncidentSvc*   m_incidentSvc;
    /// Reference to the I/O data manager
    IIODataManager* m_ioMgr;
    /// Message streamer
    MsgStream*      m_log;
    TFile*          m_output;
    TTree*          m_countersTree;
    TBranch*        m_counters;
    TDirectory*     m_treeStatDir;
    /// Map of TTreePerfStats for all input files
    RootTreeStats   m_rdFiles;
    /// Local event counter
    int             m_evtCount;
    struct timeval  m_startTime;

    PerfMonCounters*   m_beginEvent;
    PerfMonCounters*   m_endEvent;
    PerfMonCounters*   m_beginProc;
    PerfMonCounters*   m_endProc;
    PerfMonCounters*   m_beginInput;
    PerfMonCounters*   m_endInput;

    /// Helper: Use message streamer
    MsgStream& log() const {   return *m_log; }

    /** Standard way to print errors. after the printout an exception is thrown.
     * @param      msg      [IN]     Message string to be printed.
     *
     * @return     Status code returning failure.
     */
    StatusCode error(const std::string& msg);

    /// Save TTree I/O statistics to output file for given dataset
    void saveTreeStatistics(const std::string& dataset);
    /// Save TTree I/O statistics to output file
    void saveTreeStatistics(TTreePerfStats* s);
    /// Create TTree I/O statistics to output file for given dataset
    void createTreeStatistics(const std::string& dataset);
    /// Collect system counters following a given incident
    void collectCounters(const Incident& incident, PerfMonCounters& counters);

  public:

    /// Standard constructor
    RootPerfMonSvc(const std::string& name, ISvcLocator* svc);

    /// Standard destructor
    virtual ~RootPerfMonSvc();

    /// IInterface implementation : queryInterface
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppIf);

    /// Incident handler implemenentation: Inform that a new incident has occured
    virtual void handle(const Incident& inc);

    /// Service overload: initialize service
    virtual StatusCode initialize();

    /// Service overload: Finalize service
    virtual StatusCode finalize();
  };
}

#endif  // GAUDIROOTCNV_GAUDIROOTCNVSVC_H
