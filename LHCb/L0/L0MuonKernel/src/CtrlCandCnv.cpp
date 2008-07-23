#include "L0MuonKernel/CtrlCandCnv.h"

#include "ProcessorKernel/RegisterFactory.h"
#include "ProcessorKernel/Register.h"

#include "L0MuonKernel/BankUtilities.h"

#define _DET_SPEC_HEADER_ 1

/**
   Constructor.
*/
L0Muon::CtrlCandCnv::CtrlCandCnv(){
  m_side=0;
};
/**
   Constructor.
*/
L0Muon::CtrlCandCnv::CtrlCandCnv(int side){
  m_side=side;

  char buf[4096];
  char* format ;
  
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

  for (int iq = 0; iq<2 ; ++iq){
    format = "CAND_CTRLQ%d";
    sprintf(buf,format,m_side*2+iq+1);
    L0Muon::Register* reg = rfactory->createRegister(buf,CandRegisterHandler::size());
    m_candRegHandler[iq] = CandRegisterHandler(reg) ;
    for (int iboard = 0; iboard <12 ; iboard++) {
      format = "BOARDCAND_Q%d_%d";
      sprintf(buf,format,m_side*2+iq+1,iboard);
      L0Muon::Register* reg = rfactory->createRegister(buf,CandRegisterHandler::size());
      m_candRegHandlerBCSU[iq][iboard] = CandRegisterHandler(reg) ;
    }
  }
};


/**
   Destructor
*/
L0Muon::CtrlCandCnv::~CtrlCandCnv(){
}

LHCb::MuonTileID L0Muon::CtrlCandCnv::mid_BCSU(int iq, int ib){
  MuonLayout lay(1,1);
  LHCb::MuonTileID board = LHCb::MuonTileID(0);
  board.setQuarter(m_side*2+iq);
  board.setLayout(lay);
  board.setRegion(ib/3);
  board.setX( (((ib%3)+1)>>0)&1);
  board.setY( (((ib%3)+1)>>1)&1);
  return board;
}

void L0Muon::CtrlCandCnv::release(){
  for (int iq = 0; iq<2 ; ++iq) {
    m_candRegHandler[iq].clear();
    for (int ib = 0; ib<12 ; ++ib) 
      m_candRegHandlerBCSU[iq][ib].clear();
  }
}

void L0Muon::CtrlCandCnv::clearRef(){
  m_ref_l0_B_Id=-1;
  m_ref_l0EventNumber=-1;
}

std::vector<L0Muon::PMuonCandidate>  L0Muon::CtrlCandCnv::muonCandidates(){
  std::vector<L0Muon::PMuonCandidate> cands;
  for (int iq = 0; iq<2 ; ++iq){
    int ncand= m_candRegHandler[iq].numberOfCandidates();
    for (int icand = 0;icand<ncand;icand++) {
      cands.push_back(m_candRegHandler[iq].getMuonCandidate(icand));
    }    
  }
  
  return cands;
}
std::vector<L0Muon::PMuonCandidate>  L0Muon::CtrlCandCnv::muonCandidates(int iq){
  std::vector<L0Muon::PMuonCandidate> cands;
  int ncand= m_candRegHandler[iq].numberOfCandidates();
  for (int icand = 0;icand<ncand;icand++) {
    cands.push_back(m_candRegHandler[iq].getMuonCandidate(icand));
  }
  
  return cands;
}

std::vector<L0Muon::PMuonCandidate>  L0Muon::CtrlCandCnv::muonCandidatesBCSU(){
  std::vector<L0Muon::PMuonCandidate> cands;
  for (int iq = 0; iq<2 ; ++iq){
    for (int ib= 0; ib<12 ;++ib) {
      int ncand= m_candRegHandlerBCSU[iq][ib].numberOfCandidates();
      for (int icand = 0;icand<ncand;icand++) {
        cands.push_back(m_candRegHandlerBCSU[iq][ib].getMuonCandidate(icand));
      }    
    }
  }
  
  return cands;
}
std::vector<L0Muon::PMuonCandidate>  L0Muon::CtrlCandCnv::muonCandidatesBCSU(int iq){
  std::vector<L0Muon::PMuonCandidate> cands;
  for (int ib= 0; ib<12 ;++ib) {
    int ncand= m_candRegHandlerBCSU[iq][ib].numberOfCandidates();
    for (int icand = 0;icand<ncand;icand++) {
      cands.push_back(m_candRegHandlerBCSU[iq][ib].getMuonCandidate(icand));
    }    
  }
  
  return cands;
}

