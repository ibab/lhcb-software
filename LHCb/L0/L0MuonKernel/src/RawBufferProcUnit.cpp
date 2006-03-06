#include "L0MuonKernel/RawBufferProcUnit.h"

/**
   Constructor.
*/
L0Muon::RawBufferProcUnit::RawBufferProcUnit(){
};
/**
   Constructor.
*/
L0Muon::RawBufferProcUnit::RawBufferProcUnit(LHCb::MuonTileID id):RawBufferUnit(id){
  m_evtCounter=0;
};

/**
   Constructor.
*/
L0Muon::RawBufferProcUnit::RawBufferProcUnit(DOMNode* pNode):RawBufferUnit(pNode){
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

  RawBufferUnit::initialize();

  char buf[4096];
  char* format ;


  for (int iboard = 0; iboard <12 ; iboard++) {
    // Candidate Register handlers for BCSU candidates
    format = "BOARD%d";
    sprintf(buf,format,iboard);
    std::map<std::string,L0Muon::Register*>::iterator itinputs =  m_inputs.find(buf);
    if (itinputs==m_inputs.end()) continue;
    Register* reg = (*itinputs).second;
    m_candRegHandlerBCSU[iboard] = CandRegisterHandler(reg) ;
    for (int ipu = 0; ipu <4 ; ipu++) {
      // Candidate Register handlers for PU candidates
      format = "BOARD%dPU%d";
      sprintf(buf,format,iboard,ipu);
      std::map<std::string,L0Muon::Register*>::iterator itinputs =  m_inputs.find(buf);
      if (itinputs==m_inputs.end()) continue;
      std::pair<int,int> index(iboard,ipu);
      Register* reg = (*itinputs).second;
      m_candRegHandlerProc[index] = CandRegisterHandler(reg) ;
    }
  }


// //   // Loop over PUs and print formatted register tile content
// //   for (int region = 0; region < 4; region++) { // Loop over regions
// //     for (int nX=0; nX<4; nX++) {                  // Loop over X indices
// //       for (int nY=0; nY<4; nY++) {                    // Loop over Y indices
// //         if (nX<2 && nY<2) continue;
// //         int quarter = m_mid.quarter();
// //         // Get a pointer to the formatted data input register
// //         format = "FORMATTEDDATAIN_Q%dR%d%d%d";
// //         sprintf(buf,format, quarter+1,region+1,nX,nY);
// //         std::map<std::string,L0Muon::Register*>::iterator itformatted =  m_inputs.find(buf);
// //         //         std::cout <<"L0Muon::RawBufferProcUnit::execute key formatted register key "<<buf  <<"\n";
// //         if (itformatted!=m_inputs.end()) { // If PU register has been found
// //           TileRegister* formattedReg = dynamic_cast<TileRegister*>(itformatted->second);
// //           std::cout << buf<<"\n";
// //           formattedReg->print_tiles(0,1);
// //         } // End If PU register has been found
// //       } // End Of Loop over Y indices
// //     } // End Of Loop over X indices
// //   } // End Of Loop over regions
  
}



