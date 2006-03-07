#include "L0MuonKernel/DecodeRawBufferCtrl.h"
#include "L0MuonKernel/RawBufferCtrlUnit.h"

/**
   Constructor.
*/
L0Muon::DecodeRawBufferCtrl::DecodeRawBufferCtrl(){
};

/**
   Destructor
*/
L0Muon::DecodeRawBufferCtrl::~DecodeRawBufferCtrl(){
}

void  L0Muon::DecodeRawBufferCtrl::setBitset(boost::dynamic_bitset<> bitset){
  m_buffer = bitset;
}

std::vector<L0Muon::MuonCandidate> L0Muon::DecodeRawBufferCtrl::candidatesCTRL()
{
  std::vector<MuonCandidate> candidates;

  // Retrieve total number of selected candidates
  boost::dynamic_bitset<> bncand=(m_buffer>>L0Muon::ShiftRawBufCtrlNCTRLCAND);
  bncand.resize(L0Muon::BitsRawBufCtrlNCTRLCAND);
  int ncand=bncand.to_ulong();

  // Position of the first candidate 
  int shift=L0Muon::ShiftRawBufCtrlFIRSTCAND; 

  // Loop over candidates
  for (int i=0;i<ncand;++i){
    MuonCandidate muoncandidate;
    
    // Candidate info
    boost::dynamic_bitset<> candBitset=m_buffer>>shift;
    candBitset.resize(L0Muon::BitsRawBufCAND);
    unsigned long candidate = candBitset.to_ulong();
   
    // Decode candidate info
    int quarter = (candidate & L0Muon::RelMaskRawBufQUARTER) >> L0Muon::RelShiftRawBufQUARTER;
    int board   = (candidate & L0Muon::RelMaskRawBufBOARD  ) >> L0Muon::RelShiftRawBufBOARD  ;
    int pu      = (candidate & L0Muon::RelMaskRawBufPU     ) >> L0Muon::RelShiftRawBufPU     ;
    int addM3   = (candidate & L0Muon::RelMaskRawBufADDM3  ) >> L0Muon::RelShiftRawBufADDM3  ;
    int offM2   = (candidate & L0Muon::RelMaskRawBufOFFM2  ) >> L0Muon::RelShiftRawBufOFFM2  ;
    int offM1   = (candidate & L0Muon::RelMaskRawBufOFFM1  ) >> L0Muon::RelShiftRawBufOFFM1  ;
    int pt      = (candidate & L0Muon::RelMaskRawBufPT     ) >> L0Muon::RelShiftRawBufPT     ;
    int charge  = (candidate & L0Muon::RelMaskRawBufCHARGE ) >> L0Muon::RelShiftRawBufCHARGE ;

    //     std::cout << "==> Execute cand # " << i <<"\n";
    //     std::cout  << "              quarter = " << quarter <<"\n";
    //     std::cout  << "              board   = " << board   <<"\n";
    //     std::cout  << "              pu      = " << pu      <<"\n";
    //     std::cout  << "              addM3   = " << addM3   <<"\n";
    //     std::cout  << "              offM2   = " << offM2   <<"\n";
    //     std::cout  << "              offM1   = " << offM1   <<"\n";
    //     std::cout  << "              pt      = " << pt      <<"\n";
    //     std::cout  << "              charge  = " << charge  <<"\n";
   
    // Fill the MuonCandidate 
    muoncandidate.setQuarter(quarter);
    muoncandidate.setBoard(board);
    muoncandidate.setPu(pu);
    muoncandidate.setAddM3(addM3);
    muoncandidate.setOffM2(offM2);
    muoncandidate.setOffM1(offM1);
    muoncandidate.setPT(pt);
    muoncandidate.setCharge(charge);

    candidates.push_back(muoncandidate);    
  
    shift+=L0Muon::RelShiftRawBufCAND;

  } // End of Loop over candidates
  
  return candidates;
}

