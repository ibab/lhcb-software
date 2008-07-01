// $Id: EventTuple.h,v 1.1 2008-07-01 14:50:02 pkoppenb Exp $
#ifndef EVENTTUPLE_H 
#define EVENTTUPLE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"


/** @class EventTuple EventTuple.h
 *  
 *  \brief Fill event-related variables only
 *
 *  * \verbatim
statseq = GaudiSequencer("StatisticsSeq")
tuple = EventTuple("TriggerTuple")
statseq.Members += [tuple ]
tuple.ToolList = [ "TupleToolTrigger", "TupleToolEventInfo", "TupleToolGeneration" ]
 * 
 *  \sa DecayTreeTuple
 *
 *  @author Patrick Koppenburg
 *  @date   2008-07-01
 */
class EventTuple : public GaudiTupleAlg {
public: 
  /// Standard constructor
  EventTuple( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~EventTuple( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::vector<std::string> m_toolList ; ///< names of tools to be used
  std::vector<IEventTupleTool*> m_tools ; ///< tools to be filled
  std::string m_tupleName; ///< name of Tuple
};
#endif // EVENTTUPLE_H
