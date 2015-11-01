// $Id: $
// Include files
#include <cstdio>

#include "GaudiKernel/DeclareFactoryEntries.h"
#include "GaudiAlg/ISequencerTimerTool.h"
#include "Kernel/DaVinciHistoAlgorithm.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "MuonDet/DeMuonDetector.h"
#include "MuonDet/IMuonFastPosTool.h"
#include "MuonDet/MuonBasicGeometry.h"
#include "Kernel/ITriggerTisTos.h"
#include "TrackInterfaces/ITrackExtrapolator.h"

#include "MuEffMonitor.h"

using namespace LHCb;
using namespace Gaudi;
using namespace Gaudi::Utils;
using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : MuEffMonitor
//
// 2010-6-18 : Patrizia de Simone
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuEffMonitor )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuEffMonitor::MuEffMonitor( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : DaVinciHistoAlgorithm ( name , pSvcLocator ),
    m_Offline(false) {

  // M1
  m_nSigma1X.resize(4);
  m_nSigma1Y.resize(4);
  // default values for nSigma cuts per M1 regions 
  m_nSigma1X[0] = 8;
  m_nSigma1X[1] = 8;
  m_nSigma1X[2] = 8;
  m_nSigma1X[3] = 8;

  m_nSigma1Y[0] = 8;
  m_nSigma1Y[1] = 8;
  m_nSigma1Y[2] = 8;
  m_nSigma1Y[3] = 8;

  // M2 - M5
  m_nSigmaX.resize(4);
  m_nSigmaY.resize(4);
  // default values for nSigma cuts per Stations 
  m_nSigmaX[0] = 8;
  m_nSigmaX[1] = 8;
  m_nSigmaX[2] = 8;
  m_nSigmaX[3] = 8;
  
  m_nSigmaY[0] = 8;
  m_nSigmaY[1] = 8;
  m_nSigmaY[2] = 8;
  m_nSigmaY[3] = 8;

  declareProperty ( "MomentumCut"   ,m_MomentumCut =    3000. ) ; //MeV
  declareProperty ( "MomentumCutM4" ,m_MomentumCutM4 =  9000. ) ;
  declareProperty ( "MomentumCutM5" ,m_MomentumCutM5 = 10000. ) ;
  declareProperty ( "UseCalo"       ,m_useCalo = false );
  declareProperty ( "EecalMax"      ,m_EecalMax = 1000. ) ;
  declareProperty ( "EecalMin"      ,m_EecalMin = -100. ) ;
  declareProperty ( "EhcalMax"      ,m_EhcalMax = 3500. ) ;
  declareProperty ( "EhcalMin"      ,m_EhcalMin = 1000. ) ;

  declareProperty ( "doTrigger"     ,m_doTrigger = false ) ;
  declareProperty ( "TagUnBias"     ,m_TagUnBias = false ) ;
  
  declareProperty ( "TrminDof"      ,m_TrminDof = 0);
  declareProperty ( "nSigma1X"      ,m_nSigma1X ); // matching window in sigma (very loose)
  declareProperty ( "nSigma1Y"      ,m_nSigma1Y );
  declareProperty ( "nSigmaX"       ,m_nSigmaX ); // matching window in sigma (very loose)
  declareProperty ( "nSigmaY"       ,m_nSigmaY );
  declareProperty ( "nSigmaFidVol"  ,m_nSigmaFidVol = 3.);
  declareProperty ( "Chi2ProbTrMin" ,m_Chi2TrMin = 0.01) ;  // chi2 cut on tracker tracks
  declareProperty ( "Chi2MuMin"     ,m_Chi2MuMin = 10) ; // chi2 cut global track to mu matching (very loose)
  declareProperty ( "nSigmaXother"  ,m_nSigmaXother = 2); // matching window in sigma for other stations (tight)
  declareProperty ( "nSigmaYother"  ,m_nSigmaYother = 2); // matching window in sigma for other stations (tight)
  declareProperty ( "Chi2OtherMuMin",m_Chi2OtherMuMin = 99) ; // or chi2 cut global track to mu matching other stations 
  declareProperty ( "Chi2MuGlobal"  ,m_Chi2MuGlobal = 99) ; // max chi2/ndf for global candidates 
  declareProperty ( "CosThetaCut"   ,m_CosThetaCut = 0.99 ) ;
  declareProperty ( "xyDistCut"     ,m_xyDistCut = 5. ) ; // cm
  declareProperty ( "RequireCrossing", m_mustCross = true );
  declareProperty ( "RequiredStations", m_nReqStations = 4);
  declareProperty ( "ignoreM1"      ,m_ignoreM1 = false);
  declareProperty ( "Extrapolator"  ,m_extrapolatorName = "TrackMasterExtrapolator" ) ;
  declareProperty ( "HistoLevel"    ,m_histoLevel = "Online" );
  declareProperty ( "MeasureTime"   ,m_measureTime = false);
  declareProperty ( "PosToolName"   ,m_posToolName = "MuonFastPosTool");
}

//===========================
// Destructor
//===========================
MuEffMonitor::~MuEffMonitor() { } 

//===========================
// Initialization
//===========================
StatusCode MuEffMonitor::initialize() {
  
  StatusCode sc = DaVinciHistoAlgorithm::initialize();
  if ( sc.isFailure() ) { return sc; }
  debug()   << " MuEffMonitor  ==> Initialize " << endmsg;

  GaudiAlg::HistoID  name;
  m_Offline = (m_histoLevel == "Offline");
  
  // Load Muon Detector geometry:
  sc = LoadMuonGeometry();
  if ( sc.isFailure() ) { return sc; }

  if (m_Offline) {
    m_trExtErrX = book1D("m_trExtErrX","Error on X track extrapolation to M2",0.,50.,50);
    m_trExtErrY = book1D("m_trExtErrY","Error on Y track extrapolation to M2",0.,50.,50);
    m_trAccuracy = book2D("m_trAccuracy","track Ndof vs accuracy on M2", 0., 70., 50, -0.5, 49.5, 25);
    m_Angolo   = book1D("m_Angolo","Cos opening angle",0.8, 1.1, 32);
    m_Distanza = book1D("m_Distanza","Distanza ",0., 800., 250);
    m_DeltaP   = book1D("m_DeltaP","DeltaP",-1., 1., 100);
    m_DistanzaX = book1D("m_DistanzaX","Distanza X",0., 800., 250);
    m_DistanzaY = book1D("m_DistanzaY","Distanza Y",0., 800., 250);
    m_AngoloS   = book1D("m_AngoloS","Cos opening angle",0.8, 1.1, 32);
    m_DistanzaS = book1D("m_DistanzaS","Distanza ",0., 800., 250);
    m_DeltaPS   = book1D("m_DeltaPS","DeltaP",-1., 1., 100);
    m_DistanzaXS = book1D("m_DistanzaXS","Distanza X",0., 800., 250);
    m_DistanzaYS = book1D("m_DistanzaYS","Distanza Y",0., 800., 250);

    if (m_doTrigger) 
    {
      m_PpreTis = book1D("m_PpreTis","P selected tracks before TIS cut",0.0,150.0,75);
    }
   
    // fillPreHistos 
    m_PPreS  = book1D("m_PPreS","P pre selected tracks",0.0, 150.0, 75);
    m_PPPreS  = book1D("m_PPPreS","P positive pre selected tracks",0.0, 150.0, 75);
    m_PNPreS  = book1D("m_PNPreS","P negative pre selected tracks",0.0, 150.0, 75);

    m_PPreSMIP  = book1D("m_PPreSMIP","P pre selected tracks (no MIP cut)",0.0, 150.0, 75);
    m_PPPreSMIP  = book1D("m_PPPreSMIP","P positive pre selected tracks (no MIP cut)",0.0, 150.0, 75);
    m_PNPreSMIP  = book1D("m_PNPreSMIP","P negative pre selected tracks (no MIP cut)",0.0, 150.0, 75);
    // end fillPreHistos

    if ( m_TagUnBias ) {
      m_PpreTag = book1D("m_PpreTag","P selected tracks before Tag cut",0.0,150.0,75);
      m_PPTag = book1D("m_PPTag","P positive selected Tag Muons",0.0,150.0,75);
      m_PNTag = book1D("m_PNTag","N positive selected Tag Muons",0.0,150.0,75);

      m_PPTagCheck = book1D("m_PPTagCheck","P positive selected Tag Muons",0.0,150.0,75);
      m_PNTagCheck = book1D("m_PNTagCheck","N positive selected Tag Muons",0.0,150.0,75);
      m_PPTagCheckS = book1D("m_PPTagCheckS","P positive selected Tag Muons",0.0,150.0,75);
      m_PNTagCheckS = book1D("m_PNTagCheckS","N positive selected Tag Muons",0.0,150.0,75);
    }

    char name[500];
    char title[500];

    for (int s=0; s<m_NStation; s++) {

      sprintf(name,"m_PPre_S%d",s+1);
      sprintf(title,"P Pre Sample%d selected tracks",s+1);
      m_PPre[s] = book1D(name, title,2., 102., 20);
      sprintf(name,"m_PPreMIP_S%d",s+1);
      sprintf(title,"P Pre Sample%d selected tracks (no MIP cut)",s+1);
      m_PPreMIP[s] = book1D(name, title,2., 102., 20);

      sprintf(name,"m_P_S%d",s+1);
      sprintf(title,"P Sample%d selected tracks",s+1);
      m_P[s]    = book1D(name, title,2., 102., 20);
      sprintf(name,"m_P_S%dhit",s+1);      
      m_P_hit[s] = book1D(name, title,2., 102., 20);
      sprintf(name,"m_PP_S%d",s+1);
      m_PP[s]    = book1D(name, title,2., 102., 20);
      sprintf(name,"m_PP_S%dhit",s+1);      
      m_PP_hit[s] = book1D(name, title,2., 102., 20);
      sprintf(name,"m_PN_S%d",s+1);      
      m_PN[s] = book1D(name, title,2., 102., 20);
      sprintf(name,"m_PN_S%dhit",s+1);      
      m_PN_hit[s] = book1D(name, title,2., 102., 20);

      for (int r=0; r<4; r++) {

        double maxX= m_stationOuterX[s] / pow(2.,3-r);
        double maxY= m_stationOuterY[s] / pow(2.,3-r);
      
        sprintf(name,"m_M%dR%d_TrkP", s+1, r+1);
        sprintf(title, "M%dR%d #muon+ ",s+1, r+1);
        m_TrkP[s][r] = book2D(name, title, -maxX, maxX, 16, -maxY, maxY, 16);

        sprintf(name,"m_M%dR%d_TrkN", s+1, r+1);
        sprintf(title, "M%dR%d #muon- ",s+1, r+1);
        m_TrkN[s][r] = book2D(name, title, -maxX, maxX, 16, -maxY, maxY, 16);

        sprintf(name,"m_M%dR%d_TrkPMIP", s+1, r+1);
        sprintf(title, "M%dR%d #muon+ (no MIP cut) ",s+1, r+1);
        m_TrkPMIP[s][r] = book2D(name, title, -maxX, maxX, 16, -maxY, maxY, 16);

        sprintf(name,"m_M%dR%d_TrkNMIP", s+1, r+1);
        sprintf(title, "M%dR%d #muon- (no MIP cut) ",s+1, r+1);
        m_TrkNMIP[s][r] = book2D(name, title, -maxX, maxX, 16, -maxY, maxY, 16);

        sprintf(name,"m_M%dR%d_numP", s+1, r+1);
        sprintf(title, "M%dR%d #muon+ with hit",s+1, r+1);
        m_hit_numP[s][r] = book2D(name, title, -maxX, maxX, 16, -maxY, maxY, 16);

        sprintf(name,"m_M%dR%d_numN", s+1, r+1);
        sprintf(title, "M%dR%d #muon- with hit",s+1, r+1);
        m_hit_numN[s][r] = book2D(name, title, -maxX, maxX, 16, -maxY, maxY, 16);
        
        sprintf(name,"m_M%dR%d_denP", s+1, r+1);
        sprintf(title, "M%dR%d #muon+ all tracks",s+1, r+1);
        m_hit_denP[s][r] = book2D(name, title, -maxX, maxX, 16, -maxY, maxY, 16);
        
        sprintf(name,"m_M%dR%d_denN", s+1, r+1);
        sprintf(title, "M%dR%d #muon- all tracks",s+1, r+1);
        m_hit_denN[s][r] = book2D(name, title, -maxX, maxX, 16, -maxY, maxY, 16);   
    
        sprintf(name,"m_M%dR%d_hitsInW", s+1, r+1);
        sprintf(title, "M%dR%d #hits all tracks",s+1, r+1);
        m_hitsInW[s][r] = book2D(name, title, -maxX, maxX, 16, -maxY, maxY, 16);

        sprintf(name,"m_M%dR%d_nWinX", s+1, r+1);
        sprintf(title, "M%dR%d # windows X",s+1, r+1);
        m_nDx[s][r] = book1D(name, title, -14., 14., 28);

        sprintf(name,"m_M%dR%d_nWinY", s+1, r+1);
        sprintf(title, "M%dR%d # windows Y",s+1, r+1);
        m_nDy[s][r] = book1D(name, title, -14., 14., 28);

        sprintf(name,"m_M%dR%d_OccuMu", s+1, r+1);
        sprintf(title, "M%dR%d #mu hits",s+1, r+1);
        m_OccuMu[s][r] = book1D(name, title, -0.5, 249.5, 250);


        sprintf(name,"m_Chi2Hits_$M%dR%d", s+1, r+1);
        sprintf(title, "#chi^2/dof from other muon hits M%dR%d",s+1, r+1);
        m_Chi2OtherHits[s][r] = book1D(name, title, 0., 25., 100);

        sprintf(name,"m_M%dR%d_OccuMu", s+1, r+1);
        sprintf(title, "M%dR%d #mu hits",s+1, r+1);
        m_OccuMu[s][r] = book1D(name, title, -0.5, 249.5, 250);

        sprintf(name,"m_M%dR%d_OccuBck", s+1, r+1);
        sprintf(title, "M%dR%d #bck hits",s+1, r+1);
        m_OccuBck[s][r] = book1D(name, title, -0.5, 99.5, 100);   

        int nch = m_regionNch[r];

        sprintf(name,"m_chamberTrk_$M%dR%d", s+1, r+1);
        sprintf(title, "tracks per chamber M%dR%d",s+1, r+1);
        m_chamberTrk[s][r] = book1D(name, title, -0.5, nch-0.5, nch);

        sprintf(name,"m_chamberTrkMIP_$M%dR%d", s+1, r+1);
        sprintf(title, "tracks per chamber M%dR%d (no MIP cut)",s+1, r+1);
        m_chamberTrkMIP[s][r] = book1D(name, title, -0.5, nch-0.5, nch);

        sprintf(name,"m_chamberEffMIP_den_$M%dR%d", s+1, r+1);
        sprintf(title, "tracks per chamber M%dR%d (no MIP cut)",s+1, r+1);
        m_chamberEffMIP_den[s][r] = book1D(name, title, -0.5, nch-0.5, nch);
        
        sprintf(name,"m_chamberEffMIP_num_$M%dR%d", s+1, r+1);
        sprintf(title, "tracks per chamber with hit M%dR%d (no MIP cut)",s+1, r+1);
        m_chamberEffMIP_num[s][r] = book1D(name, title, -0.5, nch-0.5, nch); 

      } 
    }
  }

  //Online Monitoring

  m_nEvts   = book1D("m_nEvts","number of processed evts",0.5, 2.5, 2 );
  m_nTracks = book1D("m_nTracks","number of tracks after selection steps",-0.5, 16.5, 17 );
  m_trNdof = book1D("m_trNdof","number of track dof", -0.5,49.5,50);
  m_nMuPreS = book1D("m_nMuPreS","number of MuCan PreSel per event",-0.5, 99.5, 100 );
  m_TrkType = book1D("m_TrkType","Track type",-0.5, 8.5, 9);
  m_Chi2Hits = book1D("m_Chi2Hits","Chi2 from muon hits matching",0.0, 25.0, 100);
  m_PpreSel  = book1D("m_PpreSel","P pre selected tracks",0.0, 60.0, 60);
  m_PSel  = book1D("m_PSel","P selected tracks",0.0,150.0,75);
  m_PPSel  = book1D("m_PPSel","P positive selected tracks",0.0,150.0,75);
  m_PNSel  = book1D("m_PNSel","P negative selected tracks",0.0,150.0,75);
  m_nMuSel = book1D("m_nMuSel","number of MuCan Sel per event",-0.5, 9.5, 10 );
  
  m_Hcal = book1D("m_Hcal","Hcal distribution",10., 6000., 60); 
  m_Ecal = book1D("m_Ecal","Ecal distribution",10., 2000., 40);
  m_ene  = book2D("m_ene","Hcal vs Ecal",0.0, 10000., 100, 0.0, 2000., 20);  
  m_SHcal = book1D("m_SHcal","Hcal distribution",10., 6000., 60); 
  m_SEcal = book1D("m_SEcal","Ecal distribution",10., 2000., 40);
  m_Sene  = book2D("m_Sene","Hcal vs Ecal",0.0, 10000., 100, 0.0, 2000., 20);

  char namech[500];
  char titlech[500];

  for (int s=0; s<m_NStation; s++) {
    for (int r=0; r<4; r++) {

      int nch = m_regionNch[r];

      sprintf(namech,"m_chamberEff_den_$M%dR%d", s+1, r+1);
      sprintf(titlech, "tracks per chamber M%dR%d",s+1, r+1);
      m_chamberEff_den[s][r] = book1D(namech, titlech, -0.5, nch-0.5, nch);
        
      sprintf(namech,"m_chamberEff_num_$M%dR%d", s+1, r+1);
      sprintf(titlech, "tracks per chamber with hit M%dR%d",s+1, r+1);
      m_chamberEff_num[s][r] = book1D(namech, titlech, -0.5, nch-0.5, nch);

    } 
  }

  m_StatEffNoMIP_den = book1D("m_StatEffNoMIP_den","selected tracks (no MIP cut)",-0.5,5.5,6); 
  m_StatEffNoMIP_num = book1D("m_StatEffNoMIP_num","selected tracks (no MIP cut) with hits ",-0.5,5.5,6);  

  m_StationsEff_den = book1D("m_StationsEff_den","selected tracks",-0.5,5.5,6); 
  m_StationsEff_num = book1D("m_StationsEff_num","selected tracks with hits ",-0.5,5.5,6); 
  m_StationsEff_denP = book1D("m_StationsEff_denP","selected + tracks",-0.5,5.5,6); 
  m_StationsEff_numP = book1D("m_StationsEff_numP","selected + tracks with hits ",-0.5,5.5,6); 
  m_StationsEff_denN = book1D("m_StationsEff_denN","selected - tracks",-0.5,5.5,6); 
  m_StationsEff_numN = book1D("m_StationsEff_numN","selected - tracks with hits ",-0.5,5.5,6); 
  

  m_RegEffNoMIP_den = book1D("m_RegEffNoMIP_den","selected tracks (no MIP cut)",-0.5,20.5,21); 
  m_RegEffNoMIP_num = book1D("m_RegEffNoMIP_num","selected tracks (no MIP cut) with hits ",-0.5,20.5,21);

  m_RegionsEff_den = book1D("m_RegionsEff_den","selected tracks",-0.5,20.5,21); 
  m_RegionsEff_num = book1D("m_RegionsEff_num","selected tracks with hits ",-0.5,20.5,21);
  m_RegionsEff_denP = book1D("m_RegionsEff_denP","selected + tracks",-0.5,20.5,21); 
  m_RegionsEff_numP = book1D("m_RegionsEff_numP","selected + tracks with hits ",-0.5,20.5,21);
  m_RegionsEff_denN = book1D("m_RegionsEff_denN","selected - tracks",-0.5,20.5,21); 
  m_RegionsEff_numN = book1D("m_RegionsEff_numN","selected - tracks with hits ",-0.5,20.5,21);

  // TisTos Trigger Tool
  if (m_doTrigger) 
  {  
    m_L0TriggerTisTosTool = tool<ITriggerTisTos>( "L0TriggerTisTos",this );
    m_TriggerTisTosTool = tool<ITriggerTisTos>( "TriggerTisTos",this );
  }
  
  // Tracks extrapolator Tool:
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorName, "MuEffExtrap",this );
  if (!m_extrapolator){
    err()<<" error retrieving the Extrapolator Tool"<<endmsg;
    return StatusCode::FAILURE;
  } 
  
  m_posTool = tool<IMuonFastPosTool>(m_posToolName);
  if(!m_posTool) {
    error()<<"error retrieving the muon position tool "<<endmsg;
    return StatusCode::FAILURE;
  }

  m_rsvc = svc<IRndmGenSvc>( "RndmGenSvc", true );

  // timing
  if(m_measureTime) {
    m_timer = tool<ISequencerTimerTool>("SequencerTimerTool","TIMER");
    m_timer->increaseIndent();
    m_timeExe = m_timer->addTimer( "Execution" );
    m_timePrl = m_timer->addTimer( "Preliminaries" );
    m_timeFiC = m_timer->addTimer( "SaveCoords" );
    m_timePrt = m_timer->addTimer( "FindProto" );
    m_timePre = m_timer->addTimer( "Preselection" );
    m_timeSel = m_timer->addTimer( "Selection" );
    m_timeCal = m_timer->addTimer( "Calo" );
    m_timeIso = m_timer->addTimer( "Isolation" );
    m_timeExr = m_timer->addTimer( "Extrapolation" );
    m_timeMuM = m_timer->addTimer( "Muon Matching" );
    m_timeTis = m_timer->addTimer( "TisTossing" );
    m_timeFil = m_timer->addTimer( "Filling" );
    m_timeFnl = m_timer->addTimer( "Conclusion" );
    m_timer->decreaseIndent();
  }

  m_L0LinesNames = "L0Global";
  m_Hlt1LinesNames = "Hlt1Global";
  m_Hlt2LinesNames = "Hlt2Global";

  if (m_nReqStations >= m_NStation) // absurd request
    m_nReqStations = m_NStation-1;
  m_origReqStations = m_nReqStations; //  keep memory of requested number of stations

  return StatusCode::SUCCESS;
}

