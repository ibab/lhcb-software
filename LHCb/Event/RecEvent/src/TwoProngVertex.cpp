// $Id: TwoProngVertex.cpp,v 1.4 2007-11-27 18:25:51 sean Exp $
// Include files

// local
#include "Event/TwoProngVertex.h"
#include "Event/TrackParameters.h"
//using namespace Gaudi::Math;
//#include "/afs/cern.ch/user/s/sean/cmtuserV0/Brunel_v31r9/InstallArea/include/LHCbMath/LorentzUtils.h"

//-----------------------------------------------------------------------------
// Implementation file for class : TwoProngVertex
//
// 2007-10-16 : Wouter HULSBERGEN, Sean BRISBANE
//-----------------------------------------------------------------------------


  

namespace LHCb 
{
  TwoProngVertex::TwoProngVertex( const Gaudi::XYZPoint& position)
    : LHCb::RecVertex(position)
  {
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

// std::cout << " silly " << Gaudi::Math::aSillyNumber;

    Gaudi::Matrix4x3 JacobP1;
    Gaudi::Matrix4x3 JacobP2;
    Gaudi::Math::JacobdP4dMom( m_momA, mass1, JacobP1);
    Gaudi::Math::JacobdP4dMom( m_momB, mass2, JacobP2);


    
    
       
       // fill Linear Algebra vector from 3D-point
  
      Gaudi::Matrix4x3    mtrx1;
       Gaudi::Math::scale ( mtrx1 , 100.0   ) ;

    
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
