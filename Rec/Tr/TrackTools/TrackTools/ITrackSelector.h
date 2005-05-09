#ifndef TRACKTOOLS_ITRACKSELECTOR_H 
#define TRACKTOOLS_ITRACKSELECTOR_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class MCParticle;
class Track;

/** @class ITrackSelector ITrackSelector.h "TrackTools/ITrackSelector.h"
 *
 *  Interface class for the TrackSelector tool.
 *
 *  @author Jeroen van Tilburg
 *  @date   2003-07-28
 */

static const InterfaceID IID_ITrackSelector( "ITrackSelector", 1, 0 );

class ITrackSelector : virtual public IAlgTool
{
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackSelector; }

  virtual bool select( Track* track ) const = 0;
  virtual bool select( MCParticle* mcParticle ) = 0;

  virtual bool selectByTrackType( Track* track ) const = 0;
  virtual bool selectByTrackType( MCParticle* mcParticle ) = 0;

  virtual unsigned int trackType( MCParticle* mcPart ) = 0;

  virtual StatusCode setTrackType( MCParticle* mcPart,
                                   Track*& track ) = 0;

};

#endif // TRACKTOOLS_ITRACKSELECTOR_H
