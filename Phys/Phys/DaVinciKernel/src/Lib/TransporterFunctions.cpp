// $Id: TransporterFunctions.cpp,v 1.2 2010-05-16 11:41:53 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// GauduKernel
// ============================================================================
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
// ============================================================================
// LHcbMath
// ============================================================================
#include "LHCbMath/MatrixUtils.h"
#include "LHCbMath/MatrixTransforms.h"
// ============================================================================
// local
// ============================================================================
#include "Kernel/TransporterFunctions.h"
// ============================================================================
/** @file
 *  Impplementation file for function DaVinci::Transporter
 */
// ============================================================================
/* Transport a composite Particle to specified z position, using linear
 *  extrapolation.
 *
 *  @author Juan  PALACIOS Juan.Palacios@nikhef.nl
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date   2008-03-10
 */
// ============================================================================
StatusCode DaVinci::Transporter::transportAndProjectComposite 
( const LHCb::Particle* particle      , 
  const double          zNew          ,
  LHCb::Particle&       transParticle ) 
{
  using namespace Gaudi;
  
  const double dZ = zNew-particle->referencePoint().Z();
  const double Px = particle->momentum().X();
  const double Py = particle->momentum().Y();
  const double Pz = particle->momentum().Z();
  const double coef = dZ/Pz;
  
  // Transport matrix position-momentum block
  Matrix3x4 Dxp;
  Dxp(0,0) = coef;
  Dxp(0,2) = -1*coef*Px/Pz;
  Dxp(1,1) = coef;
  Dxp(1,2) = -1*coef*Py/Pz;
  
  const Matrix4x3 DxpT = ROOT::Math::Transpose(Dxp);
  
  // Transport matrix position block
  Matrix3x3 A;
  A(0,0) = 1.;
  A(1,1) = 1.;
  A(0,2) = -1.*Px/Pz;
  A(1,2) = -1.*Py/Pz;
  
  const Matrix3x3 AT = ROOT::Math::Transpose(A);
  
  // Old covariance matrix blocks
  const SymMatrix3x3  Cx_0  = particle->posCovMatrix();
  const Matrix4x3     Cpx_0 = particle->posMomCovMatrix();
  const SymMatrix4x4  Cp_0  = particle->momCovMatrix();
  const Matrix3x4     Cxp_0 = ROOT::Math::Transpose(Cpx_0);
  
  // New covariance matrix blocks
  const Matrix3x3 Cx_1  = 
    A*Cx_0*AT + Matrix3x3(A*Cxp_0*DxpT) +
    Matrix3x3(Dxp*Cpx_0*AT) + 
    Matrix3x3( Dxp*Cp_0*DxpT );
  
  const Matrix4x3 Cpx_1 = Cpx_0*AT + Matrix4x3(Cp_0*DxpT);
  
  // Update covariance matrices in particle
  transParticle.setPosCovMatrix( Cx_1.LowerBlock() );
  transParticle.setPosMomCovMatrix ( Cpx_1 );
  
  // calculate reference point
  Gaudi::XYZPoint refPoint(particle->referencePoint());
  refPoint += dZ*particle->slopes();
  
  // update reference point
  transParticle.setReferencePoint(refPoint);
  
  return StatusCode::SUCCESS;
  
}
// ============================================================================
/* Transport a composite Particle to specified z position, 
 *   using the linear extrapolation
 *
 *  @author Juan  PALACIOS Juan.Palacios@nikhef.nl
 *  @author Vanya BELYAEV  Ivan.Belyaev@nikhef.nl
 *  @date   2008-03-10
 */  
// ============================================================================
StatusCode DaVinci::Transporter::transportComposite 
( const LHCb::Particle* particle     , 
  const double          zNew         ,
  LHCb::Particle&       transported  ) 
{
  // check the argument:
  if ( 0 == particle ) { return StatusCode::FAILURE ; } // RETURN 
  
  const double zOld    = particle -> referencePoint().Z() ;
  // no need to transport:
  if ( zNew == zOld ) /// @toto add a real comparison of float values  
  {
    if ( &transported != particle ) { transported = *particle ; }
    return StatusCode::SUCCESS ;
    }
  
  const double deltaZ  = zNew - zOld ;
  
  
  const Gaudi::LorentzVector& v = particle->momentum () ;
  
  // the only non-trivial component of the Jacobian
  Gaudi::Matrix3x4 F ; 
  const double coeff = deltaZ   / v.Pz () ; // delta(z)/p_z 
  
  F ( 0 , 0 ) =  coeff                   ;  // d(x_1)/d(p_x0) 
  F ( 0 , 2 ) = -coeff * v.Px() / v.Pz() ;  // d(x_1)/d(p_z0)
  F ( 1 , 1 ) =  coeff                   ;  // d(y_1)/d(p_y0) 
  F ( 1 , 2 ) = -coeff * v.Py() / v.Pz() ;  // d(y_1)/d(p_z0)
  
  // Linear transformation:
  
  // redefine the quantities:
  transported.setMomentum       
    ( particle -> momentum()                                     ) ;
  transported.setReferencePoint 
    ( particle -> referencePoint() + deltaZ * particle -> slopes () ) ;
  
  // redefine the covarince matrices:
  
  // get C_0 matrix: 
  
  const Gaudi::SymMatrix4x4& C_p0  = particle -> momCovMatrix    () ;
  const Gaudi::Matrix4x3&    C_px0 = particle -> posMomCovMatrix () ;
  const Gaudi::SymMatrix3x3& C_x0  = particle -> posCovMatrix    () ;
  
  // we are modifying the same particle 
  if ( &transported == particle )  
  {
    
    //  C_x1              = C_x0 + ...      
    Gaudi::SymMatrix3x3& C_x1 = const_cast<Gaudi::SymMatrix3x3&> ( C_x0 ) ;
    //                    + F*C_p0*F^T + ...
    C_x1 += ROOT::Math::Similarity ( F , C_p0 ) ;
    //                    +  F*C_{px0} + C_{px0}^T*F^T 
    Gaudi::Math::update ( C_x1 , F * C_px0 ) ;
    
    // C_px1 = C_px0 + C_{p0}*F^T
    Gaudi::Matrix4x3& C_px1 = const_cast<Gaudi::Matrix4x3&>( C_px0 ) ;
    C_px1 += C_p0 * ROOT::Math::Transpose ( F ) ;
    
    // no need to change C_p1 
    
  }
  else  // we are dealing with different partcicles:
  {      
    //  C_x1              = C_x0 + ...      
    Gaudi::SymMatrix3x3 C_x1 = C_x0 ;
    //                    + F*C_p0*F^T + ...
    C_x1 += ROOT::Math::Similarity ( F , C_p0 ) ;
    //                    +  F*C_{px0} + C_{px0}^T*F^T 
    Gaudi::Math::update ( C_x1 , F * C_px0 ) ;
    
    transported.setPosCovMatrix    ( C_x1 ) ;
    
    // C_p1 = C_p0
    transported.setMomCovMatrix    ( C_p0 ) ;
    
    // C_px1 = C_px0 + C_{p0}*F^T
    transported.setPosMomCovMatrix ( C_px0 + C_p0 * ROOT::Math::Transpose ( F ) ) ;
  }
  // 
  return StatusCode::SUCCESS ;                                    // RETURN 
}
// ============================================================================
// The END 
// ============================================================================
