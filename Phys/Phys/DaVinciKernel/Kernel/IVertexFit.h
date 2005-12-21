// $Id: IVertexFit.h,v 1.3 2005-12-21 13:50:04 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.3 $
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
#include "VertexFitter/IParticleCombiner.h"
#include "VertexFitter/IParticleReFitter.h"
// ============================================================================
// forward declarations 
// ============================================================================
class Particle ;
class Vertex;
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
 *  StatusCode combine( const Daughters& dauhters , 
 *                      Particle&        mother   , 
 *                      Vertex&          vertex   ) const 
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
 *  StatusCode reFit( Particle& particle ) const 
 *  {
 *     Vertex* vertex = particle->endVertex() ;
 *    return fit( vertex->products().begin() , 
 *                vertex->products().end  () , 
 *                particle , *vertex         ) ; 
 *  } ; 
 *  
 *  @endcode 
 *                      
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
 *  @date   2004-12-19
 */
class IVertexFit : 
  virtual public IParticleCombiner ,
  virtual public IParticleReFitter 
{
public:
  
  /** The actual type for container of daughter particles 
   *  the actual type is imported from interface IParticleCombiner 
   *  @see IParticleCombiner 
   *  @see IParticleCombiner::Daughters
   */
  typedef IParticleCombiner::Daughters   Daughters;
  
public:
  
  /** The vertex fitting method without creation of Particle 
   *
   *  @code
   *
   *  // locate  the tool 
   *  const IVertexFit* fitter = tool<IVertexFit>( ... ) ;
   * 
   *  // container of particles to be fitetr into common vertex 
   *  IVertexFit::Daughters daughters = ... ;
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
   *  @param daughters vector of daughter particles  (input)
   *  @param vertex result of vertex fit             (output) 
   *  @return status code 
   */
  virtual StatusCode fit 
  ( const Daughters& daughters ,
    Vertex&          vertex    ) const = 0 ;
  
  /** the vertex fitting method without creation of Particle, 
   *  which allow to use almost arbotrary sequence of 
   *  daughter partricles 
   *   (E.g. following types coudll be used :
   *    - ParticleVector, 
   *    - std::vector<Particle*> 
   *    - std::vector<const Particle*> 
   *    - SmartRefVector<Particle>
   *    - Particles, etc.. )
   *
   *  The elements of the sequence shodul be convertible to 
   *  "const Particle*"
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
  ( DAUGHTER         begin     ,
    DAUGHTER         end       ,
    Vertex&          vertex    ) const 
  {
    const Daughters daughters = Daughters( begin , end ) ;
    return fit ( daughters , vertex ) ;
  };

  /** The vertex fitting method with creation of Particle 
   *  ("classical")
   *
   *  @code
   *
   *  // locate  the tool 
   *  const IVertexFit* fitter = tool<IVertexFit>( ... ) ;
   * 
   *  // container of particles to be fitted into common vertex 
   *  IVertexFit::Daughters daughters = ... ;
   *  
   *  Vertex   vertex   ;
   *  Particle particle ; 
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
   *  @param particle  result of vertex fit             (output) 
   *  @param vertex    result of vertex fit             (output) 
   *  @return status code 
   */
  virtual StatusCode fit 
  ( const Daughters& daughters ,
    Particle&        particle  ,
    Vertex&          vertex    ) const = 0 ;
  
  /** the vertex fitting method with creation of Particle ("classical")
   *  which allow to use almost arbotrary sequence of 
   *  daughter partricles 
   *  (E.g. follwoing types could be used:
   *     - ParticleVector 
   *     - std::vector<Particle*> 
   *     - std::vector<const Particle*> 
   *     - SmartRefVector<Particle>
   *     - Particles, etc.. )
   *   
   *  The elements of the sequence shodul be convertible to 
   *  "const Particle*"
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
   *  Particle particle ;
   *  StatusCode sc = fitter->fit( particles->begin()  ,
   *                               particles->end()    ,
   *                               particle            ,
   *                               vertex              ) ; 
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
  ( DAUGHTER         begin     ,
    DAUGHTER         end       ,
    Particle&        particle  ,
    Vertex&          vertex    ) const 
  {
    const Daughters daughters = Daughters( begin , end ) ;
    return fit ( daughters , particle , vertex ) ;
  };
  
  /** add the particle to the vertex and refit 
   * 
   *  @code
   *
   *  // locate  the tool 
   *  const IVertexFit* fitter = get<IVertexFit>( ... ) ;
   *    
   *  Vertex*   vertex  = ... ;
   *  Particle* oneMore = ... ;
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
  ( const Particle*  particle , 
    Vertex*          vertex   ) const = 0 ;
  
  /** remove the particle from the vertex and refit 
   *
   *  @code
   *
   *  // locate  the tool 
   *  const IVertexFit* fitter = get<IVertexFit>( ... ) ;
   *    
   *  Vertex*   vertex = ... ;
   *  Particle* remove = ... ;
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
  ( const Particle*  particle , 
    Vertex*          vertex   ) const = 0 ;
  
public: 
  
  /// Return the interface ID
  static const InterfaceID& interfaceID() ;
  
protected:
  
  // virtual and protected destructor 
  virtual ~IVertexFit() ;  ///< virtual and protected destructor 

};

// ============================================================================
// The END 
// ============================================================================
#endif // DAVINCIKERNEL_IVERTEXFIT_H
// ============================================================================
