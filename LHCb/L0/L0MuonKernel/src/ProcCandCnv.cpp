#include "L0MuonKernel/ProcCandCnv.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "ProcessorKernel/Register.h"
#include "L0MuonKernel/BankUtilities.h"

/**
   Constructor.
*/
L0Muon::ProcCandCnv::ProcCandCnv(){
  m_quarter=0;
}
/**
   Constructor.
*/
L0Muon::ProcCandCnv::ProcCandCnv(int quarter){
  m_quarter=quarter;

  char buf[4096];
  char* format ;
  
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

  for (int iboard = 0; iboard <12 ; iboard++) {
    // Candidate Register handlers for BCSU candidates
    format = "BOARDCAND_Q%d_%d";
    sprintf(buf,format,m_quarter+1,iboard);
    //    L0Muon::Register* reg_board = rfactory->createRegister(buf,L0Muon::BitsCandRegTot);
    L0Muon::Register* reg_board = rfactory->createRegister(buf,CandRegisterHandler::size());
    m_candRegHandlerBCSU[iboard] = CandRegisterHandler(reg_board) ;
    for (int ipu = 0; ipu <4 ; ipu++) {
      // Candidate Register handlers for PU candidates
      format = "PUCAND_Q%d_%d_%d";
      sprintf(buf,format,m_quarter+1,iboard,ipu);
      //       L0Muon::Register* reg_pu = rfactory->createRegister(buf,L0Muon::BitsCandRegTot);
      L0Muon::Register* reg_pu = rfactory->createRegister(buf,CandRegisterHandler::size());
      m_candRegHandlerPU[iboard][ipu] = CandRegisterHandler(reg_pu) ;
    }
  }
  
}


/**
   Destructor
*/
L0Muon::ProcCandCnv::~ProcCandCnv(){
}

void L0Muon::ProcCandCnv::release(){

  for (int ib = 0; ib <12 ; ib++) {
    m_candRegHandlerBCSU[ib].clear();
    for (int ipu = 0; ipu <4 ; ipu++) {
      m_candRegHandlerPU[ib][ipu].clear();
    }
  }

}

std::vector<L0Muon::PMuonCandidate>  L0Muon::ProcCandCnv::muonCandidatesPU(){
  std::vector<L0Muon::PMuonCandidate> cands;
  for (int ib = 0; ib<12 ; ++ib){
    for (int ipu= 0; ipu<4 ;++ipu) {
      int ncand= m_candRegHandlerPU[ib][ipu].numberOfCandidates();
      for (int icand = 0;icand<ncand;icand++) {
        cands.push_back(m_candRegHandlerPU[ib][ipu].getMuonCandidate(icand));
      }    
    }
  }
  
  return cands;
}

std::vector<L0Muon::PMuonCandidate>  L0Muon::ProcCandCnv::muonCandidatesBCSU(){
  std::vector<L0Muon::PMuonCandidate> cands;
  for (int ib = 0; ib<12 ; ++ib){
    int ncand= m_candRegHandlerBCSU[ib].numberOfCandidates();
    for (int icand = 0;icand<ncand;icand++) {
      cands.push_back(m_candRegHandlerBCSU[ib].getMuonCandidate(icand));
    }    
  }
  
  return cands;
}