//=============================
// Main execution
//=============================
StatusCode MuEffMonitor::execute() {

  if(m_measureTime) m_timer->start(m_timeExe);
  debug() << " ==> Execute" << endmsg;

  if(m_measureTime) m_timer->start(m_timePrl);
  StatusCode sc = StatusCode::SUCCESS ;
  setFilterPassed(false);
  
  m_nEvts->fill(1.);
  if(m_measureTime) m_timer->stop(m_timePrl);

  //----------------------------
  // Fill Hits coord positions
  //---------------------------
  if(m_measureTime) m_timer->start(m_timeFiC);
  sc = fillCoordVectors();
  if( sc.isFailure() ) return StatusCode::SUCCESS;
  if(m_measureTime) m_timer->stop(m_timeFiC);
    
  //--------------
  //== GET Tracks
  //--------------
  if(m_measureTime) m_timer->start(m_timePrt);
 
  const LHCb::ProtoParticle::Container* protos;
  if ( m_TagUnBias ) {
    protos = get<LHCb::ProtoParticle::Container>( "/Event/PID/Rec/ProtoP/Charged" );
  }
  else if ( !m_TagUnBias ) {
    protos = get<LHCb::ProtoParticle::Container>( LHCb::ProtoParticleLocation::Charged );
  }

  if (!protos){
    error() << " ==> Failed to get ProtoParticle container " << endmsg;
    return StatusCode::FAILURE;
  }

  debug() << " ==> number of charged ProtoParticles" << protos->size() << endmsg;
  if(m_measureTime) m_timer->stop(m_timePrt);

  // ----------------------------------------------
  // BEGIN Track loop
  // ----------------------------------------------

  // Preselection
  if(m_measureTime) m_timer->start(m_timePre);
  m_nTrk = 0;
  m_preCandidate.clear();
  m_preCandState.clear();

  const LHCb::State *muState;
  
  // Do Track Preselection 
  for (LHCb::ProtoParticle::Container::const_iterator iproto = protos->begin() ; protos->end() != iproto ; ++iproto )  {

    const LHCb::ProtoParticle* proto = *iproto ;
    const LHCb::Track *pTrack=proto->track();
    if ( 0 == pTrack ) continue ;

    if (DoTrackPreSelection(pTrack, muState)) {
      m_preCandidate.push_back(proto);
      m_preCandState.push_back(muState);
    }
  }

  // if required look for a Tag_Muon candidate
  m_pTrackTag = 0; 
  if ( m_TagUnBias ) {
    if (!LookForTagMuon()) m_pTrackTag = 0;
  }

  m_nMuPreS -> fill(m_preCandidate.size());
  if(m_preCandidate.size()==0 && m_Offline) sc = LookForBck();  

  //if(m_measureTime) m_timer->stop(m_timePre);

  // Do Track selection
  if(m_measureTime) m_timer->start(m_timeSel);
    
  for (unsigned int it=0; it<m_preCandidate.size(); it++) {
      
    m_Muon.init();
    m_Muon.proto = m_preCandidate[it];
    m_Muon.pTrack = m_preCandidate[it]->track();
    m_Muon.stateP = m_preCandState[it];
    m_Muon.Mom  = (m_Muon.stateP)->p();
    const LHCb::State *stateP0 = &(m_Muon.pTrack->firstState());
    m_Muon.Mom0 = stateP0->p();
      
    bool Lunga  = false;
    if (m_Muon.pTrack->checkType(LHCb::Track::Long) ) Lunga = true;
    if ( Lunga ) m_Muon.TracciaIP = true;

    bool MuonTag = false;
    if ( m_TagUnBias ) 
    { 
      if (m_pTrackTag == m_Muon.pTrack) MuonTag = true;
      m_Muon.ForseTag = MuonTag;
    }

    m_PpreSel -> fill(m_Muon.Mom0 / Gaudi::Units::GeV);
  
    //===== Track selection
    if ( DoTrackSelection() ) {
      // ====== Fetch the values of the track
      m_nTrk++;
      if(m_measureTime) m_timer->start(m_timeFil);
      fillHistos();
      if(m_measureTime) m_timer->stop(m_timeFil);
    }  
  } // END tracks loop
    
  if(m_measureTime) m_timer->stop(m_timeSel);
  if(m_measureTime) m_timer->start(m_timeFnl);

  m_nMuSel->fill(m_nTrk);

  if ((m_nTrk>0) && m_Offline) {
    m_nEvts->fill(2.);
    LookForMuOccu();
  }
  debug()  << " ==> after tracks loop: # Mu Can PreSel "<< m_nTrk << endmsg;
  
  setFilterPassed(true);
  if(m_measureTime) m_timer->stop(m_timeFnl);  
  if(m_measureTime) m_timer->stop(m_timeExe);
  return sc;    
}

