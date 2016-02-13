// $Id: PatVeloTrackTool.h,v 1.6 2009/06/24 18:04:35 dhcroft Exp $
#ifndef TF_PATVELOTRACKTOOL_H 
#define TF_PATVELOTRACKTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "PatVeloPhiHitManager.h"
#include "CircularRangeUtils.h"

// from VeloDet
#include "VeloDet/DeVelo.h"

// Track Event:
#include "Event/Track.h"
#include "Event/State.h"

namespace Tf {
  class PatVeloSpaceTrack;
  class PatVeloRHitManager;
  class PatVeloPhiHitManager;
  
  // updated interface ID to 2.0 as part of the Tf update
  static const InterfaceID IID_PatVeloTrackTool ("Tf::PatVeloTrackTool",2,0);

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

    ~PatVeloTrackTool( ) override; ///< Destructor

    StatusCode initialize() override; ///< initialize

    /// remove overlap hits from non-overlap tracks, 
    /// also clean R hits in corner cutouts
    /// stepErr is MS error per velo station assumed in fit errors
    bool cleanNonOverlapTracks(PatVeloSpaceTrack& tr,
			       double stepErr,
			       unsigned int fullErrorPoints = 0) const;

    /// Set (or reset) the phi coordinates to match the RZ track fit
    void setPhiCoords(PatVeloSpaceTrack& track) const;

    /// test if a track contains too many spillover like clusters
    bool isSpilloverTrack(const PatVeloSpaceTrack& track) const;

    /// Turn an LHCb::track into a PatVeloSpaceTrack
    std::unique_ptr<PatVeloSpaceTrack> makePatVeloSpaceTrack(const LHCb::Track & pTrack) const;

    /// Convert a pat track into a normal track
    StatusCode makeTrackFromPatVeloSpace(PatVeloSpaceTrack&  patTrack,
					 LHCb::Track &newTrack,
					 double forwardStepError,
					 double beamState = true ) const;
    
    /// compare the phi of a sector with the RZ track to see if compatible
    bool phiRange( double radius, int RZone, double tol,
		   PatVeloPhiHitManager::Station* phiStation, 
		   unsigned int phiZone,
		   std::pair<double,double>& phiOverlap);

    /// given the sensor number and local phi get the x offset to global frame
    inline double xOffsetGlobal(unsigned int sensorNumber,double phi) const{      
      return m_XOffsetGlobal[sensorNumber][topPhi(phi)];
    }

    /// given the sensor number and local phi get the y offset to global frame
    inline double yOffsetGlobal(unsigned int sensorNumber,double phi) const{
      return m_YOffsetGlobal[sensorNumber][topPhi(phi)];
    }

    /// given the sensor number and local phi get the x offset to other 
    /// halfbox frame
    inline double xOffsetOtherHB(unsigned int sensorNumber,double phi) const{
      return m_XOffsetOtherHB[sensorNumber][topPhi(phi)];
    }

    /// given the sensor number and local phi get the y offset to other 
    /// halfbox frame
    inline double yOffsetOtherHB(unsigned int sensorNumber,double phi) const{
      return m_YOffsetOtherHB[sensorNumber][topPhi(phi)];
    }

    /// given the sensor number and local phi get the x offset to other 
    /// halfbox frame
    inline double rOffsetOtherHB(unsigned int sensorNumber,double phi) const{
      return m_ROffsetOtherHB[sensorNumber][topPhi(phi)];
    }

    /// given the sensor number and local phi get the y offset to other 
    /// halfbox frame
    inline double phiOffsetOtherHB(unsigned int sensorNumber,
				   double phi,double r) const{
      return atan2(r*sin(phi)+yOffsetOtherHB(sensorNumber,phi),
		   r*cos(phi)+xOffsetOtherHB(sensorNumber,phi)) - phi;
    }
		   

    /// add a state to the track and correct for half box position (if required)
    void addStateToTrack(const PatVeloSpaceTrack& patTrack, LHCb::Track& newTrack,
			 LHCb::State::Location location, 
			 const Gaudi::TrackSymMatrix& covariance) const;

    /// average halfbox phi of global R zone [use global 0->7 range]
    /// (-pi/2 -> 3pi/2 for historical reasons)
    double phiGlobalRZone(unsigned int zone) const{ 
      return m_phiOfRZone[zone]; 
    }

  private:
    /// is phi in top half of detector: map to -pi -> pi range and 
    /// if +ve then top half=0, bottom=1
    inline unsigned int topPhi( double phi ) const {
      if(phi <  -1.*Gaudi::Units::pi)  phi+= Gaudi::Units::twopi;
      if(phi > Gaudi::Units::pi) phi -= Gaudi::Units::twopi;
      return (phi>0 ? 0 : 1);
    }

    /// setup the cached alignment conditions
    StatusCode registerConditionCallBacks();
    /// load the box offsets
    StatusCode updateBoxOffset();

    /// the specialized r hit manager
    PatVeloRHitManager* m_rHitManager;

    /// the specialized phi hit manager
    PatVeloPhiHitManager* m_phiHitManager;

    /// Tolerance on track to R cluster comparison for removing overlap hits
    double m_phiAngularTol; 
    double m_chargeThreshold; ///<High threshold for cluster to be signal like
    double m_highChargeFract; ///<Fraction of high clusters required on track

    bool m_tracksInHalfBoxFrame; ///<If true do not correct for half box offset

    std::string m_rHitManagerName;   ///< the name of the R hit manager instance
    std::string m_phiHitManagerName; ///< the name of the Phi hit manager instance

    DeVelo* m_velo;                        ///< pointer to DeVelo
    
    /// utilities for dealing with angles
    CircularRangeUtils<double> m_angleUtils;

    ///< X left/right box offset -> global 
    std::vector<std::vector<double> > m_XOffsetGlobal; 
    ///< Y left/right box offset -> global
    std::vector<std::vector<double> > m_YOffsetGlobal; 

    ///< X left/right box offset -> other HB
    std::vector<std::vector<double> > m_XOffsetOtherHB; 
    ///< Y left/right box offset -> other HB
    std::vector<std::vector<double> > m_YOffsetOtherHB; 

     ///< X left/right box offset -> other HB
    std::vector<std::vector<double> > m_ROffsetOtherHB; 

    /// Use correction for half boxes when looking for overlaps
    bool m_OverlapCorrection;

    /// average halfbox phi of global R zone
    std::vector<double> m_phiOfRZone;
  };
}
#endif // TF_PATVELOTRACKTOOL_H
