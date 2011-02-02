
//-----------------------------------------------------------------------------
/** @file TrackVelodEdxCharge.h
 *
 * Header file for tool TrackVelodEdxCharge
 *
 * CVS Log :-
 * $Id: TrackVelodEdxCharge.h,v 1.5 2010-03-19 18:15:00 dhcroft Exp $
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
#include "DetDesc/Condition.h"

//-----------------------------------------------------------------------------
/** @class TrackVelodEdxCharge TrackVelodEdxCharge.h
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

public:

  /// Returns the scaled truncated mean of the ADCs for Velo measurements to check for double tracks
  /// nTrk the number of estimated particles that contributed dEdx to this track in the VELO
  StatusCode nTracks( const LHCb::Track * track,
                      double & nTks ) const;

private: // definitions etc.

  /// call back to update dEdx conditions from DB
  StatusCode i_cachedEdx();

  /// List of measurements
  typedef std::vector< LHCb::Measurement * > TkMeas;

private: // data

  double m_Normalisation; ///< Expected average charge for a single track
  double m_Ratio;         ///< Fraction of cluster considered (1-Ratio highest clusters are discarded)
  bool m_useConditions; ///< use SIMCOND or LHCBCOND values instead of properties
  Condition *m_dEdx; ///< Condition with dEdx content in SIMCOND/LHCBCOND 
};

#endif // TRACKTOOLS_TrackVelodEdxCharge_H

