// $Id: MuonIDFOI.cpp,v 1.3 2002-07-05 08:58:13 dhcroft Exp $
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

  // ID routine properties
  declareProperty( "MomentumCuts", m_MomentumCuts );

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
    toolSvc()->retrieveTool("MuonTileIDXYZ", m_iTileTool, this );
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to create MuonTileIDToXYZ tool" << endreq;
    return sc;
  }

  sc = toolSvc()->retrieveTool("MuonGeometryTool", m_iGeomTool, this );
  if( sc.isFailure() ) {
    log << MSG::FATAL << "    Unable to create MuonGeometry tool" << endreq;
    return sc;
  }

  // fill local arrays of pad sizes and region sizes
  int station,region;
  for(station = 0 ; station < 5 ; station++ ){
    for(region = 0 ; region < 4 ; region++ ){
      m_iGeomTool->getPadSize(station,region,
                              m_padSizeX[station][region],
                              m_padSizeY[station][region]);

      double dz; // do not bother to store
      m_iGeomTool->getRegionBox(station,region,
                                m_regionOuterX[station][region],
                                m_regionInnerX[station][region],
                                m_regionOuterY[station][region],
                                m_regionInnerY[station][region],
                                m_stationZ[station],dz);
    }
  }

  if( m_MomentumCuts.size() != 3 || 
      m_xfoiParam1.size() != 20 || m_xfoiParam2.size() != 20 ||
      m_xfoiParam3.size() != 20 || m_xfoiParam4.size() != 20 ||
      m_yfoiParam1.size() != 20 || m_yfoiParam2.size() != 20 ||
      m_yfoiParam3.size() != 20 || m_yfoiParam4.size() != 20 ){
    log << MSG::ERROR << "OPTIONS initialising MuonIDFOI are missing" 
        << endreq;
    return StatusCode::FAILURE;
  }

  log << MSG::DEBUG << " Momenyum bins are (MeV/c)" ;
  std::vector<double>::const_iterator iMom;
  for(iMom = m_MomentumCuts.begin() ; iMom != m_MomentumCuts.end() ; iMom++){
    log << " " << *iMom ;
  }
  log << endreq;


  log << MSG::DEBUG << " FOI at 10 GeV/c is ";
  for(station = 0; station < 5; station++){
    for(region = 0; region < 4; region++){
      log << "M" << station+1 << "R" << region+1;
      log << " x=" << this->foiX(station,region,10000.0);
      log << " y=" << this->foiY(station,region,10000.0);
      log << " " << std::endl;
    }
  }
  log << endreq;

  clearCoordVectors(); // should not do anything really

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

      resetTrackLocals();
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

  int station;
  for(station = 0 ; station < 5 ; station++){
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
      
      m_coordPos[station][region].push_back(coordExtent_(x,dx,y,dy,*iCoord));
    }
  }

  return StatusCode::SUCCESS; 
}

