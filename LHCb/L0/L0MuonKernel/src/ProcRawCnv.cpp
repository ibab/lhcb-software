#include "L0MuonKernel/ProcRawCnv.h"

#include "ProcessorKernel/RegisterFactory.h"
#include "ProcessorKernel/Register.h"

#include "L0MuonKernel/BankUtilities.h"

/**
   Constructor.
*/
L0Muon::ProcRawCnv::ProcRawCnv(){

  for (int ib=0;ib<12;++ib){
    for (int ich=0;ich<2;++ich){
      m_L0_B_Id[ib][ich]=0;
      m_L0EventNumber[ib][ich]=0;
    }
    for (int ipu=0;ipu<4;++ipu){
      m_BCID_PU[ib][ipu]=0;
    }
    m_BCID_BCSU[ib]=0;
  }
  
  m_quarter=0;
};
/**
   Constructor.
*/
L0Muon::ProcRawCnv::ProcRawCnv(int quarter){


  for (int ib=0;ib<12;++ib){
    for (int ich=0;ich<2;++ich){
      m_L0_B_Id[ib][ich]=0;
      m_L0EventNumber[ib][ich]=0;
    }
    for (int ipu=0;ipu<4;++ipu){
      m_BCID_PU[ib][ipu]=0;
    }
    m_BCID_BCSU[ib]=0;
  }
  
  m_quarter=quarter;

  char buf[4096];
  char* format ;
  
  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

  for (int iboard = 0; iboard <12 ; iboard++) {
    // Candidate Register handlers for BCSU candidates
    format = "BOARDCAND_Q%d_%d";
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
}

std::vector<L0Muon::PMuonCandidate>  L0Muon::ProcRawCnv::muonCandidatesPU(){
  std::vector<L0Muon::PMuonCandidate> cands;
  for (int ib = 0; ib<12 ; ++ib){
    for (int ipu= 0; ipu<4 ;++ipu) {
      int ncand= m_candRegHandlerPU[ib][ipu].numberOfCandidates();
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
    int ncand= m_candRegHandlerBCSU[ib].numberOfCandidates();
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
    }    
  }
//   if (it==m_olsMap.end()) {
//     std::cout <<"L0Muon::ProcRawCnv::ols PU not found in ol map"<<std::endl;
//     return pads;
//   } 
//   if ((*it).second==0) {
//     std::cout <<"L0Muon::ProcRawCnv::ols no tile register associated"<<std::endl;
//     return pads;
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

std::vector<LHCb::MuonTileID>  L0Muon::ProcRawCnv::pus()
{
  std::vector<LHCb::MuonTileID> lpuids;
  std::map<LHCb::MuonTileID, TileRegister*>::iterator itmap;
  for (itmap=m_olsMap.begin(); itmap!=m_olsMap.end(); ++itmap){
    lpuids.push_back(itmap->first);
  }
  
  return lpuids;
}
    
void L0Muon::ProcRawCnv::dump(int bankVersion, int ievt) 
{
  std::string tab="";
  dump(bankVersion,ievt,tab);
}

void L0Muon::ProcRawCnv::dump(int bankVersion, int ievt,std::string tab)
{

  std::vector<unsigned int> raw = rawBank(bankVersion,ievt);

//   std::cout<<"L0Muon::ProcRawCnv::dump IN"<<std::endl;
//   std::cout<<"L0Muon::ProcRawCnv::dump rawsize="<<raw.size()<<std::endl;

  std::cout <<"__ "<<tab<<"evt#: "<<ievt<<" - Q"<<m_quarter+1<<" ; bank size: "<<raw.size()<<std::endl;
    
  std::cout <<"__ "<<tab;
  std::cout <<"  ";
  for (int ib=0; ib<12; ++ib) {
    std::cout <<"     "<<std::setw(2)<<ib<<"    ";
  }
  std::cout <<std::endl;

  std::cout.setf(std::ios::uppercase) ;

  for (unsigned int iline=0; iline<L0Muon::ProcRawCnv::board_full_data_size; ++iline) {
    
    std::cout <<"__ "<<tab;
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

void L0Muon::ProcRawCnv::formattedDump(int bankVersion, int ievt) 
{
  std::string tab="";
  formattedDump(bankVersion,ievt,tab);
}

void L0Muon::ProcRawCnv::formattedDump(int bankVersion, int ievt,std::string tab)
{

  std::vector<unsigned int> raw = rawBank(bankVersion,ievt);

  // Re-order the bank such that the second frame of each processing board is read first
  raw =reorderOLChannels(raw);

//   std::cout<<"L0Muon::ProcRawCnv::dump IN"<<std::endl;
//   std::cout<<"L0Muon::ProcRawCnv::dump rawsize="<<raw.size()<<std::endl;

  std::cout <<"++ "<<tab<<"evt#: "<<ievt<<" - Q"<<m_quarter+1<<" ; bank size: "<<raw.size()<<std::endl;
    
  std::cout <<"__ "<<tab;
  std::cout <<"   ";
  for (int ib=0; ib<12; ++ib) {
    std::cout <<"    "<<std::setw(2)<<ib<<" ";
  }
  std::cout <<std::endl;

  std::cout.setf(std::ios::uppercase) ;

  int line_count=0;
  for (unsigned int iline=0; iline<L0Muon::ProcRawCnv::board_full_data_size; ++iline) {
    for (int ipart=0; ipart<2; ++ipart) {
      std::cout <<"__ "<<tab;
      std::cout <<std::setw(3)<<line_count;
      std::cout<<std::hex;
      for (int ib=0; ib<12; ++ib) {
        int word = (raw[L0Muon::ProcRawCnv::board_full_data_size*ib+iline]>>((1-ipart)*16))&0xFFFF;
        std::cout <<" 0x"<<std::setw(4)<<std::setfill('0')<<word;
      }
      std::cout<<std::dec;
      std::cout <<std::endl;
      ++line_count;
    }
  }
  
  std::cout <<"__ ----"<<std::endl;  
  std::cout.unsetf(std::ios::uppercase);
}


void L0Muon::ProcRawCnv::decodeBank(std::vector<unsigned int> raw, int bankVersion){
  if (bankVersion<1) return;
  unsigned int word;
  int bcid;
  
//   std::cout.setf(std::ios::uppercase) ;
//   std::cout<<"\t=> L0Muon::ProcRawCnv::decodeBank -- dump raw bank of size: "<<raw.size()<<std::endl;
//   int icount=0;
//   for (std::vector<unsigned int>::iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
//     std::cout <<std::setw(3)<<icount<<std::hex<<"\t0x"<<std::setw(8)<<std::setfill('0')<<(*itraw)<<std::dec<<std::endl;
//     ++icount;
//   }
//   std::cout<<std::dec;
//   std::cout.unsetf(std::ios::uppercase);
 
  std::vector<unsigned int> original = raw;
// 
  // Clear the registers first
  release();

  // Re-order the bank such that the second frame of each processing board is read first
  raw =reorderOLChannels(raw);
  
  // Remove  words at the beginning and end of each frame
  std::vector<unsigned int>::iterator itraw;

  // Remove the empty word at the end of each frame
//   std::cout<<"\t=> L0Muon::ProcRawCnv::decodeBankV1 -- reducing bank of original size: "<<raw.size()<<std::endl;
  for (int ifr=24; ifr>0; --ifr) {
    int pos = ifr*L0Muon::ProcRawCnv::board_full_frame_size-1;
//     std::cout<<ifr<<" removing el 0x"<<std::hex<<std::setw(8)<<raw[pos]<<std::dec<<" at pos "<<pos<<std::endl;
    itraw=raw.begin()+pos;
    raw.erase(itraw);
  }
  
  // Get and Remove the L0EventNumber and the L0_B_Id at the beginning of each frame
//   std::cout<<"\t=> L0Muon::ProcRawCnv::decodeBankV1 -- reducing bank of original size: "<<raw.size()<<std::endl;
  for (int ib=11; ib>-1; --ib) {
    for (int ich=1; ich>-1; --ich){
      int pos = (ib*2+ich)*(L0Muon::ProcRawCnv::board_full_frame_size-1);
      word = raw[pos];
      m_L0_B_Id[ib][ich]       = ((word>> 0)&0xFFF);
      m_L0EventNumber[ib][ich] = ((word>>16)&0xFFF);
      
//       std::cout<<"board:"<<ib<<" ch "<<ich
//                <<std::hex
//                <<" L0_B_Id = 0x"      <<std::setw(8)<<L0_B_Id
//                <<" L0EventNumber = 0x"<<std::setw(8)<<L0EventNumber
//                <<std::dec<<std::endl;
//       std::cout<<ib<<" removing el 0x"<<std::hex<<std::setw(8)<<raw[pos]<<std::dec<<" at pos "<<pos<<std::endl;
      itraw=raw.begin()+pos;
      raw.erase(itraw);
    }
  }


//   std::cout.setf(std::ios::uppercase) ;
//   std::cout<<"\t=> L0Muon::ProcRawCnv::decodeBankV1 -- dump raw bank of size: "<<raw.size()<<std::endl;
//   int icount=0;
//   for (std::vector<unsigned int>::iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
//     std::cout <<std::setw(3)<<icount<<std::hex<<"\t0x"<<std::setw(8)<<(*itraw)<<std::dec<<std::endl;
//     ++icount;
//   }
//   std::cout<<std::dec;
//   std::cout.unsetf(std::ios::uppercase);

  //   //   unsigned int header = raw[0];
  // Loop over processing boards 
  for (int ib=0; ib<12; ++ib) {

    word = raw[ib*L0Muon::ProcRawCnv::board_data_size + 0 ];
    m_candRegHandlerBCSU[ib].setCandOffM1( ( (word>> 0)&0x0F ) ,0);
    m_candRegHandlerBCSU[ib].setCandOffM2( ( (word>> 4)&0x0F ) ,0);
    m_candRegHandlerBCSU[ib].setCandRowM3( ( (word>> 8)&0x1F ) ,0);
    m_candRegHandlerBCSU[ib].setCandColM3( ( (word>>13)&0x03 ) ,0);
    m_candRegHandlerBCSU[ib].setCandPT(    ( (word>>16)&0x7F ) ,1);
    m_candRegHandlerBCSU[ib].setCandCharge(( (word>>23)&0x01 ) ,1);
    m_candRegHandlerBCSU[ib].setCandPT(    ( (word>>24)&0x7F ) ,0);
    m_candRegHandlerBCSU[ib].setCandCharge(( (word>>30)&0x01 ) ,0);
    
    word = raw[ib*L0Muon::ProcRawCnv::board_data_size + 1 ];
    bcid = (word>> 0)&0x0F;
    m_BCID_BCSU[ib]=bcid;

    m_candRegHandlerBCSU[ib].setStatus(    ( (word>> 4)&0x0F ) );
    m_candRegHandlerBCSU[ib].setCandPU(    ( (word>> 8)&0x03 ) ,1);
    m_candRegHandlerBCSU[ib].setCandPU(    ( (word>>10)&0x03 ) ,0);
    m_candRegHandlerBCSU[ib].setCandOffM1( ( (word>>16)&0x0F ) ,1);
    m_candRegHandlerBCSU[ib].setCandOffM2( ( (word>>20)&0x0F ) ,1);
    m_candRegHandlerBCSU[ib].setCandRowM3( ( (word>>24)&0x1F ) ,1);
    m_candRegHandlerBCSU[ib].setCandColM3( ( (word>>29)&0x03 ) ,1);

//     int status_bcsuA = (word>> 4)&0x0F;
//     int status_bcsuB = m_candRegHandlerBCSU[ib].getStatus();
//     std::cout<<"\t=> L0Muon::ProcRawCnv::decodeBank     BCSU candidates : "<<ib
//              <<"status A:0x"<<std::hex<<status_bcsuA
//              <<"status B:0x"<<std::hex<<status_bcsuB
//              <<" bcid 0x"<<bcid<<std::dec<<std::endl;

    // Loop over PUs candidates
    for (int ipu = 0; ipu<4; ++ipu) {

      word = raw[ib*L0Muon::ProcRawCnv::board_data_size +2 +ipu*2 ];
      m_candRegHandlerPU[ib][ipu].setCandOffM1( ( (word>> 0)&0x0F ) ,0);
      m_candRegHandlerPU[ib][ipu].setCandOffM2( ( (word>> 4)&0x0F ) ,0);
      m_candRegHandlerPU[ib][ipu].setCandRowM3( ( (word>> 8)&0x1F ) ,0);
      m_candRegHandlerPU[ib][ipu].setCandColM3( ( (word>>13)&0x03 ) ,0);
      m_candRegHandlerPU[ib][ipu].setCandPT(    ( (word>>16)&0x7F ) ,1);
      m_candRegHandlerPU[ib][ipu].setCandCharge(( (word>>23)&0x01 ) ,1);
      m_candRegHandlerPU[ib][ipu].setCandPT(    ( (word>>24)&0x7F ) ,0);
      m_candRegHandlerPU[ib][ipu].setCandCharge(( (word>>30)&0x01 ) ,0);
    
      word = raw[ib*L0Muon::ProcRawCnv::board_data_size+3+ipu*2];
      bcid = (word>> 0)&0x0F;
      m_BCID_PU[ib][ipu]=bcid;

      m_candRegHandlerPU[ib][ipu].setStatus(    ( (word>> 4)&0x0F ) );
      m_candRegHandlerPU[ib][ipu].setCandOffM1( ( (word>>16)&0x0F ) ,1);
      m_candRegHandlerPU[ib][ipu].setCandOffM2( ( (word>>20)&0x0F ) ,1);
      m_candRegHandlerPU[ib][ipu].setCandRowM3( ( (word>>24)&0x1F ) ,1);
      m_candRegHandlerPU[ib][ipu].setCandColM3( ( (word>>29)&0x03 ) ,1);

      m_candRegHandlerPU[ib][ipu].setCandPU( ipu ,0);
      m_candRegHandlerPU[ib][ipu].setCandPU( ipu ,1);

//       int status_puA = (word>> 4)&0x0F;
//       int status_puB = m_candRegHandlerPU[ib][ipu].getStatus();
//       std::cout<<"\t=> L0Muon::ProcRawCnv::decodeBank     PU candidates : "<<ib<<" "<<ipu<<" "
//                <<"status A:0x"<<std::hex<<status_puA
//                <<"status B:0x"<<std::hex<<status_puB
//                <<" bcid 0x"<<bcid<<std::dec<<std::endl;

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
        opt_link_error[ib][ipu]= ( (word>>16)&0xFFFF);
        ser_link_error[ib][ipu]= ( (word>> 8)&0x3F);
        par_link_error[ib][ipu]= ( (word>> 0)&0xFF);
        
        ++iwd; 
        // iwd = 11 (%68)

        // 
        // OL data field
        // 
//         std::cout<<"board"<<ib<<"PU"<<ipu<<"OL from "<<iwd;
        iwd+=4;
//         std::cout<<" to "<<iwd<<std::endl;
        // iwd = 15 (%68)
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
        
//         std::cout<<"board"<<ib<<"PU"<<ipu<<"neighbours from "<<iwd;
        iwd+=9;
//         std::cout<<" to "<<iwd<<std::endl;
        // iwd = 24 (%68)
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
        opt_link_error[ib][ipu]= ( (word>> 0)&0xFFFF);
        ++iwd;
        word = raw[iwd];
        ser_link_error[ib][ipu]= ( (word>>24)&0x3F);
        par_link_error[ib][ipu]= ( (word>>16)&0xFF);
        // iwd = 25 (%68)

        //
        // OL data field
        // 

//         std::cout<<"board"<<ib<<"PU"<<ipu<<"OL from "<<iwd;
        iwd+=5;
//         std::cout<<" to "<<iwd<<std::endl;
        // iwd = 30 (%68)
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

//         std::cout<<"board"<<ib<<"PU"<<ipu<<"neighbours from "<<iwd;
        iwd+=8;
//         std::cout<<" to "<<iwd<<std::endl;
        // iwd = 38 (%68)
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
  std::vector<unsigned int> tmp = rawBank(1,0);
  for (int i=0; i<864;++i){
    if (tmp[i]!=original[i]) {
      std::cout<<"\t !!! L0Muon::ProcRawCnv::decodeBank !!! ERROR !!! line "<<i
               <<std::hex<<" 0x"<<tmp[i]<<" VS 0x"<<raw[i]<<std::dec<<std::endl;
    }
  }
  
}

std::vector<unsigned int> L0Muon::ProcRawCnv::rawBank(int bankVersion,  int ievt){
  //  std::cout<<"\t=> L0Muon::ProcRawCnv::rawBank IN : bankVersion: "<<bankVersion<<" ; ievt= "<<ievt<<std::endl;
  std::vector<unsigned int> raw;
  int event_number;
  int l0_bid;
  unsigned int event_num_word;
  int bcid;

  if (bankVersion<1) return raw;

  //  std::cout<<"\t=> L0Muon::ProcRawCnv::rawBank Starting ... "<<std::endl;
 
//   int event_number = ievt;
//   int l0_bid = ievt;
//   int bcid = ievt&0xF;
//   unsigned int event_num_word = ((event_number<<16)&0xFFF0000 )+ ( l0_bid&0xFFF );

  // Loop over processing boards 
  for (int ib=0; ib<12; ++ib) {

    //  std::cout<<"\t=> L0Muon::ProcRawCnv::rawBank    processing board "<<ib<<std::endl;
    
    event_number   = m_L0EventNumber[ib][0];
    l0_bid         = m_L0_B_Id[ib][0];
    event_num_word = ((event_number<<16)&0xFFF0000 )+ ( l0_bid&0xFFF );
    raw.push_back(event_num_word);
    //     std::cout<<"\t=> L0Muon::ProcRawCnv::rawBank  event_num_word written 0x"<< std::hex<<raw.back()<<std::dec<<std::endl;

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
      
      add[icand] = ( (colM3<<13)&0x6000 ) + ( (rowM3<<8)&0x0F00 ) + ( (offM2<<4)&0xF0 ) + ( offM1&0xF );
      pt[icand]  = ( (charge<<7)&0x80 ) + abspt ;
      pu[icand]  = m_candRegHandlerBCSU[ib].getCandPU(icand);
    }
    int status = m_candRegHandlerBCSU[ib].getStatus();

    word = ( ( (pt[0]<<24)&0xFF000000 ) + ( (pt[1]<<16)&0xFF0000 ) + ( (add[0])&0x7FFF ) );
    raw.push_back(word);

    bcid = m_BCID_BCSU[ib];
    word = ( (add[1]<<16)&0x7FFF0000 ) + ( (pu[0]<<10)&0xC00 ) + ( (pu[1]<<8)&0x200 ) + ( (status<<4)&0xF0 ) + (bcid&0xF);
    raw.push_back(word);
 
//     std::cout<<"\t=> L0Muon::ProcRawCnv::rawBank     BCSU candidates : "<<ib<<" "
//              <<"status 0x"<<std::hex<<status<<" bcid 0x"<<bcid<<std::dec<<std::endl;
    //  std::cout<<"\t=> L0Muon::ProcRawCnv::rawBank     BCSU candidates done "<<raw.size()<<" words"<<std::endl;
   
    // Loop over PUs candidates
    for (int ipu = 0; ipu<4; ++ipu) {
      for (int icand=0; icand<2; ++icand) {

        int colM3  = m_candRegHandlerPU[ib][ipu].getCandColM3(icand);
        int rowM3  = m_candRegHandlerPU[ib][ipu].getCandRowM3(icand);
        int offM1  = m_candRegHandlerPU[ib][ipu].getCandOffM1(icand);
        int offM2  = m_candRegHandlerPU[ib][ipu].getCandOffM2(icand);
        int abspt  = m_candRegHandlerPU[ib][ipu].getCandPT(icand);
        int charge = m_candRegHandlerPU[ib][ipu].getCandCharge(icand);
      
        add[icand] = ( (colM3<<13)&0x6000 ) + ( (rowM3<<8)&0x0F00 ) + ( (offM2<<4)&0xF0 ) + ( offM1&0xF );
        pt[icand]  = ( (charge<<7)&0x80 ) + abspt ;
        pu[icand]  = m_candRegHandlerPU[ib][ipu].getCandPU(icand);
      }
      int status = m_candRegHandlerPU[ib][ipu].getStatus();
      bcid = m_BCID_PU[ib][ipu];
//       std::cout<<"\t=> L0Muon::ProcRawCnv::rawBank     PU candidates : "<<ib<<" "<<ipu<<" "
//                <<"status 0x"<<std::hex<<status<<" bcid 0x"<<bcid<<std::dec<<std::endl;

      word = ( ( (pt[0]<<24)&0xFF000000 ) + ( (pt[1]<<16)&0xFF0000 ) + ( (add[0])&0x7FFF ) );
      raw.push_back(word);

      word = ( (add[1]<<16)&0x7FFF0000 ) + ( (status<<4)&0xF0 ) + (bcid&0xF);
      raw.push_back(word);
      
      //  std::cout<<"\t=> L0Muon::ProcRawCnv::rawBank        after pu"<<ipu<<" =>"<<raw.size()<<" words"<<std::endl;
      
    }// End of loop over PUs candidates
    
    //  std::cout<<"\t=> L0Muon::ProcRawCnv::rawBank     PU candidates done : "<<raw.size()<<" words"<<std::endl;

    // Loop over PUs input data
    for (int ipu = 0; ipu<4; ++ipu) {

      boost::dynamic_bitset<> olbitset(10*16);
      boost::dynamic_bitset<> neighbitset(17*16);
      
      if (m_ols[ib][ipu]   !=0) olbitset    = m_ols[ib][ipu]->getBitset();
      if (m_neighs[ib][ipu]!=0) neighbitset = m_neighs[ib][ipu]->getBitset();

      int size;
      boost::dynamic_bitset<> bitset;
      
      if (ipu%2==0) {
        int opt = opt_link_error[ib][ipu];
        int ser = ser_link_error[ib][ipu];
        int par = par_link_error[ib][ipu];
        word=( ((opt<<16)&0xFFFF0000) +((ser<<8)&0x3F00) + ((par<<0)&0xFF) );
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

//         for (int i=0; i<8; ++i) {
//           size=32*(7-i);
//           bitset = (neighbitset>>size);
//           bitset.resize(32);
//           word = bitset.to_ulong();
//           raw.push_back(word);
//         }

//         bitset = (neighbitset>>(32*8));
//         bitset.resize(16);
//         word = ( ( bitset.to_ulong()<<16 ) & 0xFFFF0000 );

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

        int opt = opt_link_error[ib][ipu];
        int ser = ser_link_error[ib][ipu];
        int par = par_link_error[ib][ipu];
        word+= (opt&0xFFFF);
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
      //  std::cout<<"\t=> L0Muon::ProcRawCnv::rawBank        after pu"<<ipu<<" =>"<<raw.size()<<" words"<<std::endl;

    }// End of loop over PUs input data


    
    // Insert missing words 
    std::vector<unsigned int>::iterator itraw;
    
    // insert empty word at the end of each frame
    word=0;
    itraw= raw.begin() + ib*L0Muon::ProcRawCnv::board_full_data_size+L0Muon::ProcRawCnv::board_full_frame_size-1;
    raw.insert(itraw,word);
    raw.push_back(word);

    // Insert the line with the event numbers
    itraw= raw.begin() + ib*L0Muon::ProcRawCnv::board_full_data_size+L0Muon::ProcRawCnv::board_full_frame_size;
    event_number   = m_L0EventNumber[ib][1];
    l0_bid         = m_L0_B_Id[ib][1];
    event_num_word = ((event_number<<16)&0xFFF0000 )+ ( l0_bid&0xFFF );
    raw.insert(itraw,event_num_word);

//     std::cout<<"\t=> L0Muon::ProcRawCnv::rawBank     PU input data done "<<raw.size()<<" words"<<std::endl;

  }// End of loop over processing boards
  

  // Re-order the bank such that the second frame of each processing board is written first
  raw = reorderOLChannels(raw);

//   std::cout.setf(std::ios::uppercase) ;
//   std::cout<<"\t=> L0Muon::ProcRawCnv::rawBank -- dump raw bank of size: "<<raw.size()<<std::endl;
//   int icount=0;
//   for (std::vector<unsigned int>::iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
//     std::cout <<std::setw(3)<<icount<<std::hex<<"\t0x"<<std::setw(8)<<std::setfill('0')<<(*itraw)<<std::dec<<std::endl;
//     ++icount;
//   }
//   std::cout<<std::dec;
//   std::cout.unsetf(std::ios::uppercase);
  
  return raw;
}

std::vector<unsigned int> L0Muon::ProcRawCnv::reorderOLChannels(std::vector<unsigned int> raw){

  std::vector<unsigned int> tmp;
  std::vector<unsigned int>::iterator itpos;
  std::vector<unsigned int>::iterator itstart;
  std::vector<unsigned int>::iterator itstop;
  
  for (int ib=0; ib<12; ++ib) {
    for (int ich=1; ich>-1; --ich){
      itstart = raw.begin()+(ib*2+ich)*L0Muon::ProcRawCnv::board_full_frame_size;
      itstop  = itstart+L0Muon::ProcRawCnv::board_full_frame_size;
      itpos   = tmp.end();
      tmp.insert(itpos,itstart,itstop);
    }
  }
  return tmp;
}
