#include "L0MuonKernel/CUL0BufferCnv.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "ProcessorKernel/Register.h"
#include <iostream>

/**
   Constructor.
*/
L0Muon::CUL0BufferCnv::CUL0BufferCnv(LHCb::MuonTileID id):L0BufferCnv(id){

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
L0Muon::CUL0BufferCnv::~CUL0BufferCnv(){

}

void L0Muon::CUL0BufferCnv::write(int ievt)
{
  if (!m_valid) return;

  int bid =  (ievt&0xF); 

  unsigned int iword;
  
  iword = 0x8000+(ievt&0xFFF);
  m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // L0EventNumber (l.0)
  iword = (ievt&0xFFF);
  m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // L0_B_Id (l.1)

  for (int i=0; i<3; ++i) m_file<<std::setw(4)<<std::setfill('0')<<0<<" \n"; // (l.2, l.3 & l.4) serial link errors

  // Candidates from processing boards
  for (int iboard = 0; iboard <12 ; iboard++) {
    if (m_candRegHandlerBCSU[iboard].isValid()) {
      for (int icand=0;icand<2;icand++) {
        iword =( ( m_candRegHandlerBCSU[iboard].getCandPT(   icand)    ) & 0x007F );
        iword|=( ( m_candRegHandlerBCSU[iboard].getCandColM3(icand)<< 8) & 0x1F00 );
        iword|=( ( m_candRegHandlerBCSU[iboard].getCandRowM3(icand)<<13) & 0x6000 );
        m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // Candidate  
      }
      m_file<<std::setw(4)<<std::setfill('0')<<bid<<" \n"; // bid
    } else {
      for (int i=0; i<3; ++i) m_file<<std::setw(4)<<std::setfill('0')<<0<<" \n"; // empty
    }
  }

  // Candidates from controller board
  for (int icand=0;icand<2;icand++) {
    iword =( ( m_candRegHandler.getCandPT(   icand)    ) & 0x007F );
    iword|=( ( m_candRegHandler.getCandColM3(icand)<< 8) & 0x1F00 );
    iword|=( ( m_candRegHandler.getCandRowM3(icand)<<13) & 0x6000 );
    m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; 
  }
  m_file<<std::setw(4)<<std::setfill('0')<<bid<<" \n"; // bid

  m_file<<"----\n";

}




