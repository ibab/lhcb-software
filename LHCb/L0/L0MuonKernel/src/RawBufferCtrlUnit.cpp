#include "L0MuonKernel/RawBufferCtrlUnit.h"

/**
   Constructor.
*/
L0Muon::RawBufferCtrlUnit::RawBufferCtrlUnit(){
};
/**
   Constructor.
*/
L0Muon::RawBufferCtrlUnit::RawBufferCtrlUnit(MuonTileID id):L0MUnit(id){
  m_evtCounter=0;
};

/**
   Constructor.
*/
L0Muon::RawBufferCtrlUnit::RawBufferCtrlUnit(DOMNode* pNode):L0MUnit(pNode){
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

  // Initialize event counters
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
  std::map<int, CandRegisterHandler>::iterator itHandlerMap;
  int nCandidates;
  int shift;
  
  // Get a pointer to the Rawbuffer Ctrl output register
  format = "RAWBUFFER_CTRL";
  sprintf(buf,format);
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute buf= "<< buf << std::endl;
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute # of output registers= "<<m_outputs.size()  <<"\n";
  std::map<std::string,L0Muon::Register*>::iterator itoutputs =  m_outputs.find(buf);
  if (itoutputs==m_outputs.end()) {
    std::cout <<"L0Muon::CtrlUnit::initialize key "<<buf  <<" not found in output registers\n";
    return;
  }
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute key= "<< buf <<" found in output registers\n";
  Register* outputreg =(*itoutputs).second;
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute outputreg= "<< outputreg->getBitset() << std::endl;

  // Set the Global ID
  outputreg->set(m_evtCounter , BitsRawBufCtrlL0EVTNUM  , ShiftRawBufCtrlL0EVTNUM  );
  outputreg->set(m_evtCounter , BitsRawBufCtrlL0BID     , ShiftRawBufCtrlL0BID     );
  outputreg->set(           0 , BitsRawBufCtrlIDCUERROR , ShiftRawBufCtrlIDCUERROR );
  outputreg->set(           0 , BitsRawBufCtrlIDSUERROR , ShiftRawBufCtrlIDSUERROR );
  
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute outputreg= "<< outputreg->getBitset() << std::endl;
  // Candidates from Control Units
  shift=ShiftRawBufCtrlFIRSTCAND;
  nCandidates=0;
  // Loop over controler candidate registers
  for (itHandlerMap=m_candRegHandlerCtrl.begin();itHandlerMap!=m_candRegHandlerCtrl.end();itHandlerMap++){
    // Loop over input candidates in register
    for (int icand = 0;icand<2;icand++) {
      int quarter = (*itHandlerMap).first;
      //       if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute quarter= "<< quarter <<" icand= "<<icand << std::endl;
      if (m_candRegHandlerCtrl[quarter].isEmpty(icand)) continue;
      unsigned long ucandidate = formattedCandidate(m_candRegHandlerCtrl[quarter],icand,quarter);
      outputreg->set(ucandidate,BitsRawBufCtrlCAND,shift);
      //       if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute add new candidate ->outputreg= "
      //                              << outputreg->getBitset() << std::endl;
      nCandidates++;
      shift+=RelShiftRawBufCtrlCAND;
      
    } // End of Loop over input candidates in register
  } // End of Loop over controler candidate registers
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute outputreg= "<< outputreg->getBitset() << std::endl;
  outputreg->set( nCandidates , BitsRawBufCtrlNCTRLCAND , ShiftRawBufCtrlNCTRLCAND );
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute outputreg= "<< outputreg->getBitset() 
  //                          << " ("<<outputreg->size()<<")"<< std::endl;
  

  // Errors

  // Candidates from Best Candidates Selection Units
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute ... done " << std::endl;
  
}

/**
   PostExecute
*/
void L0Muon::RawBufferCtrlUnit::postexecute(){
  m_evtCounter++;
  releaseOutputRegisters();
  std::map<std::string,L0Muon::Register*>::iterator itoutputs;
  for (itoutputs=m_outputs.begin();itoutputs!=m_outputs.end();itoutputs++) {
    (*itoutputs).second->getBitset().clear();
  }
  

}

