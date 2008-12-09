// $Id: FIDManipAlg.h,v 1.2 2008-12-09 20:16:10 frankb Exp $
// Include files 
//-----------------------------------------------------------------------------
// Implementation file for class : Fileidmanipulator
//
// 2008-03-03 : Albert Puig Navarro (albert.puig@cern.ch)
// 2008-11-20 : Markus Frank
//-----------------------------------------------------------------------------
#ifndef GAUDIONLINE_FIDMANIPALG_H
#define GAUDIONLINE_FIDMANIPALG_H

// Include files from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IIncidentListener.h"

// Forward declarations
class IIncidentSvc;

/*
 *   LHCb namespace
 */
namespace LHCb {

  // Forward declarations
  class RawBank;

  /** @class FIDManipAlg FIDManipAlg.h
  *  
  *  Note: This is a pure online algorithm and only works for MDF I/O.
  *        Don't even try to use it with POOL etc.
  *
  *  @author Albert Puig Navarro (albert.puig@cern.ch)
  *  @author M.Frank
  *  @date   2008-03-03
  */
  class FIDManipAlg : public Algorithm, virtual public IIncidentListener {
  public:
    enum Action { ADD=1, REMOVE, DUMP };
    /// Reference to incident service
    IIncidentSvc* m_incidentSvc;

  protected:
    /// Property: remove/add file id bank
    int          m_action;
    /// Property: Location of the raw event in the transient datastore
    std::string  m_location;
    /// Property: input data type
    int          m_type;
    /// Property: Debug flag
    int          m_debug;
    /// Property printout frequency
    int          m_printFreq;
    /// Print frequence counter
    int          m_printCnt;
    /// FID info (if retrieved using incident service
    std::pair<int,std::string> m_fid;
    /// Property: New file incident name
    std::string  m_incidentName;

  protected:
  public: 
    /// Standard constructor
    FIDManipAlg(const std::string& nam, ISvcLocator* pSvc);
    /// Standard destructor
    virtual ~FIDManipAlg()  {}
    // IInterface implementation : queryInterface
    virtual StatusCode queryInterface(const InterfaceID& riid, void** ppvInterface);
    /// Incident handler implemenentation: Inform that a new incident has occured
    virtual void handle(const Incident& inc);
    /// Algorithm initialization
    virtual StatusCode initialize();
    /// Algorithm finalization
    virtual StatusCode finalize();
    /// Algorithm execution
    virtual StatusCode execute();
  };
}
#endif // GAUDIONLINE_FIDMANIPALG_H
