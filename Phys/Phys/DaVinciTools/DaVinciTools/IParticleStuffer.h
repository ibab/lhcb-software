// $Id: IParticleStuffer.h,v 1.1 2002-03-27 20:27:45 gcorti Exp $
#ifndef DAVINCITOOLS_IPARTICLESTUFFER_H 
#define DAVINCITOOLS_IPARTICLESTUFFER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
// from Event
#include "Event/Particle.h"

static const InterfaceID IID_IParticleStuffer\
                           ("IParticleStuffer", 1 , 0); 

/** @class IParticleStuffer IParticleStuffer.h 
 *
 *  @author Paul colrain
 *  @date   14/03/2002
 */

class IParticleStuffer : virtual public IAlgTool {

public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() {
    return IID_IParticleStuffer;
  }

  /// Fill Composite Particle from Vertex
  virtual StatusCode fillParticle( const Vertex&, Particle&, 
                                   const ParticleID& ) = 0;

  /// Fill Particle from ProtoParticle
  //virtual StatusCode fillParticle( const ProtoParticle&, Particle&,  
  //                                 const ParticleID& ) = 0;

};
#endif // DAVINCITOOLS_IPARTICLESTUFFER_H


