#ifndef TF_PATVELOSPACETOOL_H 
#define TF_PATVELOSPACETOOL_H 1

#include <string>

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "TrackInterfaces/ITracksFromTrack.h"

#include "PatVeloPhiList.h"
#include "PatVeloRHitManager.h"
#include "PatVeloPhiHitManager.h"
#include "CircularRangeUtils.h"

class DeVelo;
namespace LHCb{
  class Track;
}

namespace Tf {
  class PatVeloSpaceTrack;
  class PatVeloTrackTool;

  /** @class PatVeloSpaceTool PatVeloSpaceTool.h
   * 
   *  Implemantion of ITracksFromTrack to turn RZ VELO tracks into 3D
   *  VELO tracks 
   *
   *  @author David Hutchcroft
   *  @date   2006-07-24
   */
  class PatVeloSpaceTool : public extends<GaudiTool, ITracksFromTrack> {
    public: 

      /// Standard constructor
      PatVeloSpaceTool( const std::string& type, 
          const std::string& name,
          const IInterface* parent);

      ~PatVeloSpaceTool( ) override; ///< Destructor

      StatusCode initialize() override; ///< Initialize use base class

      /// take a Velo only Track without phi clusters and add phi clusters
      StatusCode tracksFromTrack(const LHCb::Track& seed, std::vector<LHCb::Track*>& out) const override;

    private:
      /// take a PatVeloSpaceTrack without phi clusters and add phi clusters
      std::vector<std::unique_ptr<PatVeloSpaceTrack>> find3DSpaceTrack(const PatVeloSpaceTrack & track) const;


      /// Take a new sector and add to (or create) consistent phi cluster lists
      void findBestPhiClusInSect(PatVeloPhiHitManager::Station* station, 
				 unsigned int zone,
				 unsigned int RZone,
				 int nbTried,
				 double r,
				 const std::pair<double,double>& phiRange ) const;

      /// choose between phi cluster lists: best list added to track
      std::vector<std::unique_ptr<PatVeloSpaceTrack>> getBestPhiList(const PatVeloSpaceTrack & track,
          double nbTried) const ;

      /// dump a cluster/track extrapolation comparison to verbose() if required
      void dumpClusterComparison(PatVeloPhiLists::iterator & iPhiList,
          double xPred, double yPred,
          PatVeloPhiHit& pCoord) const;

      /// Merge phi lists sharing clusters
      void mergePhiLists( bool backward ) const;

    private:
      bool m_markClustersUsed; ///< Mark clusters as used if found as a 3D track
      /// Tolerance in phi when comparing a 2D track to a phi sector
      double m_phiAngularTol;  
      /// for 1st cluster tolerance is m_phiFirstTol * R 
      double m_phiFirstTol;
      /// later cluster tolerance is  m_phiMatchTol * (dz) / 60;
      double m_phiMatchTol;
      /// number of R sensors in the velo
      unsigned int m_numberRSensors;
      /// fraction of sensors crossed that must contain hits
      double           m_fractionFound;
      /// Check for adjacent RZ zones 
      bool m_adjacentSectors; 
      /// Fraction of phi clusters in common before merging lists
      double m_fractionPhiMerge;
      /// Number of stations from the end of the RZ track in which to allow 
      /// a phi list to start [Note: Also counts station beyond RZ track]
      int m_NMissedFirst;
      /// Name of the R hit manager instance
      std::string m_rHitManagerName;
      /// Name of the Phi hit manager instance
      std::string m_phiHitManagerName;
      /// Name of the Track Tool used here
      std::string m_trackToolName;


      double           m_chiSqDofMax;  ///< Maximum chiSq/ndf for an offline track
      /// MS error between two Velo points
      /// when fitting toward z=0 to  minimise position error on vertex
      double           m_stepError; 
      /// MS error between two Velo points
      /// when fitting away from z=0 to minimise error on slopes at Velo exit
      double           m_forwardStepError; ///< MS error between two Velo points
      /// Remove otherside R clusters from non-overlap tracks
      bool m_cleanOverlaps;                   
    
      unsigned int m_fullErrorPoints; ///< Number of measurements to take with a full error

      DeVelo * m_velo = nullptr; ///< Pointer to DeVelo object to get sensor numbers

      mutable PatVeloPhiLists m_phiPt; ///< Temporary storage for lists of phi clusters

      PatVeloPhiHitManager* m_phiHitManager = nullptr;
      PatVeloRHitManager*   m_rHitManager = nullptr;

      PatVeloTrackTool* m_trackTool = nullptr;

      CircularRangeUtils<double> m_angleUtils = { -Gaudi::Units::pi,Gaudi::Units::pi };
  };
}
#endif // TF_PATVELOSPACETOOL_H
