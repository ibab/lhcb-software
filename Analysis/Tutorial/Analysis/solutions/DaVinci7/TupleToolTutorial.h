// $Id: TupleToolTutorial.h,v 1.1 2009-06-11 16:22:29 rlambert Exp $
#ifndef TUPLETOOLTUTORIAL_H
#define TUPLETOOLTUTORIAL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleTupleTool.h"            // Interface

/** @class TupleToolTutorial TupleToolTutorial.h jborel/TupleToolTutorial.h
 *
 * \brief Skeleton for DaVinci Tutorial 7
 *
 *
 *
 * \sa DecayTreeTuple
 *
 *  @author Rob Lambert
 *  @date   2009-06-09
 */
class TupleToolTutorial : public GaudiTool, virtual public IParticleTupleTool {
public:
  /// Standard constructor
  TupleToolTutorial( const std::string& type,
		    const std::string& name,
		    const IInterface* parent);

  virtual ~TupleToolTutorial(){}; ///< Destructor

  virtual StatusCode fill( const LHCb::Particle*
			   , const LHCb::Particle*
			   , const std::string&
			   , Tuples::Tuple& );
};

#endif // TUPLETOOLTUTORIAL_H
