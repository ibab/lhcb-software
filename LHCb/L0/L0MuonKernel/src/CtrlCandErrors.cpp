#include "L0MuonKernel/CtrlCandErrors.h"

L0Muon::CtrlCandErrors::CtrlCandErrors()
{

  decoding = L0Muon::ErrorHandler(" %01X",0x00F);
  l0EventNumber=  L0Muon::ErrorHandler(" %03X",0xFFF);
  l0_B_Id      =  L0Muon::ErrorHandler(" %03X",0xFFF);
  for (int ich=0; ich<2; ++ich){
    ctrl_bcid[ich]    =  L0Muon::ErrorHandler(" %01X",0x00F);
  }

  best_bcid  =  L0Muon::ErrorHandler(" %01X",0x00F);
  best_status= L0Muon:: ErrorHandler(" %01X",0x00C);
  for (int ib=0; ib<12; ++ib){
    bcsu_bcid[ib]   =  L0Muon::ErrorHandler("%01X",0x00F);
    bcsu_status[ib] =  L0Muon::ErrorHandler("%01X",0x00C);
    for (int ich=0; ich<2; ++ich){
      link[ib][ich]=L0Muon::ErrorHandler(" %01X",0x003);
    }
  }
  errJ  =  L0Muon::ErrorHandler(" %01X",0x001);
  errK  =  L0Muon::ErrorHandler(" %01X",0x001);

}

L0Muon::CtrlCandErrors::~CtrlCandErrors(){}
    
bool L0Muon::CtrlCandErrors::inError()
{

  bool error=false;

  error|=l0EventNumber.inError();
  error|=l0_B_Id.inError();

  for (int ich=0; ich<2; ++ich){
    error|=ctrl_bcid[ich].inError();
  }

  error|=best_bcid.inError();
  for (int ib=0; ib<12; ++ib){
    error|=bcsu_bcid[ib].inError();
  }

  error|=best_status.inError();
  for (int ib=0; ib<12; ++ib){
    error|=bcsu_status[ib].inError();
  }

  return error;
}


void L0Muon::CtrlCandErrors::printCounters(std::string &os,std::string tab) const {

  l0EventNumber.printCounter(os,tab+"L0EventNumber");
  l0_B_Id.printCounter(os,tab+"L0_B_Id");
  for (int i=0; i<2; ++i){
    ctrl_bcid[i].printCounter(os,tab+(boost::format("BCID frame %1d") % i).str());
  }
  best_bcid.printCounter(os,tab+"BCID BEST");
  for (int ib=0; ib<12; ++ib){
    bcsu_bcid[ib].printCounter(os,tab+(boost::format("BCID BCSU%1d") % ib).str());
  }
  best_status.printCounter(os,tab+"STATUS BEST");
  for (int ib=0; ib<12; ++ib){
    bcsu_status[ib].printCounter(os,tab+(boost::format("Status BCSU%1d") % ib).str());
  }

}

int L0Muon::CtrlCandErrors::sumCounters() const {
  
  int n=0;

  n+=l0EventNumber.counter();
  n+=l0_B_Id.counter();
  for (int i=0; i<2; ++i){  
    n+=ctrl_bcid[i].counter();
  }
  
  n+=best_bcid.counter();
  n+=best_status.counter();
  for (int i=0; i<12; ++i){  
    n+=bcsu_bcid[i].counter();
    n+=bcsu_status[i].counter();
  }
  
  return n;
  
}

std::ostream &L0Muon::operator<<(std::ostream &os, const L0Muon::CtrlCandErrors &pberr) {

  if (pberr.decodingError()) {
    os<<" Data corrupted ";
  } else {
    os<<pberr.l0EventNumber;
    os<<pberr.l0_B_Id;
    os<<"  ";
    os<<pberr.ctrl_bcid[0]<<pberr.ctrl_bcid[1];
    os<<pberr.best_bcid;
    os<<" ";
    for (int ib=0; ib<12; ++ib) os<<pberr.bcsu_bcid[ib];
    os<<"  ";
    os<<pberr.best_status;
    os<<" ";
    for (int ib=0; ib<12; ++ib) os<<pberr.bcsu_status[ib];
  }
  
  return os;
}


