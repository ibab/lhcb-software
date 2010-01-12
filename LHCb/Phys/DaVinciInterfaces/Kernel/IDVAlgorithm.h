// $Id: IDVAlgorithm.h,v 1.1 2010-01-12 10:31:54 jpalac Exp $
#ifndef KERNEL_IDVALGORITHM_H 
#define KERNEL_IDVALGORITHM_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include <GaudiKernel/IInterface.h>

// from LHCb
#include <Event/Particle.h>
#include <Event/Vertex.h>

// IAlgTool forward declarations

class IDistanceCalculator;
class ILifetimeFitter;
class IVertexFit;

// 
class GaudiAlgorithm;


/** @class IDVAlgorithm Kernel/IDVAlgorithm.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2010-01-12
 */
class GAUDI_API IDVAlgorithm : virtual public IInterface {
public: 

  DeclareInterfaceID(IDVAlgorithm, 1, 0);
  
  virtual const GaudiAlgorithm* gaudiAlg() const = 0;
  
  virtual const IDistanceCalculator* distanceCalculator() const = 0;

  virtual const ILifetimeFitter* lifetimeFitter() const = 0;
  
  virtual const IVertexFit* vertexFitter() const = 0;
  
  virtual const LHCb::Particle::ConstVector* particles() const = 0;
  
  virtual const LHCb::Vertex::ConstVector* vertices() const = 0;
  
  virtual const LHCb::VertexBase* bestVertex(const LHCb::Particle*) const = 0;
  

};
#endif // KERNEL_IDVALGORITHM_H
