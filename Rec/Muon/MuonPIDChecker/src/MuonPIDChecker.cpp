// $Id: MuonPIDChecker.cpp,v 1.26 2010-04-21 21:11:59 polye Exp $
// Include files 
#include <cmath>
#include <iomanip>
#include <vector>

#include "Linker/LinkedTo.h"

// local
#include "MuonPIDChecker.h"


//-----------------------------------------------------------------------------
// Implementation file for class : MuonPIDChecker
//
// 2010-03-20 : Erica Polycarpo Macedo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonPIDChecker )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonPIDChecker::MuonPIDChecker( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  // Source of track to ID
  declareProperty("TrackLocation", m_TracksPath = LHCb::TrackLocation::Default);
  // Source of MuonPID
  declareProperty("MuonIDLocation", m_MuonPIDsPath = LHCb::MuonPIDLocation::Default);
  // Source of MuonTracks 
  declareProperty("MuonTrackLocation", m_MuonTracksPath = LHCb::TrackLocation::Muon);
  // Look at Long,Downstream or Both types of tracks
  declareProperty("TrackType", m_TrackType = 0 ); // Long
  // OutputLevel for Histograms  
  // "None", "Online", "OfflineExpress", "OfflineFull", "Expert"
  //   0         1          2                 3            4
  declareProperty( "HistosOutput", m_HistosOutput= 3 );
  // Swap between real and MC data   
  declareProperty( "RunningMC", m_RunningMC = false );

  declareProperty( "MonitorCutValues", m_monitCutValues = std::vector<double>(8, -1.) );

  // Limits for the DLL histos
  declareProperty( "DLLlower", m_DLLlower = -1. ); // -10 for DLLFlag = 0
  declareProperty( "DLLupper", m_DLLupper = 6.0 ); // 10 for DLLFlag = 0
  declareProperty( "DLLnbins", m_DLLnbins = 35 ); // 50 for DLLFlag = 0

}
//=============================================================================
// Destructor
//=============================================================================
MuonPIDChecker::~MuonPIDChecker() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MuonPIDChecker::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  // Get Geometry parameters
  MuonBasicGeometry basegeometry( detSvc(),msgSvc());
  m_NStation= basegeometry.getStations();
  m_NRegion = basegeometry.getRegions();
  if ( msgLevel(MSG::DEBUG) ) debug()   <<"initialize:: Nstations, NRegions "<<
                                m_NStation<< ", " << m_NRegion << endmsg;

  unsigned int i=0;
  while(i<m_NStation){
    m_stationNames.push_back(basegeometry.getStationName(i));
    if ( msgLevel(MSG::DEBUG) ) debug()   <<"initialize:: station "<<i<<" "<<m_stationNames[i]<<endmsg;
    i++;
  }
  m_mudet=getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");
  for(unsigned int station = 0 ; station < m_NStation ; station++ ){
    // z position of each station 
    m_stationZ.push_back(m_mudet->getStationZ(station));
  }

  // Reset overall counters
  m_nMonitorCuts = m_monitCutValues.size();
  if (m_nMonitorCuts ==0 ) Warning ("initialize: no cuts to monitore, finalize will give no info");
  if ( msgLevel(MSG::DEBUG) ) debug() << "initialize:: #of cuts = "<<m_nMonitorCuts<< endmsg;
  for (int j=0;j<m_nMonitorCuts+2;j++) {
    m_ntotTr.push_back(0);
    if ( m_nMonitorCuts>j && msgLevel(MSG::DEBUG) ) debug() <<"initialize:: cut "<<i<<""<<m_monitCutValues[j]<<endmsg;
  }

  //Book histograms according to m_HistosOutput

    if (m_HistosOutput>0){
      //fillMultiplicityPlots
      book1D("hNIMLtracks","IsMuonLoose Track multiplicity", -0.5, 11.5, 12);
      book1D("hNIMtracks","IsMuonLoose Track multiplicity", -0.5, 11.5, 12);
      book1D("hNIMLtracksRatio","#IsMuonLoose/#Tracks", 0., 1.1, 22);
      //fillIMLPlots
      book1D( "hIMLMomentum", "IsMuonLoose Candidate Momentum (GeV/c^2)", -25., 25., 100);
      book1D( "hIMLPT", "IsMuonLoose Candidate p_T (GeV/c^2)", -5., 5., 100);
      book1D( "hIMLRegion", "MS Region for IML  tracks",0.5,4.5,4); 
    }
    if (m_HistosOutput>1){
      //fillMultiplicityPlots
      book1D("hNtracks","Track multiplicity", -0.5, 60.5, 61);
      book1D("hPSNtracks","PreSelection Track multiplicity", -0.5, 50.5, 51);
      book1D("hNIMLPStracksRatio","#IsMuonLoose/#PSTracks", 0., 1.1, 22);
      book1D("hNIMPStracksRatio","#IsMuon/#PSTracks", 0., 1.1, 22);
      book1D("hNIMtracksRatio","#IsMuon/#Tracks", 0., 1.1, 22);
      //fillPreSelPlots
      book1D( "hPSRegion", "MS Region for PS  tracks",0.5,4.5,4); 
      book1D( "hPSMomentum", "PreSelected Track Momentum (GeV/c^2)", -25., 25., 100);
      book1D( "hPSPT", "PreSelected Track p_T (GeV/c^2)", -5., 5., 100);
      book1D( "hIML_PS"  , " IsMuonLoose for PS Tracks ", -0.5 , 1.5, 2 );
      book1D( "hIM_PS"  , " IsMuon for PS Tracks ", -0.5 , 1.5, 2 );
      //fillIMLPlots
      book1D( "hNShared_IML"  , " NShared for PS Tracks ", -0.5 , 5.5, 6 );
      book1D( "hDist2_IML", "Muon Dist for IML candidates", 0., 600., 100); 
      book1D( "hProbMu_IML", "Muon Probability for IML candidates", -0.1, 1.1, 60);
      book1D( "hProbNMu_IML", "Non-Muon Probability for IML candidates", -0.1, 1.1, 60);
      book1D( "hMuDLL_IML", "Muon DLL for IML candidates",m_DLLlower, m_DLLupper, m_DLLnbins); 
      book1D( "hNIMLvsXM2", "MS X position at M2 for IML  tracks",-5500, 5500, 550);
      book1D( "hNIMLvsYM2", "MS Y position at M2 for IML  tracks",-5500, 5500, 550);
      //fillIMPlots
      book1D( "hIMMomentum", "IsMuon candidate Momentum (GeV/c^2)", -25., 25., 100);
      book1D( "hIMPT", "IsMuon candidate p_T (GeV/c^2)", -5., 5., 100);
      book1D( "hIMRegion", "MS Region for IM tracks",0.5,4.5,4); 
      book1D( "hNShared_IM"  , " NShared for PS Tracks ", -0.5 , 5.5, 6 );
      book1D( "hDist2_IM", "Muon Dist for IM candidates", 0., 600., 100); 
      book1D( "hProbMu_IM", "Muon Probability for IM candidates", -0.1, 1.1, 60);
      book1D( "hProbNMu_IM", "Non-Muon Probability for IM candidates", -0.1, 1.1, 60);
      book1D( "hMuDLL_IM", "Muon DLL for IM candidates",m_DLLlower, m_DLLupper, m_DLLnbins); 
      book1D( "hNIMvsXM2", "MS X position at M2 for IM  tracks",-5500, 5500, 550);
      book1D( "hNIMvsYM2", "MS Y position at M2 for IM  tracks",-5500, 5500, 550);
      for (unsigned int i=1;i<m_NRegion+1;i++){
	//fillIMLPlots
        GaudiAlg::HistoID  hname;
        std::string htitle;
	hname = "hDist2_IML_R"+std::to_string(i);
        htitle= "Muon Dist for IML candidates at R"+std::to_string(i);
	book1D( hname, htitle, 0., 600., 100); 
	hname = "hProbMu_IML_R"+std::to_string(i);
	htitle= "Muon Probability for IML candidates at R"+std::to_string(i);
	book1D( hname , htitle , -0.1, 1.1, 60);
	//fillIMPlots
	hname = "hDist2_IM_R"+std::to_string(i);
        htitle= "Muon Dist for IM candidates at R"+std::to_string(i);
	book1D( hname, htitle, 0., 600., 100); 
	hname = "hProbMu_IM_R"+std::to_string(i);
	htitle= "Muon Probability for IM candidates at R"+std::to_string(i);
	book1D( hname , htitle , -0.1, 1.1, 60);
      }
    }
    if (m_HistosOutput>2){
      //fillPreSelPlots
      bookProfile1D( "hEffvsP_IML", "IML Efficiency vs P (GeV/c^2)", -25., 25., 100);
      bookProfile1D( "hEffvsPT_IML", "IML Efficiency vs PT (GeV/c^2)", -5., 5., 100);
      bookProfile1D( "hEffvsP_IM", "IM Efficiency vs P (GeV/c^2)", -25., 25., 100);
      bookProfile1D( "hEffvsPT_IM", "IM Efficiency vs PT (GeV/c^2)", -5., 5., 100);
      //fillIMLPlots
      book1D( "hIM_IML"  , " IsMuon for IML Tracks ", -0.5 , 1.5, 2 );
      for (unsigned int i=1;i<m_NRegion+1;i++){
	//fillIMLPlots
	GaudiAlg::HistoID hname;
	std::string htitle;
	hname = "hDLL_IML_R"+std::to_string(i);
	htitle= "Muon DLL for IML candidates at R"+std::to_string(i);
	book1D( hname , htitle , m_DLLlower, m_DLLupper, m_DLLnbins);
	//fillIMPlots
	hname = "hDLL_IM_R"+std::to_string(i);
	htitle= "Muon DLL for IM candidates at R"+std::to_string(i);
	book1D( hname , htitle , m_DLLlower, m_DLLupper, m_DLLnbins);
      }
      //fillHitMultPlots
      GaudiAlg::HistoID hname;
      std::string htitle;
      hname = "hAvTotNhitsFOIvsR";
      htitle= "Mean Number of hits in FOI vs Region (M2)";
      bookProfile1D( hname, htitle ,0.5,4.5,4); 
      hname = "hAvTotNhitsFOIvsX";
      htitle= "Mean Number of hits in FOI vs X (M2)";
      bookProfile1D( hname, htitle, -5000, 5000, 200); 
      hname = "hAvTotNhitsFOIvsY";
      htitle= "Mean Number of hits in FOI vs Y (M2)";
      bookProfile1D( hname, htitle, -5000, 5000, 200); 
      for (unsigned int i=1;i<m_NStation+1;i++){
	hname = "hAvNHhitsFOIvsR_M"+std::to_string(i);
	htitle= "Mean Number of hits in FOI for M"+std::to_string(i)+" vs MS Region";
	bookProfile1D( hname ,htitle , 0.5, 4.5, 4);
      }
    } 
    if (m_HistosOutput>3){
      //fillIMLPlots
      book1D( "hChi2_IML", "Chi2 per nDOF for IML Candidates", 0., 200., 100);
      book1D( "hQuality_IML", "Track Quality for IML Candidates", 0., 200., 100);
      book1D( "hCLQuality_IML", "Track CL Quality for IML Candidates", -0.1, 1.1, 60);
      book1D( "hCLArrival_IML", "Track CL Arrival for IML Candidates", -0.1, 1.1, 60);
      bookProfile1D( "hProbMuvsP_IML", "Mean Muon Prob vs p for IML tracks", 0.,100.,100);
      bookProfile1D( "hNProbMuvsP_IML", "Mean non-Muon Prob vs P for IML tracks ", 0.,100.,100);
      //fillIMPlots
      book1D( "hChi2_IM", "Chi2 per nDOF for IM Candidates", 0., 200., 100);
      book1D( "hQuality_IM", "Track Quality for IM Candidates", 0., 200., 100);
      book1D( "hCLQuality_IM", "Track CL Quality for IM Candidates", -0.1, 1.1, 60);
      book1D( "hCLArrival_IM", "Track CL Arrival for IM Candidates", -0.1, 1.1, 60);
      bookProfile1D( "hProbMuvsP_IM", "Mean Muon Prob vs p for IM tracks", 0.,100.,100);
      bookProfile1D( "hNProbMuvsP_IM", "Mean non-Muon Prob vs P for IM tracks ", 0.,100.,100);
      //fillHitMultPlots
      for (unsigned int i=1;i<m_NStation+1;i++){
	GaudiAlg::HistoID hname;
	std::string htitle;
	hname = "hNHhitsFOIvsR_M"+std::to_string(i);
	htitle= "Number of hits in FOI for M"+std::to_string(i)+" vs MS Region";
	book2D(hname, htitle, 0.5,4.5,4, -0.5,10.5,11); 

	hname = "hNhitsFOIvsX_M"+std::to_string(i);
	htitle= "Number of hits in FOI for M"+std::to_string(i)+" vs MS X Pos.";
	book2D(hname, htitle, -0.5,10.5, 11, -5000, 5000, 200); 

	hname = "hNhitsFOIvsY_M"+std::to_string(i);
	htitle= "Number of hits in FOI for M"+std::to_string(i)+" vs MS Y Pos.";
	book2D(hname, htitle, -0.5,10.5, 11,-5000, 5000, 200); 

	hname = "hAvNhitsFOIvsX_M"+std::to_string(i);
	htitle= "Mean Number of hits in FOI for M"+std::to_string(i)+" vs MS X Pos.";
	bookProfile1D( hname, htitle, -5000, 5000, 200); 

	hname = "hAvNhitsFOIvsY_M"+std::to_string(i);
	htitle= "Mean Number of hits in FOI for M"+std::to_string(i)+" vs MS Y Pos.";
	bookProfile1D( hname, htitle, -5000, 5000, 200); 

      }

    }

  if ( msgLevel(MSG::DEBUG) ) debug() <<"initialize:: TrackType "<< m_TrackType <<endmsg;
  if ( msgLevel(MSG::DEBUG) ) debug() <<"initialize:: HistosOutput "<<m_HistosOutput <<endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonPIDChecker::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  if ( msgLevel(MSG::DEBUG) ) debug() <<"execute:: TrackType "<< m_TrackType <<endmsg;
  if ( msgLevel(MSG::DEBUG) ) debug() <<"execute:: HistosOutput "<<m_HistosOutput <<endmsg;

  // Get tracks to loop over
  const LHCb::Tracks* trTracks = get<LHCb::Tracks>(m_TracksPath);
  if (!trTracks){ // this will never happen -- get will throw instead...
    Warning("execute:: Failed to get Track container", StatusCode::SUCCESS).ignore();
  }

  // get  MuonPID objects 
  const LHCb::MuonPIDs* pMuids= getIfExists<LHCb::MuonPIDs>(m_MuonPIDsPath);
  if (!pMuids){
    Warning("execute:: Failed. MuonPID container doesn't exist", StatusCode::SUCCESS).ignore();
  }

  // Get muon tracks to loop over
  const LHCb::Tracks* muTracks = getIfExists<LHCb::Tracks>(m_MuonTracksPath);
  if (!muTracks){
    Warning("execute:: Failed. MuonTrack container doesn't exist", StatusCode::SUCCESS).ignore();
  }

  if ( msgLevel(MSG::DEBUG) ) debug() << "execute:: Start loop over tracks" << endmsg;

  // Reset event counters  
  m_nTr=0; 
  m_nTrPreSel=0; 
  m_nTrIsMuonLoose=0; 
  m_nTrIsMuon=0; 
  for (const auto& track : *trTracks ) {

    if(!(track->checkFlag(LHCb::Track::Clone))  &&
       ( (track->checkType(LHCb::Track::Long) && (m_TrackType==0 || m_TrackType==2)) ||
         (track->checkType(LHCb::Track::Downstream) && (m_TrackType==1 || m_TrackType==2))) ){

      m_nTr++;
      resetTrInfo();
      if ( msgLevel(MSG::DEBUG) ) debug() << "execute::isDown " << track->checkType(LHCb::Track::Downstream)<<endmsg;
      if ( msgLevel(MSG::DEBUG) ) debug() << "execute::isLong " << track->checkType(LHCb::Track::Long)<<endmsg;
      if ( msgLevel(MSG::DEBUG) ) debug() << "execute::TrackType " << m_TrackType <<endmsg;
       
      StatusCode scget=getTrackInfo(*track);
      if ( msgLevel(MSG::DEBUG) ) debug() << "execute:: track info retrieved" << endmsg;

      if (scget.isFailure()) {
        Warning("execute:: Failed to get track Info ", StatusCode::SUCCESS,0).ignore();
        continue;
      }

      if(pMuids) getMuonPIDInfo(*track, pMuids);
      if ( msgLevel(MSG::DEBUG) ) debug() << "execute:: MuonPID info retrieved for track "<< m_nTr  << endmsg;

      if(muTracks) getMuonTrackInfo(*track, muTracks);
      if ( msgLevel(MSG::DEBUG) ) debug() << "execute:: MuonTrack info retrieved for track "<< m_nTr << endmsg;

      if (m_TrIsPreSel>0) {
        m_nTrPreSel++;
        if ( msgLevel(MSG::DEBUG) ) debug() << "execute:: Track is PreSelected " << m_nTr<<endmsg;
        // Find region hit by the track 
        m_TrRegionM2 = findTrackRegion(1)+1;
        if ( msgLevel(MSG::DEBUG) ) debug() << "execute:: Region was retrieved =  R"<< m_TrRegionM2<<endmsg;
        fillPreSelPlots(m_HistosOutput);
        if ( msgLevel(MSG::DEBUG) ) debug() << "execute:: PreSel Histos filled" << endmsg; 
        fillHitMultPlots(m_HistosOutput);
        if ( msgLevel(MSG::DEBUG) ) debug() << "execute:: Hit Mult Histos filled" << endmsg; 
        if (m_TrIsMuonLoose>0){
          m_nTrIsMuonLoose++;
          fillIMLPlots(m_HistosOutput);
          if ( msgLevel(MSG::DEBUG) ) debug() << "execute:: IsMuonLoose Histos filled" << endmsg; 
          if (m_TrIsMuon>0) {
            m_nTrIsMuon++;
            fillIMPlots(m_HistosOutput);
            if ( msgLevel(MSG::DEBUG) ) debug() << "execute:: IsMuon Histos filled" << endmsg; 
          }
	   
        }

      } // track is preselected

    }// track type is satisfied                 
  }// loop over tracks

  fillMultiplicityPlots(m_HistosOutput);

  m_ntotTr[0] += m_nTr;
  m_ntotTr[1] += m_nTrPreSel;
  
  if(m_nMonitorCuts>0)  m_ntotTr[2] += m_nTrIsMuonLoose;
  if(m_nMonitorCuts>1)  m_ntotTr[3] += m_nTrIsMuon;
  if ( msgLevel(MSG::DEBUG) ) for (int i=0;i<m_nMonitorCuts+2;i++)
	  debug() << "execute:: "<<i<<"-> m_ntotTr[i]"<<m_ntotTr[i]<< endmsg;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonPIDChecker::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  if (0<m_nMonitorCuts && 0<m_ntotTr[1]) {
    std::vector<double> rate(m_nMonitorCuts), rateError(m_nMonitorCuts);
    std::vector<std::string> CutName; 
    CutName.push_back("IsMuonLoose         = "); 
    CutName.push_back("IsMuon              = "); 
    CutName.push_back("IsMuoLoose MuonProb < "); 
    CutName.push_back("IsMuon     MuonProb < "); 
    CutName.push_back("IsMuoLoose DLL      > "); 
    CutName.push_back("IsMuon     DLL      > "); 
    CutName.push_back("IsMuonLoose NShared < "); 
    CutName.push_back("IsMuon     NShared  < "); 
    for (int i=2;i<m_nMonitorCuts+2;i++) {
      rate[i-2]= 100.* (double)m_ntotTr[i]/(double)m_ntotTr[1];
      rateError[i-2]= getRateError( rate[i-2]/100., m_ntotTr[1]);
      if ( msgLevel(MSG::DEBUG) ) debug() << "finalize:: "<<i<<" "
                                          <<CutName[i-2] <<m_monitCutValues[i]
                                          <<":"<< m_ntotTr[i] <<endmsg;
    }
    
    info()<< "----------------------------------------------------------------"<< endmsg;
    if (m_TrackType==0) info()<< "                  MuonID Rates for Long Tracks "<< endmsg;
    if (m_TrackType==1) info()<< "                MuonID Rates for Downstream Tracks "<< endmsg;
    if (m_TrackType==2) info()<< "             MuonID Rates for Long+Downstream Tracks "<< endmsg;
    info()<< "----------------------------------------------------------------"<< endmsg;
    info()<< "  Criterium                          Rate After IsMuonLoose (%) "<< endmsg;
    info()<< "----------------------------------------------------------------"<< endmsg;
    for (int i=0;i<m_nMonitorCuts;i=i+2){ 
      info()<< CutName[i] << format( "%3.1f      :", m_monitCutValues[i])<< format("  %7.3f +-%7.3f ", 
                                                                                   rate[i], rateError[i])  << endmsg;
      if ( msgLevel(MSG::DEBUG) ) debug() << "finalize:: i="<<i<< " cutname"<<CutName[i] <<":"<< m_ntotTr[i+2] <<endmsg;
    }
                       
    info()<< "----------------------------------------------------------------"<< endmsg;
    info()<< "  Criterium                            Rate After IsMuon (%) "<< endmsg;
    info()<< "----------------------------------------------------------------"<< endmsg;
    for (int i=1;i<m_nMonitorCuts;i=i+2){ 
      info()<< CutName[i] << format( "%3.1f      :", m_monitCutValues[i])<< format("  %7.3f +-%7.3f ", 
                                                                                   rate[i], rateError[i])  << endmsg;
      if ( msgLevel(MSG::DEBUG) ) debug() << "finalize:: i="<<i<< " cutname"<<CutName[i] <<":"<< m_ntotTr[i+2] <<endmsg;
    }
                       
  }
  info()<< "----------------------------------------------------------------"<< endmsg;
  info()<< "Total Number of Tracks analysed: "<< m_ntotTr[0] << endmsg; 
  info()<< "Total Number of PreSelected Tracks: "<< m_ntotTr[1] << endmsg; 
  if (3<m_nMonitorCuts){
    info()<< "Total Number of IsMuonLoose Candidates: "<< m_ntotTr[2] << endmsg; 
    info()<< "Total Number of IsMuon Candidates: "<< m_ntotTr[3] << endmsg; 
  }
  info()<< "----------------------------------------------------------------"<< endmsg;
  
  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//======================================================================
