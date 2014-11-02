// $Id: HltTrackReportsWriter.h $
#ifndef HLTTRACKREPORTSWRITER_H 
#define HLTTRACKREPORTSWRITER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

namespace LHCb {
class RawEvent;
}
/** @class HltTrackReportsWriter HltTrackReportsWriter.h
 *  
 *
 *  @author Sebastian Neubert
 *  @date   2014-01-10
 *
 *  Algorithm to convert Track containers on TES to HLT Raw Bank
 *
 *  Encoding of track information into RawBank
 *  For each track:
 *  (nLhHCbIDs, ID0, ID1, ... IDn)
 *   
 *  tracks are just concatenated
 *
 */
class HltTrackReportsWriter : public GaudiAlgorithm {
public: 

  enum HeaderIDs { kVersionNumber=1 };


  enum SourceIDs { kSourceID_Dummy=0,
		   kSourceID_Hlt=kSourceID_Dummy,
		   kSourceID_Hlt1_Velo = 1,
		   kSourceID_Hlt1_VeloTT = 2,
		   kSourceID_Hlt1_Forward = 3,
		   kSourceID_Hlt1_ForwardFitted = 5,
		   kSourceID_Hlt1_ForwardHPT = 4,
		   kSourceID_Max=7,
		   kSourceID_BitShift=11,         // will need bitshifting to distinguish minor and major sourceIDs 
		   kSourceID_MinorMask=0x7FF,
		   kSourceID_MajorMask=0x3800
  };


  /// Standard constructor
  HltTrackReportsWriter( const std::string& name, ISvcLocator* pSvcLocator );

  ~HltTrackReportsWriter( ) override = default; ///< Destructor

  StatusCode initialize() override;    ///< Algorithm initialization
  StatusCode execute   () override;    ///< Algorithm execution


private:
  void convert(const std::string& location, unsigned sourceID, LHCb::RawEvent* rawEvent) const ;

  /// mapping of input TES location to output bank header source ID
  std::map<std::string,int> m_map;

  /// location of output
  StringProperty m_outputRawEventLocation;

  unsigned int m_callcount;

};
#endif // HLTDECREPORTSWRITER_H
