// $Id: AlignMuonRec.h,v 1.4 2008-07-17 12:50:32 smenzeme Exp $
#ifndef ALIGNMUONREC_H 
#define ALIGNMUONREC_H 1

// Include files
// from Gaudi
//#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiAlg/ISequencerTimerTool.h"
//#include "GaudiAlg/GaudiTupleAlg.h" //add sim 09.02.07

#include "MuonDet/IMuonPosTool.h"
#include "MuonDet/MuonBasicGeometry.h"
#include "MuonDet/DeMuonRegion.h"
#include "MuonDet/DeMuonDetector.h"
#include "Kernel/IMuonLayout.h"
#include "Event/Track.h"

//#include "PatTools/PatDataStore.h"
                              
#include "AlignMuonStationRec.h"
#include "AlignMuonTrack.h"
#include "MuonDAQ/IMuonRawBuffer.h"
//tools
#include "TrackInterfaces/ITrackMomentumEstimate.h"

//#include "HltBase/HltAlgorithm.h"
//#include "HltBase/HltFunctions.h"

// from AIDA Histogramming  //MB
#include "AIDA/IHistogram1D.h"  //MB
#include "AIDA/IHistogram2D.h"  //MB


/** @class AlignMuonRec AlignMuonRec.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2004-10-06
 */
//class AlignMuonRec : public GaudiAlgorithm
class AlignMuonRec : public GaudiHistoAlg {
  //class AlignMuonRec : public GaudiAlgorithm {
public: 
  /// Standard constructor
  AlignMuonRec( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~AlignMuonRec( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  StatusCode decodeBuffer();
  
//  unsigned int readoutType(int partition, MuonLayout lay);
  LHCb::MuonTileID intercept(const LHCb::MuonTileID& stripX, 
                              const LHCb::MuonTileID& stripY) ;
  StatusCode muonSearch();
  StatusCode detectClone();    
  StatusCode strongCloneKiller();

  void insertMatch(int muon,LHCb::Track* track,float distance);
	StatusCode printOut();  

StatusCode createCoords();
protected:
private:
//  IMuonTileXYZTool  *m_iTileTool;
//  IMuonGeometryTool *m_iGeomTool;  
//  IMuonGetInfoTool  *m_iGetInfo;  
  IMuonRawBuffer* m_muonBuffer;
  ITrackMomentumEstimate* m_fCalcMomentum; // momentum tool  // Silvia July 07

  DeMuonDetector* m_muonDetector;
  //  IMuonPosTool      *m_iPosTool;
  //ITrackSelector* m_trackSelector; // tool to accept a track

  bool               m_onTES; ///< Should create States ?
  // roba di PatDataStore inizializzata ma non usata, provo a comm
  //  PatTrackContainer* m_muonTracksContainer;
  //  PatStateContainer* m_states;

  unsigned int m_nStation;
  unsigned int m_nRegion;

  ISequencerTimerTool* m_timer;
  int m_timeLoad;
  int m_timePad;
  int m_timeMuon;              
  int m_timeTag;
  //  int m_timeAsso;
  int m_timeMuonStore;
  std::vector<AlignMuonStationRec> m_station;
  std::vector<std::pair<LHCb::MuonTileID,bool> > twelfthX[12];
  std::vector<std::pair<LHCb::MuonTileID,bool> > twelfthY[12];
  std::vector<AlignMuonTrack> m_muonTracks;
  unsigned int m_maxMuonFound;  
  bool m_measureTime;  
  bool m_cloneKiller;
  bool m_Bfield;
  bool m_m1Station;
  double m_matchChisq;


//  bool m_storeTracks;
  bool m_padM5;
  bool m_padM4;
  bool m_padM3;
  bool m_padM2;
  bool m_decodingFromCoord;
  std::string m_outputMuonTracksName;
  std::string m_outputMuonTracksForAlignmentName;
  std::string m_extrapolatorName;
  bool m_Histo;
  ITrackExtrapolator* m_extrapolator; ///< extrapolator

  double PADsizeX[4];
  double PADsizeY[4];  
  
  // counters
  int m_countEvents;
  int m_countMuCandidates;
  
  AIDA::IHistogram1D *m_Chi2match, *m_tx, *m_ty, *m_p, *m_states, *m_nMuonTrack, *m_nMuonTrackNoClone,
    *m_nMuonTrackMatch;
  AIDA::IHistogram2D *m_Mfirst,*m_hitM;
  AIDA::IHistogram2D *m_resx[5][4], *m_resy[5][4];
  //AIDA::IHistogram2D *m_resx[5];
};
#endif // ALIGNMUONREC_H