// Extrapolate Tracks to get X,Y of extrapolation point in each station 
//======================================================================
StatusCode MuonPIDChecker::trackExtrapolate(const LHCb::Track &track){

  // get state closest to M1 for extrapolation
  const LHCb::State * state = &(track.closestState(9450));


  if(!state){
    err() << " Failed to get state from track " << endmsg;
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
    if ( msgLevel(MSG::DEBUG) ) debug() << "trackExtrapolate:: M"<<station << 
                                  "  x= "<< m_trackX[station]<< " and y = "<<m_trackY[station]<<endmsg;
  }
  return StatusCode::SUCCESS;
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
  if ( msgLevel(MSG::DEBUG) ) debug() << "findTrackRegion:: M"<<sta << 
                                "  x= "<< m_trackX[sta]<< " and y = "<<m_trackY[sta]<<endmsg;
  m_mudet->Pos2StChamberNumber(m_trackX[sta],m_trackY[sta],sta,chnum,regnum).ignore();
  return regnum;
    
}

//=====================================================================
//  Reset Track Variables  
//====================================================================
void MuonPIDChecker::resetTrInfo() {
  // reset info about tracks
  m_Trp0 = -10000.;
  m_TrpT = -10000.;
  m_trackX.clear();
  m_trackY.clear();
  // MuonPID Info
  m_TrIsPreSel=0;
  m_TrIsMuon = 1000;
  m_TrIsMuonLoose = 1000;
  m_TrMuonLhd= -1000.;
  m_TrNMuonLhd = -1000.;
  m_TrNShared=1000;
  // Muon Track Info
  m_TrChi2 =-1000;
  m_TrDist2 =-1000;
  m_TrCLquality =-1000;
  m_TrCLarrival =-1000;
  m_Trquality =-1000;
  for (unsigned int i=0;i<20;i++) m_Trnhitsfoi[i] = 0;
  // MC info
  m_TrMCp0 = -1000;
  m_TrnLinks = 0;
  m_TrzDecay = -1000.;
  m_TrType = 1000;

  return;
}
//=====================================================================
//  Fill MuonPID Info      
//====================================================================
void MuonPIDChecker::getMuonPIDInfo(const LHCb::Track &track, const LHCb::MuonPIDs* pMuids) {

  int nMuonPIDs=0; //number of MuonPIDs associated to track
  // link between track and MuonPID 
  for (const auto& muid : *pMuids) {
    if (muid->idTrack() == &track){  // found Associated MuonPID
      nMuonPIDs++;
      // Preselection
      if (muid->PreSelMomentum() && muid->InAcceptance() ){
        m_TrIsPreSel=1;
        m_TrIsMuon = muid->IsMuon();
        m_TrIsMuonLoose = muid->IsMuonLoose();
        if (m_TrIsMuonLoose< m_TrIsMuon){ // Sanity Check  
          Warning("getMuonPIDInfo:: Muon Track IsMuon < IsMuonLoose").ignore(); 
          if ( msgLevel(MSG::DEBUG) ) debug() << "getMuonPIDInfo::Muon Track" << m_nTr
                                              <<"  IsMuon ="<< m_TrIsMuon <<
                                        "< IsMuonLoose= "<< m_TrIsMuonLoose << endmsg;
            
        }
        m_TrMuonLhd= muid->MuonLLMu();
        m_TrNMuonLhd = muid->MuonLLBg();
        m_TrNShared = muid->nShared();
        if ( msgLevel(MSG::DEBUG) ) debug() << "getMuonPIDInfo::Muon Track" << m_nTr
                                            <<"  TrMuonLhd ="<< m_TrMuonLhd << " TrNMuonLhd= "<< m_TrNMuonLhd<< endmsg;
	    
      } // Pre-selection
    }  // Association to Track
  } // loop over MuonPIDs
  if (nMuonPIDs>1) Warning("getMuonPIDInfo:: nMuonPIDs associated to track >1").ignore();
  return;
}
//=====================================================================
//  Fill Muon Track Info      
//====================================================================
void MuonPIDChecker::getMuonTrackInfo(const LHCb::Track& track, const LHCb::Tracks* muTracks) {

  LHCb::Tracks::const_iterator imuTrack;
  std::vector<int> assocHits(m_NRegion*m_NStation);
  for (imuTrack = muTracks->begin() ; imuTrack != muTracks->end() ; imuTrack++){
    
    // Get Track ancestors
    const LHCb::Track *trParent=NULL;
    int nmothers=0;
    const SmartRefVector<LHCb::Track> Trmothers=(*imuTrack)->ancestors();
    for (SmartRefVector<LHCb::Track>::const_iterator imother=Trmothers.begin();
         imother!=Trmothers.end();imother++){
      trParent = *imother;
      nmothers++;
    }
    if (trParent == NULL){ 
      Warning( "getMuonTrackInfo:: failed to get Muon Track ancestor", StatusCode::SUCCESS,0).ignore() ; 
      continue;
    }
    // if muon track ancestor is the current track get info 
    if (trParent == &track){
    
      // Sanity Checks   
      unsigned int muTrPS=0;
      if ( (*imuTrack)->info(301,0) && (*imuTrack)->info(302,0)) muTrPS=1;
      if (muTrPS != m_TrIsPreSel) 
        Warning("getMuonTrackInfo:: PS flag is different for Muon Track and MuonPID").ignore();
      unsigned int TrIsMuonLoose = (unsigned int)(*imuTrack)->info(303,0);
      if (TrIsMuonLoose != m_TrIsMuonLoose) 
        Warning("getMuonTrackInfo:: IML flag is different for Muon Track and MuonPID").ignore();
      unsigned int TrIsMuon = (unsigned int)(*imuTrack)->info(304,0);
      if (TrIsMuon != m_TrIsMuon) 
      	Warning("getMuonTrackInfo:: IM flag is different for Muon Track and MuonPID").ignore();
 
      // Get Info
      m_Trquality = (*imuTrack)->info(300,0);
      m_TrChi2 = (*imuTrack)->chi2PerDoF();
      m_TrDist2 = (*imuTrack)->info(305,0);
      m_TrCLquality = (*imuTrack)->info(308,0);
      m_TrCLarrival = (*imuTrack)->info(309,0);
      if ( msgLevel(MSG::DEBUG) ) {
        debug() << "getMuonTrackInfo::Muon Track" << m_nTr
                <<"  Trquality ="<< m_Trquality << " TrChi2 = "<< m_TrChi2 << endmsg;
        debug() << "getMuonTrackInfo::Muon Track" << m_nTr
                <<"  TrDist2   ="<< m_TrDist2 << " TrCLquality="<< m_TrCLquality << endmsg;
        debug() << "getMuonTrackInfo::Muon Track" << m_nTr
                <<"  TrCLArrival   ="<< m_TrCLarrival << endmsg;
      }
      // Look at coords
      for (const auto& id : (*imuTrack) -> lhcbIDs() ) {
        if (!id.isMuon()) continue;
        LHCb::MuonTileID mutile = id.muonID();
        int region = mutile.region();
        int station = mutile.station();
        int nStatReg = station*m_NRegion+region;
        m_Trnhitsfoi[nStatReg]++;
        if ( msgLevel(MSG::DEBUG) ) debug() << "getMuonTrackInfo:: " << 
                                      "nHits in FOI in MR"<<nStatReg << " = "<<m_Trnhitsfoi[nStatReg]<<endmsg;
      } //end of loop over lhcbIDs 
    }
  }
}
//=====================================================================
//  Fill Multiplicity Plots         
//====================================================================
void MuonPIDChecker::fillMultiplicityPlots(int level) {

  //fill multiplicity plots
  if (level>0){
    plot1D(m_nTrIsMuonLoose, "hNIMLtracks","IsMuonLoose Track multiplicity", -0.5, 11.5, 12);
    plot1D(m_nTrIsMuon, "hNIMtracks","IsMuonLoose Track multiplicity", -0.5, 11.5, 12);
    if (m_nTr>0) plot1D((double)m_nTrIsMuonLoose/m_nTr, "hNIMLtracksRatio","#IsMuonLoose/#Tracks", 0., 1.1, 22);
  }
  if (level>1){
    plot1D(m_nTr, "hNtracks","Track multiplicity", -0.5, 60.5, 61);
    plot1D(m_nTrPreSel, "hPSNtracks","PreSelection Track multiplicity", -0.5, 50.5, 51);
    if (m_nTrPreSel>0){
      plot1D((double)m_nTrIsMuonLoose/m_nTrPreSel, "hNIMLPStracksRatio","#IsMuonLoose/#PSTracks", 0., 1.1, 22);
      plot1D((double)m_nTrIsMuon/m_nTrPreSel, "hNIMPStracksRatio","#IsMuon/#PSTracks", 0., 1.1, 22);
    }
    if (m_nTr>0) plot1D((double)m_nTrIsMuon/m_nTr, "hNIMtracksRatio","#IsMuon/#Tracks", 0., 1.1, 22);
  }
  return;
}
//=====================================================================
//  Fill Plots for PreSelected Tracks        
//====================================================================
void MuonPIDChecker::fillPreSelPlots(int level) {
  if (level>1){
    plot1D( m_TrRegionM2, "hPSRegion", "MS Region for PS  tracks",0.5,4.5,4); 
    plot1D( m_Trp0, "hPSMomentum", "PreSelected Track Momentum (GeV/c^2)", -25., 25., 100);
    plot1D( m_TrpT, "hPSPT", "PreSelected Track p_T (GeV/c^2)", -5., 5., 100);
    plot1D( m_TrIsMuonLoose, "hIML_PS"  , " IsMuonLoose for PS Tracks ", -0.5 , 1.5, 2 );
    plot1D( m_TrIsMuon, "hIM_PS"  , " IsMuon for PS Tracks ", -0.5 , 1.5, 2 );
  }
  if (level>2){
    //Efficiencies
    profile1D( m_Trp0, m_TrIsMuonLoose, "hEffvsP_IML", "IML Efficiency vs P (GeV/c^2)", -25., 25., 100);
    profile1D( m_TrpT, m_TrIsMuonLoose, "hEffvsPT_IML", "IML Efficiency vs PT (GeV/c^2)", -5., 5., 100);
    profile1D( m_Trp0, m_TrIsMuon, "hEffvsP_IM", "IM Efficiency vs P (GeV/c^2)", -25., 25., 100);
    profile1D( m_TrpT, m_TrIsMuon, "hEffvsPT_IM", "IM Efficiency vs PT (GeV/c^2)", -5., 5., 100);
  }
  return; 
}
//=====================================================================
//  Fill Plots for IsMuonLooseCandidates 
//====================================================================
void MuonPIDChecker::fillIMLPlots(int level) {

  if (2<m_nMonitorCuts && exp(m_TrMuonLhd)<m_monitCutValues[2]) m_ntotTr[4]++;
  if (4<m_nMonitorCuts && (m_TrMuonLhd-m_TrNMuonLhd)>m_monitCutValues[4]) m_ntotTr[6]++;
  if (6<m_nMonitorCuts && m_TrNShared<m_monitCutValues[6]) m_ntotTr[8]++;
  if ( msgLevel(MSG::DEBUG) ) debug() << "fillIMLPlots:: MuProb="<<exp(m_TrMuonLhd)<< 
                                " DLL="<<(m_TrMuonLhd-m_TrNMuonLhd) << " IM="<< m_TrIsMuon 
                                      <<" nShared=" << m_TrNShared<< endmsg;
  if ( msgLevel(MSG::DEBUG) ) for (int i=0;i<m_nMonitorCuts+2;i++)
	  debug() << "IMLPlots:: "<<i<<"-> m_ntotTr[i]"<<m_ntotTr[i]<< endmsg;

  if (level>0){
    plot1D( m_Trp0, "hIMLMomentum", "IsMuonLoose Candidate Momentum (GeV/c^2)", -25., 25., 100);
    plot1D( m_TrpT, "hIMLPT", "IsMuonLoose Candidate p_T (GeV/c^2)", -5., 5., 100);
    plot1D( m_TrRegionM2, "hIMLRegion", "MS Region for IML  tracks",0.5,4.5,4); 
  }
  if (level>1){
    plot1D( m_TrNShared, "hNShared_IML"  , " NShared for PS Tracks ", -0.5 , 5.5, 6 );
    plot1D( m_TrDist2, "hDist2_IML", "Muon Dist for IML candidates", 0., 600., 100); 
    plot1D( exp(m_TrMuonLhd), "hProbMu_IML", "Muon Probability for IML candidates", -0.1, 1.1, 60);
    plot1D( exp(m_TrNMuonLhd), "hProbNMu_IML", "Non-Muon Probability for IML candidates", -0.1, 1.1, 60);
    plot1D( m_TrMuonLhd-m_TrNMuonLhd, "hMuDLL_IML", "Muon DLL for IML candidates",m_DLLlower, m_DLLupper, m_DLLnbins); 
    plot1D( m_trackX[1], "hNIMLvsXM2", "MS X position at M2 for IML  tracks",-5500, 5500, 550);
    plot1D( m_trackY[1], "hNIMLvsYM2", "MS Y position at M2 for IML  tracks",-5500, 5500, 550);

    GaudiAlg::HistoID hname1 = "hDist2_IML_R"+std::to_string(m_TrRegionM2);
    std::string htitle1= "Muon Dist for IML candidates at R"+std::to_string(m_TrRegionM2);
    plot1D( m_TrDist2, hname1 , htitle1 , 0., 600., 100); 

    hname1 = "hProbMu_IML_R"+std::to_string(m_TrRegionM2);
    htitle1= "Muon Probability for IML candidates at R"+std::to_string(m_TrRegionM2);
    plot1D( exp(m_TrMuonLhd), hname1 , htitle1 , -0.1, 1.1, 60);

  }
  if (level>2){

    plot1D( m_TrIsMuon, "hIM_IML"  , " IsMuon for IML Tracks ", -0.5 , 1.5, 2 );
    GaudiAlg::HistoID hname = "hDLL_IML_R"+std::to_string(m_TrRegionM2);
    std::string htitle = "Muon DLL for IML candidates at R"+std::to_string(m_TrRegionM2);
    plot1D( m_TrMuonLhd-m_TrNMuonLhd, hname , htitle , m_DLLlower, m_DLLupper, m_DLLnbins);
  }
  if (level>3){     
    plot1D(m_TrChi2, "hChi2_IML", "Chi2 per nDOF for IML Candidates", 0., 200., 100);
    plot1D(m_Trquality, "hQuality_IML", "Track Quality for IML Candidates", 0., 200., 100);
    plot1D(m_TrCLquality, "hCLQuality_IML", "Track CL Quality for IML Candidates", -0.1, 1.1, 60);
    plot1D(m_TrCLarrival, "hCLArrival_IML", "Track CL Arrival for IML Candidates", -0.1, 1.1, 60);
    profile1D( std::abs(m_Trp0), exp(m_TrMuonLhd), "hProbMuvsP_IML", "Mean Muon Prob vs p for IML tracks", 0.,100.,100);
    profile1D( std::abs(m_Trp0), exp(m_TrNMuonLhd), "hNProbMuvsP_IML", "Mean non-Muon Prob vs P for IML tracks ", 0.,100.,100);
  }
  return;
}
//=====================================================================
//  Fill Plots for IsMuon Candidates    
//====================================================================
void MuonPIDChecker::fillIMPlots(int level) {
  if (3<m_nMonitorCuts && exp(m_TrMuonLhd)<m_monitCutValues[3]) m_ntotTr[5]++;
  if (5<m_nMonitorCuts && (m_TrMuonLhd-m_TrNMuonLhd)>m_monitCutValues[5]) m_ntotTr[7]++;
  if (7<m_nMonitorCuts && m_TrNShared<m_monitCutValues[7]) m_ntotTr[9]++;
  if ( msgLevel(MSG::DEBUG) ) for (int i=0;i<m_nMonitorCuts+2;i++)
	  debug() << "IMPlots:: "<<i<<"-> m_ntotTr[i]"<<m_ntotTr[i]<< endmsg;
  if (level>1){
    plot1D( m_Trp0, "hIMMomentum", "IsMuon candidate Momentum (GeV/c^2)", -25., 25., 100);
    plot1D( m_TrpT, "hIMPT", "IsMuon candidate p_T (GeV/c^2)", -5., 5., 100);
    plot1D( m_TrRegionM2, "hIMRegion", "MS Region for IM tracks",0.5,4.5,4); 
    plot1D( m_TrNShared, "hNShared_IM"  , " NShared for PS Tracks ", -0.5 , 5.5, 6 );
    plot1D( m_TrDist2, "hDist2_IM", "Muon Dist for IM candidates", 0., 600., 100); 
    GaudiAlg::HistoID hname = "hDist2_IM_R"+std::to_string(m_TrRegionM2);
    std::string htitle= "Muon Dist for IML candidates at R"+std::to_string(m_TrRegionM2);
    plot1D( m_TrDist2, hname , htitle , 0., 600., 100); 
    plot1D( exp(m_TrMuonLhd), "hProbMu_IM", "Muon Probability for IM candidates", -0.1, 1.1, 60);
    hname = "hProbMu_IM_R"+std::to_string(m_TrRegionM2);
    htitle= "Muon Probability for IM candidates at R"+std::to_string(m_TrRegionM2);
    plot1D( exp(m_TrMuonLhd), hname , htitle , -0.1, 1.1, 60);
    plot1D( exp(m_TrNMuonLhd), "hProbNMu_IM", "Non-Muon Probability for IM candidates", -0.1, 1.1, 60);
    plot1D( m_TrMuonLhd-m_TrNMuonLhd, "hMuDLL_IM", "Muon DLL for IM candidates",m_DLLlower, m_DLLupper, m_DLLnbins); 
    plot1D( m_trackX[1], "hNIMvsXM2", "MS X position at M2 for IM  tracks",-5500, 5500, 550);
    plot1D( m_trackY[1], "hNIMvsYM2", "MS Y position at M2 for IM  tracks",-5500, 5500, 550);
  }
  if (level>2){
    GaudiAlg::HistoID hname = "hDLL_IM_R"+std::to_string(m_TrRegionM2);
    std::string htitle = "Muon DLL for IM candidates at R"+std::to_string(m_TrRegionM2);
    plot1D( m_TrMuonLhd-m_TrNMuonLhd, hname , htitle , m_DLLlower, m_DLLupper, m_DLLnbins);
  }
  if (level>3){
    plot1D(m_TrChi2, "hChi2_IM", "Chi2 per nDOF for IM Candidates", 0., 200., 100);
    plot1D(m_Trquality, "hQuality_IM", "Track Quality for IM Candidates", 0., 200., 100);
    plot1D(m_TrCLquality, "hCLQuality_IM", "Track CL Quality for IM Candidates", -0.1, 1.1, 60);
    plot1D(m_TrCLarrival, "hCLArrival_IM", "Track CL Arrival for IM Candidates", -0.1, 1.1, 60);
    profile1D( std::abs(m_Trp0), exp(m_TrMuonLhd), "hProbMuvsP_IM", "Mean Muon Prob vs p for IM tracks", 0.,100.,100);
    profile1D( std::abs(m_Trp0), exp(m_TrNMuonLhd), "hNProbMuvsP_IM", "Mean non-Muon Prob vs P for IM tracks ", 0.,100.,100);
  }
  return;
}
//=====================================================================
//  Fill Plots for Hit Multiplicities    
//====================================================================
void MuonPIDChecker::fillHitMultPlots(int level) {

  GaudiAlg::HistoID hname; 
  std::string htitle;
  if (level>2){
    std::vector<unsigned int>  nhitsfoiS(m_NStation);    
    unsigned int NhitsFOI=0;
    for (unsigned int i=0; i<m_NStation; i++){
      for (unsigned int j=0; j<m_NRegion;j++){
        nhitsfoiS[i]+=m_Trnhitsfoi[i*m_NRegion+j];
      }
      NhitsFOI += nhitsfoiS[i];

      hname = "hAvNHhitsFOIvsR_M"+std::to_string(i+1);
      htitle= "Mean Number of hits in FOI for M"+std::to_string(i+1)+" vs MS Region";
      profile1D( m_TrRegionM2, nhitsfoiS[i],hname ,htitle , 0.5, 4.5, 4);

      if (level > 3){
	hname = "hNHhitsFOIvsR_M"+std::to_string(i+1);
	htitle= "Number of hits in FOI for M"+std::to_string(i+1)+" vs MS Region";
        plot2D(m_TrRegionM2, nhitsfoiS[i], hname, htitle, 0.5,4.5,-0.5,10.5,4,11); 
        
	hname = "hNhitsFOIvsX_M"+std::to_string(i+1);
	htitle= "Number of hits in FOI for M"+std::to_string(i+1)+" vs MS X Pos.";
        plot2D(nhitsfoiS[i], m_trackX[i], hname, htitle, -0.5,10.5, -5000, 5000, 11, 200); 

	hname = "hNhitsFOIvsY_M"+std::to_string(i+1);
	htitle= "Number of hits in FOI for M"+std::to_string(i+1)+" vs MS Y Pos.";
        plot2D(nhitsfoiS[i], m_trackY[i], hname, htitle, -0.5,10.5, -5000, 5000, 11, 200); 

	hname = "hAvNhitsFOIvsX_M"+std::to_string(i+1);
	htitle= "Mean Number of hits in FOI for M"+std::to_string(i+1)+" vs MS X Pos.";
        profile1D(m_trackX[i], nhitsfoiS[i], hname, htitle, -5000, 5000, 200); 

	hname = "hAvNhitsFOIvsY_M"+std::to_string(i+1);
	htitle= "Mean Number of hits in FOI for M"+std::to_string(i+1)+" vs MS Y Pos.";
        profile1D(m_trackY[i], nhitsfoiS[i], hname, htitle, -5000, 5000, 200); 
      }
    }
    hname = "hAvTotNhitsFOIvsR";
    htitle= "Mean Number of hits in FOI vs Region (M2)";
    profile1D(m_TrRegionM2, NhitsFOI, hname, htitle ,0.5,4.5,4); 

    hname = "hAvTotNhitsFOIvsX";
    htitle= "Mean Number of hits in FOI vs X (M2)";
    profile1D(m_trackX[1], NhitsFOI, hname, htitle ,-5000, 5000, 200);

    hname = "hAvTotNhitsFOIvsY";
    htitle= "Mean Number of hits in FOI vs Y (M2)";
    profile1D(m_trackY[1], NhitsFOI, hname, htitle ,-5000, 5000, 200);
  }
  return;
}
//=====================================================================
//  Fill MuonPID Info      
//====================================================================
StatusCode MuonPIDChecker::getTrackInfo(const LHCb::Track& track) {
  // get momentum
  // get state in zero position 
  const LHCb::State& stateP0 = track.firstState();
  if( std::abs( stateP0.qOverP() ) > 0.001 / Gaudi::Units::GeV ) {
    m_Trp0 = (1./stateP0.qOverP())/Gaudi::Units::GeV;
    m_TrpT = (stateP0.pt()/Gaudi::Units::GeV)*(int)(m_Trp0/fabs(m_Trp0));
  }else if( stateP0.qOverP() > 0. ) {
    m_Trp0 = 1000. / Gaudi::Units::GeV;
    m_TrpT = 1000. / Gaudi::Units::GeV;
  }else {
    m_Trp0 = -1000. / Gaudi::Units::GeV;
    m_TrpT = -1000. / Gaudi::Units::GeV;
  }
  if ( msgLevel(MSG::DEBUG) ) debug() << "getTrackInfo:: TrP0=" << m_Trp0 << " TrPT= "<< m_TrpT << endmsg;

  StatusCode sc = trackExtrapolate(track);
  if (sc.isFailure()) Warning("getTrackInfo:: Failed to extrapolate track ", StatusCode::FAILURE,0).ignore();

  if ( msgLevel(MSG::DEBUG) ){

    for (unsigned int i=0;i<m_NStation;i++) 
	    debug() << "getTrackInfo:: M"<<i<<": TrX =" << m_trackX[i]<< endmsg;
  }
  return StatusCode::SUCCESS;
}
//=====================================================================
//  Rate Error             
//====================================================================
double MuonPIDChecker::getRateError( double rate , int den){
  return 100.*sqrt(rate*(1.-rate)/(double)den);
}
