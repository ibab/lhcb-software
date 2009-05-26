// $Id: HltDecReportsReader.h,v 1.3 2009-05-26 20:06:10 graven Exp $
#ifndef HLTDECREPORTSREADER_H 
#define HLTDECREPORTSREADER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Event/HltDecReport.h"

#include "Kernel/IANNSvc.h"


/** @class HltDecReportsReader HltDecReportsReader.h
 *  
 *
 *  @author Tomasz Skwarnicki
 *  @date   2008-08-02
 *
 *  Algorithm to read HltDecReports from Raw Data and create container on TES
 *
 */
class HltDecReportsReader : public GaudiAlgorithm {
public: 
  /// Standard constructor
  HltDecReportsReader( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltDecReportsReader( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  enum HeaderIDs { kSourceID=0,
                   kVersionNumber=1 
  };

  /// location of input H
  StringProperty m_outputHltDecReportsLocation;

  /// location of input
  StringProperty m_inputRawEventLocation;

  /// HltANNSvc for making selection names to int selection ID
  IANNSvc* m_hltANNSvc;

};
#endif // HLTDECREPORTSREADER_H
