//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: VtxFitParams.h,v 1.3 2009-05-08 15:08:12 wouter Exp $
//
// Description:
//	Class VtxFitParams
//
// 
//
//------------------------------------------------------------------------

#ifndef VtxFitParams_hh
#define VtxFitParams_hh

//----------------------
// Base Class Headers --
//----------------------

//---------------
// C++ Headers --
//---------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "DecayTreeFitter/VtxDoubleErr.h"
#include "DecayTreeFitter/LorentzVectorErr.h"


namespace LHCb
{
  class VtxFitParams
  {
  public:
    
    VtxFitParams() {}
    
    // this one takes the full 7X7 covariance
    VtxFitParams( double charge,
		  const Gaudi::XYZPoint& pos,
		  const Gaudi::LorentzVector& p4,
		  const Gaudi::SymMatrix7x7& cov7 )
      : m_q(charge),m_pos(pos),m_p4(p4),m_cov7(cov7) {}
    
    double mass() const { return m_p4.M() ; }
    double mom() const { return m_p4.P() ; }
    double massErr() const { return p4Err().MErr() ; }
    double momErr() const { return p4Err().PErr() ; }
    
    LorentzVectorErr p4Err() const { return LorentzVectorErr(m_p4,momCovMatrix()) ; }
    
    const Gaudi::LorentzVector& p4() const { return m_p4 ; }
    VtxDoubleErr decayLength() const { return m_decayLength ; }
    void setDecayLength(const VtxDoubleErr& len) { m_decayLength = len ; }

    Gaudi::SymMatrix4x4 momCovMatrix() const { 
      Gaudi::SymMatrix4x4 rc ;
      for(size_t row=0; row<4; ++row)
	for(size_t col=0; col<=row; ++col)
	  rc(row,col) = m_cov7(row+3,col+3) ;
      return rc ;
    }
    
  private:    
    double m_q;
    Gaudi::XYZPoint      m_pos;
    Gaudi::LorentzVector m_p4;
    VtxDoubleErr         m_decayLength ;

    //Gaudi::SymMatrix4x4  m_momCovMatrix;    ///< Covariance matrix relative to momentum (4x4)
    //Gaudi::SymMatrix3x3  m_posCovMatrix;    ///< Covariance matrix relative to point at which the momentum is given (3x3)
    //Gaudi::Matrix4x3     m_posMomCovMatrix; ///< Matrix with correlation errors between momemtum and pointOnTrack (momentum x point)
    //Gaudi::Matrix8x1     m_decayLengthCovMatrix ;
    
    // the 7X7 covariance
    Gaudi::SymMatrix7x7 m_cov7 ;
  };  
} ;
#endif




