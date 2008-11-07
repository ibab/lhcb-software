#include "L0MuonKernel/FormattingOutUnit.h"
#include "ProcessorKernel/RegisterFactory.h"


/**
   Constructor.
*/
L0Muon::FormattingOutUnit::FormattingOutUnit(){
};
/**
   Constructor.
*/
L0Muon::FormattingOutUnit::FormattingOutUnit(LHCb::MuonTileID id):L0MUnit(id){
};

/**
   Constructor.
*/
L0Muon::FormattingOutUnit::FormattingOutUnit(DOMNode* pNode):L0MUnit(pNode){
};


/**
   Destructor
*/
L0Muon::FormattingOutUnit::~FormattingOutUnit(){
}


void L0Muon::FormattingOutUnit::execute() {  

  // Get a pointer to the output register
  std::map<std::string,L0Muon::Register*>::iterator irout = m_outputs.begin() ;
  TileRegister* itrout = dynamic_cast<TileRegister*>(irout->second);
  if (m_debug) std::cout <<"L0Muon::FormattingOutUnit::execute input register is : "
                         <<irout->first
                         <<" size= "<<itrout->size()<<std::endl;
  
  // Loop over the input registers 
  std::map<std::string,Register*>::iterator ir;
  for ( ir = m_inputs.begin(); ir != m_inputs.end(); ir++ ) {
    TileRegister* itr = dynamic_cast<TileRegister*>(ir->second);
    if (m_debug) std::cout <<"L0Muon::FormattingOutUnit::execute    output register is : "<<ir->first<<std::endl;
   // Loop over the fired tiles
    std::vector<LHCb::MuonTileID> firedTiles = itr->firedTiles();
    for (std::vector<LHCb::MuonTileID>::iterator ittile = firedTiles.begin();ittile<firedTiles.end();ittile++){
      if (m_debug) std::cout <<"L0Muon::FormattingOutUnit::execute     adding tile: "<<ittile->toString()<<std::endl;
      itrout->setTile(*ittile);
    } // End of Loop over the fired tiles

  } // End of Loop over the input registers 

  L0MUnit::execute();  

  
}


void L0Muon::FormattingOutUnit::postexecute() {
  L0MUnit::postexecute();
  releaseOutputRegisters();
}

