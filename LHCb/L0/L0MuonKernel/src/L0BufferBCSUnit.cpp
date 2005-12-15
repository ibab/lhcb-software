#include "L0MuonKernel/L0BufferBCSUnit.h"

/**
   Constructor.
*/
L0Muon::L0BufferBCSUnit::L0BufferBCSUnit(){
};
/**
   Constructor.
*/
L0Muon::L0BufferBCSUnit::L0BufferBCSUnit(MuonTileID id):L0MUnit(id){
  char buf[4096];
  char* format ;
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
  // Create the TileRegister
  format = "L0BUFFER_BCSUQ%dR%d%d%d";
  sprintf(buf,format,id.quarter()+1,id.region()+1,id.nX(),id.nY());
  Register* reg = rfactory->createRegister(buf,L0Muon::L0BufferBCSUSize);
  reg->setType("L0Buffer");
  addOutputRegister(reg);
};

/**
   Constructor.
*/
L0Muon::L0BufferBCSUnit::L0BufferBCSUnit(DOMNode* pNode):L0MUnit(pNode){
};


/**
   Destructor
*/
L0Muon::L0BufferBCSUnit::~L0BufferBCSUnit(){
}

void L0Muon::L0BufferBCSUnit::initialize()
{

  char buf[4096];
  char* format ;

  // Candidate Register handlers for input candidates
  for (int ipu = 0; ipu <4 ; ipu++) {
    format = "PU%d";
    sprintf(buf,format,ipu);
    std::map<std::string,L0Muon::Register*>::iterator itinputs =  m_inputs.find(buf);
    if (itinputs==m_inputs.end()) continue;
    Register* reg = (*itinputs).second;
    m_candRegHandlerProc[ipu] = CandRegisterHandler(reg) ;
  }

  // Candidate Register handler for selected candidates
  format = "CAND_BCSUQ%dR%d%d%d";
  sprintf(buf,format,m_mid.quarter()+1,m_mid.region()+1,m_mid.nX(),m_mid.nY());
  std::map<std::string,L0Muon::Register*>::iterator itinputs =  m_inputs.find(buf);
  if (itinputs==m_inputs.end()) std::cout <<"L0Muon::L0BufferBCSUnit::initialize key "
                                            <<buf  <<" not found in input registers\n";
  Register* reg =(*itinputs).second;
  m_candRegHandlerBCSU = CandRegisterHandler(reg) ;

}


void L0Muon::L0BufferBCSUnit::execute() {
}


