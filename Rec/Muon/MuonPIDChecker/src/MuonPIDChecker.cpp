// Include files
#include <cmath>
#include <iomanip>


// from Gaudi
#include "GaudiKernel/AlgFactory.h"

#include "Linker/LinkedTo.h"

// local
#include "MuonPIDChecker.h"


//------------------------------------------------------------------
// Implementation file for class : MuonPIDChecker
//
// 25-10-2002 : M. Gandelman, S. Amato, J. de Mello Neto and A. Massafferri
// 08-08-2006 : M. Gandelman, E. Polycarpo
// 12-01-2007 : M. Gandelman, E. Polycarpo
// 15-08-2007 : M. Gandelman
// 20-08-2008 : M. Gandelman, E. Polycarpo
//--------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonPIDChecker );

//====================================================================
// Standard constructor, initializes variables
//=====================================================================

MuonPIDChecker::MuonPIDChecker( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator ) {  
  

  // Source of track to ID
  declareProperty("TrackLocation",
                  m_TracksPath = LHCb::TrackLocation::Default);
  // TES path for hits    
  declareProperty("HitsLocation",
                  m_HitsPath = LHCb::MuonCoordLocation::MuonCoords);
  // Source of MuonPID
  declareProperty("MuonIDLocation",
                  m_MuonPIDsPath = LHCb::MuonPIDLocation::Default);
  // Source of Muon Tracks
  declareProperty("MuonTrackLocation",
                  m_MuonTracksPath = LHCb::TrackLocation::Muon);
  // DLL flag (to decide which DLL to use: 
  // 0=standard, 1=DLL=log(P(mu)/P(non-mu)) with integrated- p binned distance, 2=Prob(mu))  
  declareProperty( "DLLFlag", m_DLLFlag = 1 );
  // Limits for the DLL histos
  declareProperty( "DLLlower", m_DLLlower = -1. ); // -10 for DLLFlag = 0
  declareProperty( "DLLupper", m_DLLupper = 6.0 ); // 10 for DLLFlag = 0
  declareProperty( "DLLnbins", m_DLLnbins = 35 ); // 50 for DLLFlag = 0

  // IsMuon or IsMuonLoose: 0=IsMuonLoose, 1=IsMuon
  declareProperty( "IsMuonFlag", m_IsMuonFlag = 0 );

  // DLL value to define efficiency  
  declareProperty( "DLLCut", m_DLLCut = 1.40 );
  // for now only include 1 cut to Prob too
  declareProperty( "ProbCut1", m_ProbCut1 = 0.90 );
  declareProperty( "ProbCut2", m_ProbCut2 = 0.85 );

  // NShared value to define efficiency  
  declareProperty( "NSHCut", m_NSHCut = 1 );

  // Swap between real and MC data   
  declareProperty( "RunningMC", m_RunningMC = false );
  // Turn expert histos on and off
  declareProperty( "ExpertCheck", m_ExpertChk = false );

  // Reference values for efficiency and misid
  // B->jpsi ks reference numbers
  declareProperty( "RefEff1IM", m_refEff1IM );
  declareProperty( "RefEff1IML", m_refEff1IML );
  declareProperty( "RefEff1DLL", m_refEff1DLL );
  declareProperty( "RefEff1NSH", m_refEff1NSH );
  declareProperty( "RefdEff1IM", m_refdEff1IM );
  declareProperty( "RefdEff1IML", m_refdEff1IML );
  declareProperty( "RefdEff1DLL", m_refdEff1DLL );
  declareProperty( "RefdEff1NSH", m_refdEff1NSH );
  // bbar inclusive reference numbers
  declareProperty( "RefEff2IM", m_refEff2IM );
  declareProperty( "RefEff2IML", m_refEff2IML );
  declareProperty( "RefEff2DLL", m_refEff2DLL );
  declareProperty( "RefEff2NSH", m_refEff2NSH );
  declareProperty( "RefdEff2IM", m_refdEff2IM );
  declareProperty( "RefdEff2IML", m_refdEff2IML );
  declareProperty( "RefdEff2DLL", m_refdEff2DLL );
  declareProperty( "RefdEff2NSH", m_refdEff2NSH );

}

//===================================================================
// Destructor
//====================================================================

MuonPIDChecker::~MuonPIDChecker() {}; 

//====================================================================
// Initialisation. Check parameters
//=====================================================================

StatusCode MuonPIDChecker::initialize() {


 // Sets up various tools and services
  const StatusCode sc = GaudiHistoAlg::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Test if reference values are available

  if (m_refEff1IM.size()  < 5 || m_refEff1IML.size() < 5 ||
      m_refEff1DLL.size() < 5 || m_refEff1NSH.size() < 5 ||  
      m_refEff2IM.size()  < 5 || m_refEff2IML.size() < 5 ||
      m_refEff2DLL.size() < 5 || m_refEff2NSH.size() < 5 )  
    warning()<< "initialize:: reference values are not available " << endreq;
  debug() << "initialize:: size of reference values vectors : "<< m_refEff1IM.size()<< endreq;

  // Get Geometry parameters
  m_NStation = 0;
  m_NRegion = 0;
  MuonBasicGeometry basegeometry( detSvc(),msgSvc());
  m_NStation= basegeometry.getStations();
  m_NRegion = basegeometry.getRegions();
  unsigned int i=0;
  while(i<m_NStation){
    m_stationNames.push_back(basegeometry.getStationName(i));
    debug()   <<"initialize:: station "<<i<<" "<<m_stationNames[i]<<endreq;
    i++;
  }
  m_mudet=getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");
  for(unsigned int station = 0 ; station < m_NStation ; station++ ){
    // z position of each station 
    m_stationZ.push_back(m_mudet->getStationZ(station));
  }

  // Zero Counters (for all events)
  m_neventsTest = 0;
  for (unsigned int i=0; i<5 ; i++){
    m_nTrPreSel[i] = 0;
    m_nTrIM[i] = 0;
    m_nTrIML[i] = 0;
    m_nTrDLL[i] = 0;
    m_nTrNSH[i] = 0;
    if (i<2) m_nTrProb[i]=0;
  }
  // GL&SF: Check that IsMuonFlag has reasonable value:
  if (m_IsMuonFlag==0)info() << "initialize:: ======> Monitor Performance of IsMuonLoose " << endreq;
  if (m_IsMuonFlag==1)info() << "initialize:: ======> Monitor Performance of IsMuon " << endreq;
  if (m_IsMuonFlag<0 || m_IsMuonFlag>1){
    warning() << "initialize:: IsMuonFlag not valid, IsMuonFlag="<< m_IsMuonFlag <<  endreq;
  }
	  
  // GL&SF: Check that DLLFlag has reasonable value:
  if (m_DLLFlag==0)info() << "initialize:: ======> Monitor Performance of old DLL  " << endreq;
  if (m_DLLFlag==1)info() << "initialize:: ======> Monitor Performance of (binned+integrated) DLL " << endreq;
  if (m_DLLFlag==2)info() << "initialize:: ======> Monitor Performance of Prob Mu " << endreq;
  if (m_DLLFlag<0 || m_DLLFlag>2){
    warning() << "initialize::  DLLFlag chosen not valid. DLLFLag = " << m_DLLFlag << endreq;
    warning() << "initialize::  allowed values are = 0 [DLL old], 1 [DLL binned+integrated], 2 [ProbMU] " <<  endreq;
  }

  
  return StatusCode::SUCCESS;
};


