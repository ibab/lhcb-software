#ifndef DAVINCIKERNEL_IMASSVERTEXFIT_H 
#define DAVINCIKERNEL_IMASSVERTEXFIT_H 1
// ============================================================================
// Include files
// ============================================================================
// VertexFitter 
// ============================================================================
#include "Kernel/IParticleCombiner.h"
#include "Kernel/IParticleReFitter.h"
// ============================================================================

/** @class IMassVertexFit IMassVertexFit.h
 *
 *  The "concrete" mass-vertex constraint interface
 *  It inherits from "basic" abstract  interfaces 
 *  IParticleCombiner and IParticleReFitter
 *
 *  In includes 4 series of methods:
 *     - @c fit with explicit mass constrain, 
 *                  it is especially convinient 
 *                  for e.g. fit into "wrong" mass, 
 *     - @c fit with implict mass constrain 
 *                  the mass is deduced from Particle::particleID()
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
 *  StatusCode reFit( LHCb::Particle& particle ) const 
 *  {
 *     LHCb::Vertex* vertex = particle->endVertex() ;
 *     return fit( vertex->products().begin() , 
 *                 vertex->products().end  () , 
 *                 particle , *vertex         ) ; 
 *  } ; 
 *  
 *  @endcode 
 *
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
 *  @date   2004-12-19
 */
