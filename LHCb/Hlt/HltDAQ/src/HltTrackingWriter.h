// $Id: HltTrackingWriter.h $
#ifndef HLTTRACKINGWRITER_H 
#define HLTTRACKINGWRITER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

/** @class HltTrackingWriter HltTrackingWriter.h
 *  
 *
 *  @author Sebastian Neubert
 *  @date   2014-01-10
 *
 *  Algorithm to convert HltTracking containers on TES to HLT Raw Bank
 *
 *  Encoding of track information into RawBank
 *  For each track:
 *  (nLhHCbIDs, ID0, ID1, ... IDn)
 *   
 *  tracks are just concatenated
 *
 */
class HltTrackingWriter : public GaudiAlgorithm {
public: 

  enum HeaderIDs { kVersionNumber=1 };


  enum SourceIDs { kSourceID_Dummy=0,
		   kSourceID_Hlt=kSourceID_Dummy,
		   kSourceID_Hlt1=1,
		   kSourceID_Hlt2=2,
		   kSourceID_Max=7,
		   kSourceID_BitShift=13,
		   kSourceID_MinorMask=0x1FFF,
		   kSourceID_MajorMask=0xE000
  };


  /// Standard constructor
  HltTrackingWriter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltTrackingWriter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution


private:

  /// location of input
  StringProperty m_inputHltTrackLocation;

  /// location of output
  StringProperty m_outputRawEventLocation;

  /// SourceID to insert in the bank header
  UnsignedIntegerProperty m_sourceID;

};
#endif // HLTDECREPORTSWRITER_H
