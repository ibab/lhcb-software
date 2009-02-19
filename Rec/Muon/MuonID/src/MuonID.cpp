// Include files
#include <cstdio>
#include <map>
#include <vector>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "MuonID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonID
//
// first version
// 02/05/2002 : David Hutchcroft
//
// new FOI parameterization, acceptance cut for inner chamber holes,
// back to original algorithm; code expects all five chambers, even though
// it only checks hits in M2-M5 by now.
// 09/04/2003 : jtmn, miriam
//
// new Event Model version: calculates de muon LL for muons and non muons
// and the number of tracks that share hits, which are the new members of 
// the muonPID object. 
// 14/12/2005 : Erica Polycarpo, Miriam Gandelman 
//
// new FOI uses 3 parameters
// 16/10/2006 : Erica Polycarpo, Miriam Gandelman
//
// small fixes (DLL binning for pions, Landau with 9 parameters, muonPID key)
// 08/02/2007 : Erica Polycarpo, Miriam Gandelman 
//
// added the method makeMuonTrack to create a track object for each MuonPID
// added a foifactor to enlarge the foi if needed. default = 1.
// 07/05/2007 : Erica Polycarpo, Miriam Gandelman
//
// added a smartRef to the muonTracks
// 30/06/2007 : Miriam Gandelman
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonID );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonID::MuonID( const std::string& name,
                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) {

  // Source of track to ID
  declareProperty("TrackLocation",
                  m_TracksPath = LHCb::TrackLocation::Default);

  // Destination of MuonPID
  declareProperty("MuonIDLocation",
                  m_MuonPIDsPath = LHCb::MuonPIDLocation::Default);

  // Destination of MuonPID
  declareProperty("MuonTrackLocation",
                  m_MuonTracksPath = LHCb::TrackLocation::Muon);

  // Pre-selection momentum
  declareProperty( "PreSelMomentum", m_PreSelMomentum = 3000.0);

  // Different depths of stations considered in different momentum ranges
  declareProperty( "MomentumCuts", m_MomentumCuts );

  // function that defines the field of interest size
  // here momentum is scaled to Gaudi::Units::GeV....
  // new formula: p(1) + p(2)*momentum + p(3)*exp(-p(4)*momentum)

  declareProperty( "XFOIParameter1", m_xfoiParam1 );
  declareProperty( "XFOIParameter2", m_xfoiParam2 );
  declareProperty( "XFOIParameter3", m_xfoiParam3 );
  declareProperty( "YFOIParameter1", m_yfoiParam1 );
  declareProperty( "YFOIParameter2", m_yfoiParam2 );
  declareProperty( "YFOIParameter3", m_yfoiParam3 );

  declareProperty("FOIfactor",m_foifactor = 1.);

  declareProperty("distMuon",m_distMuon);
  declareProperty("distPion",m_distPion);

}

