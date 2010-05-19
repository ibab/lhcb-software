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
  std::vector<LHCb::MuonTileID> firedTiles;
  
  std::map<std::string,Register*>::iterator ir;

  if (m_debug) std::cout << "*!* Formatting::preexecute: registers in input"  << std::endl;
  for ( ir = m_inputs.begin(); ir != m_inputs.end(); ir++ ) {
    TileRegister* itr = dynamic_cast<TileRegister*>(ir->second);

    if (m_debug) {
      std::cout << "*!* Formatting::preexecute:    "
                << " register is "<<ir->first  
                << " size: "<<itr->size()  
                << std::endl;

      itr->print_bits(0,0);
      std::vector<LHCb::MuonTileID> tmp = itr->firedTiles();
      std::cout <<"*!* Formatting::preexecute:    "<<ir->first<<" "<<tmp.size()<<" fired tiles"<<std::endl;
    }

    itr->makePads();

    std::vector<LHCb::MuonTileID> tiles;

    tiles = itr->Pads();
    firedPads.insert(firedPads.end(),tiles.begin(),tiles.end());

    tiles= itr->firedTiles();
    firedTiles.insert(firedTiles.end(),tiles.begin(),tiles.end());

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

          if (outit->station()==1) {
            // Set ouput tile if it CONTAINS a fired input TILE
            for ( init = firedTiles.begin(); init != firedTiles.end(); init++) {
              if (outit->intercept(*init)==(*init)) {
                if (m_debug) std::cout << "*!* Formatting::preexecute: is contained tiles IN: "<<init->toString()
                                       <<" - OUT: "<<outit->toString() << std::endl;
                
                outtr->setTile(*outit);
              }
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


