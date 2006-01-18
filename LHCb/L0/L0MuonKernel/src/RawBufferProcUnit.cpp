#include "L0MuonKernel/RawBufferProcUnit.h"

/**
   Constructor.
*/
L0Muon::RawBufferProcUnit::RawBufferProcUnit(){
};
/**
   Constructor.
*/
L0Muon::RawBufferProcUnit::RawBufferProcUnit(LHCb::MuonTileID id):L0MUnit(id){
  m_evtCounter=0;
};

/**
   Constructor.
*/
L0Muon::RawBufferProcUnit::RawBufferProcUnit(DOMNode* pNode):L0MUnit(pNode){
};


/**
   Destructor
*/
L0Muon::RawBufferProcUnit::~RawBufferProcUnit(){
}


/**
   Initialize
*/
void L0Muon::RawBufferProcUnit::initialize(){

  char buf[4096];
  char* format ;

  // Candidate Register handlers for PU candidates
  for (int ipu = 0; ipu <12 ; ipu++) {
    format = "PU%d";
    sprintf(buf,format,ipu);
    std::map<std::string,L0Muon::Register*>::iterator itinputs =  m_inputs.find(buf);
    if (itinputs==m_inputs.end()) continue;
    Register* reg = (*itinputs).second;
    m_candRegHandlerProc[ipu] = CandRegisterHandler(reg) ;
  }

  // Candidate Register handlers for BCSU candidates
  for (int iboard = 0; iboard <12 ; iboard++) {
    format = "BOARD%d";
    sprintf(buf,format,iboard);
    std::map<std::string,L0Muon::Register*>::iterator itinputs =  m_inputs.find(buf);
    if (itinputs==m_inputs.end()) continue;
    Register* reg = (*itinputs).second;
    m_candRegHandlerBCSU[iboard] = CandRegisterHandler(reg) ;
  }
}



/**
   Execute
*/
void L0Muon::RawBufferProcUnit::execute(){
  // //std::cout << "TO BE IMPLEMENTED\n";
}
/**
   PostExecute
*/
void L0Muon::RawBufferProcUnit::postexecute(){
  m_evtCounter++;
}
