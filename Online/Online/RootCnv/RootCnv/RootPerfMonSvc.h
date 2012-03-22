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
   *  @author  Markus Frank
   *  @version 1.0
   *  @date    20/12/2009
   */
  class GAUDI_API RootPerfMonSvc : public Service  {
  protected:

    /// Reference to incident service
    IIncidentSvc*               m_incidentSvc;
    /// Property: Enable TTree IOperfStats if not empty; otherwise perf stat file name
    std::string                 m_ioPerfStats;
    /// Message streamer
    MsgStream*                  m_log;

    /// Helper: Use message streamer
    MsgStream& log() const {   return *m_log; }

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

    /// ConversionSvc overload: initialize Db service
    virtual StatusCode initialize();

    /// ConversionSvc overload: Finalize Db service
    virtual StatusCode finalize();
  };
}

#endif  // GAUDIROOTCNV_GAUDIROOTCNVSVC_H
