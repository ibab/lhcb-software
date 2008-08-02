// $Id: HltDecReportsWriter.h,v 1.1.1.1 2008-08-02 16:40:07 tskwarni Exp $
#ifndef HLTDECREPORTSWRITER_H 
#define HLTDECREPORTSWRITER_H 1

// Include files
// from Gaudi
//#include "GaudiAlg/GaudiAlgorithm.h"
#include "HltBase/HltBaseAlg.h"
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
//class HltDecReportsWriter : public GaudiAlgorithm {
class HltDecReportsWriter : public HltBaseAlg {
public: 

  enum HeaderIDs { kSourceID=0,
                   kVersionNumber=0 
  };

  /// Standard constructor
  HltDecReportsWriter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltDecReportsWriter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


protected:

  /// location of input H
  StringProperty m_inputHltDecReportsLocation;

  /// location of output
  StringProperty m_outputRawEventLocation;

private:

  class UDless {
  public:
    bool operator() (unsigned int elem1, unsigned int elem2 )const;
  };

  //  static bool UDless( unsigned int elem1, unsigned int elem2 )
  //{
  //    return elem1 < elem2;
  //};
  

};
#endif // HLTDECREPORTSWRITER_H
