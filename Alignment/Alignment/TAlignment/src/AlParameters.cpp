#include "AlParameters.h"
#include <cmath>
#include <algorithm>

#include "Math/RotationZYX.h"
#include "Math/RotationX.h"
#include "Math/RotationY.h"
#include "Math/RotationZ.h"
#include "Math/Vector3D.h"
#include "Math/SVector.h"

#include "DetDesc/3DTransformationFunctions.h"

AlParameters::AlParameters(size_t size) 
  : m_mask(size), m_parameters(size), m_covariance(size) 
{
}

AlParameters::AlParameters(DofMask mask) 
  : m_mask(mask), m_parameters(dim()), m_covariance(dim()) 
{
}

AlParameters::AlParameters(const Vector& parameters, const Covariance& covariance, DofMask mask, size_t offset) 
  : m_mask(mask), m_parameters(dim()), m_covariance(dim()), m_weightmatrix(dim())
{
  for (unsigned int i = 0u; i < dim(); ++i) {
    m_parameters[i] = parameters[i+offset] ;
    for (unsigned int j = 0u; j <= i; ++j)
      m_covariance[i][j] = covariance[offset+i][offset+j] ;
  }
}

AlParameters::AlParameters(const Vector& parameters, const Covariance& covariance, 
			   const Covariance& weightmatrix, DofMask mask, size_t offset) 
  : m_mask(mask), m_parameters(dim()), m_covariance(dim()), m_weightmatrix(dim())
{
  for (unsigned int i = 0u; i < dim(); ++i) {
    m_parameters[i] = parameters[i+offset] ;
    for (unsigned int j = 0u; j <= i; ++j) {
      m_covariance[i][j] = covariance[offset+i][offset+j] ;
      m_weightmatrix[i][j] = weightmatrix[offset+i][offset+j] ;
    }
  }
}

AlParameters::AlParameters(const TransformParameters& parameters, const TransformCovariance& covariance) 
  : m_mask(NumPars), m_parameters(6), m_covariance(6), m_weightmatrix(6)
{
  for (unsigned int i = 0u; i < dim(); ++i) {
    m_parameters[i] = parameters[i] ;
    for (unsigned int j = 0u; j <= i; ++j)
      m_covariance[i][j] = covariance[i][j] ;
  }
}

AlParameters::AlParameters(const double parameters[6], DofMask mask)
  : m_mask(mask), m_parameters(dim()), m_covariance(dim())
{
  for( unsigned int i = 0u; i < dim(); ++i) 
    m_parameters[i] = parameters[mask.parIndex(i)] ;
}

AlParameters::AlParameters(const TransformParameters& parameters, DofMask mask)
  : m_mask(mask), m_parameters(dim()), m_covariance(dim())
{
  for( unsigned int i = 0u; i < dim(); ++i) 
    m_parameters[i] = parameters[mask.parIndex(i)] ;
}

AlParameters::AlParameters(const ROOT::Math::Transform3D& transform, DofMask mask)
  : m_mask(mask), m_parameters(dim()), m_covariance(dim())
{
  TransformParameters parameters = transformParameters( transform ) ;
  for( unsigned int i = 0u; i < dim(); ++i) 
    m_parameters[i] = parameters[mask.parIndex(i)] ;
}

void
AlParameters::setParameters( const TransformParameters& parameters )
{
  for( unsigned int i = 0u; i < dim(); ++i)
    m_parameters[i] = parameters[m_mask.parIndex(i)] ;
}

std::string AlParameters::parName(int parindex)
{
  std::string rc ;
  switch(parindex) {
  case Tx: rc = "Tx" ; break ;
  case Ty: rc = "Ty" ; break ;
  case Tz: rc = "Tz" ; break ;
  case Rx: rc = "Rx" ; break ;
  case Ry: rc = "Ry" ; break ;
  case Rz: rc = "Rz" ; break ;
  default: rc = "unknown" ;
  }
  return rc ;
}

