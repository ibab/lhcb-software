//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: LorentzVectorErr.h,v 1.1 2009-05-12 07:32:01 wouter Exp $
//
// Description:
//	Add errors to a LorentzVector.  Used for direction errors 
//      BaBar native class
//
// Environment:
//	Software developed for the BaBar Detector at the SLAC B-Factory.
//
// Author List:
//      Abi Soffer
//
// History
//      Mon Apr 27 00:28:13 PDT 1998, created from BbrVectorErr.hh, Abi Soffer
//
// Copyright Information:
//	Copyright (C) 1998
//
//------------------------------------------------------------------------

#ifndef DECAYTREEFITTER_LORENTZVECTORERR_HH
#define DECAYTREEFITTER_LORENTZVECTORERR_HH

#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/Vector4DTypes.h"

namespace LHCb {

  class LorentzVectorErr : public Gaudi::LorentzVector 
  {
  public:
    typedef ROOT::Math::SMatrix<double, 1, 4> Matrix1x4;

    LorentzVectorErr() {}
    
    LorentzVectorErr(const Gaudi::LorentzVector &p, const Gaudi::SymMatrix4x4& covMat) : 
      Gaudi::LorentzVector(p),m_covMatrix(covMat) {}
    
    double PzErr() const { return std::sqrt( m_covMatrix(2,2)) ; }
    double PxErr() const { return std::sqrt( m_covMatrix(0,0)) ; }
    double PyErr() const { return std::sqrt( m_covMatrix(1,1)) ; }

    double MErr() const 
    {
      Matrix1x4 jacobian ;
      double m      =  M() ;
      jacobian(0,0) = - Px()/m ;
      jacobian(0,1) = - Py()/m ;
      jacobian(0,2) = - Pz()/m ;
      jacobian(0,3) =   E()/m ;
      return std::sqrt(ROOT::Math::Similarity( jacobian, m_covMatrix )(0,0)) ;
    }
    
    double PErr() const 
    {
      Matrix1x4 jacobian ;
      double p      =  P() ;
      jacobian(0,0) =  Px()/p ;
      jacobian(0,1) =  Py()/p ;
      jacobian(0,2) =  Pz()/p ;
      jacobian(0,3) =  0 ;
      return std::sqrt(ROOT::Math::Similarity( jacobian, m_covMatrix )(0,0)) ;
    }
private:
  Gaudi::SymMatrix4x4 m_covMatrix ;
  };
}


#endif






