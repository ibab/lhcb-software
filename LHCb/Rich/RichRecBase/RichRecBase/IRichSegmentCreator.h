// $Id: IRichSegmentCreator.h,v 1.2 2003-08-06 09:55:31 jonrob Exp $
#ifndef RICHRECTOOLS_IRICHSEGMENTCREATOR_H
#define RICHRECTOOLS_IRICHSEGMENTCREATOR_H 1

// Event model
class RichRecSegment;

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
  static const InterfaceID& interfaceID() {return IID_IRichSegmentCreator;}

  /// Saves a new RichRecSegment
  virtual void saveSegment( RichRecSegment * segment ) = 0;

  /// Create a new RichRecSegment
  virtual RichRecSegment * newSegment( RichTrackSegment& segment, 
                                       RichRecTrack* pTrk ) = 0;

  /// Returns a pointer to container of RichRecSegments
  virtual RichRecSegments *& richSegments() = 0;

};

#endif // RICHRECTOOLS_IRICHSEGMENTCREATOR_H
