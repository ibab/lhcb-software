// $Id: MuonDAQTest.cpp,v 1.1 2006-01-31 15:22:33 asatta Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "MuonDAQTest.h"
#include "MuonDAQ/IMuonRawBuffer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonDAQTest
//
// 2005-10-19 : Alessia Satta
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MuonDAQTest>          s_factory ;
const        IAlgFactory& MuonDAQTestFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonDAQTest::MuonDAQTest( const std::string& name,
                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
MuonDAQTest::~MuonDAQTest() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode MuonDAQTest::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  debug() << "==> Initialize" << endmsg;
  m_MuonBuffer=tool<IMuonRawBuffer>("MuonRawBuffer");
  if(!m_MuonBuffer)info()<<" error "<<endmsg;
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonDAQTest::execute() {

  debug() << "==> Execute" << endmsg;

  SmartDataPtr<LHCb::MuonDigits> digit(eventSvc(),
                                 LHCb::MuonDigitLocation::MuonDigit);
  LHCb::MuonDigits::iterator idigit;
  for(idigit=digit->begin();idigit<digit->end();idigit++){    
    LHCb::MuonTileID digitTile=(*idigit)->key();
    info()<< "["  <<  digitTile.layout() << ","
          <<  digitTile.station() << ","
          <<  digitTile.region() << ","
          <<  digitTile.quarter() << ","
          <<  digitTile.nX() << ","
          <<  digitTile.nY() << "]" <<"time "<<(*idigit)->TimeStamp()<<
      endmsg;
  }
  /*  std::vector<LHCb::MuonTileID> decoding=m_MuonBuffer->getTile();
std::vector<LHCb::MuonTileID>::iterator ji;
for(ji=decoding.begin();ji<decoding.end();ji++){
LHCb::MuonTileID digitTile=(*ji);
info()<<" alesia "<<(unsigned int) digitTile<<endmsg;
   info()<< "["  <<  digitTile.layout() << ","
          <<  digitTile.station() << ","
          <<  digitTile.region() << ","
          <<  digitTile.quarter() << ","
          <<  digitTile.nX() << ","
          <<  digitTile.nY() << "]" <<endmsg;


}  
  */
  std::vector<std::pair<LHCb::MuonTileID,unsigned int> >::iterator ji;
  
   std::vector<std::pair<LHCb::MuonTileID,unsigned int> > 
     decoding=m_MuonBuffer->getTileAndTDC();
for(ji=decoding.begin();ji<decoding.end();ji++){
std::pair<LHCb::MuonTileID,unsigned int> digit=(*ji);
 LHCb::MuonTileID digitTile=digit.first;
 unsigned int time = digit.second;
 //info()<<" alesia "<<(unsigned int) digitTile<<endmsg;
 info()<< "["  <<  digitTile.layout() << ","
       <<  digitTile.station() << ","
       <<  digitTile.region() << ","
       <<  digitTile.quarter() << ","
       <<  digitTile.nX() << ","
       <<  digitTile.nY() << "]" <<" time "<<time<<endmsg;
 

}  


  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode MuonDAQTest::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
