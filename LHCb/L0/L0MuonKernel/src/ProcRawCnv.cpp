#include "L0MuonKernel/ProcRawCnv.h"

#include "Kernel/MuonLayout.h"

#include "ProcessorKernel/RegisterFactory.h"
#include "ProcessorKernel/Register.h"

#include "L0MuonKernel/BankUtilities.h"

#include "L0MuonKernel/ProcRawErrors.h"


/**
   Constructor.
*/
L0Muon::ProcRawCnv::ProcRawCnv(){

  m_activ = false;
  m_quarter=0;
  m_n_decoded_banks=0;
};
/**
   Constructor.
*/
L0Muon::ProcRawCnv::ProcRawCnv(int quarter){

  m_activ = false;  
  m_quarter=quarter;
  m_n_decoded_banks=0;  

  char buf[4096];
  char* format ;
  
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

  for (int iboard = 0; iboard <12 ; iboard++) {
    // Candidate Register handlers for BCSU candidates
    format = "BCSUCAND_Q%d_%d";
    sprintf(buf,format,m_quarter+1,iboard);
    //    L0Muon::Register* reg_board = rfactory->createRegister(buf,L0Muon::BitsCandRegTot);
    L0Muon::Register* reg_board = rfactory->createRegister(buf,CandRegisterHandler::size());
    m_candRegHandlerBCSU[iboard] = CandRegisterHandler(reg_board) ;
    for (int ipu = 0; ipu <4 ; ipu++) {
      // Candidate Register handlers for PU candidates
      format = "PUCAND_Q%d_%d_%d";
      sprintf(buf,format,m_quarter+1,iboard,ipu);
      //       L0Muon::Register* reg_pu = rfactory->createRegister(buf,L0Muon::BitsCandRegTot);
      L0Muon::Register* reg_pu = rfactory->createRegister(buf,CandRegisterHandler::size());
      m_candRegHandlerPU[iboard][ipu] = CandRegisterHandler(reg_pu) ;
    }
  }

  for (int iboard = 0; iboard <12 ; iboard++) {
    for (int ipu = 0; ipu <4 ; ipu++) {

      format = "FORMATTED_OL_Q%d_%d_%d";
      sprintf(buf,format,m_quarter+1,iboard,ipu);
      TileRegister* ol = rfactory->searchTileRegister(buf);
      m_ols[iboard][ipu] = ol;
      
      format = "FORMATTED_NEIGH_Q%d_%d_%d";
      sprintf(buf,format,m_quarter+1,iboard,ipu);
      TileRegister* neigh = rfactory->searchTileRegister(buf);
      m_neighs[iboard][ipu] = neigh;

    }
  }
  
  std::vector<LHCb::MuonTileID> lpus = MuonLayout(2,2).tiles(m_quarter);
  for (std::vector<LHCb::MuonTileID>::iterator ip=lpus.begin(); ip<lpus.end(); ++ip) {

    format = "FORMATTED_OL_Q%dR%d%d%d";
    sprintf(buf,format,m_quarter+1,(*ip).region()+1,(*ip).nX(),(*ip).nY());
    TileRegister* ol = rfactory->searchTileRegister(buf);
    m_olsMap[(*ip)] = ol;
    
    format = "FORMATTED_NEIGH_Q%dR%d%d%d";
    sprintf(buf,format,m_quarter+1,(*ip).region()+1,(*ip).nX(),(*ip).nY());
    TileRegister* neigh = rfactory->searchTileRegister(buf);
    m_neighsMap[(*ip)] = neigh;
 
  }

}


/**
   Destructor
*/
L0Muon::ProcRawCnv::~ProcRawCnv(){
}

LHCb::MuonTileID L0Muon::ProcRawCnv::mid_BCSU(int ib){
  MuonLayout lay(1,1);
  LHCb::MuonTileID board = LHCb::MuonTileID(0);
  board.setQuarter(m_quarter);
  board.setLayout(lay);
  board.setRegion(ib/3);
  board.setX( (((ib%3)+1)>>0)&1);
  board.setY( (((ib%3)+1)>>1)&1);
  return board;
}

LHCb::MuonTileID L0Muon::ProcRawCnv::mid_PU(int ib, int ipu){
  LHCb::MuonTileID board = mid_BCSU(ib);
  MuonLayout lay(2,2);
  LHCb::MuonTileID pu = LHCb::MuonTileID(board,lay,(ipu>>0)&1,(ipu>>1)&1);
  return pu;
}


void L0Muon::ProcRawCnv::release(){
  for (int ib = 0; ib <12 ; ib++) {
    m_candRegHandlerBCSU[ib].clear();
    for (int ipu = 0; ipu <4 ; ipu++) {
      m_candRegHandlerPU[ib][ipu].clear();
    }
  }
  for (int iboard = 0; iboard <12 ; iboard++) {
    for (int ipu = 0; ipu <4 ; ipu++) { 
      if (m_ols[iboard][ipu]!=0) m_ols[iboard][ipu]->reset();
      if (m_neighs[iboard][ipu]!=0) m_neighs[iboard][ipu]->reset();
    }    
  }
  m_activ = false;
  
}

