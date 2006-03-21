// $Id: MuonDAQTest.cpp,v 1.2 2006-03-21 08:35:01 asatta Exp $
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
    debug()<< "["  <<  digitTile.layout() << ","
          <<  digitTile.station() << ","
          <<  digitTile.region() << ","
          <<  digitTile.quarter() << ","
          <<  digitTile.nX() << ","
          <<  digitTile.nY() << "]" <<"time "<<(*idigit)->TimeStamp()<<
      endmsg;
  }
    std::vector<LHCb::MuonTileID> decodingTile=m_MuonBuffer->getTile();
    std::vector<LHCb::MuonTileID>::iterator jitile;
    for(jitile=decodingTile.begin();jitile<decodingTile.end();jitile++){
      LHCb::MuonTileID digitTile=(*jitile);
      verbose()<<
        " ;ist of tile "<<
        (unsigned int) digitTile<<endmsg;
      debug()<< "["  <<  digitTile.layout() << ","
            <<  digitTile.station() << ","
            <<  digitTile.region() << ","
            <<  digitTile.quarter() << ","
            <<  digitTile.nX() << ","
            <<  digitTile.nY() << "]" <<endmsg;      
    }  
  
    std::vector<std::pair<LHCb::MuonTileID,unsigned int> >::iterator ji;
    
    std::vector<std::pair<LHCb::MuonTileID,unsigned int> > 
      decoding=m_MuonBuffer->getTileAndTDC();
    for(ji=decoding.begin();ji<decoding.end();ji++){
      std::pair<LHCb::MuonTileID,unsigned int> digit=(*ji);
      LHCb::MuonTileID digitTile=digit.first;
      unsigned int time = digit.second;
      //info()<<" alesia "<<(unsigned int) digitTile<<endmsg;
      debug()<< "["  <<  digitTile.layout() << ","
            <<  digitTile.station() << ","
            <<  digitTile.region() << ","
            <<  digitTile.quarter() << ","
            <<  digitTile.nX() << ","
            <<  digitTile.nY() << "]" <<" time "<<time<<endmsg;      
    }  
    for(idigit=digit->begin();idigit<digit->end();idigit++){    
      LHCb::MuonTileID digitTile=(*idigit)->key();
      bool found=false;     
      for(ji=decoding.begin();ji<decoding.end()&&!found;ji++){
        std::pair<LHCb::MuonTileID,unsigned int> digitPair=(*ji);
        LHCb::MuonTileID digitTileDecoded=digitPair.first;
        unsigned int time = digitPair.second;
        if(digitTile==digitTileDecoded){
          if(time!=(*idigit)->TimeStamp()){
	    err()<<"time not correct "<<
            time<<" "<<(*idigit)->TimeStamp()<<endreq;
          }else{ found=true;}  
      }
      
      }
      if(!found)info()<<" not found the digit "<<endreq;
      
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
