#include "L0MuonKernel/ProcRawErrors.h"

L0Muon::ProcRawErrors::ProcRawErrors()
{

  decoding = L0Muon::ErrorHandler(" %01X",0x00F);
  for (int ich=0; ich<2; ++ich){
    board_index[ich]  =  L0Muon::ErrorHandler(" %01X",0x00F);
    l0EventNumber[ich]=  L0Muon::ErrorHandler(" %03X",0xFFF);
    l0_B_Id[ich]      =  L0Muon::ErrorHandler(" %03X",0xFFF);
  }

  bcsu_bcid  =  L0Muon::ErrorHandler(" %01X",0x00F);
  bcsu_status= L0Muon:: ErrorHandler(" %01X",0x00C);
  for (int ipu=0; ipu<4; ++ipu){
    pus_bcid[ipu]   =  L0Muon::ErrorHandler("%01X",0x00F);
    pus_status[ipu] =  L0Muon::ErrorHandler("%01X",0x00C);
    add_link[ipu] =  L0Muon::ErrorHandler(" %00X",0x0FF);
    opt_link[ipu] =  L0Muon::ErrorHandler(" %02X",0x0FF);
    par_link[ipu] =  L0Muon::ErrorHandler(" %02X",0x0FF);
    ser_link[ipu] =  L0Muon::ErrorHandler(" %02X",0x0FF);
  }

}

L0Muon::ProcRawErrors::~ProcRawErrors(){}
    
bool L0Muon::ProcRawErrors::inError()
{

  bool error=false;

  for (int ich=0; ich<2; ++ich){
    error|=board_index[ich].inError();
    error|=l0EventNumber[ich].inError();
    error|=l0_B_Id[ich].inError();
  }

  error|=bcsu_bcid.inError();
  for (int ipu=0; ipu<4; ++ipu){
    error|=pus_bcid[ipu].inError();
  }

  error|=bcsu_status.inError();
  for (int ipu=0; ipu<4; ++ipu){
    error|=pus_status[ipu].inError();
  }

  return error;
}


void L0Muon::ProcRawErrors::printCounters(std::string &os,std::string tab) const {

  for (int i=0; i<2; ++i){
    board_index[i].printCounter(os,tab+(boost::format("board index frame %1d") % i).str());
    l0EventNumber[i].printCounter(os,tab+(boost::format("L0EventNumber frame %1d") % i).str());
    l0_B_Id[i].printCounter(os,tab+(boost::format("L0_B_Id frame %1d") % i).str());
  }
  bcsu_bcid.printCounter(os,tab+"BCID BCSU");
  for (int ipu=0; ipu<4; ++ipu){
    pus_bcid[ipu].printCounter(os,tab+(boost::format("BCID PU%1d") % ipu).str());
  }
  for (int ipu=0; ipu<4; ++ipu){
    pus_status[ipu].printCounter(os,tab+(boost::format("Status PU%1d") % ipu).str());
  }

}

int L0Muon::ProcRawErrors::sumCounters() const {
  
  int n=0;

  for (int i=0; i<2; ++i){  
    n+=board_index[i].counter();
    n+=l0EventNumber[i].counter();
    n+=l0_B_Id[i].counter();
  }
  
  n+=bcsu_bcid.counter();
  n+=bcsu_status.counter();
  for (int i=0; i<4; ++i){  
    n+=pus_bcid[i].counter();
    n+=pus_status[i].counter();
  }
  
  return n;
  
}

std::ostream &L0Muon::operator<<(std::ostream &os, const L0Muon::ProcRawErrors &pberr) {

  if (pberr.decodingError()) {
    os<<" Data corrupted ";
  } else {
    os<<pberr.board_index[0]<<pberr.board_index[1];
    os<<pberr.l0EventNumber[0]<<pberr.l0EventNumber[1];
    os<<pberr.l0_B_Id[0]<<pberr.l0_B_Id[1];
    os<<"  ";
    os<<pberr.bcsu_bcid;
    os<<" ";
    for (int ipu=0; ipu<4; ++ipu) os<<pberr.pus_bcid[ipu];
    os<<"  ";
    os<<pberr.bcsu_status;
    os<<" ";
    for (int ipu=0; ipu<4; ++ipu) os<<pberr.pus_status[ipu];
    os<<" ";
    for (int ipu=0; ipu<4; ++ipu) os<<pberr.opt_link[ipu];
    os<<" ";
    for (int ipu=0; ipu<4; ++ipu) os<<pberr.par_link[ipu];
    os<<" ";
    for (int ipu=0; ipu<4; ++ipu) os<<pberr.ser_link[ipu];
  }
  
  return os;
}