//==========================
//  Finalize
//==========================
StatusCode MuEffMonitor::finalize() {

  debug() << "MuEffMonitor ==> Finalize" << endmsg;
  return DaVinciHistoAlgorithm::finalize();
}

//==============================
// Load Muon Detector Geometry:
//==============================
StatusCode MuEffMonitor::LoadMuonGeometry(){

  m_NStation = 0;
  m_NRegion = 0;
  MuonBasicGeometry basegeometry( detSvc(),msgSvc());
  m_NStation= basegeometry.getStations();
  m_NRegion = basegeometry.getRegions();

  int i=0;
  while(i<m_NStation){
    m_stationNames.push_back(basegeometry.getStationName(i));
    i++;
  }

  m_mudet=getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");
  // set the size of the local vectors
  m_padSizeX.resize(m_NStation * m_NRegion);
  m_padSizeY.resize(m_NStation * m_NRegion);
  m_stationZ.resize(m_NStation);
  m_stationInnerX.resize(m_NStation);
  m_stationOuterX.resize(m_NStation);
  m_stationInnerY.resize(m_NStation);
  m_stationOuterY.resize(m_NStation);
  m_regionNch.resize(m_NRegion);
  m_regionNchCol.resize(m_NRegion);

  for(int station = 0 ; station < m_NStation ; station++ ){
    m_stationInnerX[station] = m_mudet->getInnerX(station);
    m_stationOuterX[station] = m_mudet->getOuterX(station);
    m_stationInnerY[station] = m_mudet->getInnerY(station);
    m_stationOuterY[station] = m_mudet->getOuterY(station);
    m_stationZ[station] = m_mudet->getStationZ(station);

    debug() <<"Muon Station "<<station<<" x,y Inner "<<m_stationInnerX[station]<<", "<< m_stationInnerY[station]<<endmsg;
    debug() <<"Muon Station "<<station<<" x,y Outer "<<m_stationOuterX[station]<<", "<< m_stationOuterY[station]<<endmsg;

    for(int region = 0 ; region < m_NRegion ; region++ ){
      m_padSizeX[station * m_NRegion + region]=m_mudet->getPadSizeX(station,region);
      m_padSizeY[station * m_NRegion + region]=m_mudet->getPadSizeY(station,region);
       
      int ind = station*m_NRegion+region;
      debug() <<"Muon Region "<<region<<" x Pad Size "<< ind <<", "<< m_padSizeX[ind]<<endmsg;
      debug() <<"Muon Region "<<region<<" y Pad Size "<< ind <<", "<< m_padSizeY[ind]<<endmsg;
     
      if(m_padSizeX[station * m_NRegion + region]==0){
        error() << " ==> Muon Chamber Pad Size could not be retrieved !!" <<  endmsg;
        return StatusCode::FAILURE;
      }
    }
  }

  m_regionNch[0] =  12;
  m_regionNch[1] =  24;
  m_regionNch[2] =  48;
  m_regionNch[3] = 192;

  m_regionNchCol[0] =  4;
  m_regionNchCol[1] =  4;
  m_regionNchCol[2] =  4;
  m_regionNchCol[3] =  8;

  return StatusCode::SUCCESS;
}

