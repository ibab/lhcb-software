// $Id: DaVinciP2VVAngles.cpp,v 1.1 2009-11-05 10:19:45 jpalac Exp $
// Include files 
// local
#include "Kernel/DaVinciP2VVAngles.h"
#include "GaudiKernel/Vector3DTypes.h"
//-----------------------------------------------------------------------------
// Implementation file for class : DaVinciP2VVAngles
//
// 2009-11-05 : Greig Cowan
//-----------------------------------------------------------------------------

namespace DaVinci 
{

  namespace P2VVAngles 
  {
   
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
                        )
    {
      // Function used by all angle calculating tools to boost to B frame
      const Gaudi::LorentzVector mother1 = particleA + particleB; // i.e., Jpsi
      const Gaudi::LorentzVector mother2 = particleC + particleD; // i.e., phi
      const Gaudi::LorentzVector Bcand = mother1 + mother2; // B candidate

      // We need this to boost everything to the B frame
      ROOT::Math::Boost boostToB( Bcand.BoostToCM() );

      // Now boost everything to the B frame
      boostedBmother1 = boostToB( mother1 );
      boostedBmother2 = boostToB( mother2 );
      boostedBpartA   = boostToB( particleA );
      boostedBpartB   = boostToB( particleB );
      boostedBpartC   = boostToB( particleC );
      boostedBpartD   = boostToB( particleD );

      return;
    }
 
    double calculateThetaTr(const Gaudi::LorentzVector& particleA,
                            const Gaudi::LorentzVector& particleB,
                            const Gaudi::LorentzVector& particleC,
                            const Gaudi::LorentzVector& particleD)
    {
      // Firstly we want to boost everything to the B rest frame
      Gaudi::LorentzVector boostedBmother1;
      Gaudi::LorentzVector boostedBmother2;
      Gaudi::LorentzVector boostedBpartA;
      Gaudi::LorentzVector boostedBpartB;
      Gaudi::LorentzVector boostedBpartC;
      Gaudi::LorentzVector boostedBpartD;

      boostToBframe( particleA, particleB, particleC, particleD,
                     boostedBmother1, boostedBmother2,
                     boostedBpartA, boostedBpartB, 
                     boostedBpartC, boostedBpartD);

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

    // Azimuthal angle of particleA in mother1's rest frame
    double calculatePhiTr(const Gaudi::LorentzVector& particleA,
                          const Gaudi::LorentzVector& particleB,
                          const Gaudi::LorentzVector& particleC,
                          const Gaudi::LorentzVector& particleD)
    {
      // Firstly we want to boost everything to the B rest frame
      Gaudi::LorentzVector boostedBmother1;
      Gaudi::LorentzVector boostedBmother2;
      Gaudi::LorentzVector boostedBpartA;
      Gaudi::LorentzVector boostedBpartB;
      Gaudi::LorentzVector boostedBpartC;
      Gaudi::LorentzVector boostedBpartD;

      boostToBframe( particleA, particleB, particleC, particleD,
                     boostedBmother1, boostedBmother2,
                     boostedBpartA, boostedBpartB,
                     boostedBpartC, boostedBpartD);
      
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
      const Gaudi::XYZVector vecPartA = Gaudi::XYZVector( boostedM1partA );
      const Gaudi::XYZVector vecPartC = Gaudi::XYZVector( boostedM1partC );
      const Gaudi::XYZVector vecPartD = Gaudi::XYZVector( boostedM1partD );
      const Gaudi::XYZVector vecZaxis = (vecPartD.Cross( vecPartC )).unit();

      // Decompose particle A into it's perpendicular and parallel components, 
      // where parallel is in the plane defined by the CD system in mother1's rest frame
      const Gaudi::XYZVector A_perp = vecZaxis * vecPartA.Dot( vecZaxis );
      const Gaudi::XYZVector A_para = vecPartA - A_perp;
      const Gaudi::XYZVector vecMother2 = Gaudi::XYZVector( boostedM1mother2 );

      double phi = acos( ( A_para.unit() ).Dot( vecMother2.unit() ));

      // Solve sign ambiguity - make sure that the cross product of the direction of 
      // mother2 and A_para is the same as the z-axis we have defined
      const Gaudi::XYZVector checknormal = vecMother2.Cross( A_para );

      bool samedirection = ( checknormal.Dot( vecZaxis ) > 0.0);
      if (!samedirection) phi = -phi;
      return phi;
    }
    
    // This function is used in both the helicity AND transversity bases
    double calculateHelicityPolarAngle(const Gaudi::LorentzVector& particleA,
                                       const Gaudi::LorentzVector& particleB,
                                       const Gaudi::LorentzVector& particleC,
                                       const Gaudi::LorentzVector& particleD)
    {  
      // Firstly we want to boost everything to the B rest frame
      Gaudi::LorentzVector boostedBmother1;
      Gaudi::LorentzVector boostedBmother2;
      Gaudi::LorentzVector boostedBpartA;
      Gaudi::LorentzVector boostedBpartB;
      Gaudi::LorentzVector boostedBpartC;
      Gaudi::LorentzVector boostedBpartD;

      boostToBframe( particleA, particleB, particleC, particleD,
                     boostedBmother1, boostedBmother2,
                     boostedBpartA, boostedBpartB,
                     boostedBpartC, boostedBpartD);

      // Now we need to boost to the mother2 frame
      ROOT::Math::Boost boostToMother2( boostedBmother2.BoostToCM() );

      // Do the boost to the mother2 frame
      const Gaudi::LorentzVector boostedM2mother1 = boostToMother2( boostedBmother1 );
      const Gaudi::LorentzVector boostedM2mother2 = boostToMother2( boostedBmother2 );
      const Gaudi::LorentzVector boostedM2partA   = boostToMother2( boostedBpartA );
      const Gaudi::LorentzVector boostedM2partB   = boostToMother2( boostedBpartB );
      const Gaudi::LorentzVector boostedM2partC   = boostToMother2( boostedBpartC );
      const Gaudi::LorentzVector boostedM2partD   = boostToMother2( boostedBpartD );

      // Now find the angle between particleC in it's mothers rest frame (mother2) and the 
      // direction of it's mother in the B candidates rest frame. Equivalently, you can take
      // the angle wrt to the -ve direction of mother1 in mother2's rest frame.
      const Gaudi::XYZVector vecPartC = Gaudi::XYZVector( boostedM2partC ).unit();
      const Gaudi::XYZVector vecMother2InBframe = Gaudi::XYZVector( boostedBmother2 ).unit();
 
      double psi = acos( vecPartC.Dot( vecMother2InBframe ) );      
      return psi;
    }
    

    // Angle between the two decay planes in the rest frame of the Bcand
    double calculatePlaneAngle( const Gaudi::LorentzVector& particleA,
                                const Gaudi::LorentzVector& particleB,
                                const Gaudi::LorentzVector& particleC,
                                const Gaudi::LorentzVector& particleD ) 
    {
      // Firstly we want to boost everything to the B rest frame
      Gaudi::LorentzVector boostedBmother1;
      Gaudi::LorentzVector boostedBmother2;
      Gaudi::LorentzVector boostedBpartA;
      Gaudi::LorentzVector boostedBpartB;
      Gaudi::LorentzVector boostedBpartC;
      Gaudi::LorentzVector boostedBpartD;

      boostToBframe( particleA, particleB, particleC, particleD,
                     boostedBmother1, boostedBmother2,
                     boostedBpartA, boostedBpartB,
                     boostedBpartC, boostedBpartD);

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
  
} // namespace DaVinci 
//=============================================================================
