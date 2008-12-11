#include "L0MuonKernel/PUL0BufferCnv.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "ProcessorKernel/Register.h"
#include <iostream>

/**
   Constructor.
*/
L0Muon::PUL0BufferCnv::PUL0BufferCnv(LHCb::MuonTileID id):L0BufferCnv(id){

  char buf[4096];
 
  m_valid=true;

  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

  sprintf(buf,"FORMATTED_OL_Q%dR%d%d%d",m_mid.quarter()+1,m_mid.region()+1,m_mid.nX(),m_mid.nY());
  m_ol = rfactory->searchTileRegister(buf);

  sprintf(buf,"FORMATTED_NEIGH_Q%dR%d%d%d",m_mid.quarter()+1,m_mid.region()+1,m_mid.nX(),m_mid.nY());
  m_neigh = rfactory->searchTileRegister(buf);
   
  sprintf(buf,"CAND_PUQ%dR%d%d%d",m_mid.quarter()+1,m_mid.region()+1,m_mid.nX(),m_mid.nY());
  L0Muon::Register* reg = rfactory->searchRegister(buf);
  m_candRegHandler = CandRegisterHandler(reg) ;

  if (m_ol==0 || m_neigh==0 || reg==0) m_valid = false;
};

/**
   Destructor
*/
L0Muon::PUL0BufferCnv::~PUL0BufferCnv(){
}

void L0Muon::PUL0BufferCnv::write(int ievt)
{
  if (!m_valid) return;

  unsigned int iword;
  
  iword = 0x8000+(ievt&0xFFF);
  m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // L0EventNumber (l.0)
  iword = (ievt&0xFFF);
  m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // L0_B_Id (l.1)
  
  for (int icand=0;icand<2;icand++) {
    iword =(   m_candRegHandler.getCandOffM1(  icand)      & 0x000F);
    iword|=( ( m_candRegHandler.getCandOffM2(  icand)<<4 ) & 0x00F0 );
    iword|=( ( m_candRegHandler.getCandColM3(  icand)<<8 ) & 0x1F00 );
    iword|=( ( m_candRegHandler.getCandRowM3(  icand)<<13) & 0x6000 );
    m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // addresses  (l.2 & l.3)
  }

  int pt1 = m_candRegHandler.getCandPT(0) | ( (m_candRegHandler.getCandCharge(0)<<7) & 0x80) ;
  int pt2 = m_candRegHandler.getCandPT(1) | ( (m_candRegHandler.getCandCharge(1)<<7) & 0x80) ;
  iword  = ( (pt1<<8) & 0xFF00) | ( pt2 & 0x00FF);
  m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // Pts  (l.4)
  
//   int bid =  (ievt&0xF); 
  int bid =  0xF; 
  int status = m_candRegHandler.getStatus();
  iword = ( ( ((status<<4)&0xF0) + bid)<<8 ) & 0xFF00;
  m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // Status and bid (l.5)
  
  iword = 0;
  m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; // Errors (l.6)

  // OLs (l.7 to l.16)
  for (int i=0; i<10; ++i) {
    int shift = (10-1-i)*16;
    iword = m_ol->getulong(16,shift);
    m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; 
  }

  // Neighs (l.17 to l.33)
  for (int i=0; i<17; ++i) {
    int shift = (17-1-i)*16;
    iword = m_neigh->getulong(16,shift);
    m_file<<std::setw(4)<<std::setfill('0')<<iword<<" \n"; 
  }
  
  m_file<<"----\n";


}




