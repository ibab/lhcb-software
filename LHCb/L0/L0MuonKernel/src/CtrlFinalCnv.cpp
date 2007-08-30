#include "L0MuonKernel/CtrlFinalCnv.h"

#include "ProcessorKernel/RegisterFactory.h"
#include "ProcessorKernel/Register.h"

#include "L0MuonKernel/BankUtilities.h"

/**
   Constructor.
*/
L0Muon::CtrlFinalCnv::CtrlFinalCnv(){
  m_side=0;
};
/**
   Constructor.
*/
L0Muon::CtrlFinalCnv::CtrlFinalCnv(int side){
  m_side=side;

  char buf[4096];
  char* format ;
  
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

  for (int iq = 0; iq<2 ; ++iq){
    format = "CAND_CTRLQ%d";
    sprintf(buf,format,m_side*2+iq+1);
    L0Muon::Register* reg = rfactory->createRegister(buf,CandRegisterHandler::size());
    m_candRegHandler[iq] = CandRegisterHandler(reg) ;
  }
};


/**
   Destructor
*/
L0Muon::CtrlFinalCnv::~CtrlFinalCnv(){
}

void L0Muon::CtrlFinalCnv::release(){
  for (int iq = 0; iq<2 ; ++iq) m_candRegHandler[iq].clear();
}

std::vector<L0Muon::PMuonCandidate>  L0Muon::CtrlFinalCnv::muonCandidates(){
  std::vector<L0Muon::PMuonCandidate> cands;
  for (int iq = 0; iq<2 ; ++iq){
    int ncand= m_candRegHandler[iq].numberOfCandidates();
    for (int icand = 0;icand<ncand;icand++) {
      cands.push_back(m_candRegHandler[iq].getMuonCandidate(icand));
    }    
  }
  
  return cands;
}

void L0Muon::CtrlFinalCnv::decodeBank(std::vector<unsigned int> raw, int bankVersion){

  // Clear the registers first
  release();
  
  switch (bankVersion) {
  case 0:
    decodeBankDC06(raw);
    break;
  default:
    decodeBankV1(raw);
    break;
  }
}

std::vector<unsigned int> L0Muon::CtrlFinalCnv::rawBank(int bankVersion, int ievt){
  std::vector<unsigned int> raw;
  switch (bankVersion){ 
  case 0:
    std::cout <<"L0Muon::CtrlFinalCnv::rawBank ERROR "<<std::endl;
    std::cout <<"bankVersion 0 not handled that way"<<std::endl;    
    break;
  default:
    raw=rawBankV1(ievt);
    break;
  }
  return raw;
}

void L0Muon::CtrlFinalCnv::decodeBankV1(std::vector<unsigned int> raw){
  static int bankVersion =  1;
  
//   std::cout.setf(std::ios::uppercase) ;
//   std::cout<<"\t=> L0Muon::CtrlFinalCnv::decodeBankV1 -- dump raw bank of size: "<<raw.size()<<std::hex<<std::endl;
//   for (std::vector<unsigned int>::iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
//     std::cout <<"\t0x"<<std::setw(8)<<(*itraw)<<std::endl;
//   }
//   std::cout<<std::dec;
//   std::cout.unsetf(std::ios::uppercase);

//   //   unsigned int header = raw[0];
  int iwd = 0;
  for (int iq=0; iq<2; ++iq) {
    //     int event_number = (raw[iwd+1]    )&0xFFF;     // raw[1],         
    //     int l0_bid       = (raw[iwd+1]<<16)&0xFFF;     // raw[1], 
    int status       = (raw[iwd+2]    )&  0xF; // raw[2],raw[ncand(iq)+2],  
    int ncand = status&0x3;
    ncand = ncand>2 ? 2 : ncand;
//     std::cout<<"\t=> L0Muon::CtrlFinalCnv::decodeBankV1 -- iq= "<<iq << " quarter= Q"<<m_side*2+iq
//              <<" ncand = "<<ncand
//              <<std::hex<<" status= "<<status<<std::dec<<std::endl;
    for (int icand=0; icand<ncand; ++icand){
//       std::cout<<"\t=> L0Muon::CtrlFinalCnv::decodeBankV1 --    icand= "<<icand << " word= "<<raw[iwd+3+icand]<<std::endl;
      L0Muon::writeCandInRegister(&m_candRegHandler[iq],raw[iwd+3+icand],icand,bankVersion);  //raw[3], 
      m_candRegHandler[iq].setCandQuarter(m_side*2+iq,icand); 
    }
    m_candRegHandler[iq].setStatus(status);
    iwd+=2+ncand;
  }
}

