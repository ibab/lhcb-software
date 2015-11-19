#ifndef PRDOWNSTREAM_H
#define PRDOWNSTREAM_H 1

// Include files
#include <string>
#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
//#include "GaudiAlg/GaudiHistoAlg.h"
//#include "GaudiAlg/GaudiTupleAlg.h"
#include "Kernel/ILHCbMagnetSvc.h"
#include "GaudiAlg/ISequencerTimerTool.h"

#include "TfKernel/UTStationHitManager.h"
#include "PrKernel/PrUTHit.h"

// forward declarations
class IPrDebugUTTool;
class DeSTDetector;
class PrDownTrack;

namespace LHCb {
  class Track;
}

/** @class PrDownstream PrDownstream.h
 *  Algorithm to reconstruct tracks with seed and UT, for KShort use.
 *  Based on Pr/PrKShort
 *
 *  @author Olivier Callot
 *  @date   2007-10-18
 *
 *  @author Sascha Stahl
 *  @date   2009-10-07
 *
 *  migrated code from J.C. Wang to PrAlgorithms
 *  date   2013-02-05
 *
 *  @author Georg Krocker
 *  @date   2014-07-04
 */

class PrDownstream : public GaudiAlgorithm {
//class PrDownstream : public GaudiTupleAlg {
public:
  /// Standard constructor
  PrDownstream( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PrDownstream( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  void utCoordCleanup();  ///< tag already used coordinates

  /**
  * @brief Checks if T-Seed part was already used in a Match track 
  *
  * @param inTracks All the input T-Seeds
  * @param myInTracks Vector of tracks that will be used in the Downstream
  */
  void prepareSeeds(LHCb::Tracks* inTracks, std::vector<LHCb::Track*>& myInTracks); // tag already used T-Seeds

  /**
  * @brief Checks if T-Seed part was already used in a Forward track 
  *
  * @param inTracks All the input T-Seeds
  * @param myInTracks Vector of tracks that will be used in the Downstream
  */
  void prepareSeedsNew(LHCb::Tracks* inTracks, std::vector<LHCb::Track*>& myInTracks); // tag already used T-Seeds

  /**
  * @brief Get hits in the UT/TT based on the initial track prediciton 
  *
  * @param track Initial Downstream track candidate
  */
  void getPreSelection( PrDownTrack& track );

  /**
  * @brief Fit XUVX track candidate and remove outliers
  *
  * @param track Downstream track candidate
  */
  void fitAndRemove( PrDownTrack& track );

  /**
  * @brief Find matching X hits in other X layer based on the Downstream track that has already one X hit
  *
  * @param MatchingXHits Selected X hits
  * @param track Downstream track candidate
  * @param plane Layer of the first X hits already on the track 
  */
  void findMatchingHits( PrUTHits& MatchingXHits, PrDownTrack& track, int plane ) ;

  /**
  * @brief Update Downstream track to full LHCb::Track and push to output container
  *
  * @param track Downstream track
  * @param finalTracks Output container
  * @param tr Initial T-Seed
  */
  void storeTrack( PrDownTrack& track, LHCb::Tracks* finalTracks, LHCb::Track* tr );

  /**
  * @brief Searches matching hits in UV layers based on the track candidate with
  * X hits. Returns all possible 4/3 hit combinations. 
  *
  * @param track Downstream track candidate
  * @param UVtracks Vector of track candidates
  */
  void addUVHits( PrDownTrack& track, std::vector< PrDownTrack >* UVtracks );

  /**
  * @brief Searches matching hits in UV layers based on the track candidate with
  * X hits. All found hits are added to the track candidate. 
  *
  * @param track Downstream track candidate
  */
  void addUVHits( PrDownTrack& track );

  /**
  * @brief Tags hits as used that where already used on a long track 
  *
  * @param tr Longtrack with possible UT hits
  */
  void tagUsedUT( const LHCb::Track* tr );

  /**
  * @brief Fit the X Projection of the Downstream track candidate
  *
  * @param xHits X hits that are supposed to be added to the track
  * @param track Downstream track candidate
  * @param myHit First hit on the track
  */
  void fitXProjection( PrUTHits& xHits, PrDownTrack& track, PrUTHit* myHit );

  /**
  * @brief Checks if final Downstream candidate passes selection criteria (new
  * implementation of acceptCandidate)
  *
  * @param track The Downstream track candidate
  *
  * @return 
  */
  bool legitCandidate( PrDownTrack& track );


  /**
  * @brief Checks if final Downstream candidate passes selection criteria and is
  * better than the previous one
  *
  * @param track The downstream track candidate
  * @param bestTrack The previously best track
  * @param maxPoints Number of measurements of the best canidate 
  * @param minChisq Chisq of the best candidate
  *
  * @return 
  */
  bool acceptCandidate( PrDownTrack& track, PrDownTrack& bestTrack, int& maxPoints, double& minChisq );

  /**
  * @brief Add X hits in overlap regions (if any) and fit again
  *
  * @param track The Downstream track candidate
  */
  void addOverlapRegions( PrDownTrack& track );

private:
  int           m_downTime;
  int           m_buildTime;
  //  int           m_fitTime;
  //  int           m_cleanTime;
  bool          m_printing;
  bool          m_magnetOff;

  std::string   m_inputLocation;
  std::string   m_outputLocation;
  std::string   m_forwardLocation;
  std::string   m_matchLocation;
  double        m_deltaP;
  double        m_xPredTol;
  double        m_xPredTol2;
  double        m_tolX;
  double        m_tolUV;
  double        m_tolMomentum;
  double        m_maxDistance;
  double        m_maxWindow;
  double        m_maxChisq;
  double        m_deltaChisq;
  double        m_minMomentum;
  double        m_minPt;
  bool          m_removeUsed;
  bool          m_removeAll;

  //  bool           m_mode;
  unsigned int  m_tracksToKeep;
  bool          m_removeHits;

  double        m_longChi2;


  double        m_minUTx;
  double        m_minUTy;

  bool          m_legacy;
  std::vector<double> m_zMagnetParams;
  std::vector<double> m_momentumParams;
  std::vector<double> m_yParams;
  double m_errZMag;
  ILHCbMagnetSvc*     m_magFieldSvc;

  double m_stateErrorX2;
  double m_stateErrorY2;
  double m_stateErrorTX2;
  double m_stateErrorTY2;
  double m_stateErrorP;

  bool   m_printTracks;
  bool   m_withDebugTool;

  Tf::UTStationHitManager<PrUTHit> * m_utHitManager;
  IPrDebugUTTool* m_debugTool;
  std::string m_debugToolName;

  double m_zUTa;                  ///< Position of first station
  double m_zUT;                   ///< Average position for track estimate

  PrUTHits m_xHits;

  PrUTHits m_uvHits;

  int   m_seedKey;

  ISequencerTimerTool* m_timerTool;
  bool m_doTiming;
  



};
#endif // PRDOWNSTREAM_H
