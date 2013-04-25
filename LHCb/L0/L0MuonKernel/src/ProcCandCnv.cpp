#include "L0MuonKernel/ProcCandCnv.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "ProcessorKernel/Register.h"
#include "L0MuonKernel/BankUtilities.h"

#define _DEBUG_PROCCAND_  0
/**
   Constructor.
*/
L0Muon::ProcCandCnv::ProcCandCnv(){
  m_quarter=0;
  this->clearRef();
}
/**
   Constructor.
*/
L0Muon::ProcCandCnv::ProcCandCnv(int quarter){
  m_quarter=quarter;
  this->clearRef();

  char buf[4096];
  
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

  for (int iboard = 0; iboard <12 ; iboard++) {
    // Candidate Register handlers for BCSU candidates
    sprintf(buf,"BOARDCAND_Q%d_%d",m_quarter+1,iboard);
    //    L0Muon::Register* reg_board = rfactory->createRegister(buf,L0Muon::BitsCandRegTot);
    L0Muon::Register* reg_board = rfactory->createRegister(buf,CandRegisterHandler::size());
    m_candRegHandlerBCSU[iboard] = CandRegisterHandler(reg_board) ;
    for (int ipu = 0; ipu <4 ; ipu++) {
      // Candidate Register handlers for PU candidates
      sprintf(buf,"PUCAND_Q%d_%d_%d",m_quarter+1,iboard,ipu);
      //       L0Muon::Register* reg_pu = rfactory->createRegister(buf,L0Muon::BitsCandRegTot);
      L0Muon::Register* reg_pu = rfactory->createRegister(buf,CandRegisterHandler::size());
      m_candRegHandlerPU[iboard][ipu] = CandRegisterHandler(reg_pu) ;
    }
  }
  
}


/**
   Destructor
*/
L0Muon::ProcCandCnv::~ProcCandCnv(){
}

LHCb::MuonTileID L0Muon::ProcCandCnv::mid_BCSU(int ib){
  MuonLayout lay(1,1);
  LHCb::MuonTileID board = LHCb::MuonTileID(0);
  board.setQuarter(m_quarter);
  board.setLayout(lay);
  board.setRegion(ib/3);
  board.setX( (((ib%3)+1)>>0)&1);
  board.setY( (((ib%3)+1)>>1)&1);
  return board;
}

LHCb::MuonTileID L0Muon::ProcCandCnv::mid_PU(int ib, int ipu){
  LHCb::MuonTileID board = mid_BCSU(ib);
  MuonLayout lay(2,2);
  LHCb::MuonTileID pu = LHCb::MuonTileID(board,lay,(ipu>>0)&1,(ipu>>1)&1);
  return pu;
}

void L0Muon::ProcCandCnv::setDecodingMode(){
  // In decoding mode (when filling the BCSU registers from the raw banks),
  // do not use the registers pointed at in the constructor because it already 
  // used by the controller board. Create new registers instead.

  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

  char buf[4096];
  for (int iboard = 0; iboard <12 ; iboard++) {
    sprintf(buf,"BOARDCAND_Q%d_%d_proc",m_quarter+1,iboard);
    L0Muon::Register* reg_board = rfactory->createRegister(buf,CandRegisterHandler::size());
    m_candRegHandlerBCSU[iboard] = CandRegisterHandler(reg_board) ;
  }
}

void L0Muon::ProcCandCnv::release(){

  for (int ib = 0; ib <12 ; ib++) {
    m_candRegHandlerBCSU[ib].clear();
    for (int ipu = 0; ipu <4 ; ipu++) {
      m_candRegHandlerPU[ib][ipu].clear();
    }
  }
}

void L0Muon::ProcCandCnv::clearRef(){
  m_ref_l0_B_Id=-1;
  m_ref_l0EventNumber=-1;
}

