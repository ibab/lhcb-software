// $Id: RootPerfMonSvc.h,v 1.7 2010-09-14 06:01:12 frankb Exp $
//====================================================================
//	RootPerfMonSvc definition
//--------------------------------------------------------------------
//
//	Author     : M.Frank
//====================================================================
#ifndef GAUDIROOTCNV_GAUDIROOTPERFMONSVC_H
#define GAUDIROOTCNV_GAUDIROOTPERFMONSVC_H
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/RootCnv/src/RootPerfMonSvc.h,v 1.7 2010-09-14 06:01:12 frankb Exp $

// Framework include files
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/Service.h"

#include "TTree.h"
#include "TFile.h"

#include "RootCnv/SysProcStat.h"

// C/C++ include files
#include <set>

// Forward declarations
class IIncidentSvc;

/*
 * Gaudi namespace declaration
 */
namespace Gaudi {

  /** @class RootPerfMonSvc RootPerfMonSvc.h src/RootPerfMonSvc.h
   *
   * Description:
   *
   * RootPerfMonSvc class implementation definition.
   *
   *  @author  Markus Frank, Ivan Valencik
   *  @version 1.0
   *  @date    20/12/2009
   */
  class GAUDI_API RootPerfMonSvc : public Service, virtual public IIncidentListener {
  protected:

    /// Reference to incident service
    IIncidentSvc*               m_incidentSvc;
    /// Property: Enable TTree IOperfStats if not empty; otherwise perf stat file name
    std::string                 m_ioPerfStats;
    /// Message streamer
    MsgStream*                  m_log;
    // Passed parameters
    std::string                 m_setStreams;
    std::string                 m_basketSize;
    std::string                 m_bufferSize;
    std::string                 m_splitLevel;
    // Reference to a tree with statistics
    TTree*                      m_perfTree;
    // Reference to a file where statistics are persisted
    TFile*                      m_perfFile;
    // Reference to all connected output files
    std::set<std::string>       m_outputs;
    // The newest observed values
    Int_t                       m_eventType;
    ULong_t                     m_utime, m_stime, m_vsize;
    Long_t                      m_eventNumber, m_rss, m_time;

    // Types of records
    enum EventType { EVENT = 1, FSR = 2 };

    // Adding newest observed values to the tree with statistics
    virtual void record(EventType eventType);

    /// Helper: Use message streamer
    MsgStream& log() const { return *m_log; }

  public:
    /// Standard constructor
    RootPerfMonSvc(const std::string& name, ISvcLocator* svc);

    /// Standard destructor
    virtual ~RootPerfMonSvc();

    /** Standard way to print errors. after the printout an exception is thrown.
     * @param      msg      [IN]     Message string to be printed.
     *
     * @return     Status code returning failure.
     */
    StatusCode error(const std::string& msg);

    /// Service overload: initialize the service
    virtual StatusCode initialize();

    /// Service overload: Finalize the service
    virtual StatusCode finalize();

    /// IIncidentListener override: Inform that a new incident has occurred
    virtual void handle(const Incident& incident);

    // Service overload: Stop the service
    virtual StatusCode stop();
  };
}

#endif  // GAUDIROOTCNV_GAUDIROOTPERFMONSVC_H