std::vector<L0Muon::PMuonCandidate>  L0Muon::CtrlCandCnv::muonCandidatesBCSU(int iq, int ib ){
  std::vector<L0Muon::PMuonCandidate> cands;
  int ncand= m_candRegHandlerBCSU[iq][ib].numberOfCandidates();
  for (int icand = 0;icand<ncand;icand++) {
    cands.push_back(m_candRegHandlerBCSU[iq][ib].getMuonCandidate(icand));
  }    
  
  return cands;
}

int L0Muon::CtrlCandCnv::decodeBank(const std::vector<unsigned int> &raw, int bankVersion, int mode)
{

  // Decode the L0MuonCtrlCand bank version 2 
  //
  // This banks contains 2 parts :
  // o 1st part : final candidates selected by the controller boards of 1 side (2 quarters)
  // o 2nd part : input candidates received by the controller boards of 1 side (candidates sent by the processing boards)
  //
  // The 2nd part of the bank (if present) will be decoded only if the given input mode is >0.
  //
  // Return values :
  //  o  0 : incomplete bank
  //  o -1 : bad format
  //  o -2 : bad format
  //  o  1 : only 1st first part was decoded
  //  o  2 : the two parts were decoded

  if (bankVersion<2) return 0;

  bool decodingError=false;
  
  // Clear the registers first
  release();

//   std::cout.setf(std::ios::uppercase) ;
//   std::cout<<"\t=> L0Muon::CtrlCandCnv::decodeBank -- dump raw bank of size: "<<raw.size()<<std::hex<<std::endl;
//   for (std::vector<unsigned int>::const_iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
//     std::cout <<"\t0x"<<std::setw(8)<<(*itraw)<<std::endl;
//   }
//   std::cout<<std::dec;
//   std::cout.unsetf(std::ios::uppercase);

#if _DET_SPEC_HEADER_==1
  int header = raw[0];
  unsigned int nPart1words = (header & 0xFFFF)/2; // in words of 32 bytes
  unsigned int nPart2words = raw.size()-nPart1words-1; // in words of 32 bytes
  unsigned int iwd = 1;
#else
  unsigned int iwd=0;
  //   unsigned int nPart1words =  9;
  unsigned int nPart2words = 62;
#endif

  // First part of the bank
  for (int iq = 0; iq <2 ; iq++) { // Loop over quarters (1st part)
    m_errors[iq].present.set(1);
    unsigned int empty=0;
    
    // first line : Event counters
    if (iwd>=raw.size()) return 0;
    int l0EventNumber = ( (raw[iwd]>> 0)& 0xFFF );
    int l0_B_Id       = ( (raw[iwd]>>16)& 0xFFF );
    submitL0EventNumber(l0EventNumber);
    submitL0_B_Id(l0_B_Id);
    m_errors[iq].l0EventNumber.set(l0EventNumber,m_ref_l0EventNumber);
    m_errors[iq].l0_B_Id.set(l0_B_Id,m_ref_l0_B_Id);
    m_errors[iq].errK.set( (raw[iwd]>>12)& 0x1 );
    m_errors[iq].errJ.set( (raw[iwd]>>28)& 0x1 );
    empty|=((raw[iwd])& 0xE000E000);
    ++iwd;
    
    // second line : BCIDs & Status
    if (iwd>=raw.size()) return 0;
    int status=((raw[iwd]>>0)& 0xF);
    int bcid  =((raw[iwd]>>4)& 0xF);
    int bcidCU  =((raw[iwd]>>16)& 0xF);
    int bcidSU  =((raw[iwd]>>20)& 0xF);
    bool compression =  ((raw[iwd]>>31)& 0x1);
    if (status>2) m_errors[iq].best_status.set(status);
    m_errors[iq].best_bcid.set(bcid,m_ref_l0_B_Id);
    m_errors[iq].ctrl_bcid[0].set(bcidCU,m_ref_l0_B_Id);
    m_errors[iq].ctrl_bcid[1].set(bcidSU,m_ref_l0_B_Id);
    empty|=((raw[iwd])& 0x7F007F00);
    m_candRegHandler[iq].setStatus(status);    
    ++iwd;

    // Next lines : candidates
    int ncand=status&0x3;
    ncand = ncand>2 ? 2 : ncand;
    if (!compression) ncand = 2;
    for (int icand=0; icand<ncand; ++icand){
      if (iwd>=raw.size()) return 0;
      L0Muon::writeCandInRegister(&m_candRegHandler[iq],raw[iwd],icand,bankVersion);
      empty|=((raw[iwd])& 0x00008000);
      ++iwd;
    }
    m_errors[iq].decoding.set(empty);
    decodingError=(empty>0);
    
  } // End of loop over quarters (1st part)

  // Do not try to decode the 2nd part of the bank
  if ( (mode<=0) || (nPart2words==0) ) {
    if (decodingError) return -1;
    return 1;
  }
  

  // Second part of the bank (if present)
  for (int iq = 0; iq <2 ; iq++) { // Loop over quarters (2nd part)
    unsigned int empty=m_errors[iq].decoding.value();

    int iboard;

    // Errors words (3 words)
    iboard=0;
    for (int i=0; i<3; ++i){ // Loop over the 3 words with the link errors
      if (iwd>=raw.size()) return 0;
      for (int ii=0; ii<4; ++ii) { // Loop over the 4 boards in the current word
        m_errors[iq].link[iboard][1].set( (raw[iwd]>> (28-(ii*4)))& 0x3 );
        m_errors[iq].link[iboard][0].set( (raw[iwd]>> (12-(ii*4)))& 0x3 );
        ++iboard;
      } // End of loop over the 4 boards in the current word
      empty|=((raw[iwd])& 0xCCCCCCCC);
      ++iwd;
    } // End of loop over the 3 wordss with the link errors

    // Status (2 words) (& number of candidates to decode)
    bool compression =  ((raw[iwd]>>31)& 0x1);
    int bcsu_ncand[12];
    iboard=0;
    for (int i=0; i<2; ++i){ // Loop over the 2 words with the status
      if (iwd>=raw.size()) return 0;
      for (int ii=0; ii<6; ++ii) { // Loop over the 4 boards in the current word
        int status=( (raw[iwd]>> (20-(ii*4)))& 0xF );
        int ncand=status&0x3;
        ncand= ncand>2 ? 2 : ncand;
        if (!compression) ncand=2;
        bcsu_ncand[iboard]=ncand;
        m_candRegHandlerBCSU[iq][iboard].setStatus( status );    
        if (status>2) m_errors[iq].bcsu_status[iboard].set(status);
        ++iboard;
      } // End of loop over the 4 boards in the current word
      empty|=((raw[iwd])& 0x7F000000);
      ++iwd;
    } // End of loop over the 2 words with the status
      
    // BCIDs (2 words)
    iboard=0;
    for (int i=0; i<2; ++i){ // Loop over the 2 words with the bcids
      if (iwd>=raw.size()) return 0;
      for (int ii=0; ii<6; ++ii) { // Loop over the 4 boards in the current word
        m_errors[iq].bcsu_bcid[iboard].set( (raw[iwd]>> (20-(ii*4)))& 0xF,m_ref_l0_B_Id);
        m_errors[iq].bcsu_bcid_mismatch[iboard].set( (raw[iwd]>> (29-(ii)))& 0x1);
        ++iboard;
      } // End of loop over the 4 boards in the current word
      empty|=((raw[iwd])& 0xE0000000);
      ++iwd;
    } // End of loop over the 2 words with the bcids

      // Candidates
    for (int ib=0; ib<12; ++ib){ // Loop over the candidates of the 12 processing boards
      for (int icand=0; icand<bcsu_ncand[ib]; ++icand){
        if (iwd>=raw.size()) return 0;
        L0Muon::writeCandInRegister(&m_candRegHandlerBCSU[iq][ib],raw[iwd],icand,bankVersion);
        empty|=((raw[iwd])& 0x00008000);
        ++iwd;
      }
    } // End of loop over the candidates of the 12 processing boards
    m_errors[iq].decoding.set(empty);
    decodingError|=(empty>0);
    
  } // End of loop over quarters (2nd part)
    
  if (decodingError) return -2;
  return 2;
  
}

