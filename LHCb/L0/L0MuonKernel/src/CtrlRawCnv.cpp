#include "L0MuonKernel/CtrlRawCnv.h"

#include "ProcessorKernel/RegisterFactory.h"
#include "ProcessorKernel/Register.h"

#include "L0MuonKernel/BankUtilities.h"

/**
   Constructor.
*/
L0Muon::CtrlRawCnv::CtrlRawCnv(){
  m_side=0;
};
/**
   Constructor.
*/
L0Muon::CtrlRawCnv::CtrlRawCnv(int side){
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
      //      L0Muon::Register* reg = rfactory->createRegister(buf,L0Muon::BitsCandRegTot);
      L0Muon::Register* regBCSU = rfactory->createRegister(buf,CandRegisterHandler::size());
      m_candRegHandlerBCSU[iq][iboard] = CandRegisterHandler(regBCSU) ;
    }
  }
};


/**
   Destructor
*/
L0Muon::CtrlRawCnv::~CtrlRawCnv(){
}

void L0Muon::CtrlRawCnv::release(){
 
 for (int iq = 0; iq<2 ; ++iq) {
    m_candRegHandler[iq].clear();
    for (int ib = 0; ib <12 ; ++ib) {
      m_candRegHandlerBCSU[iq][ib].clear();
    }
  }
  
}

std::vector<L0Muon::PMuonCandidate>  L0Muon::CtrlRawCnv::muonCandidates(){
  std::vector<L0Muon::PMuonCandidate> cands;
  for (int iq = 0; iq<2 ; ++iq){
    int ncand= m_candRegHandler[iq].numberOfCandidates();
    for (int icand = 0;icand<ncand;icand++) {
      cands.push_back(m_candRegHandler[iq].getMuonCandidate(icand));
    }    
  }
  
  return cands;
}

