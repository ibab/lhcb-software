
#include <cstdio>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

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
// second version
// new FOI parameterization, acceptance cut for inner chamber holes,
// back to original algorithm; code expects all five chambers, even though
// it only checks hits in M2-M5 by now.
// 09/04/2003 : jtmn, miriam
//
// this version
// 25/06/2004 : Massafferri (amassaff@)
//
//
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
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_NStation(0),
    m_NRegion(0),
    m_Momentum(0.),
    m_trackSlopeX(0.),
    m_trackSlopeY(0.),
    m_iTileTool(),
    m_iGeomTool()
{
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

  // function that defines the field of interest size
  // here momentum is scaled to GeV....
  // new formula: p(1) + p(2)*momentum + p(3)*exp(-p(4)*momentum)

  declareProperty( "XFOIParameter1", m_xfoiParam1 );
  declareProperty( "XFOIParameter2", m_xfoiParam2 );
  declareProperty( "XFOIParameter3", m_xfoiParam3 );
  declareProperty( "XFOIParameter4", m_xfoiParam4 );
  declareProperty( "YFOIParameter1", m_yfoiParam1 );
  declareProperty( "YFOIParameter2", m_yfoiParam2 );
  declareProperty( "YFOIParameter3", m_yfoiParam3 );
  declareProperty( "YFOIParameter4", m_yfoiParam4 );

  declareProperty( "MakeNoShared", m_MakeNoShared ); 
  
}

