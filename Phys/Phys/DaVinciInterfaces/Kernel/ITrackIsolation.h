// $Id: ITrackIsolation.h,v 1.1 2010-03-18 18:50:43 simone Exp $
#ifndef KERNEL_ITRACKISOLATION_H 
#define KERNEL_ITRACKISOLATION_H 1

// Include files

// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

/**
 *  @class ITrackIsolation ITrackIsolation.h Kernel/ITrackIsolation.h
 *  
 *  Abstract Interface of the Track Isolation tools
 *  
 *  @author Simone Bifani simone.bifani@cern.ch
 *  @date   2010-03-17
 */

class GAUDI_API ITrackIsolation : virtual public IAlgTool {

public:

  DeclareInterfaceID( ITrackIsolation, 1, 0 );

  /**
   *  trackConeMultiplicity method for the evaluation of
   *  the number of tracks/particles in a given range
   *  within a cone of given radius with respect to the
   *  seed track/protoparticle/particle
   *  
   *  The cone radius is defined as the difference in Eta
   *  and Phi between the seed track and a track in the
   *  range (same definition as in TupleToolTrackIsolation):
   *  
   *  R = sqrt (delta_Eta^2 + delta_Phi^2)
   *  
   *  The method returns the track multiplicity
   */

  virtual unsigned int trackConeMultiplicity( const LHCb::Track* seedTrack,
                                              const LHCb::Track::Range& eventTracks, 
                                              double coneRadius ) const = 0;

  virtual unsigned int trackConeMultiplicity( const LHCb::ProtoParticle* seedProtoParticle,
                                              const LHCb::Track::Range& eventTracks, 
                                              double coneRadius ) const = 0;

  virtual unsigned int trackConeMultiplicity( const LHCb::Particle* seedParticle,
                                              const LHCb::Track::Range& eventTracks, 
                                              double coneRadius ) const = 0;

  virtual unsigned int trackConeMultiplicity( const LHCb::ProtoParticle* seedProtoParticle,
                                              const LHCb::Particle::Range& eventParticles, 
                                              double coneRadius ) const = 0;

  virtual unsigned int trackConeMultiplicity( const LHCb::Particle* seedParticle,
                                              const LHCb::Particle::Range& eventParticles, 
                                              double coneRadius ) const = 0;

protected:

  virtual ~ITrackIsolation();

private:

};

#endif // KERNEL_ITRACKISOLATION_H
