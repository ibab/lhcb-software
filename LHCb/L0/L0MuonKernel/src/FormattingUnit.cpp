#include "L0MuonKernel/FormattingUnit.h"

L0Muon::FormattingUnit::FormattingUnit(){
    
}



L0Muon::FormattingUnit::~FormattingUnit() {}


void L0Muon::FormattingUnit::makePads() {

  std::map<std::string,Register*>::iterator ir;
  
  for ( ir = m_inputs.begin(); ir != m_inputs.end(); ir++ ) {
    
    TileRegister* itr = dynamic_cast<TileRegister*>(ir->second);
    
    std::vector<MuonTileID> tmp = itr->firedTiles();
    if ( itr->type() == L0Muon::STRIP ){
      if ( ! tmp.empty() ) {
        
        int tmpsize = tmp.size();
        int i,j;
        for ( i = 0; i<(tmpsize-1); i++ ) {
          for ( j=i+1; j<tmpsize; j++) {
            MuonTileID ip = tmp[i];
            MuonTileID jp = tmp[j];
            MuonTileID ijp = ip.intercept(jp);
            if ( ijp.isValid() ) {
              m_pads.push_back(ijp);

              }
          }
        }
      }
    } else {

      if ( ! tmp.empty() ) {
        for (std::vector<MuonTileID>::iterator iv = tmp.begin();
             iv != tmp.end();iv++){
          m_pads.push_back(*iv);
        }
      }
    }
  }
  
}


void L0Muon::FormattingUnit::initialize(){

  makePads();

  std::map<std::string,Register*>::iterator out;
  std::vector<MuonTileID>::iterator init;
  std::vector<MuonTileID>::iterator outit;


  // Iterate on all the output registers and their tiles to see if
  // they have non-zero interception with one (or many) fired pads
  // in the m_pads vector filled in by the makePads() method

  if ( ! m_outputs.empty()){
    
  for ( out = m_outputs.begin(); out != m_outputs.end(); out++ ) {
    TileRegister* outtr = dynamic_cast<TileRegister*>(out->second);
    std::vector<MuonTileID> outPads = outtr->getTileVector();
    if ( ! outPads.empty()){
    for ( outit = outPads.begin(); outit != outPads.end(); outit++) {
      for ( init = m_pads.begin(); init != m_pads.end(); init++) {
        if(outit->intercept(*init).isValid()) {
          outtr->setTile(*outit);
        }
      }
    }
    }
   }
  
  }
}



void L0Muon::FormattingUnit::execute(){
  
}

void L0Muon::FormattingUnit::finalize(){

   m_pads.clear();
  releaseRegisters();
  
}


