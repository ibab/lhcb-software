// $Id: IParticleTransporter.h,v 1.3 2002-07-24 18:20:05 gcorti Exp $
#ifndef DAVINCITOOLS_IPARTICLETRANSPORTER_H 
#define DAVINCITOOLS_IPARTICLETRANSPORTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/Matrix.h"

// Forward declarations

// from Event
#include "Event/Particle.h"

static const InterfaceID IID_IParticleTransporter("IParticleTransporter",2,0); 

/** @class IParticleTransporter IParticleTransporter.h
 *  DaVinciTools/IParticleTranporter.h
 *  
 *  Particle transporter interface.
 *
 *  @author Edgar de Oliveira
 *  @date   22/02/2002
 */

class IParticleTransporter : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IParticleTransporter;}

  /// Transport a Particle to specified z position given a Particle iterator.
  virtual StatusCode transport(ParticleVector::const_iterator &, 
                               double znew,
                               Particle &transParticle) = 0;
  
  /// Transport a Particle to specified z position.
  virtual StatusCode transport(const Particle &, 
                               double znew,
                               Particle &transParticle) = 0;

  /// Transport a Particle to specified z position.
  virtual StatusCode transport(Particle &, 
                               double znew,
                               Particle &transParticle) = 0;
};
#endif // DAVINCITOOLS_IPARTICLETRANSPORTER_H
