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

  // Z threshold to define decay (in cm) 
  declareProperty( "DecayVxCut", m_zOriginCut = 100. );

  // DLL value to define efficiency  
  declareProperty( "DLLCut", m_DLLCut = -1.0 );

  // NShared value to define efficiency  
  declareProperty( "NSHCut", m_NSHCut = 1 );

  // Swap between real and MC data   
  declareProperty( "RunningMC", m_RunningMC = false );

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

  debug()  << "==> Initialise" << endreq;

  // Get Geometry parameters
  m_NStation = 0;
  m_NRegion = 0;
  MuonBasicGeometry basegeometry( detSvc(),msgSvc());
  m_NStation= basegeometry.getStations();
  m_NRegion = basegeometry.getRegions();
  unsigned int i=0;
  while(i<m_NStation){
    m_stationNames.push_back(basegeometry.getStationName(i));
    debug()   <<" station "<<i<<" "<<m_stationNames[i]<<endreq;
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
    m_cpresel[i] = 0;
    m_cisMuon[i] = 0;
    m_cDLL[i] = 0;
    m_cnShared[i] = 0;
  }
  m_cmisID = 0;
  m_cmisIDDLL = 0;
  m_cmisIDnShared = 0;

  return StatusCode::SUCCESS;
};


//====================================================================
// Main execution
//=====================================================================
StatusCode MuonPIDChecker::execute() {

  m_neventsTest++;    // count events 

  debug()  << "==> Number of events: " << m_neventsTest << endreq;

  // get  MuonPID objects 
  LHCb::MuonPIDs* pMuids=get<LHCb::MuonPIDs>(m_MuonPIDsPath);
  if (!pMuids){
    error() << " Failed to get MuonPID objects in "
        <<  m_MuonPIDsPath << endreq;
    return StatusCode::FAILURE;
  }

  // MC association 
  LinkedTo<LHCb::MCParticle, LHCb::Track>* myLinkToTrack = NULL;
  if (m_RunningMC) {
   myLinkToTrack = new LinkedTo<LHCb::MCParticle, LHCb::Track>( eventSvc(), msgSvc(),
                                         m_TracksPath );
  }
  // Get tracks to loop over
  LHCb::Tracks* trTracks = get<LHCb::Tracks>(m_TracksPath);
  // Get muon tracks to loop over
  LHCb::Tracks* muTracks = get<LHCb::Tracks>(m_MuonTracksPath);

  if (!trTracks){
    error() << " Failed to get Track container "
        <<  m_TracksPath << endreq;
    return StatusCode::FAILURE;
  }
  if (!muTracks){
    error() << " Failed to get muon Track container "
        <<  m_MuonTracksPath << endreq;
    return StatusCode::FAILURE;
  }


  // Zero Track multiplicities after each selection step (per event)
  unsigned int m_nTr = 0;
  unsigned int m_nPSTr = 0;
  unsigned int m_nIMTr = 0;
  unsigned int m_nPSGhosts = 0;
  unsigned int m_nIMGhosts = 0;

  LHCb::Tracks::const_iterator iTrack;

  for (iTrack = trTracks->begin() ; iTrack != trTracks->end() ; iTrack++){

    if(!(*iTrack)->checkFlag(LHCb::Track::Clone)  &&
       ((*iTrack)->checkType(LHCb::Track::Long) ||
                          (*iTrack)->checkType(LHCb::Track::Downstream))){

      const LHCb::Track *pTrack = *iTrack;
      m_nTr++;

     // get state in zero position 
     const LHCb::State * stateP0 = &(pTrack->firstState());
     if (!stateP0){
       error() << " Failed to get stateP0 for track " << endreq;
       return StatusCode::FAILURE;
     }
     

     // link between track and MuonPID 
     LHCb::MuonPIDs::const_iterator imuid;
     for (imuid = pMuids->begin() ; imuid != pMuids->end() ; imuid++){
       if ((*imuid)->idTrack() == *iTrack){  // found Associated MuonPID
	 // Preselection
	 if ((*imuid)->PreSelMomentum() && (*imuid)->InAcceptance() ){
	   m_nPSTr++;
	   resetTrInfo();
	   m_TrIsMuon = (*imuid)->IsMuon();
	   m_TrMuonLhd= (*imuid)->MuonLLMu();
	   m_TrNMuonLhd = (*imuid)->MuonLLBg();
	   m_TrNShared = (*imuid)->nShared();
     if( std::abs( stateP0->qOverP() ) > 0.001 / Gaudi::Units::GeV ) {
       m_Trp0 = (1./stateP0->qOverP())/Gaudi::Units::GeV; 
     }
     else if( stateP0->qOverP() > 0. ) {
       m_Trp0 = 1000. / Gaudi::Units::GeV;
     }
     else {
       m_Trp0 = -1000. / Gaudi::Units::GeV;
     }
     
	   // Retrieve track type (MC)
           if (m_RunningMC) {
	     LHCb::MCParticle* mcP = myLinkToTrack->first(*iTrack);
	     LHCb::MCParticle* mcPnext = mcP;
	     while ( mcPnext != NULL ){
	       m_TrnLinks++;
	       if ( abs(mcPnext->particleID().pid()) == 13 ) mcP = mcPnext;
	       mcPnext = myLinkToTrack->next();
	     }
	     const LHCb::MCParticle* thismcP = mcP; 
	     m_TrType = getTrType( pTrack, thismcP ); 
	     if (m_TrType == 0 ) m_nPSGhosts++;
           }
           else {
             m_TrType = 4;
           }
	   // Fill Track Info
	   fillTrHistos(0);
	   m_cpresel[m_TrType]++;
	   if (m_TrIsMuon == 1) {
	     fillTrHistos(1);
	     m_nIMTr++;
	     if (m_TrType == 0 ) m_nIMGhosts++;
	   }

	 } // Pre-selection
       } // Association to Track
     } // loop over MuonPIDs
    } // long and Matched tracks
  } // loop over tracks 

  // Plot track multiplicity
  plot1D(m_nTr, "hNtracks","Track multiplicity", 0, 200, 200);
  plot1D(m_nPSTr, "hNPStracks","Pre-selected Track multiplicity", 0, 200, 200);
  plot1D(m_nIMTr, "hNIMtracks","IsMuon=1 Track multiplicity", 0, 20, 20);
  if (m_RunningMC) {
    plot1D(m_nPSGhosts, "hNPSGhosts","Pre-selected ghost multiplicity", 0, 200, 400);
    plot1D(m_nIMGhosts, "hNIMGhosts","IsMuon=1 ghost multiplicity", 0, 20, 20);
  }

  // loop over muon Tracks
  LHCb::Tracks::const_iterator imuTrack;

  for (imuTrack = muTracks->begin() ; imuTrack != muTracks->end() ; imuTrack++){
    std::vector<LHCb::LHCbID> mucoords = (*imuTrack) -> lhcbIDs();
    std::vector<LHCb::LHCbID>::iterator iID;
    int nhitsfoi[20]={0};
    for (iID = mucoords.begin(); iID != mucoords.end(); iID++) {
      if (!(iID->isMuon())) continue;
      LHCb::MuonTileID mutile = iID->muonID();
      int region = mutile.region();
      int station = mutile.station();
      int nStatReg = station*m_NRegion+region;
      nhitsfoi[nStatReg]++;
    } //end of loop over lhcbIDs 
    for (unsigned int i=0; i<m_NRegion*m_NStation; i++){
      plot2D(i, nhitsfoi[i],
      "hNhitsFOIReg", "Hits in FOI per region (per track)", 0, 21, 0, 5, 21, 5);
    }

  } //end of muTrack loop

    
  LHCb::MuonCoords* coords = get<LHCb::MuonCoords>(LHCb::MuonCoordLocation::MuonCoords);
  if ( coords==0 ) {
       err() << " Cannot retrieve MuonCoords " << endreq;
       return StatusCode::FAILURE;
  }
  // loop over the coords
  unsigned int m_nHit = 0;
  LHCb::MuonCoords::const_iterator iCoord;

  for ( iCoord = coords->begin() ; iCoord != coords->end() ; iCoord++ ){
    int region = (*iCoord)->key().region();
    int station = (*iCoord)->key().station();
    int nStatReg = station*m_NRegion+region;
    m_nHit++;   
    plot1D(nStatReg, "hNhitsReg", "Total Hit multiplicity per region", 0, 21, 21);

  } // loop over station coords 
  plot1D(m_nHit, "hNhits","Hit Multiplicity", 0, 3000, 500);
  // delete myLinToTrack object if created (RunningMC == true)
  if ( NULL != myLinkToTrack ) delete myLinkToTrack;

  return StatusCode::SUCCESS;

}
//=====================================================================
//  Fill track info
//====================================================================
void MuonPIDChecker::fillTrHistos(const int Level){ 
  // Level 0 => preselection
  // Level 1 => IsMuon = 1  
  char hname[10];

  if (Level<1) { 
    sprintf ( hname, "Mom_PS_%d", m_TrType);
  } else {
    sprintf ( hname, "Mom_IM_%d", m_TrType);
  }

  plot1D( m_Trp0, hname , " Momentum (GeV) ", -200. , 200., 400 ); 

  if (Level==1) {
    m_cisMuon[m_TrType]++;

    if ( m_TrType == 2 || m_TrType == 3 ) m_cmisID++ ;
    if ( (m_TrType == 2 || m_TrType == 3) && 
         (m_TrMuonLhd - m_TrNMuonLhd) > m_DLLCut ) m_cmisIDDLL++ ;
    if ( ( m_TrType == 2 || m_TrType == 3) && 
         m_TrNShared < m_NSHCut ) m_cmisIDnShared++ ;

    if ( (m_TrMuonLhd - m_TrNMuonLhd) > m_DLLCut) m_cDLL[m_TrType]++;
    if ( m_TrNShared < m_NSHCut) m_cnShared[m_TrType]++;

    sprintf ( hname, "hNShared_%d", m_TrType);
    plot1D( m_TrNShared, hname , "Tracks sharing hits", 0., 10., 10 ); 

    sprintf ( hname, "hDLL_%d", m_TrType);
    plot1D( (m_TrMuonLhd - m_TrNMuonLhd) ,hname, "DLL", -10., 10., 200 ); 
  } else {
    sprintf ( hname, "hIM_%d", m_TrType);
    plot1D( m_TrIsMuon, hname , " IsMuon ", 0. , 2., 2 ); 
    
  }

  return;
}
//=====================================================================
//  MC association and info
//====================================================================
StatusCode MuonPIDChecker::checkMCAss(const LHCb::Track *pTrack, 
                                      const LHCb::MCParticle* mcP){ 

  if (!mcP){
    debug() <<" Tr2MCTable failed, itrack = "<< *pTrack << endreq;
    m_TrType = 0;
    return StatusCode::FAILURE;
  } else {
    // look for decays in flight
    SmartRefVector< LHCb::MCVertex >::const_iterator iVtx;
    m_TrzOrigin = mcP->originVertex()->position().z()/Gaudi::Units::cm;
    if (0==mcP->endVertices().size()){
      debug() << " No end Vertex for track " << *pTrack << endreq;
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
    return StatusCode::SUCCESS;
  } // there is MCParticle associated to Track
}
//=====================================================================
//  Reset Track Variables  
//====================================================================
void MuonPIDChecker::resetTrInfo() {
    m_TrNShared= -1;
    m_TrMuonLhd= -100.;
    m_TrNMuonLhd = -100.;
    m_TrIsMuon = 0;
    m_TrnLinks = 0;
    m_TrzDecay = -10.;
    m_TrzOrigin = -10.;
    m_TrType = -1;
    m_Trp0 = 0.;
    m_TrMCp0 = 0;
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
                               const  LHCb::MCParticle* mcP ) {
  int type = -1;

  // MCParticle Association 
  StatusCode sc = checkMCAss( pTrack , mcP);

  // Fill Histograms
  if ( sc.isFailure() || 0 == mcP->mother() ) {      // ghost

    type = 0;  
	       
  }else{                        // there is MC association

    m_TrMCp0 = mcP->p()/Gaudi::Units::GeV;
    if (( abs (mcP->particleID().pid()) == 13) && ((m_TrnLinks==1 &&
	  ( abs (mcP->mother()->particleID().pid()) == 211 || 
	    abs (mcP->mother()->particleID().pid()) == 321 )) || 
	    m_TrnLinks==2) ||
	( abs (mcP->particleID().pid()) !=13 && 
	    (m_TrzDecay > 0 && m_TrzDecay<20000.)) ){

      type = 2;  
    }else if ( abs (mcP->particleID().pid()) == 13  && m_TrnLinks == 1 ){ 
        
      type = 1;  

    } else {

      type = 3;  

    } 

  } // MC associated  or ghost
  return type;

}
//=====================================================================
//  Finalize
//====================================================================


StatusCode MuonPIDChecker::finalize() {


  debug() << "==> Finalize" << endreq;
if (m_RunningMC) {
  double Ef1[4];
  double Ef2[4];
  double Ef3[4];
  double misID, misIDDLL, misIDnShared;

  for (unsigned int ityp = 0; ityp < 4; ityp++){
    if( 0 < m_cpresel[ityp] ) {
      Ef1[ityp] = 100.*m_cisMuon[ityp]/m_cpresel[ityp];
      Ef2[ityp] = 100.*m_cDLL[ityp]/m_cpresel[ityp];
      Ef3[ityp] = 100.*m_cnShared[ityp]/m_cpresel[ityp];
    }
    else {
      Ef1[ityp] = -1.;
      Ef2[ityp] = -1.;
      Ef2[ityp] = -1.;
    }
  }
  unsigned int sumPresel = m_cpresel[2]+m_cpresel[3];
  if( 0 < sumPresel ) {
    misID        = 100. * (double)m_cmisID        / (double)sumPresel;
    misIDDLL     = 100. * (double)m_cmisIDDLL     / (double)sumPresel;
    misIDnShared = 100. * (double)m_cmisIDnShared / (double)sumPresel;
  }
  else {
    misID        = -1.;
    misIDDLL     = -1.;
    misIDnShared = -1.;
  }

  info()<<"------------------MIMIMITotal MuonPID Efficiencies (%)------------------"
        <<  endreq;
  info()<<"-------------------          IsMuon             ------------------"
        <<  endreq;
  info()<<"------------------------------------------------------------------"
        <<  endreq;
  info()<<" Particle   :  These Events  :     Reference value for DC06  "
        <<  endreq;
  info()<<"                             :      J/PsiKs    -   bb inclusive "
        <<  endreq;
  info()<<"------------------------------------------------------------------"
        <<  endreq;
  info()<<" Muons      :   "<<  format( " %7.2f ", Ef1[1] ) <<
                      "    :    (98.95+-0.06) - (97.6 +- 0.2)" << endreq;
  info()<<" Decays     :   "<<  format( " %7.2f ", Ef1[2] )<< 
                      "    :    (64.2 +- 0.2) - (65.8 +- 0.2)" << endreq;
  info()<<" Non-muons  :   "<<  format( " %7.2f ", Ef1[3] ) << 
                      "    :    (2.68 +-0.01) - (2.85+- 0.01)" << endreq;
  info()<<" Ghosts     :   "<<  format( " %7.2f ", Ef1[0] ) << 
                      "    :    (6.24 +-0.03) - (6.10+- 0.02)" << endreq;
  info()<<" Total MisID:   "<<  format( " %7.2f ", misID ) <<
                      "    :    (4.34 +-0.01) - (4.64+- 0.01)" << endreq;
  info()<<"------------------------------------------------------------------"
        <<  endreq;
  info()<<"-------------------        DLL performance      ------------------"
        <<  endreq;
  info()<<"------------------------------------------------------------------"
        <<  endreq;
  info()<<" Muons      :   "<<  format( " %7.2f ", Ef2[1] ) <<
                      "    :    (93.0 +-0.1) - (88.0 +- 0.3)" << endreq;
  info()<<" Decays     :   "<<  format( " %7.2f ", Ef2[2] )<< 
                      "    :    (37.1 +-0.2) - (39.4 +- 0.2)" << endreq;
  info()<<" Non-muons  :   "<<  format( " %7.2f ", Ef2[3] ) << 
                      "    :    (0.52+-0.01) - (0.47+- 0.01)" << endreq;
  info()<<" Ghosts     :   "<<  format( " %7.2f ", Ef2[0] ) << 
                      "    :    (2.04+-0.01) - (1.60+- 0.01)" << endreq;
  info()<<" Total MisID:   "<<  format( " %7.2f ", misIDDLL ) <<
                      "    :    (1.51+-0.01) - (1.57+- 0.01)" << endreq;
  info()<<"------------------------------------------------------------------"
        <<  endreq;
  info()<<"-------------------      nShared performance    ------------------"
        <<  endreq;
  info()<<"------------------------------------------------------------------"
        <<  endreq;
  info()<<" Muons      :   "<<  format( " %7.2f ", Ef3[1] ) <<
                      "    :    (95.5 +-0.1) - (91.0 +- 0.2)" << endreq;
  info()<<" Decays     :   "<<  format( " %7.2f ", Ef3[2] )<< 
                      "    :    (47.9 +-0.2) - (49.2 +- 0.1)" << endreq;
  info()<<" Non-muons  :   "<<  format( " %7.2f ", Ef3[3] ) << 
                      "    :    (0.85+-0.01) - (1.15 +-0.01)" << endreq;
  info()<<" Ghosts     :   "<<  format( " %7.2f ", Ef3[0] ) << 
                      "    :    (2.39+-0.02) - (2.24 +-0.01)" << endreq;
  info()<<" Total MisID:   "<<  format( " %7.2f ", misIDnShared) << 
                      "    :    (2.12+-0.01) - (2.51 +-0.01)" << endreq;
  info()<<"------------------------------------------------------------------"
        <<  endreq;
}
else {
  info()<< "Total number of Tracks after the pre-selection: " << m_cpresel[4]
<< endreq;
  info()<< "Total number of Tracks with IsMuon = true: " << m_cisMuon[4]
<< endreq;
}
  return  GaudiHistoAlg::finalize();
}

