#include "L0MuonKernel/RawBufferCtrlUnit.h"

/**
   Constructor.
*/
L0Muon::RawBufferCtrlUnit::RawBufferCtrlUnit(){
};
/**
   Constructor.
*/
L0Muon::RawBufferCtrlUnit::RawBufferCtrlUnit(LHCb::MuonTileID id):RawBufferUnit(id){
  m_evtCounter=0;
};

/**
   Constructor.
*/
L0Muon::RawBufferCtrlUnit::RawBufferCtrlUnit(DOMNode* pNode):RawBufferUnit(pNode){
};


/**
   Destructor
*/
L0Muon::RawBufferCtrlUnit::~RawBufferCtrlUnit(){
}

/**
   Initialize
*/
void L0Muon::RawBufferCtrlUnit::initialize(){

  RawBufferUnit::initialize();

  // Initialize event counter
  m_evtCounter=0;

  // Initialize candidate register handlers
  char buf[4096];
  char* format ;

  // Loop over quarter
  for(int quarter=0; quarter<4; quarter++){

    // Candidate Register handlers for the BCSU candidates
    for (int iboard=0; iboard<12; iboard++){
      format = "CAND_BOARDQ%d_%d";               
      sprintf(buf,format,quarter+1,iboard);
      std::map<std::string,L0Muon::Register*>::iterator itinputs =  m_inputs.find(buf);
      if (itinputs==m_inputs.end()) continue;
      Register* reg = (*itinputs).second;
      std::pair<int,int> index(quarter,iboard);
      m_candRegHandlerBCSU[index] = CandRegisterHandler(reg) ;
    }

    // Candidate Register handler for selected candidates
    char buf[4096];
    char* format ;
    format = "CAND_CTRLQ%d";
    sprintf(buf,format,quarter+1);
    std::map<std::string,L0Muon::Register*>::iterator itinputs =  m_inputs.find(buf);
    if (itinputs==m_inputs.end()) {
      std::cout <<"L0Muon::CtrlUnit::initialize key "<<buf  <<" not found in input registers\n";
      continue;
    }
    Register* reg =(*itinputs).second;
    m_candRegHandlerCtrl[quarter] = CandRegisterHandler(reg) ;

  }

}



