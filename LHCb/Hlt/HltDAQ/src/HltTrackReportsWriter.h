// $Id: HltTrackReportsWriter.h $
#ifndef HLTTRACKREPORTSWRITER_H 
#define HLTTRACKREPORTSWRITER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

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
		   kSourceID_Hlt1_ForwardFitted = 4,
		   kSourceID_Max=7,
		   kSourceID_BitShift=13,         // will need bitshifting to distinguish minor and major sourceIDs 
		   kSourceID_MinorMask=0x1FFF,
		   kSourceID_MajorMask=0xE000
  };


  /// Standard constructor
  HltTrackReportsWriter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltTrackReportsWriter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution


private:

  /// location of input
  StringProperty m_inputHltTrackLocation;

  /// location of output
  StringProperty m_outputRawEventLocation;

  /// SourceID to insert in the bank header
  UnsignedIntegerProperty m_sourceID;

  unsigned int m_callcount;

};
#endif // HLTDECREPORTSWRITER_H
