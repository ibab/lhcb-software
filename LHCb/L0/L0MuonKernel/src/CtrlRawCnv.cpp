#include "L0MuonKernel/CtrlRawCnv.h"

#include "ProcessorKernel/RegisterFactory.h"
#include "ProcessorKernel/Register.h"

#include "L0MuonKernel/BankUtilities.h"

#include "L0MuonKernel/CtrlRawErrors.h"

#define _DEBUG_RAWDATA_  0
/**
   Constructor.
*/
L0Muon::CtrlRawCnv::CtrlRawCnv(){
  m_activ = false;
  m_side=0;
  m_n_decoded_banks=0;
}
/**
   Constructor.
*/
L0Muon::CtrlRawCnv::CtrlRawCnv(int side){
  m_activ = false;
  m_side=side;
  m_n_decoded_banks=0;

  char buf[4096];

  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

  for (int iq = 0; iq<2 ; ++iq){
    sprintf(buf,"CAND_CTRLQ%d",m_side*2+iq+1);
    L0Muon::Register* reg = rfactory->createRegister(buf,CandRegisterHandler::size());
    m_candRegHandler[iq] = CandRegisterHandler(reg) ;
    for (int iboard = 0; iboard <12 ; iboard++) {
      sprintf(buf,"BOARDCAND_Q%d_%d",m_side*2+iq+1,iboard);
      L0Muon::Register* regBCSU = rfactory->createRegister(buf,CandRegisterHandler::size());
      m_candRegHandlerBCSU[iq][iboard] = CandRegisterHandler(regBCSU) ;
    }
  }
}


/**
   Destructor
*/
L0Muon::CtrlRawCnv::~CtrlRawCnv(){
}