// convert XY position to chamber number
int MuEffMonitor::xy2Chamber(double &x, double &y, int &station, int &region) {

  double maxX=m_stationOuterX[station]/pow(2.,3-region);
  double maxY=m_stationOuterY[station]/pow(2.,3-region);

  int nrow = m_regionNch[region]*4/(3*m_regionNchCol[region]);
  int row = (int) ( (y+maxY)/(2*maxY) * (double) nrow );
  if (row > (nrow-1)) row=nrow-1;
  if (row<0) row=0;

  int col = (int) ( (maxX-x)/(2*maxX) * (double) m_regionNchCol[region] );
  if (col > (m_regionNchCol[region]-1))  col=m_regionNchCol[region]-1;
  if (col<0) col=0;

  int chid = (nrow-1-row)*m_regionNchCol[region] + col;

  //subtract missing chambers in beam pipe hole
  if (row < nrow*3/4) {
    int nrskip;
    if (row < nrow*1/4) {
      nrskip = nrow/2;
    }
    else {
      nrskip =  nrow*3/4 - row;
      if (col < m_regionNchCol[region]/2) nrskip--;
    } 
    chid -= (nrskip*m_regionNchCol[region]/2);
  }

  return chid;
}

//
//-------------------------------------------------------------------------
StatusCode MuEffMonitor::lookTisTos
    (const LHCb::Track *pTrack, const std::vector< LHCb::LHCbID > muids
     , std::string &LineName
     , int &Hlttis )
//-------------------------------------------------------------------------
{
  ////////////////////////////////////////
  //      TISTOS of candidate           //
  //      Marco                         // 
  ////////////////////////////////////////

  // loop over a vector of trigger lines name.
  // For each line, get the candidate decision, tis and tos values 
  //

  m_TriggerTisTosTool->setOfflineInput(*pTrack);
  m_TriggerTisTosTool->addToOfflineInput(muids);

  Hlttis = 0;
  
  bool decision,tis,tos;
    
  m_TriggerTisTosTool->triggerTisTos( LineName,decision,tis,tos ); 
  
  // protection in case tistos'ing is impossible. Mail Tomasz Skwarnicki
  if( m_TriggerTisTosTool->offlineLHCbIDs().size() == 0 )
  {
    info()<<"lookTisTos: problem with the signal. It has no LHCbIDs TisTos'ing will fail"<<endmsg;
    return StatusCode::SUCCESS;
  }
  if ( (decision)&&(tis) ) {
    Hlttis = 1;
  }

  // return Hlttis = 1 if the muon candidate is TIS at least in 1 of the trigger lines  
  return StatusCode::SUCCESS;
}
//
//-------------------------------------------------------------------------
StatusCode MuEffMonitor::lookL0TisTos
     (const LHCb::Track *pTrack, const std::vector< LHCb::LHCbID > muids
     , std::string &LineName
     , int &L0tis )
//-------------------------------------------------------------------------
{
  ////////////////////////////////////////
  //      TISTOS of candidate           //
  //      Marco                         // 
  ////////////////////////////////////////

  // loop over a vector of trigger lines name.
  // For each line, get the candidate decision, tis and tos values 
  // ---------  ATTENTION -------------------
  // L0 Tistos requires a different tool than hlt
  // https://twiki.cern.ch/twiki/bin/view/LHCb/L0TriggerTisTos

  m_TriggerTisTosTool->setOfflineInput(*pTrack);
  m_TriggerTisTosTool->addToOfflineInput(muids);
  
  L0tis = 0; 
  
  bool decision,tis,tos;
  m_L0TriggerTisTosTool->triggerTisTos( LineName,decision,tis,tos ); 
  if ( (decision)&&(tis) ) {
    L0tis = 1; 
  }
  // return L0tis = 1 if the muon candidate is TIS at least in 1 of the trigger lines
  return StatusCode::SUCCESS;
}

