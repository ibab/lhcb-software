// $Id: RunChangeTest.h,v 1.1 2008-07-24 19:18:59 marcocle Exp $
#ifndef COMPONENT_RUNCHANGETEST_H 
#define COMPONENT_RUNCHANGETEST_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h" 
#include "GaudiKernel/IIncidentListener.h"

class IEventTimeDecoder;
class IIncidentSvc;

namespace DAQEventTests {

/** @class RunChangeTest RunChangeTest.h component/RunChangeTest.h
 *  
 *
 *  @author Marco CLEMENCIC
 *  @date   2008-07-24
 */
class RunChangeTest : public GaudiAlgorithm, public IIncidentListener {
public: 
  /// Standard constructor
  RunChangeTest( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RunChangeTest( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
  /// Handle the ChangeRun incident
  virtual void handle(const Incident&incident);
  
protected:

private:

  unsigned int m_runNumber;
  IEventTimeDecoder *m_eventTimeDecoder;
  IIncidentSvc *m_incSvc;
};

}
#endif // COMPONENT_RUNCHANGETEST_H