//=============================================================================
// Destructor
//=============================================================================
MuonIDFOI::~MuonIDFOI() {};
//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuonIDFOI::initialize() {
  
  StatusCode sc = GaudiAlgorithm::initialize();
  if (!sc) return sc; // in principle useless
  debug() << "==> Initialise" << endreq;
  debug() << "Input tracks in : " << m_TrStoredTracksPath << endreq;
  debug() << "Output MuonID in : " << m_MuonIDsPath<< endreq;
  // get geometry tool
  m_iTileTool = tool<IMuonTileXYZTool>("MuonTileIDXYZ");
  if( !m_iTileTool ) {
    fatal() << "    Unable to create MuonTileIDToXYZ tool" << endreq;
    return StatusCode::FAILURE;
  }
  m_iGeomTool = tool<IMuonGeometryTool>("MuonGeometryTool");
  if( !m_iGeomTool) {
    fatal() << "Unable to create MuonGeometry tool" << endreq;
    return StatusCode::FAILURE;
  }
  m_NStation = 0;
  m_NRegion = 0;
  MuonBasicGeometry basegeometry( detSvc(),msgSvc());
  m_NStation= basegeometry.getStations();
  m_NRegion = basegeometry.getRegions();

  int i = 0;
  while (i<m_NStation){  
    m_stationNames.push_back(basegeometry.getStationName(i));
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
  for (station = 0 ; station < m_NStation ; station++ ){
    for (region = 0 ; region < m_NRegion ; region++ ){

      sc = m_iGeomTool->getPadSize(station,region,
                                   m_padSizeX[station * m_NRegion + region],
                                   m_padSizeY[station * m_NRegion + region]);
      if (!sc){
        return sc;
      }

      double dz; // do not bother to store
      sc = m_iGeomTool->getRegionBox(station,region,
                                     m_regionOuterX[station*m_NRegion+region],
                                     m_regionInnerX[station*m_NRegion+region],
                                     m_regionOuterY[station*m_NRegion+region],
                                     m_regionInnerY[station*m_NRegion+region],
                                     m_stationZ[station],dz);
      if (!sc){
        return sc;
      }
    }
  }
   
  if (m_MomentumCuts.empty() || 
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
  
  if (m_MomentumCuts.size() != 2 ){
    err() 
          << "OPTIONS are wrong:"
          << " size of MomentumCuts vector is not correct" 
          << endreq;
    return StatusCode::FAILURE;
  }

  debug() << " PreSelMomentum = "<<  m_PreSelMomentum << endreq;
  std::vector<double>::const_iterator iMom;
  for (iMom = m_MomentumCuts.begin() ; iMom != m_MomentumCuts.end() ; iMom++){
    debug() << "Mom. cuts = " << *iMom << endreq ;
  }
  debug() << endreq;

  return StatusCode::SUCCESS;


}
//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonIDFOI::execute() {

  debug() << "==> Execute" << endreq;

  StatusCode sc = fillCoordVectors();
  if (sc.isFailure()){
    return sc;
  }

  TrStoredTracks* trTracks = get<TrStoredTracks>(m_TrStoredTracksPath);
  if (!trTracks){
    err() << " Failed to get TrStoredTrack container "
          <<  m_TrStoredTracksPath << endreq;
    return StatusCode::FAILURE;
  }

  int NTracks = trTracks->size();
  MuonIDs *pMuids = new MuonIDs;
  int MaxHitsStation = 1;       //initialization 
  bool NoSharedFlag = true;
  if (m_MakeNoShared==1){

    for (int station = 1 ; station < m_NStation ; station++){
      for (int region = 0 ; region < m_NRegion ; region++){
        int TempMaxHitsStation = m_coordPos[station*m_NRegion + region].size();
        if (MaxHitsStation < TempMaxHitsStation){
          MaxHitsStation = TempMaxHitsStation;
        }	    
      }
    }

    long dimension_matrix = NTracks*m_NStation*MaxHitsStation;
    if (dimension_matrix > 1000000){        
      warning() << "high dimension matrix: evt no NoShared procedure" << endreq;  
      NoSharedFlag = false;
    }            

  }	


  int AddressHit[NTracks][m_NStation][MaxHitsStation];
  int iiTrackMu = 0;
  double DistScatt[NTracks];

  TrStoredTracks::const_iterator iTrack;

  for ( iTrack = trTracks->begin() ; iTrack != trTracks->end() ; iTrack++){

    if ((m_MakeNoShared==1) && (NoSharedFlag)){
      for (int station = 0 ; station < m_NStation ; station++){
        for (int nFOImax = 0 ; nFOImax < MaxHitsStation ; nFOImax++){
          AddressHit[iiTrackMu][station][nFOImax] = 0; // initialization 
        }
      }
    }
    
    if ((*iTrack)->unique()  && 
        ((*iTrack)->forward() || (*iTrack)->match() 
         || (*iTrack)->isDownstream())){

      StatusCode sc = trackExtrapolate(*iTrack);  // track extrapolation
      if (sc.isFailure()){        
        return sc;
      }
     
      MuonID * pMuid = new MuonID;
      pMuid->setIDTrack(*iTrack);
      bool passed;
      sc = preSelection(pMuid,passed);  // preselection
      if (sc.isFailure()){
        return sc;
      }     
      if (passed){

        int HitsStation[m_NStation];    
        int nFOI[m_NStation];

        nFOI[0]=0;        // only from M2
        HitsStation[0]=0; // only from M2

        for (int station = 1 ; station < m_NStation ; station++){
          HitsStation[station] = 0;
          nFOI[station]        = 0;
      
          int region;
          for (region = 0 ; region < m_NRegion ; region++){

            if ( !m_coordPos[station*m_NRegion + region].empty() ){  
        
              std::vector<coordExtent_>::const_iterator itPos;
              for (itPos = m_coordPos[station*m_NRegion + region].begin();
                   itPos != m_coordPos[station*m_NRegion + region].end();
                   itPos++){

                HitsStation[station]++;   // count total hits in station
                                          // before FOI cut! 	
                double x  = itPos ->m_x;
                double dx = itPos ->m_dx;
                double y  = itPos ->m_y;
                double dy = itPos ->m_dy;

                // not optimal this should be called only once per station, region	 
                double foiXDim = foiX( station, region, m_Momentum, dx);
                double foiYDim = foiY( station, region, m_Momentum, dy);      
	  
                double tr_x = m_trackX[station];
                double tr_y = m_trackY[station];	
		
                // check if the hit is in the window
                if (( fabs( x - tr_x ) < foiXDim ) &&
                    ( fabs( y - tr_y ) < foiYDim )  ) {

                  pMuid->addToCoords(itPos->m_pCoord);
                  nFOI[station]++; //warning: the first value is 1

                  if ((m_MakeNoShared==1) && (NoSharedFlag)){  
                    AddressHit[iiTrackMu][station][nFOI[station]] = HitsStation[station]; 
                  }
                  /////closest hit
		  
                  if (1 == nFOI[station]) {
                    m_CoordX[station]  = x;
                    m_CoordDX[station] = x - tr_x;
                    m_CoordY[station]  = y;
                    m_CoordDY[station] = y - tr_y; 

                  } else {
                    if ( ((x-tr_x)*(x-tr_x)+(y-tr_y)*(y-tr_y))<
                         ((m_CoordX[station]-tr_x)*(m_CoordX[station]-tr_x)+
                          (m_CoordY[station]-tr_y)*(m_CoordY[station]-tr_y)) ){		   
                      m_CoordX[station]  = x;
                      m_CoordDX[station] = x - m_trackX[station];
                      m_CoordY[station]  = y;
                      m_CoordDY[station] = y - m_trackY[station]; 

                    } // closest hit  
                  }    // closest hit 

                  m_CoordDX[station] /= dx;
                  m_CoordDY[station] /= dy;	

                }      // FOI cut
              }        // itPos 
            }
          }            // region
        }               // station


        pMuid->setIsMuon(0);
        pMuid->setMuProb(0.0);
       
        int momentumBin = 0;

        if (m_Momentum> 3000.0 && m_Momentum < m_MomentumCuts[0]) {momentumBin = 0;}
        if (m_Momentum >= m_MomentumCuts[0] && m_Momentum < m_MomentumCuts[1]) {momentumBin = 1;}
        if (m_Momentum >= m_MomentumCuts[1]) {momentumBin = 2;}

        // bin 0 : M2.and.M3 //
        // bin 1 : M2.and.M3.and.(M4.or.M5) // 
        // bin 2 : M2.and.M3.and.M4.and.M5 //

        bool isMuon=false;
        double NStationHit = 0.0;

        if (momentumBin == 0) {
          if (nFOI[1]>0 && nFOI[2]>0) {
            isMuon = true; NStationHit = 2.0;
          }
        }

        if (momentumBin == 1) {
          if (nFOI[1]>0 && nFOI[2]>0 && (nFOI[3]>0 || nFOI[4]>0)) {
            isMuon = true; NStationHit = 3.0;
          }
        } 

        if (momentumBin == 2) {
          if (nFOI[1]>0 && nFOI[2]>0 && nFOI[3]>0 && nFOI[4]>0) {
            isMuon = true; NStationHit = 4.0;
          }
        } 

        pMuid->setIsMuon(isMuon);
        pMuid->setMuProb(0.0);  //initializing 

        /// calculating DistScatt

        DistScatt[iiTrackMu]=0.; // initialization
        if (pMuid->IsMuon()){ 

          int chamber = 1;
          int LastOne = 0;
          while (chamber<m_NStation){
            if (nFOI[chamber]>0){  
              LastOne = chamber;         // The last station hit 

              DistScatt[iiTrackMu] += m_CoordDX[chamber]*m_CoordDX[chamber];
              DistScatt[iiTrackMu] += m_CoordDY[chamber]*m_CoordDY[chamber];

            }       
           
            chamber++;

          }

          DistScatt[iiTrackMu] /= chamber;

        } else {

          DistScatt[iiTrackMu] = 1000.; // overflow

        }	

        iiTrackMu++;               //togheter with insert 
        pMuids->insert( pMuid );   
       
      }    // endif of pre-selection    
    }
  }         // end Tracks loop
  int NMuonCand = pMuids->size();

  debug() << "Number of MuonID objects created " << NMuonCand << endreq;

  /// shared hits procedure

  if ((m_MakeNoShared==1) && (NoSharedFlag)) {

    MuonIDs::const_iterator imuid1,imuid2;
 
    int ght1 = -1;
    int NSharedHit = 0;
    
    for ( imuid1 = pMuids->begin() ; imuid1 != pMuids->end() ; imuid1++){
      ght1++;

      int ght2 = -1;
      for ( imuid2 = pMuids->begin() ; imuid2 != pMuids->end(); imuid2++){
        ght2++;

        NSharedHit = 0;

        for (int rty=1 ; rty < m_NStation ; rty++){
          for (int jgy=0 ; jgy < MaxHitsStation ; jgy++){                            
            if ((AddressHit[ght1][rty][jgy] == AddressHit[ght2][rty][jgy]) &&
                (AddressHit[ght1][rty][jgy] > 0) &&  (imuid1 != imuid2) &&  (ght1 != ght2) ){
              NSharedHit++;   // number of stations sharing hits

            }	 
          }
        }    

        if (NSharedHit != 0){

          if (DistScatt[ght1]>DistScatt[ght2]){ //if DistScatt=disc
 
            (*imuid1)->setMuProb(double(NSharedHit));

          }
        }
      }
    }
  }

  // Register the MuonID container to the TES

  sc = put(pMuids,m_MuonIDsPath);
  if (sc.isFailure()){
    err() << "TES rejected the muonIDs into location "
          << m_MuonIDsPath << endreq;
    return sc;
  }

  clearCoordVectors();
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

    MuonCoords* coords = get<MuonCoords>(TESPath);
    if(!coords){
      err() << "Failed to read TES path "
            << TESPath << " looking for MuonCoords" << endreq;
    }

    // loop over the coords

    MuonCoords::const_iterator iCoord;
    for( iCoord = coords->begin() ; iCoord != coords->end() ; iCoord++ ){
      int region = (*iCoord)->key().region();
      double x,dx,y,dy,z,dz;

      StatusCode sc =  m_iTileTool->calcTilePos((*iCoord)->key(),x,dx,y,dy,z,dz);

      if(sc.isFailure()){
        err() << "Failed to get x,y,z of tile " << (*iCoord)->key()
              << endreq;
        return sc;
      }      
      m_coordPos[station*m_NRegion+region].push_back(coordExtent_(x,dx,y,dy,*iCoord));
    }
  }
  return StatusCode::SUCCESS;
}