unsigned long  L0Muon::RawBufferCtrlUnit::formattedCandidate(CandRegisterHandler candRegHandler, int icand, int quarter)
{
  
  unsigned long cand=0;
  if (m_debug) std::cout << "*!* RawBufferCtrlUnit::formattedCandidate IN "<<std::endl;

  int  pt      = candRegHandler.getCandPT(icand)     ;
  int  charge  = candRegHandler.getCandCharge(icand) ;
  int  addM3   = candRegHandler.getCandAddM3(icand)  ;
  int  offM2   = candRegHandler.getCandOffM2(icand)  ;
  int  offM1   = candRegHandler.getCandOffM1(icand)  ;
  int  pu      = candRegHandler.getCandPU(icand)     ;
  int  board   = candRegHandler.getCandBoard(icand)  ;

  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::formattedCandidate pt    = "<< pt    
  //                          << " = " << (unsigned long) ((pt     << RelShiftRawBufCtrlPT)     &  RelMaskRawBufCtrlPT)
  //                          << " = " << (unsigned long) ((pt     << RelShiftRawBufCtrlPT)    )
  //                          <<" ; bits: "  << BitsRawBufCtrlPT   
  //                          <<" ; mask: "  << RelMaskRawBufCtrlPT   
  //                          <<" ; shift: " << RelShiftRawBufCtrlPT 
  //                          <<std::endl;  
  cand +=  (pt     << RelShiftRawBufCtrlPT)     &  RelMaskRawBufCtrlPT;
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::formattedCandidate cand    = "<< cand  <<std::endl;
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::formattedCandidate charge= "<< charge
  //                          << " = " << (unsigned long) ((charge << RelShiftRawBufCtrlCHARGE) & RelMaskRawBufCtrlCHARGE)
  //                          << " = " << (unsigned long) ((charge << RelShiftRawBufCtrlCHARGE) )
  //                          <<" ; bits: "  << BitsRawBufCtrlCHARGE   
  //                          <<" ; mask: "  << RelMaskRawBufCtrlCHARGE   
  //                          <<" ; shift: " << RelShiftRawBufCtrlCHARGE 
  //                          <<std::endl;
  cand +=  (charge << RelShiftRawBufCtrlCHARGE) & RelMaskRawBufCtrlCHARGE;                             
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::formattedCandidate cand    = "<< cand  <<std::endl;
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::formattedCandidate addM3 = "<< addM3 
  //                          << " = " <<  (unsigned long) ((addM3  << RelShiftRawBufCtrlADDM3)  & RelMaskRawBufCtrlADDM3)
  //                          << " = " <<  (unsigned long) ((addM3  << RelShiftRawBufCtrlADDM3) )
  //                          <<" ; bits: "  << BitsRawBufCtrlADDM3   
  //                          <<" ; mask: "  << RelMaskRawBufCtrlADDM3  
  //                          <<" ; shift: " << RelShiftRawBufCtrlADDM3 
  //                          <<std::endl;
  cand +=  (addM3  << RelShiftRawBufCtrlADDM3)  & RelMaskRawBufCtrlADDM3;                             
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::formattedCandidate cand    = "<< cand  <<std::endl;
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::formattedCandidate offM2 = "<< offM2 
  //                          << " = " << (unsigned long) ((offM2  << RelShiftRawBufCtrlOFFM2)  & RelMaskRawBufCtrlOFFM2)
  //                          << " = " << (unsigned long) ((offM2  << RelShiftRawBufCtrlOFFM2)  )
  //                          <<" ; bits: "  << BitsRawBufCtrlOFFM2   
  //                          <<" ; mask: "  << RelMaskRawBufCtrlOFFM2   
  //                          <<" ; shift: " << RelShiftRawBufCtrlOFFM2 
  //                          <<std::endl;
  cand +=  (offM2  << RelShiftRawBufCtrlOFFM2)  & RelMaskRawBufCtrlOFFM2;                             
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::formattedCandidate cand    = "<< cand  <<std::endl;
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::formattedCandidate offM1 = "<< offM1 
  //                          << " = " << (unsigned long) ((offM1  << RelShiftRawBufCtrlOFFM1)  & RelMaskRawBufCtrlOFFM1)
  //                          << " = " << (unsigned long) ((offM1  << RelShiftRawBufCtrlOFFM1)  )
  //                          <<" ; bits: "  << BitsRawBufCtrlOFFM1   
  //                          <<" ; mask: "  << RelMaskRawBufCtrlOFFM1   
  //                          <<" ; shift: " << RelShiftRawBufCtrlOFFM1 
  //                          <<std::endl;
  cand +=  (offM1  << RelShiftRawBufCtrlOFFM1)  & RelMaskRawBufCtrlOFFM1;                             
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::formattedCandidate cand    = "<< cand  <<std::endl;
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::formattedCandidate pu    = "<< pu     
  //                          << " = " << (unsigned long) ((pu     << RelShiftRawBufCtrlPU)     & RelMaskRawBufCtrlPU)
  //                          << " = " << (unsigned long) ((pu     << RelShiftRawBufCtrlPU)     )
  //                          <<" ; bits: "  << BitsRawBufCtrlPU   
  //                          <<" ; mask: "  << RelMaskRawBufCtrlPU   
  //                          <<" ; shift: " << RelShiftRawBufCtrlPU 
  //                          <<std::endl;
  cand +=  (pu     << RelShiftRawBufCtrlPU)     & RelMaskRawBufCtrlPU;                             
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::formattedCandidate cand    = "<< cand  <<std::endl;
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::formattedCandidate board = "<< board  
  //                          << " = " <<  (unsigned long) ((board  << RelShiftRawBufCtrlBOARD)  &RelMaskRawBufCtrlBOARD)
  //                          << " = " <<  (unsigned long) ((board  << RelShiftRawBufCtrlBOARD)  )
  //                          <<" ; bits: "  << BitsRawBufCtrlBOARD 
  //                          <<" ; mask: "  << RelMaskRawBufCtrlBOARD 
  //                          <<" ; shift: " << RelShiftRawBufCtrlBOARD
  //                          <<std::endl;
  cand +=  (board  << RelShiftRawBufCtrlBOARD)  &RelMaskRawBufCtrlBOARD;
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::formattedCandidate cand    = "<< cand  <<std::endl;
  cand +=  (quarter  << RelShiftRawBufCtrlQUARTER)  &RelMaskRawBufCtrlQUARTER;

      
  return cand;
  
}

