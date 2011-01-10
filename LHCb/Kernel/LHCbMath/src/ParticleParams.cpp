// $Id: ParticleParams.cpp,v 1.3 2010-05-26 13:19:16 ibelyaev Exp $
// ============================================================================
// Include files
// ============================================================================
// local
// ============================================================================
// LHCbMath 
// ============================================================================
#include "LHCbMath/ParticleParams.h"
#include "LHCbMath/MatrixUtils.h"
// ============================================================================
/** @file
 *  Implementation file for class Gaudi::Math::ParticleParams
 * 
 *  @date 2010-05-20 
 *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
 */
// ============================================================================
namespace Gaudi
{
  typedef ROOT::Math::SMatrix<double, 1, 4> Matrix1x4;
}
// ============================================================================
namespace
{
  // ==========================================================================
  /// fill full 8x8 matrix from subcomponents 
  void fillCov8
  ( Gaudi::SymMatrix8x8&       result          , 
    const Gaudi::SymMatrix3x3& posCovMatrix    ,
    const Gaudi::SymMatrix4x4& momCovMatrix    ,
    const double               lenCovMatrix    ,
    const Gaudi::Matrix4x3&    momPosCovMatrix ,
    const Gaudi::Vector3&      lenPosCovMatrix ,
    const Gaudi::Vector4&      lenMomCovMatrix )
  {
    result.Place_at(posCovMatrix,0,0);
    result.Place_at(momCovMatrix,3,3);
    //
    result(7,7) = lenCovMatrix ;
    
    for(int imom =0; imom<4; ++imom)
    {
      for(int ipos=0 ; ipos<3; ++ipos)
      {
        result(imom+3,ipos) = momPosCovMatrix(imom,ipos);
      }
    }
    
    for(int imom =0; imom<4; ++imom)
    { result(7,imom+3) = lenMomCovMatrix(imom); }
    
    for(int ipos=0 ; ipos<3; ++ipos)
    { result(7,ipos) = lenPosCovMatrix(ipos); }
    
  }
  // ==========================================================================
  // fill 4x4 covariance matrix (p3,length)
  /* (function commented because never referenced, ICC remark #177)
  void fillCov4
  ( Gaudi::SymMatrix4x4&       result          , 
    const Gaudi::SymMatrix4x4& momCovMatrix    ,
    const double               lenCovMatrix    ,
    const Gaudi::Vector4&      lenMomCovMatrix )
  {
    // construct the (p3,len) cov matrix
    result.Place_at(momCovMatrix.Sub<Gaudi::SymMatrix3x3>(0,0),0,0);
    //
    result(3,3) = lenCovMatrix ;
    //
    for ( int imom =0; imom<3 ; ++imom )
    { result ( 3, imom ) = lenMomCovMatrix ( imom ) ; }
    //
  }
  */
  // ==========================================================================
  /// fill 5x5 covariance matrix (p4,length):
  void fillCov5 
  ( Gaudi::SymMatrix5x5&                       result    ,
    const Gaudi::Math::LorentzVectorWithError& momentum  ,
    const Gaudi::Math::ValueWithError&         length    ,
    const Gaudi::Vector4&                      lenMomCov )
  {
    result.Place_at( momentum.cov2() , 0 , 0 );
    result (4,4)    = length.cov2() ;
    //
    for ( unsigned int i = 0 ; i < 4 ; ++i )
    { result (4,i) = lenMomCov ( i ) ; }
  }
  // ===========================================================================
}
// ============================================================================
// constructor from a full parameter set
// ============================================================================
Gaudi::Math::ParticleParams::ParticleParams 
( const Gaudi::XYZPoint&                     position , 
  const Gaudi::LorentzVector&                momentum ,
  const double                               length   ,
  const Gaudi::SymMatrix8x8&                 cov8     ) 
  : m_length   ( length   , cov8(7,7)                          ) 
  , m_position ( position , cov8.Sub<Gaudi::SymMatrix3x3>(0,0) )  
  , m_momentum ( momentum , cov8.Sub<Gaudi::SymMatrix4x4>(3,3) ) 
  , m_momPosCovMatrix ( cov8.Sub<Gaudi::Matrix4x3>(3,0) )
  , m_lenPosCovMatrix ()
  , m_lenMomCovMatrix ()
{
  for ( unsigned int i = 0 ; i < 3 ; ++i ) 
  { m_lenPosCovMatrix ( i ) = cov8 ( i     , 7 ) ; }
  for ( unsigned int i = 0 ; i < 4 ; ++i ) 
  { m_lenMomCovMatrix ( i ) = cov8 ( i + 3 , 7 ) ; }
}
// ============================================================================
// constructor from a full parameter set
// ============================================================================
Gaudi::Math::ParticleParams::ParticleParams 
( const Gaudi::XYZPoint&                     position , 
  const Gaudi::LorentzVector&                momentum ,
  const Gaudi::SymMatrix7x7&                 cov7     ) 
  : m_length   ( 0 , 0 ) 
  , m_position ( position , cov7.Sub<Gaudi::SymMatrix3x3>(0,0) )  
  , m_momentum ( momentum , cov7.Sub<Gaudi::SymMatrix4x4>(3,3) ) 
  , m_momPosCovMatrix ( cov7.Sub<Gaudi::Matrix4x3>(3,0) )
  , m_lenPosCovMatrix ()
  , m_lenMomCovMatrix ()
{}
// ============================================================================
// constructor from a full parameter set
// ============================================================================
Gaudi::Math::ParticleParams::ParticleParams 
( const Gaudi::Math::Point3DWithError&       position , 
  const Gaudi::Math::LorentzVectorWithError& momentum ,
  const Gaudi::Math::ValueWithError&         length   , 
  const Gaudi::Matrix4x3&                    posmom   , 
  const Gaudi::Vector3&                      lenpos   , 
  const Gaudi::Vector4&                      lenmom   ) 
  : m_length          ( length   ) 
  , m_position        ( position )  
  , m_momentum        ( momentum ) 
  , m_momPosCovMatrix ( posmom   )
  , m_lenPosCovMatrix ( lenpos   )
  , m_lenMomCovMatrix ( lenmom   )
{}
// ============================================================================
// constructor from a full parameter set
// ============================================================================
Gaudi::Math::ParticleParams::ParticleParams
( const Gaudi::XYZPoint&                     position , 
  const Gaudi::LorentzVector&                momentum ,
  const double                               length   , 
  const Gaudi::SymMatrix3x3&                 poscov   , 
  const Gaudi::SymMatrix4x4&                 momcov   , 
  const double                               lencov   ,
  const Gaudi::Matrix4x3&                    posmom   , 
  const Gaudi::Vector3&                      lenpos   , 
  const Gaudi::Vector4&                      lenmom   ) 
  : m_length          ( length   , lencov ) 
  , m_position        ( position , poscov )  
  , m_momentum        ( momentum , momcov ) 
  , m_momPosCovMatrix ( posmom   )
  , m_lenPosCovMatrix ( lenpos   )
  , m_lenMomCovMatrix ( lenmom   )
{}
// ============================================================================
// constructor from a full parameter set
// ============================================================================
Gaudi::Math::ParticleParams::ParticleParams 
( const Gaudi::Math::SVectorWithError<8,double>& params ) 
  : m_length          ( params.cov2( 7 , 7 )                        )  
  , m_position        ( params.cov2().Sub<Gaudi::SymMatrix3x3>(0,0) )   
  , m_momentum        ( params.cov2().Sub<Gaudi::SymMatrix4x4>(4,4) )  
  , m_momPosCovMatrix ( params.cov2().Sub<Gaudi::Matrix4x3>(3,0)    )
  , m_lenPosCovMatrix ()
  , m_lenMomCovMatrix ()
{
  // values 
  m_position . SetX     ( params(0) ) ;
  m_position . SetY     ( params(1) ) ;
  m_position . SetZ     ( params(2) ) ;
  m_momentum . SetPx    ( params(3) ) ;
  m_momentum . SetPy    ( params(4) ) ;
  m_momentum . SetPz    ( params(5) ) ;
  m_momentum . SetE     ( params(6) ) ;
  m_length   . setValue ( params(7) ) ;
  //
  for ( unsigned int i = 0 ; i < 3 ; ++i ) 
  { m_lenPosCovMatrix ( i ) = params.cov2 ( i     , 7 ) ; }
  for ( unsigned int i = 0 ; i < 4 ; ++i ) 
  { m_lenMomCovMatrix ( i ) = params.cov2 ( i + 3 , 7 ) ; }
}
// ============================================================================
// constructor from a full parameter set
// ============================================================================
Gaudi::Math::ParticleParams::ParticleParams 
( const Gaudi::Vector8&      par , 
  const Gaudi::SymMatrix8x8& cov ) 
  : m_length          ( cov ( 7 , 7 )                     )  
  , m_position        ( cov .Sub<Gaudi::SymMatrix3x3>(0,0) )   
  , m_momentum        ( cov .Sub<Gaudi::SymMatrix4x4>(4,4) )  
  , m_momPosCovMatrix ( cov .Sub<Gaudi::Matrix4x3>(3,0)    )
  , m_lenPosCovMatrix ()
  , m_lenMomCovMatrix ()
{
  // values 
  m_position . SetX     ( par(0) ) ;
  m_position . SetY     ( par(1) ) ;
  m_position . SetZ     ( par(2) ) ;
  m_momentum . SetPx    ( par(3) ) ;
  m_momentum . SetPy    ( par(4) ) ;
  m_momentum . SetPz    ( par(5) ) ;
  m_momentum . SetE     ( par(6) ) ;
  m_length   . setValue ( par(7) ) ;
  //
  for ( unsigned int i = 0 ; i < 3 ; ++i ) 
  { m_lenPosCovMatrix ( i ) = cov ( i     , 7 ) ; }
  for ( unsigned int i = 0 ; i < 4 ; ++i ) 
  { m_lenMomCovMatrix ( i ) = cov ( i + 3 , 7 ) ; }
}
// ============================================================================
// constructor from a full parameter set
// ============================================================================
Gaudi::Math::ParticleParams::ParticleParams()
  : m_length          () 
  , m_position        ()  
  , m_momentum        () 
  , m_momPosCovMatrix ()
  , m_lenPosCovMatrix ()
  , m_lenMomCovMatrix ()
{}
// ============================================================================
// virtual destructor 
// ============================================================================
Gaudi::Math::ParticleParams::~ParticleParams() {}
// ============================================================================
//  get all parameters 
// ============================================================================
Gaudi::Math::SVectorWithError<8,double> Gaudi::Math::ParticleParams::all() const 
{
  Gaudi::Math::SVectorWithError<8,double> result ;
  all ( result ) ;
  return result ;
}
// ============================================================================
//  get all parameters 
// ============================================================================
void Gaudi::Math::ParticleParams::all 
( Gaudi::Math::SVectorWithError<8,double>& a ) const 
{
  par8 ( a.value () ) ;
  cov8 ( a.cov2  () ) ;
}
// ============================================================================
// get all parameters 
// ============================================================================
void Gaudi::Math::ParticleParams::par8 ( Gaudi::Vector8&      c )  const 
{
  c (0) = m_position.point3d  ().X() ;
  c (1) = m_position.point3d  ().Y() ;
  c (2) = m_position.point3d  ().Z() ;
  c (3) = m_momentum.vector4d ().Px() ;
  c (4) = m_momentum.vector4d ().Py() ;
  c (5) = m_momentum.vector4d ().Pz() ;
  c (6) = m_momentum.vector4d ().E() ;
  c (7) = m_length  ;
}
// ============================================================================
// get all parameters 
// ============================================================================
Gaudi::Vector8 Gaudi::Math::ParticleParams::par8 ()  const 
{
  Gaudi::Vector8 c ;
  par8 ( c ) ;
  return c ;
}
// ============================================================================
// get all covariances 
// ============================================================================
void Gaudi::Math::ParticleParams::cov8 ( Gaudi::SymMatrix8x8& cov )  const 
{
  fillCov8 ( cov                , 
             m_position .cov2() , 
             m_momentum .cov2() , 
             m_length   .cov2() , 
             m_momPosCovMatrix  , 
             m_lenPosCovMatrix  , 
             m_lenMomCovMatrix  ) ;
}
// ============================================================================
// get all covariances 
// ============================================================================
Gaudi::SymMatrix8x8 Gaudi::Math::ParticleParams::cov8 ()  const 
{
  Gaudi::SymMatrix8x8 c ;
  cov8 ( c ) ;
  return c ;
}
// ============================================================================
// calculate c*tau 
// ============================================================================
Gaudi::Math::ValueWithError Gaudi::Math::ParticleParams::ctau() const 
{
  // tau = len * mass / | p3 |
  //
  // because we meaure mass and momentum in units of energy, the result is really ctau
  
  const Gaudi::LorentzVector& p4 = m_momentum;
  double px  = p4.Px() ;
  double py  = p4.Py() ;
  double pz  = p4.Pz() ;
  double E   = p4.E() ;
  double p2  = px*px+py*py+pz*pz ;
  double m2  = E*E - p2 ;
  double m   = std::sqrt( m2 ) ;
  double p   = std::sqrt(p2) ;
  double len = m_length ;
  double tau = len * m / p ;
  
  Gaudi::Vector5 jacobian ;
  jacobian(0) = -px * tau * (1/m2 + 1/p2) ;
  jacobian(1) = -py * tau * (1/m2 + 1/p2) ;
  jacobian(2) = -pz * tau * (1/m2 + 1/p2) ;
  jacobian(3) =   E * tau * (1/m2) ;
  jacobian(4) = m / p ;
  
  Gaudi::SymMatrix5x5 cov5  ;
  //
  fillCov5 ( cov5              , 
             m_momentum        , 
             m_length          , 
             m_lenMomCovMatrix ) ;
  // now: we actually measure momentum and mass both in units of
  // energy. that makes a factor of c difference. in the formula.
  return Gaudi::Math::ValueWithError 
    ( tau , ROOT::Math::Similarity ( jacobian , cov5 ) ) ;
}
// ========================================================================
/* apply mass-constrained fit to the particle 
 *  @param  input    (INPUT)  the particle to be constrained 
 *  @param  mass     (INPUT)  the mass 
 *  @param  output  (OUTPUT) the constrained particle 
 *  @return the chi2 of mass-constraiend fit 
 *  The actual code has been stollen from Wouter Hulsbergen 
 *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
 *  @date   2010-05-20
 */