std::vector<L0Muon::PMuonCandidate>  L0Muon::ProcCandCnv::muonCandidatesPU(){
  std::vector<L0Muon::PMuonCandidate> cands;
  for (int ib = 0; ib<12 ; ++ib){
    for (int ipu= 0; ipu<4 ;++ipu) {
      int ncand= m_candRegHandlerPU[ib][ipu].numberOfCandidates();
      for (int icand = 0;icand<ncand;icand++) {
        m_candRegHandlerPU[ib][ipu].setCandQuarter(m_quarter,icand); 
        m_candRegHandlerPU[ib][ipu].setCandBoard(ib,icand); 
        m_candRegHandlerPU[ib][ipu].setCandPU(ipu,icand);
        cands.push_back(m_candRegHandlerPU[ib][ipu].getMuonCandidate(icand));
      }    
    }
  }
  
  return cands;
}
std::vector<L0Muon::PMuonCandidate>  L0Muon::ProcCandCnv::muonCandidatesPU(int ib){
  std::vector<L0Muon::PMuonCandidate> cands;
  for (int ipu= 0; ipu<4 ;++ipu) {
    int ncand= m_candRegHandlerPU[ib][ipu].numberOfCandidates();
    for (int icand = 0;icand<ncand;icand++) {
      m_candRegHandlerPU[ib][ipu].setCandQuarter(m_quarter,icand); 
      m_candRegHandlerPU[ib][ipu].setCandBoard(ib,icand); 
      m_candRegHandlerPU[ib][ipu].setCandPU(ipu,icand);
      cands.push_back(m_candRegHandlerPU[ib][ipu].getMuonCandidate(icand));
    }    
  }
  
  return cands;
}
std::vector<L0Muon::PMuonCandidate>  L0Muon::ProcCandCnv::muonCandidatesPU(int ib, int ipu){
  std::vector<L0Muon::PMuonCandidate> cands;
  int ncand= m_candRegHandlerPU[ib][ipu].numberOfCandidates();
  for (int icand = 0;icand<ncand;icand++) {
    m_candRegHandlerPU[ib][ipu].setCandQuarter(m_quarter,icand); 
    m_candRegHandlerPU[ib][ipu].setCandBoard(ib,icand); 
    m_candRegHandlerPU[ib][ipu].setCandPU(ipu,icand);
    cands.push_back(m_candRegHandlerPU[ib][ipu].getMuonCandidate(icand));
  }
  
  return cands;
}

std::vector<L0Muon::PMuonCandidate>  L0Muon::ProcCandCnv::muonCandidatesBCSU(){
  std::vector<L0Muon::PMuonCandidate> cands;
  for (int ib = 0; ib<12 ; ++ib){
    int ncand= m_candRegHandlerBCSU[ib].numberOfCandidates();
    for (int icand = 0;icand<ncand;icand++) {
      m_candRegHandlerBCSU[ib].setCandQuarter(m_quarter,icand); 
      m_candRegHandlerBCSU[ib].setCandBoard(ib,icand); 
      cands.push_back(m_candRegHandlerBCSU[ib].getMuonCandidate(icand));
    }    
  }
  
  return cands;
}

std::vector<L0Muon::PMuonCandidate>  L0Muon::ProcCandCnv::muonCandidatesBCSU(int ib){
  std::vector<L0Muon::PMuonCandidate> cands;
  int ncand= m_candRegHandlerBCSU[ib].numberOfCandidates();
  for (int icand = 0;icand<ncand;icand++) {
    m_candRegHandlerBCSU[ib].setCandQuarter(m_quarter,icand); 
    m_candRegHandlerBCSU[ib].setCandBoard(ib,icand); 
    cands.push_back(m_candRegHandlerBCSU[ib].getMuonCandidate(icand));
  }    
  
  return cands;
}

int L0Muon::ProcCandCnv::decodeBank(const std::vector<unsigned int> &raw, int version)
{

  switch (version){
  case 1:
    return decodeBank_v1(raw);
    break;
  default:
    return decodeBank_v2(raw);
    break;
  };
  
  return 0;

}

