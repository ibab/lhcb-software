// $Id: IParticleStuffer.h,v 1.2 2006-03-15 13:34:02 pkoppenb Exp $
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
 *  or given a state
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

  /// Fill Composite Particle from a state
  virtual StatusCode fillParticle( const LHCb::State& state,
                                   LHCb::Particle& part) = 0;

  /// Sum 4-vectors for a vector of daughters
  virtual Gaudi::XYZTVector sumMomenta( const LHCb::Particle::ConstVector& ) = 0;

  /// Sum 4-vectors for a vector of daughters
  virtual Gaudi::XYZTVector sumMomenta( const SmartRefVector<LHCb::Particle>& ) = 0;



};
#endif // DAVINCIKERNEL_IPARTICLESTUFFER_H


