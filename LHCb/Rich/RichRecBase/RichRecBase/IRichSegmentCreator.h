// $Id: IRichSegmentCreator.h,v 1.4 2004-04-17 09:28:03 jonesc Exp $
#ifndef RICHRECTOOLS_IRICHSEGMENTCREATOR_H
#define RICHRECTOOLS_IRICHSEGMENTCREATOR_H 1

// Event model
#include "Event/RichRecSegment.h"

/** @class IRichSegmentCreator IRichSegmentCreator.h
 *
 *  Interface for tool for the creation and book-keeping of RichRecSegment objects.
 *  This is a utility tool and should only used by the RichTrackCreator tools.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichSegmentCreator( "IRichSegmentCreator", 1, 0 );

class IRichSegmentCreator : public virtual IAlgTool {

public:

  /// Return the unique interface identifier
  static const InterfaceID& interfaceID() { return IID_IRichSegmentCreator; }

  /// Saves a new RichRecSegment
  virtual void saveSegment( RichRecSegment * segment ) const = 0;

  /// Create a new RichRecSegment
  virtual RichRecSegment * newSegment( const RichTrackSegment& segment, 
                                       RichRecTrack * pTrk ) const = 0;

  /// Returns a pointer to container of RichRecSegments
  virtual RichRecSegments * richSegments() const = 0;

};

#endif // RICHRECTOOLS_IRICHSEGMENTCREATOR_H
