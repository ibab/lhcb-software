#include "L0MuonKernel/L0BufferCtrlUnit.h"
#include "ProcessorKernel/RegisterFactory.h"


/**
   Constructor.
*/
L0Muon::L0BufferCtrlUnit::L0BufferCtrlUnit(){
};
/**
   Constructor.
*/
L0Muon::L0BufferCtrlUnit::L0BufferCtrlUnit(MuonTileID id):L0MUnit(id){
  char buf[4096];
  char* format ;
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
  // Create the TileRegister
  format = "L0BUFFER_CUQ%d";
  sprintf(buf,format,id.quarter()+1);
  Register* reg = rfactory->createRegister(buf,L0Muon::L0BufferCtrlSize);
  reg->setType("L0Buffer");
  addOutputRegister(reg);
};

/**
   Constructor.
*/
L0Muon::L0BufferCtrlUnit::L0BufferCtrlUnit(DOMNode* pNode):L0MUnit(pNode){
};


/**
   Destructor
*/
L0Muon::L0BufferCtrlUnit::~L0BufferCtrlUnit(){
}

void L0Muon::L0BufferCtrlUnit::initialize()
{

  char buf[4096];
  char* format ;

  // Candidate Register handlers for BCSU candidates
  for (int iboard = 0; iboard <12 ; iboard++) {
    format = "BOARD%d";
    sprintf(buf,format,iboard);
    std::map<std::string,L0Muon::Register*>::iterator itinputs =  m_inputs.find(buf);
    if (itinputs==m_inputs.end()) continue;
    Register* reg = (*itinputs).second;
    m_candRegHandlerBCSU[iboard] = CandRegisterHandler(reg) ;
  }

  // Candidate Register handler for selected candidates
  format = "CAND_CTRLQ%d";
  sprintf(buf,format,m_mid.quarter()+1);
  std::map<std::string,L0Muon::Register*>::iterator itinputs =  m_inputs.find(buf);
  if (itinputs==m_inputs.end()) std::cout <<"L0Muon::L0BufferCtrlUnit::initialize key "
                                            <<buf  <<" not found in input registers\n";
  Register* reg =(*itinputs).second;
  m_candRegHandlerCtrl = CandRegisterHandler(reg) ;

}


void L0Muon::L0BufferCtrlUnit::execute() {
}



