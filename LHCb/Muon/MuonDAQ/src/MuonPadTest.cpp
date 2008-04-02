// $Id: MuonPadTest.cpp,v 1.3 2008-04-02 11:52:05 asatta Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// local
#include "Event/MuonCoord.h"
#include "MuonPadTest.h"
#include "MuonDAQ/IMuonRawBuffer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonPadTest
//
// 2006-03-19 : Alessia Satta
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( MuonPadTest );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonPadTest::MuonPadTest( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
MuonPadTest::~MuonPadTest() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MuonPadTest::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;
  m_MuonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer");
  if(!m_MuonBuffer)info()<<" error "<<endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonPadTest::execute() {

  debug() << "==> Execute" << endmsg;

  std::vector<LHCb::MuonTileID> decodingTile;
	m_MuonBuffer->getPads(decodingTile);
  std::vector<LHCb::MuonTileID>::iterator ipad;
  SmartDataPtr<LHCb::MuonCoords> coord(eventSvc(),
                                 LHCb::MuonCoordLocation::MuonCoords);
  LHCb::MuonCoords::iterator icoord;
  debug()<<" coord "<<coord->size()<<endreq;
  debug()<<" pad "<<decodingTile.size()<<endreq;
  for(icoord=coord->begin();icoord<coord->end();icoord++){
    LHCb::MuonTileID tileCoord=(*icoord)->key();
    if(tileCoord.station()>0){
      debug()<< "["  <<  tileCoord.layout() << ","
            <<  tileCoord.station() << ","
            <<  tileCoord.region() << ","
            <<  tileCoord.quarter() << ","
            <<  tileCoord.nX() << ","
            <<  tileCoord.nY() << "]"<<endreq;
      
      bool found=false;      
      for(ipad=decodingTile.begin();ipad<decodingTile.end();ipad++){
        if(*ipad==tileCoord){
          debug()<<" found the matching coord "<<
            "["  <<  ipad->layout() << ","
                <<  ipad->station() << ","
                <<  ipad->region() << ","
                <<  ipad->quarter() << ","
                <<  ipad->nX() << ","
                <<  ipad->nY() << "]"<<endreq;            
          found=true;
          break;          
        }        
      }   
      if(!found){
        if((tileCoord.station()==1||tileCoord.station()==2)
           &&(tileCoord.region()==0||tileCoord.region()==1)&&
           ((*icoord)->uncrossed()))continue;
        info()<< " not found the tile "<<tileCoord.station()<<" "<<
          tileCoord.region()<<" "<<(*icoord)->uncrossed()<<endreq;
      }
    }    
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonPadTest::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
