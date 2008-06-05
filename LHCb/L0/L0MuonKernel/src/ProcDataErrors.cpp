#include "L0MuonKernel/ProcDataErrors.h"

L0Muon::ProcDataErrors::ProcDataErrors()
{

  decoding = L0Muon::ErrorHandler(" %01X",0x00F);
  for (int ipu=0; ipu<4; ++ipu){
    opt_link[ipu] =  L0Muon::ErrorHandler(" %02X",0x0FF);
    par_link[ipu] =  L0Muon::ErrorHandler(" %02X",0x0FF);
    ser_link[ipu] =  L0Muon::ErrorHandler(" %02X",0x03F);
  }

}

L0Muon::ProcDataErrors::~ProcDataErrors(){}
    
bool L0Muon::ProcDataErrors::inError()
{

  bool error=false;

  for (int ipu=0; ipu<4; ++ipu){
    error|=opt_link[ipu].inError();
    error|=ser_link[ipu].inError();
    error|=par_link[ipu].inError();
  }

  return error;
}


void L0Muon::ProcDataErrors::printCounters(std::string &os,std::string tab) const {

  for (int ipu=0; ipu<4; ++ipu){
    opt_link[ipu].printCounter(os,tab+(boost::format("OPT LINK%1d") % ipu).str());
  }
  for (int ipu=0; ipu<4; ++ipu){
    ser_link[ipu].printCounter(os,tab+(boost::format("SER LINK%1d") % ipu).str());
  }
  for (int ipu=0; ipu<4; ++ipu){
    par_link[ipu].printCounter(os,tab+(boost::format("PAR LINK%1d") % ipu).str());
  }

}

int L0Muon::ProcDataErrors::sumCounters() const {
  
  int n=0;

  for (int i=0; i<4; ++i){  
    n+=opt_link[i].counter();
    n+=ser_link[i].counter();
    n+=par_link[i].counter();
  }
  
  return n;
  
}

std::ostream &L0Muon::operator<<(std::ostream &os, const L0Muon::ProcDataErrors &pberr) {

  if (pberr.decodingError()) {
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


