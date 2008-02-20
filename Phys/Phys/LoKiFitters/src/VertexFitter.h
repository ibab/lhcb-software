// $Id: VertexFitter.h,v 1.1.1.1 2008-02-20 15:48:44 ibelyaev Exp $
// ============================================================================
#ifndef LOKIFITTERS_VERTEXFITTER_H 
#define LOKIFITTERS_VERTEXFITTER_H 1
// ============================================================================
// Include files
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
// LHCbMath
// ============================================================================
#include "LHCbMath/MatrixUtils.h"
#include "LHCbMath/MatrixTransforms.h"
// ============================================================================
// ROOT/Mathlib
// ============================================================================
#include "Math/Functions.h"
// ============================================================================
// Local 
// ============================================================================
#include "FitterUtils.h"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @class VertexFitter 
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
  // ==========================================================================
  class VertexFitter 
    : public          GaudiTool 
    , public virtual IVertexFit 
  {
    // the friend factory for instantiation 
    friend class ToolFactory<VertexFitter> ;
  protected:
    // ========================================================================
    typedef FitterUtils::Entry  Entry    ;
    typedef std::vector<Entry>  Entries  ;
    typedef Entries::iterator   EIT      ;
    // ========================================================================
  public:  
    // ========================================================================
    /** The vertex fitting method without creation of a Particle 
     *
     *  @code
     *
     *  // locate  the tool 
     *  const IVertexFit* fitter = tool<IVertexFit>( ... ) ;
     * 
     *  // container of particles to be fitetr into common vertex 
     *  LHCb::Particle::ConstVector daughters = ... ;
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
    // ========================================================================
  public:
    // ========================================================================
    /** The vertex fitting method with creation of LHCb::Particle 
     *  ("classical")
     *
     *  @code
     *
     *  // locate  the tool 
     *  const IVertexFit* fitter = tool<IVertexFit>( ... ) ;
     * 
     *  // container of particles to be fitted into common vertex 
     *  LHCb::Particle::ConstVector daughters = ... ;
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
    // ========================================================================
  public:
    // ========================================================================
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
    // ========================================================================  
  public:
    // ========================================================================  
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
    // ========================================================================
  public:  
    // ========================================================================
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
    { return fit ( daughters , mother , vertex ) ; } 
    // ========================================================================
  public:
    // ========================================================================
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
    } 
    // ========================================================================
  protected:
    // ========================================================================
    /// standard constructor 
    VertexFitter 
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
      , m_transporterName ( "ParticleTransporter:PUBLIC")  
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
      declareProperty ( "Transporter"      , m_transporterName );
    } 
    // virtual & protected destrcutor
    virtual ~VertexFitter() {} ///< virtual & protected destrcutor
    // ========================================================================
  private:
    // ========================================================================
    VertexFitter() ; // the default constructor is disabled 
    VertexFitter( const VertexFitter& );// copy is disabled 
    VertexFitter& operator=( const VertexFitter& ); // disabled 
    // ========================================================================
  protected:
    // ========================================================================
    /// load the data into internal representation 
    StatusCode _load      
    ( const LHCb::Particle* particle , Entry& entry ) const ;
    /// update the internal representation
    StatusCode _update ( Entry& entry        ) const ;
    /// load the data from the daughter particles into the internal structures 
    StatusCode _load ( const LHCb::Particle::ConstVector& daughters ) const ;
    /// add one particle at the end of the queue
    StatusCode _add 
    ( const LHCb::Particle* child , const double newZ ) const ;
    /// transport the data to a certain position 
    StatusCode _transport ( Entry& entry , const double newZ ) const ;
    /// transport all data to a certain position 
    StatusCode _transport ( const double newZ ) const ;
    /// make one Kalman filter step
    StatusCode _step 
    ( Entry&                entry   , 
      const Gaudi::Vector3& x       , 
      const Gaudi::SymMatrix3x3& ci , 
      const double chi2             ) const  ;
    /// run Kalman smoother 
    StatusCode _smooth ()  const ;
    /// make few Kalman filter iterations 
    StatusCode _iterate ( const size_t nMax , const Gaudi::Vector3& x ) const ;
    /// evaluate the covariance ematrices
    StatusCode _evalCov () const ;
    /// make a seed 
    StatusCode _seed ( const LHCb::Vertex* vertex  ) const ;
    // ========================================================================
  protected:
    // ========================================================================
    /// get the tranbsporter 
    inline IParticleTransporter* transporter() const 
    {
      if ( 0 != m_transporter ) { return m_transporter ; }
      m_transporter = tool<IParticleTransporter> ( m_transporterName , this ) ;
      return m_transporter ;
    } 
    // ========================================================================
  private:
    // ========================================================================
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
    // ========================================================================
  } ;
} // end of namespace LoKi


// ============================================================================
// The END
// ============================================================================
#endif // LOKIFITTERS_VERTEXFITTER_H
// ============================================================================
