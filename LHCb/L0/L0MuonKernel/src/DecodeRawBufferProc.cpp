#include "L0MuonKernel/DecodeRawBufferProc.h"
#include "L0MuonKernel/RawBufferProcUnit.h"

/**
   Constructor.
*/
L0Muon::DecodeRawBufferProc::DecodeRawBufferProc(){
};


/**
   Destructor
*/
L0Muon::DecodeRawBufferProc::~DecodeRawBufferProc(){
}

void  L0Muon::DecodeRawBufferProc::setBitset(boost::dynamic_bitset<> bitset, bool compressed){
  m_compressed= compressed;
  m_buffer = bitset;
}

std::vector<L0Muon::MuonCandidate> L0Muon::DecodeRawBufferProc::candidatesBCSU()
{
  std::vector<L0Muon::MuonCandidate> candidates;

  // Retrieve total number of selected candidates
  boost::dynamic_bitset<> bncand=(m_buffer>>L0Muon::ShiftRawBufProcNBCSUCAND);
  bncand.resize(L0Muon::BitsRawBufProcNBCSUCAND);
  int ncand=bncand.to_ulong();

  // Position of the first candidate 
  int shift=L0Muon::ShiftRawBufProcFIRSTCAND; 

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

std::vector<L0Muon::MuonCandidate> L0Muon::DecodeRawBufferProc::candidatesPU()
{
  std::vector<L0Muon::MuonCandidate> candidates;
  
  // Position of the first candidate 
  int shift=L0Muon::ShiftRawBufProcFIRSTCAND; // start from first ctrl candidate

  // Retrieve total number of BCSU candidates
  boost::dynamic_bitset<> bnbcsucand=(m_buffer>>L0Muon::ShiftRawBufProcNBCSUCAND);
  bnbcsucand.resize(L0Muon::BitsRawBufProcNBCSUCAND);
  int nBcsucand=bnbcsucand.to_ulong();
  shift+= nBcsucand*L0Muon::BitsRawBufCAND;   // skip the  nBcsucand bcsu cand.

  // Retrieve total number of PU candidates
  boost::dynamic_bitset<> bnpucand=(m_buffer>>shift);
  bnpucand.resize(L0Muon::BitsRawBufProcNPUCAND);
  int nPucand=bnpucand.to_ulong();
  shift+= L0Muon::BitsRawBufProcNPUCAND;    // skip the word specifying the # of pu cand.

  // Loop over candidates
  for (int i=0;i<nPucand;++i){
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

boost::dynamic_bitset<> L0Muon::DecodeRawBufferProc::nonEmptyPU()
{

  // Position of the first candidate 
  int shift=ShiftRawBufProcFIRSTCAND;

  // Retrieve total number of BCSU candidates
  boost::dynamic_bitset<> bnbcsucand=(m_buffer>>L0Muon::ShiftRawBufProcNBCSUCAND);
  bnbcsucand.resize(L0Muon::BitsRawBufProcNBCSUCAND);
  int nBcsucand=bnbcsucand.to_ulong();
  shift+= nBcsucand*L0Muon::BitsRawBufCAND;   // skip the  nBcsucand bcsu cand.

  // Retrieve total number of PU candidates
  boost::dynamic_bitset<> bnpucand=(m_buffer>>shift);
  bnpucand.resize(L0Muon::BitsRawBufProcNPUCAND);
  int nPucand=bnpucand.to_ulong();
  shift+= L0Muon::BitsRawBufProcNPUCAND;    // skip the word specifying the # of pu cand.
  shift+= nPucand*L0Muon::BitsRawBufCAND;   // skip the  nPucand pu cand.
  
  boost::dynamic_bitset<> code = m_buffer>>shift;
  
  code.resize(L0Muon::BitsRawBufProcPROCID);

  return code;
  
}

boost::dynamic_bitset<> L0Muon::DecodeRawBufferProc::inputDATA(bool expand)
{
  // Position of the first candidate 
  int shift=ShiftRawBufProcFIRSTCAND;

  // Retrieve total number of BCSU candidates
  boost::dynamic_bitset<> bnbcsucand=(m_buffer>>L0Muon::ShiftRawBufProcNBCSUCAND);
  bnbcsucand.resize(L0Muon::BitsRawBufProcNBCSUCAND);
  int nBcsucand=bnbcsucand.to_ulong();
  shift+= nBcsucand*L0Muon::BitsRawBufCAND;   // skip the  nBcsucand bcsu cand.

  // Retrieve total number of PU candidates
  boost::dynamic_bitset<> bnpucand=(m_buffer>>shift);
  bnpucand.resize(L0Muon::BitsRawBufProcNPUCAND);
  int nPucand=bnpucand.to_ulong();
  shift+= L0Muon::BitsRawBufProcNPUCAND;    // skip the word specifying the # of pu cand.
  shift+= nPucand*L0Muon::BitsRawBufCAND;   // skip the  nPucand pu cand.
  shift+= L0Muon::BitsRawBufProcPROCID;     // skip the word specifying the non empty PU

  if (m_compressed & expand) uncompress(shift,m_buffer.size());

  boost::dynamic_bitset<> data = (m_buffer>>shift);
  data.resize(m_buffer.size()-shift);
  
  return data;
}


void  L0Muon::DecodeRawBufferProc::uncompress(int start, int stop){
//   std::cout << "L0Muon::DecodeRawBufferProc::uncompress IN " 
//             <<  " start= "<<start << " stop= "<<stop<<"\n";
  
  if (start==stop) return ;

  m_compressed= false;

  unsigned int totalSize =  m_buffer.size();
//   std::cout << "L0Muon::DecodeRawBufferProc::uncompress " 
//             << " m_buffer.size()= "  << m_buffer.size()
//             << " m_buffer.count()= " << m_buffer.count() <<"\n";
  //   std::cout << "L0Muon::DecodeRawBufferProc::uncompress  " 
  //             <<  "totalSize = "<< totalSize <<"\n";

  boost::dynamic_bitset<> end = m_buffer>>stop;
  //   std::cout << "L0Muon::DecodeRawBufferProc::uncompress   " 
  //             <<  "end.size() = "<< end.size() <<"\n";
  end.resize(totalSize-stop);
  unsigned int endSize =  end.size();
  //   std::cout << "L0Muon::DecodeRawBufferProc::uncompress   " 
  //             <<  " endSize= "<< endSize <<"\n";


  boost::dynamic_bitset<> compressed = (m_buffer>>start);
  compressed.resize(stop-start);
  
  m_buffer.resize(start);
  //   unsigned int compressedSize = compressed.size();
  //   std::cout << "L0Muon::DecodeRawBufferProc::uncompress  " 
  //             <<  "compressedSize = "<< compressedSize <<"\n";

  //   boost::dynamic_bitset<> uncompressed = unapplyRLE(compressed);
  boost::dynamic_bitset<> uncompressed = unapplyOCounting(compressed);
  unsigned int uncompressedSize = uncompressed.size();
//   std::cout << "L0Muon::DecodeRawBufferProc::uncompress  " 
//             <<  " uncompressed.count()= "<<uncompressed.count() 
//             <<  " uncompressedSize= "<< uncompressedSize <<"\n";
  
  uncompressed.resize(uncompressedSize+start,false);
  m_buffer.resize(uncompressedSize+start,false);
  m_buffer|=uncompressed<<start;
//   std::cout << "L0Muon::DecodeRawBufferProc::uncompress  " 
//             <<  " m_buffer.size()= "<<m_buffer.size() 
//             <<  " m_buffer.count()= "<<m_buffer.count() <<"\n";

  m_buffer.resize(uncompressedSize+start+endSize);
  end.resize(uncompressedSize+start+endSize);
  m_buffer|=end<<(start+uncompressedSize);
  
//   std::cout << "L0Muon::DecodeRawBufferProc::uncompress OUT - " 
//             <<  " m_buffer.size() = "<< m_buffer.size()  
//             <<  " m_buffer.count()= "<<m_buffer.count() <<"\n";
}

boost::dynamic_bitset<> L0Muon::DecodeRawBufferProc::unapplyOCounting(boost::dynamic_bitset<> compressed)
{                                                    
//   std::cout << "L0Muon::DecodeRawBufferProc::unapply0Counting IN -" 
//             << " compressedSize= "<< compressed.size()
//             << " # of bits set = "<< compressed.count()<<"\n";
//   std::cout << "L0Muon::DecodeRawBufferProc::unapply0Counting compressed bitset: "
//             << compressed <<"\n";

  boost::dynamic_bitset<> uncompressed;  
  unsigned int uncompressedSize = uncompressed.size();

  int nbitsPrefix  =L0Muon::encodingPrefixSizeRawBuf;
  int maximumLength = int(pow(2,nbitsPrefix));
//   std::cout << "L0Muon::DecodeRawBufferProc::unapply0Counting IN " 
//             <<  "nbitsPrefix = "<< nbitsPrefix<<"\n";

  unsigned int compressedSize = compressed.size();
  if (compressed.count()==0) {
    uncompressed.resize(compressedSize*maximumLength);
    return uncompressed;
  }
//   std::cout << "L0Muon::DecodeRawBufferProc::unapply0Counting  " 
//             << " compressedSize= "<< compressedSize <<"\n";

  boost::dynamic_bitset<>::size_type lastPos=0;
  boost::dynamic_bitset<>::size_type currentPos=compressed.find_first();

  while (currentPos<compressed.size()) {
//     std::cout << "L0Muon::RawBufferUnit::unapply0Counting 0:"<<"currentPos= "<<currentPos<<"\n";

    // Decode a sequence.
    // A sequence is made of 3 parts:
    // - a series of 0 eventually
    // - a 1
    // - a number coded on nbitsPrefix

    // the series of 0 contains n0
    int n0 =  currentPos - lastPos;
//     std::cout << "L0Muon::RawBufferUnit::unapply0Counting n0:"<<"= "<< n0<<"\n";
  
    // the number is
    boost::dynamic_bitset<> number = (compressed>>currentPos+1);
    number.resize(nbitsPrefix);
//     std::cout << "L0Muon::RawBufferUnit::unapply0Counting n0':"<<"= "<<number.to_ulong()<<"\n";

    uncompressedSize+=n0*maximumLength + number.to_ulong();
  
//     std::cout << "L0Muon::RawBufferUnit::unapply0Counting adding: "<< n0*maximumLength + number.to_ulong()<<" 0\n";
//     std::cout << "L0Muon::RawBufferUnit::unapply0Counting uncompressedSize:"<<"= "<<uncompressedSize<<"\n"; 
    uncompressed.resize(uncompressedSize);
    uncompressed.push_back(1);
  
    currentPos+=nbitsPrefix+1;
    lastPos= currentPos;
//     std::cout << "L0Muon::RawBufferUnit::unapply0Counting :"
//               <<" compressed.size()"<<"= "<<compressed.size()
//               <<" currentPos"<<"= "<<currentPos
//               <<"\n";
    currentPos=compressed.find_next(lastPos-1);   

  }

  // Remove the ending 1
  uncompressed.resize(uncompressedSize);
//   std::cout << "L0Muon::DecodeRawBufferProc::unapply0Counting OUT - " 
//             <<  " uncompressed.size()= "<< uncompressed.size()
//             <<  " uncompressed.count()= "<< uncompressed.count() <<"\n";

  return uncompressed;
}


boost::dynamic_bitset<> L0Muon::DecodeRawBufferProc::unapplyRLE(boost::dynamic_bitset<> compressed)
{
  int nbitsPrefix  =L0Muon::encodingPrefixSizeRawBuf;
  int nbitsSequence=nbitsPrefix+1;
  //   std::cout << "L0Muon::DecodeRawBufferProc::unapplyRLE IN " 
  //             <<  "nbitsPrefix = "<< nbitsPrefix<< " nbitsSequence= "<<nbitsSequence<<"\n";

  boost::dynamic_bitset<> uncompressed;
  unsigned int compressedSize = compressed.size();
  //   std::cout << "L0Muon::DecodeRawBufferProc::unapplyRLE  " 
  //             <<  " compressedSize= "<< compressedSize <<"\n"; int  icount=0;


  for (boost::dynamic_bitset<>::size_type cursor=0; cursor<compressedSize;cursor+=nbitsSequence){

    //     std::cout << "L0Muon::DecodeRawBufferProc::unapplyRLE   ." 
    //               <<  ".cursor= "<<cursor  <<"\n";
  
    //     std::cout << "L0Muon::DecodeRawBufferProc::unapplyRLE   ." 
    //               <<  " icount= "<< icount++ <<"\n";
    boost::dynamic_bitset<> segment =  compressed>>cursor;
    bool bit= segment.test(nbitsPrefix);
    //     std::cout << "L0Muon::DecodeRawBufferProc::unapplyRLE    " 
    //               <<  " bit = "<< bit <<"\n";
    segment.resize(nbitsPrefix);    
    //     std::cout << "L0Muon::DecodeRawBufferProc::unapplyRLE    " 
    //               <<  " segment= "<< segment <<"\n";
    unsigned long nchar = segment.to_ulong() ;
    //     std::cout << "L0Muon::DecodeRawBufferProc::unapplyRLE    " 
    //               <<  " nchar= "<< nchar+1 <<"\n";
  
    for (unsigned int i=0; i<(nchar+1) ; i++) {
      uncompressed.push_back(bit);
    }

    //     std::cout << "L0Muon::DecodeRawBufferProc::unapplyRLE    " 
    //               <<  " uncompressed.size()= "<< uncompressed.size() <<"\n";
  
  }
  //   std::cout << "L0Muon::DecodeRawBufferProc::unapplyRLE OUT " 
  //             <<  " uncompressed.size()= "<< uncompressed.size()
  //             <<  " uncompressed.count()= "<< uncompressed.count() <<"\n";

  return uncompressed;
}