// Clear the coord vector


void MuonIDFOI::clearCoordVectors(){

  m_coordPos.clear();
  m_coordPos.resize(m_NStation * m_NRegion);

}

// apply the preselection


StatusCode MuonIDFOI::preSelection(MuonID * pMuid, bool &passed){

  passed = true;

  // compare momentum and position to acceptance
  // here  m_PreSelMomentum is hardwired = 3000 MeV

  //if (m_Momentum < m_PreSelMomentum ){ 
  if (m_Momentum < 3000.0 ){ 

    pMuid->setPreSelMomentum(0);
    passed = false;

  } else {
    pMuid->setPreSelMomentum(1);
  }

  pMuid->setInAcceptance(1);

  // in first and last station acceptance

  double OuterX0 = m_regionOuterX[m_NRegion - 1];
  double OuterY0 = m_regionOuterY[m_NRegion - 1];
  double OuterX1 = m_regionOuterX[(m_NStation - 1)*m_NRegion + m_NRegion - 1];
  double OuterY1 = m_regionOuterY[(m_NStation - 1)*m_NRegion + m_NRegion - 1];

  if ( ! (fabs(m_trackX[0]) < OuterX0 && fabs(m_trackY[0]) < OuterY0 )  ||  
       ! (fabs(m_trackX[m_NStation-1]) < OuterX1 &&
          fabs(m_trackY[m_NStation-1]) < OuterY1 ) )
  {   // outside M1 - M5 region

    pMuid->setInAcceptance(0);
    passed = false;

  }

  double InnerX0 = m_regionInnerX[0];
  double InnerY0 = m_regionInnerY[0];
  double InnerX1 = m_regionInnerX[(m_NStation - 1)*m_NRegion];
  double InnerY1 = m_regionInnerY[(m_NStation - 1)*m_NRegion];

  // changing Inners
  InnerX0 = 240.0;
  InnerY0 = 200.0;
  InnerX1 = 376.0;
  InnerY1 = 313.0; 
	     
  if ( ( fabs(m_trackX[0]) < InnerX0 && fabs(m_trackY[0]) < InnerY0 ) ||  
       ( fabs(m_trackX[m_NStation - 1]) < InnerX1 &&
         fabs(m_trackY[m_NStation - 1]) < InnerY1 ) )
  {

    pMuid->setInAcceptance(0);
    passed = false;

  }
  return StatusCode::SUCCESS;
}

