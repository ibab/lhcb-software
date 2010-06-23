// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "MuEffMonitor.h"

using namespace LHCb;
using namespace Gaudi;
using namespace Gaudi::Utils;
using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : MuEffMonitor
//
// 2010-6-18 : monitoring of Muon Chamber Efficiency by Patrizia De Simone
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuEffMonitor );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuEffMonitor::MuEffMonitor( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator ),
    m_notOnline(true) {

  m_nSigmaX.resize(5);
  m_nSigmaY.resize(5);
  // default values for nSigma cuts
  m_nSigmaX[0] = 3.5;
  m_nSigmaX[1] = 3.5;
  m_nSigmaX[2] = 3.5;
  m_nSigmaX[3] = 3.5;
  m_nSigmaX[4] = 3.5;
  
  m_nSigmaY[0] = 2.;
  m_nSigmaY[1] = 2.;
  m_nSigmaY[2] = 2.;
  m_nSigmaY[3] = 2.;
  m_nSigmaY[4] = 2.;
  
  declareProperty ( "DoTrigger"     ,m_DoTrigger = false );
  declareProperty ( "MomentumCut"   ,m_MomentumCut = 3000. ) ;
  declareProperty ( "EecalMax"      ,m_EecalMax = 1300. ) ;
  declareProperty ( "EecalMin"      ,m_EecalMin = -100. ) ;
  declareProperty ( "EhcalMax"      ,m_EhcalMax = 4500. ) ;
  declareProperty ( "EhcalMin"      ,m_EhcalMin = 1000. ) ;

  declareProperty ( "nSigmaX"       ,m_nSigmaX );
  declareProperty ( "nSigmaY"       ,m_nSigmaY );
  declareProperty ( "Chi2Min"       ,m_Chi2Min = 30.) ;
  declareProperty ( "CosThetaCut"   ,m_CosThetaCut = 0.99 ) ;
  declareProperty ( "xyDistCut"     ,m_xyDistCut = 40. ) ;
  declareProperty ( "PCutEff"       ,m_PCutEff = 12.) ;
  declareProperty ( "Extrapolator"  ,m_extrapolatorName = "TrackMasterExtrapolator" ) ;
  declareProperty ( "HistoLevel"    ,m_histoLevel = "OfflineFull" );
}

//===========================
// Destructor
//===========================
MuEffMonitor::~MuEffMonitor() {} 