std::vector<L0Muon::PMuonCandidate>  L0Muon::CtrlRawCnv::muonCandidatesBCSU(){

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
void L0Muon::CtrlRawCnv::dump(int bankVersion, int ievt)
{
  std::string tab="";
  dump(bankVersion,ievt,tab);
}

void L0Muon::CtrlRawCnv::dump(int bankVersion, int ievt,std::string tab)
{

  std::vector<unsigned int> raw = rawBank(bankVersion,ievt);
  std::cout <<"L0Muon::CtrlRawCnv::dump evt#: "<<ievt<<" - side"<<tab<<m_side<<std::endl;
    
  std::cout <<"L0Muon::CtrlRawCnv::dump "<<tab;
  std::cout <<"  ";
  for (int iq=0; iq<2; ++iq) {
    std::cout <<"    Q"<<std::setw(2)<<m_side*2+iq+1<<"    ";
  }
  std::cout <<std::endl;

  std::cout.setf(std::ios::uppercase) ;

  for (int iline=0; iline<45; ++iline) {

    std::cout <<"L0Muon::ProcRawCnv::dump "<<tab;
    std::cout <<std::setw(2)<<iline;
    std::cout<<std::hex;
    for (int iq=0; iq<2; ++iq) {
      std::cout <<" 0x"<<std::setw(8)<<std::setfill('0')<<raw[45*iq+iline];
    }
    std::cout<<std::dec;
    std::cout <<std::endl;
    
  }
  std::cout <<"L0Muon::ProcRawCnv::dump ----"<<std::endl;  
  std::cout.unsetf(std::ios::uppercase);
}

void L0Muon::CtrlRawCnv::decodeBank(std::vector<unsigned int> raw, int bankVersion){
  if (bankVersion<1) return;
  unsigned int word;
  
  // Clear the registers first
  release();

//   std::cout.setf(std::ios::uppercase) ;
//   std::cout<<"\t=> L0Muon::CtrlRawCnv::decodeBankV1 -- dump raw bank of size: "<<raw.size()<<std::hex<<std::endl;
//   for (std::vector<unsigned int>::iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
//     std::cout <<"\t0x"<<std::setw(8)<<(*itraw)<<std::endl;
//   }
//   std::cout<<std::dec;
//   std::cout.unsetf(std::ios::uppercase);

//   //   unsigned int header = raw[0];

  // =========================
  // TELL1 NON-COMPRESSED BANK
  // =========================

  // Loop over quarters
  for (int iq=0; iq<2; ++iq) {

    int icuwd = iq*45 +  2;
    int isuwd = iq*45 + 25;

    // Loop over BCSU candidates
    for (int ib = 0; ib <12 ; ++ib) {

      if (ib%2==0) {

        // Decode CU frame
        word = (raw[icuwd]>>8)&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandPT(    ( (word    )&0x7F),0);
        m_candRegHandlerBCSU[iq][ib].setCandRowM3( ( (word>> 8)&0x1F),0);
        m_candRegHandlerBCSU[iq][ib].setCandColM3( ( (word>>13)&0x03),0);

        ++icuwd;

        word = (raw[icuwd])&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandPT(    ( (word    )&0x7F),1);
        m_candRegHandlerBCSU[iq][ib].setCandRowM3( ( (word>> 8)&0x1F),1);
        m_candRegHandlerBCSU[iq][ib].setCandColM3( ( (word>>13)&0x03),1);

        word = (raw[icuwd]>>8)&0xFFFF;
//         int bcidcu = word&0xF;

        ++icuwd;

        // Decode SU frame
        word = (raw[isuwd])&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandOffM1( ( (word    )&0x0F),0);
        m_candRegHandlerBCSU[iq][ib].setCandOffM2( ( (word>> 4)&0x0F),0);
        m_candRegHandlerBCSU[iq][ib].setCandPU(    ( (word>> 8)&0x03),0);
        
        word = (raw[isuwd]>>8)&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandOffM1( ( (word    )&0x0F),1);
        m_candRegHandlerBCSU[iq][ib].setCandOffM2( ( (word>> 4)&0x0F),1);
        m_candRegHandlerBCSU[iq][ib].setCandPU(    ( (word>> 8)&0x03),1);
        
        ++isuwd;
        
        word = (raw[isuwd])&0xFFFF;
//         int bcidsu = word&0xF;
        m_candRegHandlerBCSU[iq][ib].setStatus(    ( (word>> 4)&0x0F) );
        m_candRegHandlerBCSU[iq][ib].setCandCharge(( (word>> 8)&0x01),1);
        m_candRegHandlerBCSU[iq][ib].setCandCharge(( (word>>12)&0x01),0);
        
      } else {

        // Decode CU frame
        word = (raw[icuwd])&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandPT(    ( (word    )&0x7F),0);
        m_candRegHandlerBCSU[iq][ib].setCandRowM3( ( (word>> 8)&0x1F),0);
        m_candRegHandlerBCSU[iq][ib].setCandColM3( ( (word>>13)&0x03),0);


        word = (raw[icuwd]>>8)&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandPT(    ( (word    )&0x7F),1);
        m_candRegHandlerBCSU[iq][ib].setCandRowM3( ( (word>> 8)&0x1F),1);
        m_candRegHandlerBCSU[iq][ib].setCandColM3( ( (word>>13)&0x03),1);

        ++icuwd;

        word = (raw[icuwd])&0xFFFF;
//         int bcidcu = word&0xF;

        // Decode SU frame
        word = (raw[isuwd]>>8)&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandOffM1( ( (word    )&0x0F),0);
        m_candRegHandlerBCSU[iq][ib].setCandOffM2( ( (word>> 4)&0x0F),0);
        m_candRegHandlerBCSU[iq][ib].setCandPU(    ( (word>> 8)&0x03),0);
   
        ++isuwd;

        word = (raw[isuwd])&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandOffM1( ( (word    )&0x0F),1);
        m_candRegHandlerBCSU[iq][ib].setCandOffM2( ( (word>> 4)&0x0F),1);
        m_candRegHandlerBCSU[iq][ib].setCandPU(    ( (word>> 8)&0x03),1);
        
        word = (raw[isuwd]>>8)&0xFFFF;
//         int bcidsu = word&0xF;
        m_candRegHandlerBCSU[iq][ib].setStatus(    ( (word>> 4)&0x0F) );
        m_candRegHandlerBCSU[iq][ib].setCandCharge(( (word>> 8)&0x01),1);
        m_candRegHandlerBCSU[iq][ib].setCandCharge(( (word>>12)&0x01),0);
        
        ++isuwd;

      }
      
    }// End of loop over BCSU candidates

     // Selected pair of candidates
    
    // Decode CU frame
    word = (raw[icuwd]>>8)&0xFFFF;
    m_candRegHandler[iq].setCandPT(    ( (word    )&0x7F),0);
    m_candRegHandler[iq].setCandRowM3( ( (word>> 8)&0x1F),0);
    m_candRegHandler[iq].setCandColM3( ( (word>>13)&0x03),0);

    ++icuwd;

    word = (raw[icuwd])&0xFFFF;
    m_candRegHandler[iq].setCandPT(    ( (word    )&0x7F),1);
    m_candRegHandler[iq].setCandRowM3( ( (word>> 8)&0x1F),1);
    m_candRegHandler[iq].setCandColM3( ( (word>>13)&0x03),1);

    word = (raw[icuwd]>>8)&0xFFFF;
//     int bcidcu = word&0xF;

    ++icuwd;

    // Decode SU frame
    word = (raw[isuwd])&0xFFFF;
    m_candRegHandler[iq].setCandOffM1( ( (word    )&0x0F),0);
    m_candRegHandler[iq].setCandOffM2( ( (word>> 4)&0x0F),0);
    m_candRegHandler[iq].setCandPU(    ( (word>> 8)&0x03),0);
        
    word = (raw[isuwd]>>8)&0xFFFF;
    m_candRegHandler[iq].setCandOffM1( ( (word    )&0x0F),1);
    m_candRegHandler[iq].setCandOffM2( ( (word>> 4)&0x0F),1);
    m_candRegHandler[iq].setCandPU(    ( (word>> 8)&0x03),1);
        
    ++isuwd;
        
    word = (raw[isuwd])&0xFFFF;
//     int bcidsu = word&0xF;
    m_candRegHandler[iq].setStatus(    ( (word>> 4)&0x0F) );
    m_candRegHandler[iq].setCandCharge(( (word>> 8)&0x01),1);
    m_candRegHandler[iq].setCandCharge(( (word>>12)&0x01),0);

  }// End of loop over quarters
}

std::vector<unsigned int> L0Muon::CtrlRawCnv::rawBank(int bankVersion,  int ievt){
  std::vector<unsigned int> raw;

  if (bankVersion<1) return raw;

  unsigned int cand[2];

  int event_number = ievt;
  int l0_bid = ievt;
  int bcid = ievt&0xF;

  unsigned int event_num_word = ( (l0_bid<<16)&0xFFF0000 )+ ( event_number&0xFFF );

  // =========================
  // TELL1 NON-COMPRESSED BANK
  // =========================

  // Loop over quarters
  for (int iq=0; iq<2; ++iq) {

    unsigned int word;

    //
    // Record CU frame
    //

    raw.push_back(event_num_word);// raw[(iq*44)+ 0 ] 

    word=0;
    raw.push_back(word);// raw[(iq*44)+ 1 ]
    
    word=0;
    // Loop over BCSU candidates
    for (int ib = 0; ib <12 ; ++ib) {
      for (int icand=0; icand<2; ++icand) {
        int cand_pt = m_candRegHandlerBCSU[iq][ib].getCandPT(icand);
        int cand_colM3 = m_candRegHandlerBCSU[iq][ib].getCandColM3(icand);
        int cand_rowM3 = m_candRegHandlerBCSU[iq][ib].getCandRowM3(icand);
        cand[icand] = ( ( (cand_rowM3<<13)&0x6000 ) + ( (cand_colM3<<8)&0x1F00 ) + ( (cand_pt)&0x7F ) );
      }
        
      if (ib%2==0) {
        word |= ( (cand[0]<<16) & 0x7FFFFFFF );
        raw.push_back(word);
        word = ( ( (bcid<<16)&0xF0000 ) +  (cand[1]&0x7FFF) );
        raw.push_back(word);
        word = 0;
      } else {
        word = ( ( (cand[1]<<16) & 0x7FFF0000 ) +   (cand[0]&0x7FFF) );
        raw.push_back(word);
        word =( bcid&0xF ) ;
      }

    } // End of loop over BCSU candidates
 
    // Selected pair of candidates
    for (int icand=0; icand<2; ++icand) {
      int cand_pt = m_candRegHandler[iq].getCandPT(icand);
      int cand_colM3 = m_candRegHandler[iq].getCandColM3(icand);
      int cand_rowM3 = m_candRegHandler[iq].getCandRowM3(icand);
      cand[icand] = ( ( (cand_rowM3<<13)&0x6000 ) + ( (cand_colM3<<8)&0x1F00 ) + ( (cand_pt)&0x7F ) );
    }
    word |= ( (cand[0]<<16) & 0x7FFFFFFF );
    raw.push_back(word);
    word = ( ( (bcid<<16)&0xF0000 ) +  (cand[1]&0x7FFF) );
    raw.push_back(word);

    //
    // Record SU frame
    //

    raw.push_back(event_num_word);// raw[(iq*44) + 22 + 0 ] 

    word = ( ( (bcid<<4)&0xF0 ) + ( bcid&0xF ) );
    raw.push_back(word);// raw[(iq*44) + 22 + 1 ]
    
    word=0;
    raw.push_back(word);// raw[(iq*44) + 22 + 2 ]
   
    word=0;
    // Loop over BCSU candidates 
    for (int ib = 0; ib <12 ; ++ib) {
      for (int icand=0; icand<2; ++icand) {
        int cand_offM1  = m_candRegHandlerBCSU[iq][ib].getCandOffM1(icand);
        int cand_offM2  = m_candRegHandlerBCSU[iq][ib].getCandOffM2(icand);
        int cand_pu     = m_candRegHandlerBCSU[iq][ib].getCandPU(icand);
        cand[icand] = ( ( (cand_pu<<8)&0x300 ) + ( (cand_offM2<<4)&0xF0 ) + ( (cand_offM1)&0xF ) );
      }

      int status = m_candRegHandlerBCSU[iq][ib].getStatus();
      int c1     = m_candRegHandlerBCSU[iq][ib].getCandCharge(0);
      int c2     = m_candRegHandlerBCSU[iq][ib].getCandCharge(1);      
      if (ib%2==0) {
        word = ( ( (cand[1]<<16) & 0x03FF0000 ) +   (cand[0]&0x03FF) );
        raw.push_back(word);
        word = ( ( (c1<<12)&0x1000 ) + ( (c2<<8)&0x100 ) + ( (status<<4)&0xF0 ) + (bcid&0xF ) );
      } else {
        word|=( (cand[0]<<16) & 0x03FF0000 );
        raw.push_back(word);
        word = ( ( (c1<<28)&0x10000000 ) + ( (c2<<24)&0x1000000 ) + ( (status<<20)&0xF00000 ) + ( (bcid<<16)&0xF0000 ) \
                 + (cand[1]&0x03FF) );
        raw.push_back(word);
        word = 0;
      }

    } // End of loop over BCSU candidates
    
    // Selected pair of candidates
    for (int icand=0; icand<2; ++icand) {
      int cand_offM1  = m_candRegHandler[iq].getCandOffM1(icand);
      int cand_offM2  = m_candRegHandler[iq].getCandOffM2(icand);
      int cand_pu     = m_candRegHandler[iq].getCandPU(icand);
      int cand_board = m_candRegHandler[iq].getCandBoard(icand);
      cand[icand] = ( ( (cand_board<<12)&0xF000 ) + ( (cand_pu<<8)&0x300 ) + ( (cand_offM2<<4)&0xF0 ) + ( (cand_offM1)&0xF ) );
    }
    
    int status = m_candRegHandler[iq].getStatus();
    int c1     = m_candRegHandler[iq].getCandCharge(0);
    int c2     = m_candRegHandler[iq].getCandCharge(1);      
    word = ( ( (cand[1]<<16) & 0x03FF0000 ) +   (cand[0]&0x03FF) );
    raw.push_back(word);
    word = ( ( (c1<<12)&0x1000 ) + ( (c2<<8)&0x100 ) + ( (status<<4)&0xF0 ) + (bcid&0xF ) );
    raw.push_back(word);
    
 }// End of loop over quarters

  // ===============
  // TELL1 INFO BANK
  // ===============

  // Loop over quarters
  for (int iq=0; iq<2; ++iq) {
    for (int iw=0;iw<3;++iw) {
      unsigned int word=0;
      raw.push_back(word);// raw[(iq*44)+ 1 ]
    }
  }// End of loop over quarters
  

//   std::cout.setf(std::ios::uppercase) ;
//   std::cout<<"\t=> L0Muon::CtrlRawCnv::rawBankV1 -- dump raw bank of size: "<<raw.size()<<std::hex<<std::endl;
//   for (std::vector<unsigned int>::iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
//     std::cout <<"\t0x"<<std::setw(8)<<(*itraw)<<std::endl;
//   }
//   std::cout<<std::dec;
//   std::cout.unsetf(std::ios::uppercase);
  
  return raw;
}