int L0Muon::ProcCandCnv::decodeBank_v1(const std::vector<unsigned int> &raw)
{
  static int bankVersion=1;
  
  // Clear the registers first
  release();

  int iwd = 1; // first ligne contains the event numbers, skip it
  for (int ib = 0; ib <12 ; ib++) {
    int ncands=0;
    
    int ncandBCSU = ((raw[iwd])& 0x3);
    // If at least one candidate in this board
    if (ncandBCSU>0) {
      ncandBCSU = ncandBCSU>2 ? 2 : ncandBCSU;

      for (int icand =0;icand<ncandBCSU;++icand){
        L0Muon::writeCandInRegister(&m_candRegHandlerBCSU[ib],raw[iwd+1+ncands],icand,bankVersion);  
        m_candRegHandlerBCSU[ib].setCandQuarter(m_quarter,icand); 
        m_candRegHandlerBCSU[ib].setCandBoard(ib,icand); 
        //m_candRegHandlerBCSU[ib].dump(-1," "); 
        ++ncands;
      } 

      for (int ipu= 0; ipu<4 ;++ipu) {
        int ncandPU =( (raw[iwd]>>(4+ipu*4)) &  0x3) ;
        ncandPU = ncandPU>2 ? 2 : ncandPU;
        for (int icand =0;icand<ncandPU;++icand){
          L0Muon::writeCandInRegister(&m_candRegHandlerPU[ib][ipu],raw[iwd+1+ncands],icand,bankVersion);
          m_candRegHandlerPU[ib][ipu].setCandQuarter(m_quarter,icand); 
          m_candRegHandlerPU[ib][ipu].setCandBoard(ib,icand); 
          ++ncands;
        } 
      }
    } // End if at least one candidate in this board
  
    iwd+=ncands+2; // 1 ligne for the event number, 1 for the status and ncands for the candidates
  }
  return 1;
}

