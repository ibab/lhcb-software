// $Id: PatVeloRTracking.h,v 1.1.1.1 2007-08-26 21:03:29 krinnert Exp $
#ifndef TF_PATVELORTRACKING_H 
#define TF_PATVELORTRACKING_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "TfKernel/DefaultVeloRHitManager.h"

// Local
#include "PatRZTrack.h"

namespace Tf {
  /** @class PatVeloRTracking PatVeloRTracking.h
   *  Reconstruct the R-Z tracks in the Velo
   *
   *  @author Olivier Callot
   *  @date   2005-05-31
   */
  class PatVeloRTracking : public GaudiAlgorithm {
    public: 
      /// Standard constructor
      PatVeloRTracking( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~PatVeloRTracking( ); ///< Destructor

      virtual StatusCode initialize();    ///< Algorithm initialization
      virtual StatusCode execute   ();    ///< Algorithm execution
      virtual StatusCode finalize  ();    ///< Algorithm finalization

    protected:
      /// Print a cluster and its associated keys
      void printCluster( const VeloRHit* clus, MsgStream& msg );

    private:

      typedef VeloRHit::range_type RHitRange;                                        ///< shortcut to range of r hits
      typedef DefaultVeloRHitManager::StationIterator        StationIterator;        ///< shortcut for station iterator
      typedef DefaultVeloRHitManager::StationReverseIterator StationReverseIterator; ///< shortcut for reverse station iterator

    private:
      /** Take three sectors and set up the combinations to pass to 
        seedInSectorTriplet.
        */
      void makeForwardSectorTriplets(	 StationReverseIterator station0, unsigned int zone0,
          StationReverseIterator station1, unsigned int zone1,
          StationReverseIterator station2, unsigned int zone2,
          StationReverseIterator rStationsEnd,
          unsigned int sector,      
          std::vector<PatRZTrack> &rzTracks);

      /** find using 3 sector pointers seeds tracks into rzTracks
        also extends tracks from initial triplet, into opposite side
        for forward tracks
        */
      void seedInSectorTriplet(bool forward, 
          std::vector<PatRZTrack> &rzTracks,
          const int &iCase, unsigned int sector,
          StationIterator station0, unsigned int zone0,
          StationIterator station1, unsigned int zone1,
          StationIterator station2, unsigned int zone2);

      /** extend a tracks into more sectors, starting with sectPt
        if extraCoord==0 add hits into track, otherwise return new hits 
        in extraCoord 
        returns number of missed stations before track exits acceptance at low r
        */
      int extendTrack(PatRZTrack &newTrack, 
          StationIterator station, unsigned int zone,
          unsigned int &nbUsed,
          double tol, 
          bool forward,
          VeloRHits *extraCoord);

      /// add the hits from the other side of the detector to a track
      void addOppositeSideHits(PatRZTrack &newTrack, const int &zone,
          const DeVeloRType* sensor );

      /// Merge tracks sharing hits
      void mergeTracks( std::vector<PatRZTrack> &rzTracks );

    private:
      DefaultVeloRHitManager*   m_hitManager;

      std::string  m_outputLocation;    ///< name in PatDataStore for tracks
      bool m_mergeTracks;               ///< If true, merge tracks sharung hits
      int  m_nCommonToMerge;            ///< Num of shared hits to merge the tracks

      DeVelo* m_velo;            ///< Detector element

      double m_zVertexMin;      ///< Minimal Z of a vertex for forward tracks
      double m_zVertexMax;      ///< Maximal Z of a vertex for backward tracks
      double m_maxRSlope;       ///< Maximum RZ slope considered
      double m_rMatchTol;       ///< R match tolerance in a triplet, in pitch unit
      double m_rExtraTol;       ///< R match tol. extrapolating, in pitch unit
      double m_rOverlapTol;     ///< R match in overlap, in pitch unit
      int m_maxMissed;          ///< Maximum number of consecutive missed sensors
      unsigned int m_minToTag;  ///< Minimum number of hit on track to tag as used
      double m_chargeThreshold; ///< threshold for high signal
      double m_highChargeFract; ///< minimal fraction ofver threshold
      bool m_isDebug;           ///< is debug output level
      bool m_isVerbose;         ///< is verbose output level 
      /// If true look for tracks crossing from outer to inner R sectors
      bool m_adjacentSectors;   
      /// Minimum Z for first sensor when searching for forward triplets
      double m_zSensorSearchMin;
      /// Maximum Z for first sensor when searching for forward triplets
      double m_zSensorSearchMax;

  };
}
#endif // TF_PATVELORTRACKING_H
