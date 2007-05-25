// $Id: ITrackReconstructible.h,v 1.1.1.1 2007-05-25 16:24:33 cattanem Exp $
#ifndef TRACKMCINTERFACES_ITRACKRECONSTRUCTIBLE_H 
#define TRACKMCINTERFACES_ITRACKRECONSTRUCTIBLE_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb {
 class MCParticle;
};

static const InterfaceID IID_ITrackReconstructible( "ITrackReconstructible", 0, 0 );

/** @class ITrackReconstructible TrackMCInterfaces/ITrackReconstructible.h
 *  
 *
 *  @author Eduardo Rodrigues, Jose Hernando
 *  @date   2005-05-20
 */

class ITrackReconstructible : virtual public IAlgTool {
public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackReconstructible; }

  virtual bool hasVelo        ( const LHCb::MCParticle* tr ) const = 0;
  virtual bool hasTT          ( const LHCb::MCParticle* tr ) const = 0;
  virtual bool hasSeed        ( const LHCb::MCParticle* tr ) const = 0;
  virtual bool hasVeloAndSeed ( const LHCb::MCParticle* tr ) const = 0;
  
};
#endif // TRACKMCINTERFACES_ITRACKRECONSTRUCTIBLE_H 
