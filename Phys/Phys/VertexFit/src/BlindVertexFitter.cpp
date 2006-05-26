// $Id: BlindVertexFitter.cpp,v 1.1 2006-05-26 10:54:02 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ Vertsion $Revision: 1.1 $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
// Incldue files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <algorithm>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
// ============================================================================
// GaudiAlg 
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
// ============================================================================
// Kernel
// ============================================================================
#include "Kernel/IVertexFit.h"
#include "Kernel/IParticleTransporter.h"
// ============================================================================
// ROOT/Mathlib
// ============================================================================
#include "Math/Functions.h"
// ============================================================================
// Local 
// ============================================================================
#include "FitterUtils.h"
#include "MatrixUtils.h"
// ============================================================================

/// anonymous namespace to hide few technical constants
namespace 
{
  const double s_scale   = Gaudi::Units::perCent           ;
  const double s_scale2  = s_scale  * s_scale              ;
  const double s_small   = 0.1 * Gaudi::Units::micrometer  ;
  const double s_small2  = s_small  * s_small              ;
  const double s_middle  =  1  * Gaudi::Units::centimeter  ;
  const double s_middle2 = s_middle * s_middle             ;
  const double s_large   = 10  * Gaudi::Units::centimeter  ;
  const double s_large2  = s_large  * s_large              ;
  
}  

// ============================================================================
/** @class BlindVertexFitter  BlindVertexFitter.cpp  
 *  the most trivial implementation of Kalman-filter based vertex fitter.
 *  It should be very efficient (CPU).
 * 
 *  @attention 
 *  Current version deals only with BASIC CHARGED particles.
 *  
 *  @todo proper implementation of IVertexFit:remove 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-05-24
 */
