// $Id: IDirectionFit.h,v 1.3 2009-08-17 08:04:22 jpalac Exp $
// ============================================================================
#ifndef DAVINCIKERNEL_IDIRECTIONFIT_H 
#define DAVINCIKERNEL_IDIRECTIONFIT_H 1
// ============================================================================
// Include files
// ============================================================================
// from STL
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// VertexFitters 
// ============================================================================
#include "Kernel/IParticleReFitter.h"
// ============================================================================
namespace LHCb { class VertexBase ; }
// ============================================================================
/** @class IDirectionFit IDirectionFit.h VertexFitter/IDirectionFit.h
 *
 *  The "concrete" interface for "direction fitter".
 *  It inherits from "basic" abstract intterface IParticleReFitter.
 *  Since this type of constrains is not "primary" constrain 
 *  I see no reason to inherit it from IParticleCombiner.
 *
 *  The interface includes 2 series of methods
 *    - @c fit using the information from external Primary vertex 
 *      with *NO* change of the primary vertex.
 *    - IParticleReFitter::reFit
 *
 *  It is assumed that IParticleReFitter::reFit method 
 *  in implemented through @c fit 
 * 
 *  @code 
 *
 *  StatusCode reFit( LHCb::Particle& particle ) const 
 *  {
 *     // Get the primary vertex (implementation dependent)
 *     // e.g. it coudl be a vertex with minimal impact parameter, 
 *     // or a vertex with minimal impact parameter significantce 
 *     // alfo additional constrains may be applyes, e.g.
 *     // "the vertex with minimal impact parameter choosed from all 
 *     //  downstream vertices". 
 *     const LHCb::Vertex* primary = .... 
 *
 *     return fit( *primary , particle ) ; 
 *  } ; 
 *  
 *  @endcode 
 *
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr  
 *  @date   2004-12-19
 */
class GAUDI_API IDirectionFit : virtual public IParticleReFitter 
{
public:  
  // ==========================================================================
  /// interface machinery
  DeclareInterfaceID(IDirectionFit, 2, 0);
  // ==========================================================================
public:
  // ==========================================================================
  /** perform a "direction" fit for the particle.
   *  The angle between the momentum of the particle 
   *  and the vectro from primary vertex to the decay 
   *  vertex of the particle is constrained to 0.
   *  
   *  @code 
   * 
   *  // locate the tool:
   *  const IDirectionFit* fitter = tool<IDirectionFit>( ... ) ;
   *  
   *  // primary vertex:
   *  const LHCb::Vertex* primary = ... ;
   * 
   *  // Bs-candidate:
   *  LHCb::Particle*  Bs = ... ; 
   *
   *  StatusCode sc = fitter->fit( primary , Bs ) ;
   *  if ( sc.isFailure() ) 
   *    { Warning("Error in direction fit", sc ) ; }  
   *
   *  @endcode 
   *
   *  @see Particle
   *  @see Vertex 
   *  @see GaudiAlgorithm::tool
   * 
   *  @param particle particle to be constrained (output)
   *  @param primary  primary vertex             (input) 
   *  @return status code 
   */
  virtual StatusCode fit 
  ( const LHCb::VertexBase& primary ,
    LHCb::Particle&     particle ) const = 0 ;
  // ==========================================================================
protected:
  // ==========================================================================  
  /// virtual and protected destructor 
  virtual ~IDirectionFit() ;                // virtual and protected destructor
  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================
#endif // DAVINCIKERNEL_IDIRECTIONFIT_H
// ============================================================================
