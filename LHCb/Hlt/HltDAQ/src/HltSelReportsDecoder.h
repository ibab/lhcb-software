// $Id: HltSelReportsDecoder.h,v 1.1.1.1 2009-06-24 15:38:52 tskwarni Exp $
#ifndef HLTSELREPORTSDECODER_H 
#define HLTSELREPORTSDECODER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Kernel/IANNSvc.h"


/** @class HltSelReportsDecoder HltSelReportsDecoder.h
 *  
 *
 *  @author Tomasz Skwarnicki
 *  @date   2008-08-02
 *
 *  Algorithm to read HltSelReports from Raw Data and create containers on TES
 *
 */
class HltSelReportsDecoder : public GaudiAlgorithm {
public: 

  enum HeaderIDs { kVersionNumber=2 };

  /// Standard constructor
  HltSelReportsDecoder( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltSelReportsDecoder( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  static float floatFromInt(unsigned int i);

private:

  /// location of output
  StringProperty m_outputHltSelReportsLocation;

  /// location of input
  std::string m_inputRawEventLocation;
  std::vector<std::string> m_rawEventLocations;

  /// location of HltDecReports (will not be used unless ANNSvc not working) 
  StringProperty m_HltDecReportsLocation;

  /// HltANNSvc for making selection names to int selection ID
  IANNSvc* m_hltANNSvc;

  /// SourceID to decode 0=Hlt 1=Hlt1 2=Hlt2 ... (1,2 will decode from 0 if 1,2 not found)
  UnsignedIntegerProperty m_sourceID;

};

#endif // HLTSELREPORTSDECODER_H
