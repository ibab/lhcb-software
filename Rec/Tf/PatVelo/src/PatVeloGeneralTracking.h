#ifndef TF_PATVELOGENERALTRACKING_H 
#define TF_PATVELOGENERALTRACKING_H 1

// Include files
#include <cmath>
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/ISequencerTimerTool.h"

// from VeloDet
#include "VeloDet/DeVelo.h"

// local
#include "PatVeloRHitManager.h"
#include "PatVeloPhiHitManager.h"
#include "PatVeloSpaceTrack.h"
#include "PatVeloTrackTool.h"

namespace Tf {
  /** @class PatVeloGeneralTracking PatVeloGeneralTracking.h 
   *
   *  VELO pattern recognition designed to run after PatVeloRTracking and 
   *  PatVeloSpaceTracking. Uses remaing hits to find otherwise unfound tracks.
   *  Has no explicit cuts on track directions, implicit cuts are that track 
   *  must cross 3 stations on the same side of the detector. 
   *  It is useful for Ks decays, halo, and beam-gas tracks.
   *
   *  @author David Hutchcroft
   *  @date   2007-06-15
   */

  class PatVeloGeneralTracking : public GaudiAlgorithm {
  public: 
    /// Standard constructor
    PatVeloGeneralTracking( const std::string& name, ISvcLocator* pSvcLocator );
    virtual ~PatVeloGeneralTracking( ); ///< Destructor

    virtual StatusCode initialize(); ///< Algorithm initialization
    virtual StatusCode execute   (); ///< Algorithm execution
    virtual StatusCode finalize  (); ///< Algorithm finalization

  protected:

  private:

  /// Captive class describing a point in the VELO made of crossed coordinates
    class PatVeloLocalPoint {
    public:
      /// constructor: shifts point into global frame
      PatVeloLocalPoint(PatVeloRHit *rHit, PatVeloPhiHit *phiHit,double phi,
			double offsetX, double offsetY):
	m_rHit(rHit), m_phiHit(phiHit),
	m_pos(rHit->hit()->coordHalfBox()*cos(phi)+offsetX,
	      rHit->hit()->coordHalfBox()*sin(phi)+offsetY,rHit->z())
      {      
	m_dx = sqrt(rHit->hit()->variance()*std::pow(cos(phi),2) + 
		    phiHit->hit()->variance()*
		    std::pow(rHit->hit()->coordHalfBox()*sin(phi),2));
	m_dy = sqrt(rHit->hit()->variance()*std::pow(sin(phi),2) + 
		    phiHit->hit()->variance() * 
		    std::pow(rHit->hit()->coordHalfBox()*cos(phi),2));
      };
      /// check if either cluster is already used elsewhere
      inline bool used() const { 
	return (m_rHit->hit()->isUsedByAnyOtherThan(Tf::HitBase::UsedByVeloRZ) || 
		m_phiHit->hit()->isUsedByAnyOtherThan(Tf::HitBase::UsedByVeloRZ)); 
      }
      inline void setUsedByGeneral() {
	m_rHit->hit()->setStatus(HitBase::UsedByVeloGeneral);
	m_phiHit->hit()->setStatus(HitBase::UsedByVeloGeneral);
      }

      // inline getters
      inline double x() const { return m_pos.x(); }
      inline double y() const { return m_pos.y(); }
      inline double z() const { return m_pos.z(); }
      inline PatVeloRHit *rHit() { return m_rHit; } 
      inline PatVeloPhiHit *phiHit() { return m_phiHit; } 
      inline double deltaX() const { return m_dx; }
      inline double deltaY() const { return m_dy; }
      inline unsigned int rSensorNumber() {return m_rHit->sensorNumber();}

      /// less in X coord function for sorting and upper/lower bound
      struct lessX  {
	bool operator()(PatVeloLocalPoint const &one, 
			PatVeloLocalPoint const &two) {
	  return one.x() < two.x(); 
	};
	bool operator()(double const &one, 
			PatVeloLocalPoint const &two) { 
	  return one < two.x(); 
	};
	bool operator()(PatVeloLocalPoint const &one,
			double const &two) { 
	  return one.x() < two; 
	};
      };

    private:
      PatVeloRHit *m_rHit;            ///< R hit pointer
      PatVeloPhiHit *m_phiHit;        ///< Phi hit pointer
      Gaudi::XYZPoint m_pos;       ///< 3D point of coord (z at R sensor)
      double m_dx;                 ///< dx of point
      double m_dy;                 ///< dy of point

    };    

    typedef std::map<unsigned int, std::vector<PatVeloLocalPoint> > PointsContainer;

    /// Captive class describing a list of points in the VELO
    class PointsList {
    public:
      /// creator: always start from triplet of points
      PointsList( PatVeloLocalPoint *one,
		  PatVeloLocalPoint *two,
		  PatVeloLocalPoint *three,
		  double chi2,
		  bool isLeftSide):
	m_chi2(chi2), m_valid(true), m_isLeft(isLeftSide){
	m_points.reserve(10);
	m_points.push_back(one);
	m_points.push_back(two);
	m_points.push_back(three);
      };
      // inline getters
      inline std::vector<PatVeloLocalPoint*> & points() { return m_points; }
      inline double chi2() const { return m_chi2; }
      inline bool vaild() const { return m_valid; }
      inline bool isLeft() const { return m_isLeft; }
      // setter
      inline void setValid(bool valid) { m_valid = valid; }

      /// less in chi2 function for sorting
      struct lessChi2 {
	bool operator()(PointsList const &one, 
			PointsList const &two) { 
	  return one.chi2() < two.chi2() ; 
	}
      };

