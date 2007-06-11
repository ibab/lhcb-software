// $Id: IParticleTransporter.h,v 1.4 2007-06-11 08:07:31 cattanem Exp $
#ifndef KERNEL_IPARTICLETRANSPORTER_H 
#define KERNEL_IPARTICLETRANSPORTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// from Event
#include "Event/Particle.h"

static const InterfaceID IID_IParticleTransporter("IParticleTransporter",3,0); 

/** @class IParticleTransporter IParticleTransporter.h
 *  Kernel/IParticleTranporter.h
 *  
 *  Particle transporter interface.
 *
 *  @author Edgar de Oliveira
 *  @date   22/02/2002
 *  @author P. Koppenburg
 *  @date   16/12/2004
 *  -> Remove non const method. 
 */

class IParticleTransporter : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IParticleTransporter;}

  /// Transport a Particle to specified z position.
  virtual StatusCode transport(const LHCb::Particle*, 
                               const double znew,
                               LHCb::Particle &transParticle) = 0;

};
#endif // KERNEL_IPARTICLETRANSPORTER_H
