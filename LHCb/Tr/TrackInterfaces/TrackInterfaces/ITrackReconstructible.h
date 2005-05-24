#ifndef TRACKINTERFACES_ITRACKRECONSTRUCTIBLE_H 
#define TRACKINTERFACES_ITRACKRECONSTRUCTIBLE_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class MCParticle;

static const InterfaceID IID_ITrackReconstructible( "ITrackReconstructible", 0, 0 );

/** @class ITrackReconstructible TrackInterfaces/ITrackReconstructible.h
 *  
 *
 *  @author Eduardo Rodrigues, Jose Hernando
 *  @date   2005-05-20
 */

class ITrackReconstructible : virtual public IAlgTool {
public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackReconstructible; }

  virtual bool hasVelo        ( MCParticle* tr ) = 0;
  virtual bool hasTT          ( MCParticle* tr ) = 0;
  virtual bool hasSeed        ( MCParticle* tr ) = 0;
  virtual bool hasVeloAndSeed ( MCParticle* tr ) = 0;
  
};
#endif // TRACKINTERFACES_ITRACKRECONSTRUCTIBLE_H 
