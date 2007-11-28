// $Id: TwoProngVertex.cpp,v 1.6 2007-11-28 16:00:29 wouter Exp $
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
    
    //the Jacobians to change momenta of daughters from tx,ty,qop to PxPyPzE space
    Gaudi::Matrix4x3 JacobP1;
    Gaudi::Matrix4x3 JacobP2;
    Gaudi::Math::JacobdP4dMom( m_momA, mass1, JacobP1);
    Gaudi::Math::JacobdP4dMom( m_momB, mass2, JacobP2);
    
    //covmompos contains the transformed covariance matrix for the off diagonal part (position and momentum)
    Gaudi::Matrix4x3 covmompos;
    covmompos =  JacobP1*  m_momposcovA;
    covmompos += JacobP2 * m_momposcovB; 
    
    //covmommom contains the V0 symmetric covariance for the  momentum part
    Gaudi::Matrix4x4 covmommom12 = JacobP1 * m_mommomcov * ROOT::Math::Transpose(JacobP2) ;
    Gaudi::SymMatrix4x4 covmommom ;
    ROOT::Math::AssignSym::Evaluate(covmommom, covmommom12 + ROOT::Math::Transpose(covmommom12)) ;
    covmommom+=ROOT::Math::Similarity(JacobP1, m_momcovA); //J * C * JT
    covmommom+=ROOT::Math::Similarity(JacobP2, m_momcovB);
    
    // finally, fill the returned matrix
    Gaudi::SymMatrix7x7 theMatrix;
    // position part from rec vertex
    theMatrix.Place_at(LHCb::VertexBase::covMatrix(), 0,0);
    // momentum part
    theMatrix.Place_at(covmommom, 3,3);
    // off diagonal elements
    for(int i = 0 ; i <4; ++i)
      for(int j =0;j<3;++j)
        theMatrix(i+3,j) = covmompos(i,j);
    
    return theMatrix;
  }



 
  
  
}
