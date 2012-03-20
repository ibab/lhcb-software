// $Id: $
#ifndef STACTIVEFRACTION_H 
#define STACTIVEFRACTION_H 1

// Include files
// from Gaudi
#include "Kernel/STHistoAlgBase.h"

#include<vector> 

/** @class STActiveFraction STActiveFraction.h
 *  
 *  Class to work out the active fraction of the detector from the
 *  conditions database as a function of time.  This makes use of the
 *  FakeEventTime tool in the EventClockSvc.  The start and step time
 *  used in the tool are required inputs for this algorithm.
 *  The input options require a start time and step size given in 
 *  ns since the epoch and the total number of steps required
 *
 *  An example python scripts is available in options.
 *  Usage: gaudirun.py $STCHECKERSROOT/options/runSTActiveTrend.py
 *
 *  @author Mark Tobin
 *  @date   2011-07-20
 */
namespace ST {
  class STActiveFraction : public HistoAlgBase {
  public: 
    /// Standard constructor
    STActiveFraction( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~STActiveFraction( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization

  protected:
    
  private:
    unsigned int m_event;
    unsigned int m_nSteps;
    long long m_startTime;
    long long m_timeStep;
    std::vector<double> m_time;
    std::vector<double> m_active;

  };
}
#endif // STACTIVEFRACTION_H
