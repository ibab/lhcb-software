#include "L0MuonKernel/BCSUL0BufferCnv.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "ProcessorKernel/Register.h"
#include <iostream>

/**
   Constructor.
*/
L0Muon::BCSUL0BufferCnv::BCSUL0BufferCnv(LHCb::MuonTileID id):L0BufferCnv(id){

  char buf[4096];
  char* format ;

  m_valid=true;

  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

  format = "CAND_BCSUQ%dR%d%d%d";
  sprintf(buf,format,m_mid.quarter()+1,m_mid.region()+1,m_mid.nX(),m_mid.nY());
  L0Muon::Register* reg = rfactory->searchRegister(buf);
  m_candRegHandler = CandRegisterHandler(reg) ;
  if (reg==0) m_valid = false;

  for (int ipu=0; ipu<4; ++ipu){
    format = "PUCAND_BCSUQ%dR%d%d%d_%d";
    sprintf(buf,format,m_mid.quarter()+1,m_mid.region()+1,m_mid.nX(),m_mid.nY(),ipu);
    L0Muon::Register* reg = rfactory->searchRegister(buf);
    m_candRegHandlerPU[ipu] = CandRegisterHandler(reg) ;
  }
  
};

/**
   Destructor
*/
L0Muon::BCSUL0BufferCnv::~BCSUL0BufferCnv(){

}

void L0Muon::BCSUL0BufferCnv::write(int ievt)
{
  if (!m_valid) return;

  unsigned int iword;
  
  iword = 0x8000+(ievt&0xFFF);
  m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // L0EventNumber (l.0)
  iword = (ievt&0xFFF);
  m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // L0_B_Id (l.1)
  
  int pt1 = m_candRegHandler.getCandPT(0) | ( (m_candRegHandler.getCandCharge(0)<<7) & 0x80) ;
  int pt2 = m_candRegHandler.getCandPT(1) | ( (m_candRegHandler.getCandCharge(1)<<7) & 0x80) ;
  iword  = ( (pt1<<8) & 0xFF00) | ( pt2 & 0x00FF);
  m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // Pts  (l.2)

  for (int icand=0;icand<2;icand++) {
    iword =(   m_candRegHandler.getCandOffM1(  icand)      & 0x000F);
    iword|=( ( m_candRegHandler.getCandOffM2(  icand)<<4 ) & 0x00F0 );
    iword|=( ( m_candRegHandler.getCandColM3(  icand)<<8 ) & 0x1F00 );
    iword|=( ( m_candRegHandler.getCandRowM3(  icand)<<13) & 0x6000 );
    m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // addresses  (l.3 & l.4)
  }

  int bid    =  (ievt&0xF); 
  int status = m_candRegHandler.getStatus();
  int pu1    = m_candRegHandler.getCandPU(0);
  int pu2    = m_candRegHandler.getCandPU(1);
  iword = (  ((pu1<<12)&0x3000) + ((pu2<<8)&0x300) + ((status<<4)&0xF0) + bid)  & 0x33FF;
  m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // Status and bid (l.5)

  for (int ipu=0; ipu<4; ++ipu){
    if (m_candRegHandlerPU[ipu].isValid()) {
      int pt1 = m_candRegHandlerPU[ipu].getCandPT(0) | ( (m_candRegHandlerPU[ipu].getCandCharge(0)<<7) & 0x80) ;
      int pt2 = m_candRegHandlerPU[ipu].getCandPT(1) | ( (m_candRegHandlerPU[ipu].getCandCharge(1)<<7) & 0x80) ;
      iword  = ( (pt1<<8) & 0xFF00) | ( pt2 & 0x00FF);
      m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // Pts  (l.2)

      for (int icand=0;icand<2;icand++) {
        iword =(   m_candRegHandlerPU[ipu].getCandOffM1(  icand)      & 0x000F);
        iword|=( ( m_candRegHandlerPU[ipu].getCandOffM2(  icand)<<4 ) & 0x00F0 );
        iword|=( ( m_candRegHandlerPU[ipu].getCandColM3(  icand)<<8 ) & 0x1F00 );
        iword|=( ( m_candRegHandlerPU[ipu].getCandRowM3(  icand)<<13) & 0x6000 );
        m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // addresses  (l.3 & l.4)
      }

      int bid =  (ievt&0xF); 
      int status = m_candRegHandlerPU[ipu].getStatus();
      iword = (  ((status<<4)&0xF0) + bid)  & 0x00FF;
      m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // Status and bid (l.5)
    } else {
      for (int i=0; i<4; ++i) m_file<<std::setw(4)<<std::setfill('0')<<0<<" \n"; // empty
    }
  }
  
  m_file<<"----\n";
  
}




