#include "L0MuonKernel/FormattingUnit.h"

L0Muon::FormattingUnit::FormattingUnit(){
  
  

  
}



L0Muon::FormattingUnit::~FormattingUnit() {}




void L0Muon::FormattingUnit::makePads() {


  m_pads.clear();

  std::map<std::string,Register*>::iterator ir;

  if (m_debug) std::cout << "Registers in input in FormattingUnit"  << std::endl;
  for ( ir = m_inputs.begin(); ir != m_inputs.end(); ir++ ) {
    TileRegister* itr = dynamic_cast<TileRegister*>(ir->second);
    if (m_debug) std::cout << " " << ir->first << " " <<std::endl;
    
    std::vector<MuonTileID> tmp = itr->firedTiles();

    itr->makePads();
      
    std::vector<MuonTileID> pads = itr->Pads();
    std::vector<MuonTileID>::iterator  ipads ;
    for (ipads = pads.begin(); ipads != pads.end(); ipads++){
       
      m_pads.push_back(*ipads);
    }    
  }  
}

void L0Muon::FormattingUnit::initialize(){
   
  makePads();

  std::map<std::string,Register*>::iterator out;



  // Iterate on all the output registers and their tiles to see if
  // they have non-zero interception with one (or many) fired pads
  // in the m_pads vector filled in by the makePads() method

     
    
  if ( ! m_outputs.empty()){
   
    for ( out = m_outputs.begin(); out != m_outputs.end(); out++ ) {
      TileRegister* outtr = dynamic_cast<TileRegister*>(out->second);
      std::vector<MuonTileID> outPads = outtr->getTileVector();
      //std::vector<MuonTileID>::iterator i;
    
      std::vector<MuonTileID>::iterator init;
      std::vector<MuonTileID>::iterator outit;
       
      if ( ! outPads.empty()){
        for ( outit = outPads.begin(); outit != outPads.end(); outit++) {
                   
          for ( init = m_pads.begin(); init != m_pads.end(); init++) {
              
            if (outit->intercept(*init).isValid()) {
           
              outtr->setTile(*outit);
            }
          }
        }
      }
    }    
  }  
}


void L0Muon::FormattingUnit::execute(){
 
  if (m_debug) std::cout << "Execute Formatting" << std::endl;
  
  
}

void L0Muon::FormattingUnit::finalize(){

  m_pads.clear();
  releaseRegisters();
  
}


