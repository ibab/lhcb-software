// $Id: PatVeloTrackTool.h,v 1.1.1.1 2007-08-26 21:03:29 krinnert Exp $
#ifndef TF_PATVELOTRACKTOOL_H 
#define TF_PATVELOTRACKTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "CircularRangeUtils.h"

namespace LHCb {
  class Track;
}

namespace Tf {
  class PatVeloSpaceTrack;
  class PatVeloRHitManager;
  class PatVeloPhiHitManager;
  
  static const InterfaceID IID_PatVeloTrackTool ( "Tf::PatVeloTrackTool", 1, 0 );

  /** @class PatVeloTrackTool PatVeloTrackTool.h
   *  Tool to perform operations common to Velo pattern recognition code
   *
   *  @author David Hutchcroft
   *  @date   2006-07-27
   */
  class PatVeloTrackTool : public GaudiTool {
    public: 

      // Return the interface ID
      static const InterfaceID& interfaceID() { return IID_PatVeloTrackTool; }

      /// Standard constructor
      PatVeloTrackTool( const std::string& type, 
          const std::string& name,
          const IInterface* parent);

      virtual ~PatVeloTrackTool( ); ///< Destructor

      StatusCode initialize(); ///< initialize

      /// remove overlap hits from non-overlap tracks
      /// stepErr is MS error per velo station assumed in fit errors
      bool cleanNonOverlapTracks(PatVeloSpaceTrack *tr,
          double stepErr) const;

      /// Set (or reset) the phi coordinates to match the RZ track fit
      void setPhiCoords(PatVeloSpaceTrack* track) const;

      /// test if a track contains too many spillover like clusters
      bool isSpilloverTrack(PatVeloSpaceTrack* track) const;

      /// Turn an LHCb::track into a PatVeloSpaceTrack
      PatVeloSpaceTrack * makePatVeloSpaceTrack(const LHCb::Track & pTrack) const;

      /// Convert a pat track into a normal track
      StatusCode makeTrackFromPatVeloSpace(PatVeloSpaceTrack * patTrack,
          LHCb::Track *newTrack,
          double forwardStepError) const;

    private:

      /// the specialized r hit manager
      PatVeloRHitManager* m_rHitManager;

      /// the specialized phi hit manager
      PatVeloPhiHitManager* m_phiHitManager;

      /// Tolerance on track to R cluster comparison for removing overlap hits
      double m_phiAngularTol; 
      double m_chargeThreshold; ///< High threshold for cluster to be signal like
      double m_highChargeFract; ///< Fraction of high clusters required on track

      /// utilities for dealing with angles
      CircularRangeUtils<double> m_angleUtils;
  };
}
#endif // TF_PATVELOTRACKTOOL_H
