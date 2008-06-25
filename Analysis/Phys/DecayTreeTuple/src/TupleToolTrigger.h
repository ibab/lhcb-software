// $Id: TupleToolTrigger.h,v 1.4 2008-06-25 12:18:26 pkoppenb Exp $
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
class IHltSummaryTool;
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
  StatusCode fillHlt( Tuples::Tuple& );
  StatusCode fillHlt( Tuples::Tuple&, const std::string & );
  
  bool m_fillHlt;
  bool m_fillL0;
  bool m_verboseL0;     ///< get details on L0
  bool m_verboseHlt1; ///< get details on alleys
  bool m_verboseHlt2; ///< get details on selections
  IHltSummaryTool* m_summaryTool ; ///< HLT summary tool
  std::string m_hlt1MajorKey ; ///< Major key of Hlt1 in HltANNSvc
  std::string m_hlt2MajorKey ; ///< Major key of Hlt2 in HltANNSvc
  
  
};
#endif // JBOREL_TUPLETOOLTRIGGER_H
