// $Id: L0MuonConfNt.h,v 1.1.1.1 2007-05-22 10:20:31 asatta Exp $
#ifndef L0MUONCONFNT_H 
#define L0MUONCONFNT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "HltBase/HltAlgorithm.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/NTuple.h"
#include "PatTools/PatDataStore.h"
#include "HltBase/HltContainers.h" 
#include "MuonDet/IMuonPosTool.h"
#include "Event/MCParticle.h"
#include "Event/L0MuonCandidate.h"


/** @class L0MuonConfNt L0MuonConfNt.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2007-02-16
 */
class L0MuonConfNt : public HltAlgorithm {
public: 
  /// Standard constructor
  L0MuonConfNt( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0MuonConfNt( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  StatusCode L0MuonConfNt::linkMCToL0Muon(LHCb::L0MuonCandidate* muon,LHCb::MCParticle*& pp);


  StatusCode L0MuonConfNt::linkToL0Muon(LHCb::Track* track,LHCb::L0MuonCandidate*& muon);
  double  L0MuonConfNt::calcDLL(LHCb::Track* track,LHCb::L0MuonCandidate* muon);

  double  L0MuonConfNt::calcDLL(LHCb::Track* track,LHCb::Track* muon);

  double  L0MuonConfNt::calcDLLstd(LHCb::Track* track,LHCb::Track* muon);

  StatusCode linkMCToMuon(LHCb::Track* muon,LHCb::MCParticle*& pp);
  StatusCode linkToMuon(LHCb::Track* track,LHCb::Track*& muon);
  
  bool isReconstructable( const LHCb::MCParticle* mcpart );
  

  protected:

private:

  IMuonPosTool*      m_iPosTool;

  PatTrackContainer* m_tracksRZ; 
  Hlt::TrackContainer* m_tracksRZSelected;
  std::string m_inputRZTracksName ;  
  std::string m_inputRZSelectedTracksName ;
  Hlt::TrackContainer* m_tracksT;
  std::string m_inputTTracksName ;
  Hlt::TrackContainer* m_tracksAT;
  std::string m_inputATTracksName ;
  Hlt::TrackContainer* m_tracksSpace;
  std::string m_inputSpaceTracksName ;
  Hlt::TrackContainer* m_tracksSpaceSelected;
  std::string m_inputSpaceSelectedTracksName ;
  PatTrackContainer* m_muonTracksContainer;   
  std::string m_inputMuonTracksName ;


  NTuple::Tuple*                  m_Track;

  NTuple::Item<long>               m_RZtracks;  
  NTuple::Array<float>             m_RZslopeR;   
  NTuple::Array<float>             m_RZslopephi;
  NTuple::Array<float>             m_RZpointR;     
  NTuple::Array<float>             m_RZpointZ;   
  NTuple::Array<long>              m_RZmid; 
  NTuple::Array<float>             m_RZtruep; 
  NTuple::Array<long>              m_RZtruepid;
  //  NTuple::Array<long>              m_RZreco;  
  NTuple::Array<long>              m_RZselected;
  NTuple::Array<long>              m_RZwhoselected;
  

  NTuple::Item<long>               m_tracks;
  NTuple::Array<float>             m_slopeX;  
  NTuple::Array<float>             m_slopeY;    
  NTuple::Array<float>             m_Xpoint;   
  NTuple::Array<float>             m_Ypoint;
  NTuple::Array<float>             m_Zpoint;
  NTuple::Array<float>             m_momentum;
  NTuple::Array<long>              m_pid;
  NTuple::Array<long>              m_mid;
  NTuple::Array<float>             m_truep;
  NTuple::Array<long>              m_truepid;    
  NTuple::Array<long>              m_reco;  
  NTuple::Array<long>              m_RZIndex; 
  NTuple::Array<long>              m_3dselected; 
  NTuple::Array<long>              m_TIndex; 
  NTuple::Array<long>              m_ATIndex; 


  NTuple::Item<long>               m_muonL0;
  NTuple::Array<long>              m_l0muonMatch;
  NTuple::Array<long>              m_l0muonRZMatch;  
  NTuple::Array<float>              m_l0muonZdecay;  
  NTuple::Array<long>              m_l0muonPT;
  NTuple::Array<long>              m_l0muonPTtrue;
  NTuple::Array<long>              m_l0muonPtrue;
  NTuple::Array<long>              m_l0muonPid;
  NTuple::Array<long>              m_l0muonMid;
  NTuple::Array<long>              m_l0muonPid1;
  NTuple::Array<long>              m_l0muonPid2;
  NTuple::Array<long>              m_l0muonPid3;
  NTuple::Array<long>              m_l0muonKey1;
  NTuple::Array<long>              m_l0muonKey2;
  NTuple::Array<long>              m_l0muonKey3;
  NTuple::Array<long>              m_l0muonMid1;
  NTuple::Array<long>              m_l0muonMid2;
  NTuple::Array<long>              m_l0muonMid3;
  NTuple::Array<float>             m_l0muonx1;
  NTuple::Array<float>             m_l0muonx2;
  NTuple::Array<float>              m_l0muonx3;
  //  NTuple::Array<float>              m_l0muonx4;
  //  NTuple::Array<float>              m_l0muonx5;
  NTuple::Array<float>              m_l0muony1;
  NTuple::Array<float>              m_l0muony2;
  NTuple::Array<float>              m_l0muony3;
  //  NTuple::Array<float>              m_l0muony4;
  //  NTuple::Array<float>              m_l0muony5;
  NTuple::Array<long>               m_l0muonr1;
  NTuple::Array<long>               m_l0muonr2;
  NTuple::Array<long>               m_l0muonr3;
  //  NTuple::Array<long>               m_l0muonr4;
  //  NTuple::Array<long>               m_l0muonr5;

  NTuple::Item<long>               m_Ttracks;  
  NTuple::Array<float>             m_TslopeX;  
  NTuple::Array<float>             m_TslopeY;  
  NTuple::Array<float>             m_TXpoint;  
  NTuple::Array<float>             m_TYpoint;
  NTuple::Array<float>             m_TZpoint;
  NTuple::Array<float>             m_Tmomentum;
  NTuple::Array<float>             m_Tpt; 
  NTuple::Array<long>              m_Tpid;
  NTuple::Array<long>              m_Tmid;
  NTuple::Array<float>             m_Tchi2;
  NTuple::Array<float>             m_Ttruep;
  NTuple::Array<float>             m_Tdll;

  //NTuple::Array<long>              m_truepid;   
  //NTuple::Array<long>              m_reco;
  NTuple::Array<long>              m_TRZMatch;   
  NTuple::Array<long>              m_TL0Index;   


  NTuple::Item<long>               m_ATtracks;
  NTuple::Array<float>             m_ATslopeX;
  NTuple::Array<float>             m_ATslopeY;
  NTuple::Array<float>             m_ATXpoint;
  NTuple::Array<float>             m_ATYpoint;
  NTuple::Array<float>             m_ATZpoint;
  NTuple::Array<float>             m_ATmomentum;
  NTuple::Array<float>             m_ATpt;
  NTuple::Array<long>              m_ATpid;
  NTuple::Array<long>              m_ATmid;
  NTuple::Array<float>             m_ATchi2;
  NTuple::Array<float>             m_ATtruep;
  NTuple::Array<float>             m_ATdll;
  NTuple::Array<float>             m_ATdllstd;

  //NTuple::Array<long>              m_truepid;
  //NTuple::Array<long>              m_reco;
  NTuple::Array<long>              m_ATRZMatch;
  NTuple::Array<long>              m_ATL0Index;




  NTuple::Item<long>                m_muonTracks;
  NTuple::Array<long>               m_muonPid;
  NTuple::Array<long>               m_muonMid;
  NTuple::Array<long>               m_muonMatch;
  NTuple::Array<long>               m_muonRZMatch;
  NTuple::Array<float>              m_muonSlopeX;
  NTuple::Array<float>              m_muonSlopeY;
  NTuple::Array<float>              m_muonXpoint;
  NTuple::Array<float>              m_muonYpoint;
  NTuple::Array<float>              m_muonTrueP;
  NTuple::Array<long>               m_muonSkip;
  // NTuple::Array<float>              m_muonDecayPoint;





  NTuple::Array<long>              m_selected;
  NTuple::Array<long>              m_whoselected;
  NTuple::Array<float>              m_impact;   



 
};
#endif // L0MUONCONFNT_H
