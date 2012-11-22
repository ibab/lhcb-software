// $Id: EventTuple.h,v 1.2 2009-02-11 18:02:34 pkoppenb Exp $
#ifndef EVENTTUPLE_H
#define EVENTTUPLE_H 1

#include "GaudiAlg/GaudiTupleAlg.h"

/** @class EventTuple EventTuple.h
 *
 *  \brief Fill event-related variables only
 *
 * \verbatim
 statseq = GaudiSequencer("StatisticsSeq")
 tuple = EventTuple("TriggerTuple")
 statseq.Members += [tuple ]
 tuple.ToolList = [ "TupleToolTrigger", "TupleToolEventInfo", "TupleToolGeneration" ]\endverbatim
 *
 * You can also write an event tag collection
 *
 * \verbatim
 tag = EventTuple("TagCreator")
 tag.EvtColsProduce = True
 tag.ToolList = [ "TupleToolEventInfo", "TupleToolRecoStats"  ]
 DaVinci().ETCFile = "DVPresel_ETC2.root"
 \endverbatim
 *
 *
 *
 *  \sa DecayTreeTuple
 *
 *  @author Patrick Koppenburg
 *  @date   2008-07-01
 */
class EventTuple : public GaudiTupleAlg
{

public:

  /// Standard constructor
  EventTuple( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~EventTuple( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  std::vector<std::string> m_toolList ; ///< names of tools to be used
  std::vector<IEventTupleTool*> m_tools ; ///< tools to be filled
  std::string m_tupleName; ///< name of Tuple
  /// ETC name
  std::string m_collectionName;

};

#endif // EVENTTUPLE_H
