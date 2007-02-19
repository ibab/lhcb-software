// Include files
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

  // Z threshold to define decay (in cm) 
  declareProperty( "DecayVxCut", m_zOriginCut = 100.);

  // DLL value to define efficiency  
  declareProperty( "DLLCut", m_DLLCut = -4.2 );

  // NShared value to define efficiency  
  declareProperty( "NSHCut", m_NSHCut = 1 );
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

  m_NStation = 0;
  m_NRegion = 0;
  MuonBasicGeometry basegeometry( detSvc(),msgSvc());
  m_NStation= basegeometry.getStations();
  m_NRegion = basegeometry.getRegions();
  int i=0;
  while(i<m_NStation){
    m_stationNames.push_back(basegeometry.getStationName(i));
    debug()   <<" station "<<i<<" "<<m_stationNames[i]<<endreq;
    i++;
  }
  m_mudet=getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");
  for(int station = 0 ; station < m_NStation ; station++ ){
    // z position of each station 
    m_stationZ.push_back(m_mudet->getStationZ(station));
  }

  // Count events

  m_neventsTest = 0;
  for (int i=0; i<4 ; i++){
    m_cpresel[i] = 0;
    m_cisMuon[i] = 0;
    m_cDLL[i] = 0;
    m_cnShared[i] = 0;
  }

  info() << " Z used to define decays = " << m_zOriginCut << " cm"<< endreq;
  

  return StatusCode::SUCCESS;
};


