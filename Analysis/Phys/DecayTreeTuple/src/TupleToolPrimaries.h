// $Id: TupleToolPrimaries.h,v 1.1.1.1 2007-12-12 17:46:43 pkoppenb Exp $
#ifndef JBOREL_TUPLETOOLEVENTINFO_H
#define JBOREL_TUPLETOOLEVENTINFO_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IEventTupleTool.h"            // Interface

class IContextTool;

/** @class TupleToolPrimaries TupleToolPrimaries.h jborel/TupleToolPrimaries.h
 *
 * \brief Event and Run number for DecayTreeTuple
 *
 * Tuple columns:
 * - runNumber: well, the run number
 * - eventNumber: 
 *
 * \sa DecayTreeTuple
 *  @author Jeremie Borel
 *  @date   2007-11-07
 */
class TupleToolPrimaries : public GaudiTool, virtual public IEventTupleTool {
public:
  /// Standard constructor
  TupleToolPrimaries( const std::string& type,
		      const std::string& name,
		      const IInterface* parent);

  virtual ~TupleToolPrimaries( ){}; ///< Destructor
  virtual StatusCode initialize();

  virtual StatusCode fill( Tuples::Tuple& );

private:
  IContextTool* m_context;
};
#endif // JBOREL_TUPLETOOLEVENTINFO_H
