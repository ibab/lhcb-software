// $Id: IRichTrackCreator.h,v 1.2 2003-10-13 16:10:54 jonrob Exp $
#ifndef RICHRECTOOLS_IRICHTRACKCREATOR_H
#define RICHRECTOOLS_IRICHTRACKCREATOR_H 1

// Event
class ContainedObject;
#include "Event/RichRecTrack.h"

/** @class IRichTrackCreator IRichTrackCreator.h
 *
 *  An abstract Interface to tools for the creation and book-keeping of
 *  RichRecTrack objects.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

static const InterfaceID IID_IRichTrackCreator( "IRichTrackCreator", 1, 0 );

class IRichTrackCreator : public virtual IAlgTool {

public:

  /// static interface identification
  static const InterfaceID& interfaceID() { return IID_IRichTrackCreator; }

  /// Returns a RichRecTrack object pointer for given ContainedObject.
  /// If if it not possible NULL is return.
  virtual RichRecTrack * newTrack ( const ContainedObject * obj ) = 0;

  /// Form all possible RichRecTracks from input Objects.
  /// The most efficient way to make all RichRecTrack objects in the event.
  virtual StatusCode newTracks() = 0;

  /// Returns a pointer to the container of RichRecTracks.
  virtual RichRecTracks *& richTracks() = 0;

};

#endif // RICHRECTOOLS_IRICHTRACKCREATOR_H
