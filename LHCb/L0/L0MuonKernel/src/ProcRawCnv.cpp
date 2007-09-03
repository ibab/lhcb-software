#include "L0MuonKernel/ProcRawCnv.h"

#include "ProcessorKernel/RegisterFactory.h"
#include "ProcessorKernel/Register.h"

#include "L0MuonKernel/BankUtilities.h"

/**
   Constructor.
*/
L0Muon::ProcRawCnv::ProcRawCnv(){
  m_quarter=0;
};
/**
   Constructor.
*/
L0Muon::ProcRawCnv::ProcRawCnv(int quarter){
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

  std::cout <<"L0Muon::ProcRawCnv::dump "<<tab<<"evt#: "<<ievt<<" - Q"<<m_quarter+1<<std::endl;
    
  std::cout <<"L0Muon::ProcRawCnv::dump "<<tab;
  std::cout <<"  ";
  for (int ib=0; ib<12; ++ib) {
    std::cout <<"     "<<std::setw(2)<<ib<<"    ";
  }
  std::cout <<std::endl;

  std::cout.setf(std::ios::uppercase) ;

  for (int iline=0; iline<70; ++iline) {
    
    std::cout <<"L0Muon::ProcRawCnv::dump "<<tab;
    std::cout <<std::setw(2)<<iline;
    std::cout<<std::hex;
    for (int ib=0; ib<12; ++ib) {
      std::cout <<" 0x"<<std::setw(8)<<std::setfill('0')<<raw[70*ib+iline];
    }
    std::cout<<std::dec;
    std::cout <<std::endl;
  }
  
  std::cout <<"L0Muon::ProcRawCnv::dump ----"<<std::endl;  
  std::cout.unsetf(std::ios::uppercase);
}


