// $Id: ExtrapolatorChecker.h,v 1.3 2007-05-31 15:33:54 cattanem Exp $
#ifndef EXTRAPOLATORCHECKER_H 
#define EXTRAPOLATORCHECKER_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

// from Kernel/MCInterfaces
#include "MCInterfaces/ITrackCriteriaSelector.h"
#include "MCInterfaces/IIdealStateCreator.h"

// from Tr/TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"

// Forward declarations
namespace LHCb
{
  class MCParticle;
  class MCHit;
}

class IMagneticFieldSvc;

/** @class ExtrapolatorChecker ExtrapolatorChecker.h
 *  
 *  This algorithm checks the performance of the TrackExtrapolators. This
 *  algorithm can be run in Boole or Brunel, since it relies only on the
 *  presence of MCHits. Algorithm description:
 *  @li create a State from a MCHit,
 *  @li extrapolate this State to the next MCHit on the track,
 *  @li compare extrapolated state with MCHit parameters,
 *  @li and make the corresponding resolution and pull plots.
 *
 *  The plots are split up in:
 *  @li first MCHit (extrapolated from true vertex)
 *  @li from VELO MCHit to VELO MCHit
 *  @li from TT MCHit to TT MCHit
 *  @li from IT MCHit to IT MCHit
 *  @li from OT MCHit to OT MCHit
 *  @li through RICH1: from VELO MCHit to TT MCHit 
 *  @li through magnet: from TT MCHit to IT/OT MCHit 
 *
 *  Note that the extrapolations are done starting from the @b entry @b point
 *  of the MCHit and extrapolating to the entry point of the next MCHit. A
 *  correction for the magnetic field is applied on the slopes (@e tx and @e ty)
 *  of the MCHit. This is required since the displacement vector of the MCHit 
 *  is calculated from the difference between the exit and entry point, and
 *  therefore this displacement vector already accounts for half the effect of
 *  the magnetic field. This effect is subtracted in the correction method.
 * 
 *  @author Jeroen van Tilburg
 *  @date   2006-07-06
 */

class ExtrapolatorChecker : public GaudiHistoAlg {

public:

  /// Standard constructor
  ExtrapolatorChecker( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~ExtrapolatorChecker();

  /// Algorithm initialization
  virtual StatusCode initialize();

  /// Algorithm execution
  virtual StatusCode execute();

  /// Algorithm finalization
  virtual StatusCode finalize();

private:

  /// Find the next MCHit belonging to the same MCParticle starting from a z-pos
  int findNextHit( const LHCb::MCParticle* mcPart,
                   const double zRec,
                   LHCb::MCHit*& closestHit,
                   std::string& detectorName) const;

  /// Helper function to find the next MCHit
  void findNextXxxHit( const LHCb::MCParticle* mcPart,
                       const double zRec,
                       std::string linkPath,
                       LHCb::MCHit*& closestHit ) const;

  /// Get the q/p for a given MCHit
  double qOverP( const LHCb::MCParticle* mcPart,
                 const LHCb::MCHit* mcHit ) const;
 
  /// Correct slopes for magnetic field given an MCHit and a MCParticle
  void correctSlopes( const LHCb::MCParticle* mcPart, const LHCb::MCHit* mcHit,
                      double& tx, double& ty ) const;
 
  // Interfaces
  ITrackCriteriaSelector* m_trackSelector;///< Pointer to TrackCriteriaSelector
  IIdealStateCreator*     m_stateCreator; ///< Pointer to IdealStateCreator
  ITrackExtrapolator*     m_extrapolator; ///< Pointer to the extrapolator
  IMagneticFieldSvc*      m_magSvc;       ///< Pointer to magnetic field service

  /// String of the available detectors.
  std::vector<std::string> m_dets;

  // jobOptions
  std::string m_trackSelectorName; ///< Tracks selector tool name

};
#endif // EXTRAPOLATORCHECKER_H
