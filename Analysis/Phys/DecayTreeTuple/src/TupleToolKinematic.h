// $Id: TupleToolKinematic.h,v 1.2 2010-01-26 15:39:26 rlambert Exp $
#ifndef JBOREL_TUPLETOOLKINEMATIC_H
#define JBOREL_TUPLETOOLKINEMATIC_H 1

// Include files
// from Gaudi
#include "TupleToolBase.h"
#include "Kernel/IParticleTupleTool.h"            // Interface

/** @class TupleToolKinematic TupleToolKinematic.h jborel/TupleToolKinematic.h
 *
 * \brief Fill kinematic information for DecayTreeTuple
 *
 * - head_P : momentum's amplitude
 * - head_PT : transverse momentum
 * - head_P[E|X|Y|Z] : four vector momentum
 * - head_MM : measured mass (or assigned mass in case of 'basic' particle
 * - head_M : mass caclulated from momentum four-vector
 * - head_MMERR : error on the measured mass (only for non-basic parts)
 *
 * if Verbose:
 *
 * - head_REFP[X|Y|Z]: one point the particle momentum extrapolation
     goes through
 *
 * \sa DecayTreeTuple
 *
 *  @author Jeremie Borel
 *  @date   2007-11-07
 */
class TupleToolKinematic : public TupleToolBase, virtual public IParticleTupleTool {
public:
  /// Standard constructor
  TupleToolKinematic( const std::string& type,
		    const std::string& name,
		    const IInterface* parent);

  virtual ~TupleToolKinematic(){}; ///< Destructor

  virtual StatusCode fill( const LHCb::Particle*
			   , const LHCb::Particle*
			   , const std::string&
			   , Tuples::Tuple& );
};

#endif // JBOREL_TUPLETOOLKINEMATIC_H
