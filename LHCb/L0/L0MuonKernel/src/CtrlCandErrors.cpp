#include "L0MuonKernel/CtrlCandErrors.h"

L0Muon::CtrlCandErrors::CtrlCandErrors()
{
  present = L0Muon::ErrorHandler(" %01X",0x1,"nb of banks seen");
  decoding = L0Muon::ErrorHandler(" %08X",0xFFFFFFFF,"decoding");
  l0EventNumber=  L0Muon::ErrorHandler(" %03X",0xFFF,"l0EventNumber");
  l0_B_Id      =  L0Muon::ErrorHandler(" %03X",0xFFF,"l0_B_Id");
  for (int ich=0; ich<2; ++ich){
    std::string name;
    name=(boost::format("bcid %1d") % ich).str();
    ctrl_bcid[ich]    =  L0Muon::ErrorHandler(" %01X",0x00F,name);
  }

  best_bcid  =  L0Muon::ErrorHandler(" %01X",0x00F,"best_bcid");
  best_status= L0Muon:: ErrorHandler(" %01X",0x00C,"best_status");
  for (int ib=0; ib<12; ++ib){
    std::string name;
    name=(boost::format("PB%2d bcid") % ib).str();
    bcsu_bcid[ib]   =  L0Muon::ErrorHandler("%01X",0x00F,name);
    name=(boost::format("PB%2d bcid mismatch") % ib).str();
    bcsu_bcid_mismatch[ib]   =  L0Muon::ErrorHandler("%01X",0x001,name);
    name=(boost::format("PB%2d status") % ib).str();
    bcsu_status[ib] =  L0Muon::ErrorHandler("%01X",0x00C,name);
    for (int ich=0; ich<2; ++ich){
      name=(boost::format("PB%2d link %1d") % ib % ich).str();
      link[ib][ich]=L0Muon::ErrorHandler("%01X",0x003,name);
    }
  }
  errJ  =  L0Muon::ErrorHandler("%01X",0x001,"errJ");
  errK  =  L0Muon::ErrorHandler("%01X",0x001,"errK");
}

L0Muon::CtrlCandErrors::~CtrlCandErrors(){}
    
bool L0Muon::CtrlCandErrors::inError() const 
{
  bool error=false;

  error|= (decodingError()>0);
  error|= (hardwareError()>0);
  error|= (bcidError()>-1);
  error|= (statusError()>2);
  
  return error;
}


bool L0Muon::CtrlCandErrors::inError(int ib) const 
{
  bool error=false;

  error|= (decodingError()>0);
  error|= (hardwareError(ib)>0);
  error|= (bcidError(ib)>-1);
  error|= (statusError(ib)>2);

  return error;
}

int L0Muon::CtrlCandErrors::decodingError() const {
  return int(decoding.inError());
}

int L0Muon::CtrlCandErrors::hardwareError() const {
  int error=0;
  if (ctrl_bcid[0].inError()) error|=((ctrl_bcid[0].value()<< 0)&0x000F);
  if (ctrl_bcid[1].inError()) error|=((ctrl_bcid[1].value()<< 4)&0x00F0);
  error|=((errJ.value()<< 8)&0x0100);
  error|=((errK.value()<<12)&0x1000);
  return error;
}

int L0Muon::CtrlCandErrors::hardwareError(int ib) const {
  int error=0;
  error|=((link[ib][0].value()           <<0)&0x003);
  error|=((link[ib][1].value()           <<4)&0x030);
  error|=((bcsu_bcid_mismatch[ib].value()<<8)&0x100);
  return error;
}

int L0Muon::CtrlCandErrors::bcidError() const {
  if (best_bcid.inError()) return best_bcid.value();
  return -1;
}

int L0Muon::CtrlCandErrors::bcidError(int ib) const {
  if (bcsu_bcid[ib].inError()) return bcsu_bcid[ib].value();
  return -1;
}

int L0Muon::CtrlCandErrors::statusError() const {
  return best_status.value();
}

int L0Muon::CtrlCandErrors::statusError(int ib) const {
  return bcsu_status[ib].value();
}

int L0Muon::CtrlCandErrors::sumCounters() const {
  
  int n=0;
  
  n+=decoding.counter();
  n+=l0EventNumber.counter();
  n+=l0_B_Id.counter();
  for (int i=0; i<2; ++i){  
    n+=ctrl_bcid[i].counter();
  }
  
  n+=best_bcid.counter();
  n+=best_status.counter();
  for (int i=0; i<12; ++i){  
    n+=bcsu_bcid[i].counter();
    n+=bcsu_bcid_mismatch[i].counter();
    n+=bcsu_status[i].counter();
    for (int ich=0; ich<2; ++ich){
      n+=link[i][ich].counter();
    }
  }
  n+=errJ.counter();
  n+=errK.counter();
  
  return n;
  
}

std::ostream &L0Muon::operator<<(std::ostream &os, const L0Muon::CtrlCandErrors &pberr) {

  if (pberr.decoding.inError()) {
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


