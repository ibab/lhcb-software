#include "L0MuonKernel/SUL0BufferCnv.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "ProcessorKernel/Register.h"
#include <iostream>

/**
   Constructor.
*/
L0Muon::SUL0BufferCnv::SUL0BufferCnv(LHCb::MuonTileID id):L0BufferCnv(id){

  char buf[4096];
  char* format ;

  m_valid=true;

  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

  format = "CAND_CTRLQ%d";
  sprintf(buf,format,m_mid.quarter()+1);
  L0Muon::Register* reg = rfactory->searchRegister(buf);
  m_candRegHandler = CandRegisterHandler(reg) ;
  if (reg==0) m_valid = false;

  for (int iboard = 0; iboard <12 ; iboard++) {
    format = "BOARDCAND_Q%d_%d";
    sprintf(buf,format,m_mid.quarter()+1,iboard);
    //    L0Muon::Register* reg = rfactory->createRegister(buf,L0Muon::BitsCandRegTot);
    L0Muon::Register* reg = rfactory->createRegister(buf,CandRegisterHandler::size());
    m_candRegHandlerBCSU[iboard] = CandRegisterHandler(reg) ;
  }

  
};

/**
   Destructor
*/
L0Muon::SUL0BufferCnv::~SUL0BufferCnv(){
 
}

void L0Muon::SUL0BufferCnv::write(int ievt)
{
  
  if (!m_valid) return;

  int bid =  (ievt&0xF); 

  unsigned int iword;
  
  iword = 0x8000+(ievt&0xFFF);
  m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // L0EventNumber (l.0)
  iword = (ievt&0xFFF);
  m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // L0_B_Id (l.1)

  iword = ( bid + (bid<<4) ); 
  m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // bid

  for (int i=0; i<3; ++i) m_file<<std::setw(4)<<std::setfill('0')<<0<<" \n"; // (l.2, l.3 & l.4) serial link errors
  // Candidates from processing boards
  for (int iboard = 0; iboard <12 ; iboard++) {
    if (m_candRegHandlerBCSU[iboard].isValid()) {
      for (int icand=0;icand<2;icand++) {
        iword =( ( m_candRegHandlerBCSU[iboard].getCandOffM1(icand)    ) & 0x000F );
        iword|=( ( m_candRegHandlerBCSU[iboard].getCandOffM2(icand)<< 4) & 0x00F0 );
        iword|=( ( m_candRegHandlerBCSU[iboard].getCandPU(   icand)<< 8) & 0x0300 );
        m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // Candidate  
      }
      int status = m_candRegHandlerBCSU[iboard].getStatus();
      int c1 = m_candRegHandlerBCSU[iboard].getCandCharge(0);
      int c2 = m_candRegHandlerBCSU[iboard].getCandCharge(1);
      iword = (  ((c1<<12)&0x3000) + ((c2<<8)&0x300) + ((status<<4)&0xF0) + bid)  & 0x33FF;
      m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // bid
    } else {
      for (int i=0; i<3; ++i) m_file<<std::setw(4)<<std::setfill('0')<<0<<" \n"; // empty
    }
  }

  // Candidates from controller board
  for (int icand=0;icand<2;icand++) {
    iword =( ( m_candRegHandler.getCandOffM1(icand)    ) & 0x000F );
    iword|=( ( m_candRegHandler.getCandOffM2(icand)<< 4) & 0x00F0 );
    iword|=( ( m_candRegHandler.getCandPU(   icand)<< 8) & 0x0300 );
    iword|=( ( m_candRegHandler.getCandBoard(icand)<<12) & 0xF000 );
    m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // Candidate  
  }
  int status = m_candRegHandler.getStatus();
  int c1     = m_candRegHandler.getCandCharge(0);
  int c2     = m_candRegHandler.getCandCharge(1);
  iword = (  ((c1<<12)&0x3000) + ((c2<<8)&0x300) + ((status<<4)&0xF0) + bid)  & 0x33FF;
  m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // bid
  
  m_file<<"----\n";

}