void L0Muon::ProcRawCnv::decodeBank(std::vector<unsigned int> raw, int bankVersion){
  if (bankVersion<1) return;
  unsigned int word;
  int bcid;
  
  // Clear the registers first
  release();

//   std::cout.setf(std::ios::uppercase) ;
//   std::cout<<"\t=> L0Muon::ProcRawCnv::decodeBankV1 -- dump raw bank of size: "<<raw.size()<<std::hex<<std::endl;
//   for (std::vector<unsigned int>::iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
//     std::cout <<"\t0x"<<std::setw(8)<<(*itraw)<<std::endl;
//   }
//   std::cout<<std::dec;
//   std::cout.unsetf(std::ios::uppercase);

//   //   unsigned int header = raw[0];

  // Loop over processing boards 
  for (int ib=0; ib<12; ++ib) {

    word = raw[ib*70];
    m_candRegHandlerBCSU[ib].setCandOffM1( ( (word<< 0)&0x0F ) ,0);
    m_candRegHandlerBCSU[ib].setCandOffM2( ( (word<< 4)&0x0F ) ,0);
    m_candRegHandlerBCSU[ib].setCandRowM3( ( (word<< 8)&0x1F ) ,0);
    m_candRegHandlerBCSU[ib].setCandColM3( ( (word<<13)&0x03 ) ,0);
    m_candRegHandlerBCSU[ib].setCandPT(    ( (word<<24)&0x7F ) ,0);
    m_candRegHandlerBCSU[ib].setCandCharge(( (word<<30)&0x01 ) ,0);
    m_candRegHandlerBCSU[ib].setCandPT(    ( (word<<16)&0x7F ) ,1);
    m_candRegHandlerBCSU[ib].setCandCharge(( (word<<23)&0x01 ) ,1);
    
    word = raw[ib*70+1];
    bcid = (word<< 0)&0x0F;
    m_candRegHandlerBCSU[ib].setStatus(    ( (word<< 4)&0x0F ) );
    m_candRegHandlerBCSU[ib].setCandPU(    ( (word<< 8)&0x03 ) ,1);
    m_candRegHandlerBCSU[ib].setCandPU(    ( (word<<10)&0x03 ) ,0);
    m_candRegHandlerBCSU[ib].setCandOffM1( ( (word<<16)&0x0F ) ,1);
    m_candRegHandlerBCSU[ib].setCandOffM2( ( (word<<20)&0x0F ) ,1);
    m_candRegHandlerBCSU[ib].setCandRowM3( ( (word<<24)&0x1F ) ,1);
    m_candRegHandlerBCSU[ib].setCandColM3( ( (word<<29)&0x03 ) ,1);

    // Loop over PUs candidates
    for (int ipu = 0; ipu<4; ++ipu) {

      word = raw[ib*70+2+ipu*2];
      m_candRegHandlerBCSU[ib].setCandOffM1( ( (word<< 0)&0x0F ) ,0);
      m_candRegHandlerBCSU[ib].setCandOffM2( ( (word<< 4)&0x0F ) ,0);
      m_candRegHandlerBCSU[ib].setCandRowM3( ( (word<< 8)&0x1F ) ,0);
      m_candRegHandlerBCSU[ib].setCandColM3( ( (word<<13)&0x03 ) ,0);
      m_candRegHandlerBCSU[ib].setCandPT(    ( (word<<24)&0x7F ) ,0);
      m_candRegHandlerBCSU[ib].setCandCharge(( (word<<30)&0x01 ) ,0);
      m_candRegHandlerBCSU[ib].setCandPT(    ( (word<<16)&0x7F ) ,1);
      m_candRegHandlerBCSU[ib].setCandCharge(( (word<<23)&0x01 ) ,1);
    
      word = raw[ib*70+3+ipu*2];
      bcid = (word<< 0)&0x0F;
      m_candRegHandlerBCSU[ib].setStatus(    ( (word<< 4)&0x0F ) );
      m_candRegHandlerBCSU[ib].setCandOffM1( ( (word<<16)&0x0F ) ,1);
      m_candRegHandlerBCSU[ib].setCandOffM2( ( (word<<20)&0x0F ) ,1);
      m_candRegHandlerBCSU[ib].setCandRowM3( ( (word<<24)&0x1F ) ,1);
      m_candRegHandlerBCSU[ib].setCandColM3( ( (word<<29)&0x03 ) ,1);

      m_candRegHandlerBCSU[ib].setCandPU( ipu ,0);
      m_candRegHandlerBCSU[ib].setCandPU( ipu ,1);

    }// End of loop over PUs candidates

    int iwd = ib*70+11;
    // Loop over PUs input data
    for (int ipu = 0; ipu<4; ++ipu) {

      boost::dynamic_bitset<> rawbitset;
      int rawsize ;
      
      if (ipu%2==0) {
        
        // 
        // Signalization field
        // 
        ++iwd;

        // 
        // OL data field
        // 

        for (int i=0; i<5; ++i) { // 5 full words
          boost::dynamic_bitset<> bitset(32,raw[iwd]);
          rawsize = rawbitset.size();
          rawbitset.resize(rawsize+32);
          bitset.resize(rawsize+32);
          bitset<<=rawsize;
          rawbitset|=bitset;
          ++iwd;
        }
        
        if (m_ols[ib][ipu]!=0) m_ols[ib][ipu]->set(rawbitset);
        rawbitset.clear();

        // 
        // Neighbour data field
        // 

        for (int i=0; i<8; ++i) { // 8 full words
          boost::dynamic_bitset<> bitset(32,raw[iwd]);
          rawsize = rawbitset.size();
          rawbitset.resize(rawsize+32);
          bitset.resize(rawsize+32);
          bitset<<=rawsize;
          rawbitset|=bitset;
          ++iwd;
        }

        
        {                          // 16 bits (msb) of last word
          boost::dynamic_bitset<> bitset(16,( (raw[iwd]<<16)&0xFFFF0000 ) );
          rawsize = rawbitset.size();
          rawbitset.resize(rawsize+16);
          bitset.resize(rawsize+16);
          bitset<<=rawsize;
          rawbitset|=bitset;
        }
        
        if (m_neighs[ib][ipu]!=0) m_neighs[ib][ipu]->set(rawbitset);   
        rawbitset.clear();

      } else {
        
        // 
        // Signalization field
        // 
        ++iwd;

        //
        // OL data field
        // 

        {                          // 16 bits (lsb) of previous word
          boost::dynamic_bitset<> bitset(16,raw[iwd]);
          rawsize = rawbitset.size();
          rawbitset.resize(rawsize+16);
          bitset.resize(rawsize+16);
          bitset<<=rawsize;
          rawbitset|=bitset;
          ++iwd;
        }
        
        
        for (int i=0; i<4; ++i) { // 4 full words
          if (iwd%35 ==0 ) ++iwd; // skip the line with the event numbers
          boost::dynamic_bitset<> bitset(32,raw[iwd]);
          rawsize = rawbitset.size();
          rawbitset.resize(rawsize+32);
          bitset.resize(rawsize+32);
          bitset<<=rawsize;
          rawbitset|=bitset;
          ++iwd;
        }

        {                         // 16 bits (msb) of last word
          boost::dynamic_bitset<> bitset(16,( (raw[iwd]<<16)&0xFFFF0000 ) );
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

        { // 16 bits (lsb) of previous word
          boost::dynamic_bitset<> bitset(16,raw[iwd]);
          rawsize = rawbitset.size();
          rawbitset.resize(rawsize+16);
          bitset.resize(rawsize+16);
          bitset<<=rawsize;
          rawbitset|=bitset;
          ++iwd;
        }
        
        for (int i=0; i<8; ++i) { // 8 full words
          boost::dynamic_bitset<> bitset(32,raw[iwd]);
          rawsize = rawbitset.size();
          rawbitset.resize(rawsize+32);
          bitset.resize(rawsize+32);
          bitset<<=rawsize;
          rawbitset|=bitset;
          ++iwd;
        }

        if (m_neighs[ib][ipu]!=0) m_neighs[ib][ipu]->set(rawbitset);   
        rawbitset.clear();

      }

    }// End of loop over PUs input data

  }// End of loop over processing boards

}

std::vector<unsigned int> L0Muon::ProcRawCnv::rawBank(int bankVersion,  int ievt){
  //  std::cout<<"\t=> L0Muon::ProcRawCnv::rawBank IN : bankVersion: "<<bankVersion<<" ; ievt= "<<ievt<<std::endl;
  std::vector<unsigned int> raw;

  if (bankVersion<1) return raw;

  //  std::cout<<"\t=> L0Muon::ProcRawCnv::rawBank Starting ... "<<std::endl;
 
  int event_number = ievt;
  int l0_bid = ievt;
  int bcid = ievt&0xF;

  unsigned int event_num_word = ( (l0_bid<<16)&0xFFF0000 )+ ( event_number&0xFFF );
  
  // Loop over processing boards 
  for (int ib=0; ib<12; ++ib) {

    //  std::cout<<"\t=> L0Muon::ProcRawCnv::rawBank    processing board "<<ib<<std::endl;
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
      
      add[icand] = ( (colM3<<13)&0x6000 ) + ( (rowM3<<8)&0x0F00 ) + ( (offM2<<4)&0xF0 ) + ( offM1&0xF );
      pt[icand]  = ( (charge<<7)&0x80 ) + abspt ;
      pu[icand]  = m_candRegHandlerBCSU[ib].getCandPU(icand);
    }
    int status = m_candRegHandlerBCSU[ib].getStatus();

    word = ( ( (pt[0]<<24)&0xFF000000 ) + ( (pt[1]<<16)&0xFF0000 ) + ( (add[0])&0x7FFF ) );
    raw.push_back(word);

    word = ( (add[1]<<16)&0x7FFF0000 ) + ( (pu[0]<<10)&0xC00 ) + ( (pu[1]<<8)&0x200 ) + ( (status<<4)&0xF0 ) + bcid;
    raw.push_back(word);
 
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

      word = ( ( (pt[0]<<24)&0xFF000000 ) + ( (pt[1]<<16)&0xFF0000 ) + ( (add[0])&0x7FFF ) );
      raw.push_back(word);

      word = ( (add[1]<<16)&0x7FFF0000 ) + ( (status<<4)&0xF0 ) + bcid;
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

        word=0;
        raw.push_back(word); // Signalization field

        // 
        // OL data field
        // 

        for (int i=0; i<5; ++i) {
          size=32*i;
          bitset = (olbitset>>size);
          bitset.resize(32);
          word = bitset.to_ulong();
          raw.push_back(word);
        }

        // 
        // Neighbour data field
        // 

        for (int i=0; i<8; ++i) {
          size=32*i;
          bitset = (neighbitset>>size);
          bitset.resize(32);
          word = bitset.to_ulong();
          raw.push_back(word);
        }

        bitset = (neighbitset>>(32*8));
        bitset.resize(16);
        word = ( ( bitset.to_ulong()<<16 ) & 0xFFFF0000 );
        
      } else {

        raw.push_back(word); //  bits[0-15] are the last bits from previous PU 
        word=0;

        // 
        // OL data field
        // 

        bitset = (olbitset>>(0));
        bitset.resize(16);
        word |= ( ( bitset.to_ulong() ) & 0xFFFF );
        raw.push_back(word);

        for (int i=0; i<4; ++i) {
          size= 16 + 32*i ;
          bitset = (olbitset>>size);
          bitset.resize(32);
          word = bitset.to_ulong();
          raw.push_back(word);
        }
        
        bitset = (olbitset>>( (32*4) +16 ) );
        bitset.resize(16);
        word = ( ( bitset.to_ulong() <<16) & 0xFFFF0000 );
        
        // 
        // Neighbour data field
        // 

        bitset = (neighbitset>>(0));
        bitset.resize(16);
        word |= ( ( bitset.to_ulong() ) & 0xFFFF );
        raw.push_back(word);

        for (int i=0; i<8; ++i) {
          size= 16 + 32*i ;
          bitset = (neighbitset>>size);
          bitset.resize(32);
          word = bitset.to_ulong();
          raw.push_back(word);
          
        }

      }
      //  std::cout<<"\t=> L0Muon::ProcRawCnv::rawBank        after pu"<<ipu<<" =>"<<raw.size()<<" words"<<std::endl;

    }// End of loop over PUs input data

    //  std::cout<<"\t=> L0Muon::ProcRawCnv::rawBank     PU input data done "<<raw.size()<<" words"<<std::endl;

    // Insert the line with the event numbers
    std::vector<unsigned int>::iterator itraw = raw.begin();
    itraw+=ib*70+35;
    raw.insert(itraw,event_num_word);
    
  }// End of loop over processing boards
  
//   std::cout.setf(std::ios::uppercase) ;
//   std::cout<<"\t=> L0Muon::ProcRawCnv::rawBank -- dump raw bank of size: "<<raw.size()<<std::hex<<std::endl;
//   for (std::vector<unsigned int>::iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
//     std::cout <<"\t0x"<<std::setw(8)<<(*itraw)<<std::endl;
//   }
//   std::cout<<std::dec;
//   std::cout.unsetf(std::ios::uppercase);
  
//   std::cout<<"\t=> L0Muon::ProcRawCnv::rawBank OUT "<<std::endl;

  return raw;
}


