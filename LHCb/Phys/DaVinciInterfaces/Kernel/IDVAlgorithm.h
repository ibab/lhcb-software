// $Id: IDVAlgorithm.h,v 1.6 2010-01-14 07:49:13 jpalac Exp $
#ifndef KERNEL_IDVALGORITHM_H 
#define KERNEL_IDVALGORITHM_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include <GaudiKernel/IInterface.h>

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
class GAUDI_API IDVAlgorithm : virtual public IInterface {
public: 

  DeclareInterfaceID(IDVAlgorithm, 1, 0);

  /**
   *
   * Return a pointer to the parent GaudiAlgorithm, if in the
   * implementation's inheritance tree. Return 0 otherwise.
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
  virtual const LHCb::Particle::ConstVector* particles() const = 0;

  /**
   *
   * Return a container of LHCb::RecVertex*, containing primary 
   * vertices.
   *
   **/
  virtual const LHCb::RecVertex::ConstVector* primaryVertices() const = 0;
  
  /**
   *
   * Return the best primary vertex for a given LHCb::Particle.
   *
   **/
  virtual const LHCb::VertexBase* bestVertex(const LHCb::Particle*) const = 0;
  

};
#endif // KERNEL_IDVALGORITHM_H
