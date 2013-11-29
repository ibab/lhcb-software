#ifndef ITAGGINGUTILS_H 
#define ITAGGINGUTILS_H 1

#include <string>
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"
#include "Event/RecVertex.h"

#include <Kernel/IDistanceCalculator.h>

static const InterfaceID IID_ITaggingUtils("ITaggingUtils", 1 , 0); 

/** @class ITaggingUtils ITaggingUtils.h 
 *  
 *  v1.0
 *  @author Marco Musy (Milano)
 *  @date   2007-02-07
 */

class ITaggingUtils : virtual public IAlgTool 
{

public:
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITaggingUtils; };
 
  virtual StatusCode calcIP( const LHCb::Particle* , 
                             const LHCb::VertexBase* , double&, double&)=0;
  
  virtual StatusCode calcIP( const LHCb::Particle* ,
                             const LHCb::RecVertex::ConstVector& ,
                             double& , double& )=0;

  virtual StatusCode calcDOCAmin( const LHCb::Particle* ,
				  const LHCb::Particle* ,
				  const LHCb::Particle* , double& , double& )=0;

  virtual int countTracks( const LHCb::Particle::ConstVector& )=0;

  virtual bool isinTree( const LHCb::Particle*, 
                         const LHCb::Particle::ConstVector& , double& )=0;

  virtual const IDistanceCalculator* getDistanceCalculator()=0;
  virtual std::string getCharmDecayMode(const LHCb::Particle*, int)=0;

};

#endif // ICOMBINETAGGERSTOOL_H
