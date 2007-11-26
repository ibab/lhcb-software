// $Id: TwoProngVertex.cpp,v 1.3 2007-11-26 17:43:44 sean Exp $
// Include files

// local
#include "Event/TwoProngVertex.h"
#include "Event/TrackParameters.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TwoProngVertex
//
// 2007-10-16 : Wouter HULSBERGEN, Sean BRISBANE
//-----------------------------------------------------------------------------

 static void dP4dMom( const ROOT::Math::SVector<double,3>& mom,const double mass, Gaudi::Matrix4x3& J)
{
    double tx = mom(0) ;
    double ty = mom(1) ;
    double qop = mom(2) ;
    double p  = 1/fabs(qop) ;
    double n2 = 1 + tx*tx + ty*ty ;
    double n  = std::sqrt(n2) ;
    double n3 = n2*n ;
    double px = p*tx/n ;
    double py = p*ty/n ;
    double pz = p/n ;
    double E = sqrt(p*p+mass*mass) ;
    
    J(0,0) = p * (1+ty*ty)/n3 ;// dpx/dtx
    J(0,1) = p * tx * -ty/n3  ;// dpx/dty
    J(0,2) = -px/qop ;// dpx/dqop
    J(1,0) = p * ty * -tx/n3  ;// dpy/dtx
    J(1,1) = p * (1+tx*tx)/n3 ;// dpy/dty
    J(1,2) = -py/qop ;// dpy/dqop
    J(2,0) = pz * -tx/n2 ;// dpz/dtx
    J(2,1) = pz * -ty/n2 ;// dpz/dtx
    J(2,2) = -pz/qop ;// dpz/dqop
    J(3,2) = p/E * -p/qop ;// dE/dqop
       return ;
}
  

namespace LHCb 
{
  TwoProngVertex::TwoProngVertex( const Gaudi::XYZPoint& position)
    : LHCb::RecVertex(position)
  {
  }

  const Gaudi::LorentzVector TwoProngVertex::p4(const ROOT::Math::SVector<double,3> & mom, double mass) const
  {
    // first calculate momentum in carthesian coordinates:
    double p = 1/fabs(mom(2)) ;
    double n = sqrt( 1 + mom(0)*mom(0)+mom(1)*mom(1)) ;
    double px = p*mom(0)/n ;
    double py = p*mom(1)/n ;
    double pz = p/n ;
    return Gaudi::LorentzVector(px,py,pz,std::sqrt(p*p+mass*mass)) ;
  }

 
  const Gaudi::SymMatrix9x9 TwoProngVertex::covMatrix9x9() const
  {
    ///< Return the covariance matrix for position and track 1 & track 2 momenta
    Gaudi::SymMatrix9x9 covMatrix;
    covMatrix.Place_at(LHCb::VertexBase::covMatrix(),0,0);
    covMatrix.Place_at(m_momcovA,3,3);
    covMatrix.Place_at(m_momcovB,6,6);

    // covMatrix.Place_at(m_mompposcovA,0,3);
    //  covMatrix.Place_at(m_momposcovB,0,6);
     //  covMatrix.Place_at(m_mommomcov,6,3);
  
   //now to copy off diagonal elements into full 9x9 matrix
    for(int i = 0 ; i <3; ++i){
      for(int j =0;j<3;++j){
        covMatrix(i,j+3) = m_momposcovA(i,j);
      }
    }

    for(int i = 0 ; i <3; ++i){
      for(int j =0;j<3;++j){
        covMatrix(i,j+6) = m_momposcovB(i,j);
      }
    }
    for(int i = 0 ; i <3; ++i){
      for(int j =0;j<3;++j){
        covMatrix(i+3,j+6) = m_mommomcov(i,j);
      }
    }

      return covMatrix;
      
  }
  

  const Gaudi::SymMatrix7x7 TwoProngVertex::covMatrix7x7(const double mass1, const double mass2 ) const
  {
    //< Return the covariance matrix for the V0
    //    Gaudi::LorentzVector V0Mom = momentum(mass1, mass2);
   
Gaudi::SymMatrix7x7 theMatrix;      
if(tracks().size()<2){ throwError("covMatrix7x7 needs at least 2 V0 daughters");
 return theMatrix; }
    //the Jacobians to change momenta of daughters from tx,ty,qop to PxPyPzE space
    Gaudi::Matrix4x3 JacobP1;
    Gaudi::Matrix4x3 JacobP2;
    dP4dMom( m_momA, mass1, JacobP1);
    dP4dMom( m_momB, mass2, JacobP2);
    
    //covmompos contains the transformed covariance matrix for the off diagonal part (position and momentum)
    Gaudi::Matrix4x3 covmompos;
    //. . . . . . . 
    //. . . . . . . 
    //. . . . . . . 
    //x x x . . . . 
    //x x x . . . . 
    //x x x . . . .
    //x x x . . . . 
    covmompos =JacobP1* (m_momposcovA) ;
    covmompos += JacobP2 * m_momposcovB; 

    //covmommom contains the V0 symmetric covariance for the  momentum part
    Gaudi::SymMatrix4x4 covmommom ;
    
    ROOT::Math::AssignSym::Evaluate
      (covmommom,
       (Gaudi::Matrix4x4)  ((JacobP1*m_mommomcov) * ROOT::Math::Transpose(JacobP2)) +
       (Gaudi::Matrix4x4)((JacobP2*m_mommomcov) * (Gaudi::Matrix3x4) ROOT::Math::Transpose(JacobP1))

       );
    covmommom+=ROOT::Math::Similarity(JacobP1, m_momcovA); //J * C * JT
    covmommom+=ROOT::Math::Similarity(JacobP2, m_momcovB);
    
    //theMatrix.Place_at(m_poscov, 0,0);
    theMatrix.Place_at(covmommom, 3,3);
    theMatrix.Place_at(LHCb::VertexBase::covMatrix(), 0,0); //position part from rec vertex

    //now to copy off diagonal elements into full 7x7 matrix
    for(int i = 0 ; i <4; ++i){
      for(int j =0;j<3;++j){
        theMatrix(i+3,j) = covmompos(i,j);
      }
    }
      
    return theMatrix;
  }



 
  
  
}
