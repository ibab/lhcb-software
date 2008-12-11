#include "L0MuonKernel/OLCnv.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "ProcessorKernel/Register.h"
#include <iostream>

/**
   Constructor.
*/
L0Muon::OLCnv::OLCnv(LHCb::MuonTileID id):L0BufferCnv(id){

  char buf[4096];
 
  m_valid=true;

  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

  sprintf(buf,"OLDUMP_Q%dR%d%d%d",m_mid.quarter()+1,m_mid.region()+1,m_mid.nX(),m_mid.nY());
  
  m_ol = rfactory->searchTileRegister(buf);

  if (m_ol==0 ) m_valid = false;
};

/**
   Destructor
*/
L0Muon::OLCnv::~OLCnv(){
}

void L0Muon::OLCnv::write(int ievt)
{
  if (!m_valid) return;

  unsigned int iword;

  m_file<<"evt# "<<ievt<<"\n";
  for (int il=0; il<8; ++il) {
    for (int iw=0; iw<2; ++iw) {    
      int shift = (8-1-il)*32 + (1-iw)*16;
      iword = m_ol->getulong(16,shift);
      m_file<<std::setw(4)<<std::setfill('0')<<iword<<" "; 
    }
    m_file <<"\n"; 
  }
  m_file<<"----\n";


}




