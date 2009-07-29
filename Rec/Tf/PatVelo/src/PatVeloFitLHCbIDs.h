// $Id: PatVeloFitLHCbIDs.h,v 1.4 2009-07-29 16:25:59 dhcroft Exp $
#ifndef TF_PATVELOFITLHCBIDS_H 
#define TF_PATVELOFITLHCBIDS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "TrackInterfaces/ITrackFitter.h"

#include "PatVeloRHitManager.h"
#include "PatVeloPhiHitManager.h"

namespace LHCb {
  class Track;
}

namespace Tf {
  class PatVeloSpaceTrack;
  class PatVeloTrackTool;
  
  // updated interface ID to 2.0 as part of the Tf update
  static const InterfaceID IID_PatVeloFitLHCbIDs ("Tf::PatVeloFitLHCbIDs",1,0);

  /** @class PatVeloFitLHCbIDs PatVeloFitLHCbIDs.h
   *  Tool to take a list of LHCbIDs and fit a track as per PatVelo
   *
   *  @author David Hutchcroft
   *  @date   2008-02-07
   */
  class PatVeloFitLHCbIDs : public GaudiTool, 
			    virtual public ITrackFitter {
  public: 

    // Return the interface ID
    static const InterfaceID& interfaceID() { return IID_PatVeloFitLHCbIDs; }

    /// Standard constructor
    PatVeloFitLHCbIDs( const std::string& type, 
		      const std::string& name,
		      const IInterface* parent);

    virtual ~PatVeloFitLHCbIDs( ); ///< Destructor

    StatusCode initialize(); ///< initialize

    /// fit the track using the LHCbIDs on it with the PatVelo internal fit
    virtual StatusCode fit ( LHCb::Track & track, 
                             LHCb::ParticleID pid = LHCb::ParticleID(211) );

  private:

    /// the specialized r hit manager
    PatVeloRHitManager* m_rHitManager;

    /// the specialized phi hit manager
    PatVeloPhiHitManager* m_phiHitManager;
    
    /// Pat Velo Track Tool
    PatVeloTrackTool * m_PatVeloTrackTool;

    /// standard MS fitting parameter at vertex
    double m_stepError;
    /// standard MS fitting parameter at end of VELO
    double m_forwardStepError;
    
    /// make a state at the beam position, if false at first measurement 
    bool m_beamState;

    /// use the momentum dependant formula for MS correction
    bool m_variableMS;

    /// Name of the R hit manager instance
    std::string m_rHitManagerName;

    /// Name of the Phi hit manager instance
    std::string m_phiHitManagerName;

    /// Name of the Track Tool used here
    std::string m_trackToolName;

    /// Number of measurements to take with a full error
    unsigned int m_fullErrorPoints; 

  };
}
#endif // TF_PATVELOFITLHCBIDS_H