// fill vectors of x,y,z positions for the MuonCoords
//==========================================================================
StatusCode MuEffMonitor::fillCoordVectors(){
//==========================================================================
  
  m_coordPos.clear();
  m_coordPos.resize(m_NStation * m_NRegion);
  m_nReqStations=m_origReqStations;

  // get the MuonCoords for each station in turn
  LHCb::MuonCoords* coords = get<LHCb::MuonCoords>(LHCb::MuonCoordLocation::MuonCoords);
  if ( !coords ) {
   err() << " ==> Cannot retrieve MuonCoords " << endmsg;
   return StatusCode::FAILURE;
  }

  // loop over the coords
  LHCb::MuonCoords::const_iterator iCoord;
  double x,dx,y,dy,z,dz;
  StatusCode sc;
  bool M1seen=false;

  for ( iCoord = coords->begin() ; iCoord != coords->end() ; iCoord++ ){
    const LHCb::MuonTileID& tile = (*iCoord)->key();
    unsigned int station = tile.station();
    unsigned int region = tile.region();

    if(m_ignoreM1 && 0==station) continue;
    double uncross = (station == 0 || ((station>2)&&(region==0))) ? false : (*iCoord)->uncrossed();
    debug() <<"Muon coord  " << tile << " station "<< station <<" region "<< region  <<" cross  "<< uncross <<endmsg;

    if(uncross && m_mustCross) continue;
    if (!uncross) { //use FastPosTool
      sc = m_posTool->calcTilePos(tile,x,dx,y,dy,z,dz);
      if (sc.isFailure()){
        warning() << " Failed to get x,y,z of tile " << tile << endmsg;
        continue; 
      }
    }
    else {
      StatusCode sc = m_mudet->Tile2XYZ(tile,x,dx,y,dy,z,dz);
      if (sc.isFailure()){
        warning() << " Failed to get x,y,z of tile " << tile << endmsg;
        continue; 
      }
    }
    if (0==station) M1seen=true;
    m_coordPos[station*m_NRegion+region].push_back(coordExtent_(x,dx,y,dy,z,dz,*iCoord));
  }
  
  if(!M1seen && m_origReqStations == (m_NStation-1) ) m_nReqStations=m_origReqStations-1;


  return StatusCode::SUCCESS; 
}

//
//Do track preselection
//================================================================================
bool MuEffMonitor::DoTrackPreSelection(const LHCb::Track *pTrack, const LHCb::State * &muState){
//================================================================================
  // == long track && Downstream && !clone 
  m_nTracks->fill(0.);

  if ( (pTrack->checkFlag(LHCb::Track::Clone)) ||
       !( ( pTrack->checkType(LHCb::Track::Long) ) ||
          ( pTrack->checkType(LHCb::Track::Downstream) ) ) ){
    return false;
  }   
  
  m_trNdof->fill(pTrack->nDoF());
  if (pTrack->nDoF() < m_TrminDof)  return false;

  m_nTracks->fill(1.);
    
  muState = &(pTrack->closestState(m_stationZ[0]));
  if (!muState) {
    error() << " ==> Failed to get state from track at station M1 " << endmsg;
    return false;
  }
   
  const LHCb::State *muState0 = &(pTrack->firstState());
  if (!muState0) {
    error() << " ==> Failed to get state from track at first state " << endmsg;
    return false;
  }

  if ( muState0 -> p() < m_MomentumCut ) return false;
  m_nTracks->fill(2.);

  if (pTrack->probChi2() < m_Chi2TrMin) return false;
  m_nTracks->fill(3.);

  return true;
}//


//Do track preselection
//================================================================================
bool MuEffMonitor::LookForTagMuon(){
//================================================================================
//
  double CutIP = 0.05; // CutIP = 0.05 mm = 50. micron
  double MomP = -999999.;
  double MomN = -999999.;

  const LHCb::Track *pTrackP = 0;
  const LHCb::Track *pTrackN = 0;
  const LHCb::State *stateP0;
  
  bool MuonTagP = false;
  bool MuonTagN = false;
  bool Lunga = false;
    
  for (unsigned int it=0; it<m_preCandidate.size(); it++) {
      
 
    if ( !MuonTagP ) {
   
      const LHCb::Track *pTrack = m_preCandidate[it]->track();
      const LHCb::MuonPID *pMuon= m_preCandidate[it]->muonPID();
      if ( pMuon ) {
        
        bool flagMuon = pMuon->IsMuon();
        
        stateP0 = &( pTrack->firstState() );
        if (stateP0) {
          
          int carica = pTrack->charge();
          if ( pTrack->checkType(LHCb::Track::Long) ) Lunga = true;
          double IP = sqrt( pow(stateP0->x(),2) + pow(stateP0->y(),2) ) ; 
          if ( flagMuon && Lunga && ( IP > CutIP ) && (carica > 0) ) {
            MuonTagP = true;
            pTrackP = pTrack;
            MomP = stateP0->p();
            if(m_Offline) m_PPTag -> fill(MomP/Gaudi::Units::GeV);
          }
        } 
      } 
    }
 
    if ( !MuonTagN ) {
      
      const LHCb::Track *pTrack = m_preCandidate[it]->track();
      const LHCb::MuonPID *pMuon= m_preCandidate[it]->muonPID();
      if ( pMuon ) {
        
        bool flagMuon = pMuon->IsMuon();
        
        stateP0 = &( pTrack->firstState() );
        if (stateP0) {
          
          int carica = pTrack->charge();
          if ( pTrack->checkType(LHCb::Track::Long) ) Lunga = true;
          double IP = sqrt( pow(stateP0->x(),2) + pow(stateP0->y(),2) ) ; 
          if ( flagMuon && Lunga && ( IP > CutIP ) && (carica < 0) ) {
            MuonTagN = true;
            pTrackN = pTrack;
            MomN = stateP0->p();
            if(m_Offline) m_PNTag -> fill(MomN/Gaudi::Units::GeV);
          }
        } 
      } 
    }
  }

  if ( MuonTagP && MuonTagN ){
    m_rand = new Rndm::Numbers;
    m_rand->initialize( m_rsvc, Rndm::Flat(0.0,1.0) );  
 
    if ( (*m_rand)() < 0.5) {  
      m_pTrackTag = pTrackP;
      if(m_Offline) m_PPTagCheck -> fill(MomP/Gaudi::Units::GeV);
    } 
    else if ( (*m_rand)() >= 0.5) {
      m_pTrackTag = pTrackN;
      if(m_Offline) m_PNTagCheck -> fill(MomN/Gaudi::Units::GeV);
    }
    return true;
  }
  else if ( MuonTagP && !MuonTagN ) {
    m_pTrackTag = pTrackP;
    if(m_Offline) m_PPTagCheckS -> fill(MomP/Gaudi::Units::GeV);
    return true;
  }
  else if ( !MuonTagP && MuonTagN ) {
    m_pTrackTag = pTrackN;
    if(m_Offline) m_PNTagCheckS -> fill(MomN/Gaudi::Units::GeV);
    return true;
  }

  return false;
  
}
//
//Do track selection
//================================================================================
bool MuEffMonitor::DoTrackSelection(){
//================================================================================

  //======  require Mips in Calos       
  if(m_measureTime) m_timer->start(m_timeCal);
  m_muInCalo= DoCaloMIP(m_Muon.proto);
  if(m_measureTime) m_timer->stop(m_timeCal);

  if (!m_useCalo) m_muInCalo = true;
  m_nTracks->fill(4.);

  //======  Do acceptance cut and Extrapolation
  bool muInAcceptance=DoAccCheck();
  if (!muInAcceptance) return false;

  if(m_measureTime) m_timer->start(m_timeExr);  
  bool extrap = estrapola();  
  if(m_measureTime) m_timer->stop(m_timeExr);
  if ( !extrap ) return false;
  m_nTracks->fill(5.);

  //======= Do Fiducial Volume cut
  bool muInFV=DoAccSel();
  if (!muInFV) return false;
  m_nTracks->fill(6.);

  // =======  isolation cut
  if(m_measureTime) m_timer->start(m_timeIso);
  bool trkIso = isolatedTrack();
  if(m_measureTime) m_timer->stop(m_timeIso);
  if ( !trkIso ) return false;
  m_nTracks->fill(7.);

  if (m_Offline) fillPreHistos();       

   // ==  Require matching in muon detector
  if(m_measureTime) m_timer->start(m_timeMuM);
  bool muMatched = DoHitsInPad();
  if(m_measureTime) m_timer->stop(m_timeMuM);
  if (!muMatched) return false;     
  m_nTracks->fill(8.);

  // == require good chi2 from muon matching
  if (m_Muon.Chisq/(2*m_Muon.nMatchedSt) > m_Chi2MuMin) return false;
  m_nTracks->fill(9.);

  // TISTOSsing
  if (m_doTrigger) 
  {
    int L0Tis=0, Hlt1Tis=0, Hlt2Tis=0;
    if(m_measureTime) m_timer->start(m_timeTis);

    StatusCode scHlt2 = lookTisTos(m_Muon.pTrack, m_seleids, m_Hlt2LinesNames, Hlt2Tis );
    if (scHlt2.isFailure()) {
      info() << " ==> Failed to get Hlt2 TisTos infos " << endmsg;
      return false;
    }
    if(!Hlt2Tis)  {
      if(m_measureTime) m_timer->stop(m_timeTis);
      return false;
    }
    StatusCode scHlt1 = lookTisTos(m_Muon.pTrack, m_seleids, m_Hlt1LinesNames, Hlt1Tis );
    if (scHlt1.isFailure()) {
      info() << " ==> Failed to get Hlt1 TisTos infos " << endmsg;
      return false;
    }
    if(!Hlt1Tis) {
      if(m_measureTime) m_timer->stop(m_timeTis);
      return false;
    }
    StatusCode scL0 = lookL0TisTos(m_Muon.pTrack, m_seleids, m_L0LinesNames, L0Tis );
    if (scL0.isFailure()) {
      info() << " ==> Failed to get L0 TisTos infos " << endmsg;
      return false;
    }
    
    // remove from the sample muon candidates that trigger the L0 
    if (m_Offline) m_PpreTis -> fill(m_Muon.Mom0/Gaudi::Units::GeV);
    if(!L0Tis) {
      if(m_measureTime) m_timer->stop(m_timeTis);
      return false;
    }
    if(m_measureTime) m_timer->stop(m_timeTis);
  }
  
  // if required remove from the sample the tag-muon candidate
  if ( m_TagUnBias ) {  
    if (m_Offline) m_PpreTag -> fill(m_Muon.Mom0/Gaudi::Units::GeV); 
    if ( m_Muon.ForseTag ) return false;
  }
  if (m_Offline) m_nTracks->fill(10.);

  return true;
}

