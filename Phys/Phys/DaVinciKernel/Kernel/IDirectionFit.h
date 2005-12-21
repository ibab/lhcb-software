// $Id: IDirectionFit.h,v 1.3 2005-12-21 13:56:05 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2005/06/09 13:10:40  pkoppenb
// Joined with v3r3b1 branch
//
// Revision 1.1.2.1  2005/06/09 11:55:40  pkoppenb
// New vertex interfaces
// 
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
#include "VertexFitter/IParticleReFitter.h"
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
 *  is implemented through @c fit 
 * 
 *  @code 
 *
 *  StatusCode reFit( Particle& particle ) const 
 *  {
 *     // Get the primary vertex (implementation dependent)
 *     // e.g. it coudl be a vertex with minimal impact parameter, 
 *     // or a vertex with minimal impact parameter significantce 
 *     // alfo additional constrains may be applyes, e.g.
 *     // "the vertex with minimal impact parameter choosed from all 
 *     //  downstream vertices". 
 *     const Vertex* primary = .... 
 *
 *     return fit( *primary , particle ) ; 
 *  } ; 
 *  
 *  @endcode 
 *
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr  
 *  @date   2004-12-19
 */
class IDirectionFit : virtual public IParticleReFitter 
{
public:  

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
   *  const Vertex* primary = ... ;
   * 
   *  // Bs-candidate:
   *  Particle*  Bs = ... ; 
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
  ( const Vertex& primary ,
    Particle&     particle ) const = 0 ;

public: 
  
  /// Return the interface ID
  static const InterfaceID& interfaceID() ;
  
protected:
  
  // virtual and protected destructor 
  virtual ~IDirectionFit() ; ///< virtual and protected destructor
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // DAVINCIKERNEL_IDIRECTIONFIT_H
// ============================================================================
