// $Id: ExtrapolatorChecker.h,v 1.1 2006-07-06 13:14:59 jvantilb Exp $
#ifndef EXTRAPOLATORCHECKER_H 
#define EXTRAPOLATORCHECKER_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"

// from Tr/TrackMCInterfaces
#include "TrackMCInterfaces/ITrackCriteriaSelector.h"
#include "TrackMCInterfaces/IIdealStateCreator.h"

// from Tr/TrackInterfaces
#include "TrackInterfaces/ITrackExtrapolator.h"

// Forward declarations
namespace LHCb
{
  class MCParticle;
  class MCHit;
}

/** @class ExtrapolatorChecker ExtrapolatorChecker.h
 *  
 *  ...
 *
 *  @author Jeroen van Tilburg
 *  @date   2006-07-06
 */

class ExtrapolatorChecker : public GaudiHistoAlg {

public:

  // Standard constructor
  ExtrapolatorChecker( const std::string& name, ISvcLocator* pSvcLocator );

  // Destructor
  virtual ~ExtrapolatorChecker();

  // Algorithm initialization
  virtual StatusCode initialize();

  // Algorithm execution
  virtual StatusCode execute();

  // Algorithm finalization
  virtual StatusCode finalize();

private:

  // Get the q/p for a given MCHit
  double qOverP( const LHCb::MCParticle* mcPart, 
		 const LHCb::MCHit* mcHit ) const;
 
  // Find the next MCHit belonging to the same MCParticle starting from a z-pos.
  int findNextHit( const LHCb::MCParticle* mcPart,
		   const double zRec,
		   LHCb::MCHit*& closestHit,
		   std::string& detectorName) const;

  // Helper function to find the next MCHit
  void findNextXxxHit( const LHCb::MCParticle* mcPart,
		       const double zRec,
		       std::string linkPath,
		       LHCb::MCHit*& closestHit ) const;

  // Interfaces
  ITrackCriteriaSelector* m_trackSelector;// Pointer to TrackCriteriaSelector
  IIdealStateCreator*     m_stateCreator; // Pointer to IdealStateCreator
  ITrackExtrapolator*     m_extrapolator; // Pointer to TrackMasterExtrapolator

  // String of the available detectors.
  std::vector<std::string> m_dets;

  // jobOptions
  std::string m_trackSelectorName; // Tracks selector tool name


};
#endif // EXTRAPOLATORCHECKER_H
