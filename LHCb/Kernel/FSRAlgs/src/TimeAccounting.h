// $Id: TimeAccounting.h,v 1.1 2010-01-04 16:31:22 panmanj Exp $
#ifndef TIMEACCOUNTING_H 
#define TIMEACCOUNTING_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// event model
#include "Event/TimeSpanFSR.h"


/** @class TimeAccounting TimeAccounting.h
 *  
 *
 *  @author Jaap Panman
 *  @date   2009-01-19
 */
class TimeAccounting : public GaudiAlgorithm {
public: 
  /// Standard constructor
  TimeAccounting( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TimeAccounting( );         ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  /// Reference to file records data service
  IDataProviderSvc* m_fileRecordSvc;

  std::string m_rawEventLocation;     // Location where we get the RawEvent
  std::string m_DataName;             // input location of summary data
  std::string m_FSRName;              // output location of summary data in FSR

  LHCb::TimeSpanFSRs* m_timeSpanFSRs; // TDS container
  LHCb::TimeSpanFSR* m_timeSpanFSR;   // FSR for current file

  std::string m_current_fname;        // current file ID string 
  int         m_count_files;          // number of files read
private:

};
#endif // TIMEACCOUNTING_H