void L0Muon::ProcCandCnv::decodeBank(std::vector<unsigned int> raw, int bankVersion)
{
  // Clear the registers first
  release();

//   std::cout.setf(std::ios::uppercase) ;
//   std::cout<<"\t=> L0Muon::ProcCandCnv::decodeBank -- dump raw bank of size: "<<raw.size()<<std::hex<<std::endl;
//   for (std::vector<unsigned int>::iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
//     std::cout <<"\t0x"<<std::setw(8)<<(*itraw)<<std::endl;
//   }
//   std::cout<<std::dec;
//   std::cout.unsetf(std::ios::uppercase);

  int iwd = 1; // first ligne contains the event numbers, skip it
  for (int ib = 0; ib <12 ; ib++) {
    int ncands=0;
    
    int ncandBCSU = ((raw[iwd])& 0x3);
//     std::cout<<"\t=> L0Muon::ProcCandCnv::decodeBank -- ib= "<<ib 
//              <<" ncandBCSU = "<<ncandBCSU <<std::endl;
    // If at least one candidate in this board
    if (ncandBCSU>0) {
      ncandBCSU = ncandBCSU>2 ? 2 : ncandBCSU;

      for (int icand =0;icand<ncandBCSU;++icand){
        L0Muon::writeCandInRegister(&m_candRegHandlerBCSU[ib],raw[iwd+1+ncands],icand,bankVersion);  
        m_candRegHandlerBCSU[ib].setCandQuarter(m_quarter,icand); 
        m_candRegHandlerBCSU[ib].setCandBoard(ib,icand); 
        //m_candRegHandlerBCSU[ib].dump(-1," "); 
        ++ncands;
      } 

      for (int ipu= 0; ipu<4 ;++ipu) {
        int ncandPU =( (raw[iwd]>>(4+ipu*4)) &  0x3) ;
//         std::cout<<"\t=> L0Muon::ProcCandCnv::decodeBank -- ib= "<<ib<<" ipu= "<<ipu
//                  <<" ncandPU = "<<ncandPU <<std::endl;
        ncandPU = ncandPU>2 ? 2 : ncandPU;
        for (int icand =0;icand<ncandPU;++icand){
          L0Muon::writeCandInRegister(&m_candRegHandlerPU[ib][ipu],raw[iwd+1+ncands],icand,bankVersion);
          m_candRegHandlerPU[ib][ipu].setCandQuarter(m_quarter,icand); 
          m_candRegHandlerPU[ib][ipu].setCandBoard(ib,icand); 
          // m_candRegHandlerPU[ib][ipu].setCandPU(ipu,icand); 
          //m_candRegHandlerPU[ib][ipu].dump(-1," "); 
          ++ncands;
        } 
      }
    } // End if at least one candidate in this board
  
    iwd+=ncands+2; // 1 ligne for the event number, 1 for the status and ncands for the candidates
  }
  
}

std::vector<unsigned int> L0Muon::ProcCandCnv::rawBank(int bankVersion, int ievt)
{
  std::vector<unsigned int> raw;

  int event_number = ievt;
  int l0_bid = ievt;

  // Loop over processing boards
  for (int ib = 0; ib <12 ; ib++) {
    unsigned int word;
    
    // Event numbers
    word = ( (l0_bid<<16)&0xFFF0000 )+ ( event_number&0xFFF );
    raw.push_back(word);

    // Status
    word = m_candRegHandlerBCSU[ib].getStatus();   
    for (int ipu= 0; ipu<4 ;++ipu) {
      word|= ( m_candRegHandlerPU[ib][ipu].getStatus() <<(4+ipu*4) );
    }    
    raw.push_back(word);

    // Candidates
    for (int icand=0; icand<2; icand++){
      if (m_candRegHandlerBCSU[ib].isEmpty(icand)) break;
      word = L0Muon::readCandFromRegister(&m_candRegHandlerBCSU[ib], icand, bankVersion);
      raw.push_back(word&0x30FFFFFF);// mask the quarter and the board
    } 
    for (int ipu= 0; ipu<4 ;++ipu) {
      for (int icand=0; icand<2; icand++){
        if (m_candRegHandlerPU[ib][ipu].isEmpty(icand)) break;
        word = L0Muon::readCandFromRegister(&m_candRegHandlerPU[ib][ipu], icand, bankVersion);
        raw.push_back(word&0x30FFFFFF);// mask the quarter and the board
//        raw.push_back(word&0x00FFFFFF);// mask the quarter, the board and the PU
      } 
    }

  } // End of Loop over processing boards

//   std::cout.setf(std::ios::uppercase) ;
//   std::cout<<"\t=> L0Muon::ProcCandCnv::rawBank -- dump raw bank of size: "<<raw.size()<<std::hex<<std::endl;
//   for (std::vector<unsigned int>::iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
//     std::cout <<"\t0x"<<std::setw(8)<<(*itraw)<<std::endl;
//   }
//   std::cout<<std::dec;
//   std::cout.unsetf(std::ios::uppercase);
  
  return raw;
}