//====================================================================
// Main execution
//=====================================================================
StatusCode MuonPIDChecker::execute() {

  m_neventsTest++;    // count events 

  debug()  << "==> Number of events: " << m_neventsTest << endreq;

  // get  MuonPID objects 
  LHCb::MuonPIDs* pMuids=get<LHCb::MuonPIDs>(LHCb::MuonPIDLocation::Default);
  if (!pMuids){
    error() << " Failed to get MuonPID objects in "
        <<  LHCb::MuonPIDLocation::Default << endreq;
    return StatusCode::FAILURE;
  }

  // MC association 
  LinkedTo<LHCb::MCParticle,LHCb::MuonDigit> myLink(eventSvc(),
                                  msgSvc(),LHCb::MuonDigitLocation::MuonDigit); 

  LinkedTo<LHCb::MCParticle, LHCb::Track> myLinkToTrack( eventSvc(), msgSvc(),
                                         m_TracksPath );

  // Get tracks to loop over
  LHCb::Tracks* trTracks = get<LHCb::Tracks>(m_TracksPath);

  if (!trTracks){
    error() << " Failed to get Track container "
        <<  m_TracksPath << endreq;
    return StatusCode::FAILURE;
  }

  m_nTr = 0;
  m_nPSTr = 0;
  m_nPSGhosts = 0;

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
	   m_Trp0 = stateP0->p()/Gaudi::Units::GeV; 

	   // Retrieve track type (MC)
	   LHCb::MCParticle* mcP = myLinkToTrack.first(*iTrack);
	   LHCb::MCParticle* mcPnext = mcP;
	   while ( mcPnext != NULL ){
	     m_TrnLinks++;
	     if ( abs(mcPnext->particleID().pid()) == 13 ) mcP = mcPnext;
	     mcPnext = myLinkToTrack.next();
	   }
	   const LHCb::MCParticle* thismcP = mcP; 
	   m_TrType = getTrType( pTrack, thismcP ); 
	   if (m_TrType == 0 ) m_nPSGhosts++;

	   // Fill Track Info
	   StatusCode sc = FillTrHistos(0);
	   m_cpresel[m_TrType]++;
	   if (m_TrIsMuon == 1) sc = FillTrHistos(1);

	 } // Pre-selection
       } // Association to Track
     } // loop over MuonPIDs
    } // long and Matched tracks
  } // loop over tracks 

  // Plot track multiplicity
  plot1D(m_nTr, "hNtracks","Track multiplicity", 0, 400, 400);
  plot1D(m_nPSTr, "hNPStracks","Pre-selected Track multiplicity", 0, 400, 400);
  plot1D(m_nPSGhosts, "hNPSGhosts","Pre-selected ghost multiplicity", 0, 400, 400);

  LHCb::MuonCoords* coords = get<LHCb::MuonCoords>(LHCb::MuonCoordLocation::MuonCoords);
  if ( coords==0 ) {
       err() << " Cannot retrieve MuonCoords " << endreq;
       return StatusCode::FAILURE;
  }
    
  // loop over the coords
  m_nHit = 0;
  LHCb::MuonCoords::const_iterator iCoord;

  for ( iCoord = coords->begin() ; iCoord != coords->end() ; iCoord++ ){
    int region = (*iCoord)->key().region();
    int station = (*iCoord)->key().station();
    int nStatReg = station*m_NRegion+region;
    m_nHit++;   
    plot1D(nStatReg, "hNhitsReg", "Hit multiplicity per region", 0, 21, 21);

  } // loop over station coords 
  plot1D(m_nHit, "hNhits","Hit Multiplicity", 0, 3000, 500);

  return StatusCode::SUCCESS;

}
//=====================================================================
//  MC association and info
//====================================================================
StatusCode MuonPIDChecker::FillTrHistos(const int Level){ 
  // Level 0 => preselection
  // Level 1 => IsMuon = 1  
  char hname[10];

  sprintf ( hname, "hp_%d_%d", Level, m_TrType);
  plot1D( m_Trp0, hname , " Momentum (GeV) - PreSel", 0. , 200., 200 ); 

  sprintf ( hname, "hIM_%d_%d", Level, m_TrType);
  plot1D( m_TrIsMuon, hname , " IsMuon ", 0. , 2., 2 ); 

  if (Level==1) {
    m_cisMuon[m_TrType]++;
    if ( (m_TrMuonLhd - m_TrNMuonLhd) > m_DLLCut) m_cDLL[m_TrType]++;
    if ( m_TrNShared < m_NSHCut) m_cnShared[m_TrType]++;

    sprintf ( hname, "hNShared_%d_%d", Level, m_TrType);
    plot1D( m_TrNShared, hname , "Tracks sharing hits", 0., 10., 10 ); 

    sprintf ( hname, "hDLL_%d_%d", Level, m_TrType);
    plot1D( (m_TrMuonLhd - m_TrNMuonLhd) ,hname, "DLL", -10., 10., 200 ); 
  }

  return StatusCode::SUCCESS;
}
//=====================================================================
//  MC association and info
//====================================================================
StatusCode MuonPIDChecker::CheckMCAss(const LHCb::Track *pTrack, 
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
StatusCode MuonPIDChecker::resetTrInfo() {
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
    return StatusCode::SUCCESS;
}
//=====================================================================
//  Return Track Type     
//  0 = ghost
//  1 = muon from origin
//  2 = muon from decay/interaction
//  3 = non-muon
//====================================================================
int MuonPIDChecker::getTrType( const LHCb::Track *pTrack, 
                               const  LHCb::MCParticle* mcP ) {
  // MCParticle Association 
  StatusCode sc = CheckMCAss( pTrack , mcP);
  int type = -1;

  // Fill Histograms
  if ( sc.isFailure() ) {      // ghost

    type = 0;  
	       
  }else{                        // there is MC association

    m_TrMCp0 = mcP->p()/Gaudi::Units::GeV;

    if ( abs (mcP->particleID().pid()) == 13  && 
	(m_TrnLinks == 1 && m_TrzOrigin < m_zOriginCut) ){ 
        
      type = 1;  

    } else if ( ((abs (mcP->particleID().pid()) == 13)  &&
	  ( (m_TrnLinks==1 && m_TrzOrigin>=m_zOriginCut) || m_TrnLinks==2)) ||
	       (abs (mcP->particleID().pid()) !=13 && m_TrzDecay > 0 )){

      type = 2;  

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
  double Ef1[4];
  double Ef2[4];
  double Ef3[4];

  for (int ityp = 0; ityp < 4; ityp++){
    Ef1[ityp] = 100.*m_cisMuon[ityp]/m_cpresel[ityp];
    Ef2[ityp] = 100.*m_cDLL[ityp]/m_cpresel[ityp];
    Ef3[ityp] = 100.*m_cnShared[ityp]/m_cpresel[ityp];
  }


  info()<<"------------------Total MuonPID Efficiencies (%)------------------"
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
                      "    :    (96.49+-0.06 - (95.3 +- 0.2)" << endreq;
  info()<<" Decays     :   "<<  format( " %7.2f ", Ef1[2] )<< 
                      "    :    (66.1 +- 0.2) - (63.0 +- 0.2)" << endreq;
  info()<<" Non-muons  :   "<<  format( " %7.2f ", Ef1[3] ) << 
                      "    :    (2.48 +-0.01) - (1.78+- 0.01)" << endreq;
  info()<<" Ghosts     :   "<<  format( " %7.2f ", Ef1[0] ) << 
                      "    :    (6.08 +-0.03) - (4.81+- 0.02)" << endreq;
  info()<<"------------------------------------------------------------------"
        <<  endreq;
  info()<<"-------------------        DLL performance      ------------------"
        <<  endreq;
  info()<<"------------------------------------------------------------------"
        <<  endreq;
  info()<<" Muons      :   "<<  format( " %7.2f ", Ef2[1] ) <<
                      "    :    (90.5 +-0.1) - (80.4 +- 0.3)" << endreq;
  info()<<" Decays     :   "<<  format( " %7.2f ", Ef2[2] )<< 
                      "    :    (43.4 +-0.2) - (40.2 +- 0.2)" << endreq;
  info()<<" Non-muons  :   "<<  format( " %7.2f ", Ef2[3] ) << 
                      "    :    (0.52+-0.01) - (0.33+- 0.01)" << endreq;
  info()<<" Ghosts     :   "<<  format( " %7.2f ", Ef2[0] ) << 
                      "    :    (2.07+-0.01) - (1.66+- 0.01)" << endreq;
  info()<<"------------------------------------------------------------------"
        <<  endreq;
  info()<<"-------------------      nShared performance    ------------------"
        <<  endreq;
  info()<<"------------------------------------------------------------------"
        <<  endreq;
  info()<<" Muons      :   "<<  format( " %7.2f ", Ef3[1] ) <<
                      "    :    (83.3 +-0.1) - (88.5 +- 0.2)" << endreq;
  info()<<" Decays     :   "<<  format( " %7.2f ", Ef3[2] )<< 
                      "    :    (48.4 +-0.2) - (51.1 +- 0.1)" << endreq;
  info()<<" Non-muons  :   "<<  format( " %7.2f ", Ef3[3] ) << 
                      "    :    (0.76+-0.01) - (0.91 +-0.01)" << endreq;
  info()<<" Ghosts     :   "<<  format( " %7.2f ", Ef3[0] ) << 
                      "    :    (2.33+-0.02) - (2.37 +-0.01)" << endreq;
  info()<<"------------------------------------------------------------------"
        <<  endreq;

  return  GaudiHistoAlg::finalize();
}