int L0Muon::CtrlCandCnv::rawBank(std::vector<unsigned int> &raw, int ievt, int bankVersion, int mode, bool compression){
  raw.clear();
  if (bankVersion<2) return 0;
  if (ievt<0) return 0;

  int event_number = ievt;
  int l0_bid = ievt;
  int bcid   = l0_bid&0xF;
  
  // -- 1st part

  // Loop over controller boards
  for (int iq = 0; iq <2 ; iq++) {
    unsigned int word;
    
    // Event numbers
    word = ( (l0_bid<<16)&0xFFF0000 )+ ( event_number&0xFFF );
    raw.push_back(word);

    // Status & BCIDs
    word = m_candRegHandler[iq].getStatus();   
    word|=((bcid<<4)&0xF0);
    word|=((bcid<<16)&0x0F0000);
    word|=((bcid<<20)&0xF00000);
    if (compression) word|=0x80000000;
    raw.push_back(word);

    // Candidates
    int ncand=2;
    if (compression) {
      ncand=m_candRegHandler[iq].getStatus()&0x3;
      if (ncand>2) ncand=2;
    }
    for (int icand=0; icand<ncand; icand++){
      word = L0Muon::readCandFromRegister(&m_candRegHandler[iq], icand, bankVersion);
      raw.push_back(word);
    } 
    
  }

  unsigned int header=raw.size()*2;
  raw.insert(raw.begin(),header);
  
  // 1st part of the bank is done - 
  if (mode<2) return 1;
  
  // -- 2nd part

  // Loop over controller boards
  for (int iq = 0; iq <2 ; iq++) {
    unsigned int word=0;
    for (int i=0; i<3; ++i) raw.push_back(word); // error fields (empty in MC)
    
    // Status processing boards
    for (int i=0; i<2; ++i) {
      word=0;
      for (int ii=0;ii<6;++ii){
        int ib=ii+(i*6);
        int shift=ii*4;
        word|=(m_candRegHandlerBCSU[iq][ib].getStatus()<<shift);
      }
      if (i==0 && compression)  word|=0x80000000;
      raw.push_back(word);
    }

    // BCIDS processing boards
    for (int i=0; i<2; ++i) {
      word=0;
      for (int ii=0;ii<6;++ii){
        int shift=ii*4;
        word|=(bcid<<shift);
      }
      raw.push_back(word);
    }
    
    // Candidates
    for (int ib = 0; ib <12 ; ib++) {
      int ncand=2;
      if (compression) {
        ncand=m_candRegHandlerBCSU[iq][ib].getStatus()&0x3;
        if (ncand>2) ncand=2;
      }
      for (int icand=0; icand<ncand; icand++){
        word = L0Muon::readCandFromRegister(&m_candRegHandlerBCSU[iq][ib], icand, bankVersion);
        raw.push_back(word);
      } 
    }
  }

  return 2;
  
}