//=============================================================================
// Destructor
//=============================================================================
MuonID::~MuonID() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuonID::initialize() {

  // Sets up various tools and services
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( sc.isFailure() ) { return sc; }

  info()   << " MuonID v5r2 " << endmsg;

  debug()  << "==> Initialise" << endreq;
  debug()  << "Input tracks in: " << m_TracksPath << endreq;
  debug()  << "Output MuonPID in: " << m_MuonPIDsPath<< endreq;

  m_ntotmu=0;
  m_NStation = 0;
  m_NRegion = 0;
  MuonBasicGeometry basegeometry( detSvc(),msgSvc());
  m_NStation= basegeometry.getStations();
  m_NRegion = basegeometry.getRegions();
  int i=0;
  while(i<m_NStation){
    m_stationNames.push_back(basegeometry.getStationName(i));
    // debug()   <<" station "<<i<<" "<<m_stationNames[i]<<endreq;
    i++;
  }

  // set the size of the local vectors
  m_padSizeX.resize(m_NStation * m_NRegion);
  m_padSizeY.resize(m_NStation * m_NRegion);
  m_regionInnerX.resize(m_NStation);
  m_regionOuterX.resize(m_NStation);
  m_regionInnerY.resize(m_NStation);
  m_regionOuterY.resize(m_NStation);

  // fill local arrays of pad sizes and region sizes
  m_mudet=getDet<DeMuonDetector>("/dd/Structure/LHCb/DownstreamRegion/Muon");
  int station,region;
  for(station = 0 ; station < m_NStation ; station++ ){
    m_regionInnerX[station] = m_mudet->getInnerX(station);
    m_regionOuterX[station] = m_mudet->getOuterX(station);
    m_regionInnerY[station] = m_mudet->getInnerY(station);
    m_regionOuterY[station] = m_mudet->getOuterY(station);
    m_stationZ[station] = m_mudet->getStationZ(station);
    for(region = 0 ; region < m_NRegion ; region++ ){
      m_padSizeX[station * m_NRegion + region]=m_mudet->getPadSizeX(station,region);
      m_padSizeY[station * m_NRegion + region]=m_mudet->getPadSizeY(station,region);

      if(m_padSizeX[station * m_NRegion + region]==0){
        error() << "Muon Chamber Pad Size could not be retrieved !!!" <<  endreq;
        return StatusCode::FAILURE;
      }
    }
  }

  debug()  << "-Geometry information ----------------"<< endreq;
  debug()  << "Outer X M1 = " << m_regionOuterX[0] << endreq;
  debug()  << "Outer Y M1 = " << m_regionOuterY[0] << endreq;
  debug()  << "Outer X M5 = " << m_regionOuterX[4] << endreq;
  debug()  << "Outer Y M5 = " << m_regionOuterY[4] << endreq;
  debug()  << "Inner X M1 = " << m_regionInnerX[0] << endreq;
  debug()  << "Inner Y M1 = " << m_regionInnerY[0] << endreq;
  debug()  << "Inner X M5 = " << m_regionInnerX[4] << endreq;
  debug()  << "Inner Y M5 = " << m_regionInnerY[4] << endreq;
  debug()  << "stationZ M1 = " << m_stationZ[0] << endreq;
  debug()  << "stationZ M2 = " << m_stationZ[1] << endreq;
  debug()  << "stationZ M3 = " << m_stationZ[2] << endreq;
  debug()  << "stationZ M4 = " << m_stationZ[3] << endreq;
  debug()  << "stationZ M5 = " << m_stationZ[4] << endreq;
  debug()  << "--------------------------------------"<< endreq;

  if( m_MomentumCuts.empty() ||
      m_xfoiParam1.size() != (unsigned)m_NStation*m_NRegion ||
      m_xfoiParam2.size() != (unsigned)m_NStation*m_NRegion ||
      m_xfoiParam3.size() != (unsigned)m_NStation*m_NRegion ||
      m_yfoiParam1.size() != (unsigned)m_NStation*m_NRegion ||
      m_yfoiParam2.size() != (unsigned)m_NStation*m_NRegion ||
      m_yfoiParam3.size() != (unsigned)m_NStation*m_NRegion
      ){
    err() << "OPTIONS initialising MuonID are missing"
          << " or wrong size for " << m_NStation << " stations and "
          << m_NRegion << " regions"
          << endreq;
    return StatusCode::FAILURE;
  }

  if( m_MomentumCuts.size() != 2 ){
    err()
      << "OPTIONS are wrong:"
      << " size of MomentumCuts vector is not correct"
      << endreq;
    return StatusCode::FAILURE;
  }

  debug()  << " Momentum bins are (MeV/c) " <<endreq;
  debug()  << " PreSelMomentum = "<<  m_PreSelMomentum << endreq;

  std::vector<double>::const_iterator iMom;
  for(iMom = m_MomentumCuts.begin() ; iMom != m_MomentumCuts.end() ; iMom++){
    debug()  << "Mom. cuts = " << *iMom << endreq ;
  }

  if( m_distPion.size() != 9 || m_distMuon.size() != 9){
    err() << "OPTIONS are wrong:"
          <<" size of m_distPion or m_distMuon vector is not correct"
          << endreq;
    return StatusCode::FAILURE;
  }

  //  Load MeasurementProvider Tool
  m_measProvider = tool<IMeasurementProvider>("MeasurementProvider");

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonID::execute() {

  debug()  << "==> Execute" << endreq;
  m_nmu = 0;

  StatusCode sc = fillCoordVectors();
  if(sc.isFailure()){
    return sc;
  }

  LHCb::Tracks* trTracks = get<LHCb::Tracks>(m_TracksPath);
  if ( trTracks==0 ){
    err() << " Cannot retrieve Tracks " << endreq;
    return StatusCode::FAILURE;
  }
  debug()  << "Number of input tracks for MuonID: " << trTracks->size() << endreq;

  LHCb::MuonPIDs * pMuids = new LHCb::MuonPIDs();
  LHCb::Tracks * mutracks = new LHCb::Tracks();

  LHCb::Tracks::const_iterator iTrack;
  for( iTrack = trTracks->begin() ; iTrack != trTracks->end() ; iTrack++){
    // in the clone killed output we want only
    // unique && (matched || forward || downstream)
    if(!(*iTrack)->checkFlag(LHCb::Track::Clone)  &&
       ((*iTrack)->checkType(LHCb::Track::Long) ||
        (*iTrack)->checkType(LHCb::Track::Downstream))){

      // do the track extrapolations
      StatusCode sc = trackExtrapolate(*iTrack);
      if ( sc.isFailure() ){
        warning() << " trackExtrapolate failed for track " << *iTrack << endreq;
        continue;
      }

      // Do the complete ID, calculating IsMuon and likelihoods
      LHCb::MuonPID * pMuid = new LHCb::MuonPID;
      pMuid->setIDTrack(*iTrack);
      sc = doID(pMuid);
      if(sc.isFailure()){
        warning() << " doID failed for track " << *iTrack << endreq;
      }

      pMuids->insert( pMuid, (*iTrack)->key() );

      if (pMuid->IsMuon()) {
	LHCb::Track* mutrack = makeMuonTrack(*pMuid);
	mutracks->insert( mutrack, (*iTrack)->key() );  
        pMuid->setMuonTrack( mutrack );
      }

      sc = calcSharedHits(pMuid, pMuids);
      if (sc.isFailure()){
        warning() << " calcSharedHits failed for track " << *iTrack << endreq;
      }

    } // long tracks
  }  // loop over tracks

  // Debug : muon identification event summary
  debug()  << "Number of MuonPID objects created: " << pMuids->size()
           << endreq;
  debug()  << "Number of tracks with IsMuon = True : " << m_nmu
           << endreq;
  m_ntotmu += m_nmu;

  // Register the MuonIDs container to the TES
  put(pMuids,m_MuonPIDsPath);
  // Register the PIDTracks container to the TES
  put(mutracks,m_MuonTracksPath);

  clearCoordVectors();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonID::finalize() {

  debug()  << "==> Finalize" << endreq;
  debug()  << "==> Total number of tracks with IsMuon=1 : " <<
    m_ntotmu << endreq;

  // Execute base class method
  return GaudiAlgorithm::finalize();
}

//=============================================================================
// fill vectors of x,y,z positions for the MuonCoords
//=============================================================================
StatusCode MuonID::fillCoordVectors(){

  clearCoordVectors(); // sets up Coord vectors of vectors

  // get the MuonCoords for each station in turn
  LHCb::MuonCoords* coords = get<LHCb::MuonCoords>(LHCb::MuonCoordLocation::MuonCoords);
  if ( coords==0 ) {
    err() << " Cannot retrieve MuonCoords " << endreq;
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
      warning() << "Failed to get x,y,z of tile " << tile << endreq;
      continue;
    }
    m_coordPos[station*m_NRegion+region].
      push_back(coordExtent_(x,dx,y,dy,*iCoord));
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// Clear the coord vector
//=============================================================================
void MuonID::clearCoordVectors(){
  m_coordPos.clear();
  m_coordPos.resize(m_NStation * m_NRegion);
  m_muonMap.clear();
}

//=============================================================================
// Do the identification
//=============================================================================
StatusCode MuonID::doID(LHCb::MuonPID *pMuid){

  // Initializes data members
  pMuid->setIsMuon(0);
  pMuid->setNShared(0);
  pMuid->setPreSelMomentum(0);
  pMuid->setInAcceptance(0);
  pMuid->setMuonLLMu(-10000.);
  pMuid->setMuonLLBg(-10000.);

  // First do a preselection:
  // track is in acceptance? Track has minimum momentum?
  bool passed;
  StatusCode sc = preSelection( pMuid,passed );
  if(sc.isFailure()){
    warning() <<" preSelection failed to MuonPID object" << pMuid << endreq;
    return sc;
  }

  // OK: track failed preselection say so and return
  if(!passed){
    debug() << " Track failed preselection " << endreq;
    return StatusCode::SUCCESS;
  }

  // find the coordinates in the fields of interest
  sc = setCoords( pMuid );
  if(sc.isFailure()){
    warning() <<" setCoords failed to MuonPID object" << pMuid << endreq;
    return sc;
  }

  // apply ID: depends on the track momentum

  // find the momentum bin we are in
  // it can be 0, 1 or 2
  int momentumBin = 0;

  // m_PreSelMomentum = 3000 MeV
  if (m_MomentumPre> m_PreSelMomentum && m_MomentumPre < m_MomentumCuts[0]) {momentumBin = 0;}
  if (m_MomentumPre >= m_MomentumCuts[0] && m_MomentumPre < m_MomentumCuts[1]) {momentumBin = 1;}
  if (m_MomentumPre >= m_MomentumCuts[1]) {momentumBin = 2;}

  // now: implement original algorithm:
  // bin 0 M1.and.M2.and.M3
  // bin 1 M1.and.M2.and.M3.and.(M4.or.M5)
  // bin 2 M1.and.M2.and.M3.and.M4.and.M5
  bool isMuon=false;
  int station;
  if (momentumBin == 0) {
    if (m_occupancy[1]>0 && m_occupancy[2]>0) {isMuon = true;}
  }
  if (momentumBin == 1) {
    if (m_occupancy[1]>0 && m_occupancy[2]>0 && (m_occupancy[3]>0 ||
                                                 m_occupancy[4]>0)) {isMuon = true;}
  }

  if (momentumBin == 2) {
    if (m_occupancy[1]>0 && m_occupancy[2]>0 && m_occupancy[3]>0 &&
        m_occupancy[4]>0) {isMuon = true;}
  }

  pMuid->setIsMuon(isMuon);

  // calculate MuProb
  float MuProb = calcMuProb( pMuid );
  debug()  << "MuProb= " << MuProb <<endreq;

  // calculate Muon DLL
  sc = calcMuonLL( pMuid );
  if(sc.isFailure()){
    warning() << " calcMuonLL failed to MuonPID object " << pMuid << endreq;
  }

  //increment number of IsMuon=true tracks for monitoring
  if(pMuid->IsMuon()) m_nmu++;

  debug()  << "IsMuon = " << pMuid->IsMuon() 
	<< " bin = "   << momentumBin <<" " << " p = " << m_MomentumPre << endreq; 
  debug()  << " coord in FOI ("; 
       for(station = 0; station < m_NStation ; station++ ){
  debug() << m_occupancy[station] << "," ;
  }
  debug() << ")" << endreq;

  return StatusCode::SUCCESS;
}


//=============================================================================
// Calculate MuProb
//=============================================================================
// if found a muon make a probability from the DxDz matching
float MuonID::calcMuProb(LHCb::MuonPID * pMuid){

  if(pMuid->IsMuon()){
    // slope calculated in M2 and M3
    m_xMatchStation = 1;
    // find slope difference between track and Coords in M2-M3
    double coordSlopeX = ((m_CoordX[m_xMatchStation] -
                           m_CoordX[m_xMatchStation+1])/
                          (m_stationZ[m_xMatchStation] -
                           m_stationZ[m_xMatchStation+1]));
    double dSlopeX = fabs( m_trackSlopeX - coordSlopeX );

    // formula to make this a probability is
    // dSlopeX < 0.005 = 1.0
    // 0.005 < dSlopeX < 0.085 = 1. - ( (dSlopeX-0.005) / 0.08 )
    // 0.085 < dSlopeX = 0.
    if( dSlopeX < 0.005 ) {
      return 1.0;
    }else if( dSlopeX > 0.085 ){
      return 0.0;
    } else {
      return (1.0 - ( (dSlopeX-0.005) / 0.08 ) );
    }
  }else{
    // not passed selection as a muon so probability is zero
    return 0.0;
  }
}

//=============================================================================
// Calculate the muon DLL
//=============================================================================
StatusCode MuonID::calcMuonLL(LHCb::MuonPID * muonid){

  if ( !muonid->IsMuon() ) {
    return StatusCode::SUCCESS;
  }

  const LHCb::Track* pTrack = muonid->idTrack();
  // do the track extrapolations
  StatusCode sc = trackExtrapolate(pTrack);
  if (!sc){
    warning() << "trackExtrapolate fails for track" << pTrack << endreq;
    muonid->setMuonLLMu(-10000.);
    muonid->setMuonLLBg(-10000.);
    return StatusCode::FAILURE;
  }

  int nhits=0;
  double dist = 0.;
  std::vector<LHCb::MuonCoord*> & mcoord = m_muonMap[muonid];
  std::vector<LHCb::MuonCoord*>::const_iterator iCoord;
  for ( iCoord = mcoord.begin() ; iCoord != mcoord.end() ; iCoord++ ){

    double x,dx,y,dy,z,dz;
    LHCb::MuonTileID tile=(*iCoord)->key();
    StatusCode sc =
      m_mudet->Tile2XYZ(tile,x,dx,y,dy,z,dz);
    if (sc.isFailure()){
      warning() << "Failed to get x,y,z of tile " << tile << endreq;
      continue;
    }

    int station = (*iCoord)->key().station();
    if (station > 0 ) {
      dist = dist + pow(((x - m_trackX[station])/dx),2) +
        pow(((y - m_trackY[station])/dy),2) ;
      nhits++;
    }
  }

  dist = dist / nhits; 
     
  double LklhMu = m_distMuon[0] * (TMath::Landau (dist,m_distMuon[1],m_distMuon[2])) + 
    m_distMuon[3] * (TMath::Landau (dist,m_distMuon[4],m_distMuon[5])) +
    m_distMuon[6] * (TMath::Landau (dist,m_distMuon[7],m_distMuon[8]));
  double LklhPi = m_distPion[0] * (TMath::Landau (dist,m_distPion[1],m_distPion[2])) + 
    m_distPion[3] * (TMath::Landau (dist,m_distPion[4],m_distPion[5])) +
    m_distPion[6] * (TMath::Landau (dist,m_distPion[7],m_distPion[8]));

  muonid->setMuonLLMu(log(LklhMu));
  muonid->setMuonLLBg(log(LklhPi));

  return StatusCode::SUCCESS;

}

//=============================================================================
// calculate the number of tracks that share hits
//=============================================================================
StatusCode MuonID::calcSharedHits( LHCb::MuonPID* muonid, LHCb::MuonPIDs * pMuids ) {

  // if not a muon candidate do nothing
  if ( !muonid->IsMuon() ) return StatusCode::SUCCESS;

  // calculate the distance from the hit to the extrapolated position
  StatusCode sc = calcDist(muonid);
  if( sc.isFailure() ) {
    warning() << " calcDist 1 failure" << endreq;
    muonid->setNShared(100);
    return sc;
  }

  double dist1 = m_dist;
  debug()  << " mdist 1=  " << m_dist << endreq;

  // loop over the muonIDs
  LHCb::MuonPIDs::const_iterator iMuon;
  for( iMuon = pMuids->begin(); pMuids->end() != iMuon; ++iMuon ) {

    // skip if this muonID is the same as the input or if IsMuon is false
    if ( *iMuon == muonid || !(*iMuon)->IsMuon() ) continue;

    // see if there are shared hits between the muonIDs
    if ( compareHits( muonid, *iMuon ) ){

      // get dist for this muonID
      StatusCode sc = calcDist(*iMuon);
      if( sc.isFailure() ) {
        warning() << " calcDist 2 failure" << endreq;
        continue;
      }

      double dist2 = m_dist;
      debug()  << " mdist 2=  " << m_dist << endreq;
      // the muonID which gets the number of shared hits is the one
      // which has the biggest dist
      if( dist2 < dist1 ) {
        int nNSH = muonid->nShared();
        muonid->setNShared(nNSH+1);
      }
      else{
        int nNSH = (*iMuon)->nShared();
        (*iMuon)->setNShared(nNSH+1);
      }
    }
  }
  debug()  << "nShared=  " <<  muonid->nShared() << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Method to compare the Coords of the muonIDs
//============================================================================
bool MuonID::compareHits( LHCb::MuonPID* muonid1, LHCb::MuonPID* muonid2 ){

  bool theSame = false;

  std::vector<LHCb::MuonCoord*> & mcoord1 = m_muonMap[muonid1];
  std::vector<LHCb::MuonCoord*>::const_iterator iCoord1;
  std::vector<LHCb::MuonCoord*> & mcoord2 = m_muonMap[muonid2];
  std::vector<LHCb::MuonCoord*>::const_iterator iCoord2;
  for( iCoord1 = mcoord1.begin() ; iCoord1 != mcoord1.end() ; iCoord1++ ){
    for( iCoord2 = mcoord2.begin() ; iCoord2 != mcoord2.end() ; iCoord2++ ){
      if( (*iCoord1)->key() == (*iCoord2)->key() ) {
        if ( ((*iCoord1)->key().station()) > 0 )  theSame = true;
      }
    }
  }

  return theSame;

}

//=============================================================================
//  Method to calculate the distance from the hit to the extrapolated position
//============================================================================
StatusCode MuonID::calcDist( LHCb::MuonPID* muonid ){

  m_dist = 0.;

  if ( !muonid->IsMuon() ) {
    return StatusCode::SUCCESS;
  }
  const LHCb::Track* pTrack = muonid->idTrack();
  // do the track extrapolations
  StatusCode sc = trackExtrapolate(pTrack);
  if(!sc){
    warning() << " trackExtrapolate fails for track " <<  pTrack << endreq;
    return StatusCode::FAILURE;
  }

  std::vector<LHCb::MuonCoord*> & mcoord = m_muonMap[muonid];
  std::vector<LHCb::MuonCoord*>::const_iterator iCoord;
  double mCoordX[5] = {0.,0.,0.,0.,0.};
  double mCoordY[5] = {0.,0.,0.,0.,0.};
  double mCoordDX[5] = {0.,0.,0.,0.,0.};
  double mCoordDY[5] = {0.,0.,0.,0.,0.};
  for( iCoord = mcoord.begin() ; iCoord != mcoord.end() ; iCoord++ ){
    double x,dx,y,dy,z,dz;
    LHCb::MuonTileID tile=(*iCoord)->key();
    StatusCode sc =
      m_mudet->Tile2XYZ(tile,x,dx,y,dy,z,dz);
    if(sc.isFailure()){
      warning()<< "Failed to get x,y,z of tile " << tile << endreq;
      continue;
    }
    int station = (*iCoord)->key().station();
    if(mCoordX[station] == 0) {
      // this is the first coord found
      mCoordX[station] = x;
      mCoordY[station] = y;
      // difference between x,y and the extrapolated x,y
      mCoordDX[station] = ( x - m_trackX[station] ) / dx;
      mCoordDY[station] = ( y - m_trackY[station] ) / dy;

    }else{
      // get best match in X and Y
      if( (x - m_trackX[station])*(x - m_trackX[station]) +
          (y - m_trackY[station])*(y - m_trackY[station])  <
          pow((mCoordX[station]*dx - m_trackX[station]),2) +
          pow((mCoordY[station]*dy - m_trackY[station]),2) ){
        // this Coord is a better match
        mCoordX[station] = x;
        mCoordY[station] = y;
        mCoordDX[station] = ( x - m_trackX[station] ) / dx;
        mCoordDY[station] = ( y - m_trackY[station] ) / dy;
      }
    }
  } // end of Coords loop

  // calculate the square of the distances
  int nstn = 0;
  for( int stn = 0 ; stn < 5 ; stn++ ){
    debug()  << " mCoordDX =  " << stn << mCoordDX[stn] << endreq;
    debug()  << " mCoordDY =  " << stn << mCoordDY[stn] << endreq;
    if ( mCoordDX[stn] != 0. ) {
      nstn++;
      m_dist += mCoordDX[stn]*mCoordDX[stn] + mCoordDY[stn]*mCoordDY[stn];
    }
  }
  m_dist = m_dist / nstn; //divide by the number of station

  return StatusCode::SUCCESS;
}

//=============================================================================
// apply the preselection
//=============================================================================
StatusCode MuonID::preSelection(LHCb::MuonPID * pMuid, bool &passed){
  passed = true;

  // compare momentum and position to acceptance
  // use the first state on track for the preSel
  if (m_MomentumPre < m_PreSelMomentum ){ 
    // failed preselection momentum cut
    pMuid->setPreSelMomentum(0);
    passed = false;
  }else{
    pMuid->setPreSelMomentum(1);
  }
  pMuid->setInAcceptance(1);
  // in first and last station acceptance
  debug()  << "trackX0 = " << m_trackX[0] << endreq;
  debug()  << "trackX4 = " << m_trackX[4] << endreq;
  debug()  << "trackY0 = " << m_trackY[0] << endreq;
  debug()  << "trackY4 = " << m_trackY[4] << endreq;
  if(  ! (fabs(m_trackX[0]) <  m_regionOuterX[0] &&
          fabs(m_trackY[0]) <  m_regionOuterY[0] )  ||
       ! (fabs(m_trackX[m_NStation-1]) <
          m_regionOuterX[m_NStation-1] &&
          fabs(m_trackY[m_NStation-1]) <
          m_regionOuterY[m_NStation-1] )
       ) {
    // outside M1 - M5 region
    pMuid->setInAcceptance(0);
    passed = false;
  }

  // Inner acceptance
  if(   (fabs(m_trackX[0]) <  m_regionInnerX[0] &&
         fabs(m_trackY[0]) <  m_regionInnerY[0] )  ||
        (fabs(m_trackX[m_NStation-1]) <
         m_regionInnerX[m_NStation-1] &&
         fabs(m_trackY[m_NStation-1]) <
         m_regionInnerY[m_NStation-1] ) ) {

    // inside M1 - M5 chamber hole
    pMuid->setInAcceptance(0);
    passed = false;
  }


  return StatusCode::SUCCESS;
}

//=============================================================================
// get the coords
//=============================================================================
StatusCode MuonID::setCoords(LHCb::MuonPID *pMuid){

  int station;
  for(station = 0 ; station < m_NStation ; station++){
    int region;
    for(region = 0 ; region < m_NRegion ; region++){

      if( !m_coordPos[station*m_NRegion + region].empty() ){

        std::vector<coordExtent_>::const_iterator itPos;
        for(itPos = m_coordPos[station*m_NRegion + region].begin();
            itPos != m_coordPos[station*m_NRegion + region].end();
            itPos++){

          double x = itPos->m_x;
          double dx = itPos->m_dx;
          double y = itPos->m_y;
          double dy = itPos->m_dy;


	  // not optimal this should be called only once per station, region	 
          double foiXDim = m_foifactor*foiX( station, region, m_MomentumPre, dx);
          double foiYDim = m_foifactor*foiY( station, region, m_MomentumPre, dy);      
	  
          // check if the hit is in the window
          if(  ( fabs( x - m_trackX[station] ) < foiXDim ) &&
               ( fabs( y - m_trackY[station] ) < foiYDim )  ) {

            debug()  << "FOIfactor : " << m_foifactor << endreq;

            debug()  << "ratioX = " << fabs( x - m_trackX[station])
                     << "ratioY = " << fabs( y - m_trackY[station]) << " foiXDim = "
                     << foiXDim <<" foiYDim = " << foiYDim <<endreq;
            debug()  << "padX = " << dx << " padY = " << dy << endreq;

            // it is in the window
            // add the hit to the MuonPID
            m_muonMap[pMuid].push_back(itPos->m_pCoord);
            m_occupancy[station]++;

            // also need track angle in X in M2-M3 for angle matching
            if(1 == m_occupancy[station]) {
              // this is the first coord found
              m_CoordX[station] = x;
            }else{
              // get best match X
              if( fabs(x - m_trackX[station]) <
                  fabs(m_CoordX[station] - m_trackX[station]) ){
                // this Coord is a better match
                m_CoordX[station] = x;
              }
            }
          }
        }// itPos
      }
    } //region
  } //station
  return StatusCode::SUCCESS;
}

//=============================================================================
// Track extrapolation
//=============================================================================
StatusCode MuonID::trackExtrapolate(const LHCb::Track *pTrack){

  resetTrackLocals();

  // get first state for the preSel cut
  const LHCb::State * state1 = &(pTrack->firstState());
  // get state closest to M1 for extrapolation
  //const LHCb::State * state = &(pTrack->closestState(m_stationZ[0]));
  //change to use z=9540, which is a state that is saved to the DSTs
  const LHCb::State * state = &(pTrack->closestState(9450.));

  if(!state1){
    err() << " Failed to get 1st state from track " << endreq;
    return StatusCode::FAILURE;
  }
  if(!state){
    err() << " Failed to get state from track " << endreq;
    return StatusCode::FAILURE;
  }

  // get the momentum (MeV/c)
  m_Momentum = state->p();
  m_MomentumPre = state1->p();

  // get the track slope
  m_trackSlopeX = state->tx();

  // Project the state into the muon stations
  int station;
  for(station = 0; station < m_NStation ; station++){
    // x(z') = x(z) + (dx/dz * (z' - z))
    m_trackX.push_back(state->x() + ( state->tx() *
                                      (m_stationZ[station] - state->z()) ));
    m_trackY.push_back(state->y() + ( state->ty() *
                                      (m_stationZ[station] - state->z()) ));
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
// return the FOI in x in a station and region for momentum (in MeV/c)
// this is a simpler version of the parameterization:
// foi = par0 + par2*exp(-par3*p)
//=============================================================================
double MuonID::foiX(const int &station, const int &region, const double &p,
                    const double &dx){
  return ( m_xfoiParam1[ station * m_NRegion + region ] +
           m_xfoiParam2[ station * m_NRegion + region ]*
           exp(-m_xfoiParam3[ station * m_NRegion + region ]*p/Gaudi::Units::GeV ) )*dx;

  //in the future optimize this checking that 2*dx =m_padSizeX[station * m_NRegion + region]
  //then eliminates dx from function
}

//=============================================================================
// return the FOI in y in a station and region for momentum (in MeV/c)
//=============================================================================
double MuonID::foiY(const int &station, const int &region, const double &p,
                    const double &dy){
  return ( m_yfoiParam1[ station * m_NRegion + region ] +
           m_yfoiParam2[ station * m_NRegion + region ]*
           exp(-m_yfoiParam3[ station * m_NRegion + region ]*p/Gaudi::Units::GeV ) )*dy;
}

//=============================================================================
// reset vectors
//=============================================================================
void MuonID::resetTrackLocals(){
  m_Momentum = -1.;
  m_MomentumPre = -1.;
  m_trackSlopeX = 0.;
  m_trackX.clear();
  m_trackY.clear();
  m_occupancy.clear();
  m_CoordX.clear();
  m_occupancy.resize(m_NStation,0);
  m_CoordX.resize(m_NStation,0.);
}

//=============================================================================
// make the muon segments
//=============================================================================
LHCb::Track* MuonID::makeMuonTrack(const LHCb::MuonPID& mupid){

  LHCb::Track* mtrack = new LHCb::Track( mupid.key() );

  const LHCb::Track* mother = mupid.idTrack();
  mtrack->addToStates( mother->closestState(9540.) );

  std::vector<LHCb::MuonCoord*> & mcoord = m_muonMap[(LHCb::MuonPID*) &mupid];
  std::vector<LHCb::MuonCoord*>::const_iterator iCoord;

  for(iCoord = mcoord.begin(); iCoord != mcoord.end(); ++iCoord ) {
    LHCb::MuonTileID tile= (*iCoord)->key();
    LHCb::LHCbID id(tile);
    mtrack->addToLhcbIDs(id);
    mtrack->setType(LHCb::Track::Muon);
    mtrack->setHistory(LHCb::Track::MuonID);
    LHCb::Measurement* muMeas= m_measProvider->measurement(id);
    mtrack->addToMeasurements( *muMeas );
    delete muMeas;
  }

 return mtrack; 

}
