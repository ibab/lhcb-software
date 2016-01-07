// $Id: FilterByRunEvent.h,v 1.4 2008-12-09 13:17:31 spradlin Exp $
#ifndef FILTERBYRUNEVENT_H 
#define FILTERBYRUNEVENT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include <vector>
#include <utility>

/** @class FilterByRunEvent FilterByRunEvent.h
 *   
 *
 *  @author Patrick Spradlin
 *  @date   2007-01-18
 *
 *  @par Algorithm purpose
 *    @c FilterByRunEvent filters events based on the
 *    (run number, event number) pair of the event.
 *
 *  @par Algorithm properties
 *    @c FilterByRunEvent has two properties:
 *    - @c PassSelectedEvents:  A boolean with a default value of @c true.
 *      If @c PassSelectedEvents is set to @c true (the default),
 *      @c FilterByRunEvent will pass only the specified events and reject all
 *      other events. If @c PassSelectedEvents is set to @c false,
 *      @c FilterByRunEvent will reject the specified events and pass all
 *      other events.
 *    - @c RunEventNumList:  A vector of (run number, event number) pairs.
 *      By default, @c RunEventNumList is empty.  In python configurables,
 *      @c RunEventNumList is specified with a list of pairs (tuples of
 *      length 2).
 *
 *    The default configuration of @c FilterByRunEvent is a fail-all filter.
 *
 *  @par Example of usage
 * 
 *    In order to configure a sequencer @c MySequence to skip the two events
 *    with (run number, event number) pairs (86456, 421) and (48621, 3):
 *    @verbatim
.....
MySequence = GaudiSequencer('MySequence')
MyFilterByRunEvent = FilterByRunEvent('MyFilterByRunEvent')
MySequence.Members += [ MyFilterByRunEvent, .... ]

# Set PassSelectedEvents to false, so that it rejects the selected events
MyFilterByRunEvent.PassSelectedEvents = 0
# std::vector< std::pair<int, int> > configured with a list of pairs
MyFilterByRunEvent.RunEventNumList = [ (86456, 421), (48621, 3) ]
.... @endverbatim
 */
class FilterByRunEvent : public GaudiAlgorithm {
public: 
  /// Standard constructor
  FilterByRunEvent( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~FilterByRunEvent( ); ///< Destructor

  virtual StatusCode execute   ();    ///< Algorithm execution

protected:

private:

  std::vector<std::pair<int,int> > m_events; ///< Run/event number pairs
  //FIXME: needs to be: std::vector<std::pair<unsigned int,unsigned long long> > m_events; ///< Run/event number pairs
  bool m_passSelect;		///< If true, will pass list events; false, fail

};
#endif // FILTERBYRUNEVENT_H
