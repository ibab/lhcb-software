// $Id: LumiIntegrateFSR.h,v 1.5 2010-05-12 08:11:08 panmanj Exp $
#ifndef LUMIINTEGRATEFSR_H 
#define LUMIINTEGRATEFSR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/StatEntity.h"

// for incidents listener
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// event model
#include "Event/LumiFSR.h"
#include "Event/EventCountFSR.h"
#include "Event/TimeSpanFSR.h"
#include "Event/LumiIntegral.h"

// local
#include "LumiIntegrator.h"

/** @class LumiIntegrateFSR LumiIntegrateFSR.h
 *   
 *
 *  @author Jaap Panman
 *  @date   2009-02-27
 */
class LumiIntegrateFSR : public GaudiAlgorithm 
  , public virtual IIncidentListener {

public: 
  /// Standard constructor
  LumiIntegrateFSR( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~LumiIntegrateFSR( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  // ==========================================================================
  // IIncindentListener interface
  // ==========================================================================
  virtual void handle ( const Incident& ) ;
  // ==========================================================================

protected:
  virtual void add_file();            ///< add the FSRs of one input file
  virtual void add_fsr(LHCb::LumiIntegral* r, std::string addr, 
		       float factor, std::string tsaddr);  ///< add/subtr one FSR for consistent group
  virtual std::vector< std::string > navigate(std::string rootname, std::string tag); 
  virtual void explore(IRegistry* pObj, std::string tag, std::vector< std::string >& a);
  virtual void add_to_xml();          ///< add counters to xmlfile at Algorithm finalization

protected:
  /// Reference to run records data service
  IDataProviderSvc* m_fileRecordSvc;

  std::string m_rawEventLocation;               // Location where we get the RawEvent
  std::string m_FileRecordName;                 // location of FileRecords
  std::string m_FSRName;                        // specific tag of summary data in FSR
  std::string m_EventCountFSRName;              // specific tag of event summary data in FSR
  std::string m_TimeSpanFSRName;                // specific tag of event summary data in FSR
  std::string m_ToolName;                       // name of tool for normalization
  std::string m_PrimaryBXType;                  // BXType to normalize
  std::vector< std::string > m_BXTypes;         // list of bunch crossing types
  std::vector< std::string > m_addBXTypes;      // list of bunch crossing types to be added
  std::vector< std::string > m_subtractBXTypes; // list of bunch crossing types to be subtracted
  std::string m_current_fname;                  // current file ID string 
  int         m_count_files;                    // number of files read
  int         m_count_events;                   // number of events read
  int         m_events_in_file;                 // events after OpenFileIncident

private:
  ILumiIntegrator *m_integratorTool;            // tool to integrate luminosity
  mutable IIncidentSvc* m_incSvc ;              /// the incident service 


};
#endif // LUMIINTEGRATEFSR_H
