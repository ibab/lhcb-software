// $Id: MuonIDFOI.cpp,v 1.4 2002-08-05 13:00:24 dhcroft Exp $
// Include files
#include <cstdio>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartDataPtr.h"

// WHY do I have to include this by hand?
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/IToolSvc.h"

// From event packages
#include "Event/MuonID.h"
#include "Event/TrStoredTrack.h"

// From MuonTools
#include "MuonTools/IMuonTileXYZTool.h"
#include "MuonTools/IMuonGeometryTool.h"

// local
#include "MuonIDFOI.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonIDFOI
//
// 02/05/2002 : David Hutchcroft
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MuonIDFOI>          s_factory ;
const        IAlgFactory& MuonIDFOIFactory = s_factory ;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonIDFOI::MuonIDFOI( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator ) {

  // Source of track to ID
  declareProperty("TrackLocation",
                  m_TrStoredTracksPath = TrStoredTrackLocation::Default);

  // Destination of MuonID
  declareProperty("IDLocation",
                  m_MuonIDsPath = MuonIDLocation::Default);

  // Pre-selection momentum
  declareProperty( "PreSelMomentum", m_PreSelMomentum );

  // Different depths of stations considered in different momentum ranges
  declareProperty( "MomentumCuts", m_MomentumCuts );
  declareProperty( "MomentumDepth", m_MomentumDepth );

  // function that defines the field of interest size
  // formula is p(1) + p(2)*momentum + p(3)*exp(-p(4)*momentum)
  // here momentum is scaled to GeV....
  declareProperty( "XFOIParameter1", m_xfoiParam1 );
  declareProperty( "XFOIParameter2", m_xfoiParam2 );
  declareProperty( "XFOIParameter3", m_xfoiParam3 );
  declareProperty( "XFOIParameter4", m_xfoiParam4 );
  declareProperty( "YFOIParameter1", m_yfoiParam1 );
  declareProperty( "YFOIParameter2", m_yfoiParam2 );
  declareProperty( "YFOIParameter3", m_yfoiParam3 );
  declareProperty( "YFOIParameter4", m_yfoiParam4 );
}

