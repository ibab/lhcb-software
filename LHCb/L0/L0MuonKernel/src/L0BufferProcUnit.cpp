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
L0Muon::L0BufferProcUnit::L0BufferProcUnit(LHCb::MuonTileID id, int l0BufferMode):L0BufferUnit(id){
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
L0Muon::L0BufferProcUnit::L0BufferProcUnit(DOMNode* pNode):L0BufferUnit(pNode){
};


/**
   Destructor
*/
L0Muon::L0BufferProcUnit::~L0BufferProcUnit(){
}

void L0Muon::L0BufferProcUnit::initialize()
{
  L0BufferUnit::initialize();

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

  char buf[4096];
  char* format ;
  int shift;  

  // Get a pointer to the L0BufferProc output register
  format = "%s_Q%dR%d%d%d";
  sprintf(buf,format,type().c_str(),m_mid.quarter()+1,m_mid.region()+1,m_mid.nX(),m_mid.nY());
  std::map<std::string,L0Muon::Register*>::iterator itoutputs =  m_outputs.find(buf);
  if (itoutputs==m_outputs.end()) {
    std::cout <<"L0Muon::L0BufferProcUnit::execute key "<<buf  <<" not found in output registers\n";
    return;
  }
  //   if (m_debug) std::cout << "*!* L0BufferProcUnit::execute key= "<< buf <<" found in output registers\n";
  Register* outputreg =(*itoutputs).second;

  // Fill the input data field (29 x 16-bits words) 
  // ----------------------------------------------
  // Get a pointer to the formatted data input register
  format = "FORMATTED";
  sprintf(buf,format);
  std::map<std::string,L0Muon::Register*>::iterator itformatted =  m_inputs.find(buf);
  if (itformatted==m_inputs.end()) {
    std::cout <<"L0Muon::L0BufferProcUnit::execute key "<<buf  <<" not found in input registers\n";
    return;
  }
  TileRegister* formattedReg = dynamic_cast<TileRegister*>(itformatted->second);

  // If not already done, fill the formatted register
  if (formattedReg->isSet()) formattedReg->reset();
  
  // Loop over the raw input registers 
  for (unsigned int count=0; count<m_inputs.size(); count++) { 
    format = "RAW_%d";
    sprintf(buf,format,count);
    std::map<std::string,L0Muon::Register*>::iterator itraw =  m_inputs.find(buf);
    if (itraw==m_inputs.end()) break;
    TileRegister* rawReg = dynamic_cast<TileRegister*>(itraw->second);
    std::vector<LHCb::MuonTileID> firedTiles = rawReg->firedTiles();
    for (std::vector<LHCb::MuonTileID>::iterator ittile = firedTiles.begin();ittile<firedTiles.end();ittile++){
      formattedReg->setTile(*ittile);
    }
  } // End of Loop over the raw input registers
  
  // Set the output register
  outputreg->set(formattedReg->getBitset(),
                 BitsL0BufProcINPUTDATA ,ShiftL0BufProcINPUTDATA );

//   std::cout << "L0Muon::L0BufferProcUnit::execute L0BufProcINPUTDATA" 
//             <<", Bits "  <<  BitsL0BufProcINPUTDATA
//             <<", Shift " << ShiftL0BufProcINPUTDATA
//             <<"\n";

//   std::cout << "L0Muon::L0BufferProcUnit::execute " 
//             <<" formattedReg->size()=" << formattedReg->size() 
//             <<"\n";
  

  // Fill the link errors field (2 x 16-bits words) 
  // ----------------------------------------------
  outputreg->set(0,BitsL0BufProcOLERROR  ,ShiftL0BufProcOLERROR  );
//   std::cout << "L0Muon::L0BufferProcUnit::execute L0BufProcOLERROR" 
//             <<", Bits "  <<  BitsL0BufProcOLERROR
//             <<", Shift " << ShiftL0BufProcOLERROR
//             <<"\n";
  outputreg->set(0,BitsL0BufProcPARERROR ,ShiftL0BufProcPARERROR );
//   std::cout << "L0Muon::L0BufferProcUnit::execute L0BufProcPARERROR" 
//             <<", Bits "  <<  BitsL0BufProcPARERROR
//             <<", Shift " << ShiftL0BufProcPARERROR
//             <<"\n";
  outputreg->set(0,BitsL0BufProcSERERROR ,ShiftL0BufProcSERERROR );
//   std::cout << "L0Muon::L0BufferProcUnit::execute L0BufProcSERERROR" 
//             <<", Bits "  <<  BitsL0BufProcSERERROR
//             <<", Shift " << ShiftL0BufProcSERERROR
//             <<"\n";

  // Fill the output candidates field (3 x 16-bits words) 
  // ----------------------------------------------------

  // BCID ... TO BE FILLED
  outputreg->set(0,BitsL0BufProcBCID,ShiftL0BufProcBCID);
//   std::cout << "L0Muon::L0BufferProcUnit::execute L0BufProcBCID" 
//             <<", Bits "  <<  BitsL0BufProcBCID
//             <<", Shift " << ShiftL0BufProcBCID
//             <<"\n";

  // STATUS ... TO BE FILLED
  outputreg->set(0,BitsL0BufProcSTATUS,ShiftL0BufProcSTATUS);
//   std::cout << "L0Muon::L0BufferProcUnit::execute L0BufProcSTATUS" 
//             <<", Bits "  <<  BitsL0BufProcSTATUS
//             <<", Shift " << ShiftL0BufProcSTATUS
//             <<"\n";
  
  // Build and fill the candidate address
  // Loop over the candidates in the candidate register
  shift=ShiftL0BufProcCAND1;
  for (int icand = 0;icand<2;icand++) {
    unsigned long cand=0;
    int addM3 = m_candRegHandlerIn.getCandAddM3(icand) ;
    int offM2 = m_candRegHandlerIn.getCandOffM2(icand) ;
    int offM1 = m_candRegHandlerIn.getCandOffM1(icand) ;
    cand +=  (offM1  << RelShiftL0BufProcOFFM1)  & RelMaskL0BufProcOFFM1;   
    cand +=  (offM2  << RelShiftL0BufProcOFFM2)  & RelMaskL0BufProcOFFM2;                             
    cand +=  (addM3  << RelShiftL0BufProcADDM3)  & RelMaskL0BufProcADDM3;                             
    outputreg->set(cand,BitsL0BufProcCAND,shift);
    shift+=RelShiftL0BufProcCAND; 
  }// End of Loop over the candidates in the candidate register
  

  // Fill the identification field (2 x 16-bits words) 
  // -------------------------------------------------
  outputreg->set(m_evtCounter , BitsL0BufProcL0BID     , ShiftL0BufProcL0BID     );
//   std::cout << "L0Muon::L0BufferProcUnit::execute L0BufProcL0BID" 
//             <<", Bits "  <<  BitsL0BufProcL0BID
//             <<", Shift " << ShiftL0BufProcL0BID
//             <<"\n";

  outputreg->set(m_evtCounter , BitsL0BufProcL0EVTNUM  , ShiftL0BufProcL0EVTNUM  );
//   std::cout << "L0Muon::L0BufferProcUnit::execute L0BufProcL0EVTNUM" 
//             <<", Bits "  << BitsL0BufProcL0EVTNUM 
//             <<", Shift " << ShiftL0BufProcL0EVTNUM
//             <<"\n";

  outputreg->set(1            , BitsL0BufProcTEST     , ShiftL0BufProcTEST     );
//   std::cout << "L0Muon::L0BufferProcUnit::execute L0BufProcTEST" 
//             <<", Bits "  <<  BitsL0BufProcTEST
//             <<", Shift " << ShiftL0BufProcTEST
//             <<"\n";

  L0BufferUnit::execute();  
}


void L0Muon::L0BufferProcUnit::postexecute() {
  L0BufferUnit::postexecute();
  // Reset input registers (set bits to 0) 
  releaseInputRegisters();
}
