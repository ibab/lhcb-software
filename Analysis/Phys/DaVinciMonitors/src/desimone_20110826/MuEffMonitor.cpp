// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"
//
#include "Kernel/DVAlgorithm.h"
#include "GaudiKernel/AlgFactory.h"
//
// local
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
  : DVAlgorithm ( name , pSvcLocator ),
    m_notOnline(true) {

  m_nSigmaX.resize(5);
  m_nSigmaY.resize(5);
  // default values for nSigma cuts
  m_nSigmaX[0] = 2.;
  m_nSigmaX[1] = 3.5;
  m_nSigmaX[2] = 3.5;
  m_nSigmaX[3] = 3.5;
  m_nSigmaX[4] = 3.5;
  
  m_nSigmaY[0] = 3.5;
  m_nSigmaY[1] = 2.;
  m_nSigmaY[2] = 2.;
  m_nSigmaY[3] = 2.;
  m_nSigmaY[4] = 2.;

  declareProperty ( "MomentumCut"   ,m_MomentumCut = 3000. ) ;
  declareProperty ( "EecalMax"      ,m_EecalMax = 1000. ) ;
  declareProperty ( "EecalMin"      ,m_EecalMin = -100. ) ;
  declareProperty ( "EhcalMax"      ,m_EhcalMax = 3500. ) ;
  declareProperty ( "EhcalMin"      ,m_EhcalMin = 1000. ) ;

  declareProperty ( "nSigmaX"       ,m_nSigmaX );
  declareProperty ( "nSigmaY"       ,m_nSigmaY );
  declareProperty ( "Chi2Min"       ,m_Chi2Min = 10.) ;
  declareProperty ( "CosThetaCut"   ,m_CosThetaCut = 0.99 ) ;
  declareProperty ( "xyDistCut"     ,m_xyDistCut = 60. ) ;
  declareProperty ( "PCutEff"       ,m_PCutEff = 15.) ;
  declareProperty ( "Extrapolator"  ,m_extrapolatorName = "TrackMasterExtrapolator" ) ;
  declareProperty ( "Chi2Calculator",m_nChi2Calculator  = "TrackChi2Calculator" );  
  declareProperty ( "HistoLevel"    ,m_histoLevel = "Online" );
  //declareProperty ( "HistoLevel"    ,m_histoLevel = "OfflineFull" );
}

//===========================
// Destructor
//===========================
MuEffMonitor::~MuEffMonitor() {} 

