// $Id: PatDownstream.h,v 1.1 2007-10-22 15:50:07 ocallot Exp $
#ifndef PATDOWNSTREAM_H 
#define PATDOWNSTREAM_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// From STDet 
#include "STDet/DeSTDetector.h"

#include "TfKernel/TTStationHitManager.h"
#include "PatKernel/PatTTHit.h"
#include "TrackMCInterfaces/IPatDebugTTTruthTool.h"

/** @class PatDownstream PatDownstream.h
 *  Algorithm to reconstruct tracks with seed and TT, for KShort use.
 *  Based on Pat/PatKShort
 *
 *  @author Olivier Callot
 *  @date   2007-10-18
 */
class PatDownstream : public GaudiAlgorithm {
public: 
  /// Standard constructor
  PatDownstream( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PatDownstream( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  void ttCoordCleanup();  ///< tag already used coordinates

   void fitAndRemove ( PatDownTrack& track );
  
  void findMatchingHits( PatDownTrack& track, int plane, double tol ) ;

  void addUVHits( PatDownTrack& track );

private:
  std::string   m_inputLocation;
  std::string   m_outputLocation;
  double        m_deltaP;
  double        m_xPredTol;
  double        m_tolMatch;
  double        m_tolUV;
  double        m_maxDistance;
  double        m_maxChisq;
  bool          m_clusterFilter;
  bool          m_seedFilter;  
  double        m_minTTx;
  double        m_minTTy;

  std::vector<double> m_zMagnetParams;
  std::vector<double> m_momentumParams;
  std::vector<double> m_yParams;
  double m_errZMag;
  
  double m_stateErrorX2;
  double m_stateErrorY2;
  double m_stateErrorTX2;
  double m_stateErrorTY2;
  double m_stateErrorP;

  bool   m_useForward;
  bool   m_printTracks;
  bool   m_withTruth;

  Tf::TTStationHitManager<PatTTHit> * m_ttHitManager;
  IPatDebugTTTruthTool* m_truthTool;

  DeSTDetector*       m_ttDet;    ///< TT Detector element
  double m_zTTa;                  ///< Position of first station
  double m_zTT;                   ///< Average position for track estimate

  PatTTHits m_xHits;
  PatTTHits m_uvHits;

  int   m_seedKey;
  bool  m_printing;  
};
#endif // PATKSHORT_H
