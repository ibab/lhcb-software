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
L0Muon::L0BufferSlaveUnit::L0BufferSlaveUnit(LHCb::MuonTileID id, int l0BufferMode):L0BufferUnit(id){
  if (l0BufferMode>0) {
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
L0Muon::L0BufferSlaveUnit::L0BufferSlaveUnit(DOMNode* pNode):L0BufferUnit(pNode){
};


/**
   Destructor
*/
L0Muon::L0BufferSlaveUnit::~L0BufferSlaveUnit(){
}


void L0Muon::L0BufferSlaveUnit::initialize()
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
  if (itinputs==m_inputs.end()) std::cout <<"L0Muon::L0BufferBCSUnit::initialize key "
                                            <<buf  <<" not found in input registers\n";
  Register* reg =(*itinputs).second;
  m_candRegHandlerCtrl = CandRegisterHandler(reg) ;

}


void L0Muon::L0BufferSlaveUnit::execute() {
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
  //   if (m_debug) std::cout << "*!* L0BufferProcUnit::execute key= "<< buf <<" found in output registers\n";
  Register* outputreg =(*itoutputs).second;

  // Fill the output candidates field (3 x 16-bits words) 
  // ----------------------------------------------------

  outputreg->set(0                                    , BitsL0BufSuBCID  , ShiftL0BufSuOUTPUT + RelShiftL0BufSuBCID );
  outputreg->set(0                                    , BitsL0BufSuSTATUS, ShiftL0BufSuOUTPUT + RelShiftL0BufSuSTATUS );
  outputreg->set(m_candRegHandlerCtrl.getCandCharge(1), BitsL0BufSuCHARGE, ShiftL0BufSuOUTPUT + RelShiftL0BufSuCHARGE2 );
  outputreg->set(m_candRegHandlerCtrl.getCandCharge(0), BitsL0BufSuCHARGE, ShiftL0BufSuOUTPUT + RelShiftL0BufSuCHARGE1 );
  outputreg->set(m_candRegHandlerCtrl.getCandOffM1(1) , BitsL0BufSuOFFM1 , ShiftL0BufSuOUTPUT + RelShiftL0BufSuOFFM1_2 );
  outputreg->set(m_candRegHandlerCtrl.getCandOffM2(1) , BitsL0BufSuOFFM2 , ShiftL0BufSuOUTPUT + RelShiftL0BufSuOFFM2_2 );
  outputreg->set(m_candRegHandlerCtrl.getCandPU(1)    , BitsL0BufSuPU    , ShiftL0BufSuOUTPUT + RelShiftL0BufSuPU2 );
  outputreg->set(m_candRegHandlerCtrl.getCandBoard(1) , BitsL0BufSuBOARD , ShiftL0BufSuOUTPUT + RelShiftL0BufSuBOARD2 );
  outputreg->set(m_candRegHandlerCtrl.getCandOffM1(0) , BitsL0BufSuOFFM1 , ShiftL0BufSuOUTPUT + RelShiftL0BufSuOFFM1_1 );
  outputreg->set(m_candRegHandlerCtrl.getCandOffM2(0) , BitsL0BufSuOFFM2 , ShiftL0BufSuOUTPUT + RelShiftL0BufSuOFFM2_1 );
  outputreg->set(m_candRegHandlerCtrl.getCandPU(0)    , BitsL0BufSuPU    , ShiftL0BufSuOUTPUT + RelShiftL0BufSuPU1 );
  outputreg->set(m_candRegHandlerCtrl.getCandBoard(0) , BitsL0BufSuBOARD , ShiftL0BufSuOUTPUT + RelShiftL0BufSuBOARD1 );

  // Fill the input candidates field (12 x 3 x 16-bits words) 
  // --------------------------------------------------------

  // Candidates from BCSUs
  // Loop over controler candidate registers
  for (itHandlerMap=m_candRegHandlerBCSU.begin();itHandlerMap!=m_candRegHandlerBCSU.end();itHandlerMap++){
    int bcsu = (*itHandlerMap).first;

    shift = ShiftL0BufSuINPUT + (11-bcsu)*BitsL0BufSuCANDPAIR;

    // Set the input field for current BCSU:
    outputreg->set(0                                          , BitsL0BufSuBCID  , shift + RelShiftL0BufSuBCID );
    outputreg->set(0                                          , BitsL0BufSuSTATUS, shift + RelShiftL0BufSuSTATUS );
    outputreg->set(m_candRegHandlerBCSU[bcsu].getCandCharge(1), BitsL0BufSuCHARGE, shift + RelShiftL0BufSuCHARGE2 );
    outputreg->set(m_candRegHandlerBCSU[bcsu].getCandCharge(0), BitsL0BufSuCHARGE, shift + RelShiftL0BufSuCHARGE1 );
    outputreg->set(m_candRegHandlerBCSU[bcsu].getCandOffM1(1) , BitsL0BufSuOFFM1 , shift + RelShiftL0BufSuOFFM1_2 );
    outputreg->set(m_candRegHandlerBCSU[bcsu].getCandOffM2(1) , BitsL0BufSuOFFM2 , shift + RelShiftL0BufSuOFFM2_2 );
    outputreg->set(m_candRegHandlerBCSU[bcsu].getCandPU(1)    , BitsL0BufSuPU    , shift + RelShiftL0BufSuPU2 );
    outputreg->set(m_candRegHandlerBCSU[bcsu].getCandOffM1(0) , BitsL0BufSuOFFM1 , shift + RelShiftL0BufSuOFFM1_1 );
    outputreg->set(m_candRegHandlerBCSU[bcsu].getCandOffM2(0) , BitsL0BufSuOFFM2 , shift + RelShiftL0BufSuOFFM2_1 );
    outputreg->set(m_candRegHandlerBCSU[bcsu].getCandPU(0)    , BitsL0BufSuPU    , shift + RelShiftL0BufSuPU1 );

    shift+=BitsL0BufSuCANDPAIR;
  }

  // Fill the identification field (2 x 16-bits words) 
  // -------------------------------------------------
  outputreg->set(m_evtCounter, BitsL0BufSuL0BID   , ShiftL0BufSuL0BID    );
  outputreg->set(m_evtCounter, BitsL0BufSuL0EVTNUM, ShiftL0BufSuL0EVTNUM );
  outputreg->set(1           , BitsL0BufSuSIM     , ShiftL0BufSuSIM );

  L0BufferUnit::execute();  
}