int L0Muon::ProcCandCnv::decodeBank_v2(const std::vector<unsigned int> &raw)
{
  static int bankVersion=2;

  bool decodingError=false;
  
  // Clear the registers first
  release();

#if _DEBUG_PROCCAND_ >0
  std::cout.setf(std::ios::uppercase) ;
  std::cout<<"\t=> L0Muon::ProcCandCnv::decodeBank v2-- dump raw bank of size: "<<raw.size()<<std::hex<<std::endl;
  for (std::vector<unsigned int>::const_iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
    std::cout <<"\t0x"<<std::setw(8)<<(*itraw)<<std::endl;
  }
  std::cout<<std::dec;
  std::cout.unsetf(std::ios::uppercase);
#endif
  

  unsigned int iwd = 0; 
  for (int ib = 0; ib <12 ; ib++) { // Loop over processing boards
    m_errors[ib].present.set(1);
    unsigned int empty=0;
    
    // first line : Event counters
    if (iwd>=raw.size()) return 0;
    int l0EventNumber = ( (raw[iwd]>> 0)& 0xFFF );
    int l0_B_Id       = ( (raw[iwd]>>16)& 0xFFF );
    submitL0EventNumber(l0EventNumber);
    submitL0_B_Id(l0_B_Id);
    m_errors[ib].l0EventNumber.set(l0EventNumber,m_ref_l0EventNumber);
    m_errors[ib].l0_B_Id.set(l0_B_Id,m_ref_l0_B_Id);
    m_errors[ib].errK.set((raw[iwd]>>12)& 0x1);
    m_errors[ib].errJ.set((raw[iwd]>>28)& 0x1);
    empty|=((raw[iwd])& 0xE000E000);
    ++iwd;
    
    // second line : BCIDs
    if (iwd>=raw.size()) return 0;
    m_errors[ib].bcsu_bcid.set((raw[iwd])& 0xF, m_ref_l0_B_Id);
    for (int ipu= 0; ipu<4 ;++ipu) {
      m_errors[ib].pus_bcid[ipu].set( (raw[iwd]>>(4+ipu*4)) &  0xF, m_ref_l0_B_Id) ;
    }
    m_errors[ib].errH.set((raw[iwd]>>20)& 0xF);
    empty|=((raw[iwd])& 0xFF000000);
    ++iwd;    
    
    // third line : Status
    if (iwd>=raw.size()) return 0;
    int statusBCSU=((raw[iwd])& 0xF);
    if (statusBCSU>2) m_errors[ib].bcsu_status.set(statusBCSU);
    else m_errors[ib].bcsu_status.set(0);
    m_candRegHandlerBCSU[ib].setStatus(statusBCSU); 
    int statusPU[4];
    for (int ipu= 0; ipu<4 ;++ipu) {
      statusPU[ipu]=( (raw[iwd]>>(4+ipu*4)) &  0xF) ;
      if (statusPU[ipu]>2) m_errors[ib].pus_status[ipu].set(statusPU[ipu]);
      else m_errors[ib].pus_status[ipu].set(0);
      m_candRegHandlerPU[ib][ipu].setStatus(statusPU[ipu]); 
    }
    m_errors[ib].errF.set((raw[iwd]>>20)& 0xF);
    bool compressed   = ( (raw[iwd]>>31)& 0x1   );
    empty|=((raw[iwd])& 0x7F000000);
    ++iwd;

    // next lines : candidates
    int ncandBCSU = (statusBCSU& 0x3);
    if (ncandBCSU>0) { // If at least one candidate in this board
      ncandBCSU = ncandBCSU>2 ? 2 : ncandBCSU;
      for (int icand =0;icand<ncandBCSU;++icand){
        if ((iwd+icand)>=raw.size()) return 0;
#if _DEBUG_PROCCAND_ >0
        std::cout.setf(std::ios::uppercase) ;
        std::cout<<"\t=> L0Muon::ProcCandCnv::decodeBank v2-- \tBCSU cand: "<<std::hex<<std::endl;
        std::cout <<"\t0x"<<std::setw(8)<<raw[iwd+icand]<<std::endl;
        std::cout<<std::dec;
        std::cout.unsetf(std::ios::uppercase);
#endif
        L0Muon::writeCandInRegister(&m_candRegHandlerBCSU[ib],raw[iwd+icand],icand,bankVersion);  
        m_candRegHandlerBCSU[ib].setCandQuarter(m_quarter,icand); 
        empty|=((raw[iwd])& 0xC0008000);
      } 
      if (compressed) iwd+=ncandBCSU;
      else iwd+=2;

      for (int ipu= 0; ipu<4 ;++ipu) {
        int ncandPU =( statusPU[ipu] &  0x3) ;
        ncandPU = ncandPU>2 ? 2 : ncandPU;
        for (int icand =0;icand<ncandPU;++icand){
          if ((iwd+icand)>=raw.size()) return 0;
#if _DEBUG_PROCCAND_ >0
        std::cout.setf(std::ios::uppercase) ;
        std::cout<<"\t=> L0Muon::ProcCandCnv::decodeBank v2-- \tPU cand: "<<std::hex<<std::endl;
        std::cout <<"\t0x"<<std::setw(8)<<raw[iwd+icand]<<std::endl;
        std::cout<<std::dec;
        std::cout.unsetf(std::ios::uppercase);
#endif
          L0Muon::writeCandInRegister(&m_candRegHandlerPU[ib][ipu],raw[iwd+icand],icand,bankVersion);
          m_candRegHandlerPU[ib][ipu].setCandQuarter(m_quarter,icand); 
          empty|=((raw[iwd])& 0xC0008000);
        } 
        if (compressed) iwd+=ncandPU;
        else iwd+=2;
      }

    } // End if at least one candidate in this board
    else {
      if (!compressed) iwd+=10;
    }
    m_errors[ib].decoding.set(empty);
    decodingError=(empty>0);
  } // End of loop over processing boards

  if (decodingError) return -1;
  return 1;
}

int L0Muon::ProcCandCnv::rawBank(std::vector<unsigned int> &raw, int ievt, int bankVersion, bool compression)
{
  switch (bankVersion){
  case 1:
    return rawBank_v1(raw,ievt);
    break;
  default:
    return rawBank_v2(raw,ievt, compression);
    break;
  };
  
  return 0;
}


