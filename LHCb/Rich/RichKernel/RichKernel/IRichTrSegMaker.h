// $Id: IRichTrSegMaker.h,v 1.1 2004-06-17 12:00:47 cattanem Exp $
#ifndef RICHDETTOOLS_IRICHTRSEGMAKER_H
#define RICHDETTOOLS_IRICHTRSEGMAKER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// CLHEP
class HepPoint3D;
class HepVector3D;

// Event model
class ContainedObject;

// Rich Utils
class RichTrackSegment;
class RichGeomPhoton;

/** @class IRichTrSegMaker IRichTrSegMaker.h RichDetTools/IRichTrSegMaker.h
 *  A tool to create RichTrackSegments from reconstructed tracks
 *  or MCParticles.
 *
 *  @author Antonis Papanestis
 *  @date   2003-10-28
 */

static const InterfaceID IID_IRichTrSegMaker( "IRichTrSegMaker", 1, 0 );

class IRichTrSegMaker : public virtual IAlgTool {

public:

  static const InterfaceID& interfaceID() { return IID_IRichTrSegMaker; }

  /// Create RichTrackSegments from a track object
  virtual int constructSegments( const ContainedObject * track,
                                 std::vector<RichTrackSegment>& segments ) const = 0;

};
#endif // RICHDETTOOLS_IRICHTRSEGMAKER_H
