// $Id: $
#ifndef TIMINGTUPLE_H
#define TIMINGTUPLE_H 1

#include <algorithm>

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiKernel/Memory.h"
#include "GaudiAlg/ISequencerTimerTool.h"

#include "Event/ODIN.h"
#include "Event/RecSummary.h"

class ISequencerTimerTool ;

/** @class TimingTuple TimingTuple.h
 *
 *  Fill a Tuple with timing, memory and some event variables
 *
 *  @author Patrick Koppenburg
 *  @date   2010-08-18
 */
class TimingTuple : public GaudiTupleAlg 
{

public:

  /// Standard constructor
  TimingTuple( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TimingTuple( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  ISequencerTimerTool* m_timerTool; ///< timer tool
  int m_timer ; ///< timer index
  unsigned long long m_evtCounter;
  std::string m_recSummaryLoc;

private:

  template <class TYPE>
  void fillTuple(Tuple& tuple, const std::string& var, const TYPE number)
  {
    if (msgLevel(MSG::VERBOSE)) 
      verbose() << "Filling " << var << " with " << number << endmsg ;
    tuple->column( var , number );
  }

};
#endif // TIMINGTUPLE_H