std::vector<L0Muon::PMuonCandidate>  L0Muon::ProcRawCnv::muonCandidatesPU(){
  std::vector<L0Muon::PMuonCandidate> cands;
  for (int ib = 0; ib<12 ; ++ib){
    for (int ipu= 0; ipu<4 ;++ipu) {
      //       int ncand= m_candRegHandlerPU[ib][ipu].numberOfCandidates();
      int status =m_candRegHandlerPU[ib][ipu].getStatus();
      int ncand = status&0x3;
      ncand = ncand<3 ? ncand : 2;
      for (int icand = 0;icand<ncand;icand++) {
        cands.push_back(m_candRegHandlerPU[ib][ipu].getMuonCandidate(icand));
      }    
    }
  }
  
  return cands;
}

std::vector<L0Muon::PMuonCandidate>  L0Muon::ProcRawCnv::muonCandidatesBCSU(){
  std::vector<L0Muon::PMuonCandidate> cands;
  for (int ib = 0; ib<12 ; ++ib){
    //     int ncand= m_candRegHandlerBCSU[ib].numberOfCandidates();
    int status =m_candRegHandlerBCSU[ib].getStatus();
    int ncand = status&0x3;
    ncand = ncand<3 ? ncand : 2;
    for (int icand = 0;icand<ncand;icand++) {
      cands.push_back(m_candRegHandlerBCSU[ib].getMuonCandidate(icand));
    }    
  }
  
  return cands;
}

std::vector<LHCb::MuonTileID> L0Muon::ProcRawCnv::ols(LHCb::MuonTileID puid)
{
  std::vector<LHCb::MuonTileID> pads;

//   std::cout <<"L0Muon::ProcRawCnv::ols input PU "<<puid.toString()<<std::endl;
  
  std::map<LHCb::MuonTileID, TileRegister*>::iterator it = m_olsMap.find(puid);
  if (it!=m_olsMap.end()) {
    if ((*it).second!=0) {
      pads = ((*it).second)->firedTiles();
    } else {
      std::cout <<"L0Muon::ProcRawCnv::ols no tile register associated"<<std::endl;
    } 
  } else {
    std::cout <<"L0Muon::ProcRawCnv::ols PU not found in ol map"<<std::endl;
  } 
  
//   if ( (puid.quarter()==2 && puid.region()==3 && puid.nX()==1 && puid.nY()==2) ||
//        (puid.quarter()==2 && puid.region()==3 && puid.nX()==0 && puid.nY()==2) ||
//        (puid.quarter()==2 && puid.region()==3 && puid.nX()==1 && puid.nY()==3) ||
//        (puid.quarter()==2 && puid.region()==3 && puid.nX()==0 && puid.nY()==3)) {
//     std::cout <<"L0Muon::ProcRawCnv::ols "<<pads.size()<<" pads found"<<std::endl;
//     for (std::vector<LHCb::MuonTileID>::iterator itp=pads.begin();itp<pads.end();++itp){
//       std::cout <<"L0Muon::ProcRawCnv::ols    tile : "<<itp->toString();
//       if (!itp->isValid()) std::cout <<" NOT VALID *****";
//       std::cout<<std::endl;
//     } 
//   }
  
  //   pads = ((*it).second)->firedTiles();
  //   std::cout <<"L0Muon::ProcRawCnv::ols "<<pads.size()<<" pads found"<<std::endl;
  return pads;
}

std::vector<LHCb::MuonTileID> L0Muon::ProcRawCnv::neighs(LHCb::MuonTileID puid)   
{
  std::vector<LHCb::MuonTileID> pads;
  std::map<LHCb::MuonTileID, TileRegister*>::iterator it = m_neighsMap.find(puid);
  if (it!=m_neighsMap.end()) {
    if ((*it).second!=0) {
      pads = ((*it).second)->firedTiles();
    }    
  }
  
  return pads;
}

// int L0Muon::ProcRawCnv::checkMuonTiles(){
//   int nBadTiles=0;
//   std::map<LHCb::MuonTileID, TileRegister*>::iterator itregmap;
  
//   //   for (itregmap=m_olsMap.begin(); itregmap!=m_olsMap.end(); ++itregmap){
//   //   }

//   return nBadTiles;
// }

std::vector<LHCb::MuonTileID>  L0Muon::ProcRawCnv::pus()
{
  std::vector<LHCb::MuonTileID> lpuids;
  std::map<LHCb::MuonTileID, TileRegister*>::iterator itmap;
  for (itmap=m_olsMap.begin(); itmap!=m_olsMap.end(); ++itmap){
    lpuids.push_back(itmap->first);
  }
  
  return lpuids;
}
    