double AlParameters::signedSqrt(double root)
{
  return root > 0 ? std::sqrt(root) : ( root < 0  ? - std::sqrt(-root) : root ) ;
}

std::ostream& AlParameters::fillStream(std::ostream& os) const 
{    
  for (unsigned int iactive = 0u; iactive < dim(); ++iactive) {
    os << std::setw(6)  << parName(m_mask.parIndex(iactive)) << ": " 
       << std::setw(12) << m_parameters[iactive] << " +/- "
       << std::setw(12) << signedSqrt(m_covariance[iactive][iactive]) << std::endl ;
  }
  return os ;
}

std::vector<double> AlParameters::translation() const
{
  std::vector<double> rc(3, 0.0) ;
  rc[Tx] = m_mask.isActive(Tx) ? m_parameters[m_mask.activeParIndex(Tx)] : 0 ;
  rc[Ty] = m_mask.isActive(Ty) ? m_parameters[m_mask.activeParIndex(Ty)] : 0 ;
  rc[Tz] = m_mask.isActive(Tz) ? m_parameters[m_mask.activeParIndex(Tz)] : 0 ;
  return rc ;
}

std::vector<double> AlParameters::errTranslation() const {
  std::vector<double> errT(3, 0.0);
  errT[Tx] = m_mask.isActive(Tx) ? signedSqrt(m_covariance[m_mask.activeParIndex(Tx)][m_mask.activeParIndex(Tx)]) : 0.0;
  errT[Ty] = m_mask.isActive(Ty) ? signedSqrt(m_covariance[m_mask.activeParIndex(Ty)][m_mask.activeParIndex(Ty)]) : 0.0;
  errT[Tz] = m_mask.isActive(Tz) ? signedSqrt(m_covariance[m_mask.activeParIndex(Tz)][m_mask.activeParIndex(Tz)]) : 0.0;
  return errT;
}

std::vector<double> AlParameters::rotation() const
{
  std::vector<double> rc(3, 0.0) ;
  rc[Rx-3] = m_mask.isActive(Rx) ? m_parameters[m_mask.activeParIndex(Rx)] : 0 ;
  rc[Ry-3] = m_mask.isActive(Ry) ? m_parameters[m_mask.activeParIndex(Ry)] : 0 ;
  rc[Rz-3] = m_mask.isActive(Rz) ? m_parameters[m_mask.activeParIndex(Rz)] : 0 ;
  return rc ;
}

std::vector<double> AlParameters::errRotation() const {
  std::vector<double> errR(3, 0.0);
  errR[Rx-3] = m_mask.isActive(Rx) ? signedSqrt(m_covariance[m_mask.activeParIndex(Rx)][m_mask.activeParIndex(Rx)]) : 0.0;
  errR[Ry-3] = m_mask.isActive(Ry) ? signedSqrt(m_covariance[m_mask.activeParIndex(Ry)][m_mask.activeParIndex(Ry)]) : 0.0;
  errR[Rz-3] = m_mask.isActive(Rz) ? signedSqrt(m_covariance[m_mask.activeParIndex(Rz)][m_mask.activeParIndex(Rz)]) : 0.0;
  return errR;
}

AlParameters::TransformParameters AlParameters::transformParameters() const 
{
  TransformParameters pars ;
  for (size_t iactive = 0u; iactive < dim(); ++iactive) pars[m_mask.parIndex(iactive)] = m_parameters[iactive] ;
  return pars ;
}

AlParameters::TransformCovariance AlParameters::transformCovariance() const 
{
  TransformCovariance cov ;
  for (size_t iactive = 0u; iactive < dim(); ++iactive) 
    for (size_t jactive = 0u; jactive <= iactive; ++jactive) 
      cov(m_mask.parIndex(iactive), m_mask.parIndex(jactive)) = m_covariance(iactive,jactive) ;
  return cov ;
}

