#ifndef TALIGNMENT_ALPARAMETERS_H
#define TALIGNMENT_ALPARAMETERS_H

#include <iostream>
#include <string>
#include "SolvKernel/AlVec.h"
#include "SolvKernel/AlSymMat.h"
#include "AlDofMask.h"
#include "Math/Transform3D.h"
#include "Math/SMatrix.h"
#include "boost/array.hpp"

class AlParameters
{
public:
  /** Enums, typedefs */
  enum { Tx=0, Ty, Tz, Rx, Ry, Rz, NumPars = 6 } EParameter ;
  typedef AlVec                             Vector ;
  typedef AlSymMat                          Covariance ;
  typedef ROOT::Math::SMatrix<double, 6, 6> Matrix6x6 ;
  typedef boost::array<double,NumPars>      TransformParameters ;
  typedef AlDofMask<6>                      DofMask ; 

  /** Constructors */
  AlParameters(DofMask mask=DofMask()) ;
  AlParameters(const Vector& parameters, const Covariance& covariance, DofMask mask=DofMask(), size_t offset = 0u) ;
  AlParameters(const Vector& parameters, const Covariance& covariance, const Covariance& weightmatrix,
	       DofMask mask=DofMask(), size_t offset = 0u) ;
  AlParameters(double parameters[6], DofMask mask=DofMask()) ;
  AlParameters(const TransformParameters& parameters, DofMask mask=DofMask()) ;
  
  size_t dim() const { return m_mask.nActive() ; }
  static std::string parName( int ) ;
  std::vector<double> translation() const ;
  std::vector<double> errTranslation() const;
  std::vector<double> rotation() const ;
  std::vector<double> errRotation() const;
  ROOT::Math::Transform3D transform() const ;
  // create the transform for a given delta
  static ROOT::Math::Transform3D transform(const boost::array<double, 6>& pars) ;
  // compute the delta jacobian for a given coordinate transformation
  static Matrix6x6 jacobian(const ROOT::Math::Transform3D& transform) ;
  static Matrix6x6 jacobianNumeric(const ROOT::Math::Transform3D& transform) ;
  std::ostream& fillStream(std::ostream& s) const;

  std::string activeParName( int iactive ) const { return parName( m_mask.parIndex(iactive) ) ; }
  const Vector& parameters() const { return m_parameters ; }
  const Covariance& covariance() const { return m_covariance ; }
  double globalCorrelationCoefficient(int iactive) const ;
  double measurementCoordinateSigma(double weightR) const ;
  TransformParameters parameterArray() const ;
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
