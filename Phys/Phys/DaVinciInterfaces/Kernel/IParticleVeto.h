// $Id: $
#ifndef IPARTICLEVETO_H 
#define IPARTICLEVETO_H 1

// Include files
// from STL
#include <string>
#include <vector>
// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/Particle.h"

static const InterfaceID IID_IParticleVeto ( "IParticleVeto", 1, 0 );

/** @class IParticleVeto IParticleVeto.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2012-04-25
 */
class IParticleVeto : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IParticleVeto; }
  virtual bool foundOverlap(const LHCb::Particle* particle, std::vector<std::string> containers,int mode=0)=0;
  virtual bool foundOverlap(const LHCb::Particle* particle, std::string container,int mode=0)=0;
  virtual bool foundOverlap(const LHCb::Particle* particle, const LHCb::Particle* p2,int mode=0)=0;
protected:
  virtual ~IParticleVeto() ;               // virtual and protected destructors

private:

};
#endif // IPARTICLEVETO_H
