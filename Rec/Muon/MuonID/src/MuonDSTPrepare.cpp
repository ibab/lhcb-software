// $Id: MuonDSTPrepare.cpp,v 1.3 2005-12-12 07:41:21 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartRef.h"
 
//From MuonEvent
#include "Event/MuonID.h"

// local
#include "MuonDSTPrepare.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonDSTPrepare
//
// 16/05/2002 : David Hutchcroft
// 28/09/2005 : Erica Polycarpo (change to new message service sintaxe)
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MuonDSTPrepare>          s_factory ;
const        IAlgFactory& MuonDSTPrepareFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonDSTPrepare::MuonDSTPrepare( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ) {

  // TES string(s) for the MuonID objects
  declareProperty("IDLocation",
                  m_MuonIDsPath = MuonIDLocation::Default);

}

//=============================================================================
// Destructor
//=============================================================================
MuonDSTPrepare::~MuonDSTPrepare() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuonDSTPrepare::initialize() {

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonDSTPrepare::execute() {
  MsgStream log(msgSvc(), name());

  // vector to store the keys of the MuonCoords referenced in the 
  // MuonID Objects, one per station
  std::vector<MuonTileID> usedTiles[5];
  
  // get the existing MuonIDs from the TES
  MuonIDs* muonIDs = get<MuonIDs>(m_MuonIDsPath);
  if(!muonIDs){
    err() << "MuonIDs were not in TES at "
        << m_MuonIDsPath << endreq;
    return StatusCode::FAILURE;
  }
  
  MuonIDs::const_iterator iID;
  for( iID = muonIDs->begin() ; iID != muonIDs->end() ; iID++ ){
    // vector of references to coords 
    const SmartRefVector<MuonCoord> &coords = (*iID)->Coords();
    
    SmartRefVector<MuonCoord>::const_iterator iCoord;
    for( iCoord = coords.begin() ; iCoord != coords.end() ; iCoord++ ){
      int station = (*iCoord)->key().station();
      // add each MuonTileID to the vector
      usedTiles[station].push_back((*iCoord)->key());
    }
  }
  
  int station;
  for(station = 0 ; station < 5 ; station++){
    // get the MuonCoords for each station in turn
    std::string stationPattern = MuonCoordLocation::MuonCoords;
    char stationPath[100];
    sprintf(stationPath,stationPattern.c_str(),station+1);
    std::string TESPath = stationPath;
    MuonCoords* pCoords = get<MuonCoords>(TESPath);
    if(!pCoords){
      err() << "Failed to read TES path "
          << TESPath << " looking for MuonCoords" << endreq;
    }
    int oldSize = pCoords->size();
    StatusCode sc = removeUnusedCoords(pCoords,usedTiles[station]);
    if(sc.isFailure()){
      return sc;
    }
    int newSize = pCoords->size();
    debug()  << "Removed " << oldSize-newSize
        << " MuonCoords not used in MuonIDs in station "
        << station+1 << endreq;
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonDSTPrepare::finalize() {

  return StatusCode::SUCCESS;
}

//=============================================================================

StatusCode 
MuonDSTPrepare::removeUnusedCoords(MuonCoords *pCoords,
                                   std::vector<MuonTileID> &tilesKeep){

  //Remove the duplicates from the tiles to retain list
  // First sort (so that unqiues are next to each other)
  std::sort(tilesKeep.begin(),tilesKeep.end());
  // this removes the non-unique elements of the vector
  // (See http://www.sgi.com/tech/stl/unique.html)
  tilesKeep.erase(std::unique(tilesKeep.begin(),tilesKeep.end()),
                  tilesKeep.end());

  // Make a list of existing MuonTileIDs in the container
  std::vector<MuonTileID> tilesOld;
  MuonCoords::const_iterator iCoord;
  for(iCoord = pCoords->begin(); iCoord != pCoords->end(); iCoord++){
    tilesOld.push_back((*iCoord)->key());
  }
  std::sort(tilesOld.begin(),tilesOld.end());
  

  // delete tiles in tilesKeep from tilesOld;
  // ASSUMPTIONS:
  //  - All tiles in tilesKeep are in pCoords
  //  - Keys in pCoords are once only
  // If these assumptions are invalidated then this will fail

  std::vector<MuonTileID>::const_iterator iKeep = tilesKeep.begin();
  std::vector<MuonTileID>::iterator iOld = tilesOld.begin();
  while(iOld != tilesOld.end()){
    // matched in tilesKeep and tilesOld, erase from tilesOld
    if( iKeep != tilesKeep.end() && *iKeep == *iOld ){
      iOld = tilesOld.erase(iOld);
      iKeep++; // next iKeep
    }else{
      iOld++; // this MuonTile was not used and is to be deleted
    }
  }

  // Now tilesOld is the vector of keys to pCoords to be deleted
  for(iOld = tilesOld.begin(); iOld != tilesOld.end(); iOld++){
    pCoords->erase(*iOld);
  }
  return StatusCode::SUCCESS;
}

