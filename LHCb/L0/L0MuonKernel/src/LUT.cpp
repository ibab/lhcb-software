#include "L0MuonKernel/LUT.h"

L0Muon::LUT::LUT() {
}

L0Muon::LUT::~LUT() {

}



void L0Muon::LUT::calculateAddr(int ncand){
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
  std::map<std::string,L0Muon::Register*>::iterator ind;
  std::map<std::string,L0Muon::Register*> reg = rfactory->getRegisters();
  char buf[4096] ; //, buf1[4096], buf2[4096];
  char * nameaddr1 ="(R%d,%d,%d)_addr_candidate%d";
  sprintf(buf, nameaddr1 ,m_pu.region(),m_pu.nX(),m_pu.nY(),ncand);
  for (ind = reg.begin(); ind != reg.end(); ind ++){
    if ((*ind).first == buf){
        Register * reg = dynamic_cast<Register*>((*ind).second);
        boost::dynamic_bitset<> addr = reg->getBitset();
        //int nbits = addr.size();
    }
    
  }
  }

boost::dynamic_bitset<> L0Muon::LUT::calculatePt(signed long int  Pt)
{
  boost::dynamic_bitset<> bitsetPt(8,Pt);
  
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();
  char buf[4096];
  char * format = "(R%d,%d,%d)_Pt";
  sprintf(buf,format,m_pu.region(),m_pu.nX(),m_pu.nY());
  int nbits = bitsetPt.size();
  Register * pReg = rfactory->createRegister(buf,nbits);
  pReg->set(bitsetPt);
  addOutputRegister(pReg);
  return bitsetPt ;  
}



void L0Muon::LUT::initialize(){
}

void L0Muon::LUT::execute(){
  calculateAddr(1);
  calculatePt(0);
  
}

void L0Muon::LUT::finalize(){
}

