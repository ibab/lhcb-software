// $Id: HltTrackReportsDecoder.h,v 1.1.1.1 2009-06-24 15:38:52 tskwarni Exp $
#ifndef HLTTRACKREPORTSDECODER_H 
#define HLTTRACKREPORTSDECODER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

//#include "Kernel/IANNSvc.h"


/** @class HltTrackReportsDecoder HltTrackReportsDecoder.h
 *  
 *
 *  @author Sebastian Neubert
 *  @date   2014-01-16
 *
 *  Algorithm to read HltTracks from Raw Data and create containers on TES
 *
 */
class HltTrackReportsDecoder : public GaudiAlgorithm {
public: 

  enum HeaderIDs { kVersionNumber=1 };

  /// Standard constructor
  HltTrackReportsDecoder( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltTrackReportsDecoder( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  static float floatFromInt(unsigned int i);

private:

  /// location of output
  StringProperty m_outputHltTrackLocation;

  /// location of input
  std::string m_inputRawEventLocation;
  std::vector<std::string> m_rawEventLocations;

  /// location of HltTrackReports (will not be used unless ANNSvc not working) 
  StringProperty m_HltTrackReportsLocation;

  /// HltANNSvc for making selection names to int selection ID
  //IANNSvc* m_hltANNSvc;

  /// SourceID to decode 0=Hlt 1=Hlt1 2=Hlt2 ... (1,2 will decode from 0 if 1,2 not found)
  UnsignedIntegerProperty m_sourceID;

};

#endif // HLTTRACKREPORTSDECODER_H
