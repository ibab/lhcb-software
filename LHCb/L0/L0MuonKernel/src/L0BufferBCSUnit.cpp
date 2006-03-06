#include "L0MuonKernel/L0BufferBCSUnit.h"

/**
   Constructor.
*/
L0Muon::L0BufferBCSUnit::L0BufferBCSUnit(){
};
/**
   Constructor.
*/
L0Muon::L0BufferBCSUnit::L0BufferBCSUnit(LHCb::MuonTileID id, int l0BufferMode):L0BufferUnit(id){
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
L0Muon::L0BufferBCSUnit::L0BufferBCSUnit(DOMNode* pNode):L0BufferUnit(pNode){
};


/**
   Destructor
*/
L0Muon::L0BufferBCSUnit::~L0BufferBCSUnit(){
}

void L0Muon::L0BufferBCSUnit::initialize()
{
  L0BufferUnit::initialize();

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
                                          <<  buf  <<" not found in input registers\n";
  Register* reg =(*itinputs).second;
  m_candRegHandlerBCSU = CandRegisterHandler(reg) ;

}


void L0Muon::L0BufferBCSUnit::execute() {

  char buf[4096];
  char* format ;
  std::map<int, CandRegisterHandler>::iterator itHandlerMap;
  int shift;  

  // Get a pointer to the L0BufferProc output register
  format = "%s_Q%dR%d%d%d";
  sprintf(buf,format,type().c_str(),m_mid.quarter()+1,m_mid.region()+1,m_mid.nX(),m_mid.nY());
  std::map<std::string,L0Muon::Register*>::iterator itoutputs =  m_outputs.find(buf);
  if (itoutputs==m_outputs.end()) {
    std::cout <<"L0Muon::CtrlUnit::initialize key "<<buf  <<" not found in output registers\n";
    return;
  }
  Register* outputreg =(*itoutputs).second;
  //   std::cout << " L0Muon::L0BufferBCSUnit::execute output register is"<<(*itoutputs).first<<"\n";

  // Fill the input candidates field (4 x 4 x 16-bits words) 
  // ----------------------------------------------------

  // Candidates from PUs
  shift=ShiftL0BufBcsuINPUT;
  // Loop over controler candidate registers
  for (itHandlerMap=m_candRegHandlerProc.begin();itHandlerMap!=m_candRegHandlerProc.end();itHandlerMap++){
    int pu = (*itHandlerMap).first;
    // Loop over the candidates in the candidate register to build the addresses for both candidates
    unsigned long add[2];
    for (int icand = 0;icand<2;icand++) {
      unsigned long cand=0;
      cand += ( m_candRegHandlerProc[pu].getCandAddM3(icand) << RelShiftL0BufBcsuADDM3) & RelMaskL0BufBcsuADDM3; 
      cand += ( m_candRegHandlerProc[pu].getCandOffM2(icand) << RelShiftL0BufBcsuOFFM2) & RelMaskL0BufBcsuOFFM2;
      cand += ( m_candRegHandlerProc[pu].getCandOffM1(icand) << RelShiftL0BufBcsuOFFM1) & RelMaskL0BufBcsuOFFM1; 
      add[icand] = cand;
    }// End of Loop over the candidates in the candidate register

    // Set the input field for current PU:
    outputreg->set(0                                        , BitsL0BufBcsuBCID  , shift + RelShiftL0BufBcsuBCID   );
    outputreg->set(0                                        , BitsL0BufBcsuSTATUS, shift + RelShiftL0BufBcsuSTATUS );
    outputreg->set(add[1]                                   , BitsL0BufBcsuADD   , shift + RelShiftL0BufBcsuADD2   );
    outputreg->set(add[0]                                   , BitsL0BufBcsuADD   , shift + RelShiftL0BufBcsuADD1   );
    outputreg->set(m_candRegHandlerProc[pu].getCandPT(1)    , BitsL0BufBcsuPT    , shift + RelShiftL0BufBcsuPT2    );
    outputreg->set(m_candRegHandlerProc[pu].getCandCharge(1), BitsL0BufBcsuCHARGE, shift + RelShiftL0BufBcsuCHARGE2);
    outputreg->set(m_candRegHandlerProc[pu].getCandPT(0)    , BitsL0BufBcsuPT    , shift + RelShiftL0BufBcsuPT1    );
    outputreg->set(m_candRegHandlerProc[pu].getCandCharge(0), BitsL0BufBcsuCHARGE, shift + RelShiftL0BufBcsuCHARGE1);

    shift+=BitsL0BufBcsuCANDPAIR;    
  }
  
  //   std::cout << " L0Muon::L0BufferBCSUnit::execute input candidates field filled\n";

  // Fill the output candidates field (4 x 16-bits words) 
  // ----------------------------------------------------

  // Loop over the candidates in the candidate register to build the addresses for both candidates
  unsigned long add[2];
  for (int icand = 0;icand<2;icand++) {
    unsigned long cand=0;
    cand += ( m_candRegHandlerBCSU.getCandAddM3(icand) << RelShiftL0BufBcsuADDM3) & RelMaskL0BufBcsuADDM3; 
    cand += ( m_candRegHandlerBCSU.getCandOffM2(icand) << RelShiftL0BufBcsuOFFM2) & RelMaskL0BufBcsuOFFM2;
    cand += ( m_candRegHandlerBCSU.getCandOffM1(icand) << RelShiftL0BufBcsuOFFM1) & RelMaskL0BufBcsuOFFM1; 
    add[icand] = cand;
  }// End of Loop over the candidates in the candidate register
  
  shift=ShiftL0BufBcsuOUTPUT;
  // Set PT, addresses, BCID, Status and PU for selected candidates
  outputreg->set(0                                    , BitsL0BufBcsuBCID  , shift + RelShiftL0BufBcsuBCID   );// TO BE FILLED
  outputreg->set(0                                    , BitsL0BufBcsuSTATUS, shift + RelShiftL0BufBcsuSTATUS );// TO BE FILLED
  outputreg->set(m_candRegHandlerBCSU.getCandPU(1)    , BitsL0BufBcsuPU    , shift + RelShiftL0BufBcsuPU2    );
  outputreg->set(m_candRegHandlerBCSU.getCandPU(0)    , BitsL0BufBcsuPU    , shift + RelShiftL0BufBcsuPU1    );
  outputreg->set(add[1]                               , BitsL0BufBcsuADD   , shift + RelShiftL0BufBcsuADD2   );
  outputreg->set(add[0]                               , BitsL0BufBcsuADD   , shift + RelShiftL0BufBcsuADD1   );
  outputreg->set(m_candRegHandlerBCSU.getCandPT(1)    , BitsL0BufBcsuPT    , shift + RelShiftL0BufBcsuPT2    );
  outputreg->set(m_candRegHandlerBCSU.getCandCharge(1), BitsL0BufBcsuCHARGE, shift + RelShiftL0BufBcsuCHARGE2);
  outputreg->set(m_candRegHandlerBCSU.getCandPT(0)    , BitsL0BufBcsuPT    , shift + RelShiftL0BufBcsuPT1    );
  outputreg->set(m_candRegHandlerBCSU.getCandCharge(0), BitsL0BufBcsuCHARGE, shift + RelShiftL0BufBcsuCHARGE1);
   
  //   std::cout << " L0Muon::L0BufferBCSUnit::execute output candidates field filled\n";

  // Fill the identification field (2 x 16-bits words) 
  // -------------------------------------------------
  outputreg->set(m_evtCounter, BitsL0BufBcsuL0BID   , ShiftL0BufBcsuL0BID    );
  outputreg->set(m_evtCounter, BitsL0BufBcsuL0EVTNUM, ShiftL0BufBcsuL0EVTNUM );
  outputreg->set(1           , BitsL0BufBcsuSIM     , ShiftL0BufBcsuSIM );

  //   std::cout << " L0Muon::L0BufferBCSUnit::execute identification field filled\n";

  L0BufferUnit::execute();  
}

