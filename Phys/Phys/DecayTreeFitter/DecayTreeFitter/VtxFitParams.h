//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: VtxFitParams.h,v 1.4 2009-05-12 07:27:34 wouter Exp $
//
// Description:
//	Class VtxFitParams
//
// 
//
//------------------------------------------------------------------------

#ifndef VtxFitParams_H
#define VtxFitParams_H

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
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "DecayTreeFitter/VtxDoubleErr.h"
#include "DecayTreeFitter/LorentzVectorErr.h"

namespace Gaudi
{
  typedef ROOT::Math::SMatrix<double, 1, 4> Matrix1x4;
}

namespace LHCb
{
  class VtxFitParams
  {
  public:
    VtxFitParams() {}
    
    // constructor from a full parameter set
    VtxFitParams( double charge,
		  const Gaudi::XYZPoint& pos,
		  const Gaudi::LorentzVector& p4,
		  double decaylength,
		  const Gaudi::SymMatrix8x8& cov8) ;

    // constructor from position and momentum. the decay length is set to 0
    VtxFitParams( double charge,
		  const Gaudi::XYZPoint& pos,
		  const Gaudi::LorentzVector& p4,
		  const Gaudi::SymMatrix7x7& cov7) ;

    // access to position
    const Gaudi::XYZPoint& position() const { return m_pos ; }
    const Gaudi::SymMatrix3x3& posCovMatrix() const { return m_posCovMatrix ; }
    
    // access to p4
    const Gaudi::LorentzVector& p4() const { return m_p4 ; }
    const Gaudi::SymMatrix4x4& momCovMatrix() const { return m_momCovMatrix ; }
    LorentzVectorErr p4Err() const { return LorentzVectorErr(m_p4,m_momCovMatrix) ; }
    
    // access to decay length
    double decayLength() const { return m_len ; }
    VtxDoubleErr decayLengthErr() const { return VtxDoubleErr( m_len, m_lenCovMatrix(0,0)) ; }

    // access to momPosCovMatrix
    const Gaudi::Matrix4x3& momPosCovMatrix() const { return m_momPosCovMatrix ; }

    // return the covariance matrix for position, p4 and decaylength
    Gaudi::SymMatrix8x8 cov8() const ;

    // return a vector with position, p4 and decaylength
    Gaudi::Vector8 par8() const ;

    // set the mass. this is effectively a 'mass constraint' fit. it
    // returns the chisquare of the mass constraint.
    double setMass( double fixedmass ) ;

    // compute the proper decay time with a constrained mass
    VtxDoubleErr properDecayTime( double fixedmass ) const ;
    
    // compute the properdecaytime without a constrained mass
    VtxDoubleErr properDecayTime() const ;
    
  private:    
    double m_q;
    Gaudi::XYZPoint      m_pos;
    Gaudi::LorentzVector m_p4;
    double               m_len ;
    Gaudi::SymMatrix3x3  m_posCovMatrix;    ///< Covariance matrix relative to point at which the momentum is given (3x3)
    Gaudi::SymMatrix4x4  m_momCovMatrix;    ///< Covariance matrix relative to momentum (4x4)
    Gaudi::SymMatrix1x1  m_lenCovMatrix;    ///< Covariance matrix of the decay length
    Gaudi::Matrix4x3     m_momPosCovMatrix; ///< Matrix with correlation errors between momemtum and pointOnTrack (momentum x point)
    Gaudi::Matrix1x3     m_lenPosCovMatrix ;
    Gaudi::Matrix1x4     m_lenMomCovMatrix ;
  };  
} ;
#endif