AlParameters::TransformParameters AlParameters::transformErrors() const 
{
  TransformParameters err ;
  for (size_t iactive = 0u; iactive < dim(); ++iactive) 
    err(m_mask.parIndex(iactive)) = std::sqrt(m_covariance(iactive,iactive)) ;
  return err ;
}

ROOT::Math::Transform3D AlParameters::transform() const
{
  return transform( transformParameters() ) ;
}

ROOT::Math::Transform3D AlParameters::transform(const TransformParameters& pars)
{
  ROOT::Math::Transform3D translation(ROOT::Math::XYZVector(pars[0], pars[1], pars[2])) ;
  ROOT::Math::RotationZYX rotation = ROOT::Math::RotationZYX( pars[5], pars[4],pars[3]);
  //ROOT::Math::Transform3D rotation( ROOT::Math::RotationX(pars[3])*
  //ROOT::Math::RotationY(pars[4])*
  //ROOT::Math::RotationZ(pars[5])) ;
  return translation * rotation ;
}

AlParameters::TransformParameters AlParameters::transformParameters(const ROOT::Math::Transform3D& T ) 
{
  std::vector<double> translations(3,0.0), rotations(3,0.0);
  DetDesc::getZYXTransformParameters(T, translations, rotations);
  TransformParameters rc ;
  for( unsigned int i = 0u; i <3; ++i) {
    rc[i] = translations[i] ;
    rc[i+3] = rotations[i] ;
  }
  return rc ;
}


AlParameters AlParameters::transformTo( const ROOT::Math::Transform3D& T ) const
{
  // T is the transform that brings us from the frame in which these
  // parameters are defined to the new frame
  ROOT::Math::Transform3D Tinv = T.Inverse() ;
  ROOT::Math::Transform3D oldtransform = transform() ;
  ROOT::Math::Transform3D newtransform = T * oldtransform * Tinv ;

  TransformCovariance oldcov = transformCovariance() ;
  Matrix6x6 jac = jacobian(T) ;
  TransformCovariance newcov = ROOT::Math::Similarity( jac, oldcov ) ;
  TransformParameters newpars = transformParameters( newtransform ) ;

  return AlParameters( newpars, newcov ) ;
}


AlParameters::Matrix6x6 AlParameters::jacobian( const ROOT::Math::Transform3D& transform)
{
  // Suppose f( x ) is the transform corresponding to parameters x
  // (see routine AlParameters::transform).  This routine returns the
  // jacobian for a transformation of f: if x' is the delta in frame 0
  // and x is the delta in frame 1 and A is the transform from frame 0
  // to frame 1, then this returns the matrix Mij = dx_i / dx'_j, by taking the
  // derivative of
  //   f'(x')   =    A * f(x) * A^{-1}
  //

  typedef ROOT::Math::SMatrix<double, 3, 3> Matrix3x3 ;
  typedef ROOT::Math::SVector<double, 3>    Vector3;

  // first decompose the transform. cannot do this quicker because of old ROOT version.
  Matrix3x3 R ;
  Vector3 T ;
  transform.GetComponents(R(0,0),R(0,1),R(0,2),T(0),
			  R(1,0),R(1,1),R(1,2),T(1),
			  R(2,0),R(2,1),R(2,2),T(2)) ;

  // We also need the inverse of R, which is of course just its transpose:
  Matrix3x3 Rinv = ROOT::Math::Transpose(R) ;
  //invR.Invert() ;
  
  // const matrices for derivative of rotations to rotation pars
  Matrix3x3 drot[3] ;
  drot[0](2,1) = drot[1](0,2) = drot[2](1,0) = 1 ;
  drot[0](1,2) = drot[1](2,0) = drot[2](0,1) = -1 ;
  
  // R ( deps ) R^{-1} for each three components
  Matrix3x3 RdrotR[3] ;
  // this breaks in optimization on SLC5. the solution was to create a
  // temporary in the loop. (Thanks Lorenzo)
  for(int i=0; i<3; ++i) {
    Matrix3x3 tmp = drot[i] * Rinv ;
    RdrotR[i] = R * tmp ;
  }

  //   for(int i=0; i<3; ++i)
  //     for(int irow=0; irow<3; ++irow)
  //       for(int krow=0; krow<3; ++krow)
  // 	for(int lrow=0; lrow<3; ++lrow)
  // 	  for(int jrow=0; jrow<3; ++jrow)
  // 	    RdrotR[i](irow,jrow) += R(irow,krow) * drot[i](krow,lrow) * Rinv(lrow,jrow) ;
  
  // now construct all components of the jacobian
  Matrix6x6 jacobian ;
  
  // the derivative of the rotation to the translation (deps/deta') is zero.

  // the derivative of the translation to the translation (deta/deta')
  jacobian.Place_at( R, 0, 0) ;
  
  // the derivatives to the rotation are a bit more complicated. let's
  // first do the rotation to the rotation:
  for(int i=0; i<3; ++i) {
    jacobian(3,3+i) = RdrotR[i](2,1) ;
    jacobian(4,3+i) = RdrotR[i](0,2) ;
    jacobian(5,3+i) = RdrotR[i](1,0) ;
  }
  // SO, if you just think about it for a minute, you realize that also the delta-rotation should transform with the rotation:
  //jacobian.Place_at( R, 3, 3) ;

  // the only thing left is the derivative of the translation to the
  // rotation. it's the only part that involves T:
  for(int i=0; i<3; ++i) {
    Vector3 tmp = RdrotR[i] * T ;
    for(int j=0; j<3; ++j) jacobian(j,i+3) = - tmp(j) ;
  }
  
  // Het staat er nu zo kort, dat je geen idee meer hebt hoeveel werk
  // de algebra was ...
  return jacobian ;
}