// ========================================================================
double Gaudi::Math::FitMass::fit
( const Gaudi::Math::ParticleParams& input  , 
  const double                       mass   ,
  Gaudi::Math::ParticleParams&       output ) 
{
  // applies a mass constrained. bail out if error on mass is very
  // small, because then the mass constrained was already applied.
  double chisq = 0 ;
  
  const Gaudi::LorentzVector& p4 = input.momentum() ;
  
  const double px = p4 . Px () ;
  const double py = p4 . Py () ;
  const double pz = p4 . Pz () ;
  const double E  = p4 . E  () ;
  const double m2 = p4 . M2 () ;
  const double m  = p4.  M  () ;
  
  // jacobian for (p4->m) transformation 
  const Gaudi::Vector4 mjacobian ( -px/m , -py/m , -pz/m , E/m ) ;
  
  const double mcov = ROOT::Math::Similarity (  mjacobian , input.momentum().cov2() ) ;
  
  if ( mcov < 0.001 ) 
  { 
    if ( &input != &output ) { output = input ; }
    return 0 ; 
  }
  
  // the trick is to rotate first everything from (p3,E) to
  // (p3,m2). we then filter the mass constrained. after that we
  // rotate back.
  
  // first transform to the (px,py,pz,m2) basis. since the jacobian is
  // rather empty, this could be optimized, another time.
  ROOT::Math::SMatrix<double,4,4> p4jacobian ; // = dQnew/dQold
  for ( size_t col=0; col<3; ++col ) { p4jacobian(col,col) = 1 ; }
  p4jacobian(3,0) = -2*px ;
  p4jacobian(3,1) = -2*py ;
  p4jacobian(3,2) = -2*pz ;
  p4jacobian(3,3) =  2*E  ;
  
  // now fill the 8x8 cov matrix using the transformed momentum.
  Gaudi::SymMatrix8x8 cov8 ;
  fillCov8 
    ( cov8                                                      ,
      input.position().cov2()                                   ,
      ROOT::Math::Similarity( p4jacobian , input.momentum().cov2() ),
      input.decayLength().cov2 ()                               ,
      p4jacobian         * input.momPosCov ()                   ,
      input.lenPosCov ()                                        , 
      input.lenMomCov () * ROOT::Math::Transpose ( p4jacobian ) ) ;
  
  // and fill the parameter vector:
  Gaudi::Vector8 par8 ;
  input.par8 ( par8 ) ;
  par8(6) = m2 ;
  
  // apply the constraint, which is trivial now
  const double residual    = mass*mass - m2;
  const double residualV   = cov8 ( 6 , 6 ) ;
  const double residualV_i = 1.0/residualV ;
  const double residual_   = residual*residualV_i ;
  
  chisq = residual* residual/residualV ;
  Gaudi::Vector8  row6 ;
  for ( unsigned int i = 0 ; i < 8 ; ++i ) 
  { 
    row6 ( i )  = cov8 ( i , 6 ) ;
    par8 ( i ) += row6 ( i ) * residual_ ;
  }
  //
  Gaudi::Math::update ( cov8 , row6 , -residualV_i ) ;
  
  // extract result, rotate back
  const double new_x   = par8(0) ;
  const double new_y   = par8(1) ;
  const double new_z   = par8(2) ;
  const double new_px  = par8(3) ;
  const double new_py  = par8(4) ;
  const double new_pz  = par8(5) ;
  const double new_m2  = par8(6) ;
  const double new_len = par8(7) ;
  
  const double new_E  = std::sqrt ( std::fabs ( new_px*new_px + 
                                                new_py*new_py + 
                                                new_pz*new_pz + new_m2 ) ) ;
  
  ROOT::Math::SMatrix<double,4,4> new_p4jacobian ;
  new_p4jacobian(0,0) = 1.0 ; 
  new_p4jacobian(1,1) = 1.0 ; 
  new_p4jacobian(2,2) = 1.0 ; 
  new_p4jacobian(3,0) = new_px/new_E ;
  new_p4jacobian(3,1) = new_py/new_E ;
  new_p4jacobian(3,2) = new_pz/new_E ;
  new_p4jacobian(3,3) = 1.0/(2*new_E) ;
  
  Gaudi::Vector3 v3 ;
  Gaudi::Vector4 v4 ;
  for ( unsigned int i = 0 ; i < 3 ; ++i ) { v3 ( i ) = cov8 ( 7 , i     ) ; }
  for ( unsigned int i = 0 ; i < 4 ; ++i ) { v4 ( i ) = cov8 ( 7 , i + 3 ) ; }
  
  output = Gaudi::Math::ParticleParams 
    ( Gaudi::XYZPoint      ( new_x  , new_y  , new_z          ) , 
      Gaudi::LorentzVector ( new_px , new_py , new_pz , new_E ) , 
      new_len                                                         , 
      cov8.Sub<Gaudi::SymMatrix3x3>(0,0)   ,
      ROOT::Math::Similarity( new_p4jacobian, cov8.Sub<Gaudi::SymMatrix4x4>(3,3)) ,
      cov8(7,7) ,
      new_p4jacobian * cov8.Sub<Gaudi::Matrix4x3>(3,0) ,
      v3 , 
      v4 * ROOT::Math::Transpose( new_p4jacobian ) ) ;
  
  return chisq ;
 
}
// ========================================================================
/*  apply mass-constrained fit to the particle 
 *  @param mass     (INPUT) the mass 
 *  @param particle (UPDATE) the particle to be constrained 
 *  @return chi2 of mass-constrained fit 
 *  The actual code has been stollen from Wouter Hulsbergen 
 *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
 *  @date   2010-05-20
 */
