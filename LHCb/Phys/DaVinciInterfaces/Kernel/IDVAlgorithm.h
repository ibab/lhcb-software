// $Id: IDVAlgorithm.h,v 1.9 2010-01-20 12:22:23 jpalac Exp $
#ifndef KERNEL_IDVALGORITHM_H 
#define KERNEL_IDVALGORITHM_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include <GaudiKernel/INamedInterface.h>

// from LHCb
#include <Event/Particle.h>
#include <Event/RecVertex.h>

// IAlgTool forward declarations

class IDistanceCalculator;
class ILifetimeFitter;
class IVertexFit;

// 
class GaudiAlgorithm;


/** @class IDVAlgorithm Kernel/IDVAlgorithm.h
 *  
 *  Abstract interface containing some of the more useful methods currently
 *  in DVAlgorithm and IPhysDesktop.
 *
 *  @author Juan PALACIOS
 *  @date   2010-01-12
 */
class GAUDI_API IDVAlgorithm : virtual public INamedInterface {
public: 

  DeclareInterfaceID(IDVAlgorithm, 1, 0);

  /**
   *
   * Handle to the concrete implementation, or to the parent if
   * implementation does not derive from GaudiAlgorithm
   *
   **/  
  virtual const GaudiAlgorithm* gaudiAlg() const = 0;

  virtual const IDistanceCalculator* distanceCalculator(const std::string& name = "") const = 0;

  virtual const ILifetimeFitter* lifetimeFitter(const std::string& name = "") const = 0;
  
  virtual const IVertexFit* vertexFitter(const std::string& name = "") const = 0;

  /**
   *
   * Return a container of local LHCb::Particle*
   *
   **/  
  virtual const LHCb::Particle::Range particles() const = 0;

  /**
   *
   * Return a container of LHCb::RecVertex*, containing primary 
   * vertices.
   *
   **/
  virtual const LHCb::RecVertex::Range primaryVertices() const = 0;
  
  /**
   *
   * Return the best primary vertex for a given LHCb::Particle.
   *
   **/
  virtual const LHCb::VertexBase* bestVertex(const LHCb::Particle*) const = 0;
  

};
#endif // KERNEL_IDVALGORITHM_H