int L0Muon::CtrlCandCnv::decodeBankFinalCandidates(const std::vector<unsigned int> &raw, int bankVersion){
  if (bankVersion!=1) return 0;
  
  int iwd = 0;
  for (int iq=0; iq<2; ++iq) {
    int status       = (raw[iwd+2]    )&  0xF; // raw[2],raw[ncand(iq)+2],  
    int ncand = status&0x3;
    ncand = ncand>2 ? 2 : ncand;
    for (int icand=0; icand<ncand; ++icand){
      L0Muon::writeCandInRegister(&m_candRegHandler[iq],raw[iwd+3+icand],icand,bankVersion);  //raw[3], 
      m_candRegHandler[iq].setCandQuarter(m_side*2+iq,icand); 
    }
    m_candRegHandler[iq].setStatus(status);
    iwd+=2+ncand;
  }
  return 1;
}

int L0Muon::CtrlCandCnv::decodeBankBcsuCandidates(const std::vector<unsigned int> &raw, int bankVersion){
  if (bankVersion!=1) return 0;

  // Loop over controller boards
  for (int iq=0; iq<2; ++iq) {

    int ncand[12]={0,0,0,0,0,0,0,0,0,0,0,0};
    int iwd = 0;
    int cursor = 0;

    // 1st loop over processing boards
    for (int ib = 0; ib <12 ; ++ib) {
      int n=(raw[iwd]>>cursor)& 0x3;
      n = n>2 ? 2 : n;
      ncand[ib] = n;
      cursor+=4;
      if (cursor==32) { // next word
        cursor=0;
        iwd=1;
      }
      
    } // End of 1st loop over processing boards

    // 2nd loop over processing boards
    for (int ib = 0; ib <12 ; ++ib) {
      // Loop over the candidates of the processing board
      for (int icand =0;icand<ncand[ib];++icand){
        L0Muon::writeCandInRegister(&m_candRegHandlerBCSU[iq][ib],raw[iwd+3+icand],icand,bankVersion);  
        m_candRegHandlerBCSU[iq][ib].setCandQuarter(m_side*2+iq,icand); 
        m_candRegHandlerBCSU[iq][ib].setCandBoard(ib,icand); 
      } // End of loop over the candidates of the processing board
    } // End of 2nd loop over processing boards

  } // End of loop over controller boards

  return 1;

}

