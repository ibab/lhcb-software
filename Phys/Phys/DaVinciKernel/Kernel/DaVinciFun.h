// $Id: DaVinciFun.h,v 1.8 2009-08-31 20:43:20 jpalac Exp $
#ifndef KERNEL_DAVINCIFUN_H 
#define KERNEL_DAVINCIFUN_H 1

// Include files
// from ROOT
#include "Math/Boost.h"
#include "Kernel/Particle2Vertex.h"
#include "Event/RecVertex.h"
/** @namespace DaVinci Kernel/DaVinciFun.h
 *  
 * Collection of free functions used widely in DVAlgorithm 
 * and PhysDesktop
 *
 *
 *  @author Juan PALACIOS
 *  @date   2009-02-27
 */
namespace DaVinci {

  ///  Is in TES
  template <class T>
  inline bool inTES(const T* P) {
    return ( 0!=P && 0!=P->parent()) ;
  }

  /**
   *
   * delete contents of container of newed pointers unless they are
   * also in the TES.
   * T must be a const_iterator defined, begin(), end() and clear() methods,
   * and holding pointers.
   *
   * @param container STL container of ContainedObject pointers
   * @return number of elements that are also in the TES.
   *
   * @author Juan Palacios juan.palacios@nikhef.nl
   * @date 10/02/2009
   *
   **/
  template <class T>
  unsigned int safeContainerClear(T& container) 
  {
    if ( container.empty() ) return 0;
    int iCount(0);
    for (typename T::const_iterator iObj = container.begin();
         iObj != container.end(); ++iObj) {
      if( DaVinci::inTES(*iObj) ) {
        ++iCount;
      } else {
        delete *iObj;
      }
      
    }
    container.clear();
    return iCount;
  }

  const LHCb::VertexBase* bestVertexBase(const Particle2Vertex::Table::Range& range);

  const LHCb::RecVertex* bestRecVertex(const Particle2Vertex::Table::Range& range);


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
  namespace P2VVAngles {
    double calculateThetaTr(const Gaudi::LorentzVector& particleA,
                            const Gaudi::LorentzVector& particleB,
                            const Gaudi::LorentzVector& particleC,
                            const Gaudi::LorentzVector& particleD);

    // Azimuthal angle
    double calculatePhiTr(const Gaudi::LorentzVector& particleA,
                          const Gaudi::LorentzVector& particleB,
                          const Gaudi::LorentzVector& particleC,
                          const Gaudi::LorentzVector& particleD);
    
    // TRANSVERSITY AND HELICITY FRAMES
    double calculateHelicityPolarAngle(const Gaudi::LorentzVector& particleA,
                                       const Gaudi::LorentzVector& particleB,
                                       const Gaudi::LorentzVector& particleC,
                                       const Gaudi::LorentzVector& particleD);
    
    double calculatePlaneAngle( const Gaudi::LorentzVector& particleA,
                                const Gaudi::LorentzVector& particleB,
                                const Gaudi::LorentzVector& particleC,
                                const Gaudi::LorentzVector& particleD );
    

  } // namespace P2VVAngles


  // ==========================================================================
  namespace Utils
  {
    // ========================================================================
    /** set the input particles for some component
     *  @param component the component inself
     *  @param input the intut data 
     *  @return status code 
     * 
     *  - 300 : invalid component 
     *  - 301 : no valid ISetInputParticles interface 
     * 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-07-11
     */
    StatusCode setInput 
    ( IInterface*                        component , 
      const LHCb::Particle::ConstVector& input     ) ;
    // ========================================================================
  } // end of namespace Utils 
  // ==========================================================================

} // namespace DaVinci

#endif // KERNEL_DAVINCIFUN_H
