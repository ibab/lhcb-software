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
L0Muon::L0BufferCtrlUnit::L0BufferCtrlUnit(LHCb::MuonTileID id, int l0BufferMode):L0BufferUnit(id){
  if (l0BufferMode==1) {
    char buf[4096];
    char* format ;
    L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
    // Create the output TileRegister
    format = "%s_Q%dR%d%d%d";
    sprintf(buf,format,type().c_str(),id.quarter()+1,id.region()+1,id.nX(),id.nY());
    Register* reg = rfactory->createRegister(buf,bufferSize());
    reg->setType("L0Buffer");
    addOutputRegister(reg);
  }
  
};

/**
   Constructor.
*/
L0Muon::L0BufferCtrlUnit::L0BufferCtrlUnit(DOMNode* pNode):L0BufferUnit(pNode){
};


/**
   Destructor
*/
L0Muon::L0BufferCtrlUnit::~L0BufferCtrlUnit(){
}

void L0Muon::L0BufferCtrlUnit::initialize()
{
  L0BufferUnit::initialize();

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

  char buf[4096];
  char* format ;
  std::map<int, CandRegisterHandler>::iterator itHandlerMap;
  int shift;  

  // Get a pointer to the L0BufferCtrl output register
  format = "%s_Q%dR%d%d%d";
  sprintf(buf,format,type().c_str(),m_mid.quarter()+1,m_mid.region()+1,m_mid.nX(),m_mid.nY());
  std::map<std::string,L0Muon::Register*>::iterator itoutputs =  m_outputs.find(buf);
  if (itoutputs==m_outputs.end()) {
    std::cout <<"L0Muon::L0BufferCtrlUnit::initialize key "<<buf  <<" not found in output registers\n";
    return;
  }
  Register* outputreg =(*itoutputs).second;

  // Fill the output candidates field (3 x 16-bits words) 
  // ----------------------------------------------------

  outputreg->set(0                                    , BitsL0BufCuBCID , ShiftL0BufCuOUTPUT + RelShiftL0BufCuBCID);
  outputreg->set(m_candRegHandlerCtrl.getCandPT(1)    , BitsL0BufCuPT   , ShiftL0BufCuOUTPUT + RelShiftL0BufCuPT2 );
  outputreg->set(m_candRegHandlerCtrl.getCandAddM3(1) , BitsL0BufCuADD  , ShiftL0BufCuOUTPUT + RelShiftL0BufCuADD2);
  outputreg->set(m_candRegHandlerCtrl.getCandPT(0)    , BitsL0BufCuPT   , ShiftL0BufCuOUTPUT + RelShiftL0BufCuPT1 );
  outputreg->set(m_candRegHandlerCtrl.getCandAddM3(0) , BitsL0BufCuADD  , ShiftL0BufCuOUTPUT + RelShiftL0BufCuADD1);

  // Fill the input candidates field (12 x 3 x 16-bits words) 
  // --------------------------------------------------------

  // Candidates from BCSUs
  // Loop over controler candidate registers
  for (itHandlerMap=m_candRegHandlerBCSU.begin();itHandlerMap!=m_candRegHandlerBCSU.end();itHandlerMap++){
    int bcsu = (*itHandlerMap).first;
    
    shift=ShiftL0BufCuINPUT + (11-bcsu)*BitsL0BufCuCANDPAIR;

    // Set the input field for current BCSU:
    outputreg->set(0                                          , BitsL0BufCuBCID , shift + RelShiftL0BufCuBCID );
    outputreg->set(m_candRegHandlerBCSU[bcsu].getCandPT(1)    , BitsL0BufCuPT   , shift + RelShiftL0BufCuPT2  );
    outputreg->set(m_candRegHandlerBCSU[bcsu].getCandAddM3(1) , BitsL0BufCuADD  , shift + RelShiftL0BufCuADD2 );
    outputreg->set(m_candRegHandlerBCSU[bcsu].getCandPT(0)    , BitsL0BufCuPT   , shift + RelShiftL0BufCuPT1  );
    outputreg->set(m_candRegHandlerBCSU[bcsu].getCandAddM3(0) , BitsL0BufCuADD  , shift + RelShiftL0BufCuADD1 );

  }

  // Fill the identification field (2 x 16-bits words) 
  // -------------------------------------------------
  outputreg->set(m_evtCounter, BitsL0BufCuL0BID   , ShiftL0BufCuL0BID    );
  outputreg->set(m_evtCounter, BitsL0BufCuL0EVTNUM, ShiftL0BufCuL0EVTNUM );
  outputreg->set(1           , BitsL0BufCuSIM     , ShiftL0BufCuSIM );

  L0BufferUnit::execute();  

}


