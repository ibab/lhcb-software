
//-----------------------------------------------------------------------------
/** @file IRichTrackCreator.h
 *
 *  Header file for RICH reconstruction tool interface : IRichTrackCreator
 *
 *  CVS Log :-
 *  $Id: IRichTrackCreator.h,v 1.7 2005-10-13 15:38:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRICHTRACKCREATOR_H
#define RICHRECTOOLS_IRICHTRACKCREATOR_H 1

// Event
class ContainedObject;
#include "Event/RichRecTrack.h"

/// Static Interface Identification
static const InterfaceID IID_IRichTrackCreator( "IRichTrackCreator", 1, 0 );

/** @class IRichTrackCreator IRichTrackCreator.h
 *
 *  An abstract Interface to tools for the creation and book-keeping of
 *  RichRecTrack objects.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

class IRichTrackCreator : public virtual IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichTrackCreator; }

  /** Creates and returns the RichRecTrack associated to the given data object.
   *
   *  @param obj The data object to use to build the RichRecTrack. The runtime
   *             type of obj must correspond to that expected by the particular 
   *             implementation being used.
   *
   *  @return Pointer to the associated RichRecTrack object
   *  @retval NULL  Unable to build a RichRecTrack from the input data object
   *  @retval !NULL Object was successfully built
   */
  virtual RichRecTrack * newTrack ( const ContainedObject * obj ) const = 0;

  /** Form all possible RichRecTracks from data objects at the configured
   *  input location in the TES.
   *  The most efficient way to make all RichRecTrack objects in the event.
   *
   * @return status code for the request
   * @retval StatusCode::SUCCESS All possible RichRecTracks were successfully built
   * @retval StatusCode::FAILURE A problem occurred whilst building the RichRecTracks
   */
  virtual const StatusCode newTracks() const = 0;

  /** Access to all RichRecTracks currently created using this tool
   *  Tool should never return a null pointer.
   *
   *  @return Pointer to the container of RichRecTracks
   */
  virtual RichRecTracks * richTracks() const = 0;

  /** Number of tracks in the input track container (e.g. Tracks, TrStoredTracks or TrgTracks)
   *
   *  @todo   Review if this method is really needed
   *
   *  @return The number of data objects in the input container
   */
  virtual const long nInputTracks() const = 0;

};

#endif // RICHRECTOOLS_IRICHTRACKCREATOR_H
