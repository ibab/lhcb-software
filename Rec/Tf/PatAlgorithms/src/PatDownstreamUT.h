// $Id: PatDownstream.h,v 1.8 2009-12-04 17:42:18 sstahl Exp $
#ifndef PATDOWNSTREAMUT_H 
#define PATDOWNSTREAMUT_H 1

// Include files
#include <string>
#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Kernel/ILHCbMagnetSvc.h"
#include "GaudiAlg/ISequencerTimerTool.h"

#include "TfKernel/UTStationHitManager.h"
#include "PatKernel/PatTTHit.h"

// forward declarations
class IPatDebugUTTool;
class DeSTDetector;
class PatDownTrack;

namespace LHCb {
  class Track;
}

/** @class PatDownstream PatDownstream.h
 *  Algorithm to reconstruct tracks with seed and UT, for KShort use.
 *  Based on Pat/PatKShort
 *
 *  @author Olivier Callot
 *  @date   2007-10-18
 *
 *  @author Sascha Stahl
 *  @date   2009-10-07
 */
class PatDownstreamUT : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PatDownstreamUT( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PatDownstreamUT( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  void utCoordCleanup();  ///< tag already used coordinates

  void prepareSeeds(LHCb::Tracks* inTracks, std::vector<LHCb::Track*>& myInTracks); // tag already used T-Seeds
    
  void getPreSelection( PatDownTrack& track );
    
  void fitAndRemove( PatDownTrack& track );
  
  void findMatchingHits( PatTTHits& MatchingXHits, PatDownTrack& track, int plane ) ;

  void storeTrack( PatDownTrack& track, LHCb::Tracks* finalTracks, LHCb::Track* tr );
  
  void addUVHits( PatDownTrack& track );

  void tagUsedUT( const LHCb::Track* tr );

  void fitXProjection( PatTTHits& xHits, PatDownTrack& track, PatTTHit* myHit );
  
  bool acceptCandidate( PatDownTrack& track, PatDownTrack& bestTrack, int& maxPoints, double& minChisq );

  void addOverlapRegions( PatDownTrack& track );
    
  
private:
  int           m_downTime;
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
  double        m_minMomentum;
  double        m_minPt;
  bool          m_removeUsed;
  bool          m_removeAll;
    
  double        m_longChi2;
    

  double        m_minUTx;
  double        m_minUTy;

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

  Tf::UTStationHitManager<PatTTHit> * m_utHitManager;
  IPatDebugUTTool* m_debugTool;
  std::string m_debugToolName;

  DeSTDetector*       m_utDet;    ///< UT Detector element
  double m_zUTa;                  ///< Position of first station
  double m_zUT;                   ///< Average position for track estimate

  PatTTHits m_xHits;
 
  PatTTHits m_uvHits;
    
  int   m_seedKey;

  ISequencerTimerTool* m_timerTool;
  bool m_doTiming; 
};
#endif // PATDOWNSTREAMUT_H
