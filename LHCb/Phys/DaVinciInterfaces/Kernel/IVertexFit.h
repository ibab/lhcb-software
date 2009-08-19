// $Id: IVertexFit.h,v 1.4 2009-08-19 15:50:37 ibelyaev Exp $ 
// ============================================================================
#ifndef DAVINCIKERNEL_IVERTEXFIT_H 
#define DAVINCIKERNEL_IVERTEXFIT_H 1
// ============================================================================
// Include files
// ============================================================================
// from STL
// ============================================================================
#include <string>
// ============================================================================
// VertexFitter 
// ============================================================================
#include "Kernel/IParticleCombiner.h"
#include "Kernel/IParticleReFitter.h"
// ============================================================================
// forward declarations 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
/** @class IVertexFit IVertexFit.h VertexFitter/IVertexFit.h
 *
 *  The "concrete" vertex interface.
 *  It inherits from "basic" abstract  interfaces 
 *  IParticleCombiner and IParticleReFitter
 *
 *  In includes 4 series of methods:
 *     - @c fit with creation of particle ("classical" DaVinci fit) 
 *     - @c fit without creation of particle.
 *     - IParticleCombiner::combine
 *     - IParticleReFitter::reFit
 * 
 *  It is expected that the method 
 *  IParticleCombiner::combine is implemented throught @c fit:
 *  
 *  @code 
 *
 *  StatusCode combine( const LHCb::Particle::ConstVector& dauhters , 
 *                      LHCb::Particle&        mother   , 
 *                      LHCb::Vertex&          vertex   ) const 
 *   {
 *      return fit( daughters , mother , vertex ) ;
 *   };
 *  
 *  @endcode 
 *   
 *  In a similar way the 
 *  IParticleReFitter::reFit methdod also 
 *  needs to be implemented through @c fit :
 *  
 *  @code 
 *
 *  StatusCode reFit( LHCb::Particle& particle ) 
 *  {
 *     LHCb::Vertex* vertex = particle.endVertex() ;
 *    return fit( vertex->products().begin() , 
 *                vertex->products().end  () , 
 *                particle , *vertex       ) ; 
 *  } ; 
 *  
 *  @endcode 
 *                      
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
 *  @date   2004-12-19
 */
