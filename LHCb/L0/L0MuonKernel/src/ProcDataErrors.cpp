#include "L0MuonKernel/ProcDataErrors.h"

L0Muon::ProcDataErrors::ProcDataErrors()
{
  present = L0Muon::ErrorHandler(" %01X",0x1,"nb of banks seen");
  decoding = L0Muon::ErrorHandler(" %08X",0xFFFFFFFF,"decoding");
  for (int ipu=0; ipu<4; ++ipu){
    std::string name;
    name=(boost::format("PU%1d opt. links") % ipu).str();
    opt_link[ipu] =  L0Muon::ErrorHandler(" %02X",0x0FF,name);
    name=(boost::format("PU%1d par. links") % ipu).str();
    par_link[ipu] =  L0Muon::ErrorHandler(" %02X",0x0FF,name);
    name=(boost::format("PU%1d ser. links") % ipu).str();
    ser_link[ipu] =  L0Muon::ErrorHandler(" %02X",0x0FF,name);
  }

}

L0Muon::ProcDataErrors::~ProcDataErrors(){}
    
bool L0Muon::ProcDataErrors::inError(int ipu) const
{
  bool error=false;

  error|=(decodingError()>0);
  error|=(hardwareError(ipu)>0);

  return error;
}

int L0Muon::ProcDataErrors::decodingError() const {
 return int(decoding.inError());
}

int L0Muon::ProcDataErrors::hardwareError(int ipu) const {
  return ( ((opt_link[ipu].value()<<16)&0x00FF0000) 
           + ((ser_link[ipu].value()<<8)&0x0000FF00) 
           + ((par_link[ipu].value()<<0)&0x000000FF) );
}

int L0Muon::ProcDataErrors::sumCounters() const {
  
  int n=0;

  n+=decoding.counter();
  for (int i=0; i<4; ++i){  
    n+=opt_link[i].counter();
    n+=ser_link[i].counter();
    n+=par_link[i].counter();
  }
  
  return n;
  
}

std::ostream &L0Muon::operator<<(std::ostream &os, const L0Muon::ProcDataErrors &pberr) {

  if (pberr.decoding.inError()) {
    os<<" Data corrupted ";
  } else {
    for (int ipu=0; ipu<4; ++ipu) os<<pberr.opt_link[ipu];
    os<<" ";
    for (int ipu=0; ipu<4; ++ipu) os<<pberr.ser_link[ipu];
    os<<" ";
    for (int ipu=0; ipu<4; ++ipu) os<<pberr.par_link[ipu];
  }
  
  return os;
}