void L0Muon::ProcRawCnv::dump(const std::vector<unsigned int> &raw) 
{

  std::cout <<"__ "<<" - Q"<<m_quarter+1<<" ; bank size: "<<raw.size()<<std::endl;
    
  std::cout <<"__ ";
  std::cout <<"  ";
  for (int ib=0; ib<12; ++ib) {
    std::cout <<"     "<<std::setw(2)<<ib<<"    ";
  }
  std::cout <<std::endl;

  std::cout.setf(std::ios::uppercase) ;

  for (unsigned int iline=0; iline<L0Muon::ProcRawCnv::board_full_data_size; ++iline) {
    
    std::cout <<"__ ";
    std::cout <<std::setw(2)<<iline;
    std::cout<<std::hex;
    for (int ib=0; ib<12; ++ib) {
      std::cout <<" 0x"<<std::setw(8)<<std::setfill('0')<<raw[L0Muon::ProcRawCnv::board_full_data_size*ib+iline];
    }
    std::cout<<std::dec;
    std::cout <<std::endl;
  }
  
  std::cout <<"__ ----"<<std::endl;  
  std::cout.unsetf(std::ios::uppercase);
}

void L0Muon::ProcRawCnv::dumpErrorCounters(std::string &os) {
  int sum = 0;
  for (int ib=0; ib<12; ++ib) sum+=m_errors[ib].sumCounters();  
  os+=(boost::format("%d errors found\n") % sum ).str();
  
  for (int ib=0; ib<12; ++ib) 
    m_errors[ib].printCounters( os , (boost::format("\tPB%2d ") % ib ).str());
}


