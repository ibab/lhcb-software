
#ifndef GAUDISVC_IOFSRSVC_H
#define GAUDISVC_IOFSRSVC_H

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/StatEntity.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/AppReturnCode.h"
#include "GaudiUtils/IFileCatalog.h"
#include "IIOFSRSvc.h"

#include "FSRNavigator.h"

#include "Event/IOFSR.h"
#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>
#include <utility>

namespace Gaudi
{  
  class IIODataManager;
}

template <class TYPE> class SvcFactory;


/** @class IOFSRSvc IOFSRSvc.h
 *  
 *  Service polling information for placing in the IOFSR.
 *
 *  @author Robert Lambert
 *  @date   2012-09-27
 */
class IOFSRSvc
  : public extends1<Service,IIOFSRSvc>
  , public virtual IIncidentListener {

public:


  /// Map of FID to PFN
  typedef std::map<std::string, std::string> FidMap;
  

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual StatusCode stop();
  
  

  IOFSRSvc( const std::string& name, ISvcLocator* svc );

  // ==========================================================================
  // IIncindentListener interface
  // ==========================================================================
  virtual void handle ( const Incident& ) ;
  // ==========================================================================

  // Destructor.
  virtual ~IOFSRSvc();
  
  // Return map of input GUID : #events for all GUIDs, events read in __creating__ those files
  // For provenenance files only, since I don't necessarily know the top file's GUID
  LHCb::IOFSR::FileEventMap eventsSeenBy() { return m_seenByMap; };
  
  // Return map of input GUID : #events for all GUIDs, events read __from__ those files
  LHCb::IOFSR::FileEventMap eventsReadFrom();
  
  // Return map of input GUID : #events for all GUIDs, events written to those files
  // Only for provenenance files, since I don't necessarily know the top file's GUID
  LHCb::IOFSR::FileEventMap eventsWrittenTo() { return m_writtenToMap; };
  
  // Return map of output GUID/filename : #events for the direct ouputs of this job
  LHCb::IOFSR::FileEventMap jobOutputMap() { return m_jobOutputMap; };
  
  // Return map of output GUID : input GUIDs
  // provenance information only, the top file GUID is not necessarily known
  LHCb::IOFSR::ProvenanceMap provenanceMap() { return m_provenanceMap; };
  
  // Return list of GUIDs as input for this job
  std::vector< std::string > parents();
  
  // Return list of GUIDs as input for this job
  ulonglong eventsSeen() {return m_eventCount; };
  
  // Return list of GUIDs as input for this job, and events read from them
  LHCb::IOFSR::FileEventMap jobInputMap();
  
  
  // return number of events output into a given file in this job
  ulonglong jobOutput(const std::string & PFN);
  
  
  //merge existing IOFSRs into the maps of this service
  virtual StatusCode mergeIOFSRs();
  
  //remove any IOFSR at the top level. To be called before requesting a new FSR.
  virtual StatusCode cleanTopFSR();
  
  //Create a new IOFSR, pass it back so the requesting alg can store as needed
  LHCb::IOFSR* buildIOFSR(const std::string & outputName);
  
  //determine event count reliability, following down the tree 
  //of input/output until something doesn't match
  LHCb::IOFSR::StatusFlag traceCountsFlag();
  
  
  //Create a new IOFSR, store in the TES
  virtual StatusCode storeIOFSR(const std::string & outputName);
  //Does the accounting all add up OK?
  virtual bool traceCounts();
  
  //print contained information
  virtual void print();
  
  
private:

  /// Allow SvcFactory to instantiate the service.
  friend class SvcFactory<IOFSRSvc>;
  //IIncidentSvc* m_incSvc; ///the incident service
  IIncidentSvc* m_incSvc;
  Gaudi::IIODataManager * m_ioDataManager; ///the pointer to the data manager service
  ///type of incident to regard as begin event
  std::string m_beginIncident;
  ///type of incident to regard as end event
  std::string m_endIncident;
  
  StatEntity m_handled; ///simple counter of #handled events
  
  
  std::string m_filename; ///the current open filename for reading
  
  
  ///has at least one input file been opened in this job?
  bool m_hasinput;
  
  // Map of input GUID : #events for all GUIDs, events read in creating those files
  // Only for provenenance files, since I don't necessarily know the top GUID
  // not filled except for when merging is requested
  LHCb::IOFSR::FileEventMap   m_seenByMap;
  
  // Map of input GUID : #events for all GUIDs, events read in creating those files
  // For all daughter files, since I must know the GUID if the file has been opened
  // not filled except for when merging is requested
  LHCb::IOFSR::FileEventMap   m_readFromMap;
  
  // Map of input GUID : #events for all GUIDs, events written to those files
  // Only for provenenance files, since I don't necessarily know the top file's GUID
  // not filled except when merging is requested
  LHCb::IOFSR::FileEventMap m_writtenToMap;
  
  // Map of output filename : #events for the direct ouputs of this job (filenames as filenames I hope...)
  LHCb::IOFSR::FileEventMap m_jobOutputMap;
  
  // List of Filenames as input for this job, with number of events read (filenames as filenames I hope...)
  LHCb::IOFSR::FileEventMap m_jobInputMap;
  
  // The provenance map, only filled during merging
  LHCb::IOFSR::ProvenanceMap m_provenanceMap;
  
  
  ///shorcut to prepare the incident service
  StatusCode prepareIncSvc();
  
  std::string file2GUID(const std::string & filename);
  std::string AFN2name(const std::string & filename) const;
  
  /// Map of FID to PFN and PFN -> FID in current job
  FidMap   m_fidMap;
  
  //total number of events seen so far.
  ulonglong m_eventCount;
  
  
  std::string m_FileRecordName;                 ///< location of FileRecords
  std::string m_FSRName;                        ///< specific tag of summary data in FSR
  
  std::string m_ToolName;                       ///< name of tool for navigation
  IFSRNavigator *m_navigatorTool;               ///< tool navigate FSRs
  /// Reference to run records data service
  IDataProviderSvc* m_fileRecordSvc;
  
  //find the last GUID in the object address
  std::string address2GUID(const std::string& address);
  
  LHCb::IOFSR::StatusFlag m_trace;
  bool m_merged;
  
  
  bool m_overrideStatus; ///override status at end of job with default. Set by OverrideStatus.
  std::string m_defaultStatusStr; ///status to start with if nothing else is known, Set by DefaultStatus
  LHCb::IOFSR::StatusFlag m_defaultStatus; ///status to start with if nothing else is known, cast from DefaultStatus
  
  bool m_printIOFSR; ///print the whole IOFSR data for inspection in finalize
  
  void dumpMap(LHCb::IOFSR::FileEventMap & map); //print contents of a map as info
  
  void dumpProvenance(const std::string file, const std::string buffer, const unsigned int rec=0); //print contents of a map as info
  
  
};

#endif