//===============================
bool MuEffMonitor::DoAccCheck(){
//===============================
//       
// get state closest state to M2 and M5
// x(z') = x(z) + (dx/dz * (z' - z))

  const LHCb::State * &mstate = m_Muon.stateP;

  double M2trackX = (mstate->x() + ( mstate->tx()*(m_stationZ[1]-mstate->z()) ));
  double M2trackY = (mstate->y() + ( mstate->ty()*(m_stationZ[1]-mstate->z()) ));

  double M5trackX = (mstate->x() + ( mstate->tx()*(m_stationZ[4]-mstate->z()) ));
  double M5trackY = (mstate->y() + ( mstate->ty()*(m_stationZ[4]-mstate->z()) ));
  
  // == Returns true if the track is in the muon detector acceptance

  bool OuterFV2 =  ( (fabs(M2trackX) < (m_stationOuterX[1]+m_nSigmaFidVol*m_padSizeX[7])) &&
                     (fabs(M2trackY) < (m_stationOuterY[1]+m_nSigmaFidVol*m_padSizeY[7])) );

  bool InnerFV2 =  ( (fabs(M2trackX) > (m_stationInnerX[1]-m_nSigmaFidVol*2.*m_padSizeX[4])) ||
                     (fabs(M2trackY) > (m_stationInnerY[1]-m_nSigmaFidVol*2.*m_padSizeY[4])) );

  bool OuterFV5 =  ( (fabs(M5trackX) < (m_stationOuterX[4]+m_nSigmaFidVol*m_padSizeX[19])) &&
                     (fabs(M5trackY) < (m_stationOuterY[4]+m_nSigmaFidVol*m_padSizeY[19])) );

  bool InnerFV5 =  ( (fabs(M5trackX) > (m_stationInnerX[4]-m_nSigmaFidVol*2.*m_padSizeX[16])) ||
                     (fabs(M5trackY) > (m_stationInnerY[4]-m_nSigmaFidVol*2.*m_padSizeY[16])) );

  bool Volume_Fiduciale = ( (OuterFV2 && InnerFV2)&&(OuterFV5 && InnerFV5) );

  return Volume_Fiduciale;  
}

//===============================
/*bool MuEffMonitor::DoAccSel(){
//===============================

  for (int s=0; s<m_NStation ; s+=4) { // loop on stations 1 and 5

    double marginX = m_nSigmaFidVol*sqrt(m_Muon.err2X[s]);
    double marginY = m_nSigmaFidVol*sqrt(m_Muon.err2Y[s]); 
    
    double X = m_Muon.trackX[s];
    double Y = m_Muon.trackY[s];
    
    bool OuterFV =  ( (fabs(X) < (m_stationOuterX[s]-marginX)) && 
                      (fabs(Y) < (m_stationOuterY[s]-marginY)) );
    
    bool InnerFV =  ( (fabs(X) > (m_stationInnerX[s]+marginX)) ||
                      (fabs(Y) > (m_stationInnerY[s]+marginY)) );
    
    m_Muon.VF[s] = ( OuterFV && InnerFV );
  }
  
  return true;  
  }*/
//
// Check if the track is in the muon detector acceptance:
//===============================
bool MuEffMonitor::DoAccSel(){
//===============================
//== Returns true if the track is in the muon detector acceptance
  for (int s=0; s<m_NStation ; s+=4) { // loop on stations 1 and 5

      double marginX = m_nSigmaFidVol*sqrt(m_Muon.err2X[s]);
      double marginY = m_nSigmaFidVol*sqrt(m_Muon.err2Y[s]);
//    
      if (fabs(m_Muon.trackX[s]) > m_stationOuterX[s]-marginX) return false;
      if (fabs(m_Muon.trackY[s]) > m_stationOuterY[s]-marginY) return false;
      if (fabs(m_Muon.trackX[s]) < m_stationInnerX[s]+marginX &&
          fabs(m_Muon.trackY[s]) < m_stationInnerY[s]+marginY) return false;
    }
    return true;  
}
//
// Pick up the Calorimeter informations for a given track
//=========================================================
bool MuEffMonitor::DoCaloMIP(const LHCb::ProtoParticle *proto) {
  bool Ecut = false;      
  
  m_Muon.Eecal =  proto->info( LHCb::ProtoParticle::CaloEcalE , -1 * Gaudi::Units::GeV  );
  m_Muon.Ehcal =  proto->info( LHCb::ProtoParticle::CaloHcalE , -1 * Gaudi::Units::GeV  );
  
  Ecut = ( m_Muon.Ehcal >= m_EhcalMin &&  m_Muon.Ehcal <= m_EhcalMax &&
           m_Muon.Eecal >= m_EecalMin &&  m_Muon.Eecal <= m_EecalMax );
  
  m_Hcal -> fill(m_Muon.Ehcal);
  m_Ecal -> fill(m_Muon.Eecal);
  m_ene  -> fill(m_Muon.Ehcal,m_Muon.Eecal);
  
  return Ecut;
}
 
// Removes cases of clones/ambiguous tracks
//==============================================
bool MuEffMonitor::isolatedTrack() {
//==============================================
  const LHCb::State * &iState = m_Muon.stateP;

  double iX = m_Muon.trackX[1];
  double iY = m_Muon.trackY[1];
  // Full line
  //double DistCutX = (2.*m_nSigmaX[0])*sqrt(m_Muon.err2X[1])/Gaudi::Units::cm;
  //double DistCutY = (2.*m_nSigmaY[0])*sqrt(m_Muon.err2Y[1])/Gaudi::Units::cm;

  for (std::vector<const LHCb::State*>::iterator jt = m_preCandState.begin(); jt != m_preCandState.end(); jt++) {
    const LHCb::State * jState = *jt;
    if (jState == m_Muon.stateP) continue;

    double CosTheta= (iState->momentum()).Dot(jState->momentum())/(iState->p()*jState->p());
    double jX = jState->x() + ( jState->tx()*(m_stationZ[1]-jState->z()) );
    double jY = jState->y() + ( jState->ty()*(m_stationZ[1]-jState->z()) );

    double Dist = sqrt( (iX-jX)*(iX-jX) + (iY-jY)*(iY-jY) )/Gaudi::Units::cm;
    double DistX = sqrt( (iX-jX)*(iX-jX) )/Gaudi::Units::cm;
    double DistY = sqrt( (iY-jY)*(iY-jY) )/Gaudi::Units::cm;

    double DP  = (iState->p()-jState->p())/Gaudi::Units::GeV;
    if (m_Offline) { 
      m_Angolo   -> fill(CosTheta);
      m_Distanza -> fill(Dist);
      m_DeltaP   -> fill(DP);
      m_DistanzaX -> fill(DistX);
      m_DistanzaY -> fill(DistY);
    }
      
    bool Taglio = ( (CosTheta < m_CosThetaCut) || (Dist > m_xyDistCut) );     

    if ( Taglio ) {
      if (m_Offline) { 
        m_AngoloS   -> fill(CosTheta);
        m_DistanzaS -> fill(Dist);
        m_DeltaPS   -> fill(DP);
        m_DistanzaXS -> fill(DistX);
        m_DistanzaYS -> fill(DistY);
      }
      return true;
    }
  }
  return false;
}

