// $Id: IParticleStuffer.h,v 1.2 2009-08-04 09:45:31 jpalac Exp $
#ifndef DAVINCIKERNEL_IPARTICLESTUFFER_H 
#define DAVINCIKERNEL_IPARTICLESTUFFER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
// from Event
#include "Event/Particle.h"

/** @class IParticleStuffer IParticleStuffer.h 
 *  Interface Class for Filling a Particle. For expert use.
 *  Fill a particle given a vertex and list of daughters.
 *  @author P. Koppenburg from P. Colrain
 *  @date   14/03/2002
 */

class GAUDI_API IParticleStuffer : virtual public IAlgTool {

public:

  DeclareInterfaceID(IParticleStuffer, 2, 0);

  /// Fill Composite Particle from Vertex and daughters (PID already known)
  virtual StatusCode fillParticle( const LHCb::Particle::ConstVector& daughters,
                                   const LHCb::Vertex& vertex,
                                   LHCb::Particle& part) = 0;

  /// Fill Composite Particle from Vertex and daughters and PID
  virtual StatusCode fillParticle( const LHCb::Particle::ConstVector& daughters,
                                   const LHCb::Vertex& vertex,
                                   const LHCb::ParticleID& pid,
                                   LHCb::Particle& part) = 0;
};
#endif // DAVINCIKERNEL_IPARTICLESTUFFER_H