//=============================================================================
// Destructor
//=============================================================================
MuonIDFOI::~MuonIDFOI() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuonIDFOI::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  log << MSG::DEBUG << "Input tracks in : " << m_TrStoredTracksPath << endreq;
  log << MSG::DEBUG << "Output MuonID in : " << m_MuonIDsPath<< endreq;

  // get geometry tool
  StatusCode sc =
    toolSvc()->retrieveTool("MuonTileIDXYZ", m_iTileTool);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to create MuonTileIDToXYZ tool" << endreq;
    return sc;
  }

  sc = toolSvc()->retrieveTool("MuonGeometryTool", m_iGeomTool);
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to create MuonGeometry tool" << endreq;
    return sc;
  }

  sc = m_iGeomTool->nStation(m_NStation);
  if(!sc){
    return sc;
  }
  sc = m_iGeomTool->nRegion(m_NRegion);
  if(!sc){
    return sc;
  }

  // set the size of the local vectors
  m_padSizeX.resize(m_NStation * m_NRegion);
  m_padSizeY.resize(m_NStation * m_NRegion);
  m_regionInnerX.resize(m_NStation * m_NRegion);
  m_regionOuterX.resize(m_NStation * m_NRegion);
  m_regionInnerY.resize(m_NStation * m_NRegion);
  m_regionOuterY.resize(m_NStation * m_NRegion);

  m_stationZ.resize(m_NStation);

  m_xMatchStation = -1;
  // fill local arrays of pad sizes and region sizes
  int station,region;
  for(station = 0 ; station < m_NStation ; station++ ){
    for(region = 0 ; region < m_NRegion ; region++ ){
      sc = m_iGeomTool->getPadSize(station,region,
                                   m_padSizeX[station * m_NRegion + region],
                                   m_padSizeY[station * m_NRegion + region]);
      if(!sc){
        return sc;
      }

      double dz; // do not bother to store
      sc = m_iGeomTool->getRegionBox(station,region,
                                     m_regionOuterX[station*m_NRegion+region],
                                     m_regionInnerX[station*m_NRegion+region],
                                     m_regionOuterY[station*m_NRegion+region],
                                     m_regionInnerY[station*m_NRegion+region],
                                     m_stationZ[station],dz);
      if(!sc){
        return sc;
      }
      // need first station after 10m 
      if(10000. < m_stationZ[station] && -1 == m_xMatchStation ){
        m_xMatchStation = station;
      }
    }
  }

  if( m_MomentumCuts.empty() || 
      m_xfoiParam1.size() != (unsigned)m_NStation*m_NRegion || 
      m_xfoiParam2.size() != (unsigned)m_NStation*m_NRegion ||
      m_xfoiParam3.size() != (unsigned)m_NStation*m_NRegion || 
      m_xfoiParam4.size() != (unsigned)m_NStation*m_NRegion ||
      m_yfoiParam1.size() != (unsigned)m_NStation*m_NRegion || 
      m_yfoiParam2.size() != (unsigned)m_NStation*m_NRegion ||
      m_yfoiParam3.size() != (unsigned)m_NStation*m_NRegion || 
      m_yfoiParam4.size() != (unsigned)m_NStation*m_NRegion ){
    log << MSG::ERROR << "OPTIONS initialising MuonIDFOI are missing"
        << " or wrong size for " << m_NStation << " stations and " 
        << m_NRegion << " regions"
        << endreq;
    return StatusCode::FAILURE;
  }
  
  if( m_MomentumCuts.size() != m_MomentumDepth.size() ){
    log << MSG::ERROR 
        << "OPTIONS are wrong:"
        << " size of MomentumCuts and MomentumDepth vector do not match" 
        << endreq;
    return StatusCode::FAILURE;
  }

  log << MSG::DEBUG << " Momentum bins are (MeV/c) " << m_PreSelMomentum;
  std::vector<double>::const_iterator iMom;
  for(iMom = m_MomentumCuts.begin() ; iMom != m_MomentumCuts.end() ; iMom++){
    log << " " << *iMom ;
  }
  log << endreq;
  
  log << MSG::DEBUG << " FOI at 10 GeV/c is ";
  for(station = 0; station < m_NStation; station++){
    for(region = 0; region < m_NStation; region++){
      log << "M" << station+1 << "R" << region+1;
      log << " x=" << this->foiX(station,region,10000.0);
      log << " y=" << this->foiY(station,region,10000.0);
      log << " " << std::endl;
    }
  }
  log << endreq;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonIDFOI::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  StatusCode sc = fillCoordVectors();
  if(sc.isFailure()){
    return sc;
  }

  SmartDataPtr<TrStoredTracks> trTracks(eventSvc(),m_TrStoredTracksPath);
  if(!trTracks){
    log << MSG::ERROR << " Failed to get TrStoredTrack container "
        <<  m_TrStoredTracksPath << endreq;
    return StatusCode::FAILURE;
  }
  log << MSG::DEBUG << "Number of input tracks " << trTracks->size() << endreq;

  MuonIDs * pMuids = new MuonIDs;

  TrStoredTracks::const_iterator iTrack;
  for( iTrack = trTracks->begin() ; iTrack != trTracks->end() ; iTrack++){
    // in the clone killed output we want only 
    // unqiue && (matched || forward || upstream)
    if((*iTrack)->unique()  && 
       ((*iTrack)->forward() || (*iTrack)->match() || (*iTrack)->upstream())){

      // do the track extrapolations
      StatusCode sc = trackExtrapolate(*iTrack);
      
      MuonID * pMuid = new MuonID;
      pMuid->setIDTrack(*iTrack);
      sc = doID(pMuid);
      if(sc.isFailure()){
        return sc;
      }
      pMuids->insert( pMuid );
    }
  }

  // Debug : muon identification event summary
  log << MSG::DEBUG << "Number of MuonID objects created " << pMuids->size()
      << endreq;

  // Register the MuonID container to the TES

  sc = eventSvc()->registerObject(m_MuonIDsPath,pMuids);
  if(sc.isFailure()){
    log << MSG::ERROR << "TES rejected the muonIDs into location "
        << m_MuonIDsPath << endreq;
    return sc;
  }

  clearCoordVectors();

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonIDFOI::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}