// extrapolation to given station to get expected positions with errors
// does the actual computation only once per candidate/station
//=======================================================
bool MuEffMonitor::estrapola(){
//=======================================================
  
  LHCb::State ExtraState =*(m_Muon.stateP);

  LHCb::ParticleID pid(13);

  for (int station=0; station<m_NStation; station++) {

    StatusCode sc = m_extrapolator->propagate(ExtraState, m_stationZ[station], pid);
    if( sc.isFailure() ) {
      debug() << " ==> Extrapolating to z = " << m_stationZ[station] << " failed "<<endmsg;
      return false;
    }
    
    m_Muon.trackX[station] = ExtraState.x();
    m_Muon.trackY[station] = ExtraState.y();
    m_Muon.err2X[station] = ExtraState.errX2();
    m_Muon.err2Y[station] = ExtraState.errY2();

    if (m_Offline && station==1) { 
      m_trExtErrX->fill(sqrt(ExtraState.errX2())); 
      m_trExtErrY->fill(sqrt(ExtraState.errY2()));
      m_trAccuracy->fill(sqrt(ExtraState.errX2()+ExtraState.errY2()), m_Muon.pTrack->nDoF());
    }
    int region=0;
    while (region<3) {
      if ( fabs( m_Muon.trackX[station] ) < (m_stationOuterX[station] / pow(2.,3-region)) &&
           fabs( m_Muon.trackY[station] ) < (m_stationOuterY[station] / pow(2.,3-region)) ) break;
      region++;
    }
    m_Muon.reg[station] = region;
  }
  return true;  
}

// Look for hits that match track
//================================
bool MuEffMonitor::DoHitsInPad(){
  int nUnmatched =0;
  m_seleids.clear();
  
  for(int station = m_NStation-1 ; station >= 0 ; station--) {

    double Chisq_min = 99999.;
    double bestDx=0., bestDy=0.;
    double bestmtcSigmax=0., bestmtcSigmay=0.;
    bool crossedHitFound = false;
    std::vector<coordExtent_>::const_iterator goodHit;
    int muHits = 0;

    for(int region = 0 ; region < m_NRegion ; region++){

      if (m_coordPos[station*m_NRegion + region].empty()) continue; 
      std::vector<coordExtent_>::const_iterator itPos;  
   
      for (itPos = m_coordPos[station*m_NRegion + region].begin();
           itPos != m_coordPos[station*m_NRegion + region].end();
           itPos++) {

        double uncross = (station == 0 || ((station>2)&&(region==0))) ? false : itPos->m_pCoord->uncrossed();
        if (uncross && m_mustCross) continue;
        if (uncross && crossedHitFound) continue; // always prefer crossed hits
        
        // coordinates of the center of the hitted pad
        const double& x = itPos->m_x;
        const double& y = itPos->m_y;
        // dimensions of the hitted pad
        const double& dx = itPos->m_dx;
        const double& dy = itPos->m_dy;
       
        double Err2x = (dx*dx*4)/12. +  m_Muon.err2X[station];
        double Err2y = (dy*dy*4)/12. +  m_Muon.err2Y[station]; 
        double Dx=0., Dy=0.;
        
        if (station==0) {
          Dx = m_nSigma1X[region]*sqrt(Err2x);
          Dy = m_nSigma1Y[region]*sqrt(Err2y);
        }
        else if (station>0) {
          Dx = m_nSigmaX[station-1]*sqrt(Err2x);
          Dy = m_nSigmaY[station-1]*sqrt(Err2y);
        }    
        // dimensions of the hitted pad (+) error on extrapolated track position
      
        double xdist = (x-m_Muon.trackX[station]);
        double ydist = (y-m_Muon.trackY[station]);
        
        double mtcSigmax = xdist/sqrt(Err2x);
        double mtcSigmay = ydist/sqrt(Err2y);
        double Chisq = pow(xdist,2.)/Err2x + pow(ydist,2.)/Err2y;
        
        if ((fabs(xdist) < Dx) && (fabs(ydist) < Dy)) {

          muHits++;          
          // pad is compatible with the track
          if ( (Chisq < Chisq_min) ||
               (!crossedHitFound && !uncross) // always prefer crossed hits
              ) {
            m_Muon.hitInM[station] = true;
            Chisq_min = Chisq;
            bestmtcSigmax = mtcSigmax;
            bestmtcSigmay = mtcSigmay;
            bestDx = Dx;
            bestDy = Dy;
            goodHit = itPos;
            if (!uncross) crossedHitFound=true;
          }
        }
      } // end loop on region hits
    }// end loop on regions
    
    if(m_Muon.hitInM[station]) {
      m_Muon.nMatchedSt++;
      m_Muon.Chisq += Chisq_min;
      m_Muon.ChisqM[station] = Chisq_min;
      m_Muon.mtcSigmax[station] = bestmtcSigmax;
      m_Muon.mtcSigmay[station] = bestmtcSigmay;

      m_Muon.hitInW[station] = muHits;
      m_Muon.Dx[station] = bestDx;
      m_Muon.Dy[station] = bestDy;

      m_seleids.push_back( (LHCb::LHCbID) goodHit->m_pCoord->key());
    } else {
      nUnmatched++;
    }

    // exit as soon as it is clear not enough stations can be matched
    if ( nUnmatched > (m_NStation-m_nReqStations) ) return false;
  } //loop on station

  return (m_Muon.nMatchedSt >= m_nReqStations);
}

// Fill some histos 
//============================================
void MuEffMonitor::fillPreHistos(){
//============================================

  int carica = (m_Muon.pTrack)->charge();

  if (m_muInCalo) {     

    m_PPreS -> fill(m_Muon.Mom0/Gaudi::Units::GeV);
    if (carica > 0) 
      m_PPPreS -> fill(m_Muon.Mom0/Gaudi::Units::GeV);
    else
      m_PNPreS -> fill(m_Muon.Mom0/Gaudi::Units::GeV);
  }
  
  m_PPreSMIP -> fill(m_Muon.Mom0/Gaudi::Units::GeV);
  if (carica > 0) 
    m_PPPreSMIP -> fill(m_Muon.Mom0/Gaudi::Units::GeV);
  else
    m_PNPreSMIP -> fill(m_Muon.Mom0/Gaudi::Units::GeV);
 
  for (int s=0; s<m_NStation; s++) {
    
    //if (!m_Muon.VF[s]) continue;

    double P = m_Muon.Mom/Gaudi::Units::GeV;
    double X = m_Muon.trackX[s];
    double Y = m_Muon.trackY[s];
    int r = m_Muon.reg[s];
    
    int chamber = xy2Chamber(X, Y, s, r);

    if (m_muInCalo) {     
   
        m_PPre[s] -> fill(P);
        
        if(carica > 0) 
          m_TrkP[s][r] -> fill(X,Y);
        else 
          m_TrkN[s][r] -> fill(X,Y);

        m_chamberTrk[s][r]->fill((double) chamber);
       
    }
      
    m_PPreMIP[s] -> fill(P);
        
    if(carica > 0) 
      m_TrkPMIP[s][r] -> fill(X,Y);
    else 
      m_TrkNMIP[s][r] -> fill(X,Y);

    m_chamberTrkMIP[s][r]->fill((double) chamber);
  }
    
}

