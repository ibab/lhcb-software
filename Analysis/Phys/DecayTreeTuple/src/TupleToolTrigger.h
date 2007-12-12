// $Id: TupleToolTrigger.h,v 1.1.1.1 2007-12-12 17:46:43 pkoppenb Exp $
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

  StatusCode fill( Tuples::Tuple& );

private:
  bool m_verboseL0;
};
#endif // JBOREL_TUPLETOOLTRIGGER_H