class IMassVertexFit : 
  public virtual IParticleCombiner ,
  public virtual IParticleReFitter 
{
public:
  
  /** The major method for mass-vertex constrain fit
   *  The particles from the input container container  are fit to produce 
   *  "mother" particle and its decay vertex.
   * 
   *  @code 
   * 
   *  // locate the tool  
   *  const IMassVertexFit* fitter = tool<IMassVertexFit>( ... ) 
   * 
   *  // loop over the first daughter particle 
   *  for( ... ipi1 = ... ; ... ; ++ipi1 ) 
   *  {
   *     for( ... ipi2 = ... ; ... ; ++ipi2 ) 
   *
   *        const LHCb::Particle* pi1 = *ipi1 ;
   *        const LHCb::Particle* pi2 = *ipi2 ;
   *   
   *        IMassVertexFit::LHCb::Particle::ConstVector daughters ;
   *        daughters.push_back( pi1 ) ;
   *        daughters.push_back( pi2 ) ;
   *   
   *        LHCb::Particle K0S  ; 
   *        Vertex   Vrtx ; 
   *   
   *        StatusCode sc = fitter->fit( daughters ,
   *                                     498 * MeV ,
   *                                     K0S       , 
   *                                     Vrtx      ) ;
   *        if( sc.isFailure() ) 
   *        { Warning("Error in K0S fit, skip the pair", sc ); continue ; }  
   * 
   *        .... 
   *     }
   *  }
   *
   *  @endcode 
   *
   *  @see LHCb::Particle 
   *  @see Vertex 
   *  @see GaudiAlgorithm::tool
   *  @see GaudiAlgorithm::Warning 
   *
   *  @param daughters the vector of daughter particles    (input)
   *  @param mass      mass                                (input)
   *  @param mother    the "mother" particle               (output)
   *  @param vertex    the decay vertex of mother particle (output)
   */
  virtual StatusCode  fit
  ( const LHCb::Particle::ConstVector&  daughters ,  
    const double      mass      ,
    LHCb::Particle&         mother    , 
    LHCb::Vertex&           vertex    ) const = 0 ;

  /** The major method for mass-vertex constrain fit
   *  The particles from the input container container  are fit to produce 
   *  "mother" particle and its decay vertex.
   * 
   *  The mass is deduced from Particle::paricleID()
   *  
   *  @code 
   * 
   *  // locate the tool  
   *  const IMassVertexFit* fitter = tool<IMassVertexFit>( ... ) 
   * 
   *  // loop over the first daughter particle 
   *  for( ... ipi1 = ... ; ... ; ++ipi1 ) 
   *  {
   *     for( ... ipi2 = ... ; ... ; ++ipi2 ) 
   *
   *        const LHCb::Particle* pi1 = *ipi1 ;
   *        const LHCb::Particle* pi2 = *ipi2 ;
   *   
   *        IMassVertexFit::LHCb::Particle::ConstVector daughters ;
   *        daughters.push_back( pi1 ) ;
   *        daughters.push_back( pi2 ) ;
   *   
   *        LHCb::Particle K0S  ; 
   *        K0S.setParticleID( 310 ) ;    // << This line !!!
   *        Vertex   Vrtx ; 
   *   
   *        StatusCode sc = fitter->fit( daughters ,
   *                                     K0S       , 
   *                                     Vrtx      ) ;
   *        if( sc.isFailure() ) 
   *        { Warning("Error in K0S fit, skip the pair", sc ); continue ; }  
   * 
   *        .... 
   *     }
   *  }
   *
   *  @endcode 
   *
   *  @see Vertex 
   *  @see Particle
   *  @see ParticleID
   *  @see GaudiAlgorithm::tool
   *  @see GaudiAlgorithm::Warning 
   *  
   *  @param daughters the vector of daughter particles    (input)
   *  @param mother    the "mother" particle               (output)
   *  @param vertex    the decay vertex of mother particle (output)
   */
  virtual StatusCode  fit
  ( const LHCb::Particle::ConstVector&  daughters ,  
    LHCb::Particle&         mother    , 
    LHCb::Vertex&           vertex    ) const = 0 ;
  
  /** The method which allows to use almost an arbitrary 
   *  sequence of daughter particles, 
   *   e.g. following types could be  used:
   *    - LHCb::Particle::ConstVector
   *    - std::vector<LHCb::Particle*>
   *    - std::vector<const LHCb::Particle*>
   *    - Particles 
   *    - SmartRefVector<Particle> ..etc 
   *
   *  The elements of the container must be convertible 
   *   into "const LHCb::Particle*"
   * 
   *  @see Vertex 
   *  @see Particle
   * 
   *  @param begin    begin-iterator for sequence of daughter particles (input) 
   *  @param end      end-iterator for sequence of daughter particles   (input) 
   *  @param mass     mass                                              (input)
   *  @param mother   the "mother" particle                             (output)
   *  @param vertex   the decay vertex of mother particle               (output)
   */  
  template <class DAUGHTER>
  inline StatusCode   fit 
  ( DAUGHTER          begin     ,
    DAUGHTER          end       ,
    const double      mass      ,
    LHCb::Particle&         mother    , 
    LHCb::Vertex&           vertex    ) const 
  {
    const LHCb::Particle::ConstVector children = Daughters( begin , end ) ;
    return fit( children , mass , mother , vertex ) ;
  };

  /** The method which allows to use almost an arbitrary 
   *  sequence of daughter particles,
   *   e.g. following types could be  used:
   *    - LHCb::Particle::ConstVector
   *    - std::vector<LHCb::Particle*>
   *    - std::vector<const LHCb::Particle*>
   *    - Particles 
   *    - SmartRefVector<Particle> ..etc 
   *
   *  The elements of the container must be convertible 
   *   into "const LHCb::Particle*"
   * 
   *  The mass is deduced from Particle::particleID()
   *  
   *  @see Vertex 
   *  @see Particle
   *  @see ParticleID
   *
   *  @param begin    begin-iterator for sequence of daughetr parricles (input) 
   *  @param end      end-iterator for sequence of daughter particles   (input) 
   *  @param mother   the "mother" particle                             (output)
   *  @param vertex   the decay vertex of mother particle               (output)
   */  
  template <class DAUGHTER>
  inline StatusCode   fit 
  ( DAUGHTER          begin     ,
    DAUGHTER          end       ,
    LHCb::Particle&         mother    , 
    LHCb::Vertex&           vertex    ) const 
  {
    const LHCb::Particle::ConstVector children = Daughters( begin , end ) ;
    return fit( children , mother , vertex ) ;
  };

public: 
  
  // Return the interface ID
  static const InterfaceID& interfaceID() ;
  
protected:
  
  /// virtual and protected destructor 
  virtual ~IMassVertexFit();
};

// ============================================================================
// The END 
// ============================================================================
#endif // DAVINCIKERNEL_IMASSVERTEXFIT_H
// ============================================================================