/**
   Execute
*/
void L0Muon::RawBufferProcUnit::execute(){
  //   if (m_debug) std::cout << "*!* RawBufferProcUnit::execute evt= "<< m_evtCounter << std::endl;

  char buf[4096];
  char* format ;
  int nCandidates;
  int shift;
  int startCompression=-1, stopCompression=-1;
  
  // Get a pointer to the Rawbuffer Proc output register
  format = "RAWBUFFER_PROCQ%d";
  sprintf(buf,format, m_mid.quarter()+1);
  //   if (m_debug) std::cout << "*!* RawBufferProcUnit::execute buf= "<< buf << std::endl;
  //   if (m_debug) std::cout << "*!* RawBufferProcUnit::execute # of output registers= "<<m_outputs.size()  <<"\n";
  std::map<std::string,L0Muon::Register*>::iterator itoutputs =  m_outputs.find(buf);
  if (itoutputs==m_outputs.end()) {
    std::cout <<"L0Muon::RawBufferProcUnit::initialize key "<<buf  <<" not found in output registers\n";
    return;
  }
  //   if (m_debug) std::cout << "*!* RawBufferProcUnit::execute key= "<< buf <<" found in output registers\n";
  Register* outputreg =(*itoutputs).second;
  //   if (m_debug) std::cout << "*!* RawBufferProcUnit::execute outputreg= "<< outputreg->getBitset() << std::endl;

  // Set the Global ID
  outputreg->set(m_evtCounter , BitsRawBufProcL0EVTNUM  , ShiftRawBufProcL0EVTNUM  );
  outputreg->set(m_evtCounter , BitsRawBufProcL0BID     , ShiftRawBufProcL0BID     );

  // Errors
  outputreg->set(           0 , BitsRawBufProcIDERROR , ShiftRawBufProcIDERROR );
  
  //   if (m_debug) std::cout << "*!* RawBufferProcUnit::execute outputreg= "<< outputreg->getBitset() << std::endl;
  // Output candidates from Best Canidates Selection Units
  shift=ShiftRawBufProcFIRSTCAND;
  nCandidates=0;
  // Loop over controler candidate registers
  std::map<int, CandRegisterHandler>::iterator itHandlerBCSU;
  for (itHandlerBCSU=m_candRegHandlerBCSU.begin();itHandlerBCSU!=m_candRegHandlerBCSU.end();itHandlerBCSU++){
    // Loop over input candidates in register
    int iboard = (*itHandlerBCSU).first;
    for (int icand = 0;icand<2;icand++) {
      //       if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute quarter= "<< quarter <<" icand= "<<icand << std::endl;
      if (m_candRegHandlerBCSU[iboard].isEmpty(icand)) continue;
      unsigned long ucandidate = formattedCandidate(m_candRegHandlerBCSU[iboard],icand,-1);
      outputreg->set(ucandidate,BitsRawBufCAND,shift);
      //       if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute add new candidate ->outputreg= "
      //                              << outputreg->getBitset() << std::endl;
      nCandidates++;
      shift+=RelShiftRawBufCAND;
      
    } // End of Loop over input candidates in register
  } // End of Loop over controler candidate registers
  //   if (m_debug) std::cout << "*!* RawBufferCtrlUnit::execute outputreg= "<< outputreg->getBitset() << std::endl;
  outputreg->set( nCandidates , BitsRawBufProcNBCSUCAND , ShiftRawBufProcNBCSUCAND );
  
  // Output candidates from Proc Units
  int shiftPuCandidate = shift;
  shift+=BitsRawBufProcNPUCAND;
  nCandidates=0;
  //   if (m_debug) std::cout << "*!* RawBufferProcUnit::execute m_candRegHandlerProc size= "
  //                          << m_candRegHandlerProc.size()<< std::endl;
  // Loop over controler candidate registers
  std::map<std::pair<int,int>, CandRegisterHandler>::iterator itHandlerPU;
  for (itHandlerPU=m_candRegHandlerProc.begin();itHandlerPU!=m_candRegHandlerProc.end();itHandlerPU++){
    // Loop over input candidates in register
    std::pair<int,int> index = (*itHandlerPU).first;
    for (int icand = 0;icand<2;icand++) {
      //       if (m_debug) std::cout << "*!* RawBufferProcUnit::execute "
      //                              << " board= "<< index.first
      //                              << " pu= "<< index.second 
      //                              <<" icand= "<<icand 
      //                              << std::endl;
      if (m_candRegHandlerProc[index].isEmpty(icand)) continue;
      unsigned long ucandidate = formattedCandidate(m_candRegHandlerProc[index],icand,-1);
      //       if (m_debug) std::cout << "*!* RawBufferProcUnit::execute = ucandidate"
      //                              << ucandidate<< std::endl;
      outputreg->set(ucandidate,BitsRawBufCAND,shift);
      //       if (m_debug) std::cout << "*!* RawBufferProcUnit::execute add new candidate ->outputreg= "
      //                              << outputreg->getBitset() << std::endl;
      nCandidates++;
      shift+=RelShiftRawBufCAND;
      
    } // End of Loop over input candidates in register
  } // End of Loop over controler candidate registers
  //   if (m_debug) std::cout << "*!* RawBufferProcUnit::execute outputreg= "<< outputreg->getBitset() << std::endl;
  outputreg->set( nCandidates , BitsRawBufProcNPUCAND , shiftPuCandidate );

  // Input data to PU
  // ----------------
  int procID=0;
  int procIDField = shift;
  shift+=BitsRawBufProcPROCID;
  outputreg->set(0,BitsRawBufProcPROCID,procIDField+procID);
  startCompression = shift;
  // Loop over PUs
  for (int region = 0; region < 4; region++) { // Loop over regions
    for (int nX=0; nX<4; nX++) {                  // Loop over X indices
      for (int nY=0; nY<4; nY++) {                    // Loop over Y indices
        if (nX<2 && nY<2) continue;
        int quarter = m_mid.quarter();
        // Get a pointer to the formatted data input register
        format = "FORMATTEDDATAIN_Q%dR%d%d%d";
        sprintf(buf,format, quarter+1,region+1,nX,nY);
        std::map<std::string,L0Muon::Register*>::iterator itformatted =  m_inputs.find(buf);
        //         std::cout <<"L0Muon::RawBufferProcUnit::execute key formatted register key "<<buf  <<"\n";
        if (itformatted!=m_inputs.end()) {// If PU register has been found
          TileRegister* formattedReg = dynamic_cast<TileRegister*>(itformatted->second);

          // If formatted register is not set, set it
          //         std::cout <<"L0Muon::RawBufferProcUnit::execute formattedReg->isSet() ? "<< formattedReg->isSet()  <<"\n";
          if (formattedReg->isSet()) formattedReg->reset();
        
          // Loop over the raw input registers 
          for (unsigned int count=0; count<m_inputs.size(); count++) { 
            format = "RAWDATAINQ%dR%d%d%d_%d";
            sprintf(buf,format,quarter+1,region+1,nX,nY, count);
            //             std::cout <<"L0Muon::RawBufferProcUnit::execute key raw register key "<<buf   <<"\n";
            std::map<std::string,L0Muon::Register*>::iterator itraw =  m_inputs.find(buf);
            if (itraw==m_inputs.end()) break;
            TileRegister* rawReg = dynamic_cast<TileRegister*>(itraw->second);
            std::vector<LHCb::MuonTileID> firedTiles = rawReg->firedTiles();
            for (std::vector<LHCb::MuonTileID>::iterator ittile = firedTiles.begin();ittile<firedTiles.end();ittile++){
              formattedReg->setTile(*ittile);
            }
          } // End of Loop over the raw input registers

          // Uncomment next line to spare some bits; comment it to have uncompress raw buffer of fixed size 
          if (!formattedReg->isSet()) continue; 

          // Set the output register
          outputreg->set(formattedReg->getBitset(),formattedReg->size() ,shift );
          outputreg->set(1,1,procIDField+procID);
          shift+=formattedReg->size();
          
//           outputreg->set(formattedReg->getBitset(),BitsRawBufProcINPUTDATA ,shift );
        } // End If PU register has been found
        
//         shift+=BitsRawBufProcINPUTDATA;

        procID+=1;

      } // End of Loop over Y indices
    } // End of Loop over X indices    
  } // End of Loop over regions
  
  stopCompression = shift;

  if (m_compress) compress(outputreg, startCompression, stopCompression);
  
  RawBufferUnit::execute();
}