StatusCode MuonIDFOI::trackExtrapolate(TrStoredTrack *pTrack){


  resetTrackLocals();

  // get state closest to M1
  const TrStateP *stateP =
    dynamic_cast<const TrStateP*>(pTrack->closestState(m_stationZ[0]));
  
  if (!stateP){
    err() << " Failed to get stateP from track " << endreq;

    return StatusCode::FAILURE;

  }

  // get the momentum (MeV/c)
  m_Momentum = stateP->p();

  // get the track slope
  m_trackSlopeX = stateP->tx();

  // Project the state into the muon stations
  int station;
  for (station = 0; station < m_NStation ; station++){
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
           m_xfoiParam2[ station * m_NRegion + region ]*p/1000. +
           m_xfoiParam3[ station * m_NRegion + region ]*
           exp(-m_xfoiParam4[ station * m_NRegion + region ]*p/1000. ) )*dx;

  //in the future optimize this checking that 2*dx =m_padSizeX[station * m_NRegion + region]
  //then eliminates dx from function


}
// return the FOI in y in a station and region for momentum (in MeV/c)
double MuonIDFOI::foiY(const int &station, const int &region, const double &p, 
                       const double &dy){
  return ( m_yfoiParam1[ station * m_NRegion + region ] +
           m_yfoiParam2[ station * m_NRegion + region ]*p/1000. +
           m_yfoiParam3[ station * m_NRegion + region ]*
           exp(-m_yfoiParam4[ station * m_NRegion + region ]*p/1000. ) )*dy;
}

void MuonIDFOI::resetTrackLocals(){

  m_Momentum = -1.;
  m_trackSlopeX = 0.;
  m_trackSlopeY = 0.;
  m_trackX.clear();
  m_trackY.clear();
  m_CoordX.clear();  
  m_CoordY.clear();  
  m_CoordDX.clear();  
  m_CoordDY.clear(); 
  m_CoordX.resize(m_NStation,0.);
  m_CoordY.resize(m_NStation,0.);
  m_CoordDX.resize(m_NStation,0.);
  m_CoordDY.resize(m_NStation,0.);

}