class GAUDI_API IVertexFit : 
  virtual public extend_interfaces2<IParticleCombiner ,
                                    IParticleReFitter> 
{
public:
  // ========================================================================== 
  /// interface machinery
  DeclareInterfaceID(IVertexFit, 3, 0);
  // ==========================================================================
public:
  // ==========================================================================  
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
   *  @see Particle
   *  @see Vertex
   *  @see GaudiAlgorithm::tool
   *  @see GaudiAlgorithm::Warning
   *
   *  @param vertex    (OUTPUT) result of vertex fit   
   *  @param daughters (INPUT)  vector of daughter particles 
   *  @return status code 
   */
  virtual StatusCode fit 
  ( LHCb::Vertex&                      vertex    ,
    const LHCb::Particle::ConstVector& daughters ) const = 0 ;  
  // ==========================================================================  
  /** the vertex fitting method without creation of a Particle, 
   *  which allow to use an almost arbitrary sequence of 
   *  daughter particles 
   *   (E.g. following types coudl be used :
   *    - LHCb::Particle::ConstVector, 
   *    - std::vector<LHCb::Particle*> 
   *    - std::vector<const LHCb::Particle*> 
   *    - SmartRefVector<Particle>
   *    - Particles, etc.. )
   *
   *  The elements of the sequence should be convertible to 
   *  "const LHCb::Particle*"
   * 
   *  @code 
   *
   *  // locate  the tool 
   *  const IVertexFit* fitter = tool<IVertexFit>( ... ) ;
   * 
   *  // container of all selected particles:
   *  Particles* particles = get<Particles>( "/Event/Phys/MyParticlesFromPV" );
   *  
   *  Vertex vertex ;
   *  StatusCode sc = fitter->fit( particles->begin()  ,
   *                               particles->end()    ,
   *                               vertex              ) ; 
   *  if( sc.isFailure() ) { Warning("Error in vertex fit", sc ) ; }
   *   
   *  // check chi2 of common vertex fit 
   *  double chi2 = vertex->chi2() ;
   *
   *  @endcode 
   *  
   *  @see Particle
   *  @see Vertex
   *  @see GaudiAlgorithm::tool
   *  @see GaudiAlgorithm::get
   *  @see GaudiAlgorithm::Warning
   * 
   *  @param begin begin-iterator for sequence of daughter particles 
   *  @param end   end-iterator for sequence of daughter particles 
   *  @param vertex result of vertex fit (output) 
   *  @return status code 
   */
  template <class DAUGHTER>
  inline  StatusCode fit  
  ( LHCb::Vertex&    vertex    , 
    DAUGHTER         begin     ,
    DAUGHTER         end       ) const 
  {
    return fit ( vertex , LHCb::Particle::ConstVector( begin , end ) ) ;
  }
  // ==========================================================================  
  /** Creation a vertex from two particles without 
   *  creation of an output  Particle 
   *  
   *  @author Juan Palacios Juan.Palacios@cern.ch
   *  @date 09-05-2006
   *
   *  @see Particle
   *  @see Vertex
   *  @see GaudiAlgorithm::tool
   *  @see GaudiAlgorithm::Warning
   *
   *  @param daughter0 first daughter particle  (input)
   *  @param daughter1 second daughter particle (input)
   *  @param vertex result of vertex fit        (output) 
   *  @return status code 
   *  @todo test
   */
  virtual StatusCode fit
  ( LHCb::Vertex&         vertex    , 
    const LHCb::Particle& daughter0 , 
    const LHCb::Particle& daughter1 ) const 
  {
    LHCb::Particle::ConstVector tmp(2);
    tmp[0]=&daughter0;
    tmp[1]=&daughter1;
    return fit ( vertex , tmp ) ;
  }
  // ==========================================================================
  /** Creation a vertex from three particles without 
   *  creation of an output  Particle 
   *  
   *  @author Juan Palacios Juan.Palacios@cern.ch
   *  @date 09-05-2006
   *
   *  @see Particle
   *  @see Vertex
   *  @see GaudiAlgorithm::tool
   *  @see GaudiAlgorithm::Warning
   *
   *  @param daughter0 first daughter particle  (input)
   *  @param daughter1 second daughter particle (input)
   *  @param daughter2 third daughter particle  (input)
   *  @param vertex result of vertex fit        (output) 
   *  @return status code 
   *  @todo test
   */
  virtual StatusCode fit
  ( LHCb::Vertex&         vertex    ,
    const LHCb::Particle& daughter0 , 
    const LHCb::Particle& daughter1 ,
    const LHCb::Particle& daughter2 ) const
  {
    LHCb::Particle::ConstVector tmp(3);
    tmp[0]=&daughter0;
    tmp[1]=&daughter1;
    tmp[2]=&daughter2;
    return fit ( vertex , tmp ) ;
  }  
  // ==========================================================================  
public: 
  // ==========================================================================  
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
   *  @see Particle
   *  @see Vertex
   *  @see GaudiAlgorithm::tool
   *  @see GaudiAlgorithm::Warning
   *
   *  @param daughters vector of daughter particles  (input)
   *  @param vertex    result of vertex fit             (output) 
   *  @param particle  result of vertex fit             (output) 
   *  @return status code 
   */
  virtual StatusCode fit 
  ( const LHCb::Particle::ConstVector& daughters ,
    LHCb::Vertex&                      vertex    ,
    LHCb::Particle&                    particle  ) const = 0 ;  
  // ==========================================================================  
  /** the vertex fitting method with creation of LHCb::Particle ("classical")
   *  which allow to use almost arbotrary sequence of 
   *  daughter partricles 
   *  (E.g. following types could be used:
   *     - LHCb::Particle::ConstVector 
   *     - std::vector<LHCb::Particle*> 
   *     - std::vector<const LHCb::Particle*> 
   *     - SmartRefVector<Particle>
   *     - Particles, etc.. )
   *   
   *  The elements of the sequence shodul be convertible to 
   *  "const LHCb::Particle*"
   * 
   *  @code 
   *
   *  // locate  the tool 
   *  const IVertexFit* fitter = get<IVertexFit>( ... ) ;
   * 
   *  // container of all selected particles:
   *  Particles* particles = get<Particles>( "/Event/Phys/MyParticlesFromPV" );
   *  
   *  Vertex   vertex ;
   *  LHCb::Particle particle ;
   *  StatusCode sc = fitter->fit( particles->begin()  ,
   *                               particles->end()    ,
   *                               vertex              ,
   *                               particle            ) ; 
   *  if( sc.isFailure() ) { Warning("Error in vertex fit", sc ) ; }
   *   
   *  @endcode 
   *  
   *  @see Particle
   *  @see Vertex
   *  @see GaudiAlgorithm::get
   *  @see GaudiAlgorithm::Warning
   * 
   *  @param begin begin-iterator for sequence of daughter particles 
   *  @param end   end-iterator for sequence of daughter particles 
   *  @param particle result of vertex fit (output) 
   *  @param vertex   result of vertex fit (output) 
   *  @return status code 
   */
  template <class DAUGHTER>
  inline StatusCode  fit 
  ( DAUGHTER               begin     ,
    DAUGHTER               end       ,
    LHCb::Vertex&          vertex    ,
    LHCb::Particle&        particle  ) const 
  {
    return fit ( LHCb::Particle::ConstVector ( begin , end ) , vertex , particle ) ;
  }  
  // ==========================================================================  
  /** Creation a Vertex and an output Particle from two Particles
   *  
   *  @author Juan Palacios Juan.Palacios@cern.ch
   *  @date 09-05-2006
   *
   *  @see Particle
   *  @see Vertex
   *  @see GaudiAlgorithm::tool
   *  @see GaudiAlgorithm::Warning
   *
   *  @param daughter0 first daughter particle  (input)
   *  @param daughter1 second daughter particle (input)
   *  @param particle result of vertex fit      (output) 
   *  @param vertex result of vertex fit        (output) 
   *  @return status code 
   *  @todo test
   */
  virtual StatusCode fit
  ( const LHCb::Particle& daughter0 , 
    const LHCb::Particle& daughter1 ,
    LHCb::Vertex&         vertex    ,
    LHCb::Particle&       particle  ) const
  {
    LHCb::Particle::ConstVector tmp(2) ;
    tmp[0]=&daughter0 ;
    tmp[1]=&daughter1 ;
    return fit( tmp , vertex , particle ) ;
  }  
  // ==========================================================================  
  /** Creation a Vertex and an output Particle from three Particles
   *  
   *  @author Juan Palacios Juan.Palacios@cern.ch
   *  @date 09-05-2006
   *
   *  @see Particle
   *  @see Vertex
   *  @see GaudiAlgorithm::tool
   *  @see GaudiAlgorithm::Warning
   *
   *  @param daughter0 first daughter particle  (input)
   *  @param daughter1 second daughter particle (input)
   *  @param daughter2 third daughter particle  (input)
   *  @param vertex result of vertex fit        (output) 
   *  @param particle result of vertex fit      (output) 
   *  @return status code 
   *  @todo test
   */  
  virtual StatusCode fit 
  ( const LHCb::Particle& daughter0 , 
    const LHCb::Particle& daughter1 ,
    const LHCb::Particle& daughter2 ,
    LHCb::Vertex&         vertex    ,
    LHCb::Particle&       particle  ) const
  {
    LHCb::Particle::ConstVector tmp(3);
    tmp[0] = &daughter0 ;
    tmp[1] = &daughter1 ;
    tmp[2] = &daughter2 ;
    return fit( tmp, vertex , particle ) ;    
  }
  // ==========================================================================  
public:
  // ==========================================================================  
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
   *  @param particle particle to be added
   *  @param vertex   vertex to be modified 
   *  @return status code 
   */
  virtual StatusCode add
  ( const LHCb::Particle*  particle , 
    LHCb::Vertex&          vertex   ) const = 0 ;  
  // ==========================================================================  
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
    LHCb::Vertex&          vertex   ) const = 0 ;
  // ==========================================================================  
protected:
  // ==========================================================================
  /// virtual and protected destructor 
  virtual ~IVertexFit() ;                   // virtual and protected destructor 
  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================
#endif // DAVINCIKERNEL_IVERTEXFIT_H
// ============================================================================
