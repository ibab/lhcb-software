#include "L0MuonKernel/L0BufferProcUnit.h"
#include "ProcessorKernel/RegisterFactory.h"


/**
   Constructor.
*/
L0Muon::L0BufferProcUnit::L0BufferProcUnit(){
};
/**
   Constructor.
*/
L0Muon::L0BufferProcUnit::L0BufferProcUnit(LHCb::MuonTileID id):L0MUnit(id){
  char buf[4096];
  char* format ;
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
  // Create the TileRegister
  format = "L0BUFFER_PUQ%dR%d%d%d";
  sprintf(buf,format,id.quarter()+1,id.region()+1,id.nX(),id.nY());
  Register* reg = rfactory->createRegister(buf,L0Muon::L0BufferProcSize);
  reg->setType("L0Buffer");
  addOutputRegister(reg);
};

/**
   Constructor.
*/
L0Muon::L0BufferProcUnit::L0BufferProcUnit(DOMNode* pNode):L0MUnit(pNode){
};


/**
   Destructor
*/
L0Muon::L0BufferProcUnit::~L0BufferProcUnit(){
}

void L0Muon::L0BufferProcUnit::initialize()
{

  // Candidate Register handler for PU candidates
  char buf[4096];
  char* format ;
  format = "CAND_PUQ%dR%d%d%d";
  sprintf(buf,format,m_mid.quarter()+1,m_mid.region()+1,m_mid.nX(),m_mid.nY());
  std::map<std::string,L0Muon::Register*>::iterator itinputs =  m_inputs.find(buf);
  if (itinputs==m_inputs.end()) std::cout <<"L0Muon::L0BufferProcUnit::initialize key "<<buf  
                                            <<" not found in input registers\n";
  Register* reg =(*itinputs).second;
  m_candRegHandlerIn = CandRegisterHandler(reg) ;


}


void L0Muon::L0BufferProcUnit::execute() {
}
