// $Id: HltLumiSummaryDecoder.h,v 1.1.1.1 2009-02-16 16:04:17 panmanj Exp $
#ifndef HLTLUMISUMMARYDECODER_H
#define HLTLUMISUMMARYDECODER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/RawEvent.h"
#include "Event/HltLumiSummary.h"

// forward declarations
namespace LHCb {
  class RawBank ;
}

/** @class HltLumiSummaryDecoder HltLumiSummaryDecoder.h
 *
 *  Decodes the LumiSummary. 
 *
 *  @author Jaap Panman
 *
 *  @date   2008-08-01
 */

class HltLumiSummaryDecoder : public GaudiAlgorithm {
public: 
  /// Standard constructor
  HltLumiSummaryDecoder(  const std::string& name, ISvcLocator* pSvcLocator );
  
  /// Destructor
  virtual ~HltLumiSummaryDecoder( ) ; ///< Destructor
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
protected:
  
private:
  // data
  std::string m_rawEventLocation;           ///< Location where we get the RawEvent
  std::string m_OutputContainerName;

  // Statistics  
  double m_totDataSize;
  int m_nbEvents;
  LHCb::RawBank::BankType m_bankType;  
  LHCb::HltLumiSummary* m_HltLumiSummary;

};

//////////////////////////////////////////////////////////////////////////////////////////
#endif // HLTLUMISUMMARYDECODER_H