// ============================================================================
class BlindVertexFitter 
  : public          GaudiTool 
  , public virtual IVertexFit 
{
  friend class ToolFactory<BlindVertexFitter> ;
protected:
  typedef FitterUtils::Entry  Entry    ;
  typedef std::vector<Entry>  Entries  ;
  typedef Entries::iterator   EIT      ;
public:  
  /** The vertex fitting method without creation of a Particle 
   *
   *  @code
   *
   *  // locate  the tool 
   *  const IVertexFit* fitter = tool<IVertexFit>( ... ) ;
   * 
   *  // container of particles to be fitetr into common vertex 
   *  IVertexFit::LHCb::Particle::ConstVector daughters = ... ;
   *  
   *  Vertex vertex ;
   *  StatusCode sc = fitter->fit( daughters , vertex ) 
   *  if( sc.isFailure() ) { Warning("Error in vertex fit", sc ) ; }
   *   
   *  // check chi2 of common vertex fit 
   *  double chi2 = vertex->chi2() ;
   *
   *  @endcode 
   *
   *  @see IVertexFit 
   *  @see Particle
   *  @see Vertex
   *  @see GaudiAlgorithm::tool
   *  @see GaudiAlgorithm::Warning
   *
   *  @param daughters vector of daughter particles  (input)
   *  @param vertex result of vertex fit             (output) 
   *  @return status code 
   */
  virtual StatusCode fit 
  ( const LHCb::Particle::ConstVector& daughters ,
    LHCb::Vertex&                      vertex    ) const ;  
  /** The vertex fitting method with creation of LHCb::Particle 
   *  ("classical")
   *
   *  @code
   *
   *  // locate  the tool 
   *  const IVertexFit* fitter = tool<IVertexFit>( ... ) ;
   * 
   *  // container of particles to be fitted into common vertex 
   *  IVertexFit::LHCb::Particle::ConstVector daughters = ... ;
   *  
   *  Vertex   vertex   ;
   *  LHCb::Particle particle ; 
   *  StatusCode sc = fitter->fit( daughters , particle , vertex ) 
   *  if( sc.isFailure() ) { Warning("Error in vertex fit", sc ) ; }
   *   
   *  // check chi2 of common vertex fit 
   *  double chi2 = vertex->chi2() ;
   *
   *  @endcode 
   *  
   *  @see IVertexFit 
   *  @see Particle
   *  @see Vertex
   *  @see GaudiAlgorithm::tool
   *  @see GaudiAlgorithm::Warning
   *
   *  @param daughters vector of daughter particles  (input)
   *  @param particle  result of vertex fit             (output) 
   *  @param vertex    result of vertex fit             (output) 
   *  @return status code 
   */
  virtual StatusCode fit 
  ( const LHCb::Particle::ConstVector& daughters ,
    LHCb::Particle&        particle  ,
    LHCb::Vertex&          vertex    ) const ;  
  /** add the particle to the vertex and refit 
   * 
   *  @code
   *
   *  // locate  the tool 
   *  const IVertexFit* fitter = get<IVertexFit>( ... ) ;
   *    
   *  LHCb::Vertex*   vertex  = ... ;
   *  LHCb::Particle* oneMore = ... ;
   *
   *  StatusCode sc = fitter->add( oneMore , vertex ) ;
   *  if( sc.isFailure() ) { Warning("Error in 'add' ", sc ) ; }
   *  
   *  // check chi2 of common vertex fit 
   *  double chi2 = vertex->chi2() ;
   *
   *  @endcode 
   *
   *  @see IVertexFit 
   *  @param particle particle to be added
   *  @param vertex   vertex to be modified 
   *  @return status code 
   */
  virtual StatusCode add
  ( const LHCb::Particle*  particle , 
    LHCb::Vertex&          vertex   ) const ;  
  /** remove the particle from the vertex and refit 
   *
   *  @code
   *
   *  // locate  the tool 
   *  const IVertexFit* fitter = get<IVertexFit>( ... ) ;
   *    
   *  LHCb::Vertex*   vertex = ... ;
   *  LHCb::Particle* remove = ... ;
   *
   *  StatusCode sc = fitter->remove( remove , vertex ) ;
   *  if( sc.isFailure() ) { Warning("Error in 'remove' ", sc ) ; }
   *  
   *  // check chi2 of common vertex fit 
   *  double chi2 = vertex->chi2() ;
   *
   *  @endcode 
   *
   *  @param particle particle to be removed
   *  @param vertex   vertex to be modified 
   *  @return status code 
   */
  virtual StatusCode remove
  ( const LHCb::Particle*  particle , 
    LHCb::Vertex&          vertex   ) const ;
public:  
  /** The major method for "combining" the daughter particles 
   *  into "mother" particle.
   *
   *  The container of input particles is "combined" into
   *  "mother" particle and its decay vertex.
   *
   *  The intermediate constrains (mass, vertex, mass-vertex,
   *  etc.. could be applied in the process of "combining", 
   *  dependent on the used implementation)
   * 
   *  @code 
   *
   *  // locate the tool  
   *  const IParticleCombiner* combiner = get<IParticleCombiner>( ... ) 
   * 
   *  // loop over the first daughter particle 
   *  for( ... ipi1 = ... ; ... ; ++ipi1 ) 
   *  {
   *     for( ... ipi2 = ... ; ... ; ++ipi2 ) 
   *     {
   *        const LHCb::Particle* pi1 = *ipi1 ;
   *        const LHCb::Particle* pi2 = *ipi2 ;
   *   
   *        IParticleCombiner::LHCb::Particle::ConstVector daughters ;
   *        daughters.push_back( pi1 ) ;
   *        daughters.push_back( pi2 ) ;
   *   
   *        LHCb::Particle K0S  ; 
   *        Vertex   Vrtx ; 
   *   
   *        StatusCode sc = combiner->combine( daughters , K0S , Vrtx ) ;
   *        if( sc.isFailure() ) 
   *        { Warning("Error in K0S fit, skip the pair", sc ); continue ; }  
   * 
   *        .... 
   *     }
   *  }
   *
   *  @endcode 
   *
   *  @see IParticleCombiner 
   *
   *  @param daughters the vector of daughter particles    (input)
   *  @param mother    the "mother" particle               (output)
   *  @param vertex    the decay vertex of mother particle (output)
   *  @return status code 
   */
  virtual StatusCode combine
  ( const LHCb::Particle::ConstVector&  daughters ,  
    LHCb::Particle&                     mother    , 
    LHCb::Vertex&                       vertex    ) const 
  { return fit ( daughters , mother , vertex ) ; } ;
  
public:
  /** The basic method for "refit" of the particle
   *
   *  @code 
   *
   *  // locate the tool 
   *  const IParticleReFitter* refitter = tool<IParticleRefitter>( ... ) ;
   * 
   *  // particle to be refit 
   *  LHCb::Particle* p = ... ;
   *
   *  StatusCode sc = refitter->reFit ( *p ) ;
   *  if ( sc.isFailure() ) { Warning("Error in reFit" , sc ) ; }
   *
   *  @endcode 
   *
   *  @see IParticleReFitter 
   *
   *  @see Particle
   *  @see GaudiAlgorithm::tool
   *  @see GaudiAlgorithm::Warning
   *
   *  @param particle reference to the particle 
   *  @return statsu code 
   */
  virtual StatusCode reFit ( LHCb::Particle& particle ) const  
  {
    LHCb::Vertex* vertex = particle.endVertex() ;
    const IVertexFit* vFit = this ;
    return vFit->fit ( vertex->outgoingParticles().begin() , 
                       vertex->outgoingParticles().end  () , 
                       particle , *vertex                  ) ; 
  } ;
protected:
  /// standard constructor 
  BlindVertexFitter 
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent ) 
    : GaudiTool ( type , name , parent )
    //
    , m_nIterMaxI       ( 10 ) ///< maximal number of iteration for vertex fit
    , m_nIterMaxII      (  5 ) ///< maximal number of iteration for "add" 
    , m_nIterMaxIII     (  5 ) ///< maximal number of iteration for "remove"    
    , m_DistanceMax     ( 1.0 * Gaudi::Units::micrometer ) 
    , m_DistanceChi2    ( 1.0 * Gaudi::Units::perCent    ) 
    , m_transporterName ( "ParticleTransporter/Transporter")  
    , m_transporter     ( 0 )
    , m_seedZmin        ( -1.5 * Gaudi::Units::meter      ) 
    , m_seedZmax        (  3.0 * Gaudi::Units::meter      ) 
    , m_seedRho         ( 50.0 * Gaudi::Units::centimeter )
  {
    // declare all interfaces
    declareInterface <IVertexFit>        ( this ) ;
    declareInterface <IParticleCombiner> ( this ) ;
    declareInterface <IParticleReFitter> ( this ) ;
    declareProperty ( "MaxIterations"    , m_nIterMaxI   ) ;
    declareProperty ( "MaxIterForAdd"    , m_nIterMaxII  ) ;
    declareProperty ( "MaxIterForRemove" , m_nIterMaxIII ) ;
    declareProperty ( "SeedZmin"         , m_seedZmin    ) ;
    declareProperty ( "SeedZmax"         , m_seedZmax    ) ;
    declareProperty ( "SeedRho"          , m_seedRho     ) ;
  } ;
  // protected destrcutor
  virtual ~BlindVertexFitter() {}