// Do the final selection and fill the histos
//============================================
void MuEffMonitor::fillHistos(){
//============================================

  m_TrkType ->fill((m_Muon.pTrack)->type());
  m_PSel -> fill(m_Muon.Mom0/Gaudi::Units::GeV);
  m_Chi2Hits -> fill(m_Muon.Chisq/(2*m_Muon.nMatchedSt) );
  m_SHcal -> fill(m_Muon.Ehcal);
  m_SEcal -> fill(m_Muon.Eecal);
  m_Sene  -> fill(m_Muon.Ehcal, m_Muon.Eecal);

  int carica = (m_Muon.pTrack)->charge();

  if (carica > 0) 
    m_PPSel -> fill(m_Muon.Mom0/Gaudi::Units::GeV);
  else
    m_PNSel -> fill(m_Muon.Mom0/Gaudi::Units::GeV); 

  // total efficiency 
  if ( (m_Muon.Mom > m_MomentumCutM5) &&
       (m_Muon.Chisq/(2*m_Muon.nMatchedSt) < m_Chi2MuGlobal) ) { // good global candidate

    bool allGood=true;
    for (int is=(m_ignoreM1 ? 1 : 0); is<m_NStation; is++) 
      if (!m_Muon.hitInM[is]) allGood=false;
    
    m_StatEffNoMIP_den -> fill(0.);
    if(allGood) m_StatEffNoMIP_num -> fill(0.);
  
    if (m_muInCalo) {
      
      m_StationsEff_den -> fill(0.);
      if(carica > 0) m_StationsEff_denP -> fill(0.);
      else m_StationsEff_denN -> fill(0.);  
      
      if(allGood) {
        m_StationsEff_num -> fill(0.);
        if(carica > 0) m_StationsEff_numP -> fill(0.);
        else m_StationsEff_numN -> fill(0.);
      }
    } 
  }

  // bool goodGlobalCandidate=true;

  // efficiency of each station/region
  for (int s= (m_ignoreM1 ? 1 : 0); s <m_NStation; s++) {

    //if (!m_Muon.VF[s]) continue;

    int nTrigSt = m_Muon.nMatchedSt;
    double muChisq = m_Muon.Chisq;
    int muDof=m_Muon.nMatchedSt*2;
    int r = m_Muon.reg[s];

    if(m_Muon.hitInM[s]) {
      nTrigSt--;
      muChisq -= m_Muon.ChisqM[s];
      muDof -= 2;
    }
    if (nTrigSt < m_nReqStations) continue; // track would not be selected by other stations: skip
    if (s == 0 && m_Muon.TracciaIP == false) continue;
    if (s == 3 && m_Muon.Mom < m_MomentumCutM4) continue; // require higher range for M4
    if (s == 4 && m_Muon.Mom < m_MomentumCutM5) continue; // require higher range for M5

    if (m_Offline) m_Chi2OtherHits[s][r] -> fill(muChisq/muDof);
    if(muChisq/muDof > m_Chi2OtherMuMin) continue; // track is not matched well enough with other mu stations

    bool otherStationsWellMatched=true;

    for (int js=0 ; js<m_NStation; js++) {
      if(js == s) continue;

      if (false == m_Muon.hitInM[js]) continue;
      double quanteDx = fabs(m_Muon.mtcSigmax[js]);
      double quanteDy = fabs(m_Muon.mtcSigmay[js]);
      if(quanteDx > m_nSigmaXother) { otherStationsWellMatched=false; break;}
      if(quanteDy > m_nSigmaYother) { otherStationsWellMatched=false; break;}
    }

    if (!otherStationsWellMatched) continue; // track is not matched well enough with other mu stations
    m_nTracks->fill(12.+s);

    double P = m_Muon.Mom/Gaudi::Units::GeV;
    double X = m_Muon.trackX[s];
    double Y = m_Muon.trackY[s];
    int chamber = xy2Chamber(X, Y, s, m_Muon.reg[s]);
    double wHit = m_Muon.hitInW[s];
    double quanteDx = m_Muon.mtcSigmax[s];
    double quanteDy = m_Muon.mtcSigmay[s];
    
    if (m_muInCalo) {      
      
      if (m_Offline) {
        
        m_P[s] -> fill(P);
        if(carica > 0)
          m_PP[s] -> fill(P);
        else       
          m_PN[s] -> fill(P);
        
        if(carica > 0) {
          m_hit_denP[s][r] -> fill(X,Y);
        }
        else {
          m_hit_denN[s][r] -> fill(X,Y);
        }
        
        if (m_Muon.hitInM[s]) {
          m_P_hit[s]-> fill(P);        
          if(carica > 0) {
            m_PP_hit[s] -> fill(P);
            m_hit_numP[s][r] -> fill(X,Y);
          }
          else {
            m_PN_hit[s] -> fill(P);
            m_hit_numN[s][r] -> fill(X,Y);
          }
          m_hitsInW[s][r] -> fill(X,Y,wHit);
          m_nDx[s][r] -> fill(quanteDx);
          m_nDy[s][r] -> fill(quanteDy);       
        }
      }
      // Online histos
      m_chamberEff_den[s][r]->fill((double) chamber);
      if (m_Muon.hitInM[s])  m_chamberEff_num[s][r]->fill((double) chamber);
      //
    }
    if (m_Offline) { 
      m_chamberEffMIP_den[s][r]->fill((double) chamber);
      if (m_Muon.hitInM[s])  m_chamberEffMIP_num[s][r]->fill((double) chamber);
    }

    // Online Histos
    int region = s*4 + m_Muon.reg[s] +1;

    m_StatEffNoMIP_den -> fill(s+1);
    m_RegEffNoMIP_den  -> fill(region);

    if (m_muInCalo) {
      
      m_StationsEff_den -> fill(s+1);
      m_RegionsEff_den  -> fill(region);
      if(carica > 0) {
        m_StationsEff_denP -> fill(s+1);
        m_RegionsEff_denP -> fill(region);
      }
      else {
        m_StationsEff_denN -> fill(s+1);
        m_RegionsEff_denN -> fill(region);
      }
    }
    
    if (m_Muon.hitInM[s]) {
      m_StatEffNoMIP_num -> fill(s+1);
      m_RegEffNoMIP_num  -> fill(region);

      if (m_muInCalo) {
        
        m_StationsEff_num -> fill(s+1);
        m_RegionsEff_num  -> fill(region);
        if(carica > 0) {
          m_StationsEff_numP -> fill(s+1);
          m_RegionsEff_numP -> fill(region);
        }
        else {
          m_StationsEff_numN -> fill(s+1);
          m_RegionsEff_numN -> fill(region);
        }
      } 
    }
    
  }

}

//  from MuonCoords -> evaluate Bck when the number of selected muons is  zero
//==========================================================================
StatusCode MuEffMonitor::LookForBck(){
//==========================================================================

  double nBck[5][4];
  for ( int s = 0; s < 5; s++){
    for ( int r = 0; r < 4; r++){
      nBck[s][r] = 0;
    }  
  }

  // get the MuonCoords for each station in turn
  LHCb::MuonCoords* coords = get<LHCb::MuonCoords>(LHCb::MuonCoordLocation::MuonCoords);
  if ( !coords ) {
   err() << " ==> Cannot retrieve MuonCoords " << endmsg;
   return StatusCode::FAILURE;
  }

  // loop over the coords
  LHCb::MuonCoords::const_iterator iCoord;

  for ( iCoord = coords->begin() ; iCoord != coords->end() ; iCoord++ ){
    const LHCb::MuonTileID& tile = (*iCoord)->key();
    unsigned int station = tile.station();
    unsigned int region = tile.region();

    double uncross = (station == 0 || ((station>2)&&(region==0))) ? false : (*iCoord)->uncrossed();
    debug() <<"Muon coord  " << tile << " station "<< station <<" region "<< region  <<" cross  "<< uncross <<endmsg;

    if(uncross && m_mustCross) continue;
    nBck[station][region]++;
    
  }
  
  for ( int s = 0; s < 5; s++){
    for ( int r = 0; r < 4; r++){
      m_OccuBck[s][r] -> fill(nBck[s][r]);
    }  
  }

  return StatusCode::SUCCESS; 
}

//  from MuonCoords -> evaluate Occu when the number of selected tracks/muons != 0
//================================================================================
StatusCode MuEffMonitor::LookForMuOccu(){
//================================================================================

  double nHits[5][4];
  for ( int s = 0; s < 5; s++){
    for ( int r = 0; r < 4; r++){
      nHits[s][r] = 0;
    }  
  }

  // get the MuonCoords for each station in turn
  LHCb::MuonCoords* coords = get<LHCb::MuonCoords>(LHCb::MuonCoordLocation::MuonCoords);
  if ( !coords ) {
   err() << " ==> Cannot retrieve MuonCoords " << endmsg;
   return StatusCode::FAILURE;
  }

  // loop over the coords
  LHCb::MuonCoords::const_iterator iCoord;

  for ( iCoord = coords->begin() ; iCoord != coords->end() ; iCoord++ ){
    const LHCb::MuonTileID& tile = (*iCoord)->key();
    unsigned int station = tile.station();
    unsigned int region = tile.region();

    double uncross = (station == 0 || ((station>2)&&(region==0))) ? false : (*iCoord)->uncrossed();
    debug() <<"Muon coord  " << tile << " station "<< station <<" region "<< region  <<" cross  "<< uncross <<endmsg;

    if(uncross && m_mustCross) continue;
    nHits[station][region]++;
    
  }
  
  for ( int s = 0; s < 5; s++){
    for ( int r = 0; r < 4; r++){
      m_OccuMu[s][r] -> fill(nHits[s][r]);
    }  
  }

  return StatusCode::SUCCESS; 
}
