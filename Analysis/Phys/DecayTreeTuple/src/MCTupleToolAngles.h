// $Id: MCTupleToolAngles.h,v 1.1 2009-01-22 17:52:41 pkoppenb Exp $
#ifndef MCTUPLETOOLANGLES_H 
#define MCTUPLETOOLANGLES_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IMCParticleTupleTool.h"            // Interface
#include "Math/Boost.h"
#include "GaudiKernel/Vector4DTypes.h"


/** @class MCTupleToolAngles MCTupleToolAngles.h
 *  
 *  Fill MC Particle 
 *
 * See the properties inline documentation to toggle on/off some of
 * the columns
 *
 * - head_TRUEID : true pid
 *
 * - head_TRUEP[E|X|Y|Z] : true four vector momentum
 * - head_TRUEORIGINVERTEX_[X|Y|Z] : position of the true origin vertex.
 * - head_TRUEENDVERTEX_[X|Y|Z] : position of the true end vertex (the
     first one)
 * - head_TRUETAU : true propertime
 * - head_TRUE{ThetaL,ThetaK,Phi} : true helicity angles
 * - head_TRUE{ThetaTr,PhiTr,ThetaVtr} : true transversity angles
 * 
 * \sa TupleToolAngles, DecayTreeTuple, MCDecayTreeTuple
 * 
 *  @author Patrick Koppenburg
 *  @date   2009-01-19
 */

class MCTupleToolAngles : public GaudiTool, virtual public IMCParticleTupleTool {
public: 
  /// Standard constructor
  MCTupleToolAngles( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~MCTupleToolAngles( ); ///< Destructor
  virtual StatusCode initialize();
  virtual StatusCode fill( const LHCb::MCParticle*
			   , const LHCb::MCParticle*
			   , const std::string&
			   , Tuples::Tuple& );


protected:

private:
#include "Theta.icpp"
private:

};
#endif // MCTUPLETOOLANGLES_H
