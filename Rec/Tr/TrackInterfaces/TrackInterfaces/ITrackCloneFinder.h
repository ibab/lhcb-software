// $Id: ITrackCloneFinder.h,v 1.7 2009-06-26 12:49:19 wouter Exp $
#ifndef TRACKINTERFACES_ITRACKCLONEFINDER_H 
#define TRACKINTERFACES_ITRACKCLONEFINDER_H 1

// Include files
// -------------

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
namespace LHCb {
 class Track;
}

static const InterfaceID IID_ITrackCloneFinder ( "ITrackCloneFinder", 1, 0 );

/** @class ITrackCloneFinder ITrackCloneFinder.h TrackInterfaces/ITrackCloneFinder.h
 *  
 *  Interface for the clone finder tool among two input tracks
 *
 *  @author Eduardo Rodrigues
 *  @date   2005-12-08
 *  Modified for speed reason 
 *  @author Adrian Perieanu
 *  @date   2008-05-05
 */
class ITrackCloneFinder : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackCloneFinder; }

  /** Compare two input Tracks and find whether one is a clone
   *  of the other based on some "overlap criteria".
   *  Note: the method ignores whether the Tracks themselves have been
   *        previously flagged as clones! It merely does a comparison.
   *  @param  track1 input 1st track
   *  @param  track2 input 2nd track
   *  @param  setFlag input parameter indicates whether the clone track
   *          is to be set as such (default = false)
   */
  virtual bool areClones( const LHCb::Track& track1,
                          const LHCb::Track& track2 ) const = 0 ;

  /** Compare two input Tracks and find whether one is a clone
   *  of the other based on some "overlap criteria".
   *  The corresponding flag may be set accordingly (NOT DONE BY DEFAULT)
   *  depending on the value of the "setFlag" argument.
   *  Note: the method ignores whether the Tracks themselves have been
   *        previously flagged as clones! It merely does a comparison.
   *  @param  track1 input 1st track
   *  @param  track2 input 2nd track
   *  @param  setFlag input parameter indicates whether the clone track
   *          is to be set as such (default = false)
   */
  virtual bool flagClones( LHCb::Track& track1,
			   LHCb::Track& track2 ) const = 0 ;
};
#endif // TRACKINTERFACES_ITRACKCLONEFINDER_H
