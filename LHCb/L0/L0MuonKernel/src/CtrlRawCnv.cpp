#include "L0MuonKernel/CtrlRawCnv.h"

#include "ProcessorKernel/RegisterFactory.h"
#include "ProcessorKernel/Register.h"

#include "L0MuonKernel/BankUtilities.h"

/**
   Constructor.
*/
L0Muon::CtrlRawCnv::CtrlRawCnv(){
  m_activ = false; 
  m_side=0;
};
/**
   Constructor.
*/
L0Muon::CtrlRawCnv::CtrlRawCnv(int side){
  m_activ = false;
   
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
 m_activ = false;
 
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
void L0Muon::CtrlRawCnv::dump(int bankVersion)
{
  std::string tab="";
  dump(bankVersion,tab);
}

void L0Muon::CtrlRawCnv::dump(int bankVersion,std::string tab)
{

  std::vector<unsigned int> raw = rawBank(bankVersion);
  std::cout <<"L0Muon::CtrlRawCnv::dump side"<<tab<<m_side<<std::endl;
    
  std::cout <<"L0Muon::CtrlRawCnv::dump "<<tab;
  std::cout <<"  ";
  for (int iq=0; iq<2; ++iq) {
    std::cout <<"    Q"<<std::setw(1)<<m_side*2+iq+1<<"    ";
  }
  std::cout <<std::endl;

  std::cout.setf(std::ios::uppercase) ;

  for (unsigned int iline=0; iline<L0Muon::CtrlRawCnv::board_full_data_size; ++iline) {

    std::cout <<"L0Muon::CtrlRawCnv::dump "<<tab;
    std::cout <<std::setw(2)<<iline;
    std::cout<<std::hex;
    for (int iq=0; iq<2; ++iq) {
      std::cout <<" 0x"<<std::setw(8)<<std::setfill('0')<<raw[45*iq+iline];
    }
    std::cout<<std::dec;
    std::cout <<std::endl;
    
  }
  std::cout <<"L0Muon::CtrlRawCnv::dump ----"<<std::endl;  
  std::cout.unsetf(std::ios::uppercase);
}

void L0Muon::CtrlRawCnv::formattedDump(int bankVersion) 
{
  std::string tab="";
  formattedDump(bankVersion,tab);
}

void L0Muon::CtrlRawCnv::formattedDump(int bankVersion, std::string tab)
{
  std::vector<unsigned int> raw = rawBank(bankVersion);
  std::cout <<"++ "<<tab<<" - Side "<<m_side<<" ; bank size: "<<raw.size()<<std::endl;
  std::cout <<"++ "<<tab<<" formattedDump !!! TO BE IMPLEMENTED !!!"<<std::endl;
}

void L0Muon::CtrlRawCnv::decodeBank(std::vector<unsigned int> raw, int bankVersion){

  if (bankVersion<1) return;

  unsigned int word;
  unsigned int empty;
  
  // temporary !!!
  std::vector<unsigned int> original = raw;

  // Clear the registers first
  release();
  m_activ = true;
  
  for (int iq=0; iq<2; ++iq){
    m_decodingError[iq]=0;
  }

  // =========================
  // TELL1 NON-COMPRESSED BANK
  // =========================

  //   //   unsigned int header = raw[0];

  // Loop over quarters
  for (int iq=0; iq<2; ++iq) {

    int icuwd = iq*L0Muon::CtrlRawCnv::board_full_data_size;
    int isuwd = iq*L0Muon::CtrlRawCnv::board_full_data_size + L0Muon::CtrlRawCnv::board_full_frame_size;
    
    // first word contains event number and BID
    word = raw[icuwd];
    m_L0_B_Id[iq][0]       = ((word>>16)&0xFFF);    
    m_L0EventNumber[iq][0] = ((word>> 0)&0xFFF);
    m_boardIndex[iq][0]    = ((word>>12)&0xF) ;
    empty = ((word>>28)&0xF);
    if (empty!=0) {
      ++m_decodingError[iq];
      std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" A"<<std::endl;
    }
    
    word = raw[isuwd];
    m_L0_B_Id[iq][1]       = ((word>>16)&0xFFF);    
    m_L0EventNumber[iq][1] = ((word>> 0)&0xFFF);
    m_boardIndex[iq][1]    = ((word>>12)&0xF) ;
    empty = ((word>>28)&0xF);
    if (empty!=0) {
      ++m_decodingError[iq];
      std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" B"<<std::endl;
    }
    
    ++icuwd;
    ++isuwd;

    // CU ERROR FIELD 
    word = raw[icuwd];
    for (int ib=0; ib<8;++ib){
      m_pb_link_error[iq][CU][ib]=((word>>(4*(ib)))&0x3);
      empty =((word>>((4*(ib))+2))&0x3);
      if (empty!=0) {
        ++m_decodingError[iq];
        std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" C"<<std::endl;
      }
      
    }
    ++icuwd;
    word = raw[icuwd];
    for (int ib=8; ib<12;++ib){
      m_pb_link_error[iq][CU][ib]=((word>>(4*(ib-8)))&0x3);
      empty = ((word>>((4*(ib-8))+2))&0x3);
      if (empty!=0) {
        ++m_decodingError[iq];
        std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" D"<<std::endl;
      }
      
    }
    // the 16 ls-bits of this word will be decoded later
    // Get the CU and SU BCIDs from the msb of the next word in the SU frame
    word = raw[isuwd];
    m_BCID_CU[iq]=((word>>4)&0xF);
    m_BCID_SU[iq]=((word>>0)&0xF);
    m_error[iq]=((word>>8)&0x1);
    empty = ((word>>9)&0x7F);
    if (empty!=0) {
      ++m_decodingError[iq];std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" E"<<std::endl;
    }
    
    
    // SU ERROR FIELD 
    for (int ib=0; ib<4;++ib){
      m_pb_link_error[iq][SU][ib]=((word>>(16+(4*ib)))&0x3);
      empty =((word>>(16+(4*ib)+2))&0x3);
      if (empty!=0) {
        ++m_decodingError[iq];
        std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" F"<<std::endl;
      }
      
    }
    ++isuwd;
    word = raw[isuwd];
    for (int ib=4; ib<12;++ib){
      m_pb_link_error[iq][SU][ib]=((word>>(4*(ib-4)))&0x3);
      empty =((word>>((4*(ib-4))+2))&0x3);
      if (empty!=0) {
        ++m_decodingError[iq];std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" F"<<std::endl;
      }
      
    }
    ++isuwd;

    // Loop over BCSU candidates
    for (int ib = 0; ib <12 ; ++ib) {

      if (ib%2==0) {

        // Decode CU frame
        
        word = (raw[icuwd]>>16)&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandPT(    ( (word    )&0x7F),0);
        m_candRegHandlerBCSU[iq][ib].setCandColM3( ( (word>> 8)&0x1F),0);
        m_candRegHandlerBCSU[iq][ib].setCandRowM3( ( (word>>13)&0x03),0);
        
        empty =(word&0x8080);
        if (empty!=0) {
          ++m_decodingError[iq];std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" G"<<std::endl;
        }
        
        
        ++icuwd;

        word = (raw[icuwd])&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandPT(    ( (word    )&0x7F),1);
        m_candRegHandlerBCSU[iq][ib].setCandColM3( ( (word>> 8)&0x1F),1);
        m_candRegHandlerBCSU[iq][ib].setCandRowM3( ( (word>>13)&0x03),1);

        empty =(word&0x8080);
        if (empty!=0) {
          ++m_decodingError[iq];std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" H"<<std::endl;
        }
        

        word = (raw[icuwd]>>16)&0xFFFF;
        int bcidcu = word&0xF;
        m_BCID_BCSU[iq][CU][ib]=bcidcu;

        empty =(word&0xFFF0);
        if (empty!=0) {
          ++m_decodingError[iq];
          std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" I"<<std::endl;
        }
        

        ++icuwd;

        // Decode SU frame
        word = (raw[isuwd])&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandOffM1( ( (word    )&0x0F),0);
        m_candRegHandlerBCSU[iq][ib].setCandOffM2( ( (word>> 4)&0x0F),0);
        m_candRegHandlerBCSU[iq][ib].setCandPU(    ( (word>> 8)&0x03),0);

        empty =(word&0xFC00);
        if (empty!=0) {
          ++m_decodingError[iq];
          std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" J"<<std::endl;
        }
        
        
        word = (raw[isuwd]>>16)&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandOffM1( ( (word    )&0x0F),1);
        m_candRegHandlerBCSU[iq][ib].setCandOffM2( ( (word>> 4)&0x0F),1);
        m_candRegHandlerBCSU[iq][ib].setCandPU(    ( (word>> 8)&0x03),1);

        empty =(word&0xFC00);
        if (empty!=0) {
          ++m_decodingError[iq];std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" K"<<std::endl;
        }
        
        ++isuwd;
        
        word = (raw[isuwd])&0xFFFF;
        int bcidsu = word&0xF;
        m_BCID_BCSU[iq][SU][ib]=bcidsu;
        m_candRegHandlerBCSU[iq][ib].setStatus(    ( (word>> 4)&0x0F) );
        m_candRegHandlerBCSU[iq][ib].setCandCharge(( (word>> 8)&0x01),1);
        m_candRegHandlerBCSU[iq][ib].setCandCharge(( (word>>12)&0x01),0);
        
        empty =(word&0xEE00);
        if (empty!=0) {
          ++m_decodingError[iq];std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" L"<<std::endl;
        }
        

      } else {

        // Decode CU frame
        word = (raw[icuwd])&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandPT(    ( (word    )&0x7F),0);
        m_candRegHandlerBCSU[iq][ib].setCandColM3( ( (word>> 8)&0x1F),0);
        m_candRegHandlerBCSU[iq][ib].setCandRowM3( ( (word>>13)&0x03),0);

        empty =(word&0x8080);
        if (empty!=0) {
          ++m_decodingError[iq];std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" M"<<std::endl;
        }
        

        word = (raw[icuwd]>>16)&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandPT(    ( (word    )&0x7F),1);
        m_candRegHandlerBCSU[iq][ib].setCandColM3( ( (word>> 8)&0x1F),1);
        m_candRegHandlerBCSU[iq][ib].setCandRowM3( ( (word>>13)&0x03),1);

        empty =(word&0x8080);
        if (empty!=0) {
          ++m_decodingError[iq];std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" N"<<std::endl;
        }
        

        ++icuwd;

        word = (raw[icuwd])&0xFFFF;
        int bcidcu = word&0xF;
        m_BCID_BCSU[iq][CU][ib]=bcidcu;

        empty =(word&0xFFF0);
        if (empty!=0) {
          ++m_decodingError[iq];std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" O"<<std::endl;
        }
        

        // Decode SU frame
        word = (raw[isuwd]>>16)&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandOffM1( ( (word    )&0x0F),0);
        m_candRegHandlerBCSU[iq][ib].setCandOffM2( ( (word>> 4)&0x0F),0);
        m_candRegHandlerBCSU[iq][ib].setCandPU(    ( (word>> 8)&0x03),0);

        empty =(word&0xFC00);
        if (empty!=0) {
          ++m_decodingError[iq];std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" P"<<std::endl;
        }
        
   
        ++isuwd;

        word = (raw[isuwd])&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandOffM1( ( (word    )&0x0F),1);
        m_candRegHandlerBCSU[iq][ib].setCandOffM2( ( (word>> 4)&0x0F),1);
        m_candRegHandlerBCSU[iq][ib].setCandPU(    ( (word>> 8)&0x03),1);
        
        empty =(word&0xFC00);
        if (empty!=0) {
          ++m_decodingError[iq];std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" Q"<<std::endl;
        }
        

        word = (raw[isuwd]>>16)&0xFFFF;
        int bcidsu = word&0xF;
        m_BCID_BCSU[iq][SU][ib]=bcidsu;
        m_candRegHandlerBCSU[iq][ib].setStatus(    ( (word>> 4)&0x0F) );
        m_candRegHandlerBCSU[iq][ib].setCandCharge(( (word>> 8)&0x01),1);
        m_candRegHandlerBCSU[iq][ib].setCandCharge(( (word>>12)&0x01),0);
        
        empty =(word&0xEE00);
        if (empty!=0) {
          ++m_decodingError[iq];std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" S"<<std::endl;
        }
        

        ++isuwd;

      }
      
      int status = m_candRegHandlerBCSU[iq][ib].getStatus();
      int ncand = status&0x3;
      ncand = ncand<3 ? ncand : 2;
      if (ncand!=m_candRegHandlerBCSU[iq][ib].numberOfCandidates()) {
        m_candRegHandlerBCSU[iq][ib].clear();
        m_candRegHandlerBCSU[iq][ib].setStatus(status);
        ++m_decodingError[iq];
        std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" T"<<std::endl;
        std::cout<<"L0Muon::CtrlRawCnv::decodeBank ERROR : non empty candidate in contradiction with status !!!"
                 << " Q"<<m_side*2+iq<<" BCSU rd# "<<ib
                 <<std::hex<<" 0x"<<m_candRegHandlerBCSU[iq][ib].getStatus()<<std::dec<<std::endl;
        m_candRegHandlerBCSU[iq][ib].dump();
      } else {
        for( int icand =0; icand<ncand;++icand){
          m_candRegHandlerBCSU[iq][ib].setCandBoard(ib,icand);
          m_candRegHandlerBCSU[iq][ib].setCandQuarter(m_side*2+iq,icand);
        }
      }
    
    }// End of loop over BCSU candidates

     // Selected pair of candidates
    
    // Decode CU frame
    word = (raw[icuwd]>>16)&0xFFFF;
    m_candRegHandler[iq].setCandPT(    ( (word    )&0x7F),0);
    m_candRegHandler[iq].setCandColM3( ( (word>> 8)&0x1F),0);
    m_candRegHandler[iq].setCandRowM3( ( (word>>13)&0x03),0);

    empty =(word&0x8080);
    if (empty!=0) {
      ++m_decodingError[iq];
      std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" V"<<std::endl;
    }
    
    ++icuwd;

    word = (raw[icuwd])&0xFFFF;
    m_candRegHandler[iq].setCandPT(    ( (word    )&0x7F),1);
    m_candRegHandler[iq].setCandColM3( ( (word>> 8)&0x1F),1);
    m_candRegHandler[iq].setCandRowM3( ( (word>>13)&0x03),1);

    empty =(word&0x8080);
    if (empty!=0) {
      ++m_decodingError[iq];std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" W"<<std::endl;
    }
    

    word = (raw[icuwd]>>16)&0xFFFF;
    int bcidcu = word&0xF;
    m_BCID[iq][CU]=bcidcu;

    empty =(word&0xFFF0);
    if (empty!=0) {
      ++m_decodingError[iq];std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" X"<<std::endl;
    }
    

    ++icuwd;

    empty = (raw[icuwd])&0xFFFFFFFF;
    if (empty!=0) {
      ++m_decodingError[iq];std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" Y"<<std::endl;
    }
    

    // Decode SU frame
    word = (raw[isuwd])&0xFFFF;
    m_candRegHandler[iq].setCandOffM1( ( (word    )&0x0F),0);
    m_candRegHandler[iq].setCandOffM2( ( (word>> 4)&0x0F),0);
    m_candRegHandler[iq].setCandPU(    ( (word>> 8)&0x03),0);
    m_candRegHandler[iq].setCandBoard( ( (word>>12)&0x0F),0);
        
    empty =(word&0x0C00);
    if (empty!=0) {
      ++m_decodingError[iq];std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" Z"<<std::endl;
    }
    

    word = (raw[isuwd]>>16)&0xFFFF;
    m_candRegHandler[iq].setCandOffM1( ( (word    )&0x0F),1);
    m_candRegHandler[iq].setCandOffM2( ( (word>> 4)&0x0F),1);
    m_candRegHandler[iq].setCandPU(    ( (word>> 8)&0x03),1);
    m_candRegHandler[iq].setCandBoard( ( (word>>12)&0x0F),1);
        
    empty =(word&0x0C00);
    if (empty!=0) {
      ++m_decodingError[iq];std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" AA"<<std::endl;
    }
    

    ++isuwd;
        
    word = (raw[isuwd])&0xFFFF;
    int bcidsu = word&0xF;
    m_BCID[iq][SU]=bcidsu;
    m_candRegHandler[iq].setStatus(    ( (word>> 4)&0x0F) );
    m_candRegHandler[iq].setCandCharge(( (word>> 8)&0x01),1);
    m_candRegHandler[iq].setCandCharge(( (word>>12)&0x01),0);

    empty =(word&0xEE00);
    if (empty!=0) {
      ++m_decodingError[iq];
      std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" AB"<<std::endl;
    }
    
    empty = (raw[isuwd]>>16)&0xFFFF;
    if (empty!=0) {
      ++m_decodingError[iq];
      std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" AC"<<std::endl;
    }
    

    int status =m_candRegHandler[iq].getStatus();
    int ncand = status&0x3;
    ncand = ncand<3 ? ncand : 2;
    if (ncand!=m_candRegHandler[iq].numberOfCandidates()) {
      m_candRegHandler[iq].clear();
      m_candRegHandler[iq].setStatus(status);
      ++m_decodingError[iq];
      std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" AD"<<std::endl;
      std::cout<<"L0Muon::CtrlRawCnv::decodeBank ERROR : non empty candidate in contradiction with status !!!"
               << " Q"<<m_side*2+iq
               <<std::hex<<" 0x"<<m_candRegHandler[iq].getStatus()<<std::dec<<std::endl;
      m_candRegHandler[iq].dump();
    } else {
      for( int icand =0; icand<ncand;++icand) {
        m_candRegHandler[iq].setCandQuarter(m_side*2+iq,icand);
      }
    }

    if (m_decodingError[iq]>0){
      m_candRegHandler[iq].clear();
      for (int ib = 0; ib <12 ; ++ib) {
        m_candRegHandlerBCSU[iq][ib].clear();
      }
    }
    
  }// End of loop over quarters

  // temporary check
  std::vector<unsigned int> tmp = rawBank(bankVersion);
  for (int iq=0; iq<2; ++iq){
    for (unsigned int i=0; i<L0Muon::CtrlRawCnv::board_full_data_size;++i){
      unsigned int iw = iq*L0Muon::CtrlRawCnv::board_full_data_size+i;
      if (tmp[iw]!=original[iw]) {
        if (m_decodingError[iq]==0){
          ++m_decodingError[iq];
          std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" AE"<<std::endl;
          std::cout<<"\t !!! L0Muon::CtrlRawCnv::decodeBank !!! ERROR !!! line "<<iw<<" Q"<<m_side*2+iq
                   <<std::hex<<" rebuild= 0x"<<tmp[iw]<<" VS original= 0x"<<original[iw]<<std::dec<<std::endl;
        }
      }
    }
  }

  // Reset registers of a board if a decoding error has been detected
  for (int iq=0; iq<2; ++iq){
    if (m_decodingError[iq]>0) {
      m_candRegHandler[iq].clear();
      for (int ib = 0; ib<12; ++ib) {
        m_candRegHandlerBCSU[iq][ib].clear();
      }
      std::cout<<"\tL0Muon::ProcRawCnv::decodeBank !!! DECODING ERROR !!! Q"<<m_side*2+iq<<std::endl;
    }
  }
  
}

void L0Muon::CtrlRawCnv::setEventCounters(int L0EventNum, int L0_B_Id){
  for (int iq=0; iq<2; ++iq){
    for (int ich=0; ich<2; ++ich){
      m_L0EventNumber[iq][ich]=L0EventNum;
      m_L0_B_Id[iq][ich]=L0_B_Id;
      m_BCID[iq][ich]=L0_B_Id&0xF;
      for (int ib=0; ib<12; ++ib){
        m_BCID_BCSU[iq][ich][ib]=L0_B_Id&0xF;
      }
    }
    m_BCID_CU[iq]=L0_B_Id&0xF;
    m_BCID_SU[iq]=L0_B_Id&0xF;
  }
}

std::vector<unsigned int> L0Muon::CtrlRawCnv::rawBank(int bankVersion){
  std::vector<unsigned int> raw;
  int event_number;
  int l0_bid;
  int board_index;
  unsigned int event_num_word;
  int bcid;

  if (bankVersion<1) return raw;

  unsigned int cand[2];

  // =========================
  // TELL1 NON-COMPRESSED BANK
  // =========================

  // Loop over quarters
  for (int iq=0; iq<2; ++iq) {

    unsigned int word;

    //
    // Record CU frame
    //

    event_number   = m_L0EventNumber[iq][CU];
    l0_bid         = m_L0_B_Id[iq][CU];
    board_index    = m_boardIndex[iq][CU];
    event_num_word = ((l0_bid<<16)&0xFFF0000 ) + ((board_index<<12)&0xF000) + ( event_number&0xFFF );
    raw.push_back(event_num_word);// raw[(iq*44)+ 0 ] 

    word=0;
    word+= ( (m_pb_link_error[iq][CU][7]<<28)&0x30000000);
    word+= ( (m_pb_link_error[iq][CU][6]<<24)&0x03000000);
    word+= ( (m_pb_link_error[iq][CU][5]<<20)&0x00300000);
    word+= ( (m_pb_link_error[iq][CU][4]<<16)&0x00030000);
    word+= ( (m_pb_link_error[iq][CU][3]<<12)&0x00003000);
    word+= ( (m_pb_link_error[iq][CU][2]<< 8)&0x00000300);
    word+= ( (m_pb_link_error[iq][CU][1]<< 4)&0x00000030);
    word+= ( (m_pb_link_error[iq][CU][0]<< 0)&0x00000003);
    raw.push_back(word);// raw[(iq*44)+ 1 ]
    
    word=0;
    word+= ( (m_pb_link_error[iq][CU][11]<<12)&0x00003000);
    word+= ( (m_pb_link_error[iq][CU][10]<< 8)&0x00000300);
    word+= ( (m_pb_link_error[iq][CU][ 9]<< 4)&0x00000030);
    word+= ( (m_pb_link_error[iq][CU][ 8]<< 0)&0x00000003);

    // Loop over BCSU candidates
    for (int ib = 0; ib <12 ; ++ib) {
      for (int icand=0; icand<2; ++icand) {
        int cand_pt    = m_candRegHandlerBCSU[iq][ib].getCandPT(icand);
        int cand_colM3 = m_candRegHandlerBCSU[iq][ib].getCandColM3(icand);
        int cand_rowM3 = m_candRegHandlerBCSU[iq][ib].getCandRowM3(icand);
        cand[icand] = ( ( (cand_rowM3<<13)&0x6000 ) + ( (cand_colM3<<8)&0x1F00 ) + ( (cand_pt)&0x7F ) );
      }
        
      bcid = m_BCID_BCSU[iq][CU][ib];
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
    bcid = m_BCID[iq][CU];
    word = ( ( (bcid<<16)&0xF0000 ) +  (cand[1]&0x7FFF) );
    raw.push_back(word);
    word = 0;
    raw.push_back(word); 
    // 23 words (32 bits have been written)
    for (unsigned int i=L0Muon::CtrlRawCnv::board_frame_size;i<L0Muon::CtrlRawCnv::board_full_frame_size;++i){
      raw.push_back(word);
    }  

    //
    // Record SU frame
    //

    event_number   = m_L0EventNumber[iq][SU];
    l0_bid         = m_L0_B_Id[iq][SU];
    board_index    = m_boardIndex[iq][SU];
    event_num_word = ((l0_bid<<16)&0xFFF0000 ) + ((board_index<<12)&0xF000) + ( event_number&0xFFF );
    raw.push_back(event_num_word);// raw[(iq*44) + 22 + 0 ] 

    word = ( ( (m_error[iq]<<8)&0x1 ) + ( (m_BCID_CU[iq]<<4)&0xF0 ) + ( m_BCID_SU[iq]&0xF ) );
    word+= ( (m_pb_link_error[iq][SU][3]<<28)&0x30000000);
    word+= ( (m_pb_link_error[iq][SU][2]<<24)&0x03000000);
    word+= ( (m_pb_link_error[iq][SU][1]<<20)&0x00300000);
    word+= ( (m_pb_link_error[iq][SU][0]<<16)&0x00030000);
    
    raw.push_back(word);// raw[(iq*44) + 22 + 1 ]
    
    word=0;
    word+= ( (m_pb_link_error[iq][SU][11]<<28)&0x30000000);
    word+= ( (m_pb_link_error[iq][SU][10]<<24)&0x03000000);
    word+= ( (m_pb_link_error[iq][SU][ 9]<<20)&0x00300000);
    word+= ( (m_pb_link_error[iq][SU][ 8]<<16)&0x00030000);
    word+= ( (m_pb_link_error[iq][SU][ 7]<<12)&0x00003000);
    word+= ( (m_pb_link_error[iq][SU][ 6]<< 8)&0x00000300);
    word+= ( (m_pb_link_error[iq][SU][ 5]<< 4)&0x00000030);
    word+= ( (m_pb_link_error[iq][SU][ 4]<< 0)&0x00000003);
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
      bcid       = m_BCID_BCSU[iq][SU][ib];
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
      int cand_board  = m_candRegHandler[iq].getCandBoard(icand);
      cand[icand] = ( ( (cand_board<<12)&0xF000 ) + ( (cand_pu<<8)&0x300 ) + ( (cand_offM2<<4)&0xF0 ) + ( (cand_offM1)&0xF ) );
    }
    
    int status = m_candRegHandler[iq].getStatus();
    int c1     = m_candRegHandler[iq].getCandCharge(0);
    int c2     = m_candRegHandler[iq].getCandCharge(1);      
    bcid       = m_BCID[iq][SU];
    word = ( ( (cand[1]<<16) & 0xF3FF0000 ) +   (cand[0]&0xF3FF) );
    raw.push_back(word);
    word = ( ( (c1<<12)&0x1000 ) + ( (c2<<8)&0x100 ) + ( (status<<4)&0xF0 ) + (bcid&0xF ) );
    raw.push_back(word);

    // 23 words (32 bits have been written)
    word = 0;
    for (unsigned int i=L0Muon::CtrlRawCnv::board_frame_size;i<L0Muon::CtrlRawCnv::board_full_frame_size;++i){
      raw.push_back(word);
    }
    

 }// End of loop over quarters

  // Loop over quarters
  for (int iq=0; iq<2; ++iq) {
    for (int iw=0;iw<3;++iw) {
      unsigned int word=0;
      raw.push_back(word);// raw[(iq*44)+ 1 ]
    }
  }// End of loop over quarters
  

//   std::cout.setf(std::ios::uppercase) ;
//   int icount=0;
//   std::cout<<"\t=> L0Muon::CtrlRawCnv::rawBank -- dump raw bank of size: "<<raw.size()<<std::endl;
//   for (std::vector<unsigned int>::iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
//     std::cout <<std::setw(3)<<icount<<std::hex<<"\t0x"<<std::setw(8)<<std::setfill('0')<<(*itraw)<<std::dec<<std::endl;
//     ++icount;
//   }
//   std::cout<<std::dec;
//   std::cout.unsetf(std::ios::uppercase);
  
  return raw;
}

bool L0Muon::CtrlRawCnv::inError(bool verbose)
{
  bool error = false;
  
  for (int iq=0;iq<2;++iq){
    for (int ich =0; ich<2; ++ich){
      if (boardIndexError(iq,ich)){
        error = true;
        if (verbose) std::cout<<"L0Muon::CtrlRawCnv::inError : "<<m_side
                              <<" iq="<<iq<<" ich= "<<ich 
                              <<" boardIndexError "
                              <<std::dec<<std::endl;
      }
      for (int ib=0; ib<12;++ib){
        if (m_pb_link_error[iq][ich][ib]>0){
          error = true;
          if (verbose) std::cout<<"L0Muon::CtrlRawCnv::inError : "<<m_side
                                <<" iq= "<<iq<<" ich= "<<ich<<" ib= "<<ib 
                                <<" pb_link_error 0x"<<std::hex<<m_pb_link_error[iq][ich][ib]
                                <<std::dec<<std::endl;
        }
      }
    }
    if (m_error[iq]>0){
      error = true;
      if (verbose) std::cout<<"L0Muon::CtrlRawCnv::inError : "<<m_side
                            <<" iq="<<iq
                            <<" error 0x"<<std::hex<<m_error
                            <<std::dec<<std::endl;
      }
  }
  
  int l0_b_id=m_L0_B_Id[0][0];
  for (int iq=0;iq<2;++iq){
    for (int ich =0; ich<2; ++ich){
      if (m_L0_B_Id[iq][ich]!=l0_b_id){
        error = true;
        if (verbose) std::cout<<"L0Muon::CtrlRawCnv::inError : "<<m_side
                              <<" iq="<<iq<<" ich= "<<ich 
                              <<" L0_B_Id 0x"<<std::hex<<m_L0_B_Id[iq][ich]<<" -VS- ref:0x"<<l0_b_id
                              <<std::dec<<std::endl;
      }
      if ((m_BCID[iq][ich]&0xF)!=(l0_b_id&0xF)){
        error = true;
        if (verbose) std::cout<<"L0Muon::CtrlRawCnv::inError : "<<m_side
                              <<" iq="<<iq<<" ich= "<<ich 
                              <<" BCID 0x"<<std::hex<<(m_BCID[iq][ich]&0xF)<<" -VS- ref:0x"<<l0_b_id
                              <<std::dec<<std::endl;
      }
      for (int ib=0;ib<12;++ib){
        if ((m_BCID_BCSU[iq][ich][ib]&0xF)!=(l0_b_id&0xF)){
          error = true;
          if (verbose) std::cout<<"L0Muon::CtrlRawCnv::inError : "<<m_side
                                <<" iq= "<<iq<<" ich= "<<ich<<" ib= "<<ib 
                                <<" BCID_BCSU 0x"<<std::hex<<(m_BCID_BCSU[iq][ich][ib]&0xF)<<" -VS- ref:0x"<<l0_b_id
                                <<std::dec<<std::endl;
        }
      }
    }
    if ((m_BCID_SU[iq]&0xF)!=(l0_b_id&0xF)){
      error = true;
      if (verbose) std::cout<<"L0Muon::CtrlRawCnv::inError : "<<m_side
                            <<" iq="<<iq
                            <<" BCID_SU 0x"<<std::hex<<(m_BCID_SU[iq]&0xF)<<" -VS- ref:0x"<<l0_b_id
                            <<std::dec<<std::endl;
    }
    if ((m_BCID_CU[iq]&0xF)!=(l0_b_id&0xF)){
      error = true;
      if (verbose) std::cout<<"L0Muon::CtrlRawCnv::inError : "<<m_side
                            <<" iq="<<iq
                            <<" BCID_CU 0x"<<std::hex<<(m_BCID_CU[iq]&0xF)<<" -VS- ref:0x"<<l0_b_id
                            <<std::dec<<std::endl;
    }
    
  }
  int l0_evt_num=m_L0EventNumber[0][0];
  for (int iq=0;iq<2;++iq){
    for (int ich =0; ich<2; ++ich){
      if (m_L0EventNumber[iq][ich]!=l0_evt_num){
        error = true;
        if (verbose) std::cout<<"L0Muon::CtrlRawCnv::inError : "<<m_side
                              <<" iq= "<<iq<<" ich= "<<ich
                              <<" L0EventNumber 0x"<<std::hex<<(m_L0EventNumber[iq][ich])<<" -VS- ref:0x"<<l0_evt_num
                              <<std::dec<<std::endl;
      }
    }
  }
  

  if (verbose) std::cout<<"L0Muon::CtrlRawCnv::inError : "<<m_side
                        <<" return "<<error<<std::dec<<std::endl;
  return error;
}
