// $Id: TupleToolDebug.h,v 1.1 2010-06-02 12:01:44 pkoppenb Exp $
#ifndef PKOPPENB_TUPLETOOLDEBUG_H
#define PKOPPENB_TUPLETOOLDEBUG_H 1

// Include files
// from Gaudi
#include "TupleToolBase.h"
#include "Kernel/IEventTupleTool.h"            // Interface

class DVAlgorithm ;

/** @class TupleToolDebug TupleToolDebug.h 
 *
 * \brief Test
 *
 * \sa DecayTreeTuple
 *  @author P. Koppenburg
 *  @date   2010-06-02
 */
class TupleToolDebug : public TupleToolBase, virtual public IEventTupleTool {
public:
  /// Standard constructor
  TupleToolDebug( const std::string& type,
		      const std::string& name,
		      const IInterface* parent);

  virtual ~TupleToolDebug( ){}; ///< Destructor

  virtual StatusCode fill( Tuples::Tuple& );

private:

};
#endif // PKOPPENB_TUPLETOOLDEBUG_H
