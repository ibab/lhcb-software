//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: VtxFitParams.h,v 1.2 2009-05-07 14:14:55 wouter Exp $
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
    
    const Gaudi::LorentzVector& p4() const { return m_p4 ; }
    VtxDoubleErr decayLength() const { return m_decayLength ; }
    void setDecayLength(const VtxDoubleErr& len) { m_decayLength = len ; }

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




