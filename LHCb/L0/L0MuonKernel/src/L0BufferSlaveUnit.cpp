#include "L0MuonKernel/L0BufferSlaveUnit.h"
#include "ProcessorKernel/RegisterFactory.h"


/**
   Constructor.
*/
L0Muon::L0BufferSlaveUnit::L0BufferSlaveUnit(){
};
/**
   Constructor.
*/
L0Muon::L0BufferSlaveUnit::L0BufferSlaveUnit(LHCb::MuonTileID id):L0MUnit(id){
  char buf[4096];
  char* format ;
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
  // Create the TileRegister
  format = "L0BUFFER_SUQ%d";
  sprintf(buf,format,id.quarter()+1);
  Register* reg = rfactory->createRegister(buf,L0Muon::L0BufferSlaveSize);
  reg->setType("L0Buffer");
  addOutputRegister(reg);
};

/**
   Constructor.
*/
L0Muon::L0BufferSlaveUnit::L0BufferSlaveUnit(DOMNode* pNode):L0MUnit(pNode){
};


/**
   Destructor
*/
L0Muon::L0BufferSlaveUnit::~L0BufferSlaveUnit(){
}


void L0Muon::L0BufferSlaveUnit::initialize()
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
  if (itinputs==m_inputs.end()) std::cout <<"L0Muon::L0BufferBCSUnit::initialize key "
                                            <<buf  <<" not found in input registers\n";
  Register* reg =(*itinputs).second;
  m_candRegHandlerCtrl = CandRegisterHandler(reg) ;

}


void L0Muon::L0BufferSlaveUnit::execute() {
}


