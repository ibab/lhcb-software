// $Id: DaVinciFun.cpp,v 1.3 2009-08-31 20:43:20 jpalac Exp $
// Include files 
// Gaudi
#include "GaudiKernel/SmartIF.h"
// DaVinciInterfaces
#include "Kernel/ISetInputParticles.h"
// local
#include "Kernel/DaVinciFun.h"

//-----------------------------------------------------------------------------
// Implementation file for DaVinciFun
//
// 2009-08-02 : Juan Palacios
//-----------------------------------------------------------------------------
namespace DaVinci 
{
  
  const LHCb::VertexBase* bestVertexBase(const Particle2Vertex::Table::Range& range) 
  {
    if (range.empty()) return 0;
    return range.back().to();
  }

  const LHCb::RecVertex* bestRecVertex(const Particle2Vertex::Table::Range& range) 
  {
    if (range.empty()) return 0;
    return dynamic_cast<const LHCb::RecVertex*>( range.back().to() );
  }

  
  namespace P2VVAngles 
  {
    
    double calculateThetaTr(const Gaudi::LorentzVector& particleA,
                            const Gaudi::LorentzVector& particleB,
                            const Gaudi::LorentzVector& particleC,
                            const Gaudi::LorentzVector& particleD)
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
    double calculatePhiTr(const Gaudi::LorentzVector& particleA,
                          const Gaudi::LorentzVector& particleB,
                          const Gaudi::LorentzVector& particleC,
                          const Gaudi::LorentzVector& particleD)
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
  } // namespace P2VVangles


  namespace Utils 
  {
    // ========================================================================
    /*  set the input particles for some component
     *  @param component the component inself
     *  @param input the intut data 
     *  @return status code 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date   2008-07-11
     */
    // ========================================================================
    StatusCode setInput 
    ( IInterface*                        component , 
      const LHCb::Particle::ConstVector& input     ) 
    {
      if ( 0 == component ) { return StatusCode ( 300 ) ; }        // RETURN 
      // get the interface 
      SmartIF<ISetInputParticles> alg ( component ) ;
      // valid ? 
      if ( !alg           ) { return StatusCode ( 301 ) ; }        // RETURN  
      // use the valid inyterface 
      return alg->setInput ( input ) ;                             // RETURN 
    }
  } // namespace Utils
  
} // namespace DaVinci 
