// $Id: HltDecReportsWriter.h,v 1.3 2009-04-18 18:52:37 graven Exp $
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
                   kVersionNumber=0 
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
