// $Id: MuonIDFOI.cpp,v 1.14 2005-12-07 07:01:55 pkoppenb Exp $
// Include files
#include <cstdio>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"

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
// first version
// 02/05/2002 : David Hutchcroft
//
// new FOI parameterization, acceptance cut for inner chamber holes,
// back to original algorithm; code expects all five chambers, even though
// it only checks hits in M2-M5 by now.
// 09/04/2003 : jtmn, miriam
//
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MuonIDFOI>          s_factory ;
const        IAlgFactory& MuonIDFOIFactory = s_factory ;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonIDFOI::MuonIDFOI( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) {

  // Source of track to ID
  declareProperty("TrackLocation",
                  m_TrStoredTracksPath = TrStoredTrackLocation::Default);

  // Destination of MuonID
  declareProperty("IDLocation",
                  m_MuonIDsPath = MuonIDLocation::Default);

  // >>>> this is not being read correctly from the DaVinci.opts
  // >>>> it is hardwired  later  
  // Pre-selection momentum
  declareProperty( "PreSelMomentum", m_PreSelMomentum = 3000.0);

  // Different depths of stations considered in different momentum ranges
  declareProperty( "MomentumCuts", m_MomentumCuts );

  // function that defines the field of interest size
  // here momentum is scaled to GeV....
  // new formula: p(1) + p(2)*exp(-p(3)*momentum)

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

  info()  << " MuonID v3r11" << endreq;
  debug()  << "==> Initialise" << endreq;
  debug()  << "Input tracks in : " << m_TrStoredTracksPath << endreq;
  debug()  << "Output MuonID in : " << m_MuonIDsPath<< endreq;

  m_iTileTool = tool<IMuonTileXYZTool>("MuonTileIDXYZ");
  m_iGeomTool = tool<IMuonGeometryTool>("MuonGeometryTool");
 
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
  m_regionInnerX.resize(m_NStation * m_NRegion);
  m_regionOuterX.resize(m_NStation * m_NRegion);
  m_regionInnerY.resize(m_NStation * m_NRegion);
  m_regionOuterY.resize(m_NStation * m_NRegion);

  m_stationZ.resize(m_NStation);

  // fill local arrays of pad sizes and region sizes
  int station,region;
  for(station = 0 ; station < m_NStation ; station++ ){
    for(region = 0 ; region < m_NRegion ; region++ ){
      StatusCode sc = m_iGeomTool->getPadSize(station,region,
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

    }
  }
  
          debug()  << "Outer X M1 = " << m_regionOuterX[3] << endreq;       
	  debug()  << "Outer Y M1 = " << m_regionOuterY[3] << endreq;
          debug()  << "Outer X M5 = " << m_regionOuterX[19] << endreq;
          debug()  << "Outer Y M5 = " << m_regionOuterY[19] << endreq;
          debug()  << "Inner X M1 = " << m_regionInnerX[0] << endreq;
          debug()  << "Inner Y M1 = " << m_regionInnerY[0] << endreq;
          debug()  << "Inner X M5 = " << m_regionInnerX[16] << endreq;
          debug()  << "Inner Y M5 = " << m_regionInnerY[16] << endreq;
  
  if( m_MomentumCuts.empty() || 
      m_xfoiParam1.size() != (unsigned)m_NStation*m_NRegion || 
      m_xfoiParam2.size() != (unsigned)m_NStation*m_NRegion ||
      m_xfoiParam3.size() != (unsigned)m_NStation*m_NRegion || 
      m_xfoiParam4.size() != (unsigned)m_NStation*m_NRegion || 
      m_yfoiParam1.size() != (unsigned)m_NStation*m_NRegion || 
      m_yfoiParam2.size() != (unsigned)m_NStation*m_NRegion ||
      m_yfoiParam3.size() != (unsigned)m_NStation*m_NRegion ||
      m_yfoiParam4.size() != (unsigned)m_NStation*m_NRegion  
      ){
    err() << "OPTIONS initialising MuonIDFOI are missing"
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
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonIDFOI::execute() {

  debug()  << "==> Execute" << endreq;

  StatusCode sc = fillCoordVectors();
  if(sc.isFailure()){
    return sc;
  }

  SmartDataPtr<TrStoredTracks> trTracks(eventSvc(),m_TrStoredTracksPath);
  if(!trTracks){
    err() << " Failed to get TrStoredTrack container "
        <<  m_TrStoredTracksPath << endreq;
    return StatusCode::FAILURE;
  }
  debug()  << "Number of input tracks " << trTracks->size() << endreq;

  MuonIDs * pMuids = new MuonIDs;

  TrStoredTracks::const_iterator iTrack;
  for( iTrack = trTracks->begin() ; iTrack != trTracks->end() ; iTrack++){
    // in the clone killed output we want only 
    // unqiue && (matched || forward || downstream)
    if((*iTrack)->unique()  && 
       ((*iTrack)->forward() || (*iTrack)->match() 
                             || (*iTrack)->isDownstream())){

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
  debug()  << "Number of MuonID objects created " << pMuids->size()
      << endreq;

  // Register the MuonID container to the TES

  sc = eventSvc()->registerObject(m_MuonIDsPath,pMuids);
  if(sc.isFailure()){
    err() << "TES rejected the muonIDs into location "
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

  debug()  << "==> Finalize" << endreq;

  // Release the tools
  if( m_iTileTool ) toolSvc()->releaseTool( m_iTileTool );
  if( m_iGeomTool ) toolSvc()->releaseTool( m_iGeomTool );
  

  return StatusCode::SUCCESS;
}

//=============================================================================
// fill vectors of x,y,z positions for the MuonCoords
StatusCode MuonIDFOI::fillCoordVectors(){

  clearCoordVectors(); // sets up Coord vectors of vectors

  int station;
  for(station = 0 ; station < m_NStation ; station++){
    std::string stationPattern = MuonCoordLocation::MuonCoords;

    std::string stationNameWithoutM=m_stationNames[station].substr(1,2);

    std::string::size_type allsize=stationPattern.size();

    std::string::size_type findFirstSeparator=stationPattern.find("%");

    std::string::size_type findLastSeparator=stationPattern.rfind("/");
 
    std::string firstPartOfName=stationPattern.substr(0,findFirstSeparator);

    std::string lastPartOfName=stationPattern.substr(findLastSeparator,allsize);

    std::string TESPath = firstPartOfName+stationNameWithoutM+lastPartOfName; 


    // get the MuonCoords for each station in turn

    SmartDataPtr<MuonCoords> coords(eventSvc(),TESPath);
    if(!coords){
      err() << "Failed to read TES path "
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
        err() << "Failed to get x,y,z of tile " << (*iCoord)->key()
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

  // First do a preselection:
  // track is in acceptance? Track has minimum momentum?
  bool passed;
  StatusCode sc = preSelection(pMuid,passed);
  if(sc.isFailure()){
    return sc;
  }

  // OK: track failed preselection say so and return
  if(!passed){
    debug() << " Track failed preselection " << endreq;
    return StatusCode::SUCCESS;
  }

  // find the coordinates in the fields of interest
  sc = setCoords(pMuid);
  if(sc.isFailure()){
    return sc;
  }

  // apply ID: depends on the track momentum
  pMuid->setIsMuon(0);

  // find the momentum bin we are in
  // it can be 0, 1 or 2
  int momentumBin = 0;

  // here  m_PreSelMomentum is hardwired = 3000 MeV
  if (m_Momentum> 3000.0 && m_Momentum < m_MomentumCuts[0]) {momentumBin = 0;}
  if (m_Momentum >= m_MomentumCuts[0] && m_Momentum < m_MomentumCuts[1]) {momentumBin = 1;}
  if (m_Momentum >= m_MomentumCuts[1]) {momentumBin = 2;}

  // now: implement original algorithm:
  // bin 0 M2.and.M3
  // bin 1 M2.and.M3.and.(M4.or.M5)
  // bin 2 M2.and.M3.and.M4.and.M5
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

  // if found a muon make a probability from the DxDz matching
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

  debug()  << "IsMuon = " << pMuid->IsMuon() 
	<< " bin = "   << momentumBin <<" " << " p = " << m_Momentum << endreq; 
  debug()  << " coord in FOI ("; 
       for(station = 0; station < m_NStation ; station++ ){
  debug() << m_occupancy[station] << "," ;
  }
  debug() << ")" << endreq;
 
  return StatusCode::SUCCESS;
}

// apply the preselection
StatusCode MuonIDFOI::preSelection(MuonID * pMuid, bool &passed){
  passed = true;

  // compare momentum and position to acceptance
  // here  m_PreSelMomentum is hardwired = 3000 MeV
  if (m_Momentum < 3000.0 ){ 
    // failed preselection momentum cut
    pMuid->setPreSelMomentum(0);
    passed = false;
  }else{
    pMuid->setPreSelMomentum(1);
  }
  pMuid->setInAcceptance(1);
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
  }

  // >>>>>>  the inner values are coming with bad values - hardwire them by now
/*  if(   (fabs(m_trackX[0]) <  m_regionInnerX[0] && 
            fabs(m_trackY[0]) <  m_regionInnerY[0] )  ||  
          (fabs(m_trackX[m_NStation-1]) < 
            m_regionInnerX[(m_NStation-1)*m_NRegion] &&
            fabs(m_trackY[m_NStation-1]) <  
            m_regionInnerY[(m_NStation-1)*m_NRegion] ) 
         ) {
*/
  if(   (fabs(m_trackX[0]) <  240.0 && 
          fabs(m_trackY[0]) < 200.0 )  ||  
        (fabs(m_trackX[m_NStation-1]) < 376.0 &&
          fabs(m_trackY[m_NStation-1]) < 313.0 ) 
       ) { 
    // inside M1 - M5 chamber hole
    pMuid->setInAcceptance(0);
    passed = false;
  }


  return StatusCode::SUCCESS;
}

StatusCode MuonIDFOI::setCoords(MuonID *pMuid){

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
          double foiXDim = foiX( station, region, m_Momentum, dx);
          double foiYDim = foiY( station, region, m_Momentum, dy);      
	  
          // check if the hit is in the window
          if(  ( fabs( x - m_trackX[station] ) < foiXDim ) &&
               ( fabs( y - m_trackY[station] ) < foiYDim )  ) {

           debug()  << "ratioX = " << fabs( x - m_trackX[station]) 
                 << "ratioY = " << fabs( y - m_trackY[station]) << " foiXDim = " 
                 << foiXDim <<" foiYDim = " << foiYDim <<endreq;
           debug()  << "padX = " << dx << " padY = " << dy << endreq;

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
    err() << " Failed to get stateP from track " << endreq;
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
// this is a simpler version of the parameterization: foi = par0 + par1*exp(-par2*p)
double MuonIDFOI::foiX(const int &station, const int &region, const double &p,
                       const double &dx){
  return ( m_xfoiParam1[ station * m_NRegion + region ] +
           m_xfoiParam2[ station * m_NRegion + region ]*p/GeV +
           m_xfoiParam3[ station * m_NRegion + region ]*
      exp(-m_xfoiParam4[ station * m_NRegion + region ]*p/GeV ) )*dx;

  //in the future optimize this checking that 2*dx =m_padSizeX[station * m_NRegion + region]
  //then eliminates dx from function
}

// return the FOI in y in a station and region for momentum (in MeV/c)
double MuonIDFOI::foiY(const int &station, const int &region, const double &p, 
                       const double &dy){
  return ( m_yfoiParam1[ station * m_NRegion + region ] +
           m_yfoiParam2[ station * m_NRegion + region ]*p/GeV +
           m_yfoiParam3[ station * m_NRegion + region ]*
      exp(-m_yfoiParam4[ station * m_NRegion + region ]*p/GeV ) )*dy;
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