int L0Muon::CtrlCandCnv::rawBankFinalCandidates(std::vector<unsigned int> &raw, int ievt){
  static int bankVersion =  1;

  unsigned int header = 0;
  raw.push_back(header); // raw[0]

  int event_number = ievt;
  int l0_bid = ievt;

  for (int iq=0; iq<2; ++iq) {

    unsigned int word;

    word = ((l0_bid)&0xFFF)<<16 + (event_number)&0xFFF;
    raw.push_back(word);// raw[1] & raw[...]
    word = (m_candRegHandler[iq].getStatus())&0xF;
    raw.push_back(word);// raw[2] & raw[...+2]
    for (int icand=0; icand<2; icand++){
      if (m_candRegHandler[iq].isEmpty(icand)) break;
      word = L0Muon::readCandFromRegister(&m_candRegHandler[iq], icand, bankVersion);
      raw.push_back(word&0x3FFFFFFF);// raw[3(,4)] & raw[...+3] // mask the quarter
    } 
  }
  return 1;

}

int L0Muon::CtrlCandCnv::rawBankBcsuCandidates(std::vector<unsigned int> &raw){
  static int bankVersion =  1;

  // Loop over controller boards
  for (int iq=0; iq<2; ++iq) {

    unsigned int word = 0;
    int cursor = 0;
    // 1st loop over processing boards
    for (int ib = 0; ib <12 ; ++ib) {
      word |= ( ((m_candRegHandlerBCSU[iq][ib].getStatus())&0xF)<<cursor );
      cursor+=4;
      if (cursor==32) { // next word
        cursor=0;
        raw.push_back(word);
      }  
    } // End of 1st loop over processing boards
    raw.push_back(word);
    // 2nd loop over processing boards
    for (int ib = 0; ib <12 ; ++ib) {
      // Loop over the candidates of the processing board
      for (int icand=0; icand<2; icand++){
        if (m_candRegHandlerBCSU[iq][ib].isEmpty(icand)) break;
        word = L0Muon::readCandFromRegister(&m_candRegHandlerBCSU[iq][ib], icand, bankVersion);
        raw.push_back(word&0x30FFFFFF); // mask the quarter and the board
      } // End of loop over the candidates of the processing board
    } // End of 2nd loop over processing boards

  } // End of loop over controller boards

  return 1;
  
}


int L0Muon::CtrlCandCnv::decodeBankDC06(const std::vector<unsigned int> &raw, int bankVersion){
  if (bankVersion!=0) return 0;
  

  static int size    = 31;
  static int max     = 32;
  
  int qcand[2] = {0,0};

  int status =( (raw[1]>>4)& 0xF);

  int ncandidates = (status& 0xF);
  int start = 8;
  int iwd   = 1;
  unsigned int data  = 0;
  int end   = 0;
  int filled= 0;
  int icand=0;
  while(icand<ncandidates) {
    data+= ( (raw[iwd]>>start) << filled ) &0x7FFFFFFF;
    end = start+size-filled;
    if (end>=max) {
      end = end-max;
      filled=max-start;
      start = 0 ;
      ++iwd;
    } else {
      int q=( data>> 29 )& ( (1<<2 ) -1 );
      if (q/2==m_side) {
        int ir = q-2*m_side;
        L0Muon::writeCandInRegister(&m_candRegHandler[ir],data,qcand[ir],bankVersion);
        qcand[ir]++;
      }
      ++icand;
      filled= 0;
      data  = 0;
      start = end;
      end   = 0;
    }
  }

  for (int ir=0;ir<2;++ir) m_candRegHandler[ir].setStatus(qcand[ir]);

  return 1;
  
}

int L0Muon::CtrlCandCnv::rawBankDC06(std::vector<unsigned int> &raw){
  static int bankVersion =  0;

  for (int i=0; i<2; ++i) {
    for (int icand=0; icand<2; icand++){
      if (m_candRegHandler[i].isEmpty(icand)) break;
      unsigned int word = L0Muon::readCandFromRegister(&m_candRegHandler[i], icand, bankVersion);
      raw.push_back(word);
    } 
  }
  
  return 1;

}