static inline void deltaFromTransform(const ROOT::Math::Transform3D& transform,
				      double delta[6])
{
  // this is really only correct in the limit delta -->0! Otherwise,
  // need to use Juan's tools. For now, we only need this in the
  // routine below for which this is just fine.
  ROOT::Math::SMatrix<double, 3, 3> R ;
  transform.GetComponents(R(0,0),R(0,1),R(0,2),delta[0],
			  R(1,0),R(1,1),R(1,2),delta[1],
			  R(2,0),R(2,1),R(2,2),delta[2]) ;
  delta[3] = R(2,1) ;
  delta[4] = R(0,2) ;
  delta[5] = R(1,0) ;
}

AlParameters::Matrix6x6 AlParameters::jacobianNumeric( const ROOT::Math::Transform3D& T)
{
  // calculate the jacobian numerically
  AlParameters::Matrix6x6 jacobian ;
  // what is the relevant scale?
  const double epsilon = 1e-6 ;
  
  ROOT::Math::Transform3D Tinv = T.Inverse() ;
  
  for(int j=0; j<6; ++j) {
    TransformParameters delta ;
    delta[j] += epsilon ;
    ROOT::Math::Transform3D Delta      = transform(delta) ;
    ROOT::Math::Transform3D DeltaPrime = T * Delta * Tinv ;
    double deltaprime[6] ;
    deltaFromTransform(DeltaPrime,deltaprime) ;
    for(int i=0; i<6; ++i) jacobian(i,j) = deltaprime[i]/epsilon ;
  }
  return jacobian ;
}


double AlParameters::globalCorrelationCoefficient(int iactive) const 
{
  double varweight =  m_covariance[iactive][iactive] *  m_weightmatrix[iactive][iactive] ;
  return varweight>1 ? std::sqrt(1 - 1/varweight) : 0  ;
}

double AlParameters::measurementCoordinateSigma(double weightR) const 
{
  double sum(0) ;
  for(size_t iactive=0; iactive<dim(); ++iactive)  
    for(size_t jactive=0; jactive<dim(); ++jactive)  
      sum += m_covariance[iactive][jactive] *  m_weightmatrix[iactive][jactive] ;
  return (sum/weightR)>0 ? std::sqrt(sum/weightR) : 0;
}
