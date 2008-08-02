// $Id: HltDecReportsReader.h,v 1.1.1.1 2008-08-02 16:40:07 tskwarni Exp $
#ifndef HLTDECREPORTSREADER_H 
#define HLTDECREPORTSREADER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
//#include "HltBase/HltBaseAlg.h"
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
//class HltDecReportsReader : public HltBaseAlg {
public: 

  enum HeaderIDs { kSourceID=0,
                   kVersionNumber=0 
  };

  /// Standard constructor
  HltDecReportsReader( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltDecReportsReader( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


protected:

  /// location of input H
  StringProperty m_outputHltDecReportsLocation;

  /// location of input
  StringProperty m_inputRawEventLocation;

  /// HltANNSvc for making selection names to int selection ID
  IANNSvc* m_hltANNSvc;

private:
  

};
#endif // HLTDECREPORTSREADER_H