//===========================
// Initialization
//===========================
StatusCode MuEffMonitor::initialize() {
  
  StatusCode sc =  GaudiHistoAlg::initialize();
  if ( sc.isFailure() ) { return sc; }
  debug()   << " MuEffMonitor ==> Initialize " << endmsg;

  GaudiAlg::HistoID  name;
  m_notOnline = (m_histoLevel != "Online"); 

  debug() << "required histogram Level="<<m_histoLevel<<std::endl;

  if (m_notOnline) {
    m_nEvts   = book1D("m_nEvts","number of processed evts",0.5, 1.5, 1 );
    m_nHLT1mu = book1D("m_nHLT1mu","number of HLT1mu triggers",-0.5, 2.5, 3 );
    m_nTracks = book1D("m_nTracks","number of tracks after selection steps",-0.5, 4.5, 5 );

    m_nMuPreS = book1D("m_nMuPreS","number of MuCan PreSel per event",-0.5, 9.5, 10 );   
    
    m_TrkType = book1D("m_TrkType","Track type",-0.5, 8.5, 9);

    m_PpreSel  = book1D("m_PpreSel","P pre selected tracks",0.0, 60.0, 60);

    m_Chi2Hits = book1D("m_Chi2Hits","Chi2-like distribution",0.0, 100.0, 100);

    m_Hcal = book1D("m_Hcal","Hcal distribution",10., 6000., 60); 
    m_Ecal = book1D("m_Ecal","Ecal distribution",10., 2000., 40);
    m_ene  = book2D("m_ene","Hcal vs Ecal",0.0, 10000., 100, 0.0, 2000., 20);

    m_Angolo   = book1D("m_Angle","Cos opening angle",0.8, 1.1, 32);
    m_Distanza = book1D("m_Distance","Distance",0., 800., 250);
    m_DeltaP   = book2D("m_DeltaP","DeltaP",-1., 1., 100, 0.8, 1.1, 32);

    m_nMuSel = book1D("m_nMuSel","number of MuCan Sel per event",-0.5, 9.5, 10 );   

    m_PSel  = book1D("m_PSel","P selected tracks",0.0, 60.0, 60);
    
    m_P_S1    = book1D("m_P_S_$1","P Sample1 selected tracks",0.0, 40., 8);
    m_P_S1hit = book1D("m_P_Shit_$1","P Sample1 selected tracks",0.0, 40., 8);
    m_P_S2    = book1D("m_P_S_$2","P Sample2 selected tracks",0.0, 40., 8);
    m_P_S2hit = book1D("m_P_Shit_$2","P Sample2 selected tracks",0.0, 40., 8);
    m_P_S3    = book1D("m_P_S_$3","P Sample3 selected tracks",0.0, 40., 8);
    m_P_S3hit = book1D("m_P_Shit_$3","P Sample3 selected tracks",0.0, 40., 8);
    m_P_S4    = book1D("m_P_S_$4","P Sample4 selected tracks",0.0, 40., 8);
    m_P_S4hit = book1D("m_P_Shit_$4","P Sample4 selected tracks",0.0, 40., 8);
    m_P_S5    = book1D("m_P_S_$5","P Sample5 selected tracks",0.0, 40., 8);
    m_P_S5hit = book1D("m_P_Shit_$5","P Sample5 selected tracks",0.0, 40., 8);
  }

  m_P_S    = book1D("m_P_S","P selected tracks",0.0, 40., 8);
  m_P_Shit = book1D("m_P_Shit","P selected tracks",0.0, 40., 8);

  m_StationsEff_den = book1D("m_StationsEff_den","selected tracks",-0.5,6.5,7); 
  m_StationsEff_num = book1D("m_StationsEff_num","selected tracks with hits ",-0.5,6.5,7); 

  m_RegionsEff_den = book1D("m_RegionsEff_den","selected tracks",-0.5,21.5,22); 
  m_RegionsEff_num = book1D("m_RegionsEff_num","selected tracks with hits ",-0.5,21.5,22);

  if (m_DoTrigger) {   
    // TisTos Trigger Tool
    m_TriggerTisTosTool = tool<ITriggerTisTos>( "TriggerTisTos",this );
    if (!m_TriggerTisTosTool){
      err()<<"error retrieving the TriggerTisTos Tool"<<endreq;
      return StatusCode::FAILURE;
    } 
  }
  
  
  // Tracks extrapolator Tool:
  m_extrapolator = tool<ITrackExtrapolator>( m_extrapolatorName, "Extrapolator",this );
  if (!m_extrapolator){
    err()<<" error retrieving the Extrapolator Tool"<<endreq;
    return StatusCode::FAILURE;
  } 

  // Load Muon Detector geometry:
  sc = LoadMuonGeometry();
  if ( sc.isFailure() ) { return sc; }

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

  //--------------
  // Trigger
  //--------------

  if (m_DoTrigger) {   
    sc = DoTrigger();
    if ( sc.isFailure() ) { return sc; }
    if (m_HLTMuon <= 0) return StatusCode::SUCCESS;
  }
  if (m_notOnline) m_nHLT1mu->fill(m_HLTMuon ? 1. : 0.);

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
      m_chi2trk = (pTrack->chi2())/(pTrack->nDoF());
      
      m_Mom  = m_stateP->p(); 
      m_Mom0 = m_stateP0->p();
      m_TrMom  = m_stateP->pt(); 
      m_TrMom0 = m_stateP0->pt();
      m_TrSlx  = m_stateP->tx();
      m_TrSly  = m_stateP->ty();
      m_TrSlx0 = m_stateP0->tx(); 
      m_TrSly0 = m_stateP0->ty();
      
      //======  Do Mip       
      passed = DoCaloMIP(pTrack);
      if(!passed)continue;

      if (m_DoTrigger && (m_HLTMuon>0)) {
       
        m_TriggerTisTosTool->setOfflineInput();
        if (MuoneCan) {
          m_TriggerTisTosTool->setOfflineInput(*pTrack);
          m_TriggerTisTosTool->addToOfflineInput(m_seleids);
          bool SeleDec = false;
          bool SeleTIS = false;
          bool SeleTOS = false;
          m_TriggerTisTosTool->triggerTisTos("Hlt1.*Mu.*",SeleDec,SeleTIS,SeleTOS);
          m_SeleTIS = (int)SeleTIS;
          m_SeleTOS = (int)SeleTOS;
          m_SeleTOB = (int)( SeleDec && !SeleTIS && !SeleTOS );
          debug() << " ==> SeleTrk HLT Trigger = " << SeleDec <<" "<< m_SeleTIS <<" "<< m_SeleTOS <<" "<< m_SeleTOB << endmsg;
        }
        
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
  return   GaudiHistoAlg::finalize();
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

    for(int region = 0 ; region < m_NRegion ; region++ ){
      m_padSizeX[station * m_NRegion + region]=m_mudet->getPadSizeX(station,region);
      m_padSizeY[station * m_NRegion + region]=m_mudet->getPadSizeY(station,region);

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

//========================
// Trigger infos:
//========================

StatusCode MuEffMonitor::DoTrigger(){

  // L0
  bool L0Decision = false;  
  m_L0Decision = 0;
  // HLT1
  m_HLTMuon = 0;

  if (!exist<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default) ) {
    error() << " ==> L0DUReport not found in " << LHCb::L0DUReportLocation::Default << endreq;
    return StatusCode::SUCCESS;
  }
  
  LHCb::L0DUReport* l0 = get<LHCb::L0DUReport>(LHCb::L0DUReportLocation::Default);
  L0Decision = l0->decision();

  m_L0Decision = (int)L0Decision;
  
   //Protect against missing Hlt output:
  if (!exist<HltDecReports>(LHCb::HltDecReportsLocation::Default) ) {
    error() << " ==> HltDecReports not found in " << LHCb::HltDecReportsLocation::Default << endreq;
    return StatusCode::SUCCESS;
  }
  
  if (!exist<HltSelReports>(LHCb::HltSelReportsLocation::Default) ) {
    error() << " ==> HltSelReports not found in " << LHCb::HltSelReportsLocation::Default << endreq;
    return StatusCode::SUCCESS;
  }

  bool hlt1dec, dummyTis, dummyTos;
  m_TriggerTisTosTool->setOfflineInput();
  m_TriggerTisTosTool->triggerTisTos("Hlt1.*Mu.*",hlt1dec,dummyTis,dummyTos);
  
  m_HLTMuon    = (int)hlt1dec;

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

  if (m_notOnline) m_nTracks->fill(0.); 
  // == long track && Downstream && !clone 
  if ( (pTrack->checkFlag(LHCb::Track::Clone)) ||
       !( ( pTrack->checkType(LHCb::Track::Long) ) ||
          ( pTrack->checkType(LHCb::Track::Downstream) ) ) ){
    return false;
  }
  if (m_notOnline) m_nTracks->fill(1.);   

  // get the momentum (MeV/c) in the first state of the track and cut on it
  m_momentum0 = m_stateP0 -> p();
  if (!(m_momentum0 > m_MomentumCut)) {
    return false;
  }
  if (m_notOnline) m_nTracks->fill(2.); 

  if (!DoAccCheck(pTrack)) return false;
  if (m_notOnline) m_nTracks->fill(3.); 

  if (!DoHitsInPad()) return false;     
  if (m_notOnline) m_nTracks->fill(4.); 

  return true;
}

// Check if the track is in the muon detector acceptance:
//=================================================================
bool MuEffMonitor::DoAccCheck(const LHCb::Track *pTrack){
//=================================================================

  
  LHCb::State ExtraState;
  double z[5];

  for (int i=0;i<5;i++){
    z[i] = -1.;
  }

  bool inacc = true;

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
    
  // == Returns true if the track is in the muon detector acceptance
  if( ! (fabs(m_trackX[0]) <  m_regionOuterX[0] && 
         fabs(m_trackY[0]) <  m_regionOuterY[0] )  ||  
      ! (fabs(m_trackX[m_NStation-1]) < 
         m_regionOuterX[m_NStation-1] &&
         fabs(m_trackY[m_NStation-1]) <  
         m_regionOuterY[m_NStation-1] ) ) {
    // outside M1 - M5 region
    inacc = false;
  }
  // Inner acceptance
  if( (fabs(m_trackX[0]) <  m_regionInnerX[0] && 
       fabs(m_trackY[0]) <  m_regionInnerY[0] )  ||  
      (fabs(m_trackX[m_NStation-1]) < 
       m_regionInnerX[m_NStation-1] &&
       fabs(m_trackY[m_NStation-1]) <  
       m_regionInnerY[m_NStation-1] )) {
    
    // inside M1 - M5 chamber hole
    inacc = false;
  }
    
  return inacc;  
}

// Look for hits that match track
//============================================
bool MuEffMonitor::DoHitsInPad(){
//============================================
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
          
          // dimensions of the hitted pad + error on extrapolated track position
          Dx = 2.*dx + m_nSigmaX[station]*sqrt(m_err2X[station]);
          Dy = 2.*dy + m_nSigmaY[station]*sqrt(m_err2Y[station]);

          double Err2x = pow((2.*dx),2.) + m_err2X[station];
          double Err2y = pow((2.*dy),2.) + m_err2Y[station];
      
          double xdist = (x-m_trackX[station]);
          double ydist = (y-m_trackY[station]);         

          double Chisq = pow(xdist,2.)/Err2x + pow(ydist,2.)/Err2y;

          // check if the hit is inside the pad
          if (( ( fabs(ydist) <= Dy ) && ( fabs(xdist) <= Dx ) ) &&
                (Chisq < Chisq_min) )  {

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

// Pick up the Calorimeter informations for a given track
//===========================================================================
  bool MuEffMonitor::DoCaloMIP(const LHCb::Track *pTrack){
//===========================================================================
        
    float Zecal=12660.;
    float Zhcal=13500.;

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
       
        return true; 
      } 
    } // end loop iproto
    return true;
  }

//
//====================
// reset variables
//====================
void MuEffMonitor::resetTrkVariables(){

  m_FlagMCan = 0;

  m_carica = 0;
  m_trtype = -99999.;
  m_trflag = -99999.;
  m_chi2trk = -99999.;
  
  m_Mom  = -99999.;
  m_Mom0 = -99999.;
  m_TrMom  = -99999.;
  m_TrMom0 = -99999.;
  m_TrSlx  = -99999.;
  m_TrSly  = -99999.;
  m_TrSlx0 = -99999.;
  m_TrSly0 = -99999.;

  m_Eecal = -1000.;
  m_Ehcal = -1000.;
  m_Espd = -1000.;
  m_Eprs = -1000.;
  m_Xecal = 0.;
  m_Yecal = 0.;
  m_Xhcal = 0.;
  m_Yhcal = 0.;

  // Trigger info:
  m_SeleTIS = 0;
  m_SeleTOS = 0;
  m_SeleTOB = 0;
  
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

  m_P_SeleTIS.push_back(m_SeleTIS);
  m_P_SeleTOS.push_back(m_SeleTOS);
  m_P_SeleTOB.push_back(m_SeleTOB); 
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

  //--------------
  // Trigger Block:
  //--------------
  m_L0Decision = 0;
  m_HLTMuon = 0;

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

  m_P_SeleTIS.clear();
  m_P_SeleTOS.clear();
  m_P_SeleTOB.clear();
  
}

// Do the final selection and fill the histos
//============================================
void MuEffMonitor::fillHistos(){
//============================================

  static const bool includeM1=false;
  int i, j;
  double Chi2traccia;

  bool Ecut;
  bool range = false;

  int nStations;
  int selezionata[40];
  int selezionate;
  
  double Pi, Pj, CosTheta, Dist;

  if (m_nTrk > 40) m_nTrk = 40;
  
  for ( i = 0; i < 40; ++i ) selezionata[i] = 1;

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
    if (m_notOnline) m_Chi2Hits -> fill(Chi2traccia);
    
    Ecut = ( (( m_P_Ehcal[i] >= m_EhcalMin) && ( m_P_Ehcal[i] <= m_EhcalMax)) && ( m_P_Eecal[i] <= m_EecalMax) );
    if (m_notOnline) m_Hcal -> fill(m_P_Ehcal[i]);
    if (m_notOnline) m_Ecal -> fill(m_P_Eecal[i]);
    if (m_notOnline) m_ene  -> fill(m_P_Ehcal[i],m_P_Eecal[i]);
    
    if ( (Chi2traccia > m_Chi2Min) || (!Ecut) ) {
      if (selezionata[i] == 1) {
        selezionate--;
        selezionata[i] = 0;
      }
    }
    
    Pi = sqrt(m_P_Xp1[i]*m_P_Xp1[i]+m_P_Yp1[i]*m_P_Yp1[i]+m_P_Zp1[i]*m_P_Zp1[i]);
    
    for ( j = 0; j < m_nTrk; ++j ) {
      
      if (j == i) continue;
      
      Pj = sqrt(m_P_Xp1[j]*m_P_Xp1[j]+m_P_Yp1[j]*m_P_Yp1[j]+m_P_Zp1[j]*m_P_Zp1[j]);
      CosTheta = (m_P_Xp1[i]*m_P_Xp1[j]+m_P_Yp1[i]*m_P_Yp1[j]+m_P_Zp1[i]*m_P_Zp1[j])/(Pi*Pj);
      Dist = sqrt( (m_P_Xs1[i]-m_P_Xs1[j])*(m_P_Xs1[i]-m_P_Xs1[j]) + (m_P_Ys1[i]-m_P_Ys1[j])*(m_P_Ys1[i]-m_P_Ys1[j]) )/10.;
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
    bool InnerFV1 =  ( (fabs(m_P_Xs1[i]) > (m_regionInnerX[0]+m_nSigmaX[0]*m_padSizeX[0])) &&
                       (fabs(m_P_Ys1[i]) > (m_regionInnerY[0]+m_nSigmaY[0]*m_padSizeY[0])) );

    bool OuterFV5 =  ( (fabs(m_P_Xs5[i]) < (m_regionOuterX[4]-m_nSigmaX[4]*m_padSizeX[19])) &&
                       (fabs(m_P_Ys5[i]) < (m_regionOuterY[4]-m_nSigmaY[4]*m_padSizeY[19])) );
    bool InnerFV5 =  ( (fabs(m_P_Xs5[i]) > (m_regionInnerX[4]+m_nSigmaX[4]*m_padSizeX[16])) &&
                       (fabs(m_P_Ys5[i]) > (m_regionInnerY[4]+m_nSigmaY[4]*m_padSizeY[16])) );

    bool Volume_Fiduciale = ( (OuterFV1 && InnerFV1)&&(OuterFV5 && InnerFV5) );
   
    bool Hit5 =((m_P_occu5[i]>=1)&&(fabs(m_P_xdist5[i])<= m_P_Dx5[i] )&&(fabs(m_P_ydist5[i])<= m_P_Dy5[i]) );
    bool Hit4 =((m_P_occu4[i]>=1)&&(fabs(m_P_xdist4[i])<= m_P_Dx4[i] )&&(fabs(m_P_ydist4[i])<= m_P_Dy4[i]) );

    bool non_triggera = true;
    if (m_DoTrigger) non_triggera = (m_P_SeleTOS[i] == 0);

    bool Norma1 = ((m_P_Sample1[i] == 1) && Volume_Fiduciale && Hit5 && non_triggera);
    bool Norma2 = ((m_P_Sample2[i] == 1) && Volume_Fiduciale && Hit5 && non_triggera);
    bool Norma3 = ((m_P_Sample3[i] == 1) && Volume_Fiduciale && Hit5 && non_triggera);
    bool Norma4 = ((m_P_Sample4[i] == 1) && Volume_Fiduciale && Hit5 && non_triggera);
    bool Norma5 = ((m_P_Sample5[i] == 1) && Volume_Fiduciale && Hit4 && non_triggera);

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
      if (includeM1) m_P_S -> fill(P);
      if (m_notOnline) m_P_S1 -> fill(P);
      if (m_P_occu1[i] >=1) {
        if (includeM1) m_P_Shit -> fill(P);
        if (m_notOnline) m_P_S1hit -> fill(P);
      }
      if (range && includeM1) {
        int stazione = 1;
        m_StationsEff_den -> fill(stazione);
        m_RegionsEff_den  -> fill(regione[0]);
        if (m_P_occu1[i] >=1) {
          m_StationsEff_num -> fill(stazione);
          m_RegionsEff_num  -> fill(regione[0]);
        }
      }
    }

    if (Norma2) {
      m_P_S -> fill(P);
      if (m_notOnline) m_P_S2 -> fill(P);
      if (m_P_occu1[i] >=1) {
        m_P_Shit -> fill(P);
        if (m_notOnline) m_P_S2hit -> fill(P);
      }

      if (range) {
        int stazione = 2;
        m_StationsEff_den -> fill(stazione);
        m_RegionsEff_den  -> fill(regione[1]);
        if (m_P_occu2[i] >=1) {
          m_StationsEff_num -> fill(stazione);
          m_RegionsEff_num  -> fill(regione[1]);
        }
      }
    }

    if (Norma3) {
      m_P_S -> fill(P);
      if (m_notOnline) m_P_S3 -> fill(P);
      if (m_P_occu1[i] >=1) {
        m_P_Shit -> fill(P);
        if (m_notOnline) m_P_S3hit -> fill(P);
      }

      if (range) {
        int stazione = 3;
        m_StationsEff_den -> fill(stazione);
        m_RegionsEff_den  -> fill(regione[2]);
        if (m_P_occu3[i] >=1) {
          m_StationsEff_num -> fill(stazione);
          m_RegionsEff_num  -> fill(regione[2]);
        }
      }
    }

    if (Norma4) {
      m_P_S -> fill(P);
      if (m_notOnline) m_P_S4 -> fill(P);
      if (m_P_occu1[i] >=1) {
        m_P_Shit -> fill(P);
        if (m_notOnline) m_P_S4hit -> fill(P);
      }
      
      if (range) {
        int stazione = 4;
        m_StationsEff_den -> fill(stazione);
        m_RegionsEff_den  -> fill(regione[3]);
        if (m_P_occu4[i] >=1) {
          m_StationsEff_num -> fill(stazione);
          m_RegionsEff_num  -> fill(regione[3]);
        }
      }
    }

    if (Norma5) {
      m_P_S -> fill(P);
      if (m_notOnline) m_P_S5 -> fill(P);
      if (m_P_occu1[i] >=1) {
        m_P_Shit -> fill(P);
        if (m_notOnline) m_P_S5hit -> fill(P);
      }
      
      if (range) {
        int stazione = 5;
        m_StationsEff_den -> fill(stazione);
        m_RegionsEff_den  -> fill(regione[4]);
        if (m_P_occu5[i] >=1) {
          m_StationsEff_num -> fill(stazione);
          m_RegionsEff_num  -> fill(regione[4]);
        }
      }
    }
  }

}
