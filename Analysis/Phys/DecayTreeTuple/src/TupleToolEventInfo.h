// $Id: TupleToolEventInfo.h,v 1.5 2010-06-02 12:01:44 pkoppenb Exp $
#ifndef JBOREL_TUPLETOOLEVENTINFO_H
#define JBOREL_TUPLETOOLEVENTINFO_H 1

// Include files
// from Gaudi
#include "TupleToolBase.h"
#include "Kernel/IEventTupleTool.h"            // Interface

class ITupleTool;

/** @class TupleToolEventInfo TupleToolEventInfo.h jborel/TupleToolEventInfo.h
 *
 * \brief Event and Run number for DecayTreeTuple
 *
 * Tuple columns:
 * - runNumber: well, the run number
 * - eventNumber: 
 * - BCID and BC type
 * - Odin, L0 and Hlt TCKs
 * - GPS time
 * - if Verbose is on, also gps time in year,month,day,hour,min,second
 *   Note that months are numbered [0-11]. That's a convention. Sorry.
 *
 * \sa DecayTreeTuple
 *  @author Jeremie Borel
 *  @date   2007-11-07
 */
class TupleToolEventInfo : public TupleToolBase, virtual public IEventTupleTool {
public:
  /// Standard constructor
  TupleToolEventInfo( const std::string& type,
		      const std::string& name,
		      const IInterface* parent);

  virtual ~TupleToolEventInfo( ){}; ///< Destructor

  StatusCode fill( Tuples::Tuple& );

private :
  std::string m_pvLocation ;
};
#endif // JBOREL_TUPLETOOLEVENTINFO_H
