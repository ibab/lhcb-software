// $Id: CtrlRawErrors.cpp,v 1.2 2008-09-04 08:25:41 jucogan Exp $
// Include files 



// local
#include "L0MuonKernel/CtrlRawErrors.h"

L0Muon::CtrlRawErrors::CtrlRawErrors()
{

  decoding = L0Muon::ErrorHandler(" %08X",0xFFFFFFFF);
  for (int ich=0; ich<2; ++ich){
    board_index[ich]  =  L0Muon::ErrorHandler(" %01X",0x00F);
    l0EventNumber[ich]=  L0Muon::ErrorHandler(" %03X",0xFFF);
    l0_B_Id[ich]      =  L0Muon::ErrorHandler(" %03X",0xFFF);
  }

  su_bcid  =  L0Muon::ErrorHandler(" %01X",0x00F);
  cu_bcid  =  L0Muon::ErrorHandler(" %01X",0x00F);
  err_bcid =  L0Muon::ErrorHandler(" %01X",0x00F);
  su_bcid_best  =  L0Muon::ErrorHandler(" %01X",0x00F);
  cu_bcid_best  =  L0Muon::ErrorHandler(" %01X",0x00F);
  su_status=  L0Muon::ErrorHandler(" %01X",0x00F);
  for (int ipb=0; ipb<12; ++ipb){
    bcsus_bcid_su[ipb]   =  L0Muon::ErrorHandler("%01X",0x00F);
    bcsus_bcid_cu[ipb]   =  L0Muon::ErrorHandler("%01X",0x00F);
    bcsus_status[ipb]    =  L0Muon::ErrorHandler("%01X",0x00C);
    pb_link_su[ipb]      =  L0Muon::ErrorHandler("%01X",0x003);
    pb_link_cu[ipb]      =  L0Muon::ErrorHandler("%01X",0x003);
  }

}

L0Muon::CtrlRawErrors::~CtrlRawErrors(){}
    
bool L0Muon::CtrlRawErrors::inError()
{

  bool error=false;

  for (int ich=0; ich<2; ++ich){
    error|=board_index[ich].inError();
    error|=l0EventNumber[ich].inError();
    error|=l0_B_Id[ich].inError();
  }

  error|=su_bcid.inError();
  error|=cu_bcid.inError();
  error|=err_bcid.inError();
  error|=su_bcid_best.inError();
  error|=cu_bcid_best.inError();
  for (int ipb=0; ipb<12; ++ipb){
    error|=bcsus_bcid_su[ipb].inError();
    error|=bcsus_bcid_cu[ipb].inError();
  }
//   error|=su_status.inError();
//   for (int ipb=0; ipb<12; ++ipb){
//     error|=bcsus_status[ipb].inError();
//   }

  return error;
}


void L0Muon::CtrlRawErrors::printCounters(std::string &os,std::string tab) const {

  for (int i=0; i<2; ++i){
    board_index[i].printCounter(os,tab+(boost::format("board index frame %1d") % i).str());
    l0EventNumber[i].printCounter(os,tab+(boost::format("L0EventNumber frame %1d") % i).str());
    l0_B_Id[i].printCounter(os,tab+(boost::format("L0_B_Id frame %1d") % i).str());
  }
  
  su_bcid.printCounter(os,tab+"BCID SU");
  cu_bcid.printCounter(os,tab+"BCID CU");
  su_bcid_best.printCounter(os,tab+"BCID SU_best");
  cu_bcid_best.printCounter(os,tab+"BCID CU_best");
  err_bcid.printCounter(os,tab+"BCID CU-SU");
  for (int ib=0; ib<12; ++ib){
    bcsus_bcid_cu[ib].printCounter(os,tab+(boost::format("BCID BCSU PB%2d (CU)") % ib).str());
    bcsus_bcid_su[ib].printCounter(os,tab+(boost::format("BCID BCSU PB%2d (SU)") % ib).str());
  }

//   su_status.printCounter(os,tab+"Status SU");
//   for (int ib=0; ib<12; ++ib){
//     bcsus_status[ib].printCounter(os,tab+(boost::format("Status BCSU PB%2d ") % ib).str());
//     bcsus_status[ib].printCounter(os,tab+(boost::format("Status BCSU PB%2d ") % ib).str());
//   }
  

}
int L0Muon::CtrlRawErrors::sumCounters() const {
  
  int n=0;

  for (int i=0; i<2; ++i){  
    n+=board_index[i].counter();
    n+=l0EventNumber[i].counter();
    n+=l0_B_Id[i].counter();
  }
  
  n+=cu_bcid.counter();
  n+=su_bcid.counter();
  n+=err_bcid.counter();
  n+=cu_bcid_best.counter();
  n+=su_bcid_best.counter();
//   n+=su_status.counter();
  for (int i=0; i<12; ++i){  
//     n+=bcsus_status[i].counter();
    n+=bcsus_bcid_cu[i].counter();
    n+=bcsus_bcid_su[i].counter();
  }
  
  return n;
  
}


std::ostream &L0Muon::operator<<(std::ostream &os, const L0Muon::CtrlRawErrors &pberr) {

  if (pberr.decodingError()) {
    os<<" Data corrupted ";
  } else {
    os<<pberr.board_index[0]  <<pberr.board_index[1];
    os<<pberr.l0EventNumber[0]<<pberr.l0EventNumber[1];
    os<<pberr.l0_B_Id[0]      <<pberr.l0_B_Id[1];
    os<<pberr.su_bcid <<pberr.cu_bcid <<pberr.err_bcid ;
    os<<pberr.cu_bcid_best ;
    os<<" ";
    for (int ipb=0; ipb<12; ++ipb) os<<pberr.bcsus_bcid_cu[ipb];
    os<<pberr.su_bcid_best ;
    os<<" ";
    for (int ipb=0; ipb<12; ++ipb) os<<pberr.bcsus_bcid_su[ipb];
    os<<pberr.su_status;
    os<<" ";
    for (int ipb=0; ipb<12; ++ipb) os<<pberr.bcsus_status[ipb];
  }
  
  return os;
}
