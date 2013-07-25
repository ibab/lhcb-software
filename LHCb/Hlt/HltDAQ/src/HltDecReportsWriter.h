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

  enum HeaderIDs { kSourceID=0,
                   kVersionNumber=1 
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


};
#endif // HLTDECREPORTSWRITER_H