private:
  BlindVertexFitter() ; // the default constructor is disabled 
  BlindVertexFitter( const BlindVertexFitter& );// copy is disabled 
  BlindVertexFitter& operator=( const BlindVertexFitter& ); // disabled 
protected:
  /// load the data into internal representation 
  inline StatusCode _load      
  ( const LHCb::Particle* particle , Entry& entry ) const ;
  /// update the internal representation
  inline StatusCode _update ( Entry& entry        ) const ;
  /// load the data from the daughter particles into the internal structures 
  inline StatusCode _load ( const LHCb::Particle::ConstVector& daughters ) const ;
  /// add one particle at the end of the queue
  inline StatusCode _add 
  ( const LHCb::Particle* child , const double newZ ) const ;
  /// transport the data to a certain position 
  inline StatusCode _transport ( Entry& entry , const double newZ ) const ;
  /// transport all data to a certain position 
  inline StatusCode _transport ( const double newZ ) const ;
  /// make one Kalman filter step
  inline StatusCode _step 
  ( Entry&                entry   , 
    const Gaudi::Vector3& x       , 
    const Gaudi::SymMatrix3x3& ci , 
    const double chi2             ) const  ;
  /// run Kalman smoother 
  inline StatusCode _smooth ()  const ;
  /// make few Kalman filter iterations 
  inline StatusCode _iterate ( const size_t nMax , const Gaudi::Vector3& x ) const ;
  /// evaluate the covariance ematrices
  inline StatusCode _evalCov () const ;
  /// make a seed 
  inline StatusCode _seed ( const LHCb::Vertex* vertex  ) const ;
protected:
  /// get the tranbsporter 
  inline IParticleTransporter* transporter() const 
  {
    if ( 0 != m_transporter ) { return m_transporter ; }
    m_transporter = tool<IParticleTransporter> ( m_transporterName , this ) ;
    return m_transporter ;
  } ;
