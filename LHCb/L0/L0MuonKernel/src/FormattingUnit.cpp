#include "L0MuonKernel/FormattingUnit.h"

#include <vector>
#include "ProcessorKernel/TileRegister.h"

L0Muon::FormattingUnit::FormattingUnit(){
}

L0Muon::FormattingUnit::FormattingUnit(LHCb::MuonTileID id):L0MUnit(id){
}

L0Muon::FormattingUnit::FormattingUnit(DOMNode* pNode):L0MUnit(pNode){
}


L0Muon::FormattingUnit::~FormattingUnit() {}


void L0Muon::FormattingUnit::preexecute(){
  if (m_debug) std::cout << "*!* Formatting::preexecute IN"  << std::endl;
   
  std::vector<LHCb::MuonTileID> firedPads;

  std::map<std::string,Register*>::iterator ir;

  if (m_debug) std::cout << "*!* Formatting::preexecute: registers in input"  << std::endl;
  for ( ir = m_inputs.begin(); ir != m_inputs.end(); ir++ ) {
    TileRegister* itr = dynamic_cast<TileRegister*>(ir->second);
    if (m_debug) std::cout << "*!* Formatting::preexecute:    "
                           << " register is "<<ir->first  
                   //<< " add: "<< itr
                           << " size: "<<itr->size()  
                           << std::endl;
    //if (m_debug) itr->print_tiles(0);
    if (m_debug) itr->print_bits(0,0);
    
    
    std::vector<LHCb::MuonTileID> tmp = itr->firedTiles();

    if (m_debug) std::cout <<"*!* Formatting::preexecute:    "<<ir->first<<" "<<tmp.size()<<" fired tiles"<<std::endl;
    itr->makePads();
      
    std::vector<LHCb::MuonTileID> pads = itr->Pads();
    std::vector<LHCb::MuonTileID>::iterator  ipads ;
    for (ipads = pads.begin(); ipads != pads.end(); ipads++){
      firedPads.push_back(*ipads);
    }    
  }  
  if (m_debug) std::cout <<"*!* Formatting::preexecute: firedPads contains "<<firedPads.size()<<" tiles"<<std::endl;

  std::map<std::string,Register*>::iterator out;

  // Iterate on all the output registers and their tiles to see if
  // they have non-zero interception with one (or many) fired pads
  // in the firedPads vector filled in by the makePads() method
    
  if ( ! m_outputs.empty()){
   
    if (m_debug) std::cout << "*!* Formatting::preexecute: registers in ouput"  << std::endl;
    for ( out = m_outputs.begin(); out != m_outputs.end(); out++ ) {
      TileRegister* outtr = dynamic_cast<TileRegister*>(out->second);
      std::vector<LHCb::MuonTileID> outPads = outtr->getTileVector();
      
      if (m_debug) std::cout << "*!* Formatting::preexecute: ouput register: "<<out->first<< std::endl;
      
      std::vector<LHCb::MuonTileID>::iterator init;
      std::vector<LHCb::MuonTileID>::iterator outit;
       
      if ( ! outPads.empty()){
        for ( outit = outPads.begin(); outit != outPads.end(); outit++) {

          for ( init = firedPads.begin(); init != firedPads.end(); init++) {
              
            if (outit->intercept(*init).isValid()) {
           
              if (m_debug) std::cout << "*!* Formatting::preexecute: tiles IN: "<<init->toString() 
                                     <<" - OUT: "<<outit->toString() << std::endl;
              outtr->setTile(*outit);
            }
          }
        }
      }
      if (m_debug) std::cout <<"*!* Formatting::preexecute:   "<<out->first
			     <<" "<<outPads.size()<<" pads"
			     <<" "<<outtr->firedTiles().size()<<" fired tiles"
			     <<std::endl;      
    }    
  }  
}


