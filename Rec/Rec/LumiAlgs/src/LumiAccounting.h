// $Id: LumiAccounting.h,v 1.2 2009-04-15 16:05:06 cattanem Exp $
#ifndef LUMIACCOUNTING_H 
#define LUMIACCOUNTING_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// event model
#include "Event/HltLumiSummary.h"
#include "Event/LumiFSR.h"


/** @class LumiAccounting LumiAccounting.h
 *  
 *
 *  @author Jaap Panman
 *  @date   2009-01-19
 */
class LumiAccounting : public GaudiAlgorithm {
public: 
  /// Standard constructor
  LumiAccounting( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~LumiAccounting( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  /// Reference to file records data service
  IDataProviderSvc* m_fileRecordSvc;

  std::string m_rawEventLocation;     // Location where we get the RawEvent
  std::string m_DataName;             // input location of summary data
  std::string m_FSRName;              // output location of summary data in FSR

  LHCb::LumiFSRs* m_lumiFSRs;         // TDS container
  LHCb::LumiFSR* m_lumiFSR;           // FSR for current file

  std::string m_current_fname;        // current file ID string 
  int         m_count_files;          // number of files read
private:

};
#endif // LUMIACCOUNTING_H