int L0Muon::ProcCandCnv::rawBank_v1(std::vector<unsigned int> &raw, int ievt)
{
  raw.clear();
  static int bankVersion=1;

  int event_number = ievt;
  int l0_bid = ievt;

  // Loop over processing boards
  for (int ib = 0; ib <12 ; ib++) {
    unsigned int word;
    
    // Event numbers
    word = ( (l0_bid<<16)&0xFFF0000 )+ ( event_number&0xFFF );
    raw.push_back(word);

    // Status
    word = m_candRegHandlerBCSU[ib].getStatus();   
    for (int ipu= 0; ipu<4 ;++ipu) {
      word|= ( m_candRegHandlerPU[ib][ipu].getStatus() <<(4+ipu*4) );
    }    
    raw.push_back(word);

    // Candidates
    for (int icand=0; icand<2; icand++){
      if (m_candRegHandlerBCSU[ib].isEmpty(icand)) break;
      word = L0Muon::readCandFromRegister(&m_candRegHandlerBCSU[ib], icand, bankVersion);
      raw.push_back(word&0x30FFFFFF);// mask the quarter and the board
    } 
    for (int ipu= 0; ipu<4 ;++ipu) {
      for (int icand=0; icand<2; icand++){
        if (m_candRegHandlerPU[ib][ipu].isEmpty(icand)) break;
        word = L0Muon::readCandFromRegister(&m_candRegHandlerPU[ib][ipu], icand, bankVersion);
        raw.push_back(word&0x30FFFFFF);// mask the quarter and the board
      } 
    }

  } // End of Loop over processing boards
  
  return 1;
}



int L0Muon::ProcCandCnv::rawBank_v2(std::vector<unsigned int> &raw, int ievt, bool compression)
{
  raw.clear();

  static int bankVersion=2;

  int event_number = ievt;
  int l0_bid = ievt;
  int bcid   = (ievt&0xF);
  
  // Loop over processing boards
  for (int ib = 0; ib <12 ; ib++) {
    unsigned int word;
    
    // 1st line : Event numbers
    word = ( (l0_bid<<16)&0xFFF0000 ) + ( event_number&0xFFF );
    raw.push_back(word);

    // 2nd line : BCIDs
    word = bcid;
    for (int ipu= 0; ipu<4 ;++ipu) {
      word|= ( bcid <<(4+ipu*4) );
    }    
    raw.push_back(word);

    // 3rd line : Status
    word = m_candRegHandlerBCSU[ib].getStatus();   
    for (int ipu= 0; ipu<4 ;++ipu) {
      word|= ( m_candRegHandlerPU[ib][ipu].getStatus() <<(4+ipu*4) );
    }    
    if (compression) word|=0x80000000;
    raw.push_back(word);

    // Next lines : Candidates
    int ncand=2;
    if (compression) {
      ncand=m_candRegHandlerBCSU[ib].getStatus()&0x3;
      if (ncand>2) ncand=2;
    }
    for (int icand=0; icand<ncand; icand++){
      word = L0Muon::readCandFromRegister(&m_candRegHandlerBCSU[ib], icand, bankVersion);
      word&=0x3FFFFFFF; // mask the quarter 
      raw.push_back(word);
    } 
    for (int ipu= 0; ipu<4 ;++ipu) {
      ncand=2;
      if (compression) {
        ncand=m_candRegHandlerPU[ib][ipu].getStatus()&0x3;
        if (ncand>2) ncand=2;
      }
      for (int icand=0; icand<ncand; icand++){
        word = L0Muon::readCandFromRegister(&m_candRegHandlerPU[ib][ipu], icand, bankVersion);
        word&=0x3FFFFFFF; // mask the quarter 
        raw.push_back(word);
      } 
    }

  } // End of Loop over processing boards

//   std::cout.setf(std::ios::uppercase) ;
//   std::cout<<"\t=> L0Muon::ProcCandCnv::rawBank -- dump raw bank of size: "<<raw.size()<<std::hex<<std::endl;
//   for (std::vector<unsigned int>::const_iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
//     std::cout <<"\t0x"<<std::setw(8)<<(*itraw)<<std::endl;
//   }
//   std::cout<<std::dec;
//   std::cout.unsetf(std::ios::uppercase);
  
  return 1;
}