    private:
      std::vector<PatVeloLocalPoint*> m_points; ///< list of points
      double m_chi2;                            ///< chi2 of set of points
      bool m_valid;                             ///< still vaild to continue
      bool m_isLeft;                            ///< is in left half of detector
    };

    /// create all valid 3D points by crossing VeloCoords
    void buildAll3DClusters( PointsContainer & createdPoints );

    /// for a given R sector and inner and outer phi sector make points
    void build3DClusters(int zone,
			 PatVeloRHitManager::Station * rStation,
			 PatVeloPhiHitManager::Station * phiStation,
			 PointsContainer &createdPoints);

    /// take the container of 3D points and find tracks in it
    /// "overlap" sets how opposite side sensors are used
    void findTracks(PointsContainer &points,
		    std::vector<PatVeloSpaceTrack*> &tracks,
		    bool overlap);

    /// turn the three lists of points (representing 3 sensors) 
    /// into valid triplets
    void makeAllGoodTriplets(std::vector<PatVeloLocalPoint> &one,
			     std::vector<PatVeloLocalPoint> &two,      
			     std::vector<PatVeloLocalPoint> &three,
			     std::vector<PointsList> &triplets,
			     bool overlap);

    /// choose between triplets based on chi2 and only keep best of overlaps
    void sortAndKeepGoodTriplets(std::vector<PointsList> &triplets);

    /// extend a triplet into a longer track
    bool extendTrack(PointsList &trackPoints,
		     PointsContainer &points,
		     PatVeloSpaceTrack *newTrack);

    /// extend a track using single clusters
    void extendTrackSingleClusters(PatVeloSpaceTrackLocal::FrameParam &xFit,
				   PatVeloSpaceTrackLocal::FrameParam &yFit,
				   PatVeloSpaceTrack *newTrack);

    /// write out tracks to store that pass cuts
    void storeTracks(std::vector<PatVeloSpaceTrack*> & tracks);

    PatVeloRHitManager* m_rHitManager;     ///< R hit storage
    PatVeloPhiHitManager* m_phiHitManager; ///< Phi hit storage
    DeVelo* m_velo;                        ///< pointer to DeVelo
    PatVeloTrackTool * m_PatVeloTrackTool; ///< tool to handle PatVeloSpaceTracks

    int m_RSensorsMin; ///< index of the first R sensor
    int m_RSensorsMax; ///< index of the last R sensor
    int m_PhiSensorsMin; ///< index of the first Phi sensor
    int m_PhiSensorsMax; ///< index of the last Phi sensor

    bool m_isDebug;           ///< is debug output level
    bool m_isVerbose;         ///< is verbose output level 

    std::map<unsigned int, bool> m_isLeftRSens; ///< is R sensor on left side?

    std::string m_outputTracksLocation;///< name of place to write the tracks
    std::string m_rHitManagerName; ///< name of the R hit manager instance
    std::string m_phiHitManagerName; ///< name of the Phi hit manager instance
    std::string m_trackToolName; ///< name of the track tool instance used 
    double m_phiAngularTol;  ///< tolerance in rad between R and phi sectors
    double m_ErrScale;       ///< scale factor for linear cluster errors
    double m_ErrMin;         ///< const term to add to linear cluster errors
    double m_ErrExtrapScale; ///< error scale factor to apply to track
    double m_MaxMissedSensor;///< Number of sensors thet can be missed
    double m_chiSqDofMax;    ///< max chi2/dof of track to keep
    bool m_allCoords;        ///< make all tracks even if cluster already used
    /// MS error between two Velo points
    /// when fitting toward z=0 to  minimise position error on vertex
    double m_stepError;        
    /// Number of measurements to take with a full error
    unsigned int m_fullErrorPoints; 
    /// MS error between two Velo points
    /// when fitting away from z=0 to minimise error on slopes at Velo exit
    double m_forwardStepError;
    /// Also add single cluster to tracks after looking for 3D points
    bool m_useSingleClusters;
    ///< Number of stations beyond end of track to seach for extra single clusters
    unsigned int m_NStationSingle;    
    ///< 2*m_NStationSingle as an int
    int m_NSensorSingle;    
    /// Tolerance for matching singke clusters to tracks (in mm)
    double m_singleClusTol;   

    /// when XZ and YZ extrapolating only consider this many prior points
    unsigned int m_maxExtrapStat;

    unsigned int m_Num3DCreated;   ///< Number of 3D points created
    unsigned int m_NumEvt;         ///< Number of Events Processed

    CircularRangeUtils<double> m_angleUtils; ///< compare phi ranges

    std::vector<double> m_XOffsetTop; ///< X left/right box offset at top
    std::vector<double> m_XOffsetBottom; ///< X left/right box offset 
    std::vector<double> m_YOffsetTop; ///< Y left/right box offset at top
    std::vector<double> m_YOffsetBottom; ///< Y left/right box offset 

    /// Use correction for half boxes when looking for overlaps
    bool m_OverlapCorrection;

    /// Use different cuts for tracks in the overlap region
    bool m_overlapSearch;

    ///Window to make clusters in (abs(x)) if in overlap region
    double m_overlapXWindow;

    /// Number of 3D points in an R/Phi zone to cause the zone to be killed as too hot
    unsigned int m_ZoneMaxPoints;

    /// Timing tools
    ISequencerTimerTool* m_timerTool;
    int  m_veloGeneralTime;
    bool m_doTiming;

  };

}
#endif // TF_PATVELOGENERALTRACKING_H
