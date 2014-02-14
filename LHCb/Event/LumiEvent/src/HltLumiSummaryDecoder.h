#ifndef HLTLUMISUMMARYDECODER_H
#define HLTLUMISUMMARYDECODER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "DAQKernel/DecoderAlgBase.h"
// std
#include <string>

/** @class HltLumiSummaryDecoder HltLumiSummaryDecoder.h
 *
 *  Decodes the LumiSummary. 
 *
 *  @author Jaap Panman
 *
 *  @date   2008-08-01
 */

class HltLumiSummaryDecoder : public Decoder::AlgBase {
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
  std::string m_OutputContainerName;

  // Statistics  
  double m_totDataSize;
  int m_nbEvents;

};

//////////////////////////////////////////////////////////////////////////////////////////
#endif // HLTLUMISUMMARYDECODER_H
