
//-----------------------------------------------------------------------------
/** @file IRichSegmentCreator.h
 *
 *  Header file for RICH reconstruction tool interface : IRichSegmentCreator
 *
 *  CVS Log :-
 *  $Id: IRichSegmentCreator.h,v 1.7 2006-01-23 14:08:55 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_IRICHSEGMENTCREATOR_H
#define RICHRECTOOLS_IRICHSEGMENTCREATOR_H 1

// Event model
#include "Event/RichRecSegment.h"

/// Static Interface Identification
static const InterfaceID IID_IRichSegmentCreator( "IRichSegmentCreator", 1, 0 );

//-----------------------------------------------------------------------------
/** @class IRichSegmentCreator IRichSegmentCreator.h
 *
 *  Interface for tool for the creation and book-keeping of RichRecSegment objects.
 *  This is a utility tool and should only used by the RichTrackCreator tools.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class IRichSegmentCreator : public virtual IAlgTool
{

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichSegmentCreator; }

  /** Saves a previously created RichRecSegment to the container,
   *  which is stored in the configured location in the TES
   *
   *  @param segment Pointer to the RichRecSegment to save
   */
  virtual void saveSegment( LHCb::RichRecSegment * segment ) const = 0;

  /** Create a new RichRecSegment object for the given data objects
   *
   *  @attention It is the responsibility of the user to save or delete
   *             the RichRecSegments returned by this method
   *
   *  @param segment The geometrical segment object to build the RichRecSegment with
   *  @param pTrk    The RichRecTrack with which to associate the new RichRecSegment
   */
  virtual LHCb::RichRecSegment * newSegment( LHCb::RichTrackSegment * segment,
                                             LHCb::RichRecTrack * pTrk ) const = 0;

  /** Access to the container of RichRecSegments
   *
   *  @return Pointer to the RichRecSegments currently saved by this tool
   */
  virtual LHCb::RichRecSegments * richSegments() const = 0;

};

#endif // RICHRECTOOLS_IRICHSEGMENTCREATOR_H
