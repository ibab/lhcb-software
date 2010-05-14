// $Id: IDVAlgorithm.h,v 1.10 2010-05-14 15:03:45 ibelyaev Exp $
// ============================================================================
#ifndef KERNEL_IDVALGORITHM_H 
#define KERNEL_IDVALGORITHM_H 1
// ============================================================================
// Include files
// ============================================================================
// from STL
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/INamedInterface.h"
// ============================================================================
// LHCb
// ============================================================================
#include "Event/Particle.h"
#include "Event/RecVertex.h"
// ============================================================================
// Forward declarations
// ============================================================================
class IDistanceCalculator ;
class ILifetimeFitter     ;
class IVertexFit          ;
class IMassFit            ;
class IDirectionFit       ;
class IParticleReFitter   ;
class IParticleCombiner   ;
class IPVReFitter         ;
class GaudiAlgorithm      ;
// ============================================================================
/** @class IDVAlgorithm Kernel/IDVAlgorithm.h
 *  
 *  Abstract interface containing some of the more useful methods currently
 *  in DVAlgorithm and IPhysDesktop.
 *
 *  @author Juan PALACIOS
 *  @date   2010-01-12
 */
class GAUDI_API IDVAlgorithm : virtual public INamedInterface 
{
public: 
  // ==========================================================================
  DeclareInterfaceID(IDVAlgorithm, 2, 0);
  // ==========================================================================
public: // tools 
  // ==========================================================================
  /// get the distance calculator tool
  virtual const IDistanceCalculator* 
  distanceCalculator    ( const std::string& nickname = "" ) const = 0 ;
  /// get lifetime fitter tool 
  virtual const ILifetimeFitter* 
  lifetimeFitter        ( const std::string& nickname = "" ) const = 0 ;
  /// get the vertex fitter tool 
  virtual const IVertexFit* 
  vertexFitter          ( const std::string& nickname = "" ) const = 0 ;
  /// get particle re-fitter tool
  virtual const IParticleReFitter* 
  particleReFitter      ( const std::string& nickname = "" ) const = 0 ;
  /// Accessor for ParticleCombiner tool
  virtual const IParticleCombiner* 
  particleCombiner      ( const std::string& nickname = "" ) const = 0 ;
  /// Accessor for mass-fitter tool
  virtual const IMassFit* 
  massFitter            ( const std::string& nickname = "" ) const = 0 ;
  /// Accessor for direction-fitter tool
  virtual const IDirectionFit* 
  directionFitter       ( const std::string& nickname = "" ) const = 0 ;
  /// Accessor for primary vertex re-fitter tool
  virtual const IPVReFitter* 
  primaryVertexReFitter ( const std::string& nickname = "" ) const = 0 ;
  // ==========================================================================
public: // implementation 
  // ==========================================================================
  /** Handle to the concrete implementation, or to the parent if
   * implementation does not derive from GaudiAlgorithm
   */  
  virtual const GaudiAlgorithm* gaudiAlg() const = 0;
  // ==========================================================================
public: // data 
  // ==========================================================================
  /// Return a container of local LHCb::Particle*
  virtual const LHCb::Particle::Range  particles       () const = 0;  
  /// Return a container of LHCb::RecVertex*, containing primary vertices.
  virtual const LHCb::RecVertex::Range primaryVertices () const = 0;
  /// Return the best primary vertex for a given LHCb::Particle.
  virtual const LHCb::VertexBase* bestVertex(const LHCb::Particle*) const = 0;
  // ==========================================================================
protected: 
  // ==========================================================================
  /// virtual and protected destructor 
  virtual ~IDVAlgorithm () ;                // virtual and protected destructor 
  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_IDVALGORITHM_H
// ============================================================================