//===========================
// Initialization
//===========================
StatusCode MuEffMonitor::initialize() {

  StatusCode sc = DVAlgorithm::initialize();
  if ( sc.isFailure() ) { return sc; }
  debug()   << " MuEffMonitor v1r0  ==> Initialize " << endmsg;

  GaudiAlg::HistoID  name;
  m_notOnline = (m_histoLevel != "Online");

  if (m_notOnline) {

    m_nEvts   = book1D("m_nEvts","number of processed evts",-0.5, 2.5, 3 );
    m_nTracks = book1D("m_nTracks","number of tracks after selection steps",-0.5, 7.5, 8 );

    m_nMuPreS = book1D("m_nMuPreS","number of MuCan PreSel per event",-0.5, 9.5, 10 );   
    
    m_TrkType = book1D("m_TrkType","Track type",-0.5, 8.5, 9);

    m_PpreSel  = book1D("m_PpreSel","P pre selected tracks",0.0, 60.0, 60);

    m_Chi2Hits = book1D("m_Chi2Hits","Chi2-like distribution",0.0, 100.0, 100);

    m_Hcal = book1D("m_Hcal","Hcal distribution",10., 6000., 60); 
    m_Ecal = book1D("m_Ecal","Ecal distribution",10., 2000., 40);
    m_ene  = book2D("m_ene","Hcal vs Ecal",0.0, 10000., 100, 0.0, 2000., 20);

    m_SHcal = book1D("m_SHcal","Hcal distribution",10., 6000., 60); 
    m_SEcal = book1D("m_SEcal","Ecal distribution",10., 2000., 40);
    m_Sene  = book2D("m_Sene","Hcal vs Ecal",0.0, 10000., 100, 0.0, 2000., 20);

    m_Angolo   = book1D("m_Angolo","Cos opening angle",0.8, 1.1, 32);
    m_Distanza = book1D("m_Distanza","Distanza ",0., 800., 250);
    m_DeltaP   = book2D("m_DeltaP","DeltaP",-1., 1., 100, 0.8, 1.1, 32);

    m_nMuSel = book1D("m_nMuSel","number of MuCan Sel per event",-0.5, 9.5, 10 );   

    m_PSel  = book1D("m_PSel","P selected tracks",0.0,150.0,75);

    m_P_S1    = book1D("m_P_S1","P Sample1 selected tracks",0.0, 100., 10);
    m_P_S1hit = book1D("m_P_S1hit","P Sample1 selected tracks",0.0, 100., 10);
    m_P_S2    = book1D("m_P_S2","P Sample2 selected tracks",0.0, 100., 10);
    m_P_S2hit = book1D("m_P_S2hit","P Sample2 selected tracks",0.0, 100., 10);
    m_P_S3    = book1D("m_P_S3","P Sample3 selected tracks",0.0, 100., 10);
    m_P_S3hit = book1D("m_P_S3hit","P Sample3 selected tracks",0.0, 100., 10);
    m_P_S4    = book1D("m_P_S4","P Sample4 selected tracks",0.0, 100., 10);
    m_P_S4hit = book1D("m_P_S4hit","P Sample4 selected tracks",0.0, 100., 10);
    m_P_S5    = book1D("m_P_S5","P Sample5 selected tracks",0.0, 100., 10);
    m_P_S5hit = book1D("m_P_S5hit","P Sample5 selected tracks",0.0, 100., 10);

    m_PP_S1    = book1D("m_PP_S1","P Sample1 selected tracks",0.0, 100., 10);
    m_PP_S1hit = book1D("m_PP_S1hit","P Sample1 selected tracks",0.0, 100., 10);
    m_PP_S2    = book1D("m_PP_S2","P Sample2 selected tracks",0.0, 100., 10);
    m_PP_S2hit = book1D("m_PP_S2hit","P Sample2 selected tracks",0.0, 100., 10);
    m_PP_S3    = book1D("m_PP_S3","P Sample3 selected tracks",0.0, 100., 10);
    m_PP_S3hit = book1D("m_PP_S3hit","P Sample3 selected tracks",0.0, 100., 10);
    m_PP_S4    = book1D("m_PP_S4","P Sample4 selected tracks",0.0, 100., 10);
    m_PP_S4hit = book1D("m_PP_S4hit","P Sample4 selected tracks",0.0, 100., 10);
    m_PP_S5    = book1D("m_PP_S5","P Sample5 selected tracks",0.0, 100., 10);
    m_PP_S5hit = book1D("m_PP_S5hit","P Sample5 selected tracks",0.0, 100., 10);

    m_PN_S1    = book1D("m_PN_S1","P Sample1 selected tracks",0.0, 100., 10);
    m_PN_S1hit = book1D("m_PN_S1hit","P Sample1 selected tracks",0.0, 100., 10);
    m_PN_S2    = book1D("m_PN_S2","P Sample2 selected tracks",0.0, 100., 10);
    m_PN_S2hit = book1D("m_PN_S2hit","P Sample2 selected tracks",0.0, 100., 10);
    m_PN_S3    = book1D("m_PN_S3","P Sample3 selected tracks",0.0, 100., 10);
    m_PN_S3hit = book1D("m_PN_S3hit","P Sample3 selected tracks",0.0, 100., 10);
    m_PN_S4    = book1D("m_PN_S4","P Sample4 selected tracks",0.0, 100., 10);
    m_PN_S4hit = book1D("m_PN_S4hit","P Sample4 selected tracks",0.0, 100., 10);
    m_PN_S5    = book1D("m_PN_S5","P Sample5 selected tracks",0.0, 100., 10);
    m_PN_S5hit = book1D("m_PN_S5hit","P Sample5 selected tracks",0.0, 100., 10);

    m_M1R1_hitP = book2D("m_M1R1_hitP","M1 R1",-60.,60.,30,-60.,60.,30);
    m_M1R2_hitP = book2D("m_M1R2_hitP","M1 R2",-150.,150.,75,-150.,150.,75);
    m_M1R3_hitP = book2D("m_M1R3_hitP","M1 R3",-250.,250.,125,-250.,250.,125);
    m_M1R4_hitP = book2D("m_M1R4_hitP","M1 R4",-400.,400.,200,-400.,400.,200);

    m_M2R1_hitP = book2D("m_M2R1_hitP","M2 R1",-70.,70.,35,-70.,70.,35);
    m_M2R2_hitP = book2D("m_M2R2_hitP","M2 R2",-150.,150.,75,-150.,150.,75);
    m_M2R3_hitP = book2D("m_M2R3_hitP","M2 R3",-300.,300.,150,-300.,300.,150);
    m_M2R4_hitP = book2D("m_M2R4_hitP","M2 R4",-450.,450.,225,-450.,450.,225);

    m_M3R1_hitP = book2D("m_M3R1_hitP","M3 R1",-80.,80.,40,-80.,80.,40);
    m_M3R2_hitP = book2D("m_M3R2_hitP","M3 R2",-150.,150.,75,-150.,150.,75);
    m_M3R3_hitP = book2D("m_M3R3_hitP","M3 R3",-300.,300.,150,-300.,300.,150);
    m_M3R4_hitP = book2D("m_M3R4_hitP","M3 R4",-500.,500.,250,-500.,500.,250);
    
    m_M4R1_hitP = book2D("m_M4R1_hitP","M4 R1",-80.,80.,40,-80.,80.,40);
    m_M4R2_hitP = book2D("m_M4R2_hitP","M4 R2",-200.,200.,100,-200.,200.,100);
    m_M4R3_hitP = book2D("m_M4R3_hitP","M4 R3",-350.,350.,175,-350.,350.,175);
    m_M4R4_hitP = book2D("m_M4R4_hitP","M4 R4",-600.,600.,300,-600.,600.,300);
 
    m_M5R1_hitP = book2D("m_M5R1_hitP","M5 R1",-90.,90.,45,-90.,90.,45);
    m_M5R2_hitP = book2D("m_M5R2_hitP","M5 R2",-200.,200.,100,-200.,200.,100);
    m_M5R3_hitP = book2D("m_M5R3_hitP","M5 R3",-350.,350.,175,-350.,350.,175);
    m_M5R4_hitP = book2D("m_M5R4_hitP","M5 R4",-600.,600.,300,-600.,600.,300); 

    m_M1R1_hitN = book2D("m_M1R1_hitN","M1 R1",-60.,60.,30,-60.,60.,30);
    m_M1R2_hitN = book2D("m_M1R2_hitN","M1 R2",-150.,150.,75,-150.,150.,75);
    m_M1R3_hitN = book2D("m_M1R3_hitN","M1 R3",-250.,250.,125,-250.,250.,125);
    m_M1R4_hitN = book2D("m_M1R4_hitN","M1 R4",-400.,400.,200,-400.,400.,200);
    
    m_M2R1_hitN = book2D("m_M2R1_hitN","M2 R1",-70.,70.,35,-70.,70.,35);
    m_M2R2_hitN = book2D("m_M2R2_hitN","M2 R2",-150.,150.,75,-150.,150.,75);
    m_M2R3_hitN = book2D("m_M2R3_hitN","M2 R3",-300.,300.,150,-300.,300.,150);
    m_M2R4_hitN = book2D("m_M2R4_hitN","M2 R4",-450.,450.,225,-450.,450.,225);

    m_M3R1_hitN = book2D("m_M3R1_hitN","M3 R1",-80.,80.,40,-80.,80.,40);
    m_M3R2_hitN = book2D("m_M3R2_hitN","M3 R2",-150.,150.,75,-150.,150.,75);
    m_M3R3_hitN = book2D("m_M3R3_hitN","M3 R3",-300.,300.,150,-300.,300.,150);
    m_M3R4_hitN = book2D("m_M3R4_hitN","M3 R4",-500.,500.,250,-500.,500.,250);

    m_M4R1_hitN = book2D("m_M4R1_hitN","M4 R1",-80.,80.,40,-80.,80.,40);
    m_M4R2_hitN = book2D("m_M4R2_hitN","M4 R2",-200.,200.,100,-200.,200.,100);
    m_M4R3_hitN = book2D("m_M4R3_hitN","M4 R3",-350.,350.,175,-350.,350.,175);
    m_M4R4_hitN = book2D("m_M4R4_hitN","M4 R4",-600.,600.,300,-600.,600.,300);
    
    m_M5R1_hitN = book2D("m_M5R1_hitN","M5 R1",-90.,90.,45,-90.,90.,45);
    m_M5R2_hitN = book2D("m_M5R2_hitN","M5 R2",-200.,200.,100,-200.,200.,100);
    m_M5R3_hitN = book2D("m_M5R3_hitN","M5 R3",-350.,350.,175,-350.,350.,175);
    m_M5R4_hitN = book2D("m_M5R4_hitN","M5 R4",-600.,600.,300,-600.,600.,300);  

    m_M1R1_nohitP = book2D("m_M1R1_nohitP","M1 R1",-60.,60.,30,-60.,60.,30);
    m_M1R2_nohitP = book2D("m_M1R2_nohitP","M1 R2",-150.,150.,75,-150.,150.,75);
    m_M1R3_nohitP = book2D("m_M1R3_nohitP","M1 R3",-250.,250.,125,-250.,250.,125);
    m_M1R4_nohitP = book2D("m_M1R4_nohitP","M1 R4",-400.,400.,200,-400.,400.,200);

    m_M2R1_nohitP = book2D("m_M2R1_nohitP","M2 R1",-70.,70.,35,-70.,70.,35);
    m_M2R2_nohitP = book2D("m_M2R2_nohitP","M2 R2",-150.,150.,75,-150.,150.,75);
    m_M2R3_nohitP = book2D("m_M2R3_nohitP","M2 R3",-300.,300.,150,-300.,300.,150);
    m_M2R4_nohitP = book2D("m_M2R4_nohitP","M2 R4",-450.,450.,225,-450.,450.,225);

    m_M3R1_nohitP = book2D("m_M3R1_nohitP","M3 R1",-80.,80.,40,-80.,80.,40);
    m_M3R2_nohitP = book2D("m_M3R2_nohitP","M3 R2",-150.,150.,75,-150.,150.,75);
    m_M3R3_nohitP = book2D("m_M3R3_nohitP","M3 R3",-300.,300.,150,-300.,300.,150);
    m_M3R4_nohitP = book2D("m_M3R4_nohitP","M3 R4",-500.,500.,250,-500.,500.,250);
    
    m_M4R1_nohitP = book2D("m_M4R1_nohitP","M4 R1",-80.,80.,40,-80.,80.,40);
    m_M4R2_nohitP = book2D("m_M4R2_nohitP","M4 R2",-200.,200.,100,-200.,200.,100);
    m_M4R3_nohitP = book2D("m_M4R3_nohitP","M4 R3",-350.,350.,175,-350.,350.,175);
    m_M4R4_nohitP = book2D("m_M4R4_nohitP","M4 R4",-600.,600.,300,-600.,600.,300);
 
    m_M5R1_nohitP = book2D("m_M5R1_nohitP","M5 R1",-90.,90.,45,-90.,90.,45);
    m_M5R2_nohitP = book2D("m_M5R2_nohitP","M5 R2",-200.,200.,100,-200.,200.,100);
    m_M5R3_nohitP = book2D("m_M5R3_nohitP","M5 R3",-350.,350.,175,-350.,350.,175);
    m_M5R4_nohitP = book2D("m_M5R4_nohitP","M5 R4",-600.,600.,300,-600.,600.,300); 

    m_M1R1_nohitN = book2D("m_M1R1_nohitN","M1 R1",-60.,60.,30,-60.,60.,30);
    m_M1R2_nohitN = book2D("m_M1R2_nohitN","M1 R2",-150.,150.,75,-150.,150.,75);
    m_M1R3_nohitN = book2D("m_M1R3_nohitN","M1 R3",-250.,250.,125,-250.,250.,125);
    m_M1R4_nohitN = book2D("m_M1R4_nohitN","M1 R4",-400.,400.,200,-400.,400.,200);
    
    m_M2R1_nohitN = book2D("m_M2R1_nohitN","M2 R1",-70.,70.,35,-70.,70.,35);
    m_M2R2_nohitN = book2D("m_M2R2_nohitN","M2 R2",-150.,150.,75,-150.,150.,75);
    m_M2R3_nohitN = book2D("m_M2R3_nohitN","M2 R3",-300.,300.,150,-300.,300.,150);
    m_M2R4_nohitN = book2D("m_M2R4_nohitN","M2 R4",-450.,450.,225,-450.,450.,225);

    m_M3R1_nohitN = book2D("m_M3R1_nohitN","M3 R1",-80.,80.,40,-80.,80.,40);
    m_M3R2_nohitN = book2D("m_M3R2_nohitN","M3 R2",-150.,150.,75,-150.,150.,75);
    m_M3R3_nohitN = book2D("m_M3R3_nohitN","M3 R3",-300.,300.,150,-300.,300.,150);
    m_M3R4_nohitN = book2D("m_M3R4_nohitN","M3 R4",-500.,500.,250,-500.,500.,250);

    m_M4R1_nohitN = book2D("m_M4R1_nohitN","M4 R1",-80.,80.,40,-80.,80.,40);
    m_M4R2_nohitN = book2D("m_M4R2_nohitN","M4 R2",-200.,200.,100,-200.,200.,100);
    m_M4R3_nohitN = book2D("m_M4R3_nohitN","M4 R3",-350.,350.,175,-350.,350.,175);
    m_M4R4_nohitN = book2D("m_M4R4_nohitN","M4 R4",-600.,600.,300,-600.,600.,300);
    
    m_M5R1_nohitN = book2D("m_M5R1_nohitN","M5 R1",-90.,90.,45,-90.,90.,45);
    m_M5R2_nohitN = book2D("m_M5R2_nohitN","M5 R2",-200.,200.,100,-200.,200.,100);
    m_M5R3_nohitN = book2D("m_M5R3_nohitN","M5 R3",-350.,350.,175,-350.,350.,175);
    m_M5R4_nohitN = book2D("m_M5R4_nohitN","M5 R4",-600.,600.,300,-600.,600.,300);  
    
  }
  
  m_StationsEff_den = book1D("m_StationsEff_den","selected tracks",-0.5,6.5,7); 
  m_StationsEff_num = book1D("m_StationsEff_num","selected tracks with hits ",-0.5,6.5,7); 
  m_StationsEff_denP = book1D("m_StationsEff_denP","selected + tracks",-0.5,6.5,7); 
  m_StationsEff_numP = book1D("m_StationsEff_numP","selected + tracks with hits ",-0.5,6.5,7); 
  m_StationsEff_denN = book1D("m_StationsEff_denN","selected - tracks",-0.5,6.5,7); 
  m_StationsEff_numN = book1D("m_StationsEff_numN","selected - tracks with hits ",-0.5,6.5,7); 

  m_RegionsEff_den = book1D("m_RegionsEff_den","selected tracks",-0.5,21.5,22); 
  m_RegionsEff_num = book1D("m_RegionsEff_num","selected tracks with hits ",-0.5,21.5,22);
  m_RegionsEff_denP = book1D("m_RegionsEff_denP","selected + tracks",-0.5,21.5,22); 
  m_RegionsEff_numP = book1D("m_RegionsEff_numP","selected + tracks with hits ",-0.5,21.5,22);
  m_RegionsEff_denN = book1D("m_RegionsEff_denN","selected - tracks",-0.5,21.5,22); 
  m_RegionsEff_numN = book1D("m_RegionsEff_numN","selected - tracks with hits ",-0.5,21.5,22);

  // TisTos Trigger Tool
  m_L0TriggerTisTosTool = tool<ITriggerTisTos>( "L0TriggerTisTos",this );
  m_TriggerTisTosTool = tool<ITriggerTisTos>( "TriggerTisTos",this );
 
  // Tracks extrapolator Tool:
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorName, "MuEffExtrap",this );
  //m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorName, "Extrapolator",this );
  if (!m_extrapolator){
    err()<<" error retrieving the Extrapolator Tool"<<endreq;
    return StatusCode::FAILURE;
  } 

  // Chi2 calculator Tool:
  m_chi2Calculator = tool<ITrackChi2Calculator>( m_nChi2Calculator, "Chi2Calculator",this );
  if (!m_chi2Calculator){
    err()<<" error retrieving the chi2Calculator Tool"<<endreq;
    return StatusCode::FAILURE;
  } 
  // Load Muon Detector geometry:
  sc = LoadMuonGeometry();
  if ( sc.isFailure() ) { return sc; }

  m_L0LinesNames.resize(6);
  m_L0LinesNames.push_back("L0Global");
  m_L0LinesNames.push_back("L0DiMuon");
  m_L0LinesNames.push_back("L0DiMuon,lowMult");
  m_L0LinesNames.push_back("L0Muon");
  m_L0LinesNames.push_back("L0Muon,lowMult");
  m_L0LinesNames.push_back("L0MuonHigh");
  
  m_Hlt1LinesNames.resize(17);
  m_Hlt1LinesNames.push_back("Hlt1TrackMuon");
  m_Hlt1LinesNames.push_back("Hlt1SingleMuonNoIPL0");
  m_Hlt1LinesNames.push_back("Hlt1SingleMuonNoIPL0HighPT");
  m_Hlt1LinesNames.push_back("Hlt1SingleMuonIPCL0");
  m_Hlt1LinesNames.push_back("Hlt1DiMuonNoIPL0Di");
  m_Hlt1LinesNames.push_back("Hlt1SingleMuon4BsMuMu");
  m_Hlt1LinesNames.push_back("Hlt1DiMuon4BsMuMu");
  m_Hlt1LinesNames.push_back("Hlt1DiMuonNoIP2L0");
  m_Hlt1LinesNames.push_back("Hlt1DiMuonNoIPL0Seg");
  m_Hlt1LinesNames.push_back("Hlt1DiMuonIPCL0Di");
  m_Hlt1LinesNames.push_back("Hlt1DiMuonIPC2L0");
  m_Hlt1LinesNames.push_back("Hlt1DiMuonIPCL0Seg");
  m_Hlt1LinesNames.push_back("Hlt1DiMuonNoPVL0Di");
  m_Hlt1LinesNames.push_back("Hlt1DiMuonNoPV2L0");
  m_Hlt1LinesNames.push_back("Hlt1DiMuonNoPVL0Seg");
  m_Hlt1LinesNames.push_back("Hlt1MuTrack");
  m_Hlt1LinesNames.push_back("Hlt1Global");
 
  m_Hlt2LinesNames.resize(22);
  m_Hlt2LinesNames.push_back("Hlt2DiMuonSameSign");
  m_Hlt2LinesNames.push_back("Hlt2UnbiasedDiMuon");
  m_Hlt2LinesNames.push_back("Hlt2UnbiasedDiMuonLowMass");
  m_Hlt2LinesNames.push_back("Hlt2DiMuonUnbiasedJPsi");
  m_Hlt2LinesNames.push_back("Hlt2DiMuonUnbiasedPsi2S");
  m_Hlt2LinesNames.push_back("Hlt2DiMuonUnbiasedBmm");
  m_Hlt2LinesNames.push_back("Hlt2DiMuonUnbiasedZmm");
  m_Hlt2LinesNames.push_back("Hlt2DiMuonDY1");
  m_Hlt2LinesNames.push_back("Hlt2DiMuonDY2");
  m_Hlt2LinesNames.push_back("Hlt2DiMuonDY3");
  m_Hlt2LinesNames.push_back("Hlt2DiMuonDY4");
  m_Hlt2LinesNames.push_back("Hlt2BiasedDiMuonSimple");
  m_Hlt2LinesNames.push_back("Hlt2BiasedDiMuonRefined");
  m_Hlt2LinesNames.push_back("Hlt2BiasedDiMuonMass");
  m_Hlt2LinesNames.push_back("Hlt2BiasedDiMuonIP");
  m_Hlt2LinesNames.push_back("Hlt2DiMuonBiasedJPsi");
  m_Hlt2LinesNames.push_back("Hlt2MuonFromHLT1");
  m_Hlt2LinesNames.push_back("Hlt2SingleMuon");
  m_Hlt2LinesNames.push_back("Hlt2SingleHighPTMuon");
  m_Hlt2LinesNames.push_back("Hlt2MuTrack");
  m_Hlt2LinesNames.push_back("Hlt2MuTrackNoIP");
  m_Hlt2LinesNames.push_back("Hlt2Global");

  return StatusCode::SUCCESS;
}