void L0Muon::ProcRawCnv::decodeBank(const std::vector<unsigned int> &original, int bankVersion,
                                    int &Refl0EventNumber, int &Refl0_B_Id){
  
  if (bankVersion<1) return;

  unsigned int word;
  unsigned int empty;

  // //  Dump bank   
  //   std::cout.setf(std::ios::uppercase) ;
  //   std::cout<<"\t=> L0Muon::ProcRawCnv::decodeBank -- dump raw bank of size: "<<raw.size()<<std::endl;
  //   int icount=0;
  //   for (std::vector<unsigned int>::iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
  //     std::cout <<std::setw(3)<<icount<<std::hex<<"\t0x"<<std::setw(8)<<std::setfill('0')<<(*itraw)<<std::dec<<std::endl;
  //     ++icount;
  //   }
  //   std::cout<<std::dec;
  //   std::cout.unsetf(std::ios::uppercase);
 

  // Clear the registers first
  release();
  m_activ = true;
  
  int decodingError[12];
  for (int ib=0; ib<12; ++ib){
    decodingError[ib]=0;
  }
  
  // Re-order the bank such that the second frame of each processing board is read first
  std::vector<unsigned int> raw;
  reorderOLChannels(original,raw);
  
  // Remove  words at the beginning and end of each frame
  std::vector<unsigned int>::iterator itraw;

  // Remove the empty word at the end of each frame
  for (int ifr=24; ifr>0; --ifr) {
    int pos = ifr*L0Muon::ProcRawCnv::board_full_frame_size-1;
    empty = raw[pos];
    
    if (empty!=0) {
      int ib = (ifr-1)/2;
      ++decodingError[ib];
      std::cout << "L0Muon::ProcRawCnv::decodeBank decodingError ib= "<<ib<<" I "<<std::endl;
      std::cout <<" ifr= "<<ifr<<" pos= "<<pos<<" wd=0x"<<std::hex<<empty<<std::dec<<std::endl;
    }
    itraw=raw.begin()+pos;
    raw.erase(itraw);
  }
  
  // Get and Remove the l0EventNumber and the l0_B_Id at the beginning of each frame
  for (int ib=11; ib>-1; --ib) {
    for (int ich=1; ich>-1; --ich){
      int pos = (ib*2+ich)*(L0Muon::ProcRawCnv::board_full_frame_size-1);
      word = raw[pos];
      empty                    = ((word>>28)&0xF);
      if (empty!=0) {
        ++decodingError[ib];
        std::cout << "L0Muon::ProcRawCnv::decodeBank decodingError ib= "<<ib<<" A"<<std::endl;
      }
    
      int l0_B_Id = ((word>> 0)&0xFFF);
      Refl0_B_Id  = Refl0_B_Id==-1 ? l0_B_Id : Refl0_B_Id;
      m_errors[ib].l0_B_Id[ich].set(l0_B_Id,Refl0_B_Id);
  
      int l0EventNumber = ((word>>16)&0xFFF);
      Refl0EventNumber  = Refl0EventNumber==-1 ? l0EventNumber : Refl0EventNumber;      
      m_errors[ib].l0EventNumber[ich].set(l0EventNumber,Refl0EventNumber);
      
      m_errors[ib].board_index[ich].set(((word>>12)&0xF),ib);

      itraw=raw.begin()+pos;
      raw.erase(itraw);
  
    }
  }

  // Loop over processing boards 
  for (int ind=0; ind<12; ++ind) {

    int ib = ind;

    // BCSU candidates :
    word = raw[ib*L0Muon::ProcRawCnv::board_data_size + 0 ];
    m_candRegHandlerBCSU[ib].setCandOffM1( ( (word>> 0)&0x0F ) ,0);
    m_candRegHandlerBCSU[ib].setCandOffM2( ( (word>> 4)&0x0F ) ,0);
    m_candRegHandlerBCSU[ib].setCandColM3( ( (word>> 8)&0x1F ) ,0);
    m_candRegHandlerBCSU[ib].setCandRowM3( ( (word>>13)&0x03 ) ,0);
    m_candRegHandlerBCSU[ib].setCandPT(    ( (word>>16)&0x7F ) ,1);
    m_candRegHandlerBCSU[ib].setCandCharge(( (word>>23)&0x01 ) ,1);
    m_candRegHandlerBCSU[ib].setCandPT(    ( (word>>24)&0x7F ) ,0);
    m_candRegHandlerBCSU[ib].setCandCharge(( (word>>31)&0x01 ) ,0);

    empty = (word>>15)&0x1;
    if (empty!=0) {
      ++decodingError[ib];
      std::cout << "L0Muon::ProcRawCnv::decodeBank decodingError ib= "<<ib<<" B"<<std::endl;
    }

    word = raw[ib*L0Muon::ProcRawCnv::board_data_size + 1 ];
    m_errors[ib].bcsu_bcid.set((word>> 0)&0x0F,Refl0_B_Id);

    m_candRegHandlerBCSU[ib].setStatus(    ( (word>> 4)&0x0F ) );
    m_candRegHandlerBCSU[ib].setCandPU(    ( (word>> 8)&0x03 ) ,1);
    m_candRegHandlerBCSU[ib].setCandPU(    ( (word>>10)&0x03 ) ,0);
    m_candRegHandlerBCSU[ib].setCandOffM1( ( (word>>16)&0x0F ) ,1);
    m_candRegHandlerBCSU[ib].setCandOffM2( ( (word>>20)&0x0F ) ,1);
    m_candRegHandlerBCSU[ib].setCandColM3( ( (word>>24)&0x1F ) ,1);
    m_candRegHandlerBCSU[ib].setCandRowM3( ( (word>>29)&0x03 ) ,1);

    empty = (word>>31)&0x1;
    if (empty!=0) {
      ++decodingError[ib];
      std::cout << "L0Muon::ProcRawCnv::decodeBank decodingError ib= "<<ib<<" C"<<std::endl;
    }
    
    int status =m_candRegHandlerBCSU[ib].getStatus();
    
    m_errors[ib].bcsu_status.set(status);
    
    int ncand = status&0x3;
    ncand = ncand<3 ? ncand : 2;
    //     if (ncand!=m_candRegHandlerBCSU[ib].numberOfCandidates()) {
    
    //       std::cout << "L0Muon::ProcRawCnv::decodeBank decodingError ib= "<<ib<<" D"<<std::endl;
    //       std::cout<<"L0Muon::ProcRawCnv::decodeBank ERROR : non empty candidate in contradiction with status !!!"
    //                << " BCSU board# "<<ib
    //                <<std::hex<<" status = 0x"<<m_candRegHandlerBCSU[ib].getStatus()<<std::dec
    //                << "ncand= "<<ncand<<" numberOfCandidates()= "<<m_candRegHandlerBCSU[ib].numberOfCandidates()<<std::endl;
    //       m_candRegHandlerBCSU[ib].dump();
    
    //       ++decodingError[ib];
    //       m_candRegHandlerBCSU[ib].clear();
    //       m_candRegHandlerBCSU[ib].setStatus(status);
    //     } else {
    for( int icand =0; icand<ncand;++icand){
      m_candRegHandlerBCSU[ib].setCandBoard( ib ,icand);
      m_candRegHandlerBCSU[ib].setCandQuarter( m_quarter ,icand);
    }
    //     }
    
    // Loop over PUs candidates
    for (int ipu = 0; ipu<4; ++ipu) {
      
      word = raw[ib*L0Muon::ProcRawCnv::board_data_size +2 +ipu*2 ];
      m_candRegHandlerPU[ib][ipu].setCandOffM1( ( (word>> 0)&0x0F ) ,0);
      m_candRegHandlerPU[ib][ipu].setCandOffM2( ( (word>> 4)&0x0F ) ,0);
      m_candRegHandlerPU[ib][ipu].setCandColM3( ( (word>> 8)&0x1F ) ,0);
      m_candRegHandlerPU[ib][ipu].setCandRowM3( ( (word>>13)&0x03 ) ,0);
      m_candRegHandlerPU[ib][ipu].setCandPT(    ( (word>>16)&0x7F ) ,1);
      m_candRegHandlerPU[ib][ipu].setCandCharge(( (word>>23)&0x01 ) ,1);
      m_candRegHandlerPU[ib][ipu].setCandPT(    ( (word>>24)&0x7F ) ,0);
      m_candRegHandlerPU[ib][ipu].setCandCharge(( (word>>31)&0x01 ) ,0);
      
      empty = (word>>15)&0x1;
      if (empty!=0) {
        ++decodingError[ib];
        std::cout << "L0Muon::ProcRawCnv::decodeBank decodingError ib= "<<ib<<" E"<<std::endl;
      }
      
      word = raw[ib*L0Muon::ProcRawCnv::board_data_size+3+ipu*2];
      m_errors[ib].pus_bcid[ipu].set((word>> 0)&0x0F,Refl0_B_Id);
      
      empty = (word>>8)&0xFF;
      if (empty!=0) {
        ++decodingError[ib];
        std::cout << "L0Muon::ProcRawCnv::decodeBank decodingError ib= "<<ib<<" F"<<std::endl;
      }
      
      m_candRegHandlerPU[ib][ipu].setStatus(    ( (word>> 4)&0x0F ) );
      m_candRegHandlerPU[ib][ipu].setCandOffM1( ( (word>>16)&0x0F ) ,1);
      m_candRegHandlerPU[ib][ipu].setCandOffM2( ( (word>>20)&0x0F ) ,1);
      m_candRegHandlerPU[ib][ipu].setCandColM3( ( (word>>24)&0x1F ) ,1);
      m_candRegHandlerPU[ib][ipu].setCandRowM3( ( (word>>29)&0x03 ) ,1);
      
      int status = m_candRegHandlerPU[ib][ipu].getStatus();

      m_errors[ib].pus_status[ipu].set(status);

      int ncand = status&0x3;
      ncand = ncand<3 ? ncand : 2;
      //       if (ncand!=m_candRegHandlerPU[ib][ipu].numberOfCandidates()) {

      //         std::cout << "L0Muon::ProcRawCnv::decodeBank decodingError ib= "<<ib<<" G"<<std::endl;
      //         std::cout<<"L0Muon::ProcRawCnv::decodeBank ERROR : non empty candidate in contradiction with status !!!"
      //                  << " board# "<<ib<<" PU# "<<ipu
      //                  <<std::hex<<" 0x"<<m_candRegHandlerPU[ib][ipu].getStatus()<<std::dec<<std::endl;
      //         m_candRegHandlerPU[ib][ipu].dump();

      //         m_candRegHandlerPU[ib][ipu].clear();
      //         m_candRegHandlerPU[ib][ipu].setStatus(status);
      //         ++decodingError[ib];

      //       } else {
      for( int icand =0; icand<ncand;++icand){
        m_candRegHandlerPU[ib][ipu].setCandPU( ipu ,icand);
        m_candRegHandlerPU[ib][ipu].setCandBoard( ib ,icand);
        m_candRegHandlerPU[ib][ipu].setCandQuarter( m_quarter ,icand);
      }
      //       }
      
    }// End of loop over PUs candidates

    int iwd = ib*L0Muon::ProcRawCnv::board_data_size+10;
    // Loop over PUs input data
    for (int ipu = 0; ipu<4; ++ipu) {
      
      boost::dynamic_bitset<> rawbitset;
      int rawsize ;
      
      if (ipu%2==0) {
        
        // 
        // Error signalization field
        // 
        word = raw[iwd];
        m_errors[ib].add_link[ipu].set( (word>>24)&0xFF);
        m_errors[ib].opt_link[ipu].set( (word>>16)&0xFF);
        m_errors[ib].ser_link[ipu].set( (word>> 8)&0x3F);
        m_errors[ib].par_link[ipu].set( (word>> 0)&0xFF);
        
        ++iwd; // iwd = 11 (%68)

        // 
        // OL data field
        // 
        iwd+=4; // iwd = 15 (%68)
        for (int i=0; i<5; ++i) { // 5 full words
          boost::dynamic_bitset<> bitset(32,raw[iwd-i]);
          rawsize = rawbitset.size();
          rawbitset.resize(rawsize+32);
          bitset.resize(rawsize+32);
          bitset<<=rawsize;
          rawbitset|=bitset;
        }
        
        if (m_ols[ib][ipu]!=0) m_ols[ib][ipu]->set(rawbitset);
        rawbitset.clear();

        // 
        // Neighbour data field
        // 
        
        iwd+=9; // iwd = 24 (%68)
        {                          // 16 bits (msb) of last word
          boost::dynamic_bitset<> bitset(16,( (raw[iwd]>>16)&0xFFFF ) );
          rawsize = rawbitset.size();
          rawbitset.resize(rawsize+16);
          bitset.resize(rawsize+16);
          bitset<<=rawsize;
          rawbitset|=bitset;
        }

        for (int i=0; i<8; ++i) { // 8 full words
          boost::dynamic_bitset<> bitset(32,raw[iwd-1-i]);
          rawsize = rawbitset.size();
          rawbitset.resize(rawsize+32);
          bitset.resize(rawsize+32);
          bitset<<=rawsize;
          rawbitset|=bitset;
        }
        
        if (m_neighs[ib][ipu]!=0) m_neighs[ib][ipu]->set(rawbitset);   
        rawbitset.clear();

      } else {
        
        // 
        // Error signalization field
        // 
        word = raw[iwd];
        m_errors[ib].add_link[ipu].set( (word>> 8)&0xFF);
        m_errors[ib].opt_link[ipu].set( (word>> 0)&0xFF);
        ++iwd;
        word = raw[iwd];
        m_errors[ib].ser_link[ipu].set( (word>>24)&0x3F);
        m_errors[ib].par_link[ipu].set( (word>>16)&0xFF);
        // iwd = 25 (%68)
        
        //
        // OL data field
        // 

        iwd+=5; // iwd = 30 (%68)
        {                         // 16 bits (msb) of last word (30)
          boost::dynamic_bitset<> bitset(16,( (raw[iwd]>>16)&0xFFFF ) );
          rawsize = rawbitset.size();
          rawbitset.resize(rawsize+16);
          bitset.resize(rawsize+16);
          bitset<<=rawsize;
          rawbitset|=bitset;
        }
        
        
        for (int i=0; i<4; ++i) { // 4 full words (29-28-27-26)
          boost::dynamic_bitset<> bitset(32,raw[iwd-1-i]);
          rawsize = rawbitset.size();
          rawbitset.resize(rawsize+32);
          bitset.resize(rawsize+32);
          bitset<<=rawsize;
          rawbitset|=bitset;
        }

        {                          // 16 bits (lsb) of first word (30-5=25)
          boost::dynamic_bitset<> bitset(16,(raw[iwd-5]&0xFFFF));
          rawsize = rawbitset.size();
          rawbitset.resize(rawsize+16);
          bitset.resize(rawsize+16);
          bitset<<=rawsize;
          rawbitset|=bitset;
        }
        
        if (m_ols[ib][ipu]!=0) m_ols[ib][ipu]->set(rawbitset);
        rawbitset.clear();

        //
        // Neighbour data field
        // 

        iwd+=8; // iwd = 38 (%68)
        for (int i=0; i<8; ++i) { // 8 full words
          boost::dynamic_bitset<> bitset(32,raw[iwd-i]);
          rawsize = rawbitset.size();
          rawbitset.resize(rawsize+32);
          bitset.resize(rawsize+32);
          bitset<<=rawsize;
          rawbitset|=bitset;
        }

        { // 16 bits (lsb) of previous word
          boost::dynamic_bitset<> bitset(16,raw[iwd-8]);
          rawsize = rawbitset.size();
          rawbitset.resize(rawsize+16);
          bitset.resize(rawsize+16);
          bitset<<=rawsize;
          rawbitset|=bitset;
        }
        
        if (m_neighs[ib][ipu]!=0) m_neighs[ib][ipu]->set(rawbitset);   
        rawbitset.clear();

        ++iwd;

      }

    }// End of loop over PUs input data


  }// End of loop over processing boards

  // temporary check
  std::vector<unsigned int> tmp;
  rawBank(tmp, bankVersion);
  for (int ib=0; ib<12;++ib){
    for (unsigned int i=0; i<L0Muon::ProcRawCnv::board_full_data_size; ++i){
      unsigned int iw=ib*L0Muon::ProcRawCnv::board_full_data_size+i;
      if (tmp[iw]!=original[iw]) {
        if (decodingError[ib]==0){
          ++decodingError[ib];
          std::cout << "L0Muon::ProcRawCnv::decodeBank decodingError ib= "<<ib<<" H"<<std::endl;
          std::cout<<"\t !!! L0Muon::ProcRawCnv::decodeBank !!! ERROR !!! line "<<iw<<" board "<<ib
                   <<std::hex<<" rebuild= 0x"<<tmp[iw]<<" VS original= 0x"<<original[iw]<<std::dec<<std::endl;
        }
      }
    }
  }

  
  for (int ib=0; ib<12; ++ib)  m_errors[ib].decoding.set(decodingError[ib]);
  
  // Reset registers of a board if a decoding error has been detected
  for (int ib=0; ib<12; ++ib){
    if (decodingError[ib]>0) {
      m_candRegHandlerBCSU[ib].clear();
      for (int ipu = 0; ipu<4; ++ipu) {
        m_candRegHandlerPU[ib][ipu].clear();
        m_ols[ib][ipu]->reset();
        m_neighs[ib][ipu]->reset();
      }
      std::cout<<"\tL0Muon::ProcRawCnv::decodeBank decodingError !!! DECODING ERROR !!! board "<<ib<<std::endl;
    }
  }

  raw = original;
  ++m_n_decoded_banks;
  

}



