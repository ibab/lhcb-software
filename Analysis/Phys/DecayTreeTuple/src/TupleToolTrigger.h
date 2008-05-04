// $Id: TupleToolTrigger.h,v 1.3 2008-05-04 18:53:49 gligorov Exp $
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
  
  bool m_fillHLT;
  bool m_fillL0;
  bool m_verboseL0;     ///< get details on L0
  bool m_verboseAlleys; ///< get details on alleys
  bool m_verboseSelections; ///< get details on selections
  IHltSummaryTool* m_summaryTool ; ///< HLT summary tool
  
};
#endif // JBOREL_TUPLETOOLTRIGGER_H
