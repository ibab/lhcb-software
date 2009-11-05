// $Id: TupleToolTrigger.h,v 1.9 2009-11-05 18:14:45 pkoppenb Exp $
#ifndef JBOREL_TUPLETOOLTRIGGER_H
#define JBOREL_TUPLETOOLTRIGGER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IEventTupleTool.h"            // Interface

/** @class TupleToolTrigger TupleToolTrigger.h jborel/TupleToolTrigger.h
 *
 * \brief Fill the trigger informations for the DecayTreeTuple.
 *
 * - L0Decision : LHCb::L0DUReport->decision()
 *
 * If \b VerboseL0 = true
 * L0Decision_xxx : LHCb::L0DUReport->channelDecisionByName(xxx)
 *
 *  @author Jeremie Borel
 *  @date   2007-11-07
 */
class TupleToolTrigger : public GaudiTool, virtual public IEventTupleTool {
public:
  /// Standard constructor
  TupleToolTrigger( const std::string& type,
		      const std::string& name,
		      const IInterface* parent);

  virtual ~TupleToolTrigger( ){}; ///< Destructor

  StatusCode initialize() ;

  StatusCode fill( Tuples::Tuple& );

private:

  StatusCode fillL0( Tuples::Tuple& );
  StatusCode fillHlt( Tuples::Tuple&, const std::string &, bool );
  StatusCode fillRoutingBits( Tuples::Tuple& );
 
  bool m_fillHlt;
  bool m_fillL0;
  bool m_verboseL0;     ///< get details on L0
  bool m_verboseHlt1; ///< get details on Hlt1
  bool m_verboseHlt2; ///< get details on Hlt2
  //  std::string m_hlt1MajorKey ; ///< Major key of Hlt1 in HltANNSvc
  //  std::string m_hlt2MajorKey ; ///< Major key of Hlt2 in HltANNSvc
  bool  m_allSteps ; ///< Fill also intermediate steps
  bool m_fillGlobal ; ///< Fill Hlt1Global and Hlt2Global (backward-compatible mode)
  std::vector<unsigned int> m_routingBits ; ///< Routing bits to fill
  
};
#endif // JBOREL_TUPLETOOLTRIGGER_H
