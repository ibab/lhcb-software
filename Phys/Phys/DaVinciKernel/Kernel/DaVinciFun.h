// $Id: DaVinciFun.h,v 1.4 2009-08-02 15:56:06 gcowan Exp $
#ifndef KERNEL_DAVINCIFUN_H 
#define KERNEL_DAVINCIFUN_H 1

// Include files
// from ROOT
#include "Math/Boost.h"
#include "Kernel/Particle2Vertex.h"

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

  /// Find the position of an LHCb::VertexBase inside a range
  inline Particle2Vertex::Range::iterator findPos(const Particle2Vertex::Range& range, 
                                                  const LHCb::VertexBase* to )
  {
    for (Particle2Vertex::Range::iterator r = range.begin(); 
         range.end()!=r; 
         ++r) {
      if ( to == r->to() ) { return r; }  
    }
    return range.end();
  }

  const LHCb::VertexBase* bestVertexBase(const Particle2Vertex::Range& range) 
  {
    if (range.empty()) return 0;
    return range.back().to();
  }

  const LHCb::RecVertex* bestRecVertex(const Particle2Vertex::Range& range) 
  {
    if (range.empty()) return 0;
    return dynamic_cast<const LHCb::RecVertex*>( range.back().to() );
  }

  /** @namespace DaVinci::P2VVAngles Kernel/DaVinciFun.h
   *  
   * Collection of Tools to calculate the transversity and helicity angles in 
   * a P->VV decay. Particularly useful for decays such as Bs->JpsiPhi, 
   * Bd->JpsiKstar and Bd->KstarMuMu. In each function we perform consecutive 
   * boosts, first to the rest frame of the B and the to the rest frame of
   * the appropriate mother particle.
   *
   * 1. DEFINITIONS USED IN THESE FUNCTIONS:
   *
   * * The Bcandidate decays to vector mesons mother1 and mother2.\n
   * * mother1 decays to particles A and B.\n
   * * mother2 decays to particles C and D.\n          
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
   * > The defition is the same whether it is a Bcandidate or a BbarCandidate.\n
   * > theta_tr is the angle formed by particleA and the z-axis, in mother1's 
   * rest frame.\n
   * > phi_tr is the azimuthal angle of particleA in mother1's rest frame.\n 
   * > psi is the helicity angle between particleC and the opposite direction of 
   * mother1's momentum. This is the same angle as is defined in the HELICITY 
   * BASIS.\n 
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
   * * particleD -> negative kaon\n
   *
   * If this approach is followed then the calculated angles will be the same as those
   * defined in the beta_s roadmap document.
   *
   *  @author Greig COWAN
   *  @date   2009-08-02
   */
  namespace P2VVAngles {
    double calculateThetaTr(const Gaudi::LorentzVector particleA,
                            const Gaudi::LorentzVector particleB,
                            const Gaudi::LorentzVector particleC,
                            const Gaudi::LorentzVector particleD)
    {
      const Gaudi::LorentzVector mother1 = particleA + particleB; // i.e., Jpsi
      const Gaudi::LorentzVector mother2 = particleC + particleD; // i.e., phi
      const Gaudi::LorentzVector Bcand = mother1 + mother2; // B candidate
      
      // We need to boost everything to the B frame first
      ROOT::Math::Boost boostToB( Bcand.BoostToCM() );
      
      // Now boost everything to the B frame
      const Gaudi::LorentzVector boostedBmother1 = boostToB( mother1 );
      const Gaudi::LorentzVector boostedBmother2 = boostToB( mother2 );
      const Gaudi::LorentzVector boostedBpartA   = boostToB( particleA );
      const Gaudi::LorentzVector boostedBpartB   = boostToB( particleB );
      const Gaudi::LorentzVector boostedBpartC   = boostToB( particleC );
      const Gaudi::LorentzVector boostedBpartD   = boostToB( particleD );
      
      // Now we need to boost to the mother1 frame
      ROOT::Math::Boost boostToMother1( boostedBmother1.BoostToCM() );

      // Do the boost to the mother1 frame
      const Gaudi::LorentzVector boostedM1mother1 = boostToMother1( boostedBmother1 );
      const Gaudi::LorentzVector boostedM1mother2 = boostToMother1( boostedBmother2 );
      const Gaudi::LorentzVector boostedM1partA   = boostToMother1( boostedBpartA );
      const Gaudi::LorentzVector boostedM1partB   = boostToMother1( boostedBpartB );
      const Gaudi::LorentzVector boostedM1partC   = boostToMother1( boostedBpartC );
      const Gaudi::LorentzVector boostedM1partD   = boostToMother1( boostedBpartD );
      
      // Find the vector that is normal to the plane defined by the CD system
      const Gaudi::XYZVector vecPartA = Gaudi::XYZVector( boostedM1partA ).unit();
      const Gaudi::XYZVector vecPartC = Gaudi::XYZVector( boostedM1partC ).unit();
      const Gaudi::XYZVector vecPartD = Gaudi::XYZVector( boostedM1partD ).unit();
      const Gaudi::XYZVector vecZaxis = (vecPartD.Cross( vecPartC )).unit();
      
      // Theta is the polar angle of A wrt this z-axis
      double theta = acos( vecZaxis.Dot( vecPartA ) );
      return theta;
    }

    // Azimuthal angle
    double calculatePhiTr(const Gaudi::LorentzVector particleA,
                          const Gaudi::LorentzVector particleB,
                          const Gaudi::LorentzVector particleC,
                          const Gaudi::LorentzVector particleD)
    {
      // Code here same as in thetaTr since we need to boost everything to the same 
      // frame.
      const Gaudi::LorentzVector mother1 = particleA + particleB; // i.e., Jpsi
      const Gaudi::LorentzVector mother2 = particleC + particleD; // i.e., phi
      const Gaudi::LorentzVector Bcand = mother1 + mother2;       // B candidate

      // We need to boost everything to the B frame first
      ROOT::Math::Boost boostToB( Bcand.BoostToCM() );
      
      // Now boost everything to the B frame
      const Gaudi::LorentzVector boostedBmother1 = boostToB( mother1 );
      const Gaudi::LorentzVector boostedBmother2 = boostToB( mother2 );
      const Gaudi::LorentzVector boostedBpartA   = boostToB( particleA );
      const Gaudi::LorentzVector boostedBpartB   = boostToB( particleB );
      const Gaudi::LorentzVector boostedBpartC   = boostToB( particleC );
      const Gaudi::LorentzVector boostedBpartD   = boostToB( particleD );
      
      // Now we need to boost to the mother1 frame
      ROOT::Math::Boost boostToMother1( boostedBmother1.BoostToCM() );

      // Do the boost to the mother1 frame
      const Gaudi::LorentzVector boostedM1mother1 = boostToMother1( boostedBmother1 );
      const Gaudi::LorentzVector boostedM1mother2 = boostToMother1( boostedBmother2 );
      const Gaudi::LorentzVector boostedM1partA   = boostToMother1( boostedBpartA );
      const Gaudi::LorentzVector boostedM1partB   = boostToMother1( boostedBpartB );
      const Gaudi::LorentzVector boostedM1partC   = boostToMother1( boostedBpartC );
      const Gaudi::LorentzVector boostedM1partD   = boostToMother1( boostedBpartD );

      // Find the vector that is normal to the plane defined by the CD system
      const Gaudi::XYZVector vecPartA = Gaudi::XYZVector( boostedM1partA ).unit();
      const Gaudi::XYZVector vecPartC = Gaudi::XYZVector( boostedM1partC ).unit();
      const Gaudi::XYZVector vecPartD = Gaudi::XYZVector( boostedM1partD ).unit();
      const Gaudi::XYZVector vecZaxis = (vecPartD.Cross( vecPartC )).unit();

      // Decompose particle A into it's perpendicular and parallel components, 
      // where parallel is in the plane defined by the CD system in mother1's rest frame.
      const Gaudi::XYZVector A_perp = ( vecZaxis * vecPartA.Dot( vecZaxis ) ).unit();
      const Gaudi::XYZVector A_para = ( vecPartA - A_perp ).unit();
      const Gaudi::XYZVector vecMother2 = Gaudi::XYZVector( boostedM1mother2 ).unit();

      double phi = acos( A_para.Dot( vecMother2 ));
        
      // Solve sign ambiguity - make sure that the cross product of the direction of 
      // mother2 and A_para is the same as the z-axis we have defined. 
      const Gaudi::XYZVector checknormal = vecMother2.Cross( A_para );
      
      bool samedirection = ( checknormal.Dot( vecZaxis ) > 0.0);      
      if (!samedirection) phi = -phi;
      
      return phi;      
    }
    
    // TRANSVERSITY AND HELICITY FRAMES
    double calculateHelicityPolarAngle(const Gaudi::LorentzVector& particleA,
                                      const Gaudi::LorentzVector& particleB,
                                      const Gaudi::LorentzVector& particleC,
                                      const Gaudi::LorentzVector& particleD)
    {  
      const Gaudi::LorentzVector mother1 = particleA + particleB; // i.e., Jpsi
      const Gaudi::LorentzVector mother2 = particleC + particleD; // i.e., phi
      const Gaudi::LorentzVector Bcand = mother1 + mother2;       // B candidate

      // We need to boost everything to the B frame first
      ROOT::Math::Boost boostToB( Bcand.BoostToCM() );

      // Now boost everything to the B frame
      const Gaudi::LorentzVector boostedBmother1 = boostToB( mother1 );
      const Gaudi::LorentzVector boostedBmother2 = boostToB( mother2 );
      const Gaudi::LorentzVector boostedBpartA   = boostToB( particleA );
      const Gaudi::LorentzVector boostedBpartB   = boostToB( particleB );
      const Gaudi::LorentzVector boostedBpartC   = boostToB( particleC );
      const Gaudi::LorentzVector boostedBpartD   = boostToB( particleD );
      
      // Now we need to boost to the mother2 frame
      ROOT::Math::Boost boostToMother2( boostedBmother2.BoostToCM() );

      // Do the boost to the mother2 frame
      const Gaudi::LorentzVector boostedM2mother1 = boostToMother2( boostedBmother1 );
      const Gaudi::LorentzVector boostedM2mother2 = boostToMother2( boostedBmother2 );
      const Gaudi::LorentzVector boostedM2partA   = boostToMother2( boostedBpartA );
      const Gaudi::LorentzVector boostedM2partB   = boostToMother2( boostedBpartB );
      const Gaudi::LorentzVector boostedM2partC   = boostToMother2( boostedBpartC );
      const Gaudi::LorentzVector boostedM2partD   = boostToMother2( boostedBpartD );

      // Now find the angle between particleC and the direction of 
      // mother2 in the _B_ rest frame.
      const Gaudi::XYZVector vecPartC = Gaudi::XYZVector( boostedM2partC ).unit();
      const Gaudi::XYZVector vecMother2InBframe = Gaudi::XYZVector( boostedBmother2 ).unit();
 
      double psi = acos( vecPartC.Dot( vecMother2InBframe ) );
      
      return psi;
    }
    
    double calculatePlaneAngle( const Gaudi::LorentzVector& particleA,
                                const Gaudi::LorentzVector& particleB,
                                const Gaudi::LorentzVector& particleC,
                                const Gaudi::LorentzVector& particleD ) 
    {
      // calculate the angle between two planes in the rest frame of the mother particle
      const Gaudi::LorentzVector mother1 = particleA + particleB; // i.e., Jpsi
      const Gaudi::LorentzVector mother2 = particleC + particleD; // i.e., phi
      const Gaudi::LorentzVector Bcand = mother1 + mother2;       // B candidate

      // We need to boost everything to the B frame first
      ROOT::Math::Boost boostToB( Bcand.BoostToCM() );

      // Now boost everything to the B frame
      const Gaudi::LorentzVector boostedBmother1 = boostToB( mother1 );
      const Gaudi::LorentzVector boostedBmother2 = boostToB( mother2 );
      const Gaudi::LorentzVector boostedBpartA   = boostToB( particleA );
      const Gaudi::LorentzVector boostedBpartB   = boostToB( particleB );
      const Gaudi::LorentzVector boostedBpartC   = boostToB( particleC );
      const Gaudi::LorentzVector boostedBpartD   = boostToB( particleD );

      // Define some vectors that we need
      const Gaudi::XYZVector vecA = Gaudi::XYZVector( boostedBpartA ).unit();
      const Gaudi::XYZVector vecB = Gaudi::XYZVector( boostedBpartB ).unit();
      const Gaudi::XYZVector vecC = Gaudi::XYZVector( boostedBpartC ).unit();
      const Gaudi::XYZVector vecD = Gaudi::XYZVector( boostedBpartD ).unit();

      // Get the normals to the decay planes
      const Gaudi::XYZVector el = ( vecA.Cross( vecB ) ).unit();      
      const Gaudi::XYZVector ek = ( vecC.Cross( vecD ) ).unit();
      // The direction of mother2 in the B frame
      const Gaudi::XYZVector ez = ( Gaudi::XYZVector( boostedBmother2 ) ).unit();

      double cosPhi = ( ek.Dot( el ) );      
      double sinPhi = ( el.Cross( ek ) ).Dot( ez );
      double phi    = acos( cosPhi ) ;
      
      // Resolve ambiguity
      return ( sinPhi > 0.0 ? phi : -phi );      
    }
  } // namespace P2VVAngles


} // namespace DaVinci

#endif // KERNEL_DAVINCIFUN_H
