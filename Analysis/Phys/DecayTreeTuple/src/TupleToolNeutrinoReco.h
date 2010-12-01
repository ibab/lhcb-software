// $Id: TupleToolNeutrinoReco.h,v 1.4 2010-03-16 13:23:19 xieyu Exp $
#ifndef TUPLETOOLNEUTRINORECO_H
#define TUPLETOOLNEUTRINORECO_H 1

// Include files
// from Gaudi
#include "TupleToolBase.h"
#include "Kernel/IParticleTupleTool.h"            // Interface

/** @class TupleToolNeutrinoReco TupleToolNeutrinoReco.h 
 *
 * \brief Fill neutrino reconstruction variables
 *
 * Assuming a particle of zero-mass has gone undetected, we can calculate
 * the corrected momentum of the B of a specific mass.
 * 0 = aP(neutrino)^2+bP(neutrino)+c
 * a = 4(PT^2+M^2)
 * b = 4P||^2(2PT^2 - mB^2 + M^2)           = X - 4P||^2*mB^2
 * c = 4PT^2(2P||^2 + mB^2) -(mB^2 - M^2)^2 = Y - 4PT ^2*mB*2 - (mB^2 - M^2)^2 
 * - head_Nu_a    : a in the above equation
 * - head_Nu_Parl : momentum of the particle paralell to the direction of flight
 * - head_Nu_Perp : momentum of the particle perpendicular to the direction of flight
 * - head_Nu_X    : X in the above equation, 4P||^2(2PT^2 + M^2)
 * - head_Nu_Y    : Y in the above equation, 8PT^2*P||^2
 *
 * Note that the solution is given by the quadratic formula, and the factors of 4 cancel
 * Note also that -b/2a simplifies to:
 *
 * -b/2a = - P||^2 [ 1 - (mB^2 - PT^2)/(PT^2+M^2)    ] / 2
 *
 * So, with 'head' replaced by something like 'B0', and mB being the PDG mass:
 * 
 * b = B0_Nu_X - 4*B0_Nu_Perp*B0_Nu_Perp*mB*mB
 * c = B0_Nu_Y - 4*B0_Nu_Parl*B0_Nu_Parl*mB*mB - (mB*mB - B0_MM*B0_MM)*(mB*mB - B0_MM*B0_MM)
 * 
 * pb = -B0_Nu_Parl*B0_Nu_Parl*( 1 - (mB*mB-B0_Nu_Perp*B0_Nu_Perp)/(B0_Nu_Perp*B0_Nu_Perp + B0_MM*B0_MM)  )/2
 * pd = sqrt(b*b - 4*B0_Nu_a*c)/(2*B0_Nu_a)
 *
 * B0_Corrected_P_lo = B0_Nu_Parl+pb-pd
 * B0_Corrected_P_hi = B0_Nu_Parl+pb+pd
 *
 * Are the two B-momentum solutions
 *
 * \sa DecayTreeTuple
 *
 *  @author Rob Lambert
 *  @date   2010-11-30
 */
namespace LHCb{
  class VertexBase;
  class Particle;
};
class DVAlgorithm;

class TupleToolNeutrinoReco : public TupleToolBase, virtual public IParticleTupleTool {
public:
  /// Standard constructor
  TupleToolNeutrinoReco( const std::string& type,
		    const std::string& name,
		    const IInterface* parent);

  virtual ~TupleToolNeutrinoReco(){}; ///< Destructor

  virtual StatusCode fill( const LHCb::Particle*
			   , const LHCb::Particle*
			   , const std::string&
			   , Tuples::Tuple& );
  
  StatusCode initialize();
  
  double MomentumParallel(const LHCb::VertexBase* , 
                          const LHCb::Particle* , 
                          const ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >* );
  
  double MomentumPerpendicular(const LHCb::VertexBase* , 
                               const LHCb::Particle* , 
                               const ROOT::Math::LorentzVector<ROOT::Math::PxPyPzE4D<double> >* ) ;
  
private:
  
  DVAlgorithm* m_dva;
  
};

#endif // TUPLETOOLNEUTRINORECO_H