//=============================
// Main execution
//=============================
StatusCode MuEffMonitor::execute() {
  
  bool passed = true;
  
  debug() << " ==> Execute" << endmsg;

  StatusCode sc = StatusCode::SUCCESS ;
  setFilterPassed(false);

  if (m_notOnline) m_nEvts->fill(1.);

  ClearSeleVecs();
  
  //----------------------------------------------------------
  // Retrieve informations about event (event and run number)
  //----------------------------------------------------------
  sc = DoEvent();
  if ( sc.isFailure() ) { return sc; }

  //----------------------------
  // Fill Hits coord positions
  //---------------------------
  
  sc = fillCoordVectors();
  if ( sc.isFailure() ) return StatusCode::SUCCESS;

  //--------------
  //== GET Tracks
  //--------------
  LHCb::Tracks* trTracks  =NULL;
  if (exist<LHCb::Tracks>(LHCb::TrackLocation::Default)) trTracks = get<LHCb::Tracks>(LHCb::TrackLocation::Default);

  if (!trTracks){
    error() << " ==> Failed to get Track container " << (LHCb::TrackLocation::Default) << endreq;
    return StatusCode::FAILURE;
  }
  debug() << " ==> number of tracks " << trTracks->size() << endmsg;
  
  // ----------------------------------------------
  // BEGIN long Tracks loop
  // ----------------------------------------------

  // reset all the variables:

  m_nTrk = 0;

  LHCb::Tracks::const_iterator iTrack;
   
  for (iTrack = trTracks->begin() ; iTrack != trTracks->end() ; iTrack++){
    
    const LHCb::Track *pTrack=*iTrack;

    resetTrkVariables();

    bool MuoneCan;
    
    // get state closest state to M1
    m_stateP = &(pTrack->closestState(m_stationZ[0]));
    if (!m_stateP) {
      error() << " ==> Failed to get state from track " << endreq;
      continue;
    }
    
    // get state in zero position 
    m_stateP0 = &(pTrack->firstState()); 
    if (!m_stateP0) {
      error() << " ==> Failed to get stateP0 from track " << endreq;
      continue;
    }
    
    //===== Track selection
    MuoneCan = false;  
    passed = DoTrackSelection(pTrack);
    if (passed) {
      MuoneCan = true;
      m_FlagMCan = 1;
    }

    if (MuoneCan) {    
      
      // ====== Fetch the values of the tracks:
      m_nTrk++;
      
      m_carica = pTrack->charge();          
      
      // -------------------------
      // track typology:
      //-----------------
      // trtype = 1 Velo
      // trtype = 2 VeloR
      // trtype = 3 Long
      // trtype = 4 Upstream
      // trtype = 5 Downstream
      // trtype = 6 Ttrack
      // trtype = 7 Muon
      
      m_trtype  = pTrack->type();
      m_trflag  = pTrack->flag();
      
      m_Mom  = m_stateP->p(); 
      m_Mom0 = m_stateP0->p();
      m_TrMom  = m_stateP->pt(); 
      m_TrMom0 = m_stateP0->pt();
      m_TrSlx  = m_stateP->tx();
      m_TrSly  = m_stateP->ty();
      m_TrSlx0 = m_stateP0->tx(); 
      m_TrSly0 = m_stateP0->ty();

      //
      //======= Do linear fit with selected Muon Hits 
      //======= and match with the selected track  
      linFit(passed);
      if (passed) {

        double Zmatch = m_stationZ[0];
        
        Gaudi::XYZPoint trackPos(m_bx + m_sx*Zmatch,
                                 m_by + m_sy*Zmatch, Zmatch);  

        LHCb::State MhitsState(LHCb::StateVector(trackPos, 
                               Gaudi::XYZVector(m_sx, m_sy, 1.0 ), m_carica/10000.));

        Gaudi::TrackSymMatrix seedCov;
        seedCov(0,0) = m_errbx*m_errbx;
        seedCov(2,2) = m_errsx*m_errsx;
        seedCov(1,1) = m_errby*m_errby;
        seedCov(3,3) = m_errsy*m_errsy;
        seedCov(4,4) = m_carica/10000.;
        MhitsState.setCovariance(seedCov);

        LHCb::State M1state = pTrack->closestState(m_stationZ[0]);
        /// Now calculate the match chi2
        sc = m_chi2Calculator->calculateChi2( M1state.stateVector(), M1state.covariance(),
                                              MhitsState.stateVector(), MhitsState.covariance(),
                                              m_MatchChi2 );
	//
        if ( !sc.isSuccess() ) debug() << "Could not invert matrices" << endreq;
      }
   
      m_L0Tis = 0;
      m_Hlt1Tis = 0;
      m_Hlt2Tis = 0;

      StatusCode scL0 = lookL0TisTos(pTrack, m_seleids, m_L0LinesNames, m_L0Tis );
      if (scL0.isFailure()) {
        info() << " ==> Failed to get L0 TisTos infos " << endreq;
	continue;
      }
      StatusCode scHlt1 = lookTisTos(pTrack, m_seleids, m_Hlt1LinesNames, m_Hlt1Tis );
      if (scHlt1.isFailure()) {
	info() << " ==> Failed to get Hlt1 TisTos infos " << endreq;
	continue;
      }
      StatusCode scHlt2 = lookTisTos(pTrack, m_seleids, m_Hlt2LinesNames, m_Hlt2Tis );
      if (scHlt2.isFailure()) {
	info() << " ==> Failed to get Hlt2 TisTos infos " << endreq;
	continue;
      }
      
      fillTrkVecs();
    }
    
  }  // END tracks loop

   if (m_notOnline) m_nMuPreS->fill(m_nTrk);   

  debug()  << " ==> after tracks loop: # Mu Can PreSel "<< m_nTrk << endreq;

  fillHistos();    

  setFilterPassed(true);
  return sc;
  
}

//==========================
//  Finalize
//==========================
StatusCode MuEffMonitor::finalize() {

  debug() << "MuEffMonitor ==> Finalize" << endmsg;
  return DVAlgorithm::finalize();
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
    //debug()   <<"pippo station "<<i<<" "<<m_stationNames[i]<<endreq;
    i++;
  }

  m_mudet=getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");
  // set the size of the local vectors
  m_padSizeX.resize(m_NStation * m_NRegion);
  m_padSizeY.resize(m_NStation * m_NRegion);
  m_regionInnerX.resize(m_NStation);
  m_regionOuterX.resize(m_NStation);
  m_regionInnerY.resize(m_NStation);
  m_regionOuterY.resize(m_NStation);

  for(int station = 0 ; station < m_NStation ; station++ ){
    m_stationZ.push_back(m_mudet->getStationZ(station));
    m_regionInnerX[station] = m_mudet->getInnerX(station);
    m_regionOuterX[station] = m_mudet->getOuterX(station);
    m_regionInnerY[station] = m_mudet->getInnerY(station);
    m_regionOuterY[station] = m_mudet->getOuterY(station);
    m_stationZ[station] = m_mudet->getStationZ(station);

    debug() <<"Muon Station "<<station<<" x,y Inner "<<m_regionInnerX[station]<<", "<< m_regionInnerY[station]<<endreq;
    debug() <<"Muon Station "<<station<<" x,y Outer "<<m_regionOuterX[station]<<", "<< m_regionOuterY[station]<<endreq;

    for(int region = 0 ; region < m_NRegion ; region++ ){
      m_padSizeX[station * m_NRegion + region]=m_mudet->getPadSizeX(station,region);
      m_padSizeY[station * m_NRegion + region]=m_mudet->getPadSizeY(station,region);
       
      int ind = station*m_NRegion+region;
      debug() <<"Muon Region "<<region<<" x Pad Size "<< ind <<", "<< m_padSizeX[ind]<<endreq;
      debug() <<"Muon Region "<<region<<" y Pad Size "<< ind <<", "<< m_padSizeY[ind]<<endreq;

      if(m_padSizeX[station * m_NRegion + region]==0){
        error() << " ==> Muon Chamber Pad Size could not be retrieved !!" <<  endreq;
        return StatusCode::FAILURE;
      }
    }
  }
  return StatusCode::SUCCESS;
}

//========================
// Read Event/Run number:
//========================
StatusCode MuEffMonitor::DoEvent(){

  if (exist<LHCb::ODIN>(LHCb::ODINLocation::Default)) {

    LHCb::ODIN* odin = get<LHCb::ODIN>(LHCb::ODINLocation::Default);
    m_run = odin->runNumber();
    m_event = odin->eventNumber();
    m_BX = odin->bunchId();    
  } else {
    Error(" ==> Can't get LHCb::ODINLocation::Default (" +
	  LHCb::ODINLocation::Default + ")" );
  }
  
  return StatusCode::SUCCESS;
}

