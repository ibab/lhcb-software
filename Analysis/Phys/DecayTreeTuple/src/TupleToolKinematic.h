// $Id: TupleToolKinematic.h,v 1.1.1.1 2007-12-12 17:46:43 pkoppenb Exp $
#ifndef JBOREL_TUPLETOOLKINEMATIC_H
#define JBOREL_TUPLETOOLKINEMATIC_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleTupleTool.h"            // Interface

/** @class TupleToolKinematic TupleToolKinematic.h jborel/TupleToolKinematic.h
 *
 * \brief Fill kinematic information for DecayTreeTuple
 *
 * - head_P : momentum's amplitude
 * - head_PT : transverse momentum
 * - head_P[E|X|Y|Z] : four vector momentum
 * - head_REFP[X|Y|Z]: one point the particle momentum extrapolation
     goes through
 * - head_MM : measured mass (or assigned mass in case of 'basic' particle
 * - head_MMERR : error on the measured mass (only for non-basic parts)
 *
 *
 * \sa DecayTreeTuple
 *
 *  @author Jeremie Borel
 *  @date   2007-11-07
 */
class TupleToolKinematic : public GaudiTool, virtual public IParticleTupleTool {
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
