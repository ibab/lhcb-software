// $Id: ITrackCloneFinder.h,v 1.1 2005-12-13 16:04:30 erodrigu Exp $
#ifndef TRACKINTERFACES_ITRACKCLONEFINDER_H 
#define TRACKINTERFACES_ITRACKCLONEFINDER_H 1

// Include files
// -------------

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from TrackEvent
#include "Event/Track.h"

static const InterfaceID IID_ITrackCloneFinder ( "ITrackCloneFinder", 1, 0 );

/** @class ITrackCloneFinder ITrackCloneFinder.h TrackInterfaces/ITrackCloneFinder.h
 *  
 *  Interface for the clone finder tool among two input tracks
 *
 *  @author Eduardo Rodrigues
 *  @date   2005-12-08
 */
class ITrackCloneFinder : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackCloneFinder; }

  /** Compares two input Tracks and finds whether one is a clone
   *  of the other based on some "overlap criteria".
   *  @return bool: True if one Track is a clone of the other.
   *                False otherwise.
   *  @param  track1 input 1st track
   *  @param  track2 input 2nd track
   */
  virtual bool areClones( const Track& track1,
                          const Track& track2 ) const = 0;

  /** Compares two input Tracks and finds whether one is a clone
   *  of the other based on some "overlap criteria".
   *  The corresponding flag may be set accordingly (done by default)
   *  depending on the value of the "setFlag" argument.
   *  @return bool: True if one Track was set as a clone of the other.
   *                False otherwise.
   *  @param  track1 input 1st track
   *  @param  track2 input 2nd track
   *  @param  setFlag input parameter indicates whether the clone track
   *          is to be set as such (default = true)
   */
  virtual bool areClones( Track& track1,
                          Track& track2,
                          bool setFlag = true ) const = 0;

protected:

private:

};
#endif // TRACKINTERFACES_ITRACKCLONEFINDER_H
