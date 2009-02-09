// $Id: TupleToolEventInfo.h,v 1.2 2009-02-09 09:16:55 pkoppenb Exp $
#ifndef JBOREL_TUPLETOOLEVENTINFO_H
#define JBOREL_TUPLETOOLEVENTINFO_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IEventTupleTool.h"            // Interface

class ITupleTool;

/** @class TupleToolEventInfo TupleToolEventInfo.h jborel/TupleToolEventInfo.h
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
class TupleToolEventInfo : public GaudiTool, virtual public IEventTupleTool {
public:
  /// Standard constructor
  TupleToolEventInfo( const std::string& type,
		      const std::string& name,
		      const IInterface* parent);

  virtual ~TupleToolEventInfo( ){}; ///< Destructor

  StatusCode fill( Tuples::Tuple& );

private :
};
#endif // JBOREL_TUPLETOOLEVENTINFO_H
