// $Id: DaVinciP2VVAngles.h,v 1.1 2009-11-05 10:19:45 jpalac Exp $
#ifndef KERNEL_DAVINCIP2VVANGLES_H
#define KERNEL_DAVINCIP2VVANGLES_H 1
// Include files 
// local
#include "Math/Boost.h"
#include "GaudiKernel/Vector4DTypes.h"
/** @namespace DaVinci::P2VVAngles Kernel/DaVinciFun.h
 *  
 * Collection of functions to calculate the transversity and helicity angles in 
 * a P->VV decay. Particularly useful for decays such as Bs->JpsiPhi, 
 * Bd->JpsiKstar and Bd->KstarMuMu. In each function we perform consecutive 
 * boosts, first to the rest frame of the B and the to the rest frame of
 * the appropriate mother particle. These functions deal only with the
 * kinematics of the decays. The user is required to "unpack" the decay
 * and order the particles according to charge and whichever convention
 * they are using. Once ordered, these particles are passed to these 
 * functions to perform the kinematic calculation. An example is given below. 
 *
 * 1. DEFINITIONS USED IN THESE FUNCTIONS:
 *
 * * The Bcandidate decays to vector mesons mother1 and mother2.\n
 * * mother1 decays to particles A and B.\n
 * * mother2 decays to particles C and D. 
 *
 * 2. TRANSVERSITY BASIS
 *
 * The transversity basis is defined in Section 2.2 of the beta_s roadmap.
 * (LHCb-2009-XXX). What follows here is taken from the roadmap and adapted 
 * to the general notation used in these functions.
 *
 * 2.1 Transversity Coordinate Defintion:
 *
 * In the coordinate system of the rest frame (where mother2 and the B meson
 * move in the x direction, the z axis is perpendicular to the decay plane of
 * mother2 -> C+D, and Py(particleC) >= 0), the transversity polar and 
 * azimuthal angles (theta_tr, phi_tr) describe the direction of particle A.
 * In the rest frame of mother2, the angle psi is the angle between 
 * P(particleC) and -P(mother1) (i.e., the direction opposite to the flight 
 * direction of mother1).
 *
 * 2.2 Transversity Angle Definition:
 *
 * * The defition is the same whether it is a Bcandidate or a BbarCandidate.\n
 * * theta_tr is the angle formed by particleA and the z-axis, in mother1's 
 * rest frame.\n
 * * phi_tr is the azimuthal angle of particleA in mother1's rest frame.\n 
 * * psi is the helicity angle between particleC and the opposite direction of 
 * mother1's momentum. This is the same angle as is defined in the HELICITY 
 * BASIS.
 *
 * 3. HELICITY BASIS
 *
 * Need to add something in here!
 *
 * 3.1 Helicity Coordinate Definition
 * 
 * 3.2 Helicity Angle Definition
 *
 * 4. CONSECUTIVE BOOSTS
 *
 * In ALL functions, we first boost all particles to the rest frame of the B candidate
 * _before_ boosting to the rest frame of the relevant mother particle.
 *
 * 5. EXAMPLE OF USE
 * 
 * These functions were written with the calculation of the decay angles for decays
 * such as Bs->Jpsi(mumu)Phi(KK) at LHCb. In this case, the functions should be used 
 * as follows:
 *
 * * particleA -> positive muon\n
 * * particleB -> negative muon\n
 * * particleC -> positive kaon\n
 * * particleD -> negative kaon
 *
 * If this approach is followed then the calculated angles will be the same as those
 * defined in the beta_s roadmap document.
 *
 *  @author Greig COWAN
 *  @date   2009-08-02
 */

namespace DaVinci 
{

  namespace P2VVAngles 
  {
    /// Function used by all angle calculating tools to boost to B frame
    void boostToBframe( const Gaudi::LorentzVector& particleA,
                        const Gaudi::LorentzVector& particleB,
                        const Gaudi::LorentzVector& particleC,
                        const Gaudi::LorentzVector& particleD,
                        Gaudi::LorentzVector& boostedBmother1,
                        Gaudi::LorentzVector& boostedBmother2,
                        Gaudi::LorentzVector& boostedBpartA,
                        Gaudi::LorentzVector& boostedBpartB,
                        Gaudi::LorentzVector& boostedBpartC,
                        Gaudi::LorentzVector& boostedBpartD
                        );
 
    double calculateThetaTr(const Gaudi::LorentzVector& particleA,
                            const Gaudi::LorentzVector& particleB,
                            const Gaudi::LorentzVector& particleC,
                            const Gaudi::LorentzVector& particleD);

    /// Azimuthal angle of particleA in mother1's rest frame
    double calculatePhiTr(const Gaudi::LorentzVector& particleA,
                          const Gaudi::LorentzVector& particleB,
                          const Gaudi::LorentzVector& particleC,
                          const Gaudi::LorentzVector& particleD);
    

    /// This function is used in both the helicity AND transversity bases
    double calculateHelicityPolarAngle(const Gaudi::LorentzVector& particleA,
                                       const Gaudi::LorentzVector& particleB,
                                       const Gaudi::LorentzVector& particleC,
                                       const Gaudi::LorentzVector& particleD);

    /// Angle between the two decay planes in the rest frame of the Bcand
    double calculatePlaneAngle( const Gaudi::LorentzVector& particleA,
                                const Gaudi::LorentzVector& particleB,
                                const Gaudi::LorentzVector& particleC,
                                const Gaudi::LorentzVector& particleD );

  } // namespace P2VVangles
  
} // namespace DaVinci 
#endif // KERNEL_DAVINCIP2VVANGLES_H 1