// Clear the coord vector
void MuonIDFOI::clearCoordVectors(){
  int station;
  for(station = 0 ; station < 5 ; station++){
    int region;
    for(region = 0 ; region < 4 ; region++){
      
      m_coordPos[station][region].clear();
    }
  }
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
  pMuid->setIsMuon(0);
  if ( m_Momentum < m_MomentumCuts[1] ) {
    // test M1, M2 and M3
    if ( m_occupancy[0] > 0 && m_occupancy[1] > 0 && m_occupancy[2] >0 ) {
      // Have a muon
      pMuid->setIsMuon(1);
    }
  } else if ( m_Momentum < m_MomentumCuts[2] ) {
    if (  m_occupancy[0] > 0 && m_occupancy[1] > 0 && m_occupancy[2] >0  &&
          m_occupancy[3] > 0) {
      // Have a muon
      pMuid->setIsMuon(1);
    }
  } else {
    if ( m_occupancy[0] > 0 && m_occupancy[1] > 0 && m_occupancy[2] >0  &&
         m_occupancy[3] > 0 && m_occupancy[4] > 0) {
      pMuid->setIsMuon(1);
    }
  }

  // if found a muon make a probability from the DxDz matching
  if(pMuid->IsMuon()){
    // find slope difference between track and Coords in M2-M3
    double coordSlopeX = ((m_CoordX[2] - m_CoordX[1])/
                          (m_stationZ[2] - m_stationZ[1]));
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
      << " coord in FOI ("
      << m_occupancy[0] << ","  << m_occupancy[1] << "," 
      << m_occupancy[2] << ","  << m_occupancy[3] << "," 
      << m_occupancy[4] << ")" << endreq;
 
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
  if(  ! (fabs(m_trackX[0]) <  m_regionOuterX[0][3] && 
          fabs(m_trackY[0]) <  m_regionOuterY[0][3] )  ||  //M1 acceptance
       ! (fabs(m_trackX[4]) <  m_regionOuterX[4][3] &&
          fabs(m_trackY[4]) <  m_regionOuterY[4][3] ) //M5 acceptance
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
  for(station = 0 ; station < 5 ; station++){
    int region;
    for(region = 0 ; region < 4 ; region++){

      if( 0 < m_coordPos[station][region].size()){
        double foiXDim = foiX( station, region, m_Momentum);
        double foiYDim = foiY( station, region, m_Momentum);      
        
        std::vector<coordExtent_>::const_iterator itPos;
        for(itPos = m_coordPos[station][region].begin();
            itPos != m_coordPos[station][region].end();
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

  // get state closest to M1
  //  TrStateP *stateP =
  //  dynamic_cast<TrStateP*>(pTrack->closestState(m_stationZ[0]));
  // above is "correct" version, need to put back when available
  const TrStateP *stateP;
  double minDZ = -1.;
  SmartRefVector<TrState>::const_iterator iState;
  for(iState = pTrack->states().begin() ; 
      iState != pTrack->states().end() ; 
      iState++){
    const TrState *state = *iState;
    if(!state){
      MsgStream log(msgSvc(), name());
      log << MSG::ERROR << " Failed to get state from track " << endreq;
      return StatusCode::FAILURE;
    }
    const TrStateP *currentStateP = dynamic_cast<const TrStateP*>(state);
    if(!currentStateP){
      MsgStream log(msgSvc(), name());
      log << MSG::ERROR << " Failed to get stateP from track " << endreq;
      return StatusCode::FAILURE;
    }
    double DZ = fabs(currentStateP->z() - m_stationZ[0]);
    if(0 > minDZ || DZ < minDZ){
      minDZ = DZ;
      stateP = currentStateP;
    }
  }

  // get the momentum (MeV/c)
  m_Momentum = stateP->p();

  // get the track slope
  m_trackSlopeX = stateP->tx();

  // Project the state into the muon stations
  int station;
  for(station = 0; station < 5 ; station++){
    // x(z') = x(z) + (dx/dz * (z' - z))
    m_trackX[station] = stateP->x() + ( stateP->tx() *
                                        (m_stationZ[station] - stateP->z()) );
    m_trackY[station] = stateP->y() + ( stateP->ty() *
                                        (m_stationZ[station] - stateP->z()) );
  }

  return StatusCode::SUCCESS;
}

// return the FOI in x in a station and region for momentum (in MeV/c)
double MuonIDFOI::foiX(const int &station, const int &region, const double &p){
  return ( m_xfoiParam1[ station * 4 + region ] +
           m_xfoiParam2[ station * 4 + region ]*p/1000. +
           m_xfoiParam3[ station * 4 + region ]*
      exp(-m_xfoiParam4[ station * 4 + region ]*p/1000. ) )
    *m_padSizeX[station][region];
}

// return the FOI in y in a station and region for momentum (in MeV/c)
double MuonIDFOI::foiY(const int &station, const int &region, const double &p){
  return ( m_yfoiParam1[ station * 4 + region ] +
           m_yfoiParam2[ station * 4 + region ]*p/1000. +
           m_yfoiParam3[ station * 4 + region ]*
      exp(-m_yfoiParam4[ station * 4 + region ]*p/1000. ) )
    *m_padSizeY[station][region];
}

void MuonIDFOI::resetTrackLocals(){
  m_Momentum = -1.;
  m_trackSlopeX = 0.;
  int station;
  for(station=0 ; station < 5 ; station++){
    m_trackX[station] = 0.;
    m_trackY[station] = 0.;
    m_occupancy[station] = 0;
    m_CoordX[station] = 0.;
  }
}
