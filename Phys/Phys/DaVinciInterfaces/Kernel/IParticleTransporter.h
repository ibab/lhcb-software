// $Id: IParticleTransporter.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
#ifndef KERNEL_IPARTICLETRANSPORTER_H 
#define KERNEL_IPARTICLETRANSPORTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// from Event
#include "Event/Particle.h"

/** @class IParticleTransporter IParticleTransporter.h
 *  Kernel/IParticleTranporter.h
 *  
 *  Particle transporter interface.
 *
 *  @author Edgar de Oliveira
 *  @date   22/02/2002
 *
 *  @author P. Koppenburg
 *  @date   16/12/2004
 *
 *  @author J. Palacios
 *  @date   15/04/2008
 */

class GAUDI_API IParticleTransporter : virtual public IAlgTool {
public:

  DeclareInterfaceID(IParticleTransporter, 4, 0);

  /// Transport a Particle to specified z position.
  virtual StatusCode transport(const LHCb::Particle*, 
                               const double znew,
                               LHCb::Particle& transParticle) = 0;

  /// Transport and project a Particle to specified z position.
  virtual StatusCode transportAndProject(const LHCb::Particle*, 
                                         const double znew,
                                         LHCb::Particle& transParticle) = 0;

};
#endif // KERNEL_IPARTICLETRANSPORTER_H