//=============================================================================
// fill vectors of x,y,z positions for the MuonCoords
StatusCode MuonIDFOI::fillCoordVectors(){

  clearCoordVectors(); // sets up Coord vectors of vectors

  int station;
  for(station = 0 ; station < m_NStation ; station++){
    // get the MuonCoords for each station in turn
    std::string stationPattern = MuonCoordLocation::MuonCoords;
    char stationPath[100];
    sprintf(stationPath,stationPattern.c_str(),station+1);
    std::string TESPath = stationPath;
    SmartDataPtr<MuonCoords> coords(eventSvc(),TESPath);
    if(!coords){
      MsgStream log(msgSvc(), name());
      log << MSG::ERROR << "Failed to read TES path "
          << TESPath << " looking for MuonCoords" << endreq;
    }

    // loop over the coords
    MuonCoords::const_iterator iCoord;
    for( iCoord = coords->begin() ; iCoord != coords->end() ; iCoord++ ){
      int region = (*iCoord)->key().region();
      double x,dx,y,dy,z,dz;
      StatusCode sc = 
        m_iTileTool->calcTilePos((*iCoord)->key(),x,dx,y,dy,z,dz);
      if(sc.isFailure()){
        MsgStream log(msgSvc(), name());
        log << MSG::ERROR << "Failed to get x,y,z of tile " << (*iCoord)->key()
            << endreq;
        return sc;
      }
      
      m_coordPos[station*m_NRegion+region].
        push_back(coordExtent_(x,dx,y,dy,*iCoord));
    }
  }

  return StatusCode::SUCCESS; 
}

// Clear the coord vector
void MuonIDFOI::clearCoordVectors(){
  m_coordPos.clear();
  m_coordPos.resize(m_NStation * m_NRegion);
}

// Do the identification
StatusCode MuonIDFOI::doID(MuonID *pMuid){
  MsgStream log(msgSvc(), name());

  // First do a preselection:
  // track is in acceptance? Track has minimum momentum?
  bool passed;
  StatusCode sc = preSelection(pMuid,passed);
  if(sc.isFailure()){
    return sc;
  }

  // OK: track failed preselection say so and return
  if(!passed){
    log <<MSG::DEBUG << " Track failed preselection " << endreq;
    return StatusCode::SUCCESS;
  }

  // find the coordinates in the fields of interest
  sc = setCoords(pMuid);
  if(sc.isFailure()){
    return sc;
  }

  // apply ID: depends on the track momentum
  // changed here to allow variable numbers of stations: only 4 or 5 for now
  pMuid->setIsMuon(0);

  // find the momentum bin we are in
  int momentumBin = 0;
  while( (unsigned) momentumBin < m_MomentumCuts.size() && 
         m_Momentum > m_MomentumCuts[momentumBin] ){
    momentumBin++;
  }
  int lastStation;
  if( (unsigned)momentumBin == m_MomentumCuts.size()){
    lastStation = m_NStation;
  }else{
    lastStation = m_MomentumDepth[momentumBin];
    if(lastStation > m_NStation){
      log << MSG::ERROR << "Error in the MomentumDepth property" 
          << endreq;
      return StatusCode::FAILURE;
    }
  }

  // check all stations up to lastStation have a hit
  bool isMuon=true;
  int station;
  for( station = 0 ; station < lastStation ; station++ ) {
    if ( m_occupancy[station] == 0 ){
      isMuon = false;
    }
  }
  pMuid->setIsMuon(isMuon);

  // if found a muon make a probability from the DxDz matching
  if(pMuid->IsMuon()){
    // find slope difference between track and Coords in M2-M3
    double coordSlopeX = ((m_CoordX[m_xMatchStation] - 
                           m_CoordX[m_xMatchStation+1])/
                          (m_stationZ[m_xMatchStation] - 
                           m_stationZ[m_xMatchStation+1]));
    double dSlopeX = fabs( m_trackSlopeX - coordSlopeX );

    // formular to make this a probability is 
    // dSlopeX < 0.005 = 1.0
    // 0.005 < dSlopeX < 0.085 = 1. - ( (dSlopeX-0.005) / 0.08 )
    // 0.085 < dSlopeX = 0.
    if( dSlopeX < 0.005 ) {
      pMuid->setMuProb(1.0);
    }else if( dSlopeX > 0.085 ){
      pMuid->setMuProb(0.0);
    } else {
      pMuid->setMuProb(1.0 - ( (dSlopeX-0.005) / 0.08 ) );
    }
  }else{
    // not passed selection as a muon so probability is zero
    pMuid->setMuProb(0.0);
  }

  log << MSG::DEBUG << "ID Prob = " << pMuid->MuProb() 
      << " p = " << m_Momentum << "  "
      << " coord in FOI (";
  for(station = 0; station < m_NStation ; station++ ){
    log << m_occupancy[station] << "," ;
  }
  log << ")" << endreq;
 
  return StatusCode::SUCCESS;
}