// ============================================================================
double Gaudi::Math::FitMass::fit   
( const double                 mass     , 
  Gaudi::Math::ParticleParams& particle ) 
{ return fit ( particle , mass , particle ) ; }
// ============================================================================
/* apply mass-constrained fit to the particle 
 * 
 *  @code
 *
 *     const Gaudi::Math::ParticleParams& params = .... ;
 *
 *
 *     const double mass_B = 5.279 * GeV ;
 *     double       chi2 = 0.0 
 *
 *     Gaudi::Math::ParticleParams fitted  = .... ;
 *         Gaudi::Math::FitMass::fit( params , mass_B , chi2 ) ;
 *  
 *  @code 
 *
 *  @param  input    (INPUT)  the particle to be constrained 
 *  @param  mass     (INPUT)  the mass 
 *  @param  output  (OUTPUT) the constrained particle 
 *  @return the chi2 of mass-constraiend fit 
 *  The actual code has been stollen from Wouter Hulsbergen 
 *  @author Vanya Belyaev Ivan.Belyaev@nikhef.nl
 *  @date   2010-05-20
 */
// ============================================================================
Gaudi::Math::ParticleParams Gaudi::Math::FitMass::fit
( const Gaudi::Math::ParticleParams& input , 
  const double                       mass  , 
  double&                            chi2  ) 
{
  Gaudi::Math::ParticleParams fitted ;
  chi2 = fit ( input , mass , fitted ) ;
  return fitted ;
}
// ============================================================================
// The END 
// ============================================================================