//
//-------------------------------------------------------------------------
StatusCode MuEffMonitor::lookTisTos
    (const LHCb::Track *pTrack, const std::vector< LHCb::LHCbID > muids
     , const std::vector<std::string> Lines
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
  
  //loop over the trigger lines to tistos
  for (int ind=0; ind < static_cast<int>( Lines.size() ); ind++)
  {
    debug()<<"In loop i =" << ind << " max:  "<< Lines.size()<<" line  "
           <<Lines.at(ind)<<endreq;
    
    std::string LineName = Lines.at(ind);
    bool decision,tis,tos;
    
    //All lines end with "Decision", but for the Hlt{1,2}Global
    m_TriggerTisTosTool->triggerTisTos( LineName+"(Decision)?",decision,tis,tos ); 
    
    // protection in case tistos'ing is impossible. Mail Tomasz Skwarnicki
    if( m_TriggerTisTosTool->offlineLHCbIDs().size() == 0 )
    {
      info()<<"lookTisTos: problem with the signal. It has no LHCbIDs TisTos'ing will fail"<<endreq;
      return StatusCode::SUCCESS;
    }
    if ( (decision)&&(tis) ) {
      Hlttis = 1;
      return StatusCode::SUCCESS;
    }
  }
  // return Hlttis = 1 if the muon candidate is TIS at least in 1 of the trigger lines
  
  return StatusCode::SUCCESS;
}
//
//-------------------------------------------------------------------------
StatusCode MuEffMonitor::lookL0TisTos
     (const LHCb::Track *pTrack, const std::vector< LHCb::LHCbID > muids
     , const std::vector<std::string> Lines
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
  
  //loop over the trigger lines to tistos
  for (int ind=0; ind < static_cast<int>( Lines.size() ); ind++)
  {
    std::string LineName = Lines.at(ind);
     bool decision,tis,tos;
     //All lines start with L0 and  end with "Decision", but for the L0Global
     
       m_L0TriggerTisTosTool->triggerTisTos( LineName+"(Decision)?",decision,tis,tos ); 
       if ( (decision)&&(tis) ) {
         L0tis = 1; 
         return StatusCode::SUCCESS;
       }
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

  // get the MuonCoords for each station in turn
  LHCb::MuonCoords* coords =NULL;
  if( exist<LHCb::MuonCoords>(LHCb::MuonCoordLocation::MuonCoords) ) 
      coords = get<LHCb::MuonCoords>(LHCb::MuonCoordLocation::MuonCoords);
  if ( !coords ) {
   err() << " ==> Cannot retrieve MuonCoords " << endreq;
   return StatusCode::FAILURE;
  }

  // loop over the coords
  LHCb::MuonCoords::const_iterator iCoord;
  for ( iCoord = coords->begin() ; iCoord != coords->end() ; iCoord++ ){
      int region = (*iCoord)->key().region();
      int station = (*iCoord)->key().station();
      double x,dx,y,dy,z,dz;
      LHCb::MuonTileID tile=(*iCoord)->key();

      StatusCode sc = m_mudet->Tile2XYZ(tile,x,dx,y,dy,z,dz);
      if (sc.isFailure()){
        warning() << " Failed to get x,y,z of tile " << tile << endreq; 
        continue; 
      }
      m_coordPos[station*m_NRegion+region].push_back(coordExtent_(x,dx,y,dy,z,dz,*iCoord));
  }

  return StatusCode::SUCCESS; 
}
//
//Do track selection
//================================================================================
bool MuEffMonitor::DoTrackSelection(const LHCb::Track *pTrack){
//================================================================================

  m_momentum0 = -1.;
  m_chi2trk = -1.;
  
  for (int i=0;i<5;i++){
    m_trackX[i] = -99999.;
    m_trackY[i] = -99999.;
    m_err2X[i] = -99999.;
    m_err2Y[i] = -99999.;
    m_trackPX[i] = -99999.;
    m_trackPY[i] = -99999.;
    m_trackPZ[i] = -99999.;
  }

  for (int i=0;i<5;i++){
    m_occupancy[i] = 0;
    m_xdista[i] = -99999.;
    m_ydista[i] = -99999.;
    m_Dxa[i] = -99999.;
    m_Dya[i] = -99999.;
    m_dimx[i] = -99999.;
    m_dimy[i] = -99999.;
    m_Sample[i] = 0;
    m_reg[i] = 0;
  }

  // == long track && Downstream && !clone 
  if ( (pTrack->checkFlag(LHCb::Track::Clone)) ||
       !( ( pTrack->checkType(LHCb::Track::Long) ) ||
          ( pTrack->checkType(LHCb::Track::Downstream) ) ) ){
    return false;
  }   
  if (m_notOnline) m_nTracks->fill(1.);   

  // get the momentum (MeV/c) in the first state of the track and cut on it
  m_momentum0 = m_stateP0 -> p();
  if (!(m_momentum0 > m_MomentumCut)) return false;
  if (m_notOnline) m_nTracks->fill(2.);

  m_chi2trk = (pTrack->chi2())/(pTrack->nDoF());
  if (!(m_chi2trk < m_Chi2Min)) return false;
  if (m_notOnline) m_nTracks->fill(3.);

  if (!DoAccCheck()) return false;
  if (m_notOnline) m_nTracks->fill(4.);
  
  //======  Do Mip       
  if (!DoCaloMIP(pTrack)) return false;
  if (m_notOnline) m_nTracks->fill(5.);

   //======  Do Extrapolation
  if (!estrapola(pTrack)) return false;
  if (m_notOnline) m_nTracks->fill(6.);

   // ==  Returns true if track passes the
  if (!DoHitsInPad()) return false;     
  if (m_notOnline) m_nTracks->fill(7.);

  return true;
}

// Check if the track is in the muon detector acceptance:
//===============================
bool MuEffMonitor::DoAccCheck(){
//===============================

// get state closest state to M1

  double M1trackX = m_stateP->x();
  double M1trackY = m_stateP->y();
         
// get state closest state to M5
// x(z') = x(z) + (dx/dz * (z' - z))

  double M5trackX = (m_stateP->x() + ( m_stateP->tx()*(m_stationZ[4]-m_stateP->z()) ));
  double M5trackY = (m_stateP->y() + ( m_stateP->ty()*(m_stationZ[4]-m_stateP->z()) ));
  
  // == Returns true if the track is in the muon detector acceptance

  bool OuterFV1 =  ( (fabs(M1trackX) < (m_regionOuterX[0]+m_nSigmaX[0]*m_padSizeX[3])) &&
                     (fabs(M1trackY) < (m_regionOuterY[0]+m_nSigmaY[0]*m_padSizeY[3])) );

  bool InnerFV1 =  ( (fabs(M1trackX) > (m_regionInnerX[0]-2.*m_nSigmaX[0]*m_padSizeX[0])) ||
                     (fabs(M1trackY) > (m_regionInnerY[0]-2.*m_nSigmaY[0]*m_padSizeY[0])) );

  bool OuterFV5 =  ( (fabs(M5trackX) < (m_regionOuterX[4]+m_nSigmaX[4]*m_padSizeX[19])) &&
                     (fabs(M5trackY) < (m_regionOuterY[4]+m_nSigmaY[4]*m_padSizeY[19])) );

  bool InnerFV5 =  ( (fabs(M5trackX) > (m_regionInnerX[4]-2.*m_nSigmaX[4]*m_padSizeX[16])) ||
                     (fabs(M5trackY) > (m_regionInnerY[4]-2.*m_nSigmaY[4]*m_padSizeY[16])) );

  bool Volume_Fiduciale = ( (OuterFV1 && InnerFV1)&&(OuterFV5 && InnerFV5) );

  return Volume_Fiduciale;  
}

// Pick up the Calorimeter informations for a given track
//=========================================================
  bool MuEffMonitor::DoCaloMIP(const LHCb::Track *pTrack){
//=========================================================

    float Zecal=12660.;
    float Zhcal=13500.;
    bool Ecut = false;

    // Project the state into ECAL:
    m_Xecal = m_stateP->x() +  m_stateP->tx() * (Zecal - m_stateP->z());
    m_Yecal = m_stateP->y() +  m_stateP->ty() * (Zecal - m_stateP->z());
    // Project the state into HCAL:
    m_Xhcal = m_stateP->x() +  m_stateP->tx() * (Zhcal - m_stateP->z());
    m_Yhcal = m_stateP->y() +  m_stateP->ty() * (Zhcal - m_stateP->z());
    
    // Get Protoparticles to get calo infos:
    const LHCb::ProtoParticle::Container* protos = 
      get<LHCb::ProtoParticle::Container>( LHCb::ProtoParticleLocation::Charged ) ;
    
    // loop over the protoparticles:
    LHCb:: ProtoParticle::Container:: const_iterator iproto; 
    for( iproto = protos->begin() ; protos->end() != iproto ; ++iproto )  {
      
      const LHCb::ProtoParticle* proto = *iproto ;    
      if ( 0 == proto ) continue ;
      
      const LHCb::Track* protoTrack = proto->track() ;
      if ( 0 == protoTrack ) continue ;
      
      if ((protoTrack) == (pTrack)) {      
        m_Eecal =  proto->info( LHCb::ProtoParticle::CaloEcalE , -1 * Gaudi::Units::GeV  );
        m_Ehcal =  proto->info( LHCb::ProtoParticle::CaloHcalE , -1 * Gaudi::Units::GeV  );
        m_Espd  =  proto->info( LHCb::ProtoParticle::CaloSpdE , -1 * Gaudi::Units::GeV  );
        m_Eprs  =  proto->info( LHCb::ProtoParticle::CaloPrsE , -1 * Gaudi::Units::GeV  );
       
        Ecut = ( (( m_Ehcal >= m_EhcalMin) && ( m_Ehcal <= m_EhcalMax)) && ( m_Eecal <= m_EecalMax) );
        if (m_notOnline) m_Hcal -> fill(m_Ehcal);
        if (m_notOnline) m_Ecal -> fill(m_Eecal);
        if (m_notOnline) m_ene  -> fill(m_Ehcal,m_Eecal);

        return Ecut;
      } 
    } // end loop iproto
     
    return Ecut;
  }

// Check if the track is in the muon detector acceptance:
//=======================================================
bool MuEffMonitor::estrapola(const LHCb::Track *pTrack){
//=======================================================
  
  LHCb::State ExtraState;
  double z[5];

  for (int i=0;i<5;i++){
    z[i] = -1.;
  }

  double z_OuterT9 = 9.415*Gaudi::Units::m;
  ExtraState = pTrack -> closestState(z_OuterT9);

  int station;
  for(station = 0; station < m_NStation ; station++) {

    z[station] = m_stationZ[station];

    LHCb::ParticleID pid(13);
    StatusCode sc = m_extrapolator->propagate(ExtraState,z[station],pid);
    if( sc.isFailure() ) {
      debug() << " ==> Extrapolating to z = " << z[station] << " failed "<<endmsg;
      return false;
    }

    m_trackX[station] = ExtraState.x();
    m_trackY[station] = ExtraState.y();
    m_err2X[station] = ExtraState.errX2();
    m_err2Y[station] = ExtraState.errY2(); 

    //// 4-momentum:
    // const double p    = state -> p  () ;
    // const double sX   = state -> tx () ;
    // const double sY   = state -> ty () ;     
    // const double pZ   = p / sqrt( 1.0 + sX * sX + sY * sY ) ;
    // HepLorentzVector mom;
    // mom.setE  ( sqrt( mass * mass + p * p ) ) ;
    // mom.setPx ( sX * pZ  ) ;
    // mom.setPy ( sY * pZ  ) ;
    // mom.setPz (      pZ  ) ;
    // particle -> setMomentum( mom ) ;

    double sX = ExtraState.tx();
    double sY = ExtraState.ty();
    double p  = ExtraState.p();
    double pZ = p/sqrt( 1.0 + sX*sX + sY*sY );
    m_trackPX[station] = sX*pZ;
    m_trackPY[station] = sY*pZ;
    m_trackPZ[station] = pZ;

  }
  
  return true;
  
}

// Look for hits that match track
//================================
bool MuEffMonitor::DoHitsInPad(){
//================================
  bool passed = false;

  int station;
  int region;
  int sample;
  int ii;

  int occupancy[5] = {0,0,0,0,0};
  int regione[5]   = {0,0,0,0,0};
  double xdista[5] = {-99999.,-99999.,-99999.,-99999.,-99999.};
  double ydista[5] = {-99999.,-99999.,-99999.,-99999.,-99999.};
  double Dxa[5] = {-99999.,-99999.,-99999.,-99999.,-99999.};
  double Dya[5] = {-99999.,-99999.,-99999.,-99999.,-99999.};
  double dimx[5] = {-99999.,-99999.,-99999.,-99999.,-99999.};
  double dimy[5] = {-99999.,-99999.,-99999.,-99999.,-99999.};
  double Chisqa[5] = {10000.,10000.,10000.,10000.,10000.};
  bool written[5]  = {false,false,false,false,false};

  m_seleids.clear();

  std::vector<coordExtent_>::const_iterator index;

  for(station = 0 ; station < m_NStation ; station++){
    
    int occu = 0;
    int reg_min = 0;
    double Chisq_min = 99999.;
    double xdist_min = 0.;
    double ydist_min = 0.;
    double Dx_min = 0.;
    double Dy_min = 0.;
    double dimx_min = 0.;
    double dimy_min = 0.; 
        
    for(region = 0 ; region < m_NRegion ; region++){

      if (!m_coordPos[station*m_NRegion + region].empty() ) {
        std::vector<coordExtent_>::const_iterator itPos; 

        for (itPos = m_coordPos[station*m_NRegion + region].begin();
             itPos != m_coordPos[station*m_NRegion + region].end();
             itPos++) {

          // coordinates of the center of the hitted pad
          double x = itPos->m_x;
          double y = itPos->m_y;
          // dimensions of the hitted pad
          double dx = itPos->m_dx;
          double dy = itPos->m_dy;

          double Dx;
          double Dy;
          double Err2x;
          double Err2y;
          double erropadx;
          double erropady;
          
          // dimensions of the hitted pad + error on extrapolated track position
          if (station == 0) {
            Dx = m_nSigmaX[0]*(dx*2.);
            Dy = m_nSigmaY[0]*(dy*2.);
          
            erropadx = (dx*2.)/sqrt(12.);
            erropady = (dy*2.)/sqrt(12.);

            Err2x = pow(erropadx,2.);
            Err2y = pow(erropady,2.);
          }
          if (station != 0) {
            Dx = 2.*dx + m_nSigmaX[station]*sqrt(m_err2X[station]);
            Dy = 2.*dy + m_nSigmaY[station]*sqrt(m_err2Y[station]);
          
            erropadx = (dx*2.)/sqrt(12.);
            erropady = (dy*2.)/sqrt(12.);

            Err2x = pow(erropadx,2.) + m_err2X[station];
            Err2y = pow(erropady,2.) + m_err2Y[station];
          }

          double xdist = (x-m_trackX[station]);
          double ydist = (y-m_trackY[station]);

          double Chisq = pow(xdist,2.)/Err2x + pow(ydist,2.)/Err2y;
          bool uncross;

          // check if the hit is inside the pad
          if (( ( fabs(ydist) <= Dy ) && ( fabs(xdist) <= Dx ) ) &&
                (Chisq < Chisq_min) )  {

            if (station == 0) {
              uncross = false;
            }
            else if ((station==3)&&(region==0)) {
              uncross = false;
            }
            else if ((station==4)&&(region==0)) {
              uncross = false;
            }
            else {
	      
              uncross = itPos->m_pCoord->uncrossed();
            }
            
            if (!uncross) {

              occu++;
              Chisq_min = Chisq;
              xdist_min = xdist;
              ydist_min = ydist;
              Dx_min = sqrt(m_err2X[station]);
              Dy_min = sqrt(m_err2Y[station]);
              dimx_min = dx;
              dimy_min = dy;
              reg_min  = region+1;
              index    = itPos;
            }
          }
          
        } // loop on coords
      }   // if region is not empty
      
    }     // loop on region
    
    if (occu>0) {
         occupancy[station] = occu;
         Chisqa[station] = Chisq_min;
         xdista[station] = xdist_min;
         ydista[station] = ydist_min;
         Dxa[station]  = Dx_min;
         Dya[station]  = Dy_min;
         dimx[station] = dimx_min;
         dimy[station] = dimy_min;
         regione[station] = reg_min;

         LHCb::MuonTileID tile = index->m_pCoord->key();
         LHCb::LHCbID id(tile);
         m_seleids.push_back(id);
      }

  } //loop on station

  for (sample = 0 ; sample < m_NStation ; sample++){
    int conta = 0;

    for (station = 0 ; station < m_NStation ; station++){
      if (station == sample || station == 0) continue;
      if (occupancy[station]>0) {
        conta++;
      }
    }
    if (conta >= 3) {

        m_Sample[sample] = 1;
        
        for (ii = 0 ; ii < m_NStation ; ii++){
          
          if (!written[ii]) {   
            
            if (occupancy[ii]>0) {
             
              m_xdista[ii] = xdista[ii];
              m_ydista[ii] = ydista[ii];
              m_Dxa[ii] = Dxa[ii];
              m_Dya[ii] = Dya[ii];
              m_dimx[ii] = dimx[ii];
              m_dimy[ii] = dimy[ii];
              m_Chisq[ii] = Chisqa[ii];
              m_reg[ii] = regione[ii];
            }
            m_occupancy[ii] = occupancy[ii];
            written[ii] = true;
          }
        }
        passed = true;
    }
  }
  
  return passed;
}
//
/// track fitting with linear Chi^2
void MuEffMonitor::linFit(bool &passed)
{

  passed = false;
// first extract the hits

  std::vector<LHCb::LHCbID>::const_iterator ih;
  int nPunti = 0;
  
  for(ih = m_seleids.begin(); ih != m_seleids.end(); ih++){
    int station = ih->muonID().station();
    if (station == 0) continue;
    nPunti++;
  }
  //
  double dof = nPunti - 2.;
  if (dof<0) return;

  double xc11,xc12,xc22,xv1,xv2,xxx;
  xc11 = xc12 = xc22 = xv1 = xv2 = xxx = 0;
  double yc11,yc12,yc22,yv1,yv2,yyy;
  yc11 = yc12 = yc22 = yv1 = yv2 = yyy = 0;
  double xdet,ydet;
  double xerr,yerr;
 
  int conta = 0;
  
  for(ih = m_seleids.begin(); ih != m_seleids.end(); ih++){

    int station = ih->muonID().station();
    conta++;
    if (station == 0) continue;

    double x,dx,y,dy,z,dz;
    LHCb::MuonTileID tile = ih->muonID();
           
    StatusCode sc = m_mudet->Tile2XYZ(tile,x,dx,y,dy,z,dz);
    if (sc.isFailure())  return;

    xerr = 2.* dx;
    yerr = 2.* dy;

    // then fit them with a min chi^2 in the 2 projections xz and yz
    //1) XZ projection:
    xc11 += z*z/xerr/xerr;
    xc12 +=   z/xerr/xerr;
    xc22 += 1.0/xerr/xerr;
    xv1  += z*x/xerr/xerr;
    xv2  +=   x/xerr/xerr;
    xxx  += x*x/xerr/xerr;
    
    //2) YZ projection:
    yc11 += z*z/yerr/yerr;
    yc12 +=   z/yerr/yerr;
    yc22 += 1.0/yerr/yerr;
    yv1  += z*y/yerr/yerr;
    yv2  +=   y/yerr/yerr;
    yyy  += y*y/yerr/yerr;

  }

  if( (xdet = xc11*xc22 - xc12*xc12) == 0 ) return;
  if( (ydet = yc11*yc22 - yc12*yc12) == 0 ) return;
  m_sx = (xv1*xc22-xv2*xc12)/xdet;
  m_sy = (yv1*yc22-yv2*yc12)/ydet;
  m_bx = (xv2*xc11-xv1*xc12)/xdet;
  m_by = (yv2*yc11-yv1*yc12)/ydet;
  
  m_errbx = sqrt(xc11/xdet);
  m_errsx = sqrt(xc22/xdet);
  m_covbsx = -xc12/xdet;

  m_errby = sqrt(yc11/ydet);
  m_errsy = sqrt(yc22/ydet);
  m_covbsy = -yc12/ydet;
  
  m_chi2x = (xxx + m_sx*m_sx*xc11 + m_bx*m_bx*xc22 - 2.*m_sx*xv1 -2.*m_bx*xv2 + 2*m_sx*m_bx*xc12)/dof;
  m_chi2y = (yyy + m_sy*m_sy*yc11 + m_by*m_by*yc22 - 2.*m_sy*yv1 -2.*m_by*yv2 + 2*m_sy*m_by*yc12)/dof;
  
  passed = true;

  return;
}
//
//
//====================
// reset variables
//====================
void MuEffMonitor::resetTrkVariables(){

  m_FlagMCan = 0;

  m_carica = 0;
  m_trtype = -99999.;
  m_trflag = -99999.;
    
  m_Mom  = -99999.;
  m_Mom0 = -99999.;
  m_TrMom  = -99999.;
  m_TrMom0 = -99999.;
  m_TrSlx  = -99999.;
  m_TrSly  = -99999.;
  m_TrSlx0 = -99999.;
  m_TrSly0 = -99999.;

  m_sx = -99999.;
  m_sy = -99999.;
  m_bx = -99999.;
  m_by = -99999.;
  m_errbx = -99999.;
  m_errsx = -99999.; 
  m_covbsx = -99999.;
  m_errby = -99999.;
  m_errsy = -99999.;
  m_covbsy = -99999.;
  m_chi2x = -99999.;
  m_chi2y = -99999.;
  m_MatchChi2 = -99999.;

  m_Eecal = -1000.;
  m_Ehcal = -1000.;
  m_Espd = -1000.;
  m_Eprs = -1000.;
  m_Xecal = 0.;
  m_Yecal = 0.;
  m_Xhcal = 0.;
  m_Yhcal = 0.;
  
}

//====================
// Fill Array
//====================
void MuEffMonitor::fillTrkVecs(){

  //-------------------
  // Products block:
  //-------------------

  m_P_FlagMCan.push_back(m_FlagMCan); 

  m_P_carica.push_back(m_carica);
  m_P_chi2trk.push_back(m_chi2trk);
  m_P_trtype.push_back(m_trtype);
  m_P_trflag.push_back(m_trflag);

  m_P_Mom.push_back(m_Mom);
  m_P_Mom0.push_back(m_Mom0);
  m_P_TrMom.push_back(m_TrMom);
  m_P_TrMom0.push_back(m_TrMom0);
  m_P_TrSlx.push_back(m_TrSlx);
  m_P_TrSlx0.push_back(m_TrSlx0);
  m_P_TrSly.push_back(m_TrSly);
  m_P_TrSly0.push_back(m_TrSly0);
 
  m_P_sx.push_back(m_sx);
  m_P_sy.push_back(m_sy);
  m_P_bx.push_back(m_bx);
  m_P_by.push_back(m_by);
  m_P_errbx.push_back(m_errbx);
  m_P_errsx.push_back(m_errsx); 
  m_P_covbsx.push_back(m_covbsx);
  m_P_errby.push_back(m_errby);
  m_P_errsy.push_back(m_errsy);
  m_P_covbsy.push_back(m_covbsy);
  m_P_chi2x.push_back(m_chi2x);
  m_P_chi2y.push_back(m_chi2y);
  m_P_MatchChi2.push_back(m_MatchChi2);

  m_P_Xs1.push_back(m_trackX[0]);
  m_P_Ys1.push_back(m_trackY[0]);
  m_P_Xs2.push_back(m_trackX[1]);
  m_P_Ys2.push_back(m_trackY[1]);
  m_P_Xs3.push_back(m_trackX[2]);
  m_P_Ys3.push_back(m_trackY[2]);
  m_P_Xs4.push_back(m_trackX[3]);
  m_P_Ys4.push_back(m_trackY[3]);
  m_P_Xs5.push_back(m_trackX[4]);
  m_P_Ys5.push_back(m_trackY[4]);

  m_P_Xp1.push_back(m_trackPX[0]);
  m_P_Yp1.push_back(m_trackPY[0]);
  m_P_Zp1.push_back(m_trackPZ[0]);
  m_P_Xp2.push_back(m_trackPX[1]);
  m_P_Yp2.push_back(m_trackPY[1]);
  m_P_Zp2.push_back(m_trackPZ[1]);
  m_P_Xp3.push_back(m_trackPX[2]);
  m_P_Yp3.push_back(m_trackPY[2]);
  m_P_Zp3.push_back(m_trackPZ[2]);
  m_P_Xp4.push_back(m_trackPX[3]);
  m_P_Yp4.push_back(m_trackPY[3]);
  m_P_Zp4.push_back(m_trackPZ[3]);
  m_P_Xp5.push_back(m_trackPX[4]);
  m_P_Yp5.push_back(m_trackPY[4]);
  m_P_Zp5.push_back(m_trackPZ[4]);

  m_P_occu1.push_back(m_occupancy[0]);
  m_P_occu2.push_back(m_occupancy[1]);
  m_P_occu3.push_back(m_occupancy[2]);
  m_P_occu4.push_back(m_occupancy[3]);
  m_P_occu5.push_back(m_occupancy[4]);

  m_P_xdist1.push_back(m_xdista[0]);
  m_P_xdist2.push_back(m_xdista[1]);
  m_P_xdist3.push_back(m_xdista[2]);
  m_P_xdist4.push_back(m_xdista[3]);
  m_P_xdist5.push_back(m_xdista[4]);
  m_P_ydist1.push_back(m_ydista[0]);
  m_P_ydist2.push_back(m_ydista[1]);
  m_P_ydist3.push_back(m_ydista[2]);
  m_P_ydist4.push_back(m_ydista[3]);
  m_P_ydist5.push_back(m_ydista[4]);

  m_P_Dx1.push_back(m_Dxa[0]);
  m_P_Dx2.push_back(m_Dxa[1]);
  m_P_Dx3.push_back(m_Dxa[2]);
  m_P_Dx4.push_back(m_Dxa[3]);
  m_P_Dx5.push_back(m_Dxa[4]);
  m_P_Dy1.push_back(m_Dya[0]);
  m_P_Dy2.push_back(m_Dya[1]);
  m_P_Dy3.push_back(m_Dya[2]);
  m_P_Dy4.push_back(m_Dya[3]);
  m_P_Dy5.push_back(m_Dya[4]);

  m_P_dimx1.push_back(m_dimx[0]);
  m_P_dimx2.push_back(m_dimx[1]);
  m_P_dimx3.push_back(m_dimx[2]);
  m_P_dimx4.push_back(m_dimx[3]);
  m_P_dimx5.push_back(m_dimx[4]);
  m_P_dimy1.push_back(m_dimy[0]);
  m_P_dimy2.push_back(m_dimy[1]);
  m_P_dimy3.push_back(m_dimy[2]);
  m_P_dimy4.push_back(m_dimy[3]);
  m_P_dimy5.push_back(m_dimy[4]);
 
  m_P_Sample1.push_back(m_Sample[0]);
  m_P_Sample2.push_back(m_Sample[1]);
  m_P_Sample3.push_back(m_Sample[2]);
  m_P_Sample4.push_back(m_Sample[3]);
  m_P_Sample5.push_back(m_Sample[4]);

  m_P_Chisq1.push_back(m_Chisq[0]);
  m_P_Chisq2.push_back(m_Chisq[1]);
  m_P_Chisq3.push_back(m_Chisq[2]);
  m_P_Chisq4.push_back(m_Chisq[3]);
  m_P_Chisq5.push_back(m_Chisq[4]);

  m_P_reg1.push_back(m_reg[0]);
  m_P_reg2.push_back(m_reg[1]);
  m_P_reg3.push_back(m_reg[2]);
  m_P_reg4.push_back(m_reg[3]);
  m_P_reg5.push_back(m_reg[4]);
 
  m_P_Eecal.push_back(m_Eecal);
  m_P_Ehcal.push_back(m_Ehcal); 
  m_P_Espd.push_back(m_Espd);
  m_P_Eprs.push_back(m_Eprs);
  m_P_Xecal.push_back(m_Xecal);
  m_P_Yecal.push_back(m_Yecal);
  m_P_Xhcal.push_back(m_Xhcal);
  m_P_Yhcal.push_back(m_Yhcal);

  m_P_L0Tis.push_back(m_L0Tis);
  m_P_Hlt1Tis.push_back(m_Hlt1Tis);
  m_P_Hlt2Tis.push_back(m_Hlt2Tis);

}

//
//===================================
// Clear Vecs to do the Selection
//===================================
void MuEffMonitor::ClearSeleVecs(){

  //--------------
  // Event Block:
  //--------------
  m_event = 0;
  m_run = 0;
  m_BX = 0;

  // ----------------
  // Decay products:
  // ----------------

  m_P_FlagMCan.clear();    

  m_P_carica.clear();
  m_P_chi2trk.clear();
  m_P_trtype.clear();
  m_P_trflag.clear();

  m_P_Mom.clear();
  m_P_Mom0.clear();
  m_P_TrMom.clear();
  m_P_TrMom0.clear();
  m_P_TrSlx.clear();
  m_P_TrSlx0.clear();
  m_P_TrSly.clear();
  m_P_TrSly0.clear();

  m_P_sx.clear();
  m_P_sy.clear();
  m_P_bx.clear();
  m_P_by.clear();
  m_P_errbx.clear();
  m_P_errsx.clear(); 
  m_P_covbsx.clear();
  m_P_errby.clear();
  m_P_errsy.clear();
  m_P_covbsy.clear();
  m_P_chi2x.clear();
  m_P_chi2y.clear();
  m_P_MatchChi2.clear();

  m_P_Xs1.clear();
  m_P_Ys1.clear();           
  m_P_Xs2.clear();
  m_P_Ys2.clear();
  m_P_Xs3.clear();
  m_P_Ys3.clear();
  m_P_Xs4.clear();
  m_P_Ys4.clear();
  m_P_Xs5.clear();
  m_P_Ys5.clear();
  m_P_Xp1.clear();
  m_P_Yp1.clear();
  m_P_Zp1.clear();
  m_P_Xp2.clear();
  m_P_Yp2.clear();
  m_P_Zp2.clear();
  m_P_Xp3.clear();
  m_P_Yp3.clear();
  m_P_Zp3.clear();
  m_P_Xp4.clear();
  m_P_Yp4.clear();
  m_P_Zp4.clear();
  m_P_Xp5.clear();
  m_P_Yp5.clear();
  m_P_Zp5.clear();
  m_P_occu1.clear();
  m_P_occu2.clear();
  m_P_occu3.clear();
  m_P_occu4.clear();
  m_P_occu5.clear();
  
  m_P_xdist1.clear();
  m_P_xdist2.clear();
  m_P_xdist3.clear();
  m_P_xdist4.clear();
  m_P_xdist5.clear();
  m_P_ydist1.clear();
  m_P_ydist2.clear();
  m_P_ydist3.clear();
  m_P_ydist4.clear();
  m_P_ydist5.clear();

  m_P_Dx1.clear();
  m_P_Dx2.clear();
  m_P_Dx3.clear();
  m_P_Dx4.clear();
  m_P_Dx5.clear();
  m_P_Dy1.clear();
  m_P_Dy2.clear();
  m_P_Dy3.clear();
  m_P_Dy4.clear();
  m_P_Dy5.clear();

  m_P_dimx1.clear();
  m_P_dimx2.clear();
  m_P_dimx3.clear();
  m_P_dimx4.clear();
  m_P_dimx5.clear();
  m_P_dimy1.clear();
  m_P_dimy2.clear();
  m_P_dimy3.clear();
  m_P_dimy4.clear();
  m_P_dimy5.clear();
 
  m_P_Sample1.clear();
  m_P_Sample2.clear();
  m_P_Sample3.clear();
  m_P_Sample4.clear();
  m_P_Sample5.clear();

  m_P_reg1.clear();
  m_P_reg2.clear();
  m_P_reg3.clear();
  m_P_reg4.clear();
  m_P_reg5.clear();

  m_P_Chisq1.clear();
  m_P_Chisq2.clear();
  m_P_Chisq3.clear();
  m_P_Chisq4.clear();
  m_P_Chisq5.clear();

  m_P_Eecal.clear();
  m_P_Ehcal.clear();
  m_P_Espd.clear();
  m_P_Eprs.clear();
  m_P_Xecal.clear();
  m_P_Yecal.clear();
  m_P_Xhcal.clear();
  m_P_Yhcal.clear();

  m_P_L0Tis.clear();
  m_P_Hlt1Tis.clear();
  m_P_Hlt2Tis.clear();

}

// Do the final selection and fill the histos
//============================================
void MuEffMonitor::fillHistos(){
//============================================

  //static const bool includeM1=false;
  int i, j;
  double Chi2traccia;

  bool range = false;

  int nStations;
  int selezionata[40];
  int selezionate;
  double Chi2S[40];  
  
  double Pi, Pj, CosTheta, Dist;

  if (m_nTrk > 40) m_nTrk = 40;
  
  for ( i = 0; i < 40; ++i ) {
    selezionata[i] = 1;
    Chi2S[i] = 0.0;
  }

  selezionate = m_nTrk;

  for ( i = 0; i < m_nTrk; ++i) {

    if (m_notOnline) m_TrkType -> fill(m_P_trtype[i]);

    double P  = m_P_Mom[i]/1000.;
    if (m_notOnline) m_PpreSel -> fill(P);

    Chi2traccia = 0.0;
    nStations = 0;

    if (m_P_occu2[i]>=1) {
      Chi2traccia = Chi2traccia+m_P_Chisq2[i];
      nStations = nStations+1;
    }    
       
    if (m_P_occu3[i]>=1) {
      Chi2traccia = Chi2traccia+m_P_Chisq3[i];
      nStations = nStations+1;
    }

    if (m_P_occu4[i]>=1) {
      Chi2traccia = Chi2traccia+m_P_Chisq4[i];
      nStations = nStations+1;
    }
    
    if (m_P_occu5[i]>=1) {
      Chi2traccia = Chi2traccia+m_P_Chisq5[i];
      nStations = nStations+1;
    }
    
    Chi2traccia = Chi2traccia/nStations + m_P_chi2trk[i];
    Chi2S[i] = Chi2traccia;
    if (m_notOnline) m_Chi2Hits -> fill(Chi2traccia);
    
    if ( Chi2traccia > m_Chi2Min ) {
      if (selezionata[i] == 1) {
        selezionate--;
        selezionata[i] = 0;
      }
    }
    
    Pi = sqrt(m_P_Xp2[i]*m_P_Xp2[i]+m_P_Yp2[i]*m_P_Yp2[i]+m_P_Zp2[i]*m_P_Zp2[i]);
    
    for ( j = 0; j < m_nTrk; ++j ) {
      
      if (j == i) continue;
      
      Pj = sqrt(m_P_Xp2[j]*m_P_Xp2[j]+m_P_Yp2[j]*m_P_Yp2[j]+m_P_Zp2[j]*m_P_Zp2[j]);
      CosTheta = (m_P_Xp2[i]*m_P_Xp2[j]+m_P_Yp2[i]*m_P_Yp2[j]+m_P_Zp2[i]*m_P_Zp2[j])/(Pi*Pj);
      Dist = sqrt( (m_P_Xs2[i]-m_P_Xs2[j])*(m_P_Xs2[i]-m_P_Xs2[j]) + (m_P_Ys2[i]-m_P_Ys2[j])*(m_P_Ys2[i]-m_P_Ys2[j]) )/10.;
      double DP  = P - (m_P_Mom[j]/1000.);

      if (m_notOnline) m_Angolo   -> fill(CosTheta);
      if (m_notOnline) m_Distanza -> fill(Dist);
      if (m_notOnline) m_DeltaP   -> fill(DP,CosTheta);
      
      if ( !((CosTheta < m_CosThetaCut)||(Dist > m_xyDistCut)) ) {
        if (selezionata[i] == 1) {
          selezionate--;
          selezionata[i] = 0;
        }
      }
    }

  }

  if (m_notOnline) m_nMuSel -> fill(selezionate);
  
  for ( i = 0; i < m_nTrk; ++i) {

    if (selezionata[i]==0) continue;

    double P  = m_P_Mom[i]/1000.;

    if (m_notOnline) m_PSel -> fill(P);

    bool OuterFV1 =  ( (fabs(m_P_Xs1[i]) < (m_regionOuterX[0]-m_nSigmaX[0]*m_padSizeX[3])) &&
                       (fabs(m_P_Ys1[i]) < (m_regionOuterY[0]-m_nSigmaY[0]*m_padSizeY[3])) );
    bool InnerFV1 =  ( (fabs(m_P_Xs1[i]) > (m_regionInnerX[0]+m_nSigmaX[0]*m_padSizeX[0])) ||
                       (fabs(m_P_Ys1[i]) > (m_regionInnerY[0]+m_nSigmaY[0]*m_padSizeY[0])) );

    // bool OuterFV2 =  ( (fabs(m_P_Xs2[i]) < (m_regionOuterX[1]-m_nSigmaX[1]*m_padSizeX[7])) &&
    //                    (fabs(m_P_Ys2[i]) < (m_regionOuterY[1]-m_nSigmaY[1]*m_padSizeY[7])) );
    // bool InnerFV2 =  ( (fabs(m_P_Xs2[i]) > (m_regionInnerX[1]+m_nSigmaX[1]*m_padSizeX[4])) ||
    //                    (fabs(m_P_Ys2[i]) > (m_regionInnerY[1]+m_nSigmaY[1]*m_padSizeY[4])) );

    bool OuterFV5 =  ( (fabs(m_P_Xs5[i]) < (m_regionOuterX[4]-m_nSigmaX[4]*m_padSizeX[19])) &&
                       (fabs(m_P_Ys5[i]) < (m_regionOuterY[4]-m_nSigmaY[4]*m_padSizeY[19])) );
    bool InnerFV5 =  ( (fabs(m_P_Xs5[i]) > (m_regionInnerX[4]+m_nSigmaX[4]*m_padSizeX[16])) ||
                       (fabs(m_P_Ys5[i]) > (m_regionInnerY[4]+m_nSigmaY[4]*m_padSizeY[16])) );

    bool Volume_Fiduciale = ( (OuterFV1 && InnerFV1)&&(OuterFV5 && InnerFV5) );

    bool Hit5 = ((m_P_occu5[i]>=1) && (fabs(m_P_xdist5[i]) <= m_nSigmaX[4]*m_P_Dx5[i] ) && 
                                      (fabs(m_P_ydist5[i]) <= m_nSigmaY[4]*m_P_Dy5[i]) );
    bool Hit4 = ((m_P_occu4[i]>=1) && (fabs(m_P_xdist4[i]) <= m_nSigmaX[3]*m_P_Dx4[i] ) &&
                                      (fabs(m_P_ydist4[i]) <= m_nSigmaY[3]*m_P_Dy4[i]) );

    bool Match =  ((m_P_MatchChi2[i] <= 4.) && (m_P_chi2x[i] <= .3) && (Chi2S[i] <= 7.)); 

    bool TisOK =  ((m_P_L0Tis[i]==1) && (m_P_Hlt1Tis[i]==1) && (m_P_Hlt2Tis[i]==1));

    bool Norma1 = ((m_P_Sample1[i] == 1) && Volume_Fiduciale && Match && Hit5 && TisOK);
    bool Norma2 = ((m_P_Sample2[i] == 1) && Volume_Fiduciale && Hit5 && TisOK);
    bool Norma3 = ((m_P_Sample3[i] == 1) && Volume_Fiduciale && Hit5 && TisOK);
    bool Norma4 = ((m_P_Sample4[i] == 1) && Volume_Fiduciale && Hit5 && TisOK);
    bool Norma5 = ((m_P_Sample5[i] == 1) && Volume_Fiduciale && Hit4 && TisOK);

    double XendM1[4] = {47.7,95.5,191.,382.};
    double YendM1[4] = {39.8,79.6,159.,318.};    
    double XendM2[4] = {60.,120.,240.,480.};
    double YendM2[4] = {50.,100.,200.,400.};
    double XendM3[4] = {65.,129.,259.,517.};
    double YendM3[4] = {54.,110.,210.,430.};
    double XendM4[4] = {69.,139.,277.,555.};
    double YendM4[4] = {57.,116.,230.,460.};
    double XendM5[4] = {74.,148.,269.,593.};
    double YendM5[4] = {62.,124.,247.,490.};

    range = (P > m_PCutEff);
   
    if (m_notOnline) {
      if ( range && Volume_Fiduciale ) {
        m_SHcal -> fill(m_P_Ehcal[i]);
        m_SEcal -> fill(m_P_Eecal[i]);
        m_Sene  -> fill(m_P_Ehcal[i],m_P_Eecal[i]);
      }
    }

    double xTrk;
    double yTrk;     
    int regione[5] = {0,0,0,0,0};

    xTrk = m_P_Xs1[i]/10.;
    yTrk = m_P_Ys1[i]/10.;
    if ((fabs(xTrk) <= XendM1[0]) && (fabs(yTrk) <= YendM1[0])) regione[0] = 1;
    
    if ( ((fabs(xTrk) <= XendM1[1]) && (fabs(yTrk) <= YendM1[1]) ) &&
         ((fabs(xTrk) >= XendM1[0]) || (fabs(yTrk) >= YendM1[0])) ) regione[0] = 2;
    
    if ( ((fabs(xTrk) <= XendM1[2]) && (fabs(yTrk) <= YendM1[2]) ) &&
         ((fabs(xTrk) >= XendM1[1]) || (fabs(yTrk) >= YendM1[1])) ) regione[0] = 3;
    
    if ( ((fabs(xTrk) <= XendM1[3]) && (fabs(yTrk) <= YendM1[3])) &&
         ((fabs(xTrk) >= XendM1[2]) || (fabs(yTrk) >= YendM1[2])) ) regione[0] = 4; 

    xTrk = m_P_Xs2[i]/10.;
    yTrk = m_P_Ys2[i]/10.;
    if ((fabs(xTrk) <= XendM2[0]) && (fabs(yTrk) <= YendM2[0])) regione[1] = 5;
    
    if ( ((fabs(xTrk) <= XendM2[1]) && (fabs(yTrk) <= YendM2[1]) ) &&
         ((fabs(xTrk) >= XendM2[0]) || (fabs(yTrk) >= YendM2[0])) ) regione[1] = 6;
    
    if ( ((fabs(xTrk) <= XendM2[2]) && (fabs(yTrk) <= YendM2[2]) ) &&
         ((fabs(xTrk) >= XendM2[1]) || (fabs(yTrk) >= YendM2[1])) ) regione[1] = 7;
    
    if ( ((fabs(xTrk) <= XendM2[3]) && (fabs(yTrk) <= YendM2[3])) &&
         ((fabs(xTrk) >= XendM2[2]) || (fabs(yTrk) >= YendM2[2])) ) regione[1] = 8; 

    xTrk = m_P_Xs3[i]/10.;
    yTrk = m_P_Ys3[i]/10.;
    if ((fabs(xTrk) <= XendM3[0]) && (fabs(yTrk) <= YendM3[0])) regione[2] = 9;
    
    if ( ((fabs(xTrk) <= XendM3[1]) && (fabs(yTrk) <= YendM3[1]) ) &&
         ((fabs(xTrk) >= XendM3[0]) || (fabs(yTrk) >= YendM3[0])) ) regione[2] = 10;
    
    if ( ((fabs(xTrk) <= XendM3[2]) && (fabs(yTrk) <= YendM3[2]) ) &&
         ((fabs(xTrk) >= XendM3[1]) || (fabs(yTrk) >= YendM3[1])) ) regione[2] = 11;
    
    if ( ((fabs(xTrk) <= XendM3[3]) && (fabs(yTrk) <= YendM3[3])) &&
         ((fabs(xTrk) >= XendM3[2]) || (fabs(yTrk) >= YendM3[2])) ) regione[2] = 12; 
 
    xTrk = m_P_Xs4[i]/10.;
    yTrk = m_P_Ys4[i]/10.;
    if ((fabs(xTrk) <= XendM4[0]) && (fabs(yTrk) <= YendM4[0])) regione[3] = 13;
    
    if ( ((fabs(xTrk) <= XendM4[1]) && (fabs(yTrk) <= YendM4[1]) ) &&
         ((fabs(xTrk) >= XendM4[0]) || (fabs(yTrk) >= YendM4[0])) ) regione[3] = 14;
    
    if ( ((fabs(xTrk) <= XendM4[2]) && (fabs(yTrk) <= YendM4[2]) ) &&
         ((fabs(xTrk) >= XendM4[1]) || (fabs(yTrk) >= YendM4[1])) ) regione[3] = 15;
    
    if ( ((fabs(xTrk) <= XendM4[3]) && (fabs(yTrk) <= YendM4[3])) &&
         ((fabs(xTrk) >= XendM4[2]) || (fabs(yTrk) >= YendM4[2])) ) regione[3] = 16; 

    xTrk = m_P_Xs5[i]/10.;
    yTrk = m_P_Ys5[i]/10.;
    if ((fabs(xTrk) <= XendM5[0]) && (fabs(yTrk) <= YendM5[0])) regione[4] = 17;
    
    if ( ((fabs(xTrk) <= XendM5[1]) && (fabs(yTrk) <= YendM5[1]) ) &&
         ((fabs(xTrk) >= XendM5[0]) || (fabs(yTrk) >= YendM5[0])) ) regione[4] = 18;
    
    if ( ((fabs(xTrk) <= XendM5[2]) && (fabs(yTrk) <= YendM5[2]) ) &&
         ((fabs(xTrk) >= XendM5[1]) || (fabs(yTrk) >= YendM5[1])) ) regione[4] = 19;
    
    if ( ((fabs(xTrk) <= XendM5[3]) && (fabs(yTrk) <= YendM5[3])) &&
         ((fabs(xTrk) >= XendM5[2]) || (fabs(yTrk) >= YendM5[2])) ) regione[4] = 20;


    if (Norma1) {

      if (m_notOnline) {
    	m_P_S1 -> fill(P);
	if (m_P_carica[i] > 0) m_PP_S1 -> fill(P);
        if (m_P_carica[i] < 0) m_PN_S1 -> fill(P);
	if (m_P_occu1[i] >=1) {
	  m_P_S1hit -> fill(P);
	  if (m_P_carica[i] > 0) m_PP_S1hit -> fill(P);
	  if (m_P_carica[i] < 0) m_PN_S1hit -> fill(P);
	}
      }

      if (range) {
        int stazione = 1;
        m_StationsEff_den -> fill(stazione);
        m_RegionsEff_den  -> fill(regione[0]);
        if (m_P_carica[i] > 0) {
          m_StationsEff_denP -> fill(stazione);
	  m_RegionsEff_denP  -> fill(regione[0]); 
	}
        if (m_P_carica[i] < 0) {
          m_StationsEff_denN -> fill(stazione);
	  m_RegionsEff_denN  -> fill(regione[0]); 
	}
        if (m_P_occu1[i] >=1) {
          m_StationsEff_num -> fill(stazione);
          m_RegionsEff_num  -> fill(regione[0]);
	  if (m_P_carica[i] > 0) {
	    m_StationsEff_numP -> fill(stazione);
	    m_RegionsEff_numP  -> fill(regione[0]); 
	  }
	  if (m_P_carica[i] < 0) {
	    m_StationsEff_numN -> fill(stazione);
	    m_RegionsEff_numN  -> fill(regione[0]); 
	  }

          if (m_notOnline) {
	    if (m_P_carica[i] > 0) {
	      if (regione[0]==1) m_M1R1_hitP -> fill(m_P_Xs1[i]/10.,m_P_Ys1[i]/10.);
	      if (regione[0]==2) m_M1R2_hitP -> fill(m_P_Xs1[i]/10.,m_P_Ys1[i]/10.);
	      if (regione[0]==3) m_M1R3_hitP -> fill(m_P_Xs1[i]/10.,m_P_Ys1[i]/10.);
	      if (regione[0]==4) m_M1R4_hitP -> fill(m_P_Xs1[i]/10.,m_P_Ys1[i]/10.); 
	    }
	    else if (m_P_carica[i] < 0) {
	      if (regione[0]==1) m_M1R1_hitN -> fill(m_P_Xs1[i]/10.,m_P_Ys1[i]/10.);
	      if (regione[0]==2) m_M1R2_hitN -> fill(m_P_Xs1[i]/10.,m_P_Ys1[i]/10.);
	      if (regione[0]==3) m_M1R3_hitN -> fill(m_P_Xs1[i]/10.,m_P_Ys1[i]/10.);
	      if (regione[0]==4) m_M1R4_hitN -> fill(m_P_Xs1[i]/10.,m_P_Ys1[i]/10.);
	    }
	  }
        }

	if (m_notOnline) {
	  if (m_P_occu1[i] <= 0) {
	    if (m_P_carica[i] > 0) {
	      if (regione[0]==1) m_M1R1_nohitP -> fill(m_P_Xs1[i]/10.,m_P_Ys1[i]/10.);
	      if (regione[0]==2) m_M1R2_nohitP -> fill(m_P_Xs1[i]/10.,m_P_Ys1[i]/10.);
	      if (regione[0]==3) m_M1R3_nohitP -> fill(m_P_Xs1[i]/10.,m_P_Ys1[i]/10.);
	      if (regione[0]==4) m_M1R4_nohitP -> fill(m_P_Xs1[i]/10.,m_P_Ys1[i]/10.); 
	    }
	    else if (m_P_carica[i] < 0) {
	      if (regione[0]==1) m_M1R1_nohitN -> fill(m_P_Xs1[i]/10.,m_P_Ys1[i]/10.);
	      if (regione[0]==2) m_M1R2_nohitN -> fill(m_P_Xs1[i]/10.,m_P_Ys1[i]/10.);
	      if (regione[0]==3) m_M1R3_nohitN -> fill(m_P_Xs1[i]/10.,m_P_Ys1[i]/10.);
	      if (regione[0]==4) m_M1R4_nohitN -> fill(m_P_Xs1[i]/10.,m_P_Ys1[i]/10.);
	    }
	  }
	}

      }

    }

    if (Norma2) {

      if (m_notOnline) {
    	m_P_S2 -> fill(P);
	if (m_P_carica[i] > 0) m_PP_S2 -> fill(P);
        if (m_P_carica[i] < 0) m_PN_S2 -> fill(P);
	if (m_P_occu2[i] >=1) {
	  m_P_S2hit -> fill(P);
	  if (m_P_carica[i] > 0) m_PP_S2hit -> fill(P);
	  if (m_P_carica[i] < 0) m_PN_S2hit -> fill(P);
	}
      }

      if (range) {
        int stazione = 2;
        m_StationsEff_den -> fill(stazione);
        m_RegionsEff_den  -> fill(regione[1]);
        if (m_P_carica[i] > 0) {
          m_StationsEff_denP -> fill(stazione);
	  m_RegionsEff_denP  -> fill(regione[1]); 
	}
        if (m_P_carica[i] < 0) {
          m_StationsEff_denN -> fill(stazione);
	  m_RegionsEff_denN  -> fill(regione[1]); 
	}
        if (m_P_occu2[i] >=1) {
          m_StationsEff_num -> fill(stazione);
          m_RegionsEff_num  -> fill(regione[1]);
	  if (m_P_carica[i] > 0) {
	    m_StationsEff_numP -> fill(stazione);
	    m_RegionsEff_numP  -> fill(regione[1]); 
	  }
	  if (m_P_carica[i] < 0) {
	    m_StationsEff_numN -> fill(stazione);
	    m_RegionsEff_numN  -> fill(regione[1]); 
	  }
	  if (m_notOnline) {
	    if (m_P_carica[i] > 0) {
	      if (regione[1]==5) m_M2R1_hitP -> fill(m_P_Xs2[i]/10.,m_P_Ys2[i]/10.);
	      if (regione[1]==6) m_M2R2_hitP -> fill(m_P_Xs2[i]/10.,m_P_Ys2[i]/10.);
	      if (regione[1]==7) m_M2R3_hitP -> fill(m_P_Xs2[i]/10.,m_P_Ys2[i]/10.);
	      if (regione[1]==8) m_M2R4_hitP -> fill(m_P_Xs2[i]/10.,m_P_Ys2[i]/10.); 
	    }
	    else if (m_P_carica[i] < 0) {
	      if (regione[1]==5) m_M2R1_hitN -> fill(m_P_Xs2[i]/10.,m_P_Ys2[i]/10.);
	      if (regione[1]==6) m_M2R2_hitN -> fill(m_P_Xs2[i]/10.,m_P_Ys2[i]/10.);
	      if (regione[1]==7) m_M2R3_hitN -> fill(m_P_Xs2[i]/10.,m_P_Ys2[i]/10.);
	      if (regione[1]==8) m_M2R4_hitN -> fill(m_P_Xs2[i]/10.,m_P_Ys2[i]/10.);
	    }
	  }
        }

	if (m_notOnline) {
	  if (m_P_occu2[i] <= 0) {
	    if (m_P_carica[i] > 0) {
	      if (regione[1]==5) m_M2R1_nohitP -> fill(m_P_Xs2[i]/10.,m_P_Ys2[i]/10.);
	      if (regione[1]==6) m_M2R2_nohitP -> fill(m_P_Xs2[i]/10.,m_P_Ys2[i]/10.);
	      if (regione[1]==7) m_M2R3_nohitP -> fill(m_P_Xs2[i]/10.,m_P_Ys2[i]/10.);
	      if (regione[1]==8) m_M2R4_nohitP -> fill(m_P_Xs2[i]/10.,m_P_Ys2[i]/10.); 
	    }
	    else if (m_P_carica[i] < 0) {
	      if (regione[1]==5) m_M2R1_nohitN -> fill(m_P_Xs2[i]/10.,m_P_Ys2[i]/10.);
	      if (regione[1]==6) m_M2R2_nohitN -> fill(m_P_Xs2[i]/10.,m_P_Ys2[i]/10.);
	      if (regione[1]==7) m_M2R3_nohitN -> fill(m_P_Xs2[i]/10.,m_P_Ys2[i]/10.);
	      if (regione[1]==8) m_M2R4_nohitN -> fill(m_P_Xs2[i]/10.,m_P_Ys2[i]/10.);
	    }
	  }
	}
	
      }
    }

    if (Norma3) {

      if (m_notOnline) {
    	m_P_S3 -> fill(P);
	if (m_P_carica[i] > 0) m_PP_S3 -> fill(P);
        if (m_P_carica[i] < 0) m_PN_S3 -> fill(P);
	if (m_P_occu3[i] >=1) {
	  m_P_S3hit -> fill(P);
	  if (m_P_carica[i] > 0) m_PP_S3hit -> fill(P);
	  if (m_P_carica[i] < 0) m_PN_S3hit -> fill(P);
	}
      }

      if (range) {
        int stazione = 3;
        m_StationsEff_den -> fill(stazione);
        m_RegionsEff_den  -> fill(regione[2]);
        if (m_P_carica[i] > 0) {
          m_StationsEff_denP -> fill(stazione);
	  m_RegionsEff_denP  -> fill(regione[2]); 
	}
        if (m_P_carica[i] < 0) {
          m_StationsEff_denN -> fill(stazione);
	  m_RegionsEff_denN  -> fill(regione[2]); 
	}
        if (m_P_occu3[i] >=1) {
          m_StationsEff_num -> fill(stazione);
          m_RegionsEff_num  -> fill(regione[2]);
	  if (m_P_carica[i] > 0) {
	    m_StationsEff_numP -> fill(stazione);
	    m_RegionsEff_numP  -> fill(regione[2]); 
	  }
	  if (m_P_carica[i] < 0) {
	    m_StationsEff_numN -> fill(stazione);
	    m_RegionsEff_numN  -> fill(regione[2]); 
	  }

          if (m_notOnline) {
	    if (m_P_carica[i] > 0) {
	      if (regione[2]==9) m_M3R1_hitP -> fill(m_P_Xs3[i]/10.,m_P_Ys3[i]/10.);
	      if (regione[2]==10) m_M3R2_hitP -> fill(m_P_Xs3[i]/10.,m_P_Ys3[i]/10.);
	      if (regione[2]==11) m_M3R3_hitP -> fill(m_P_Xs3[i]/10.,m_P_Ys3[i]/10.);
	      if (regione[2]==12) m_M3R4_hitP -> fill(m_P_Xs3[i]/10.,m_P_Ys3[i]/10.); 
	    }
	    else if (m_P_carica[i] < 0) {
	      if (regione[2]==9) m_M3R1_hitN -> fill(m_P_Xs3[i]/10.,m_P_Ys3[i]/10.);
	      if (regione[2]==10) m_M3R2_hitN -> fill(m_P_Xs3[i]/10.,m_P_Ys3[i]/10.);
	      if (regione[2]==11) m_M3R3_hitN -> fill(m_P_Xs3[i]/10.,m_P_Ys3[i]/10.);
	      if (regione[2]==12) m_M3R4_hitN -> fill(m_P_Xs3[i]/10.,m_P_Ys3[i]/10.);
	    }
	  }
        }

	if (m_notOnline) {
	  if (m_P_occu3[i] <= 0) {
	    if (m_P_carica[i] > 0) {
	      if (regione[2]==9) m_M3R1_nohitP -> fill(m_P_Xs3[i]/10.,m_P_Ys3[i]/10.);
	      if (regione[2]==10) m_M3R2_nohitP -> fill(m_P_Xs3[i]/10.,m_P_Ys3[i]/10.);
	      if (regione[2]==11) m_M3R3_nohitP -> fill(m_P_Xs3[i]/10.,m_P_Ys3[i]/10.);
	      if (regione[2]==12) m_M3R4_nohitP -> fill(m_P_Xs3[i]/10.,m_P_Ys3[i]/10.); 
	    }
	    else if (m_P_carica[i] < 0) {
	      if (regione[2]==9) m_M3R1_nohitN -> fill(m_P_Xs3[i]/10.,m_P_Ys3[i]/10.);
	      if (regione[2]==10) m_M3R2_nohitN -> fill(m_P_Xs3[i]/10.,m_P_Ys3[i]/10.);
	      if (regione[2]==11) m_M3R3_nohitN -> fill(m_P_Xs3[i]/10.,m_P_Ys3[i]/10.);
	      if (regione[2]==12) m_M3R4_nohitN -> fill(m_P_Xs3[i]/10.,m_P_Ys3[i]/10.);
	    }
	  }
	}

      }
    }

    if (Norma4) {

      if (m_notOnline) {
    	m_P_S4 -> fill(P);
	if (m_P_carica[i] > 0) m_PP_S4 -> fill(P);
        if (m_P_carica[i] < 0) m_PN_S4 -> fill(P);
	if (m_P_occu4[i] >=1) {
	  m_P_S4hit -> fill(P);
	  if (m_P_carica[i] > 0) m_PP_S4hit -> fill(P);
	  if (m_P_carica[i] < 0) m_PN_S4hit -> fill(P);
	}
      }

      if (range) {
        int stazione = 4;
        m_StationsEff_den -> fill(stazione);
        m_RegionsEff_den  -> fill(regione[3]);
        if (m_P_carica[i] > 0) {
          m_StationsEff_denP -> fill(stazione);
	  m_RegionsEff_denP  -> fill(regione[3]); 
	}
        if (m_P_carica[i] < 0) {
          m_StationsEff_denN -> fill(stazione);
	  m_RegionsEff_denN  -> fill(regione[3]); 
	}
        if (m_P_occu4[i] >=1) {
          m_StationsEff_num -> fill(stazione);
          m_RegionsEff_num  -> fill(regione[3]);
	  if (m_P_carica[i] > 0) {
	    m_StationsEff_numP -> fill(stazione);
	    m_RegionsEff_numP  -> fill(regione[3]); 
	  }
	  if (m_P_carica[i] < 0) {
	    m_StationsEff_numN -> fill(stazione);
	    m_RegionsEff_numN  -> fill(regione[3]); 
	  }
	  if (m_notOnline) {
	    if (m_P_carica[i] > 0) {
	      if (regione[3]==13) m_M4R1_hitP -> fill(m_P_Xs4[i]/10.,m_P_Ys4[i]/10.);
	      if (regione[3]==14) m_M4R2_hitP -> fill(m_P_Xs4[i]/10.,m_P_Ys4[i]/10.);
	      if (regione[3]==15) m_M4R3_hitP -> fill(m_P_Xs4[i]/10.,m_P_Ys4[i]/10.);
	      if (regione[3]==16) m_M4R4_hitP -> fill(m_P_Xs4[i]/10.,m_P_Ys4[i]/10.); 
	    }
	    else if (m_P_carica[i] < 0) {
	      if (regione[3]==13) m_M4R1_hitN -> fill(m_P_Xs4[i]/10.,m_P_Ys4[i]/10.);
	      if (regione[3]==14) m_M4R2_hitN -> fill(m_P_Xs4[i]/10.,m_P_Ys4[i]/10.);
	      if (regione[3]==15) m_M4R3_hitN -> fill(m_P_Xs4[i]/10.,m_P_Ys4[i]/10.);
	      if (regione[3]==16) m_M4R4_hitN -> fill(m_P_Xs4[i]/10.,m_P_Ys4[i]/10.);
	    }
	  }

        }

	if (m_notOnline) {
	  if (m_P_occu4[i] <= 0) {
	    if (m_P_carica[i] > 0) {
	      if (regione[3]==13) m_M4R1_nohitP -> fill(m_P_Xs4[i]/10.,m_P_Ys4[i]/10.);
	      if (regione[3]==14) m_M4R2_nohitP -> fill(m_P_Xs4[i]/10.,m_P_Ys4[i]/10.);
	      if (regione[3]==15) m_M4R3_nohitP -> fill(m_P_Xs4[i]/10.,m_P_Ys4[i]/10.);
	      if (regione[3]==16) m_M4R4_nohitP -> fill(m_P_Xs4[i]/10.,m_P_Ys4[i]/10.); 
	    }
	    else if (m_P_carica[i] < 0) {
	      if (regione[3]==13) m_M4R1_nohitN -> fill(m_P_Xs4[i]/10.,m_P_Ys4[i]/10.);
	      if (regione[3]==14) m_M4R2_nohitN -> fill(m_P_Xs4[i]/10.,m_P_Ys4[i]/10.);
	      if (regione[3]==15) m_M4R3_nohitN -> fill(m_P_Xs4[i]/10.,m_P_Ys4[i]/10.);
	      if (regione[3]==16) m_M4R4_nohitN -> fill(m_P_Xs4[i]/10.,m_P_Ys4[i]/10.);
	    }
	  }
	}
	
      }
    }

    if (Norma5) {

      if (m_notOnline) {
    	m_P_S5 -> fill(P);
	if (m_P_carica[i] > 0) m_PP_S5 -> fill(P);
        if (m_P_carica[i] < 0) m_PN_S5 -> fill(P);
	if (m_P_occu5[i] >=1) {
	  m_P_S5hit -> fill(P);
	  if (m_P_carica[i] > 0) m_PP_S5hit -> fill(P);
	  if (m_P_carica[i] < 0) m_PN_S5hit -> fill(P);
	}
      }

      if (range) {
        int stazione = 5;
        m_StationsEff_den -> fill(stazione);
        m_RegionsEff_den  -> fill(regione[4]);
        if (m_P_carica[i] > 0) {
          m_StationsEff_denP -> fill(stazione);
	  m_RegionsEff_denP  -> fill(regione[4]); 
	}
        if (m_P_carica[i] < 0) {
          m_StationsEff_denN -> fill(stazione);
	  m_RegionsEff_denN  -> fill(regione[4]); 
	}
        if (m_P_occu5[i] >=1) {
          m_StationsEff_num -> fill(stazione);
          m_RegionsEff_num  -> fill(regione[4]);
	  if (m_P_carica[i] > 0) {
	    m_StationsEff_numP -> fill(stazione);
	    m_RegionsEff_numP  -> fill(regione[4]); 
	  }
	  if (m_P_carica[i] < 0) {
	    m_StationsEff_numN -> fill(stazione);
	    m_RegionsEff_numN  -> fill(regione[4]); 
	  }
	  if (m_notOnline) {
	    if (m_P_carica[i] > 0) {
	      if (regione[4]==17) m_M5R1_hitP -> fill(m_P_Xs5[i]/10.,m_P_Ys5[i]/10.);
	      if (regione[4]==18) m_M5R2_hitP -> fill(m_P_Xs5[i]/10.,m_P_Ys5[i]/10.);
	      if (regione[4]==19) m_M5R3_hitP -> fill(m_P_Xs5[i]/10.,m_P_Ys5[i]/10.);
	      if (regione[4]==20) m_M5R4_hitP -> fill(m_P_Xs5[i]/10.,m_P_Ys5[i]/10.); 
	    }
	    else if (m_P_carica[i] < 0) {
	      if (regione[4]==17) m_M5R1_hitN -> fill(m_P_Xs5[i]/10.,m_P_Ys5[i]/10.);
	      if (regione[4]==18) m_M5R2_hitN -> fill(m_P_Xs5[i]/10.,m_P_Ys5[i]/10.);
	      if (regione[4]==19) m_M5R3_hitN -> fill(m_P_Xs5[i]/10.,m_P_Ys5[i]/10.);
	      if (regione[4]==20) m_M5R4_hitN -> fill(m_P_Xs5[i]/10.,m_P_Ys5[i]/10.);
	    }
	  }

        }

	if (m_notOnline) {
	  if (m_P_occu5[i] <= 0) {
	    if (m_P_carica[i] > 0) {
	      if (regione[4]==17) m_M5R1_nohitP -> fill(m_P_Xs5[i]/10.,m_P_Ys5[i]/10.);
	      if (regione[4]==18) m_M5R2_nohitP -> fill(m_P_Xs5[i]/10.,m_P_Ys5[i]/10.);
	      if (regione[4]==19) m_M5R3_nohitP -> fill(m_P_Xs5[i]/10.,m_P_Ys5[i]/10.);
	      if (regione[4]==20) m_M5R4_nohitP -> fill(m_P_Xs5[i]/10.,m_P_Ys5[i]/10.); 
	    }
	    else if (m_P_carica[i] < 0) {
	      if (regione[4]==17) m_M5R1_nohitN -> fill(m_P_Xs5[i]/10.,m_P_Ys5[i]/10.);
	      if (regione[4]==18) m_M5R2_nohitN -> fill(m_P_Xs5[i]/10.,m_P_Ys5[i]/10.);
	      if (regione[4]==19) m_M5R3_nohitN -> fill(m_P_Xs5[i]/10.,m_P_Ys5[i]/10.);
	      if (regione[4]==20) m_M5R4_nohitN -> fill(m_P_Xs5[i]/10.,m_P_Ys5[i]/10.);
	    }
	  }
	}

      }
    }

  }

}
