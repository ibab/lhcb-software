#include "L0MuonKernel/CtrlAllCnv.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "ProcessorKernel/Register.h"
#include "L0MuonKernel/BankUtilities.h"

/**
   Constructor.
*/
L0Muon::CtrlAllCnv::CtrlAllCnv(){
  m_side=0;
};
/**
   Constructor.
*/
L0Muon::CtrlAllCnv::CtrlAllCnv(int side){
  m_side=side;

  char buf[4096];
  char* format ;
  
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

    
  for (int iq = 0; iq<2 ; ++iq){
    for (int iboard = 0; iboard <12 ; iboard++) {
      format = "BOARDCAND_Q%d_%d";
      sprintf(buf,format,m_side*2+iq+1,iboard);
      //      L0Muon::Register* reg = rfactory->createRegister(buf,L0Muon::BitsCandRegTot);
      L0Muon::Register* reg = rfactory->createRegister(buf,CandRegisterHandler::size());
      m_candRegHandler[iq][iboard] = CandRegisterHandler(reg) ;
    }
  }
  
};


/**
   Destructor
*/
L0Muon::CtrlAllCnv::~CtrlAllCnv(){
}

void L0Muon::CtrlAllCnv::release(){
  for (int iq = 0; iq<2 ; ++iq) {
    for (int ib = 0; ib <12 ; ++ib) {
      m_candRegHandler[iq][ib].clear();
    }
  }
}

std::vector<L0Muon::PMuonCandidate>  L0Muon::CtrlAllCnv::muonCandidates(){
  std::vector<L0Muon::PMuonCandidate> cands;
  for (int iq = 0; iq<2 ; ++iq){
    for (int ib= 0; ib<12 ;++ib) {
      int ncand= m_candRegHandler[iq][ib].numberOfCandidates();
      for (int icand = 0;icand<ncand;icand++) {
        cands.push_back(m_candRegHandler[iq][ib].getMuonCandidate(icand));
      }    
    }
  }
  
  return cands;
}

void L0Muon::CtrlAllCnv::decodeBank(std::vector<unsigned int> raw, int bankVersion)
{
  // Clear the registers first
  release();
  
//   std::cout.setf(std::ios::uppercase) ;
//   std::cout<<"\t=> L0Muon::CtrlAllCnv::decodeBank -- dump raw bank of size: "<<raw.size()<<std::hex<<std::endl;
//   for (std::vector<unsigned int>::iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
//     std::cout <<"\t0x"<<std::setw(8)<<(*itraw)<<std::endl;
//   }
//   std::cout<<std::dec;
//   std::cout.unsetf(std::ios::uppercase);
  
  // Loop over controller boards
  for (int iq=0; iq<2; ++iq) {

    int ncand[12]={0,0,0,0,0,0,0,0,0,0,0,0};
    int iwd = 0;
    int cursor = 0;

    // 1st loop over processing boards
    for (int ib = 0; ib <12 ; ++ib) {
      int n=(raw[iwd]>>cursor)& 0x3;
      n = n>2 ? 2 : n;
      ncand[ib] = n;
      cursor+=4;
      if (cursor==32) { // next word
        cursor=0;
        iwd=1;
      }
      
    } // End of 1st loop over processing boards

    // 2nd loop over processing boards
    for (int ib = 0; ib <12 ; ++ib) {
      // Loop over the candidates of the processing board
      for (int icand =0;icand<ncand[ib];++icand){
        L0Muon::writeCandInRegister(&m_candRegHandler[iq][ib],raw[iwd+3+icand],icand,bankVersion);  
        m_candRegHandler[iq][ib].setCandQuarter(m_side*2+iq,icand); 
        m_candRegHandler[iq][ib].setCandBoard(ib,icand); 
      } // End of loop over the candidates of the processing board
    } // End of 2nd loop over processing boards

  } // End of loop over controller boards

}

std::vector<unsigned int> L0Muon::CtrlAllCnv::rawBank(int bankVersion)
{
  std::vector<unsigned int> raw;

  // Loop over controller boards
  for (int iq=0; iq<2; ++iq) {

    unsigned int word = 0;
    int cursor = 0;
    // 1st loop over processing boards
    for (int ib = 0; ib <12 ; ++ib) {
      word |= ( ((m_candRegHandler[iq][ib].getStatus())&0xF)<<cursor );
      cursor+=4;
      if (cursor==32) { // next word
        cursor=0;
        raw.push_back(word);
      }  
    } // End of 1st loop over processing boards
    raw.push_back(word);
    // 2nd loop over processing boards
    for (int ib = 0; ib <12 ; ++ib) {
      // Loop over the candidates of the processing board
      for (int icand=0; icand<2; icand++){
        if (m_candRegHandler[iq][ib].isEmpty(icand)) break;
        word = L0Muon::readCandFromRegister(&m_candRegHandler[iq][ib], icand, bankVersion);
        raw.push_back(word&0x30FFFFFF); // mask the quarter and the board
      } // End of loop over the candidates of the processing board
    } // End of 2nd loop over processing boards

  } // End of loop over controller boards
  
//   std::cout.setf(std::ios::uppercase) ;
//   std::cout<<"\t=> L0Muon::CtrlAllCnv::rawBank -- dump raw bank of size: "<<raw.size()<<std::hex<<std::endl;
//   for (std::vector<unsigned int>::iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
//     std::cout <<"\t0x"<<std::setw(8)<<(*itraw)<<std::endl;
//   }
//   std::cout<<std::dec;
//   std::cout.unsetf(std::ios::uppercase);

  return raw;
}