LHCb::MuonTileID L0Muon::CtrlRawCnv::mid_BCSU(int iq, int ib){
  MuonLayout lay(1,1);
  LHCb::MuonTileID board = LHCb::MuonTileID(0);
  board.setQuarter(m_side*2+iq);
  board.setLayout(lay);
  board.setRegion(ib/3);
  board.setX( (((ib%3)+1)>>0)&1);
  board.setY( (((ib%3)+1)>>1)&1);
  return board;
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
void L0Muon::CtrlRawCnv::dump(const std::vector<unsigned int> &raw)
{
  std::cout <<"L0Muon::CtrlRawCnv::dump side"<<m_side<<std::endl;

  std::cout <<"L0Muon::CtrlRawCnv::dump ";
  std::cout <<"  ";
  for (int iq=0; iq<2; ++iq) {
    std::cout <<"    Q"<<std::setw(1)<<m_side*2+iq+1<<"    ";
  }
  std::cout <<std::endl;

  std::cout.setf(std::ios::uppercase) ;

  for (unsigned int iline=0; iline<L0Muon::CtrlRawCnv::board_full_data_size; ++iline) {

    std::cout <<"L0Muon::CtrlRawCnv::dump ";
    std::cout <<std::setw(2)<<iline;
    std::cout<<std::hex;
    for (int iq=0; iq<2; ++iq) {
      std::cout <<" 0x"<<std::setw(8)<<std::setfill('0')<<raw[L0Muon::CtrlRawCnv::board_full_data_size*iq+iline];
    }
    std::cout<<std::dec;
    std::cout <<std::endl;

  }
  std::cout <<"L0Muon::CtrlRawCnv::dump ----"<<std::endl;
  std::cout.unsetf(std::ios::uppercase);
}

void L0Muon::CtrlRawCnv::dumpErrorCounters(std::string &os) {
  int sum = 0;
  for (int iq=0; iq<2; ++iq) sum+=m_errors[iq].sumCounters();
  os+=(boost::format("%d errors found\n") % sum ).str();

  for (int iq=0; iq<2; ++iq)
    m_errors[iq].printCounters( os ,(boost::format("\tQ%1d ") % (m_side*2+iq+1)).str() );
}

void L0Muon::CtrlRawCnv::decodeBank(const std::vector<unsigned int> &raw, int bankVersion,
                                    int &Refl0EventNumber, int &Refl0_B_Id){

  if (bankVersion<1) return;

  if (bankVersion==1) decodeBank_v1(raw,Refl0EventNumber,Refl0_B_Id);

  if (bankVersion==2) decodeBank_v2(raw,Refl0EventNumber,Refl0_B_Id);

}

void L0Muon::CtrlRawCnv::decodeBank_v1(const std::vector<unsigned int> &raw, int &Refl0EventNumber, int &Refl0_B_Id){

  unsigned int word;
  unsigned int empty;

  // temporary !!!
  std::vector<unsigned int> original = raw;

  // Clear the registers first
  release();
  m_activ = true;

  int decodingError[2];
  for (int iq=0; iq<2; ++iq){
    decodingError[iq]=0;
  }

  // =========================
  // TELL1 NON-COMPRESSED BANK
  // =========================

  //   //   unsigned int header = raw[0];

  // Loop over quarters
  for (int iq=0; iq<2; ++iq) {
    int l0_B_Id, l0EventNumber;

    int icuwd = iq*L0Muon::CtrlRawCnv::board_full_data_size;
    int isuwd = iq*L0Muon::CtrlRawCnv::board_full_data_size + L0Muon::CtrlRawCnv::board_full_frame_size;

    // first word contains event number and BID
    word = raw[icuwd];
    l0_B_Id       = ((word>>16)&0xFFF);
    Refl0_B_Id  = Refl0_B_Id==-1 ? l0_B_Id : Refl0_B_Id;
    m_errors[iq].l0_B_Id[0].set(l0_B_Id,Refl0_B_Id);

    l0EventNumber = ((word>> 0)&0xFFF);
    Refl0EventNumber  = Refl0EventNumber==-1 ? l0EventNumber : Refl0EventNumber;
    m_errors[iq].l0EventNumber[0].set(l0EventNumber,Refl0EventNumber);

    m_errors[iq].board_index[0].set((word>>12)&0xF,m_side*2+iq) ;

    empty = ((word>>28)&0xF);
    if (empty!=0) {
      ++decodingError[iq];
      //std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" A"<<std::endl;
    }

    word = raw[isuwd];
    l0_B_Id       = ((word>>16)&0xFFF);
    m_errors[iq].l0_B_Id[1].set(l0_B_Id,Refl0_B_Id);
    l0EventNumber = ((word>> 0)&0xFFF);
    m_errors[iq].l0EventNumber[1].set(l0EventNumber,Refl0EventNumber);
    m_errors[iq].board_index[1].set((word>>12)&0xF,m_side*2+iq) ;
    empty = ((word>>28)&0xF);
    if (empty!=0) {
      ++decodingError[iq];
      //std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" B"<<std::endl;
    }

    ++icuwd;
    ++isuwd;

    // CU ERROR FIELD
    word = raw[icuwd];
    for (int ib=0; ib<8;++ib){
      m_errors[iq].pb_link_cu[ib].set((word>>(4*(ib)))&0x3);
      empty =((word>>((4*(ib))+2))&0x3);
      if (empty!=0) {
        ++decodingError[iq];
        //std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" C"<<std::endl;
      }

    }
    ++icuwd;
    word = raw[icuwd];
    for (int ib=8; ib<12;++ib){
      m_errors[iq].pb_link_cu[ib].set((word>>(4*(ib-8)))&0x3);
      empty = ((word>>((4*(ib-8))+2))&0x3);
      if (empty!=0) {
        ++decodingError[iq];
        //std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" D"<<std::endl;
      }

    }
    // the 16 ls-bits of this word will be decoded later
    // Get the CU and SU BCIDs from the msb of the next word in the SU frame
    word = raw[isuwd];
    m_errors[iq].cu_bcid.set((word>>4)&0xF,Refl0_B_Id);
    m_errors[iq].su_bcid.set((word>>0)&0xF,Refl0_B_Id);
    m_errors[iq].err_bcid.set((word>>8)&0x1);
    empty = ((word>>9)&0x7F);
    if (empty!=0) {
      ++decodingError[iq];//std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" E"<<std::endl;
    }


    // SU ERROR FIELD
    for (int ib=0; ib<4;++ib){
      m_errors[iq].pb_link_su[ib].set((word>>(16+(4*ib)))&0x3);
      empty =((word>>(16+(4*ib)+2))&0x3);
      if (empty!=0) {
        ++decodingError[iq];
        //std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" F"<<std::endl;
      }

    }
    ++isuwd;
    word = raw[isuwd];
    for (int ib=4; ib<12;++ib){
      m_errors[iq].pb_link_su[ib].set((word>>(4*(ib-4)))&0x3);
      empty =((word>>((4*(ib-4))+2))&0x3);
      if (empty!=0) {
        ++decodingError[iq];//std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" F"<<std::endl;
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
          ++decodingError[iq];//std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" G"<<std::endl;
        }


        ++icuwd;

        word = (raw[icuwd])&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandPT(    ( (word    )&0x7F),1);
        m_candRegHandlerBCSU[iq][ib].setCandColM3( ( (word>> 8)&0x1F),1);
        m_candRegHandlerBCSU[iq][ib].setCandRowM3( ( (word>>13)&0x03),1);

        empty =(word&0x8080);
        if (empty!=0) {
          ++decodingError[iq];//std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" H"<<std::endl;
        }


        word = (raw[icuwd]>>16)&0xFFFF;
        m_errors[iq].bcsus_bcid_cu[ib].set(word&0xF,Refl0_B_Id);

        empty =(word&0xCFF0);
        if (empty!=0) {
          ++decodingError[iq];
          //std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" I"<<std::endl;
        }


        ++icuwd;

        // Decode SU frame
        word = (raw[isuwd])&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandOffM1( ( (word    )&0x0F),0);
        m_candRegHandlerBCSU[iq][ib].setCandOffM2( ( (word>> 4)&0x0F),0);
        m_candRegHandlerBCSU[iq][ib].setCandPU(    ( (word>> 8)&0x03),0);
        m_candRegHandlerBCSU[iq][ib].setCandCharge(( (word>>12)&0x01),0);

        empty =(word&0xEC00);
        if (empty!=0) {
          ++decodingError[iq];
          //std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" J"<<std::endl;
        }


        word = (raw[isuwd]>>16)&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandOffM1( ( (word    )&0x0F),1);
        m_candRegHandlerBCSU[iq][ib].setCandOffM2( ( (word>> 4)&0x0F),1);
        m_candRegHandlerBCSU[iq][ib].setCandPU(    ( (word>> 8)&0x03),1);
        m_candRegHandlerBCSU[iq][ib].setCandCharge(( (word>>12)&0x01),1);

        empty =(word&0xEC00);
        if (empty!=0) {
          ++decodingError[iq];//std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" K"<<std::endl;
        }

        ++isuwd;

        word = (raw[isuwd])&0xFFFF;
        m_errors[iq].bcsus_bcid_su[ib].set(word&0xF,Refl0_B_Id);
        m_candRegHandlerBCSU[iq][ib].setStatus(    ( (word>> 4)&0x0F) );
        // on bit 12 & 13 : crate number -> ignored in decoding

        empty =(word&0xCF00);
        if (empty!=0) {
          ++decodingError[iq];//std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" L"<<std::endl;
        }


      } else {

        // Decode CU frame
        word = (raw[icuwd])&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandPT(    ( (word    )&0x7F),0);
        m_candRegHandlerBCSU[iq][ib].setCandColM3( ( (word>> 8)&0x1F),0);
        m_candRegHandlerBCSU[iq][ib].setCandRowM3( ( (word>>13)&0x03),0);

        empty =(word&0x8080);
        if (empty!=0) {
          ++decodingError[iq];//std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" M"<<std::endl;
        }


        word = (raw[icuwd]>>16)&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandPT(    ( (word    )&0x7F),1);
        m_candRegHandlerBCSU[iq][ib].setCandColM3( ( (word>> 8)&0x1F),1);
        m_candRegHandlerBCSU[iq][ib].setCandRowM3( ( (word>>13)&0x03),1);

        empty =(word&0x8080);
        if (empty!=0) {
          ++decodingError[iq];//std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" N"<<std::endl;
        }


        ++icuwd;

        word = (raw[icuwd])&0xFFFF;
        m_errors[iq].bcsus_bcid_cu[ib].set(word&0xF,Refl0_B_Id);

        empty =(word&0xCFF0);
        if (empty!=0) {
          ++decodingError[iq];//std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" O"<<std::endl;
        }


        // Decode SU frame
        word = (raw[isuwd]>>16)&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandOffM1( ( (word    )&0x0F),0);
        m_candRegHandlerBCSU[iq][ib].setCandOffM2( ( (word>> 4)&0x0F),0);
        m_candRegHandlerBCSU[iq][ib].setCandPU(    ( (word>> 8)&0x03),0);
        m_candRegHandlerBCSU[iq][ib].setCandCharge(( (word>>12)&0x01),0);

        empty =(word&0xEC00);
        if (empty!=0) {
          ++decodingError[iq];//std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" P"<<std::endl;
        }


        ++isuwd;

        word = (raw[isuwd])&0xFFFF;
        m_candRegHandlerBCSU[iq][ib].setCandOffM1( ( (word    )&0x0F),1);
        m_candRegHandlerBCSU[iq][ib].setCandOffM2( ( (word>> 4)&0x0F),1);
        m_candRegHandlerBCSU[iq][ib].setCandPU(    ( (word>> 8)&0x03),1);
        m_candRegHandlerBCSU[iq][ib].setCandCharge(( (word>>12)&0x01),1);

        empty =(word&0xEC00);
        if (empty!=0) {
          ++decodingError[iq];//std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" Q"<<std::endl;
        }


        word = (raw[isuwd]>>16)&0xFFFF;
        m_errors[iq].bcsus_bcid_su[ib].set(word&0xF,Refl0_B_Id);
        m_candRegHandlerBCSU[iq][ib].setStatus(    ( (word>> 4)&0x0F) );
        // on bit 12 & 13 : crate number -> ignored in decoding

        empty =(word&0xCF00);
        if (empty!=0) {
          ++decodingError[iq];//std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" S"<<std::endl;
        }


        ++isuwd;

      }

      int status = m_candRegHandlerBCSU[iq][ib].getStatus();
      m_errors[iq].bcsus_status[ib].set(status);

      int ncand = m_candRegHandlerBCSU[iq][ib].numberOfCandidates();
      for( int icand =0; icand<ncand;++icand){
        m_candRegHandlerBCSU[iq][ib].setCandBoard(ib,icand);
        m_candRegHandlerBCSU[iq][ib].setCandQuarter(m_side*2+iq,icand);
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
      ++decodingError[iq];
      //std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" V"<<std::endl;
    }

    ++icuwd;

    word = (raw[icuwd])&0xFFFF;
    m_candRegHandler[iq].setCandPT(    ( (word    )&0x7F),1);
    m_candRegHandler[iq].setCandColM3( ( (word>> 8)&0x1F),1);
    m_candRegHandler[iq].setCandRowM3( ( (word>>13)&0x03),1);

    empty =(word&0x8080);
    if (empty!=0) {
      ++decodingError[iq];//std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" W"<<std::endl;
    }


    word = (raw[icuwd]>>16)&0xFFFF;
    m_errors[iq].cu_bcid_best.set(word&0xF,Refl0_B_Id);

    empty =(word&0xFFF0);
    if (empty!=0) {
      ++decodingError[iq];//std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" X"<<std::endl;
    }


    ++icuwd;

    empty = (raw[icuwd])&0xFFFFFFFF;
    if (empty!=0) {
      ++decodingError[iq];//std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" Y"<<std::endl;
    }


    // Decode SU frame
    word = (raw[isuwd])&0xFFFF;
    m_candRegHandler[iq].setCandOffM1( ( (word    )&0x0F),0);
    m_candRegHandler[iq].setCandOffM2( ( (word>> 4)&0x0F),0);
    m_candRegHandler[iq].setCandPU(    ( (word>> 8)&0x03),0);
    m_candRegHandler[iq].setCandBoard( ( (word>>12)&0x0F),0);

    empty =(word&0x0C00);
    if (empty!=0) {
      ++decodingError[iq];//std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" Z"<<std::endl;
    }


    word = (raw[isuwd]>>16)&0xFFFF;
    m_candRegHandler[iq].setCandOffM1( ( (word    )&0x0F),1);
    m_candRegHandler[iq].setCandOffM2( ( (word>> 4)&0x0F),1);
    m_candRegHandler[iq].setCandPU(    ( (word>> 8)&0x03),1);
    m_candRegHandler[iq].setCandBoard( ( (word>>12)&0x0F),1);

    empty =(word&0x0C00);
    if (empty!=0) {
      ++decodingError[iq];//std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" AA"<<std::endl;
    }


    ++isuwd;

    word = (raw[isuwd])&0xFFFF;
    m_errors[iq].su_bcid_best.set(word&0xF,Refl0_B_Id);
    m_candRegHandler[iq].setStatus(    ( (word>> 4)&0x0F) );
    m_candRegHandler[iq].setCandCharge(( (word>> 8)&0x01),1);
    m_candRegHandler[iq].setCandCharge(( (word>>12)&0x01),0);

    empty =(word&0xEE00);
    if (empty!=0) {
      ++decodingError[iq];
      //std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" AB"<<std::endl;
    }

    empty = (raw[isuwd]>>16)&0xFFFF;
    if (empty!=0) {
      ++decodingError[iq];
      //std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" AC"<<std::endl;
    }


    int status =m_candRegHandler[iq].getStatus();
    m_errors[iq].su_status.set(status);

    int ncand = m_candRegHandler[iq].numberOfCandidates();
    for( int icand =0; icand<ncand;++icand) {
      m_candRegHandler[iq].setCandQuarter(m_side*2+iq,icand);
    }

    if (decodingError[iq]>0){
      m_candRegHandler[iq].clear();
      for (int ib = 0; ib <12 ; ++ib) {
        m_candRegHandlerBCSU[iq][ib].clear();
      }
    }

  }// End of loop over quarters

//   // temporary check
//   std::vector<unsigned int> tmp;
//   rawBank(tmp,bankVersion);
//   for (int iq=0; iq<2; ++iq){
//     for (unsigned int i=0; i<L0Muon::CtrlRawCnv::board_full_data_size;++i){
//       unsigned int iw = iq*L0Muon::CtrlRawCnv::board_full_data_size+i;
//       if (tmp[iw]!=original[iw]) {
//         if (decodingError[iq]==0){
//           ++decodingError[iq];
// //           std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" AE"<<std::endl;
// //           std::cout<<"\t !!! L0Muon::CtrlRawCnv::decodeBank !!! ERROR !!! line "<<iw<<" Q"<<m_side*2+iq
// //                    <<std::hex<<" rebuild= 0x"<<tmp[iw]<<" VS original= 0x"<<original[iw]<<std::dec<<std::endl;
//         }
//       }
//     }
//   }

  for (int iq=0; iq<2; ++iq) m_errors[iq].decoding.set(decodingError[iq]);

  // Reset registers of a board if a decoding error has been detected
  for (int iq=0; iq<2; ++iq){
    if (decodingError[iq]>0) {
      m_candRegHandler[iq].clear();
      for (int ib = 0; ib<12; ++ib) {
        m_candRegHandlerBCSU[iq][ib].clear();
      }
      //std::cout<<"\tL0Muon::CtrlRawCnv::decodeBank !!! DECODING ERROR !!! Q"<<m_side*2+iq<<std::endl;
    }
  }
  ++m_n_decoded_banks;
  return;
}

void L0Muon::CtrlRawCnv::decodeBank_v2(const std::vector<unsigned int> &raw, int &Refl0EventNumber, int &Refl0_B_Id){

  unsigned int word;
  unsigned int empty;

  // temporary !!!
  std::vector<unsigned int> original = raw;

  // Clear the registers first
  release();
  m_activ = true;

  int decodingError[2];
  for (int iq=0; iq<2; ++iq){
    decodingError[iq]=0;
  }

  // =========================
  // TELL1 NON-COMPRESSED BANK
  // =========================

  //   //   unsigned int header = raw[0];

  int iwd=0;

  // 1st Loop over quarters
  for (int iq=0; iq<2; ++iq) {
    int l0_B_Id, l0EventNumber;

    // first word contains evt num & crate index
    word = raw[iwd];
    l0EventNumber = ((word>> 0)&0xFFF);
    Refl0EventNumber  = Refl0EventNumber==-1 ? l0EventNumber : Refl0EventNumber;
    m_errors[iq].l0EventNumber[0].set(l0EventNumber,Refl0EventNumber);
    l0EventNumber = ((word>>16)&0xFFF);
    m_errors[iq].l0EventNumber[1].set(l0EventNumber,Refl0EventNumber);

    m_errors[iq].board_index[0].set((word>>12)&0x3,m_side*2+iq) ;
    m_errors[iq].board_index[1].set((word>>28)&0x3,m_side*2+iq) ;

    empty = ((word>>14)&0x3);
    if (empty!=0) {
      ++decodingError[iq];
#if _DEBUG_RAWDATA_==1
      std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" A"<<std::endl;
#endif
    }
    empty = ((word>>30)&0x3);
    if (empty!=0) {
      ++decodingError[iq];
#if _DEBUG_RAWDATA_==1
      std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" B"<<std::endl;
#endif
    }

    ++iwd;

    // 2nd word contains L0BID
    word = raw[iwd];
    l0_B_Id       = ((word>>0)&0xFFF);
    Refl0_B_Id  = Refl0_B_Id==-1 ? l0_B_Id : Refl0_B_Id;
    m_errors[iq].l0_B_Id[0].set(l0_B_Id,Refl0_B_Id);
    l0_B_Id       = ((word>>16)&0xFFF);
    m_errors[iq].l0_B_Id[1].set(l0_B_Id,Refl0_B_Id);

    empty = ((word>>14)&0x3);
    if (empty!=0) {
      ++decodingError[iq];
#if _DEBUG_RAWDATA_==1
      std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" C"<<std::endl;
#endif
    }
    empty = ((word>>30)&0x3);
    if (empty!=0) {
      ++decodingError[iq];
#if _DEBUG_RAWDATA_==1
      std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" D"<<std::endl;
#endif
    }

    ++iwd;

    // Next 2 words are candidates best pair
    for (int icand=0; icand<2; ++icand){
      word = raw[iwd];
      m_candRegHandler[iq].setCandOffM1( ( (word    )&0x0F),icand);
      m_candRegHandler[iq].setCandOffM2( ( (word>> 4)&0x0F),icand);
      m_candRegHandler[iq].setCandPU(    ( (word>> 8)&0x03),icand);
      m_candRegHandler[iq].setCandBoard( ( (word>>12)&0x0F),icand);
      m_candRegHandler[iq].setCandPT(    ( (word>>16)&0x7F),icand);
      m_candRegHandler[iq].setCandColM3( ( (word>>24)&0x1F),icand);
      m_candRegHandler[iq].setCandRowM3( ( (word>>29)&0x03),icand);

      empty =(word&0x80800C00);
      if (empty!=0) {
        ++decodingError[iq];
#if _DEBUG_RAWDATA_==1
        std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" E"<<std::endl;
#endif
      }

      ++iwd;
    }

    // Next word : charge , status & bcid
    word=raw[iwd];
    m_errors[iq].su_bcid_best.set(word&0xF,Refl0_B_Id);
    m_candRegHandler[iq].setStatus(    ( (word>> 4)&0x0F) );
    m_candRegHandler[iq].setCandCharge(( (word>> 8)&0x01),1);
    m_candRegHandler[iq].setCandCharge(( (word>>12)&0x01),0);
    m_errors[iq].cu_bcid_best.set((word>>16)&0xF,Refl0_B_Id);

    empty =(word&0xFFF0EE00);
    if (empty!=0) {
      ++decodingError[iq];
#if _DEBUG_RAWDATA_==1
      std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" F"<<std::endl;
#endif
    }

    int status =m_candRegHandler[iq].getStatus();
    m_errors[iq].su_status.set(status);

    int ncand = m_candRegHandler[iq].numberOfCandidates();
    //std::cout<<"L0Muon::CtrlRawCnv::decodeBank ncand= "<<ncand<<" Q"<<m_side*2+iq<<std::endl;
    for( int icand =0; icand<ncand;++icand) {
      m_candRegHandler[iq].setCandQuarter(m_side*2+iq,icand);
    }

    ++iwd;

    // Next word : bcids CU & SU
    word=raw[iwd];
    m_errors[iq].su_bcid.set((word>>0)&0xF,Refl0_B_Id);
    m_errors[iq].cu_bcid.set((word>>4)&0xF,Refl0_B_Id);
    m_errors[iq].err_bcid.set((word>>8)&0x1);

    empty =(word&0xFFFFFE00);
    if (empty!=0) {
      ++decodingError[iq];
#if _DEBUG_RAWDATA_==1
      std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" G"<<std::endl;
#endif
    }

    ++iwd;


  } // End of 1st loop over quarters

  // 2nd Loop over quarters
  for (int iq=0; iq<2; ++iq) {

    // Next 3 words : link errors
    for (int i=0; i<3; ++i){
      word=raw[iwd];
      for (int ii=0; ii<4; ++ii){
        int ib=i*4+ii;
        m_errors[iq].pb_link_su[ib].set((word>>(32-(4*(ii+1))))&3);
        m_errors[iq].pb_link_cu[ib].set((word>>(16-(4*(ii+1))))&3);
      }
      empty =(word&0xCCCCCCCC);
      if (empty!=0) {
        ++decodingError[iq];
#if _DEBUG_RAWDATA_==1
        std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" H"<<std::endl;
#endif
      }
      ++iwd;
    }

    // Next words : PB candidates
    for (int ib=0; ib<12; ++ib) {
      // Adresses & PT
      for (int icand=0; icand<2; ++icand){
        word = raw[iwd];
        m_candRegHandlerBCSU[iq][ib].setCandOffM1( ( (word    )&0x0F),icand);
        m_candRegHandlerBCSU[iq][ib].setCandOffM2( ( (word>> 4)&0x0F),icand);
        m_candRegHandlerBCSU[iq][ib].setCandPU(    ( (word>> 8)&0x03),icand);
        m_candRegHandlerBCSU[iq][ib].setCandCharge(( (word>>12)&0x01),icand);
        m_candRegHandlerBCSU[iq][ib].setCandPT(    ( (word>>16)&0x7F),icand);
        m_candRegHandlerBCSU[iq][ib].setCandColM3( ( (word>>24)&0x1F),icand);
        m_candRegHandlerBCSU[iq][ib].setCandRowM3( ( (word>>29)&0x03),icand);

        empty =(word&0x8080EC00);
        if (empty!=0) {
          ++decodingError[iq];
#if _DEBUG_RAWDATA_==1
          std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" I"<<std::endl;
#endif
        }

        ++iwd;
      }

      // BCID, charge & status
      word = raw[iwd];
      m_errors[iq].bcsus_bcid_su[ib].set(word&0xF,Refl0_B_Id);
      m_candRegHandlerBCSU[iq][ib].setStatus(    ( (word>> 4)&0x0F) );
      m_errors[iq].bcsus_bcid_cu[ib].set((word>>16)&0xF,Refl0_B_Id);

      empty =(word&0xCFF0CF00);
      if (empty!=0) {
        ++decodingError[iq];
#if _DEBUG_RAWDATA_==1
        std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" J"<<std::endl;
#endif
      }
      ++iwd;

      int status =m_candRegHandler[iq].getStatus();
      m_errors[iq].su_status.set(status);

      int ncand = m_candRegHandler[iq].numberOfCandidates();
      //std::cout<<"L0Muon::CtrlRawCnv::decodeBank ncand= "<<ncand<<" Q"<<m_side*2+iq<<" ib= "<<ib<<std::endl;
      for( int icand =0; icand<ncand;++icand) {
        m_candRegHandlerBCSU[iq][ib].setCandBoard(ib,icand);
        m_candRegHandlerBCSU[iq][ib].setCandQuarter(m_side*2+iq,icand);
      }
    }

    // Next word : unused
    ++iwd;

  }// End of 2nd loop over quarters

//   // temporary check
//   std::vector<unsigned int> tmp;
//   rawBank(tmp,bankVersion);
//   for (int iq=0; iq<2; ++iq){
//     for (unsigned int i=0; i<L0Muon::CtrlRawCnv::board_full_data_size;++i){
//       unsigned int iw = iq*L0Muon::CtrlRawCnv::board_full_data_size+i;
//       if (tmp[iw]!=original[iw]) {
//         if (decodingError[iq]==0){
//           ++decodingError[iq];
// //           std::cout<<"L0Muon::CtrlRawCnv::decodeBank decodingError Q"<<m_side*2+iq<<" AE"<<std::endl;
// //           std::cout<<"\t !!! L0Muon::CtrlRawCnv::decodeBank !!! ERROR !!! line "<<iw<<" Q"<<m_side*2+iq
// //                    <<std::hex<<" rebuild= 0x"<<tmp[iw]<<" VS original= 0x"<<original[iw]<<std::dec<<std::endl;
//         }
//       }
//     }
//   }

  for (int iq=0; iq<2; ++iq) m_errors[iq].decoding.set(decodingError[iq]);

  // Reset registers of a board if a decoding error has been detected
  for (int iq=0; iq<2; ++iq){
    if (decodingError[iq]>0) {
      m_candRegHandler[iq].clear();
      for (int ib = 0; ib<12; ++ib) {
        m_candRegHandlerBCSU[iq][ib].clear();
      }
#if _DEBUG_RAWDATA_==1
      std::cout<<"\tL0Muon::CtrlRawCnv::decodeBank !!! DECODING ERROR !!! Q"<<m_side*2+iq<<std::endl;
#endif
    }
  }

  ++m_n_decoded_banks;

  return;

}

// void L0Muon::CtrlRawCnv::setErrors(int L0EventNum, int L0_B_Id){
//   for (int iq=0; iq<2; ++iq){
//     for (int ich=0; ich<2; ++ich){
//       m_errors[iq].board_index[ich].set(ib,ib);
//       m_errors[iq].l0EventNumber[ich].set(l0EventNum,l0EventNum);
//       m_errors[iq].l0_B_Id[ich].set(l0_B_Id,l0_B_Id);
//     }
//     m_errors[iq].su_bcid.set(l0_B_Id,l0_B_Id);
//     m_errors[iq].cu_bcid.set(l0_B_Id,l0_B_Id);
//     for (int ipb=0; ipb<12; ++ipb){
//       m_errors[iq].bcsus_bcid_su[ipb].set(l0_B_Id,l0_B_Id);
//       m_errors[iq].bcsus_bcid_cu[ipb].set(l0_B_Id,l0_B_Id);
//     }
//   }
// }

void L0Muon::CtrlRawCnv::rawBank( std::vector<unsigned int> &raw, int bankVersion){
  int event_number;
  int l0_bid;
  int board_index;
  unsigned int event_num_word;
  int bcid;

  if (bankVersion<1) return ;

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

    event_number   = m_errors[iq].l0EventNumber[CU].value();
    l0_bid         = m_errors[iq].l0_B_Id[CU].value();
    board_index    = m_errors[iq].board_index[CU].value();
    event_num_word = ((l0_bid<<16)&0xFFF0000 ) + ((board_index<<12)&0xF000) + ( event_number&0xFFF );
    raw.push_back(event_num_word);// raw[(iq*44)+ 0 ]

    word=0;
    word+= ( ((m_errors[iq].pb_link_cu[7].value())<<28)&0x30000000);
    word+= ( ((m_errors[iq].pb_link_cu[6].value())<<24)&0x03000000);
    word+= ( ((m_errors[iq].pb_link_cu[5].value())<<20)&0x00300000);
    word+= ( ((m_errors[iq].pb_link_cu[4].value())<<16)&0x00030000);
    word+= ( ((m_errors[iq].pb_link_cu[3].value())<<12)&0x00003000);
    word+= ( ((m_errors[iq].pb_link_cu[2].value())<< 8)&0x00000300);
    word+= ( ((m_errors[iq].pb_link_cu[1].value())<< 4)&0x00000030);
    word+= ( ((m_errors[iq].pb_link_cu[0].value())<< 0)&0x00000003);
    raw.push_back(word);// raw[(iq*44)+ 1 ]

    word=0;
    word+= ( ((m_errors[iq].pb_link_cu[11].value())<<12)&0x00003000);
    word+= ( ((m_errors[iq].pb_link_cu[10].value())<< 8)&0x00000300);
    word+= ( ((m_errors[iq].pb_link_cu[ 9].value())<< 4)&0x00000030);
    word+= ( ((m_errors[iq].pb_link_cu[ 8].value())<< 0)&0x00000003);

    // Loop over BCSU candidates
    for (int ib = 0; ib <12 ; ++ib) {
      for (int icand=0; icand<2; ++icand) {
        int cand_pt    = m_candRegHandlerBCSU[iq][ib].getCandPT(icand);
        int cand_colM3 = m_candRegHandlerBCSU[iq][ib].getCandColM3(icand);
        int cand_rowM3 = m_candRegHandlerBCSU[iq][ib].getCandRowM3(icand);
        cand[icand] = ( ( (cand_rowM3<<13)&0x6000 ) + ( (cand_colM3<<8)&0x1F00 ) + ( (cand_pt)&0x7F ) );
      }

      bcid = m_errors[iq].bcsus_bcid_cu[ib].value();
      if (ib%2==0) {
        word |= ( (cand[0]<<16) & 0x7FFFFFFF );
        raw.push_back(word);
        word = ( ((board_index<<28) & 0x30000000) +( (bcid<<16)&0xF0000 ) +  (cand[1]&0x7FFF) );
        raw.push_back(word);
        word = 0;
      } else {
        word = ( ( (cand[1]<<16) & 0x7FFF0000 ) +   (cand[0]&0x7FFF) );
        raw.push_back(word);
        word =( ((board_index<<12) & 0x3000) + (bcid&0xF) ) ;
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
    bcid = m_errors[iq].cu_bcid_best.value();
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

    event_number   = m_errors[iq].l0EventNumber[SU].value();
    l0_bid         = m_errors[iq].l0_B_Id[SU].value();
    board_index    = m_errors[iq].board_index[SU].value();
    event_num_word = ((l0_bid<<16)&0xFFF0000 ) + ((board_index<<12)&0xF000) + ( event_number&0xFFF );
    raw.push_back(event_num_word);// raw[(iq*44) + 22 + 0 ]

    word = ( ( (m_errors[iq].err_bcid.value()<<8)&0x1 )
             + ( (m_errors[iq].cu_bcid.value()<<4)&0xF0 )
             + ( m_errors[iq].cu_bcid.value()&0xF ) );
    word+= ( ((m_errors[iq].pb_link_su[3].value()<<28))&0x30000000);
    word+= ( ((m_errors[iq].pb_link_su[2].value()<<24))&0x03000000);
    word+= ( ((m_errors[iq].pb_link_su[1].value()<<20))&0x00300000);
    word+= ( ((m_errors[iq].pb_link_su[0].value()<<16))&0x00030000);

    raw.push_back(word);// raw[(iq*44) + 22 + 1 ]

    word=0;
    word+= ( ((m_errors[iq].pb_link_su[11].value())<<28)&0x30000000);
    word+= ( ((m_errors[iq].pb_link_su[10].value())<<24)&0x03000000);
    word+= ( ((m_errors[iq].pb_link_su[ 9].value())<<20)&0x00300000);
    word+= ( ((m_errors[iq].pb_link_su[ 8].value())<<16)&0x00030000);
    word+= ( ((m_errors[iq].pb_link_su[ 7].value())<<12)&0x00003000);
    word+= ( ((m_errors[iq].pb_link_su[ 6].value())<< 8)&0x00000300);
    word+= ( ((m_errors[iq].pb_link_su[ 5].value())<< 4)&0x00000030);
    word+= ( ((m_errors[iq].pb_link_su[ 4].value())<< 0)&0x00000003);
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
      bcid       = m_errors[iq].bcsus_bcid_su[ib].value();
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
    bcid       = m_errors[iq].su_bcid_best.value();
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

}

