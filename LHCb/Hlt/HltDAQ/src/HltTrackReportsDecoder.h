// $Id$
#ifndef HLTTRACKREPORTSDECODER_H 
#define HLTTRACKREPORTSDECODER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "DAQKernel/DecoderAlgBase.h"

/** @class HltTrackReportsDecoder HltTrackReportsDecoder.h
 *  
 *
 *  @author Sebastian Neubert
 *  @date   2014-01-16
 *
 *  Algorithm to read HltTracks from Raw Data and create containers on TES
 *
 */
class HltTrackReportsDecoder : public Decoder::AlgBase {
public: 

  enum HeaderIDs { kVersionNumber=1 };

  /// Standard constructor
  HltTrackReportsDecoder( const std::string& name, ISvcLocator* pSvcLocator );

  StatusCode initialize() override;    ///< Algorithm initialization
  StatusCode execute   () override;    ///< Algorithm execution

private:

  /// location of output
  StringProperty m_outputLocation;
  
  /// location of HltTrackReports 
  StringProperty m_HltTrackReportsLocation;

  /// SourceID to decode. source ids are linked to track stages in TrackNames.trackingSources 
  IntegerProperty m_sourceID;

  unsigned int m_callcount;

};

#endif // HLTTRACKREPORTSDECODER_H