// void L0Muon::ProcRawCnv::setErrors(int l0EventNum, int l0_B_Id){
//   for (int ib=0; ib<12; ++ib){
//     for (int ich=0; ich<2; ++ich){
//       m_errors[ib].board_index[ich].set(ib,ib);
//       m_errors[ib].l0EventNumber[ich].set(l0EventNum,l0EventNum);
//       m_errors[ib].l0_B_Id[ich].set(l0_B_Id,l0_B_Id);
//     }
//     m_errors[ib].bcsu_bcid.set(l0_B_Id,l0_B_Id);
//     for (int ipu=0; ipu<4; ++ipu){
//       m_errors[ib].pus_bcid[ipu].set(l0_B_Id,l0_B_Id);
//     }
//   }
// }

void L0Muon::ProcRawCnv::rawBank(std::vector<unsigned int> &data, int bankVersion){
  std::vector<unsigned int> raw;
  int event_number;
  int l0_bid;
  int board_index;
  unsigned int event_num_word;
  int bcid;

  if (bankVersion<1) return;

  // Loop over processing boards 
  for (int ib=0; ib<12; ++ib) {

    event_number   = m_errors[ib].l0EventNumber[0].value();
    l0_bid         = m_errors[ib].l0_B_Id[0].value();
    board_index    = m_errors[ib].board_index[0].value();   
    event_num_word = ((event_number<<16)&0xFFF0000 ) + ((board_index<<12)&0xF000) + ( l0_bid&0xFFF );
    raw.push_back(event_num_word);

    unsigned int word;

    unsigned int add[2];
    unsigned int pt[2];
    unsigned int pu[2];

    for (int icand=0; icand<2; ++icand) {
      int colM3  = m_candRegHandlerBCSU[ib].getCandColM3(icand);
      int rowM3  = m_candRegHandlerBCSU[ib].getCandRowM3(icand);
      int offM1  = m_candRegHandlerBCSU[ib].getCandOffM1(icand);
      int offM2  = m_candRegHandlerBCSU[ib].getCandOffM2(icand);
      int abspt  = m_candRegHandlerBCSU[ib].getCandPT(icand);
      int charge = m_candRegHandlerBCSU[ib].getCandCharge(icand);
      
      add[icand] = ( (rowM3<<13)&0x6000 ) + ( (colM3<<8)&0x1F00 ) + ( (offM2<<4)&0xF0 ) + ( offM1&0xF );
      pt[icand]  = ( (charge<<7)&0x80 ) + abspt ;
      pu[icand]  = m_candRegHandlerBCSU[ib].getCandPU(icand);
    }
    int status = m_candRegHandlerBCSU[ib].getStatus();

    word = ( ( (pt[0]<<24)&0xFF000000 ) + ( (pt[1]<<16)&0xFF0000 ) + ( (add[0])&0x7FFF ) );
    raw.push_back(word);

    bcid = m_errors[ib].bcsu_bcid.value();
    word = ( (add[1]<<16)&0x7FFF0000 ) + ( (pu[0]<<10)&0xC00 ) + ( (pu[1]<<8)&0x200 ) + ( (status<<4)&0xF0 ) + (bcid&0xF);
    raw.push_back(word);
 
    // Loop over PUs candidates
    for (int ipu = 0; ipu<4; ++ipu) {
      for (int icand=0; icand<2; ++icand) {

        int colM3  = m_candRegHandlerPU[ib][ipu].getCandColM3(icand);
        int rowM3  = m_candRegHandlerPU[ib][ipu].getCandRowM3(icand);
        int offM1  = m_candRegHandlerPU[ib][ipu].getCandOffM1(icand);
        int offM2  = m_candRegHandlerPU[ib][ipu].getCandOffM2(icand);
        int abspt  = m_candRegHandlerPU[ib][ipu].getCandPT(icand);
        int charge = m_candRegHandlerPU[ib][ipu].getCandCharge(icand);
      
        add[icand] = ( (rowM3<<13)&0x6000 ) + ( (colM3<<8)&0x1F00 ) + ( (offM2<<4)&0xF0 ) + ( offM1&0xF );
        pt[icand]  = ( (charge<<7)&0x80 ) + abspt ;
        pu[icand]  = m_candRegHandlerPU[ib][ipu].getCandPU(icand);
      }
      int status = m_candRegHandlerPU[ib][ipu].getStatus();
      bcid = m_errors[ib].pus_bcid[ipu].value();

      word = ( ( (pt[0]<<24)&0xFF000000 ) + ( (pt[1]<<16)&0xFF0000 ) + ( (add[0])&0x7FFF ) );
      raw.push_back(word);

      word = ( (add[1]<<16)&0x7FFF0000 ) + ( (status<<4)&0xF0 ) + (bcid&0xF);
      raw.push_back(word);
      
    }// End of loop over PUs candidates
    
    // Loop over PUs input data
    for (int ipu = 0; ipu<4; ++ipu) {

      boost::dynamic_bitset<> olbitset(10*16);
      boost::dynamic_bitset<> neighbitset(17*16);
      
      if (m_ols[ib][ipu]   !=0) olbitset    = m_ols[ib][ipu]->getBitset();
      if (m_neighs[ib][ipu]!=0) neighbitset = m_neighs[ib][ipu]->getBitset();

      int size;
      boost::dynamic_bitset<> bitset;
      
      if (ipu%2==0) {
        int add = m_errors[ib].add_link[ipu].value();
        int opt = m_errors[ib].opt_link[ipu].value();
        int ser = m_errors[ib].ser_link[ipu].value();
        int par = m_errors[ib].par_link[ipu].value();
        word=( ((add<<24)&0xFF000000) + ((opt<<16)&0xFF0000) + ((ser<<8)&0x3F00) + ((par<<0)&0xFF) );
        raw.push_back(word); // Signalization field

        // 
        // OL data field
        // 

        for (int i=0; i<5; ++i) {
          size=32*(4-i);
          bitset = (olbitset>>size);
          bitset.resize(32);
          word = bitset.to_ulong();
          raw.push_back(word);
        }

        // 
        // Neighbour data field
        // 

        for (int i=0; i<8; ++i) {
          size=32*(7-i)+16;
          bitset = (neighbitset>>size);
          bitset.resize(32);
          word = bitset.to_ulong();
          raw.push_back(word);
        }

        bitset = neighbitset;
        bitset.resize(16);
        word = ( ( bitset.to_ulong()<<16 ) & 0xFFFF0000 );
        
      } else {

        int add = m_errors[ib].add_link[ipu].value();
        int opt = m_errors[ib].opt_link[ipu].value();
        int ser = m_errors[ib].ser_link[ipu].value();
        int par = m_errors[ib].par_link[ipu].value();
        word+= ((add<<8)&0xFF00) + (opt&0x00FF);
        raw.push_back(word); //  bits[0-15] are the last bits from previous PU 
        word=( ((ser<<24)&0x3F000000) + ((par<<16)&0xFF0000) );

        // 
        // OL data field
        // 

        bitset = (olbitset>>( (32*4) +16 ) );
        bitset.resize(16);
        word |= ( ( bitset.to_ulong() ) & 0xFFFF );
        raw.push_back(word);

        for (int i=0; i<4; ++i) {
          size= 16 + 32*(3-i) ;
          bitset = (olbitset>>size);
          bitset.resize(32);
          word = bitset.to_ulong();
          raw.push_back(word);
        }
        
        bitset = (olbitset>>(0));
        bitset.resize(16);
        word = ( ( bitset.to_ulong() <<16) & 0xFFFF0000 );
        
        // 
        // Neighbour data field
        // 

        bitset = (neighbitset>>(16*16));
        bitset.resize(16);
        word |= ( ( bitset.to_ulong() ) & 0xFFFF );
        raw.push_back(word);

        for (int i=0; i<8; ++i) {
          size= 32*(7-i) ;
          bitset = (neighbitset>>size);
          bitset.resize(32);
          word = bitset.to_ulong();
          raw.push_back(word);
        }
      }
    } // End of loop over PUs input data
    
    // Insert missing words 
    std::vector<unsigned int>::iterator itraw;
    
    // insert empty word at the end of each frame
    word=0;
    itraw= raw.begin() + ib*L0Muon::ProcRawCnv::board_full_data_size+L0Muon::ProcRawCnv::board_full_frame_size-1;
    raw.insert(itraw,word);
    raw.push_back(word);

    // Insert the line with the event numbers
    itraw= raw.begin() + ib*L0Muon::ProcRawCnv::board_full_data_size+L0Muon::ProcRawCnv::board_full_frame_size;
    event_number   = m_errors[ib].l0EventNumber[1].value(); 
    l0_bid         = m_errors[ib].l0_B_Id[1].value();       
    board_index    = m_errors[ib].board_index[1].value();   
    event_num_word = ((event_number<<16)&0xFFF0000 )+ ((board_index<<12)&0xF000) + ( l0_bid&0xFFF );
    raw.insert(itraw,event_num_word);

  }// End of loop over processing boards
  

  // Re-order the bank such that the second frame of each processing board is written first
  reorderOLChannels(raw,data);

  //   std::cout.setf(std::ios::uppercase) ;
  //   std::cout<<"\t=> L0Muon::ProcRawCnv::rawBank -- dump raw bank of size: "<<raw.size()<<std::endl;
  //   int icount=0;
  //   for (std::vector<unsigned int>::iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
  //     std::cout <<std::setw(3)<<icount<<std::hex<<"\t0x"<<std::setw(8)<<std::setfill('0')<<(*itraw)<<std::dec<<std::endl;
  //     ++icount;
  //   }
  //   std::cout<<std::dec;
  //   std::cout.unsetf(std::ios::uppercase);
  
}

void L0Muon::ProcRawCnv::reorderOLChannels(const std::vector<unsigned int> &raw, std::vector<unsigned int> &reordered){

  std::vector<unsigned int>::iterator itpos;
  std::vector<unsigned int>::const_iterator itstart;
  std::vector<unsigned int>::const_iterator itstop;
  
  for (int ib=0; ib<12; ++ib) {
    for (int ich=1; ich>-1; --ich){
      itstart = raw.begin()+(ib*2+ich)*L0Muon::ProcRawCnv::board_full_frame_size;
      itstop  = itstart+L0Muon::ProcRawCnv::board_full_frame_size;
      itpos   = reordered.end();
      reordered.insert(itpos,itstart,itstop);
    }
  }
}