/**
   Execute
*/
void L0Muon::RawBufferCtrlUnit::execute(){

  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute evt= "<< m_evtCounter << std::endl;

  char buf[4096];
  char* format ;
  int nCandidates;
  int shift;
  
  // Get a pointer to the Rawbuffer Ctrl output register
  format = "RAWBUFFER_CTRL";
  sprintf(buf,format);
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute buf= "<< buf << std::endl;
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute # of output registers= "<<m_outputs.size()  <<"\n";
  std::map<std::string,L0Muon::Register*>::iterator itoutputs =  m_outputs.find(buf);
  if (itoutputs==m_outputs.end()) {
    std::cout <<"L0Muon::RawBufferCtrlUnit::initialize key "<<buf  <<" not found in output registers\n";
    return;
  }
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute key= "<< buf <<" found in output registers\n";
  Register* outputreg =(*itoutputs).second;
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute START "<< std::endl;
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute outputreg= "<< outputreg->getBitset() << std::endl;

  // Set the Global ID
  // -----------------
  outputreg->set(m_evtCounter , BitsRawBufCtrlL0EVTNUM  , ShiftRawBufCtrlL0EVTNUM  );
  outputreg->set(m_evtCounter , BitsRawBufCtrlL0BID     , ShiftRawBufCtrlL0BID     );

  // Errors
  // ------
  outputreg->set(           0 , BitsRawBufCtrlIDCUERROR , ShiftRawBufCtrlIDCUERROR );
  outputreg->set(           0 , BitsRawBufCtrlIDSUERROR , ShiftRawBufCtrlIDSUERROR );
  
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute AFTER HEADER "<< std::endl;
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute outputreg= "<< outputreg->getBitset() << std::endl;


  // Output candidates from Control Units
  // ------------------------------------
  shift=ShiftRawBufCtrlFIRSTCAND;
  nCandidates=0;
  // Loop over controler candidate registers
  std::map<int, CandRegisterHandler>::iterator itHandlerCtrl;
  for (itHandlerCtrl=m_candRegHandlerCtrl.begin();itHandlerCtrl!=m_candRegHandlerCtrl.end();itHandlerCtrl++){
    // Loop over input candidates in register
    int quarter = (*itHandlerCtrl).first;
    for (int icand = 0;icand<2;icand++) {
      //       if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute quarter= "<< quarter <<" icand= "<<icand << std::endl;
      if (m_candRegHandlerCtrl[quarter].isEmpty(icand)) continue;
      unsigned long ucandidate = formattedCandidate(m_candRegHandlerCtrl[quarter],icand,quarter);
      //       if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute CTRL cand."
      //                              <<" quarter= "<< quarter 
      //                              <<" icand= "<<icand 
      //                              <<" ucandidate= "<<ucandidate
      //                              << std::endl;
      outputreg->set(ucandidate,BitsRawBufCAND,shift);
      //       if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute AFTER CTRL CANDIDATE # "<<nCandidates << std::endl;
      //       if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute add new candidate ->outputreg= "
      //                              << outputreg->getBitset() << std::endl;
      nCandidates++;
      shift+=RelShiftRawBufCAND;
      
    } // End of Loop over input candidates in register
  } // End of Loop over controler candidate registers
  outputreg->set( nCandidates , BitsRawBufCtrlNCTRLCAND , ShiftRawBufCtrlNCTRLCAND );
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute AFTER N CTRL CANDIDATE"<< std::endl;
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute outputreg= "<< outputreg->getBitset() << std::endl;
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute outputreg= "<< outputreg->getBitset() 
  //                          << " ("<<outputreg->size()<<")"<< std::endl;

  // Candidates from Best Candidates Selection Units
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute ... done " << std::endl;


  // Output candidates from Best Candidates Selection Units
  // ------------------------------------------------------
  int shiftBcsuCandidate = shift;
  shift+=BitsRawBufCtrlNBCSUCAND;
  nCandidates=0;
  // Loop over controler candidate registers
  std::map<std::pair<int,int>, CandRegisterHandler>::iterator itHandlerBCSU;
  for (itHandlerBCSU=m_candRegHandlerBCSU.begin();itHandlerBCSU!=m_candRegHandlerBCSU.end();itHandlerBCSU++){
    // Loop over input candidates in register
    std::pair<int,int> index = (*itHandlerBCSU).first;
    int quarter = index.first;
    for (int icand = 0;icand<2;icand++) {
      //       if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute quarter= "<< quarter <<" icand= "<<icand << std::endl;
      if (m_candRegHandlerBCSU[index].isEmpty(icand)) continue;
      unsigned long ucandidate = formattedCandidate(m_candRegHandlerBCSU[index],icand,quarter);
      //       if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute BCSU cand."
      //                              <<" quarter= "<< quarter 
      //                              <<" icand= "<<icand 
      //                              <<" ucandidate= "<<ucandidate
      //                              << std::endl;
      outputreg->set(ucandidate,BitsRawBufCAND,shift);
      //      if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute AFTER BCSU CANDIDATE # "<<nCandidates << std::endl;
      //      if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute add new candidate ->outputreg= "
      //                             << outputreg->getBitset() << std::endl;
      nCandidates++;
      shift+=RelShiftRawBufCAND;
      
    } // End of Loop over input candidates in register
  } // End of Loop over controler candidate registers
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute outputreg= "<< outputreg->getBitset() << std::endl;
  outputreg->set( nCandidates , BitsRawBufCtrlNBCSUCAND , shiftBcsuCandidate );
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute AFTER N BCSU CANDIDATE"<< std::endl;
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute outputreg= "<< outputreg->getBitset() << std::endl;
  
  RawBufferUnit::execute();
}

