// $Id: ITaggingUtilsChecker.h,v 1.3 2010-06-22 09:43:37 mgrabalo Exp $
#ifndef ITAGGINGUTILSCHECKER_H 
#define ITAGGINGUTILSCHECKER_H 1

#include <string>
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"
#include "Event/MCParticle.h"
#include "Event/RecVertex.h"
#include "Event/GenHeader.h"

static const InterfaceID IID_ITaggingUtilsChecker("ITaggingUtilsChecker", 1 , 0); 

/** @class ITaggingUtilsChecker ITaggingUtilsChecker.h 
 *  
 *
 *  @author Marco Musy, Marc Grabalosa
 *  @date   2007-06-09
 */

class ITaggingUtilsChecker : virtual public IAlgTool {

public:
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITaggingUtilsChecker; };
 
  virtual StatusCode calcIP( const LHCb::Particle* , 
                             const LHCb::VertexBase* , double&, double&)=0;
  
  virtual StatusCode calcIP( const LHCb::Particle* ,
                             const LHCb::RecVertex::ConstVector& ,
                             double& , double& )=0;

  virtual StatusCode calcIPPU( const LHCb::Particle* ,
                               const LHCb::Particle* ,
                               const LHCb::RecVertex::ConstVector& ,
                               const double,
                               double& , double&, double&,
                               double& , double&, double&,
                               double& , double&, double&,
                               int& , double&, double&)=0;

  virtual int countTracks( LHCb::Particle::ConstVector& )=0;
  
  virtual const LHCb::Particle* motherof( const LHCb::Particle* ,
                                          const LHCb::Particle::ConstVector& )=0;
  
  virtual bool isinTree(const LHCb::Particle*,
                        LHCb::Particle::ConstVector&, double&)=0;

  virtual LHCb::Particle::ConstVector FindDaughters( const LHCb::Particle* )=0;

  virtual const LHCb::MCParticle* originof( const LHCb::MCParticle* )=0 ;

  virtual int comes_from_excitedB(const LHCb::MCParticle* , 
                                  const LHCb::MCParticle* )=0;

  virtual StatusCode calcDOCAmin( const LHCb::Particle* ,
				  const LHCb::Particle* , 
				  const LHCb::Particle* , double& , double& )=0;
};
#endif // ITAGGINGUTILSCHECKER_H