//====================================================================
// Main execution
//=====================================================================
StatusCode MuonPIDChecker::execute() {

  m_neventsTest++;    // count events 

  debug()  << "execute:: ==> Number of events: " << m_neventsTest << endreq;

  // MC association 
  LinkedTo<LHCb::MCParticle, LHCb::Track>* myLinkToTrack = NULL;
  if (m_RunningMC) {
   myLinkToTrack = new LinkedTo<LHCb::MCParticle, LHCb::Track>( eventSvc(), 
                                      msgSvc(),   m_TracksPath );
  }

  // get  all info about Tracks and associated MuonPIDs 
  StatusCode scTr = getTrPIDInfo(myLinkToTrack);
  if (scTr.isFailure()){
    warning() << "execute::Failed to get Tracks info " << endreq;
  }
  debug() << "execute::Finish track info retrieval "<< endreq;

  // get  all info about MuonTracks 
  StatusCode scMuTr = getMuTrPIDInfo(myLinkToTrack);
  if (scMuTr.isFailure()){
    warning() << "execute::Failed to get MuonTracks info " << endreq;
  }

  debug() << "execute::Finish muon track info retrieval "<< endreq;

  // delete myLinToTrack object if created (RunningMC == true)
  if ( NULL != myLinkToTrack ) delete myLinkToTrack;

  return StatusCode::SUCCESS;
}
//=====================================================================
//  Get All Track and MuonPID Info 
//====================================================================
StatusCode MuonPIDChecker::getTrPIDInfo(LinkedTo<LHCb::MCParticle, LHCb::Track>* LinkToTrack){ 

  // Get tracks to loop over
  LHCb::Tracks* trTracks = get<LHCb::Tracks>(m_TracksPath);
  if (!trTracks){
    error() << "getTrPIDInfo:: Failed to get Track container "
        <<  m_TracksPath << endreq;
    return StatusCode::FAILURE;
  }
  
  // get  MuonPID objects 
  LHCb::MuonPIDs* pMuids=get<LHCb::MuonPIDs>(m_MuonPIDsPath);
  if (!pMuids){
    error() << "getTrPIDInfo::  Failed to get MuonPID objects in "
        <<  m_MuonPIDsPath << endreq;
    return StatusCode::FAILURE;
  }

  // Zero Track multiplicities after each selection step (per event)
  unsigned int nTr = 0;
  unsigned int nPSTr = 0;
  unsigned int nIMTr = 0;
  unsigned int nIMLTr = 0;
  unsigned int nPSGhosts = 0;
  unsigned int nIMGhosts = 0;
  unsigned int nIMLGhosts = 0;
  std::vector<unsigned int> nPStracks(5);

  debug()<< "getTrPIDInfo:: Finish getting objects, start loop over tracks"<< endreq;
  LHCb::Tracks::const_iterator iTrack;
  for (iTrack = trTracks->begin() ; iTrack != trTracks->end() ; iTrack++){

    if(!(*iTrack)->checkFlag(LHCb::Track::Clone)  &&
       ((*iTrack)->checkType(LHCb::Track::Long) ||
                          (*iTrack)->checkType(LHCb::Track::Downstream))){

      const LHCb::Track *pTrack = *iTrack;
      // get state in zero position 
      const LHCb::State * stateP0 = &(pTrack->firstState());
      if (!stateP0){
	warning() << "getTrPIDInfo:: Failed to get stateP0 for track " << nTr+1 <<" !!! " << endreq;
	continue;
      }
      nTr++;

      resetTrInfo();

      unsigned int isPs=0;
      unsigned int nMuonPID=0;
      // link between track and MuonPID 
      LHCb::MuonPIDs::const_iterator imuid;
      for (imuid = pMuids->begin() ; imuid != pMuids->end() ; imuid++){
	if ((*imuid)->idTrack() == *iTrack){  // found Associated MuonPID
	  nMuonPID++;
	  // Preselection
	  if ((*imuid)->PreSelMomentum() && (*imuid)->InAcceptance() ){
	    isPs=1;
	    m_TrIsMuon = (*imuid)->IsMuon();
	    m_TrIsMuonLoose = (*imuid)->IsMuonLoose();
	    if (m_TrIsMuonLoose< m_TrIsMuon) 
	      warning() << "getTrPIDInfo:: Muon Track" << nTr<<"  IsMuon ="<< m_TrIsMuon << 
		                              "> IsMuonLoose= "<< m_TrIsMuonLoose << endreq;
	    m_TrMuonLhd= (*imuid)->MuonLLMu();
	    m_TrNMuonLhd = (*imuid)->MuonLLBg();
	    m_TrNShared = (*imuid)->nShared();
	  } // loop over MuonPIDs
	} // Pre-selection
      } // Association to Track
      if (nMuonPID>1) warning() <<"getTrPIDInfo:: #of MuonID assoc to Track " << nTr<<" > 1!!!" << endreq;
      debug() << "getTrPIDInfo:: MuonPID retrieved "<< nTr <<endreq;
     
      // Get info only for PS tracks 
      if (isPs>0) {

	if( std::abs( stateP0->qOverP() ) > 0.001 / Gaudi::Units::GeV ) {
	  m_Trp0 = (1./stateP0->qOverP())/Gaudi::Units::GeV; 
	}else if( stateP0->qOverP() > 0. ) {
	  m_Trp0 = 1000. / Gaudi::Units::GeV;
	}else {
	  m_Trp0 = -1000. / Gaudi::Units::GeV;
	}


	m_TrType = 4; 
	// Retrieve track type (MC)
	if (m_RunningMC) m_TrType = getTrType( pTrack, LinkToTrack ); 
	debug() << "getTrPIDInfo:: MC info retrieved for track "<< nTr <<" = " << m_TrType <<endreq;

	if (m_TrType == 0 ) nPSGhosts++;
	nPSTr++;
	nPStracks[m_TrType]++;
	m_nTrPreSel[m_TrType]++;

	if (m_RunningMC && (nPSTr< (nPStracks[0]+nPStracks[1]+nPStracks[2]+nPStracks[3]))) 
	    warning() << "getTrPIDInfo:: MC association failed nTr< ghosts+ided tracks !!!" << endreq;
	// Fill Track Info
	fillTrHistos(0);
	debug() << "getTrPIDInfo:: PS track Histos filled "<< nTr << endreq;

	char hnameEff[10];
	// IsMuonLoose Efficiency vs p 
	sprintf ( hnameEff, "Eff_IML_%d", m_TrType);
	profile1D( m_Trp0, m_TrIsMuonLoose, hnameEff, "Eff. versus Momentum(GeV)", -200., 200., 400 );

	// IsMuon Efficiency vs p 
	sprintf ( hnameEff, "Eff_IM_%d", m_TrType);
	profile1D( m_Trp0, m_TrIsMuon, hnameEff, "Eff. versus Momentum(GeV)", -200., 200., 400 );
	debug() << "getTrPIDInfo:: Efficiency  Histos filled for "<< nTr <<endreq;

	if (m_TrIsMuonLoose > 0) {
	  fillTrHistos(1);
	  debug() << "getTrPIDInfo:: IML track Histos filled for Track "<< nTr << endreq;
	  nIMLTr++;
	  if (m_TrType == 0 ) nIMLGhosts++;
	  if (m_TrIsMuon > 0) {
	    fillTrHistos(2);
	    debug() << "IM track Histos filled for Track "<< nTr << endreq;
	    nIMTr++;
	    if (m_TrType == 0 ) nIMGhosts++;
	  }
	}
      } // ony for PS tracks
    } // long and Matched tracks
  } // loop over tracks 

  // Plot track multiplicity
  plot1D(nTr,   "hNtracks",  "Track multiplicity", -0.5, 199.5, 200);
  plot1D(nPSTr, "hNPStracks","Pre-selected Track multiplicity", -0.5, 199.5, 200);
  plot1D(nIMTr, "hNIMtracks","IsMuon=1 Track multiplicity", -0.5, 19.5, 20);
  plot1D(nIMLTr, "hNIMLtracks","IsMuonLoose=1 Track multiplicity", -0.5, 19.5, 20);
  if (m_RunningMC) {
    plot1D(nPSGhosts, "hNPSGhosts","Pre-selected ghost multiplicity", -0.5, 199.5, 200);
    plot1D(nIMGhosts, "hNIMGhosts","IsMuon=1 ghost multiplicity", -0.5, 19.5, 20);
    plot1D(nIMLGhosts, "hNIMGhosts","IsMuonLoose=1 ghost multiplicity", -0.5, 19.5, 20);
  }

  return StatusCode::SUCCESS;
}
//=====================================================================
//  Fill all MuonTrack info
//====================================================================
StatusCode MuonPIDChecker::getMuTrPIDInfo(LinkedTo<LHCb::MCParticle, LHCb::Track>* LinkToTrack){ 

  // Get muon tracks to loop over
  LHCb::Tracks* muTracks = get<LHCb::Tracks>(m_MuonTracksPath);
  if (!muTracks){
    error() << "getMuTrPIDInfo:: Failed to get muon Track container "
        <<  m_MuonTracksPath << endreq;
    return StatusCode::FAILURE;
  }

  // counters to be compared to MuonPID info
  unsigned int nTr=0;
  unsigned int nPSTr=0;
  unsigned int nIMTr=0;
  unsigned int nIMLTr=0;

  debug() << "getMuTrPIDInfo:: Loop over Muon Tracks " << endreq;
  LHCb::Tracks::const_iterator imuTrack;
  std::vector<int> assocHits(m_NRegion*m_NStation);
  for (imuTrack = muTracks->begin() ; imuTrack != muTracks->end() ; imuTrack++){
    nTr++;
    resetMuTrInfo();
    // Muon Track Info
    double muTrPS = 0;
    if ( (*imuTrack)->info(301,0) && (*imuTrack)->info(302,0)) muTrPS=1.; 
    m_Trquality = (*imuTrack)->info(300,0);
    m_TrIsMuonLoose = (unsigned int)(*imuTrack)->info(303,0);
    m_TrIsMuon = (unsigned int)(*imuTrack)->info(304,0);
    m_TrChi2 = (*imuTrack)->chi2PerDoF(); 
    m_TrDist2 = (*imuTrack)->info(305,0);
    m_TrCLquality = (*imuTrack)->info(308,0);
    m_TrCLarrival = (*imuTrack)->info(309,0);
    if (muTrPS>0 ) nPSTr++;
    if (m_TrIsMuonLoose>0 ) nIMLTr++;
    if (m_TrIsMuon>0 ) nIMTr++;
    if (m_TrIsMuonLoose < m_TrIsMuon) 
      warning() << "getMuTrPIDInfo:: Muon Track IsMuon ="<< m_TrIsMuon
	      << "> IsMuonLoose= "<< m_TrIsMuonLoose<< endreq;
    debug() << "getMuTrPIDInfo:: Muon Track  quality and Dist2: "<< m_Trquality << " " 
              <<  m_TrDist2 << endreq;
    debug() << "getMuTrPIDInfo:: Muon Track  CLquality and CLArrival: "<< m_TrCLquality << " " 
              <<  m_TrCLarrival << endreq;
    debug() << "getMuTrPIDInfo:: DLL and NShared: "<< (*imuTrack)->info(306,0) << " " 
              << (*imuTrack)->info(307,0) << endreq;
    
    
    // Get Track ancestors
    const LHCb::Track *trParent=NULL;
    int nmothers=0;
    const SmartRefVector<LHCb::Track> Trmothers=(*imuTrack)->ancestors(); 
    for (SmartRefVector<LHCb::Track>::const_iterator imother=Trmothers.begin();
	  imother!=Trmothers.end();imother++){
        trParent = *imother;
        nmothers++;
    }	
    if (trParent == NULL) warning()<< "getMuTrPIDInfo:: failed to get Mu Track ancestor" << endreq;
    const LHCb::Track *pTrack = trParent;
    debug()<< "getMuTrPIDInfo:: # Mu Track ancestors = "<< nmothers<< endreq; 
    
    // Get Track Type according to MC (0-3) or data (4)
    m_TrType=4;
    if (m_RunningMC && pTrack!=NULL) m_TrType=getTrType(pTrack, LinkToTrack);
    debug()<< "getMuTrPIDInfo:: MuonTrack Type = "<< m_TrType << endreq; 
    
    // extrapolates track to Muon Stations
    StatusCode sc = trackExtrapolate(pTrack);
    if (sc.isFailure()){
	warning() << "getTrPIDInfo:: Failed to extrapolate track " << nTr << endreq;
    }

    int ismuon= m_TrIsMuonLoose;
    if (m_IsMuonFlag>0) ismuon= m_TrIsMuon;
    if (ismuon>0 ){
      char hname[15];
      debug()<< " muTrchi2= " << m_TrChi2;
      sprintf ( hname, "hchi2_%d", m_TrType);
      plot1D(m_TrChi2, hname ,"Chi2 per nDoF", 0., 200., 100);
      debug()<< " muTrDist= " << m_TrDist2;
      sprintf ( hname, "hDist2_%d", m_TrType);
      plot1D(m_TrDist2, hname ,"Average Square Distance", 0., 600., 100);
      if (m_ExpertChk){
	debug()<< " muTrquality= " << m_Trquality;
	sprintf ( hname, "hquality_%d", m_TrType);
	plot1D(m_Trquality, hname ,"Track Quality", 0., 200., 100);
	debug()<< " muClQUal= " << m_TrCLquality;
	sprintf ( hname, "hCLQual_%d", m_TrType);
	plot1D(m_TrCLquality, hname,"Chi2 CL", 0., 1., 50);
	debug()<< " muClArrival= " << m_TrCLarrival;
	sprintf ( hname, "hCLArrival_%d", m_TrType);
	plot1D(m_TrCLarrival, hname ,"Arrival CL", 0., 1., 50);
      }
    }
    // Look at track multiplicities
    std::vector<LHCb::LHCbID> mucoords = (*imuTrack) -> lhcbIDs();
    std::vector<LHCb::LHCbID>::iterator iID;
    std::vector<unsigned int>  nhitsfoi(m_NRegion*m_NStation);
    for (iID = mucoords.begin(); iID != mucoords.end(); iID++) {
      if (!(iID->isMuon())) continue;

      LHCb::MuonTileID mutile = iID->muonID();
      int region = mutile.region();
      int station = mutile.station();
      int nStatReg = station*m_NRegion+region;
      nhitsfoi[nStatReg]++;
      assocHits[nStatReg]++;

    } //end of loop over lhcbIDs 
    
    // Find region hit by the track 
    // get number of hits per station
    int TrackRegion[5]={-1,-1,-1,-1,-1};
    std::vector<unsigned int>  nhitsfoiS(m_NStation);
    for (unsigned int i=0; i<m_NStation; i++){
      TrackRegion[i] = findTrackRegion(i);
      // Add hits in different regions and 
      for (unsigned int j=0; j<m_NRegion;j++){
	debug() <<"getMuTrPIDInfo:: nhitsfoi  " << nhitsfoi[i*m_NRegion+j];
	debug() << " R" << i<< " M"<< j <<endreq;
	nhitsfoiS[i]+=nhitsfoi[i*m_NRegion+j];  
      }
    }
    // Fill hit multiplicity histos for each station
    if ( m_ExpertChk) {
      for (unsigned int i=0; i<m_NStation; i++){
	 
	debug()<< "getMuTrPIDInfo:: M" <<i<<" Track Region and nhitsfoiS  " << TrackRegion[i] <<" "<< nhitsfoiS[i]<< endreq;
	
	// Hit Multiplicity (within FOI) as a function of the Muon regions
	char hmult[15];
	sprintf (hmult,"hNhitsFOIReg_%d",m_TrType);
	plot2D(nhitsfoiS[i],i*m_NRegion+TrackRegion[i],
	    hmult, "Hits in FOI per region (per track)", -0.5, 9.5,
	      -0.5, m_NRegion*m_NStation-0.5, 10, m_NRegion*m_NStation); 
	sprintf (hmult,"hPNhitsFOIReg_%d",m_TrType);
	profile1D ( i*m_NRegion+TrackRegion[i], nhitsfoiS[i], hmult, "Hits in FOI per region (per track)",
	    -0.5, m_NRegion*m_NStation-0.5, m_NRegion*m_NStation); 
	   
	// Hit Multiplicity (within FOI) as a function of Track X,Y 
	char hname[15]; 
	sprintf ( hname, "hNhitsFOIvsX_M%d_%d", i+1, m_TrType);
	plot2D(nhitsfoiS[i],m_trackX[i],
	    hname, "Hits in FOI vs X (per track)", -0.5, 9.5, -5500, 5500, 10, 100);
	sprintf ( hname, "hNhitsFOIvsY_M%d_%d", i+1, m_TrType);
	plot2D(nhitsfoiS[i],m_trackY[i],
	    hname, "Hits in FOI vs Y (per track)", -0.5, 9.5, -5000, 5000, 10, 100);
	sprintf ( hname, "PNhitsFOIvsX_M%d_%d", i+1, m_TrType);
	profile1D( m_trackX[i], nhitsfoiS[i], hname, "Hits in FOI vs X (per track)", -5500, 5500, 100); 
	sprintf ( hname, "PNhitsFOIvsY_M%d_%d", i+1, m_TrType);
	profile1D( m_trackY[i], nhitsfoiS[i], hname, "Hits in FOI vs Y (per track)", -5500, 5500, 100); 
      }
    }

  } //end of muTrack loop

  // Histos to be compared to the ones filled with MuonPID info
  if (m_ExpertChk){
    plot1D(nTr,   "hNMutracks", "Muon Track multiplicity", -0.5, 199.5, 200);
    plot1D(nPSTr, "hNPSMutracks","Pre-selected Muon Track multiplicity", -0.5, 199.5, 200);
    plot1D(nIMLTr, "hNIMLMutracks","IsMuonLoose=1 Muon Track multiplicity", -0.5, 19.5, 20);
    plot1D(nIMTr, "hNIMMutracks","IsMuon=1 Muon Track multiplicity", -0.5, 19.5, 20);
  }
   // number of muon hits used for identified muons (can have double counting due to hit sharing)
  for (unsigned int i=0; i<m_NRegion*m_NStation; i++){
    profile1D ( i, assocHits[i], "hNhitsAssReg", "Hits in FOI per region",
	-0.5, m_NRegion*m_NStation-0.5, m_NRegion*m_NStation);
  }
  debug()<< "getMuTrPIDInfo:: finish getMuTrPIDInfo " << endreq;

  return StatusCode::SUCCESS;
}
//=====================================================================
//  Fill track info
//====================================================================
void MuonPIDChecker::fillTrHistos(const int Level){ 
  // Level 0 => preselection
  // Level 1 => IsMuonLoose = 1  
  // Level 2 => IsMuon = 1  
  char hname[10] ;

  if (Level==0) {            
    if (m_TrIsMuon > m_TrIsMuonLoose) warning() << 
	    " Track isMuon but not isMuonLoose"<< endreq; 
    sprintf ( hname, "Mom_PS_%d", m_TrType);
  } else if (Level==1){
    m_nTrIML[m_TrType]++;
    sprintf ( hname, "Mom_IML_%d", m_TrType);
  }else{
    m_nTrIM[m_TrType]++;
    sprintf ( hname, "Mom_IM_%d", m_TrType);
  }
  plot1D( m_Trp0, hname , " Momentum (GeV) ", -200. , 200. , 400 ); 

  if (Level<2){
      sprintf ( hname, "hIM_%d_%d", Level, m_TrType);
      plot1D( m_TrIsMuon, hname , " IsMuon ", -0.5 , 1.5, 2 ); 
  }

  if (Level>0){               // IsMuon or IsMuonLoose
    double DLL = -999;
    if (m_DLLFlag<2) DLL =m_TrMuonLhd-m_TrNMuonLhd;
    if (m_DLLFlag==2) DLL =exp(m_TrMuonLhd);

    if ((m_IsMuonFlag ==0 && Level==1) || (m_IsMuonFlag ==1 && Level==2)) {

      if ( m_TrNShared < m_NSHCut) m_nTrNSH[m_TrType]++;
      sprintf ( hname, "hNShared_%d", m_TrType);
      plot1D( m_TrNShared, hname , "Tracks sharing hits", -0.5, 9.5, 10 ); 

      // Standard DLL
      // prepare to fill Eff and Misid vs DLL cut histos
      sprintf ( hname, "hDLLeff_%d", m_TrType);
      double dll_binw = (m_DLLupper - m_DLLlower)/50.;
      if ( m_DLLFlag<2 ) {
	if ( DLL > m_DLLCut) m_nTrDLL[m_TrType]++;
	for (unsigned int idllbin=0;idllbin<50;idllbin++){
	  if (DLL> (idllbin*dll_binw) ) {
	    profile1D((idllbin+0.5)*dll_binw,1,hname,"Eff vs DLL cut", m_DLLlower, m_DLLupper, 50);
	  }else{
	    profile1D((idllbin+0.5)*dll_binw,0,hname,"Eff vs DLL cut", m_DLLlower, m_DLLupper, 50);
	  }
	}
      }else {
	  if ( DLL < m_DLLCut) m_nTrDLL[m_TrType]++;
	  if ( DLL < m_ProbCut1 && m_TrType==1 ) m_nTrProb[0]++;
	  if ( DLL < m_ProbCut2 && m_TrType==1 ) m_nTrProb[1]++;
	  for (unsigned int idllbin=0;idllbin<50;idllbin++){
	    if (DLL< (idllbin*dll_binw) ) {
	      profile1D((idllbin+0.5)*dll_binw,1,hname,"Eff vs DLL cut", m_DLLlower, m_DLLupper, 50);
	    }else{      
	      profile1D((idllbin+0.5)*dll_binw,0,hname,"Eff vs DLL cut", m_DLLlower, m_DLLupper, 50);
	    }
	  }
      }
      
      sprintf ( hname, "hDLL_%d", m_TrType);
      plot1D( DLL  ,hname, hname , m_DLLlower, m_DLLupper, m_DLLnbins ); 
      //GL&SF: Prob Mu:
      sprintf ( hname, "hProbMu_%d", m_TrType);
      plot1D( exp(m_TrMuonLhd) ,hname, "ProbMu", -0.1, 1.1, 60 );
      sprintf ( hname, "Pmu_vs_p_%d", m_TrType);
      profile1D( m_Trp0, exp(m_TrMuonLhd), hname, "ProbMu_vs_p", 0., 80., 40);

      //GL&SF: Prob NonMu:
      sprintf ( hname, "hProbNMu_%d", m_TrType);
      plot1D( exp(m_TrNMuonLhd) ,hname, "ProbNonMu", -0.1, 1.1, 60 );
      sprintf ( hname, "Pnmu_vs_p_%d", m_TrType);
      profile1D( m_Trp0, exp(m_TrNMuonLhd), hname, "ProbNMu_vs_p", 0.,80.,40);
    }
  }

  return;
}
//======================================================================
// Extrapolate Tracks to get X,Y of extrapolation point in each station 
//======================================================================
StatusCode MuonPIDChecker::trackExtrapolate(const LHCb::Track *pTrack){

    // get state closest to M1 for extrapolation
    const LHCb::State * state = &(pTrack->closestState(9450));
    
    if(!state){
      err() << " Failed to get state from track " << endreq;
      return StatusCode::FAILURE;
    }
    
   //Project the state into the muon stations
   unsigned int station;
   for(station = 0; station < m_NStation ; station++){
      // x(z') = x(z) + (dx/dz * (z' - z))
      m_trackX.push_back(state->x() + ( state->tx() *
			                (m_stationZ[station] - state->z()) ));
      
      m_trackY.push_back(state->y() + ( state->ty() *
			                (m_stationZ[station] - state->z()) ));
   }
   
   return StatusCode::SUCCESS;
}
   