std::vector<unsigned int> L0Muon::CtrlFinalCnv::rawBankV1( int ievt){
  static int bankVersion =  1;

  std::vector<unsigned int> raw;

  unsigned int header = 0;
  raw.push_back(header); // raw[0]

  int event_number = ievt;
  int l0_bid = ievt;

  for (int iq=0; iq<2; ++iq) {

    unsigned int word;

    word = ((l0_bid)&0xFFF)<<16 + (event_number)&0xFFF;
    raw.push_back(word);// raw[1] & raw[...]
    word = (m_candRegHandler[iq].getStatus())&0xF;
//     std::cout<<"\t=> L0Muon::CtrlFinalCnv::rawBankV1 -- iq="<<iq
//              <<" ncand = "<<((m_candRegHandler[iq].getStatus())&0x3)
//              <<std::hex<<" status= "<<((m_candRegHandler[iq].getStatus())&0xF)<<std::dec<<std::endl;
    raw.push_back(word);// raw[2] & raw[...+2]
    for (int icand=0; icand<2; icand++){
      if (m_candRegHandler[iq].isEmpty(icand)) break;
      word = L0Muon::readCandFromRegister(&m_candRegHandler[iq], icand, bankVersion);
      raw.push_back(word&0x3FFFFFFF);// raw[3(,4)] & raw[...+3] // mask the quarter
    } 
  }

//   std::cout.setf(std::ios::uppercase) ;
//   std::cout<<"\t=> L0Muon::CtrlFinalCnv::rawBankV1 -- dump raw bank of size: "<<raw.size()<<std::hex<<std::endl;
//   for (std::vector<unsigned int>::iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
//     std::cout <<"\t0x"<<std::setw(8)<<(*itraw)<<std::endl;
//   }
//   std::cout<<std::dec;
//   std::cout.unsetf(std::ios::uppercase);
  
  return raw;
}

void L0Muon::CtrlFinalCnv::decodeBankDC06(std::vector<unsigned int> raw){

  static int size    = 31;
  static int max     = 32;
  static int bankVersion =  0;
  
  int qcand[2] = {0,0};
 
// std::cout.setf(std::ios::uppercase) ;
// std::cout<<"\t=> L0Muon::CtrlFinalCnv::decodeBankDC06 -- dump raw bank of size: "<<raw.size()<<std::hex<<std::endl;
// for (std::vector<unsigned int>::iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
//   std::cout <<"\t0x"<<std::setw(8)<<(*itraw)<<std::endl;
// }
// std::cout<<std::dec;
// std::cout.unsetf(std::ios::uppercase);

  int status =( (raw[1]>>4)& 0xF);

// std::cout<<"\t=> L0Muon::CtrlFinalCnv::decodeBankDC06 => status= 0x"<<std::hex<<std::setw(8)<<status<<std::dec<<std::endl;
  int ncandidates = (status& 0xF);
  //ncandidates = ncandidates>2 ? 2 : ncandidates;
// std::cout<<"\t=> L0Muon::CtrlFinalCnv::decodeBankDC06 => ncandidates= "<<ncandidates<<std::endl;
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
// std::cout<<"\t=> L0Muon::CtrlFinalCnv::decodeBankDC06  \ticand= "<<icand<< " q:" <<q<<" belong to this side"<<std::endl;
// std::cout<<"\t=> L0Muon::CtrlFinalCnv::decodeBankDC06  \tdata= "<<std::hex<<std::setw(8)<<data<<std::dec<<std::endl;
// std::cout<<"\t=> L0Muon::CtrlFinalCnv::decodeBankDC06  \t PT=     " << int(( data>>  0 ) & ( (1<<7 ) -1 )) << std::endl;
// std::cout<<"\t=> L0Muon::CtrlFinalCnv::decodeBankDC06  \t Charge= " << int(( data>>  7 ) & ( (1<<1 ) -1 )) << std::endl;
// std::cout<<"\t=> L0Muon::CtrlFinalCnv::decodeBankDC06  \t RowM3=  " << int(( data>>  8 ) & ( (1<<2 ) -1 )) << std::endl;
// std::cout<<"\t=> L0Muon::CtrlFinalCnv::decodeBankDC06  \t ColM3=  " << int(( data>> 10 ) & ( (1<<5 ) -1 )) << std::endl;
// std::cout<<"\t=> L0Muon::CtrlFinalCnv::decodeBankDC06  \t OffM2=  " << int(( data>> 15 ) & ( (1<<4 ) -1 )) << std::endl;
// std::cout<<"\t=> L0Muon::CtrlFinalCnv::decodeBankDC06  \t OffM1=  " << int(( data>> 19 ) & ( (1<<4 ) -1 )) << std::endl;
// std::cout<<"\t=> L0Muon::CtrlFinalCnv::decodeBankDC06  \t PU=     " << int(( data>> 23 ) & ( (1<<2 ) -1 )) << std::endl;
// std::cout<<"\t=> L0Muon::CtrlFinalCnv::decodeBankDC06  \t Board=  " << int(( data>> 25 ) & ( (1<<4 ) -1 )) << std::endl;
// std::cout<<"\t=> L0Muon::CtrlFinalCnv::decodeBankDC06  \t Quarter=" << int(( data>> 29 ) & ( (1<<2 ) -1 )) << std::endl;
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
  
}

std::vector<unsigned int> L0Muon::CtrlFinalCnv::rawBankDC06(){
  static int bankVersion =  0;

  std::vector<unsigned int> raw;

  for (int i=0; i<2; ++i) {
    for (int icand=0; icand<2; icand++){
      if (m_candRegHandler[i].isEmpty(icand)) break;
      unsigned int word = L0Muon::readCandFromRegister(&m_candRegHandler[i], icand, bankVersion);
      raw.push_back(word);
    } 
  }

  return raw;
}

