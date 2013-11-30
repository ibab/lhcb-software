// $Id: HltDecReportsWriter.h,v 1.1.1.1 2009-06-24 15:38:52 tskwarni Exp $
#ifndef HLTDECREPORTSWRITER_H 
#define HLTDECREPORTSWRITER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/HltDecReport.h"

/** @class HltDecReportsWriter HltDecReportsWriter.h
 *  
 *
 *  @author Tomasz Skwarnicki
 *  @date   2008-07-26
 *
 *  Algorithm to convert HltDecReports container on TES to HLT Raw Bank
 *
 */
class HltDecReportsWriter : public GaudiAlgorithm {
public: 

  enum HeaderIDs { kVersionNumber=2 };


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
  HltDecReportsWriter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltDecReportsWriter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution


private:

  /// location of input
  StringProperty m_inputHltDecReportsLocation;

  /// location of output
  StringProperty m_outputRawEventLocation;

  /// SourceID to insert in the bank header
  UnsignedIntegerProperty m_sourceID;

};
#endif // HLTDECREPORTSWRITER_H
