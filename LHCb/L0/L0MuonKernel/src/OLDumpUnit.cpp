#include "L0MuonKernel/OLDumpUnit.h"
#include "ProcessorKernel/RegisterFactory.h"


/**
   Constructor.
*/
L0Muon::OLDumpUnit::OLDumpUnit(){
};
/**
   Constructor.
*/
L0Muon::OLDumpUnit::OLDumpUnit(LHCb::MuonTileID id):L0BufferUnit(id){
};

/**
   Constructor.
*/
L0Muon::OLDumpUnit::OLDumpUnit(DOMNode* pNode):L0BufferUnit(pNode){
};


/**
   Destructor
*/
L0Muon::OLDumpUnit::~OLDumpUnit(){
}

void L0Muon::OLDumpUnit::initialize()
{
  L0BufferUnit::initialize();  
}


void L0Muon::OLDumpUnit::execute() {

  char buf[4096];
  char* format ;

  // Get a pointer to the input register
  format = "OUTPUT";
  sprintf(buf,format);
  std::map<std::string,L0Muon::Register*>::iterator itoutput =  m_outputs.find(buf);
  if (itoutput==m_outputs.end()) {
    std::cout <<"L0Muon::OLDumpUnit::execute key "<<buf  <<" not found in output registers\n";
    return;
  }
  TileRegister* outputReg = dynamic_cast<TileRegister*>(itoutput->second);
  
  // Loop over the raw input registers 
  for (unsigned int count=0; count<m_inputs.size(); count++) { 
    format = "RAW_%d";
    sprintf(buf,format,count);
    std::map<std::string,L0Muon::Register*>::iterator itraw =  m_inputs.find(buf);
    if (itraw==m_inputs.end()) break;
    TileRegister* rawReg = dynamic_cast<TileRegister*>(itraw->second);
    std::vector<LHCb::MuonTileID> firedTiles = rawReg->firedTiles();
    for (std::vector<LHCb::MuonTileID>::iterator ittile = firedTiles.begin();ittile<firedTiles.end();ittile++){
      outputReg->setTile(*ittile);
    }
  } // End of Loop over the raw input registers

  //L0BufferUnit::execute();  
  outputReg->print_words(m_outputFile,2);
  fprintf(m_outputFile,"----\n");
}


void L0Muon::OLDumpUnit::postexecute() {
  L0BufferUnit::postexecute();
  // Reset input registers (set bits to 0) 
  releaseInputRegisters();
}