std::vector<L0Muon::MuonCandidate> L0Muon::DecodeRawBufferCtrl::candidatesBCSU()
{
  std::vector<L0Muon::MuonCandidate> candidates;

  // Position of the first candidate 
  int shift=L0Muon::ShiftRawBufCtrlFIRSTCAND; // start from first ctrl candidate

  // Retrieve total number of controler candidates
  boost::dynamic_bitset<> bnctrlcand=(m_buffer>>L0Muon::ShiftRawBufCtrlNCTRLCAND);
  bnctrlcand.resize(L0Muon::BitsRawBufCtrlNCTRLCAND);
  int nCtrlcand=bnctrlcand.to_ulong();
  shift+= nCtrlcand*L0Muon::BitsRawBufCAND;   // skip the  nCtrlcand ctrl cand.

  // Retrieve total number of bcsu candidates
  boost::dynamic_bitset<> bnbcsucand=(m_buffer>>shift);
  bnbcsucand.resize(L0Muon::BitsRawBufCtrlNBCSUCAND);
  int nBcsucand=bnbcsucand.to_ulong();
  shift+= L0Muon::BitsRawBufCtrlNBCSUCAND;    // skip the word specifying the # of bcsu cand.

  // Loop over candidates
  for (int i=0;i<nBcsucand;++i){
    MuonCandidate muoncandidate;
    
    // Candidate info
    boost::dynamic_bitset<> candBitset=m_buffer>>shift;
    candBitset.resize(L0Muon::BitsRawBufCAND);
    unsigned long candidate = candBitset.to_ulong();
   
    // Decode candidate info
    int quarter = (candidate & L0Muon::RelMaskRawBufQUARTER) >> L0Muon::RelShiftRawBufQUARTER;
    int board   = (candidate & L0Muon::RelMaskRawBufBOARD  ) >> L0Muon::RelShiftRawBufBOARD  ;
    int pu      = (candidate & L0Muon::RelMaskRawBufPU     ) >> L0Muon::RelShiftRawBufPU     ;
    int addM3   = (candidate & L0Muon::RelMaskRawBufADDM3  ) >> L0Muon::RelShiftRawBufADDM3  ;
    int offM2   = (candidate & L0Muon::RelMaskRawBufOFFM2  ) >> L0Muon::RelShiftRawBufOFFM2  ;
    int offM1   = (candidate & L0Muon::RelMaskRawBufOFFM1  ) >> L0Muon::RelShiftRawBufOFFM1  ;
    int pt      = (candidate & L0Muon::RelMaskRawBufPT     ) >> L0Muon::RelShiftRawBufPT     ;
    int charge  = (candidate & L0Muon::RelMaskRawBufCHARGE ) >> L0Muon::RelShiftRawBufCHARGE ;

    //     std::cout << "==> Execute cand # " << i <<"\n";
    //     std::cout  << "              quarter = " << quarter <<"\n";
    //     std::cout  << "              board   = " << board   <<"\n";
    //     std::cout  << "              pu      = " << pu      <<"\n";
    //     std::cout  << "              addM3   = " << addM3   <<"\n";
    //     std::cout  << "              offM2   = " << offM2   <<"\n";
    //     std::cout  << "              offM1   = " << offM1   <<"\n";
    //     std::cout  << "              pt      = " << pt      <<"\n";
    //     std::cout  << "              charge  = " << charge  <<"\n";
   
    // Fill the MuonCandidate 
    muoncandidate.setQuarter(quarter);
    muoncandidate.setBoard(board);
    muoncandidate.setPu(pu);
    muoncandidate.setAddM3(addM3);
    muoncandidate.setOffM2(offM2);
    muoncandidate.setOffM1(offM1);
    muoncandidate.setPT(pt);
    muoncandidate.setCharge(charge);

    candidates.push_back(muoncandidate);    
  
    shift+=L0Muon::RelShiftRawBufCAND;

  } // End of Loop over candidates
  
  return candidates;
}