private:
  // maximal number of iteration for vertex fit 
  unsigned short m_nIterMaxI    ; ///< maximal number of iteration for vertex fit 
  // maximal number of iteratio n for "add" 
  unsigned short m_nIterMaxII   ; ///< maximal number of iteration for "add" 
  // maximal number of iteration for "remove" 
  unsigned short m_nIterMaxIII  ; ///< maximal number of iteration for "remove" 
  // distance (stop-iteration criterion)
  double         m_DistanceMax  ; ///< distance (stop-iteration criterion)
  // chi2 distance (stop iteration criterion)
  double         m_DistanceChi2 ; ///< chi2 distance (stop iteration criterion)
  // propagator/extrapolator/transporter name
  std::string                        m_transporterName ;
  // the transporter itself
  mutable IParticleTransporter*      m_transporter     ;
  /// fiducial volume for valid seed/vertex 
  double                             m_seedZmin        ;
  double                             m_seedZmax        ;
  double                             m_seedRho         ;
  /// local auxillary storages 
  mutable Entries                    m_entries  ;
  mutable const LHCb::Vertex*        m_vertex   ;
  // seed (evaluated internally) 
  mutable Gaudi::Vector3             m_seed     ;
  mutable Gaudi::SymMatrix3x3        m_seedci   ;
  // internal helper matrices 
  mutable Gaudi::SymMatrix2x2        m_cixy     ;
  mutable Gaudi::SymMatrix4x4        m_cmom     ;
  mutable Gaudi::Matrix4x4           m_cmom1    ;
  mutable Gaudi::Matrix4x3           m_mpcov    ;
  mutable Gaudi::SymMatrix3x3        m_seedaux  ;  
} ;
// ============================================================================
DECLARE_TOOL_FACTORY ( BlindVertexFitter ) ;
// ============================================================================
/// load the data from the daughter particles into the internal structures 
// ============================================================================
inline StatusCode BlindVertexFitter::_load 
( const LHCb::Particle::ConstVector& ds ) const 
{
  m_entries.resize ( ds.size() ) ;
  LHCb::Particle::ConstVector::const_iterator c = ds.begin()        ;
  Entries::iterator                           e = m_entries.begin() ;
  StatusCode sc = StatusCode::SUCCESS ;
  for ( ; ds.end() != c ; ++c , ++e ) { _load ( *c , *e ) ; } ;
  if ( m_entries.empty() ) { return Error("_load(): no valid data found") ; }
  return StatusCode::SUCCESS ;
} ;
// ============================================================================/
/// load the data into internal representation 
// ============================================================================/
inline StatusCode BlindVertexFitter::_load      
( const LHCb::Particle*     particle , 
  BlindVertexFitter::Entry& entry    ) const 
{
  if ( 0 == particle ) { return Error("_load(): invalid particle") ;}
  entry.m_p0 = particle    ;
  entry.m_p  = (*particle) ;
  return _update ( entry ) ;
}
// ============================================================================/
/// update the representation 
// ============================================================================/
inline StatusCode BlindVertexFitter::_update 
( BlindVertexFitter::Entry& entry ) const 
{
  const Gaudi::SymMatrix3x3& _pmcov = entry.m_p.posCovMatrix() ;
  m_cixy(0,0) = _pmcov ( 0, 0 ) ;
  m_cixy(0,1) = _pmcov ( 0, 1 ) ;
  m_cixy(1,1) = _pmcov ( 1, 1 ) ;
  int iFail = 0 ;
  m_cixy = m_cixy.Sinverse( iFail ) ;
  if ( iFail ) { return Error("_update(): Error in C<xy> matrix inversion") ; }
  // The most tricky part I 
  entry.m_vxi(0,0)= m_cixy(0,0) ;
  entry.m_vxi(0,1)= m_cixy(0,1) ;
  entry.m_vxi(1,1)= m_cixy(1,1) ;
  // The most tricky part II 
  const Gaudi::LorentzVector& mom = entry.m_p.momentum() ;
  const Gaudi::Vector2 slopes ( mom.Px() / mom.Pz() , mom.Py() / mom.Pz() ) ;
  const Gaudi::Vector2 cslope ( m_cixy * slopes )  ;
  entry.m_vxi (0,2) = -1 * cslope(0) ;
  entry.m_vxi (1,2) = -1 * cslope(1) ;
  entry.m_vxi (2,2) = ROOT::Math::Similarity( slopes , m_cixy ) ;
  //
  Gaudi::Math::geo2LA ( entry.m_p.referencePoint () , entry.m_parx ) ;
  Gaudi::Math::geo2LA ( entry.m_p.momentum       () , entry.m_parq ) ;
  //
  return StatusCode::SUCCESS ;
} ;
// ============================================================================/
// add one particle at the end of the queue
// ============================================================================
inline StatusCode BlindVertexFitter::_add 
( const LHCb::Particle*              child     , 
  const double                       newZ      ) const
{
  m_entries.push_back( Entry() ) ;
  _load      ( child , m_entries.back() ) ;
  _transport ( m_entries.back() , newZ ) ;
  return StatusCode::SUCCESS ;
} ;
// ============================================================================
// transport all internal data to new Z-position
// ============================================================================
inline StatusCode BlindVertexFitter::_transport
( BlindVertexFitter::Entry& entry , 
  const double              newZ ) const 
{
  // 
  StatusCode sc = transporter()->transport 
    ( entry.m_p0 , newZ , entry.m_p ) ;
  if ( sc.isFailure() ) 
  { 
    Error ( "Could not transport the particle" , sc )  ; 
    entry.m_p = *entry.m_p0 ;
  } ;  
  return _update ( entry ) ;
} ;
// ============================================================================
// transport all internal data to new Z-position
// ============================================================================
inline StatusCode BlindVertexFitter::_transport
( const double newZ ) const 
{
  for ( EIT entry = m_entries.begin() ; m_entries.end() != entry ; ++entry ) 
  { _transport ( *entry , newZ ) ; }
  return StatusCode::SUCCESS ;
} ;
// ============================================================================
/// making one step of Kalman filter 
// ============================================================================
inline  StatusCode BlindVertexFitter::_step
( BlindVertexFitter::Entry&  entry , 
  const Gaudi::Vector3&      x     , 
  const Gaudi::SymMatrix3x3& ci    , 
  const double               chi2  ) const 
{
  // OK ! 
  entry.m_ci = ci + entry.m_vxi  ;  /// NB !!!
  // OK ! 
  int ifail = 0 ;
  entry.m_c  = entry.m_ci.Sinverse( ifail ) ;
  if ( 0 != ifail ) 
  { return Error("_step: Error in inversion of inverse covarinace matrix ") ; }
  // OK ! 
  entry.m_x = entry.m_c * 
    ( ci*x + entry.m_vxi * entry.m_parx  ) ;
  // OK ! 
  const Gaudi::Vector3 dx = entry.m_parx - entry.m_x ;  
  // OK !
  entry.m_q = entry.m_parq - entry.m_p.posMomCovMatrix() * ( entry.m_vxi * dx ) ; 
  // OK ! 
  const double dchi2 = 
    ROOT::Math::Similarity ( entry.m_vxi  , dx            ) + 
    ROOT::Math::Similarity ( ci           , entry.m_x - x ) ;
  //
  if ( 0 > dchi2 ) { Warning ( "_step: delta chi2 is negative!" ); }    
  entry.m_chi2 = chi2 + dchi2 ;
  return StatusCode::SUCCESS ;
} ;
// ============================================================================
/// kalman smoothing  
// ============================================================================
inline StatusCode BlindVertexFitter::_smooth() const 
{
  const Entry& last = m_entries.back() ;
  for ( EIT entry = m_entries.begin() ; m_entries.end() != entry ; ++entry ) 
  {
    entry->m_x = last.m_x ;
    entry->m_q = last.m_q ;
  }
  return StatusCode::SUCCESS ;
} ;
// ============================================================================
// evaluate all covariances properly 
// ============================================================================
inline StatusCode BlindVertexFitter::_evalCov() const 
{
  using namespace ROOT::Math ;
  for ( EIT entry = m_entries.begin() ; m_entries.end() != entry ; ++entry ) 
  {
    // OK ! 
    entry->m_c = m_entries.back().m_c ;
    // OK !  
    entry->m_d = entry->m_p.momCovMatrix() 
      - Similarity( entry->m_p.posMomCovMatrix() , entry->m_vxi ) 
      + Similarity( entry->m_p.posMomCovMatrix() * entry->m_vxi , entry->m_c ) 
      ;
    // OK !
    entry->m_f = -1.0 * entry->m_p.posMomCovMatrix()*entry->m_vxi   ;
    entry->m_e = -1.0 * entry->m_f*entry->m_c ;
  }
  return StatusCode::SUCCESS ;
} ;
// ============================================================================
/// make few kalman iterations 
// ============================================================================
inline StatusCode BlindVertexFitter::_iterate 
( const size_t               nIterMax , 
  const Gaudi::Vector3&      _x       ) const 
{  
  // initial position
  const Gaudi::Vector3* x = &_x ;
  // chi2 
  const double  _chi2 = 0 ;
  const double*  chi2 = &_chi2 ;
  // inverse covariance matrix for the position  
  const Gaudi::SymMatrix3x3* ci = &m_seedci ;
  //
  for ( size_t iIter = 0 ; iIter < nIterMax ; ++iIter ) 
  {    
    // make a proper transportation 
    const double newZ = (*x)(2) ;
    StatusCode sc = _transport ( newZ ) ;
    if ( sc.isFailure() ) 
    { Warning ( "_iterate(): problem with transport ", sc ) ;}    
    // initialize the covariance matrix 
    if ( 0 != iIter ) { m_seedci = (*ci) * s_scale2 ; }
    ci   = &m_seedci ;
    // initialize the chi2 
    chi2 = &_chi2 ;
    const Gaudi::Vector3 x0 ( *x ) ;    
    // start the kalman filter 
    for ( EIT entry = m_entries.begin() ; m_entries.end() != entry ; ++entry ) 
    {
      // make one Kalman step 
      sc = _step ( *entry , *x , *ci , *chi2 ) ;
      // skip on Failure
      if ( sc.isFailure() ) { continue ; }                    // CONTINUE 
      // update the parameters 
      ci   = &entry->m_ci   ;
      x    = &entry->m_x    ;
      chi2 = &entry->m_chi2 ;
    }
    // kalman smooth
    sc = _smooth() ;
    if ( sc.isFailure() ) 
    { Warning ( "_iterate(): problem with smoother", sc ) ; }
    // distance in the absolute position 
    const double d1 = ROOT::Math::Mag        ( (*x) - x0 ) ;
    // distance in the chi2 units 
    const double d2 = ROOT::Math::Similarity ( (*x) - x0 , *ci ) ;
    if ( d2 < 0 ) 
    {  Warning ( "_iterate: negative chi2 detected, ignore" ) ; }
    // termination conditions:
    //
    //  (1) STOP if the distance is sufficiently small 
    //      - either the absolute distance 
    //      - or chi2 distance (if at least one iteration is performed) 
    //
    if ( d1 < m_DistanceMax || ( 0 < iIter && 0 <= d2 && d2 < m_DistanceChi2 ) )
    {
      sc = _evalCov () ;
      if ( sc.isFailure() ) 
      { Warning ( "_iterate(): problems with covarinces" , sc ) ; }
      debug() << "_iterate: Convergency after #" 
              << iIter << " iterations: " 
              << " dist=" << d1 << "/" << m_DistanceMax 
              << " chi2=" << d2 << "/" << m_DistanceChi2 
              << endreq ; 
      if ( msgLevel ( MSG::INFO ) ) { counter("#Iterations") += (1+iIter) ; }
      break ;                                                  // BREAK 
    }
    if ( nIterMax == iIter ) 
    { Error ( "No convergency has been reached after 'nIterMax' iterations" ) ; }
  } // end of iterations
  return StatusCode::SUCCESS ;
} ;
// ============================================================================
/// make a seed 
// ============================================================================
inline StatusCode BlindVertexFitter::_seed ( const LHCb::Vertex* vertex ) const
{
  // check if vertex could be used as a seed 
  const Gaudi::XYZPoint& p     = vertex->position  () ;
  const Gaudi::SymMatrix3x3& c = vertex->covMatrix () ;
  if ( m_seedZmin < p.Z() && m_seedZmax > p.Z() && m_seedRho > p.Rho()   && 
       s_small2 < Gaudi::Math::min_diagonal ( c ) && 
       s_large2 > Gaudi::Math::max_diagonal ( c )   ) 
  {
    // use the vertex parameters as proper seed 
    Gaudi::Math::geo2LA ( p , m_seed ) ;
    int ifail = 0 ;
    m_seedci = c.Sinverse( ifail ) ;
    if ( !ifail ) 
    {
      // properly scale the seed matrix 
      Gaudi::Math::scale ( m_seedci , s_scale2 ) ;
      return StatusCode::SUCCESS ;                            // RETURN 
    } 
  }
  /// construct the seed from the data 
  Gaudi::Math::setToScalar ( m_seedci , 0.0 ) ;
  Gaudi::Vector3      seed ;
  for ( EIT it = m_entries.begin() ; m_entries.end() != it ; ++it ) 
  {
    m_seedci += it->m_vxi ; 
    seed     += it->m_vxi * it->m_parx ;
  } 
  int ifail = 0 ;
  m_seedaux = m_seedci.Sinverse( ifail ) ;
  m_seed    = m_seedaux * seed ;
  if ( 0 != ifail ) 
  { 
    std::cerr <<  " I am SEED " << m_seed  << std::endl 
              << " #="       << m_entries.size() << std::endl 
              << " m_seedci" << m_seedci         << std::endl ;
    
    m_seed[0] = 0.0 ; m_seed[1] = 0.0 ; m_seed [2] = 0.0 ;
    Gaudi::Math::setToUnit ( m_seedci , 1.0/s_middle2 ) ;
    m_seedci(2,2) = 1.0/s_large2 ;
    Warning ( "_seed(): error in matrix inversion" ) ; 
  }
  // properly scale the seed error matrix 
  Gaudi::Math::scale ( m_seedci , s_scale2 ) ;
  /// check the validity of the seed 
  Gaudi::XYZPoint pnt ( m_seed[0] , m_seed[1] , m_seed[2] ) ;
  if ( m_seedZmin > pnt.Z() ) 
  { 
    Warning ("_seed(): Seed is outside of 'Zmin' fiducial volume ") ; 
    m_seed[2] = 0.5 * ( 0.0 + m_seedZmin ) ; 
    m_seedci(0,2) = 0 ; m_seedci(1,2) = 0 ;
    m_seedci(2,2) = 4.0/m_seedZmin/m_seedZmin;
  }
  if ( m_seedZmax < pnt.Z() ) 
  { 
    Warning ("_seed(): Seed is outside of 'Zmax' fiducial volume ") ; 
    m_seed[2] = 0.5 * ( 0.0 + m_seedZmax ) ; 
    m_seedci(0,2) = 0 ; m_seedci(1,2) = 0 ;
    m_seedci(2,2) = 4.0/m_seedZmax/m_seedZmax;
  }
  if ( m_seedRho  < pnt.Rho() ) 
  { 
    Warning ("_seed(): Seed is outside of 'Rho'  fiducial volume ") ; 
    m_seed[0]     = 0.0 ; m_seed[1] = 0.0 ; 
    m_seedci(0,1) = 0   ; m_seedci(0,2) = 0 ; 
    m_seedci(1,2) = 0   ; m_seedci(0,2) = 0 ;
    m_seedci(0,0) = 4.0/m_seedRho/m_seedRho ;
    m_seedci(1,1) = m_seedci(0,0);
  }  
  return StatusCode::SUCCESS ;
}  
// ============================================================================
/// The vertex fitting method without creation of a Particle
// ============================================================================
StatusCode BlindVertexFitter::fit 
( const LHCb::Particle::ConstVector& daughters ,
  LHCb::Vertex&                      vertex    ) const 
{
  // load the data 
  StatusCode sc = _load ( daughters ) ;
  if ( sc.isFailure() ) { return Error ( "fit(): failure from _load() ", sc ) ; }
  sc = _seed ( &vertex ) ; 
  if ( sc.isFailure() ) { Warning  ( "fit(): failure from _seed()"     , sc ) ; }
  // make "m_nIterMax" iterations 
  sc = _iterate ( m_nIterMaxI , m_seed ) ;
  if ( sc.isFailure() ) { Error    ( "fit(): failrue from _iterate()"  , sc ) ; }
  // get the data from filter 
  const Entry&               entry = m_entries.back() ;
  const Gaudi::Vector3&      x     = entry.m_x         ;
  const Gaudi::SymMatrix3x3& c     = entry.m_c         ;
  const double               chi2  = entry.m_chi2      ;
  // update the vertex parameters:
  vertex.setPosition  ( Gaudi::XYZPoint ( x(0) , x(1) , x(2) ) ) ;
  vertex.setCovMatrix ( c    ) ;
  vertex.setChi2      ( chi2 ) ;
  // fill the vertex 
  vertex.clearOutgoingParticles() ;
  for ( LHCb::Particle::ConstVector::const_iterator ip = daughters.begin() ; 
        daughters.end() != ip ; ++ip ) 
  { vertex.addToOutgoingParticles( *ip ) ; } ;
  // keep for future tracing
  m_vertex = &vertex ;
  if ( m_seedZmin > vertex.position().Z() ) 
  { Warning ("fit():Vertex is outside of 'Zmin' fiducial volume ") ; }
  if ( m_seedZmax < vertex.position().Z() ) 
  { Warning ("fit():Vertex is outside of 'Zmax' fiducial volume ") ; }
  if ( m_seedRho  < vertex.position().Rho() ) 
  { Warning ("fit():Vertex is outside of 'Rho'  fiducial volume ") ; }
  //
  return sc ;
} ;
// ============================================================================
/// The vertex fitting method with creation of a Particle
// ============================================================================
StatusCode BlindVertexFitter::fit 
( const LHCb::Particle::ConstVector& daughters ,
  LHCb::Particle&                    particle  ,
  LHCb::Vertex&                      vertex    ) const 
{
  using namespace ROOT::Math ;
  // make a vertex fit 
  StatusCode sc = fit ( daughters , vertex ) ;
  if ( sc.isFailure() ) { return Error("fit(): failure form fit", sc ) ; }
  // links:
  particle.clearDaughters() ;
  for ( LHCb::Particle::ConstVector::const_iterator dau = 
          daughters.begin() ; daughters.end() != dau ; ++dau ) 
  { particle.addToDaughters( *dau ) ; } ;
  particle.setEndVertex( &vertex ) ;  
  // fill the particle with the proper information 
  // 1) reference point and proper covariance matrices
  particle.setReferencePoint ( vertex.position  () ) ;
  particle.setPosCovMatrix   ( vertex.covMatrix () ) ;
  // 2) momentum, its covariance matrix and covariance with position
  // reset existing matrices
  Gaudi::Math::setToScalar ( m_cmom  , 0.0 ) ;
  Gaudi::Math::setToScalar ( m_cmom1 , 0.0 ) ;
  Gaudi::Math::setToScalar ( m_mpcov , 0.0 ) ;
  //
  Gaudi::LorentzVector vct   ;
  const Gaudi::SymMatrix3x3& pos = particle.posCovMatrix() ;
  for ( EIT i = m_entries.begin() ; m_entries.end() != i ; ++i ) 
  {
    Gaudi::Math::add ( vct , i->m_parq ) ;
    m_cmom  += i->m_d ;
    m_mpcov += i->m_e ;
    for ( EIT j = i + 1 ; m_entries.end() != j ; ++j ) 
    { 
      m_cmom1 += i->m_f * pos * Transpose( j->m_f ) ;
      m_cmom1 += j->m_f * pos * Transpose( i->m_f ) ;
    } 
  }
  Gaudi::Math::add ( m_cmom , m_cmom1 ) ;
  particle.setMomentum        ( vct     ) ;
  particle.setMomCovMatrix    ( m_cmom  ) ;
  particle.setPosMomCovMatrix ( m_mpcov ) ;
  // mass & error in mass 
  const double mass = particle.momentum().M() ;
  if ( 0 > mass ) { Warning( "fit(): mass is negative!") ; }
  Gaudi::Vector4 dmdp (  -vct.X() , -vct.Y() , -vct.Z() , vct.E() ) ;
  dmdp /= mass ;
  const double merr = Similarity ( dmdp , m_cmom ) ;
  if ( 0 > merr ) { Warning( "fit(): mass error is negative " ) ; }  
  particle.setMeasuredMass    ( mass             ) ;
  particle.setMeasuredMassErr ( ::sqrt ( ::fabs ( merr  ) ) ) ;
  // 
  return StatusCode::SUCCESS ;
} ;
// ============================================================================
/// add the particle to the vertex and refit 
// ============================================================================
StatusCode BlindVertexFitter::add
( const LHCb::Particle*  particle , 
  LHCb::Vertex&          vertex   ) const
{
  if ( 0 == particle ) { return Error ( "add: Particle* point to NULL!" ) ;}
  //
  if ( &vertex != m_vertex ) 
  {
    // first need to fit it! 
    const IVertexFit* vFit = this ;
    StatusCode sc = 
      vFit->fit ( vertex.outgoingParticles().begin () , 
                  vertex.outgoingParticles().end   () , vertex ) ;
    if ( sc.isFailure() ) 
    { return Error ( "add: error from 'fit'", sc ) ; }
  }
  StatusCode sc = _add ( particle , vertex.position().Z() ) ;
  if ( sc.isFailure() ) { Warning ("add(): failure from _add" , sc ) ; }
  //
  Entry& entry    =   m_entries.back()   ;
  const Entry& e0 = *(m_entries.end()-2) ;
  // make one Kalman step using the previos parameters as estimate
  sc = _step  ( entry , e0.m_x , e0.m_ci , e0.m_chi2) ;
  if ( sc.isFailure() ) { Warning ("add(): failure from _step" , sc ) ; }
  // smooth it!
  sc = _smooth() ;
  if ( sc.isFailure() ) { Warning ("add(): failure from _smooth" , sc ) ; }
  // make few more full iterations 
  m_seedci = entry.m_ci ;
  Gaudi::Math::scale ( m_seedci , s_scale2 ) ;
  sc = _iterate ( m_nIterMaxII , entry.m_x ) ;
  if ( sc.isFailure() ) { Warning ("add(): failure from _iterate" , sc ) ; }
  //
  const Gaudi::Vector3&      x     = entry.m_x         ;
  const Gaudi::SymMatrix3x3& c     = entry.m_c         ;
  const double               chi2  = entry.m_chi2      ;
  // update the vertex parameters:
  vertex.setPosition  ( Gaudi::XYZPoint ( x(0) , x(1) , x(2) ) ) ; 
  vertex.setCovMatrix ( c    ) ;
  vertex.setChi2      ( chi2 ) ;
  // fill the vertex 
  vertex.addToOutgoingParticles( particle ) ;
  // keep for future tracing
  m_vertex = &vertex ;
  // check for positions
  if ( m_seedZmin > vertex.position().Z() ) 
  { Warning ("fit():Vertex is outside of 'Zmin' fiducial volume ") ; }
  if ( m_seedZmax < vertex.position().Z() ) 
  { Warning ("fit():Vertex is outside of 'Zmax' fiducial volume ") ; }
  if ( m_seedRho  < vertex.position().Rho() ) 
  { Warning ("fit():Vertex is outside of 'Rho'  fiducial volume ") ; }
  //
  return StatusCode::SUCCESS ;
} ;
// ============================================================================
/// remove the particle from the vertex and refit 
// ============================================================================
StatusCode BlindVertexFitter::remove
( const LHCb::Particle*  particle , 
  LHCb::Vertex&          vertex   ) const
{
  return Warning("remove(): not implemented (yet)!") ; // ATTENTION!!!
  //
  const StatusCode OK = StatusCode::SUCCESS ;
  if ( 0 == particle ) 
  { return Warning ( "remove: nothing to remove" , OK ) ; }        // RETURN 
  if ( &vertex != m_vertex ) 
  {
    // first need to fit it !
    const IVertexFit* vFit = this ;
    StatusCode sc = 
      vFit->fit ( vertex.outgoingParticles().begin() , 
                  vertex.outgoingParticles().end  () , vertex ) ;
    if ( sc.isFailure() ) 
    { return Error("remove: error from 'fit'", sc ) ;  }           // RETURN 
  }
  // remove the particle from the vertex 
  vertex.removeFromOutgoingParticles ( particle ) ;
  // find the corresponding entry
  
//   EIT ifind = m_entries.end() ;
//   for ( EIT entry = m_entries.begin() ; m_entries.end() != entry ; ++entry ) 
//   { if ( particle == entry->m_p0 ) { ifind = entry ; break ; } }
//   if ( m_entries.end() == ifind ) 
//   { return Warning ("remove: particle is not in the fit", OK ) ; } // RETURN
  
//   const Entry& back = m_entries.back() ;
//   // FIX ME!!!
//   const Gaudi::SymMatrix3x3 nci= ( back.m_ci  /* + ... FIX ME !!!! */ ) ;
//   int iFail = 0 ;
//   const Gaudi::SymMatrix3x3 nc = nci.Sinverse( iFail ) ;
//   if ( 0 != iFail ) { iFail  = 0 ; Error ("remove: invaid matrix inversion") ;}
//   // FIX ME!!!
//   const Gaudi::Vector3 x     = nc*back.m_ci*back.m_x /** - ... FIX ME */ ;
//   // FIX ME !!!
//   const double         nchi2 = back.m_chi2 /** - ... FIX ME!!! */  ;
  
//   // remove the entry 
//   m_entries.erase ( ifind ) ;
  
//   // make few more full iterations 
//   StatusCode sc  = _iterate ( m_nIterMaxIII , x ) ;
  
  return StatusCode::SUCCESS ;
} ;

// ============================================================================
// The END 
// ============================================================================