//=====================================================================
//  MC association and info
//====================================================================
StatusCode MuonPIDChecker::checkMCAss(const LHCb::Track *pTrack, 
                                      const LHCb::MCParticle* mcP){ 

  if (!mcP){
    debug() <<"checkMCAssoc:: Tr2MCTable failed " << endreq;
    return StatusCode::FAILURE;
  } 
  else {
    // get z of track end vertex if a muon is produced there 
    SmartRefVector< LHCb::MCVertex >::const_iterator iVtx;
    if (0==mcP->endVertices().size()){
      debug() << "checkMCAssoc:: No end Vertex for track " << *pTrack << endreq;
    } else {

      for (iVtx = mcP->endVertices().begin();
	  iVtx != mcP->endVertices().end() ;
	  iVtx++){
	SmartRefVector<LHCb::MCParticle>::const_iterator it;
	for ( it= (*iVtx)->products().begin();it !=
	    (*iVtx)->products().end(); it++ ){ 
	  if ( abs((*it)->particleID().pid()) == 13) {  

	    m_TrzDecay = (*iVtx)->position().z()/Gaudi::Units::cm;

	  } // muon is a daughter
	} // loop over the daughters  
      } // loop over end vertices  
    } // there are end vertices 

    // there is MCParticle associated to Track
    return StatusCode::SUCCESS;
  } 
}
//=====================================================================
//  Reset Track Variables  
//====================================================================
void MuonPIDChecker::resetTrInfo() {
    m_TrNShared=1000;
    m_TrMuonLhd= -1000.;
    m_TrNMuonLhd = -1000.;
    m_TrIsMuon = 1000;
    m_TrIsMuonLoose = 1000;
    m_TrnLinks = 0;
    m_TrzDecay = -1000.;
    m_TrType = 1000;
    m_Trp0 = -1000.;
    m_TrMCp0 = -1000;

    return;
}
//=====================================================================
//  Reset Muon Track Variables  
//====================================================================
void MuonPIDChecker::resetMuTrInfo() {
    m_TrIsMuon = 1000;
    m_TrIsMuonLoose = 1000;
    m_TrnLinks = 0;
    m_TrzDecay = -1000.;
    m_TrType = 1000;
    m_TrChi2 =-1000;
    m_TrDist2 =-1000;
    m_TrCLquality =-1000;
    m_TrCLarrival =-1000;
    m_Trquality =-1000;
    m_trackX.clear();
    m_trackY.clear();

    return;
}
//=====================================================================
//  Return Track Type     
//  0 = ghost
//  1 = muon from origin
//  2 = muon from decay/interaction
//  3 = non-muon
//  4 = real data
//====================================================================
int MuonPIDChecker::getTrType( const LHCb::Track *pTrack, 
          LinkedTo<LHCb::MCParticle, LHCb::Track>* LinkToTrack ){

  // start as unknown data track
  int type = 4;

  // MCParticle Association 
  if (m_RunningMC) {
    LHCb::MCParticle* mcP = LinkToTrack->first(pTrack);
    LHCb::MCParticle* mcPnext = mcP;
    while ( mcPnext != NULL ){
      m_TrnLinks++;
      if ( abs(mcPnext->particleID().pid()) == 13 ) mcP = mcPnext;
      mcPnext = LinkToTrack->next();
    }
    const LHCb::MCParticle* finalmcP= mcP;
    StatusCode sc = checkMCAss( pTrack , finalmcP);
    debug() << "getTrType:: MC assoc checked : sc = "<< sc << endreq;

    // Get type for histos and counters
    if ( sc.isFailure() ) {      

      debug() << "getTrType::MC assoc is failure " << endreq;
      type = 0;                        // ghost 

    }else{                             // there is MC association

      m_TrMCp0 = mcP->p()/Gaudi::Units::GeV;            // true momentum 
      if( abs( mcP->particleID().pid() ) == 13 ) {
	    if (m_TrnLinks>1) {
	       type=2;
	    }else if ( m_TrnLinks==1 && mcP->mother()!= NULL){
	      if ( abs(mcP->mother()->particleID().pid()) == 211 || abs( mcP->mother()->particleID().pid() ) == 321 ){
	       type=2;
	      }else{
	       type=1;
	      }
	    } else if ( m_TrnLinks==1 ){
	      type=1;
	    }
      } else if ( abs( mcP->particleID().pid() ) != 13 && (m_TrzDecay > 0  && m_TrzDecay<20000.) ){
	type=2;
      } else {
	type=3;
      }  
      debug() << "getTrType::MCID,MotherMCID,zDecay,type "<<  finalmcP->particleID().pid();
      if (finalmcP->mother()!=0) debug()<< " " << finalmcP->mother()->particleID().pid();
      debug() << " "<< m_TrzDecay << " " << type << " nlinks=" << m_TrnLinks<< endreq;
    } // ghost or MC associated  
  } // Running MC
  debug() << "getTrType::type "<<  type<< endreq;
  return type;
}
//=====================================================================
//  Finalize
//====================================================================
StatusCode MuonPIDChecker::finalize() {

  double Ef1[5]={-1.};
  double Ef2[5]={-1.};
  double Ef3[5]={-1.};
  double Ef4[5]={-1.};
  double EfProb[2]={-1.};
  double dEf1[5]={-1.};
  double dEf2[5]={-1.};
  double dEf3[5]={-1.};
  double dEf4[5]={-1.};
  double dEfProb[2]={-1.};
  // MC total Misid ( non-muon + decays )
  double misIDIM=-1, misIDIML=-1, misIDDLL=-1, misIDNSH=-1;
  double dmisIDIM=-1, dmisIDIML=-1, dmisIDDLL=-1, dmisIDNSH=-1;

  // Add all track types to find total rates in case of MC
  if (m_RunningMC) {
	  m_nTrPreSel[4]=m_nTrPreSel[0]+m_nTrPreSel[1]+m_nTrPreSel[2]+m_nTrPreSel[3];
	  m_nTrIM[4]=m_nTrIM[0]+m_nTrIM[1]+m_nTrIM[2]+m_nTrIM[3];
	  m_nTrIML[4]=m_nTrIML[0]+m_nTrIML[1]+m_nTrIML[2]+m_nTrIML[3];
	  m_nTrDLL[4]=m_nTrDLL[0]+m_nTrDLL[1]+m_nTrDLL[2]+m_nTrDLL[3];
	  m_nTrNSH[4]=m_nTrNSH[0]+m_nTrNSH[1]+m_nTrNSH[2]+m_nTrNSH[3];
  }

  // Calculate ID Rates for all tracks types and criteria
  for (unsigned int ityp = 0; ityp < 5; ityp++){
      if( 0 < m_nTrPreSel[ityp] ) {
	Ef1[ityp] = 100.*m_nTrIML[ityp]/m_nTrPreSel[ityp];
	Ef2[ityp] = 100.*m_nTrIM[ityp]/m_nTrPreSel[ityp];
        Ef3[ityp] = 100.*m_nTrDLL[ityp]/m_nTrPreSel[ityp];
        Ef4[ityp] = 100.*m_nTrNSH[ityp]/m_nTrPreSel[ityp];
	// Errors
	dEf1[ityp] = sqrt(Ef1[ityp]*(100.-Ef1[ityp])/m_nTrPreSel[ityp]);
	dEf2[ityp] = sqrt(Ef2[ityp]*(100.-Ef2[ityp])/m_nTrPreSel[ityp]);
	dEf3[ityp] = sqrt(Ef3[ityp]*(100.-Ef3[ityp])/m_nTrPreSel[ityp]);
	dEf4[ityp] = sqrt(Ef4[ityp]*(100.-Ef4[ityp])/m_nTrPreSel[ityp]);
      }
  }
  // Prob efficiency only for muons
  int nismuon = m_nTrIML[1];
  if (m_IsMuonFlag>0) nismuon = m_nTrIM[1]; 
  if (0< nismuon) {
    EfProb[0] = 100.*m_nTrProb[0]/nismuon;
    dEfProb[0] =sqrt(EfProb[0]*(100.-EfProb[0])/nismuon); 
    EfProb[1] = 100.*m_nTrProb[1]/nismuon;
    dEfProb[1] =sqrt(EfProb[1]*(100.-EfProb[1])/nismuon); 
  }

  unsigned int sumPresel = m_nTrPreSel[2]+m_nTrPreSel[3];
  if( 0 < sumPresel ) {
      misIDIML = 100. * (double) (m_nTrIML[2]+m_nTrIML[3])/(double)sumPresel;
      misIDIM  = 100. * (double) (m_nTrIM[2] + m_nTrIM[3])/(double)sumPresel;
      misIDDLL = 100. * (double) (m_nTrDLL[2]+m_nTrDLL[3])/(double)sumPresel;
      misIDNSH = 100. * (double) (m_nTrNSH[2]+m_nTrNSH[3])/(double)sumPresel;
      //Errors
      dmisIDIML = sqrt(misIDIML*(100.-misIDIML)/(double)sumPresel);
      dmisIDIM  = sqrt(misIDIM*(100.-misIDIM)/(double)sumPresel);
      dmisIDDLL = sqrt(misIDDLL*(100.-misIDDLL)/(double)sumPresel);
      dmisIDNSH = sqrt(misIDNSH*(100.-misIDNSH)/(double)sumPresel);
  }

  info()<<"------------------------Total MuonPID Efficiencies (%)----------------------"
        <<  endreq;
  info()<<"----------------------------------------------------------------------------"
        <<  endreq;
  info()<<" Particle   :  These Events  :               Reference values   " <<  endreq;
  info()<<"                             :        J/PsiKs        -       bb inclusive "
        <<  endreq;
  info()<<"----------------------------------------------------------------------------"
        <<  endreq;
  info()<<"---------------------            IsMuonLoose           ---------------------"
        <<  endreq;
  info()<<"----------------------------------------------------------------------------"
        <<  endreq;
  if (m_RunningMC) {
  info()<<" Muons      :   "<<  format( "(%6.2f +-%6.2f) ", Ef1[1], dEf1[1] ) << 
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1IML[1], m_refdEff1IML[1])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2IML[1], m_refdEff2IML[1])<<endreq;
  info()<<" Decays     :   "<<  format( "(%6.2f +-%6.2f) ", Ef1[2], dEf1[2] ) << 
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1IML[2], m_refdEff1IML[2])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2IML[2], m_refdEff2IML[2])<<endreq;
  info()<<" Non-muons  :   "<<  format( "(%6.2f +-%6.2f) ", Ef1[3], dEf1[3] ) <<   
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1IML[3], m_refdEff1IML[3])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2IML[3], m_refdEff2IML[3])<<endreq;
  info()<<" MC MisID   :   "<<  format( "(%6.2f +-%6.2f) ", misIDIML, dmisIDIML ) <<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1IML[5], m_refdEff1IML[5])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2IML[5], m_refdEff2IML[5])<<endreq;
  info()<<" Ghosts     :   "<<  format( "(%6.2f +-%6.2f) ", Ef1[0] , dEf1[0] ) << 
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1IML[0], m_refdEff1IML[0])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2IML[0], m_refdEff2IML[0])<<endreq;
  }
  info()<<"Tot ID Rate :   "<<  format( "(%6.2f +-%6.2f) ", Ef1[4], dEf1[4] ) << 
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1IML[4], m_refdEff1IML[4])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2IML[4], m_refdEff2IML[4])<<endreq;
  info()<<"----------------------------------------------------------------------------"
        <<  endreq;
  info()<<"--------------------               IsMuon               --------------------"
        <<  endreq;
  info()<<"----------------------------------------------------------------------------"
        <<  endreq;
  if (m_RunningMC) {
  info()<<" Muons      :   "<<  format( "(%6.2f +-%6.2f) ", Ef2[1], dEf2[1] ) << 
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1IM[1], m_refdEff1IM[1])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2IM[1], m_refdEff2IM[1])<<endreq;
  info()<<" Decays     :   "<<  format( "(%6.2f +-%6.2f) ", Ef2[2], dEf2[2] )<< 
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1IM[2], m_refdEff1IM[2])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2IM[2], m_refdEff2IM[2])<<endreq;
  info()<<" Non-muons  :   "<<  format( "(%6.2f +-%6.2f) ", Ef2[3], dEf2[3] ) << 
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1IM[3], m_refdEff1IM[3])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2IM[3], m_refdEff2IM[3])<<endreq;
  info()<<" MC MisID   :   "<<  format( "(%6.2f +-%6.2f) ", misIDIM, dmisIDIM ) <<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1IM[5], m_refdEff1IM[5])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2IM[5], m_refdEff2IM[5])<<endreq;
  info()<<" Ghosts     :   "<<  format( "(%6.2f +-%6.2f) ", Ef2[0], dEf2[0] ) << 
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1IM[0], m_refdEff1IM[0])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2IM[0], m_refdEff2IM[0])<<endreq;
  }
  info()<<"Tot ID Rate :   "<<  format( "(%6.2f +-%6.2f) ", Ef2[4], dEf2[4] ) << 
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1IM[4], m_refdEff1IM[4])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2IM[4], m_refdEff2IM[4])<<endreq;
  info()<<"----------------------------------------------------------------------------"
        <<  endreq;
  info()<<"----------------   DLL performance for DLLCut= "<< m_DLLCut<<"   --------------"
        <<  endreq;
  info()<<"----------------------------------------------------------------------------"
        <<  endreq;
  if (m_RunningMC) {
  info()<<" Muons      :   "<<  format( "(%6.2f +-%6.2f) ", Ef3[1], dEf3[1] ) << 
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1DLL[1], m_refdEff1DLL[1])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2DLL[1], m_refdEff2DLL[1])<<endreq;
  info()<<" Decays     :   "<<  format( "(%6.2f +-%6.2f) ", Ef3[2], dEf3[2] )<< 
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1DLL[2], m_refdEff1DLL[2])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2DLL[2], m_refdEff2DLL[2])<<endreq;
  info()<<" Non-muons  :   "<<  format( "(%6.2f +-%6.2f) ", Ef3[3], dEf3[3] ) << 
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1DLL[3], m_refdEff1DLL[3])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2DLL[3], m_refdEff2DLL[3])<<endreq;
  info()<<" MC MisID   :   "<<  format( "(%6.2f +-%6.2f) ", misIDDLL, dmisIDDLL ) <<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1DLL[5], m_refdEff1DLL[5])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2DLL[5], m_refdEff2DLL[5])<<endreq;
  info()<<" Ghosts     :   "<<  format( "(%6.2f +-%6.2f) ", Ef3[0], dEf3[0] ) << 
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1DLL[0], m_refdEff1DLL[0])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2DLL[0], m_refdEff2DLL[0])<<endreq;
  }
  info()<<"Tot ID Rate :   "<<  format( "(%6.2f +-%6.2f) ", Ef3[4], dEf3[4] ) << 
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1DLL[4], m_refdEff1DLL[4])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2DLL[4], m_refdEff2DLL[4])<<endreq;
  info()<<"----------------------------------------------------------------------------"
        <<  endreq;
  info()<<"------------------------      nShared performance    -----------------------"
        <<  endreq;
  info()<<"----------------------------------------------------------------------------"
        <<  endreq;
  if (m_RunningMC) {
  info()<<" Muons      :   "<<  format( "(%6.2f +-%6.2f) ", Ef4[1], dEf4[1] ) << 
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1NSH[1], m_refdEff1NSH[1])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2NSH[1], m_refdEff2NSH[1])<<endreq;
  info()<<" Decays     :   "<<  format( "(%6.2f +-%6.2f) ", Ef4[2], dEf4[2] )<< 
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1NSH[2], m_refdEff1NSH[2])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2NSH[2], m_refdEff2NSH[2])<<endreq;
  info()<<" Non-muons  :   "<<  format( "(%6.2f +-%6.2f) ", Ef4[3], dEf4[3] ) << 
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1NSH[3], m_refdEff1NSH[3])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2NSH[3], m_refdEff2NSH[3])<<endreq;
  info()<<" MC MisID   :   "<<  format( "(%6.2f +-%6.2f) ", misIDNSH, dmisIDNSH ) <<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1NSH[5], m_refdEff1NSH[5])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2NSH[5], m_refdEff2NSH[5])<<endreq;
  info()<<" Ghosts     :   "<<  format( "(%6.2f +-%6.2f) ", Ef4[0], dEf4[0] ) << 
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1NSH[0], m_refdEff1NSH[0])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2NSH[0], m_refdEff2NSH[0])<<endreq;
  }
  info()<<"Tot ID Rate :   "<<  format( "(%6.2f +-%6.2f) ", Ef4[4], dEf4[4] ) << 
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff1NSH[4], m_refdEff1NSH[4])<<
               "    :   "<< format( "(%6.2f +-%6.2f) ", m_refEff2NSH[4], m_refdEff2NSH[4])<<endreq;
  info()<<"----------------------------------------------------------------------------"
        <<  endreq;
  if (m_DLLFlag==2) { // cut on Prob_mu
    info()<<"----------------------------------------------------------------------------"
        <<  endreq;
    info()<<"------------ Muon Efficiency for Prob_mu wrt IsMuonFlag = "<<m_IsMuonFlag<<"  ----------"
        <<  endreq;
    info()<<"----------------------------------------------------------------------------"
        <<  endreq;
    info()<<" Muon Effic :   "<<  format( "(%6.2f +-%6.2f) ", EfProb[0], dEfProb[0] ) << 
      "    :  for cut on Prob_mu < "<< m_ProbCut1 << endreq;	
    info()<<" Muon Effic :   "<<  format( "(%6.2f +-%6.2f) ", EfProb[1], dEfProb[1] ) << 
      "    :  for cut on Prob_mu < "<< m_ProbCut2 << endreq;	
    info()<<"----------------------------------------------------------------------------"
        <<  endreq;
  }
  debug()<< "Total # of Tracks after pre-selection: " << m_nTrPreSel[4]<< endreq;

  debug()<< "Total # of Tracks with IsMuonLoose 1 :" << m_nTrIML[4]    << endreq;
  debug()<< "Total # of Tracks with IsMuon      1 :" << m_nTrIM[4]     << endreq;
  debug()<< "Total # of Tracks with DLL>"<<m_DLLCut<<" :" <<m_nTrDLL[4]<< endreq;
  debug()<< "Total # of Tracks with NSH<"<<m_NSHCut<<" :" <<m_nTrNSH[4]<< endreq;
  debug()<<"----------------------------------------------------------------------------"
        <<  endreq;

//  AIDA::IHistogram1D *my1Dhisto = histo1D(std::string("hDLL_1"));
              
  return  GaudiHistoAlg::finalize();
}

//=====================================================================
int MuonPIDChecker::findTrackRegion(const int sta){
//=====================================================================
// comment: Returns the muon detector region of the extrapolated track;
// authors: G. Lanfranchi & S. Furcas, 
// date:    10/5/09
//=====================================================================
    int chnum = -1;
      int regnum = -1;
        m_mudet->Pos2StChamberNumber(m_trackX[sta],m_trackY[sta],sta,chnum,regnum).ignore();
	  return regnum;

}

