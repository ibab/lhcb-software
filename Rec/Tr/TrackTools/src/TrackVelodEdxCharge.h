
//-----------------------------------------------------------------------------
/** @file TrackVelodEdxCharge.h
 *
 * Header file for tool TrackVelodEdxCharge
 *
 * CVS Log :-
 * $Id: TrackVelodEdxCharge.h,v 1.2 2007-02-06 13:15:36 cattanem Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 18/07/2006
 */
//-----------------------------------------------------------------------------

#ifndef TRACKTOOLS_TrackVelodEdxCharge_H
#define TRACKTOOLS_TrackVelodEdxCharge_H

// base class
#include "GaudiAlg/GaudiTool.h"

// Interfaces
#include "TrackInterfaces/ITrackVelodEdxCharge.h"

// Event
#include "Event/Track.h"
#include "Event/Measurement.h"
#include "Event/VeloPhiMeasurement.h"
#include "Event/VeloRMeasurement.h"

//-----------------------------------------------------------------------------
/** @class TrackVelodEdxCharge TrackVelodEdxCharge.h RecoTools/TrackVelodEdxCharge.h
 *
 *  Implementation of ITrackVelodEdxCharge
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

class TrackVelodEdxCharge : public GaudiTool,
                            virtual public ITrackVelodEdxCharge
{

public:

  /// Constructor
  TrackVelodEdxCharge(const std::string& type,
                      const std::string& name,
                      const IInterface* parent);

  /// Destructor
  virtual ~TrackVelodEdxCharge();

  /// Tool initialisation
  StatusCode initialize();

  /// Tool finalisation
  StatusCode finalize();

public:

  // Returns the estimated number of tracks in the VELO for the given
  StatusCode nTracks( const LHCb::Track * track,
                      double & nTks ) const;

private: // definitions etc.

  /// List of measurements
  typedef std::vector< LHCb::Measurement * > TkMeas;

private: // data

  double m_Normalisation; ///< average charge for a track
  double m_Ratio;         ///< Ratio to exclude high signals in velocluster
};

#endif // TRACKTOOLS_TrackVelodEdxCharge_H

