// $Id: PropagateFSRs.h,v 1.1 2010-03-26 11:43:18 rlambert Exp $
#ifndef PROPAGATEFSRS_H 
#define PROPAGATEFSRS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiUtils/IIODataManager.h"

// event model
#include "Event/HltLumiSummary.h"
#include "Event/LumiFSR.h"


/** @class PropagateFSRs PropagateFSRs.h
 *  
 *
 *  @author Rob LAmbert
 *  @date   2010-03-26
 */
class PropagateFSRs : public GaudiAlgorithm 
  , public virtual IIncidentListener {
public: 
  /// Standard constructor
  PropagateFSRs( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PropagateFSRs( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  
  // ==========================================================================
  // IIncindentListener interface
  // ==========================================================================
  virtual void handle ( const Incident& ) ;
  // ==========================================================================
  
protected:
  /// Reference to file records data service
  IDataProviderSvc* m_fileRecordSvc;
  std::string m_rawEventLocation;     // Location where we get the RawEvent

private:
  mutable IIncidentSvc* m_incSvc ;                      /// the incident service 
  
  //copied from XMLSummarySvc
  std::string m_filename; ///the current open filename
  
  Gaudi::IIODataManager * m_ioDataManager; ///the pointer to the data manager service
  
  std::string file2GUID(const std::string & filename);
  std::string AFN2name(const std::string & filename) const;
  std::string GUIDinEvent() const;
  void AddName2GUID(const std::string & filename, const std::string & GUID);
  
  /// Map of FID to PFN
  typedef std::map<std::string, std::string> FidMap;
  /// Map of FID to PFN
  std::map<std::string, std::string>   m_fidMap;
  /// Map of PFN to whether the GUID is known
  typedef std::map<std::string, bool> KnowGUIDMap;
  /// Map of  PFN to whether the GUID is known
  std::map<std::string, bool>   m_knownFiles;

};
#endif // LUMIACCOUNTING_H
