// $Id: IParticleStuffer.h,v 1.3 2006-05-09 16:21:27 pkoppenb Exp $
#ifndef DAVINCIKERNEL_IPARTICLESTUFFER_H 
#define DAVINCIKERNEL_IPARTICLESTUFFER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
// from Event
#include "Event/Particle.h"

static const InterfaceID IID_IParticleStuffer("IParticleStuffer", 1 , 0); 

/** @class IParticleStuffer IParticleStuffer.h 
 *  Interface Class for Filling a Particle.
 *  Fill a particle given a vertex and list of daughters
 *  @author Paul colrain
 *  @date   14/03/2002
 */

class IParticleStuffer : virtual public IAlgTool {

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() {
    return IID_IParticleStuffer;
  }

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


