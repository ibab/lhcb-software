#ifndef TALIGNMENT_ALPARAMETERS_H
#define TALIGNMENT_ALPARAMETERS_H

#include <iostream>
#include <string>
#include "AlignKernel/AlVec.h"
#include "AlignKernel/AlSymMat.h"
#include "AlDofMask.h"
#include "Math/Transform3D.h"
#include "Math/SMatrix.h"
#include "boost/array.hpp"
#include "GaudiKernel/SymmetricMatrixTypes.h"

class AlParameters
{
public:
  /** Enums, typedefs */
  enum { Tx=0, Ty, Tz, Rx, Ry, Rz, NumPars = 6 } EParameter ;
  typedef AlVec                             Vector ;
  typedef AlSymMat                          Covariance ;
  typedef ROOT::Math::SMatrix<double, 6, 6> Matrix6x6 ;
  typedef ROOT::Math::SVector<double, 6>    TransformParameters ;
  typedef Gaudi::SymMatrix6x6               TransformCovariance ;
  typedef AlDofMask                         DofMask ; 

  /** Constructors */
  AlParameters(size_t size = NumPars) ;
  AlParameters(DofMask mask) ;
  AlParameters(const Vector& parameters, const Covariance& covariance, DofMask mask=DofMask(NumPars), size_t offset = 0u) ;
  AlParameters(const Vector& parameters, const Covariance& covariance, const Covariance& weightmatrix,
	       DofMask mask=DofMask(NumPars), size_t offset = 0u) ;
  AlParameters(const double parameters[6], DofMask mask=DofMask(NumPars)) ;
  AlParameters(const TransformParameters& parameters, DofMask mask=DofMask(NumPars)) ;
  AlParameters(const ROOT::Math::Transform3D& transform, DofMask mask=DofMask(NumPars)) ;
  AlParameters(const TransformParameters& parameters, const TransformCovariance& covariance) ;
  
  size_t dim() const { return m_mask.nActive() ; }
  static std::string parName( int ) ;
  std::vector<double> translation() const ;
  std::vector<double> errTranslation() const;
  std::vector<double> rotation() const ;
  std::vector<double> errRotation() const;
  ROOT::Math::Transform3D transform() const ;
  // create the transform for a given delta
  static ROOT::Math::Transform3D transform(const TransformParameters& pars) ;
  // create a delta from a transform
  static TransformParameters transformParameters(const ROOT::Math::Transform3D& T ) ;
  // compute the delta jacobian for a given coordinate transformation
  static Matrix6x6 jacobian(const ROOT::Math::Transform3D& transform) ;
  // same but now numerical
  static Matrix6x6 jacobianNumeric(const ROOT::Math::Transform3D& transform) ;
  // dump to stream
  std::ostream& fillStream(std::ostream& s) const;
  // get a new set of parameters in a new frame
  AlParameters AlParameters::transformTo( const ROOT::Math::Transform3D& T ) const;
  
  std::string activeParName( int iactive ) const { return parName( m_mask.parIndex(iactive) ) ; }
  const Vector& parameters() const { return m_parameters ; }
  const Covariance& covariance() const { return m_covariance ; }
  double globalCorrelationCoefficient(int iactive) const ;
  double measurementCoordinateSigma(double weightR) const ;
  TransformParameters transformParameters() const ;
  TransformCovariance transformCovariance() const ;
  // returns a vector with the sqrt of the diagonal elements of the cov matrix
  TransformParameters transformErrors() const ;
  static double signedSqrt(double root) ;
private:
  DofMask     m_mask;
  Vector      m_parameters ;
  Covariance  m_covariance ;
  Covariance  m_weightmatrix ;
} ;

inline  std::ostream& operator<<(std::ostream& os, const AlParameters& par) {
  return par.fillStream(os) ;
}

#endif
