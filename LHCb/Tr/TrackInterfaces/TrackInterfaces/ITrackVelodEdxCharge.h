
//-----------------------------------------------------------------------------
/** @file ITrackVelodEdxCharge.h
 *
 * Interface header file for ITrackVelodEdxCharge
 *
 * CVS Log :-
 * $Id: ITrackVelodEdxCharge.h,v 1.1 2006-07-19 13:04:04 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 18/07/2006
 */
//-----------------------------------------------------------------------------

#ifndef TRACKINTERFACES_ITrackVelodEdxCharge_H
#define TRACKINTERFACES_ITrackVelodEdxCharge_H

// Include files
#include "GaudiKernel/IAlgTool.h"

// forward declarations
namespace LHCb
{
  class Track;
}

static const InterfaceID IID_ITrackVelodEdxCharge( "ITrackVelodEdxCharge" , 1, 0 );

//-----------------------------------------------------------------------------
/** @class ITrackVelodEdxCharge ITrackVelodEdxCharge.h
 *
 *  Interface for TrackVeloCharge tool.
 *
 *  This tool takes as input a Track
 *  and gives as result the "estimated number of
 *  tracks which created its VELO track" using dE/dx
 *  information from the VELO.
 *
 *  This is mainly equal to one for standard track,
 *  and 2 for track (e+, e-) which comes from a photon
 *
 *  @author Richard Beneyton
 *  @date   21/05/2003
 *
 *  Updated for DC06
 *  @author Chris Jones
 *  @date 18/07/2006
 */
//-----------------------------------------------------------------------------

class ITrackVelodEdxCharge : virtual public IAlgTool
{

public:

  /// retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_ITrackVelodEdxCharge; }

  /** Returns the estimated number of tracks in the VELO for the given
   *  Track object
   *  @param track Pointer to a Track object to analyze
   *  @param nTrks Number of velo tracks
   *  @return StatusCode indicating if the calculation was successfully
   *          performed or not
   */
  virtual StatusCode nTracks( const LHCb::Track * track,
                              double & nTks ) const = 0;

};

#endif // TRACKINTERFACES_ITrackVelodEdxCharge_H