// apply the preselection
StatusCode MuonIDFOI::preSelection(MuonID * pMuid, bool &passed){
  passed = true;

  // compare momentum and position to acceptance
  if(m_Momentum < m_MomentumCuts[0]) {
    // failed preselection momentum cut
    pMuid->setPreSelMomentum(0);
    passed = false;
  }else{
    pMuid->setPreSelMomentum(1);
  }
  // in first and last station acceptance
  if(  ! (fabs(m_trackX[0]) <  m_regionOuterX[m_NRegion-1] && 
          fabs(m_trackY[0]) <  m_regionOuterY[m_NRegion-1] )  ||  
       ! (fabs(m_trackX[m_NStation-1]) < 
          m_regionOuterX[(m_NStation-1)*m_NRegion + m_NRegion-1] &&
          fabs(m_trackY[m_NStation-1]) <  
          m_regionOuterY[(m_NStation-1)*m_NRegion + m_NRegion-1] ) 
       ) {
    // outside M1 - M5 region
    pMuid->setInAcceptance(0);
    passed = false;
  }else{
    // Inside Muon acceptance
    pMuid->setInAcceptance(1);
  }

  return StatusCode::SUCCESS;
}

StatusCode MuonIDFOI::setCoords(MuonID *pMuid){

  int station;
  for(station = 0 ; station < m_NStation ; station++){
    int region;
    for(region = 0 ; region < m_NRegion ; region++){

      if( !m_coordPos[station*m_NRegion + region].empty() ){
        double foiXDim = foiX( station, region, m_Momentum);
        double foiYDim = foiY( station, region, m_Momentum);      
        
        std::vector<coordExtent_>::const_iterator itPos;
        for(itPos = m_coordPos[station*m_NRegion + region].begin();
            itPos != m_coordPos[station*m_NRegion + region].end();
            itPos++){

          double x = itPos->m_x;
          double dx = itPos->m_dx;
          double y = itPos->m_y;
          double dy = itPos->m_dy;

          // check if the hit is in the window
          if( ( (fabs( x - m_trackX[station] ) - dx) < foiXDim ) &&
              ( (fabs( y - m_trackY[station] ) - dy) < foiYDim ) ) {
            // it is in the window
            // add the hit to the MuonID
            pMuid->addToCoords(itPos->m_pCoord);
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

StatusCode MuonIDFOI::trackExtrapolate(TrStoredTrack *pTrack){

  resetTrackLocals();

  // get state closest to M1
  const TrStateP *stateP =
    dynamic_cast<const TrStateP*>(pTrack->closestState(m_stationZ[0]));
  if(!stateP){
    MsgStream log(msgSvc(), name());
    log << MSG::ERROR << " Failed to get stateP from track " << endreq;
    return StatusCode::FAILURE;
  }

  // get the momentum (MeV/c)
  m_Momentum = stateP->p();

  // get the track slope
  m_trackSlopeX = stateP->tx();

  // Project the state into the muon stations
  int station;
  for(station = 0; station < m_NStation ; station++){
    // x(z') = x(z) + (dx/dz * (z' - z))
    m_trackX.push_back(stateP->x() + ( stateP->tx() *
                                       (m_stationZ[station] - stateP->z()) ));
    m_trackY.push_back(stateP->y() + ( stateP->ty() *
                                       (m_stationZ[station] - stateP->z()) ));
  }

  return StatusCode::SUCCESS;
}

// return the FOI in x in a station and region for momentum (in MeV/c)
double MuonIDFOI::foiX(const int &station, const int &region, const double &p){
  return ( m_xfoiParam1[ station * m_NRegion + region ] +
           m_xfoiParam2[ station * m_NRegion + region ]*p/1000. +
           m_xfoiParam3[ station * m_NRegion + region ]*
      exp(-m_xfoiParam4[ station * m_NRegion + region ]*p/1000. ) )
    *m_padSizeX[station * m_NRegion + region];
}

// return the FOI in y in a station and region for momentum (in MeV/c)
double MuonIDFOI::foiY(const int &station, const int &region, const double &p){
  return ( m_yfoiParam1[ station * m_NRegion + region ] +
           m_yfoiParam2[ station * m_NRegion  + region ]*p/1000. +
           m_yfoiParam3[ station * m_NRegion + region ]*
      exp(-m_yfoiParam4[ station * m_NRegion + region ]*p/1000. ) )
    *m_padSizeY[station * m_NRegion + region];
}

void MuonIDFOI::resetTrackLocals(){
  m_Momentum = -1.;
  m_trackSlopeX = 0.;
  m_trackX.clear();
  m_trackY.clear();
  m_occupancy.clear();
  m_CoordX.clear();  
  m_occupancy.resize(m_NStation,0);
  m_CoordX.resize(m_NStation,0.);
}
