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

  // classify charm decay modes
  // 0) D0 -> K pi, full reco
  // 1) D0 -> K pi pi pi, full reco
  // 2) D0 -> Ks pi pi, full reco
  // 3) D0 -> K pi pi0, full reco
  // 4) D+ -> K pi pi, full reco
  // 5) D+ -> Ks pi, full reco
  // 6) D0 -> K pi X, part reco
  // 7) D0 -> K e X, part reco
  // 8) D0 -> K mu X, part reco
  // 9) D* -> (D0 -> Ks pi pi) pi, full reco
  virtual int getCharmDecayModeInt(const LHCb::Particle*, int)=0;

  // remove candidates with daughters in common with signal B
  virtual LHCb::Particle::ConstVector purgeCands(const LHCb::Particle::Range& cands, const LHCb::Particle& BS)=0;

};

#endif // ICOMBINETAGGERSTOOL_H
