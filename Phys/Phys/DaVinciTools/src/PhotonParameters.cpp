// $Id: PhotonParameters.cpp,v 1.3 2004-03-11 13:02:14 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2003/02/12 19:34:04  gcorti
// win compilation
//
// Revision 1.1  2003/01/22 16:43:23  ibelyaev
//  new tools for Photons
// 
// ============================================================================
// Include files
// CLHEP 
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Vector/LorentzVector.h"
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
// Event 
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
#include "Event/Vertex.h"
#include "Event/PrimVertex.h"
// CaloEvent 
#include "Event/CaloHypo.h"
#include "Event/CaloPosition.h"
// local
#include "PhotonParameters.h"

/** @file 
 *  Implementation file for class : PhotonParameters
 * 
 *  @date 2003-01-19 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */


// ============================================================================
/** @var  PhotonParametersFactory
 *  Declaration of mandatory Tool Factory
 */
// ============================================================================
static const  ToolFactory<PhotonParameters>         s_Factory ;
const        IToolFactory&PhotonParametersFactory = s_Factory ; 
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param type tool type 
 *  @param name tool name 
 *  @param parent pointer to tool's parent 
 */
// ============================================================================
PhotonParameters::PhotonParameters( const std::string& type   ,
                                    const std::string& name   ,
                                    const IInterface*  parent )
  : CaloTool ( type, name , parent ) 
  //
  , m_scale  ( 1.0 )  ///< default value of the scale factor 
  //
  , m_momentum ()
  , m_position () 
  , m_F11      ( 4 , 3 , 0 )  // block of transformation matrix 
  , m_F12      ( 4 , 3 , 0 )  // block of transformation matrix 
  , m_S11      ( 3 ,     0 )  // block of initial covariance matrix 
  , m_C11      ( 4 ,     0 )  // block of final covariance matrix 
  , m_C12      ( 4 , 3 , 0 )  // block of final covariance matrix 
  , m_C22      ( 0         )  // pointer to block of final covariance matrix 
{
  declareProperty ( "ScaleFactor" , m_scale );
  // declare new interface  
  declareInterface<IPhotonParams> ( this );
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
PhotonParameters::~PhotonParameters() {}
// ============================================================================

// ============================================================================
/** (Re)evaluate the photon's parameters using the knowledge 
 *  of the primary vertex  
 *
 *  Error codes:
 *  <li> <b>401</b> invalid (NULL) primary vertex </li> 
 *  Other error codes from method with <c>Vertex</c> argument 
 *
 *  @see IPhotonParams 
 *  @see Particle 
 *  @see PrimaryVertex 
 *  @param  photon  pointer to the photon to be (re)evaluated
 *  @param  vertex  pointer to the primary vertex 
 *  @return status code 
 */
// ============================================================================
StatusCode PhotonParameters::process 
( Particle*         photon , 
  const PrimVertex* vertex ) const 
{
  if( 0 == vertex ) { return Error(" PrimVertex* points to NULL", 401 ); }
  const Vertex* vrtx = vertex ;
  return process( photon , vrtx );
};
// ============================================================================

// ============================================================================
/** (Re)evaluate the photon's parameters using the knowledge 
 *  of photon production  vertex  
 *
 *  Error codes:
 *  <li> <b>402</b> invalid (NULL) vertex </li> 
 *  Other error codes from method with <c>HepPoint3D</c> argument 
 *
 *  @see IPhotonParams 
 *  @see Particle 
 *  @see Vertex 
 *  @param  photon  pointer to the photon to be (re)evaluated
 *  @param  vertex  pointer to the ptoton's production vertex 
 *  @return status code 
 */  
// ============================================================================
StatusCode PhotonParameters::process 
( Particle*            photon , 
  const Vertex*        vertex ) const 
{
  if( 0 == vertex ) { return Error(" Vertex* points to NULL", 402 ); } 
  return  1.0 == m_scale ? 
    process ( photon , vertex->position() , vertex->positionErr()           ) :
    process ( photon , vertex->position() , vertex->positionErr() * m_scale ) ;
};
// ============================================================================

// ============================================================================
namespace PhotonParametersLocal
{ 
  
  // ==========================================================================
  class IsHypo : public std::unary_function<const CaloHypo*,bool>
  {
  public: 
    /// constructor 
    IsHypo( CaloHypotheses::Hypothesis hypo ): m_hypo ( hypo ) {};
    /// functor interface 
    bool operator() ( const CaloHypo* hypo ) const 
    { return 0 != hypo && m_hypo == hypo->hypothesis() ? true : false ; }
  private:
    IsHypo();
  private:
    CaloHypotheses::Hypothesis m_hypo ;
  };
  // ==========================================================================
  
};
// ============================================================================

// ============================================================================
/** (Re)evaluate the photon's parameters using the knowledge 
 *  of photon production vertex  
 *
 *  Error codes:
 *  <li> <b>400</b> invalid (NULL) photon    </li> 
 *  <li> <b>405</b> Particle is not a photon </li> 
 *
 *  @see IPhotonParams 
 *  @see Particle 
 *  @param  photon  pointer to the photon to be (re)evaluated
 *  @param  point   position of the photon production vertex 
 *  @param  vxcov   covariance matrix of photon production vertex 
 *  @return status code 
 */  
// ============================================================================
StatusCode PhotonParameters::process 
( Particle*            photon , 
  const HepPoint3D&    point  , 
  const HepSymMatrix&  vxcov  ) const 
{
  // access to local utilities 
  using namespace PhotonParametersLocal ;
  using namespace CaloHypotheses        ;
  
  if( 0 == photon ) { return Error (" Particle* points to NULL"     , 400 ) ; }
  if( !(ParticleID(22) == photon->particleID()) ) 
    {                 return Error (" Particle* is not a photon"    , 405 ) ; }
  const ContainedObject* origin = photon->origin();
  if( 0 == origin ) { return Error (" Origin* points to NULL "      , 406 ) ; }
  const ProtoParticle*   proto  = dynamic_cast<const ProtoParticle*>(origin) ;
  if( 0 == proto  ) { return Error (" ProtoParticle points to NULL" , 407 ) ; }
  if( proto->calo().empty() ) 
    {              return Error( " ProtoParticle has no CaloHypos " , 408 ) ; }
  
  typedef const SmartRefVector<CaloHypo> Hypos;
  const Hypos& hypos = proto->calo();
  Hypos::const_iterator ihypo = 
    std::find_if( hypos.begin () , hypos.end () , IsHypo( Photon ) ) ;
  if( hypos.end() == ihypo ) 
    {         return Error(" CaloHypothesis 'Photon' is not found " , 409 ) ; }
  const CaloHypo* hypo = *ihypo ;
  
  // get the position 
  const CaloPosition* pos = hypo->position() ;
  if( 0 == pos    ) { return Error(" CaloPosition* points to NULL!" , 410 ) ; }
  
  // perform the actual evaluation 
  StatusCode sc = calculate( pos  , point , vxcov );
  if( sc.isFailure() ) { return Error(" Unable to calculate!" , sc ) ; }
  
  // modify the photon parameters
  photon -> setMomentum        (  m_momentum ) ;
  photon -> setPointOnTrack    (  m_position ) ;
  
  photon -> setMomentumErr     (  m_C11      ) ;
  photon -> setPointOnTrackErr ( *m_C22      ) ;
  photon -> setPosMomCorr      (  m_C12      ) ;
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** auxillary method for actual evaluation of 
 *  intermediate matrices and parameters 
 *  @param calo  pointer to CaloPosition object 
 *  @param point position of primary vertex 
 *  @param vxcov covarinace matrix for position of primary vertex 
 *  @return status code 
 */
// ============================================================================
StatusCode PhotonParameters::calculate 
( const CaloPosition* calo  , 
  const HepPoint3D&   point , 
  const HepSymMatrix& vxcov ) const 
{
  if( 0 == calo ) { return Error("calculate(): CaloPosition* is NULL!", 420 );}
  
  typedef CaloPosition CP;
  const CP::Covariance& caloCov = calo -> covariance();
  
  //  vector of "old" parameters 
  //    vold( 1 ) = pos   -> e () ;   // energy     in Calorimeter
  //    vold( 2 ) = pos   -> x () ;   // x-position in Calorimiter 
  //    vold( 3 ) = pos   -> y () ;   // y-position in Calorimeter 
  //    vold( 4 ) = point .  x () ;   // x-position at production vertex  
  //    vold( 5 ) = point .  y () ;   // y-position at production vertex 
  //    vold( 6 ) = point .  z () ;   // z-position at production vertex
  
  // "old" covariance matrix has the block structure 
  //     --------------
  //     |  S11 | S12 |
  //     --------------
  //     |  S21 | S22 | 
  //     --------------
  // Where S11 is 3x3 covarinace matrix of E  , xc and yc 
  //   and S22 is 3x3 covariance matrix of xv , yv and zv 
  //   matrices S12 and S21 both are equal to null matrix 
  //   
  
  m_S11.fast( 1 , 1 ) = caloCov ( CP::E , CP::E ) ;
  m_S11.fast( 2 , 1 ) = caloCov ( CP::X , CP::E ) ;
  m_S11.fast( 3 , 1 ) = caloCov ( CP::Y , CP::E ) ;
  
  m_S11.fast( 2 , 2 ) = caloCov ( CP::X , CP::X ) ;
  m_S11.fast( 3 , 2 ) = caloCov ( CP::Y , CP::X ) ;
  
  m_S11.fast( 3 , 3 ) = caloCov ( CP::Y , CP::Y ) ;
  
  const HepSymMatrix& S11 = m_S11 ;
  const HepSymMatrix& S22 = vxcov ;
  
  
  { // some auxillary mathematics 
    const double dx  = calo -> x() - point.x() ;
    const double dy  = calo -> y() - point.y() ;
    const double dz  = calo -> z() - point.z() ;
    
    // 
    const double sx        =  dx / dz ;
    const double sy        =  dy / dz ;
    const double sz        =  1.0     ;
    const double sx2       =  sx * sx ;
    const double sy2       =  sy * sy ;
    const double sz2       =  1.0     ;
    const double gamma     =  1.0 / sqrt( sx2 + sy2 + sz2 );
    
    const double gz        = gamma / dz ;
    
    //  'new' parameters 
    const double px = calo -> e () * gamma * sx ;
    const double py = calo -> e () * gamma * sy ;
    const double pz = calo -> e () * gamma * sz ;
    const double e  = calo -> e ()              ;
    const double x0 = point.x ()              ;
    const double y0 = point.y ()              ;
    const double z0 = point.z ()              ;
    
    m_momentum .setPx ( px ) ;
    m_momentum .setPy ( py ) ;
    m_momentum .setPz ( pz ) ;
    m_momentum .setE  ( e  ) ;
    
    m_position .setX  ( x0 ) ;
    m_position .setY  ( y0 ) ;
    m_position .setZ  ( z0 ) ;
    
    
    const double dx_dxc  =  1.0 ; // d( dx ) / d( xc ) 
    const double dx_dyc  =  0.0 ; // d( dx ) / d( yc )
    const double dx_dxv  = -1.0 ; // d( dx ) / d( xv ) 
    const double dx_dyv  =  0.0 ; // d( dx ) / d( yv ) 
    const double dx_dzv  =  0.0 ; // d( dx ) / d( zv )
    
    const double dy_dxc  =  0.0                ; // d( dy ) / d( xc ) 
    const double dy_dyc  =  1.0                ; // d( dy ) / d( yc ) 
    const double dy_dxv  =  0.0                ; // d( dy ) / d( xv ) 
    const double dy_dyv  = -1.0                ; // d( dy ) / d( yv ) 
    const double dy_dzv  =  0.0                ; // d( dy ) / d( zv ) 
    
    const double dz_dxc  =  0.0                ; // d( dz ) / d( xc ) 
    const double dz_dyc  =  0.0                ; // d( dz ) / d( yc ) 
    const double dz_dxv  =  0.0                ; // d( dz ) / d( xv )
    const double dz_dyv  =  0.0                ; // d( dz ) / d( yv ) 
    const double dz_dzv  = -1.0                ; // d( dz ) / d( zv ) 
    
    const double dgz_dxc = -1.0 * dx * gz * gz ; // d( gz ) / d( xc ) 
    const double dgz_dyc = -1.0 * dy * gz * gz ; // d( gz ) / d( yc ) 
    const double dgz_dxv =        dx * gz * gz ; // d( gz ) / d( xv ) 
    const double dgz_dyv =        dy * gz * gz ; // d( gz ) / d( yv ) 
    const double dgz_dzv =        dz * gz * gz ; // d( gz ) / d( zv ) 
    
    //    vnew ( 1 ) = px ;   // x-component of photon momentum 
    //    vnew ( 2 ) = py ;   // y-component of photon momentum  
    //    vnew ( 3 ) = pz ;   // z-component of photon momentum  
    //    vnew ( 4 ) = e  ;   // photon energy 
    //    vnew ( 5 ) = x0 ;   // x-component of photon production point 
    //    vnew ( 6 ) = y0 ;   // y-component of photon production point 
    //    vnew ( 7 ) = z0 ;   // z-component of photon production point 
    
    //  evaluate the "transport matrix"
    
    // 
    //  The transport matrix F = d(vnew) / d(vold)    (7x6) 
    //
    //   has block structure
    //        --------------
    //        | F11 |  F12 | 
    //        --------------
    //        | F21 |  F22 | 
    //        --------------
    //  Where F11 = d( px , py , pz , e) / d(  e , xc , yc )   4x3 matrix 
    //        F12 = d( px , py , pz , e) / d( xv , yv , zv )   4x4 matrix 
    //        F21 = d( x0 , y0 , z0    ) / d(  e , xc , yc )   3x3 matrix 
    //        F22 = d( x0 , y0 , z0    ) / d( xv , yv , zv )   3x3 matrix
    //
    //  From definition of parameters it is obvious that 
    //  F21 is null matrix ans F22 is unit matrix
    
    //   px =     e * dx * gz  
    //   d(px)/d(e) 
    m_F11 ( 1 , 1 ) = dx * gz ;
    //   d(px)/d(xc) 
    m_F11 ( 1 , 2 ) =  e * ( dx_dxc * gz + dx * dgz_dxc ) ;
    //   d(px)/d(yc) 
    m_F11 ( 1 , 3 ) =  e * ( dx_dyc * gz + dx * dgz_dyc ) ;
    //   d(px)/d(xv) 
    m_F12 ( 1 , 1 ) =  e * ( dx_dxv * gz + dx * dgz_dxv ) ;
    //   d(px)/d(yv) 
    m_F12 ( 1 , 2 ) =  e * ( dx_dyv * gz + dx * dgz_dyv ) ;
    //   d(px)/d(zv) 
    m_F12 ( 1 , 3 ) =  e * ( dx_dzv * gz + dx * dgz_dzv ) ;
    
    //   py = e * dy * gz  
    //   d(py)/d(e) 
    m_F11 ( 2 , 1 ) = dy * gz;
    //   d(py)/d(xc) 
    m_F11 ( 2 , 2 ) =  e * ( dy_dxc * gz + dy * dgz_dxc ) ;
    //   d(py)/d(yc) 
    m_F11 ( 2 , 3 ) =  e * ( dy_dyc * gz + dy * dgz_dyc ) ;
    //   d(py)/d(xv) 
    m_F12 ( 2 , 1 ) =  e * ( dy_dxv * gz + dy * dgz_dxv ) ;
    //   d(py)/d(yv) 
    m_F12 ( 2 , 2 ) =  e * ( dy_dyv * gz + dy * dgz_dyv ) ;
    //   d(py)/d(zv) 
    m_F12 ( 2 , 3 ) =  e * ( dy_dzv * gz + dy * dgz_dzv )  ;
    
    //   pz = e * dz * gz   
    //   d(pz)/d(e) 
    m_F11 ( 3 , 1 ) = dz * gz ;
    //   d(pz)/d(xc) 
    m_F11 ( 3 , 2 ) =  e * ( dz_dxc * gz + dz * dgz_dxc ) ;
    //   d(pz)/d(yc) 
    m_F11 ( 3 , 3 ) =  e * ( dz_dyc * gz + dz * dgz_dyc ) ;
    //   d(pz)/d(xv) 
    m_F12 ( 3 , 1 ) =  e * ( dz_dxv * gz + dz * dgz_dxv ) ;
    //   d(pz)/d(yv) 
    m_F12 ( 3 , 2 ) =  e * ( dz_dyv * gz + dz * dgz_dyv ) ;
    //   d(pz)/d(zv) 
    m_F12 ( 3 , 3 ) =  e * ( dz_dzv * gz + dz * dgz_dzv ) ;
    
    //   e (new) = e (old)          
    //   d(e)/d(e) 
    m_F11 ( 4 , 1 ) =  1.0 ;
    //   d(e)/d(xc) 
    m_F11 ( 4 , 2 ) =  0.0 ;
    //   d(e)/d(yc) 
    m_F11 ( 4 , 3 ) =  0.0 ;
    //   d(e)/d(xv) 
    m_F12 ( 4 , 1 ) =  0.0 ;
    //   d(e)/d(yv) 
    m_F12 ( 4 , 2 ) =  0.0 ;
    //   d(e)/d(zv) 
    m_F12 ( 4 , 3 ) =  0.0 ;
    
  }; // end of axillary mathematics 
  
  const HepMatrix& F11 = m_F11 ;
  const HepMatrix& F12 = m_F12 ;
  
    // ok, now all block of transport matrix are 
  // known and we coudl evaluate the covarinace matrix 
  // of new parameters
  
  // The covariance matrix of new parameters has block structure:
    //     -------------
    //     | C11 | C12 |  
    //     -------------
    //     | C21 | C22 | 
    //     -------------
    // 
    // C = F * S * F^T 
    //
    // Where:
    //
    // C11 = F11*S11*F11^T + F12*S21*F11^T + F11*S12*F12^T + F12*S22*F12^T
    // C12 = F11*S11*F21^T + F12*S21*F21^T + F11*S12*F22^T + F12*S22*F22^T
    // C21 = F21*S11*F11^T + F22*S21*F11^T + F21*S12*F12^T + F22*S22*F12^T 
    // C22 = F21*S11*F21^T + F22*S21*F21^T + F21*S12*F22^T + F22*S22*F22^T 
    //
    // taking into account that S12 = 0 , S21 = 0 , F21 = 0 , F22 = 1 on gets 
    //
    // C11 = F11 * S11 * F11^T + F12 * S22 * F12^T 
    // C12 = F12 * S22 * F22^T                     = F12 * S22 
    // C21 = F22 * S22 * F12^T                     = S22 * F12^T 
    // C22 = F22 * S22 * F22^T                     = S22 

  m_C11 = S11.similarity( F11 ) + S22.similarity( F12 ) ;
  m_C12 =                 F12   * S22                   ;
  m_C22 =                        &S22                   ;  // ATTENTION !!
  
  return StatusCode::SUCCESS ;
  
};

// ============================================================================
// The END 
// ============================================================================
