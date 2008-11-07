#include "L0MuonKernel/ProcDataCnv.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "Kernel/MuonLayout.h"
#include <math.h>

#define _DET_SPEC_HEADER_ 1
#define _DEBUG_PROCDATA_  0
/**
   Constructor.
*/
L0Muon::ProcDataCnv::ProcDataCnv(){
};
/**
   Constructor.
*/
L0Muon::ProcDataCnv::ProcDataCnv(int quarter){
  m_quarter=quarter;

  char buf[4096];
  char* format ;

  L0Muon::RegisterFactory* rfactory = L0Muon::RegisterFactory::instance();

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
  
};

/**
   Destructor
*/
L0Muon::ProcDataCnv::~ProcDataCnv(){
}


LHCb::MuonTileID L0Muon::ProcDataCnv::mid_BCSU(int ib){
  MuonLayout lay(1,1);
  LHCb::MuonTileID board = LHCb::MuonTileID(0);
  board.setQuarter(m_quarter);
  board.setLayout(lay);
  board.setRegion(ib/3);
  board.setX( (((ib%3)+1)>>0)&1);
  board.setY( (((ib%3)+1)>>1)&1);
  return board;
}

LHCb::MuonTileID L0Muon::ProcDataCnv::mid_PU(int ib, int ipu){
  LHCb::MuonTileID board = mid_BCSU(ib);
  MuonLayout lay(2,2);
  LHCb::MuonTileID pu = LHCb::MuonTileID(board,lay,(ipu>>0)&1,(ipu>>1)&1);
  return pu;
}

void L0Muon::ProcDataCnv::release(){
  for (int iboard = 0; iboard <12 ; iboard++) {
    for (int ipu = 0; ipu <4 ; ipu++) { 
      if (m_ols[iboard][ipu]!=0) m_ols[iboard][ipu]->reset();
      if (m_neighs[iboard][ipu]!=0) m_neighs[iboard][ipu]->reset();
    }    
  }
}

std::vector<LHCb::MuonTileID> L0Muon::ProcDataCnv::ols(LHCb::MuonTileID puid)
{
  std::vector<LHCb::MuonTileID> pads;

  //   std::cout <<"L0Muon::ProcDataCnv::ols input PU "<<puid.toString()<<std::endl;
  
  std::map<LHCb::MuonTileID, TileRegister*>::iterator it = m_olsMap.find(puid);
  if (it!=m_olsMap.end()) {
    if ((*it).second!=0) {
      pads = ((*it).second)->firedTiles();
    }    
  }
  //   if (it==m_olsMap.end()) {
  //     std::cout <<"L0Muon::ProcDataCnv::ols PU not found in ol map"<<std::endl;
  //     return pads;
  //   } 
  //   if ((*it).second==0) {
  //     std::cout <<"L0Muon::ProcDataCnv::ols no tile register associated"<<std::endl;
  //     return pads;
  //   }
  
  //   pads = ((*it).second)->firedTiles();
  //   std::cout <<"L0Muon::ProcDataCnv::ols "<<pads.size()<<" pads found"<<std::endl;
  return pads;
}
    
std::vector<LHCb::MuonTileID> L0Muon::ProcDataCnv::neighs(LHCb::MuonTileID puid)   
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
std::vector<LHCb::MuonTileID>  L0Muon::ProcDataCnv::pus()
{
  std::vector<LHCb::MuonTileID> lpuids;
  std::map<LHCb::MuonTileID, TileRegister*>::iterator itmap;
  for (itmap=m_olsMap.begin(); itmap!=m_olsMap.end(); ++itmap){
    lpuids.push_back(itmap->first);
  }
  
  return lpuids;
}


int L0Muon::ProcDataCnv::decodeBank(const std::vector<unsigned int> raw, int version)
{

  switch (version){
  case 1:
    return decodeBank_v1(raw);
    break;
  case 2:
    return decodeBank_v2(raw);
    break;
  };
  
  return 0;
}

int L0Muon::ProcDataCnv::decodeBank_v1(const std::vector<unsigned int> & raw)
{

  // Clear the registers first
  release();

  //   std::cout << "L0Muon::ProcDataCnv::decodeBank  bank size= "<<raw.size()<<" words"<<std::endl;
  //   std::cout.setf(std::ios::uppercase) ;
  //   std::cout<<"\t=> L0Muon::ProcDataCnv::decodeBank -- dump raw bank of size: "<<raw.size()<<std::hex<<std::endl;
  //   for (std::vector<unsigned int>::const_iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
  //     std::cout <<"\t0x"<<std::setw(8)<<(*itraw)<<std::endl;
  //   }
  //   std::cout<<std::dec;
  //   std::cout.unsetf(std::ios::uppercase);
  
  int header = raw[0];
  //   std::cout.setf(std::ios::uppercase) ;
  //   std::cout << "L0Muon::ProcDataCnv::decodeBank  header= 0x"<<std::setw(8)<<std::hex<<header<<std::dec<<std::endl;
  //   std::cout.unsetf(std::ios::uppercase);
  unsigned int nOLwords = (header & 0x3FFF)/4; // in words of 32 bytes
  //   std::cout << "L0Muon::ProcDataCnv::decodeBank  nOLwords= "<<nOLwords<<std::endl;

  //
  // Optical links
  //
 
  // Read the first nOLwords words of raw into a single bitset
  boost::dynamic_bitset<> rawbitset;
  for (unsigned int iwd=0; iwd<nOLwords; ++iwd){
    boost::dynamic_bitset<> bitset(32,raw[iwd+1]);
    int rawsize = rawbitset.size();
    rawbitset.resize(rawsize+32);
    bitset.resize(rawsize+32);
    bitset<<=rawsize;
    rawbitset|=bitset;
  }

  //
  // rawbitset contains the compressed optical link data for the full quarter.
  // CAUTION: it also contains the compressed error data bits (32 bits once decompressed)
  // - uncompressed the data for each processing board 
  // - fill the register for each PU of each board
  int padding=0;
  for (int iboard = 0; iboard <12 ; iboard++) {
 
    if (iboard%3==0) {
      padding = rawbitset.size();
    }

    boost::dynamic_bitset<> olbitset = unapplyCompression(rawbitset,4*12*16);
    for (int ipu = 0; ipu <4 ; ipu++) {
      boost::dynamic_bitset<> bitset = olbitset>>((ipu*12+2)*16);
      bitset.resize(10*16);
      if (m_ols[iboard][ipu]!=0) 
        m_ols[iboard][ipu]->set(bitset);        
    }

    if ((iboard+1)%3==0) {
      padding -= rawbitset.size();
      padding  = ( 4 - (padding%4) )% 4;
      rawbitset = rawbitset>>padding;
      int size=rawbitset.size();
      rawbitset.resize(size-padding);
    }

  }
  
  if (nOLwords+1==raw.size()) return 1; // raw bank stops here
  
  //
  // Neighbours
  //

  rawbitset.clear();
  for (unsigned int iwd=nOLwords+1; iwd<raw.size(); ++iwd){
    boost::dynamic_bitset<> bitset(32,raw[iwd]);
    int rawsize = rawbitset.size();
    rawbitset.resize(rawsize+32);
    bitset.resize(rawsize+32);
    bitset<<=rawsize;
    rawbitset|=bitset;
  }

  // rawbitset contains the compressed neighbours data for the full quarter.
  // - uncompressed the data for each processing board
  // - fill the register for each PU of each board
  padding=0;
  for (int iboard = 0; iboard <12 ; iboard++) {

    if (iboard%3==0) {
      padding = rawbitset.size();
    }

    boost::dynamic_bitset<> neighbitset = unapplyCompression(rawbitset,4*17*16);
    for (int ipu = 0; ipu <4 ; ipu++) {
      boost::dynamic_bitset<> bitset = neighbitset>>(ipu*17*16);
      bitset.resize(17*16);
      if (m_neighs[iboard][ipu]!=0)
        m_neighs[iboard][ipu]->set(bitset);
    }

    if ((iboard+1)%3==0) {
      padding -= rawbitset.size(); // size of the 3 processing boards part that have just been decompressed
      padding  = ( 4 - (padding%4) )% 4;
      rawbitset = rawbitset>>padding;
      int size=rawbitset.size();
      rawbitset.resize(size-padding);
    }

  }

  return 2;
}

int L0Muon::ProcDataCnv::decodeBank_v2(const std::vector<unsigned int> & raw)
{

  // Decode the L0MuonProcData bank version 2
  // 
  // This bank contains 2 parts :
  //  o 1st part : hardware error detected on PU input links + PU optical link data     
  //  o 2nd part (not always present) : PU neighbour data 
  //
  // Return values :
  //  o -1 : in case of error in the 1st part
  //  o -2 : in case of error in the 1st part
  //  o  1 : only 1st first part was decoded
  //  o  2 : the two parts were decoded 

  // Clear the registers first
  release();

  bool compressed =  false;
#if _DET_SPEC_HEADER_==1
  static unsigned int nheader =1;
  //  unsigned int nOLwords= (raw[0] & 0xFFFF)/2; 
  unsigned int nOLbytes= (raw[0] & 0xFFFF);
  unsigned int nOLwords= (nOLbytes+2)/4;
  compressed = (raw[0]>>31)&1;
#else
  static unsigned int nheader = 0;
  unsigned int nOLbytes = PB_OpticalLinks_size*12*4;
  unsigned int nOLwords = PB_OpticalLinks_size*12;
#endif

#if _DEBUG_PROCDATA_ >0
  std::cout<<"L0Muon::ProcDataCnv::decodeBank (v2) "
           <<" nheader= "<<nheader
           <<" nOLbytes= "<<nOLbytes
           <<" nOLwords= "<<nOLwords
           <<" compressed= "<<compressed
           <<std::endl;
#endif


  for (int ib=0; ib<12; ++ib)  m_errors[ib].present.set(1);
  
  //
  //--- 1st part 
  //

  // By default set the decoding error flag ON
  int decoding_error[12];
  for (int ib=0; ib<12; ++ib)  decoding_error[ib]=1;
  
  //for (int ib=0; ib<12; ++ib)  m_errors[ib].decoding.set(1);

  
  unsigned int iwd=nheader;
  int ibit=-1;
  for (int ib =0; ib<12; ++ib) 
  { // Loop over processing boards

    int ok;
    boost::dynamic_bitset<> bitset;
#if _DEBUG_PROCDATA_ >0
    std::cout<<"L0Muon::ProcDataCnv::decodeBank OLs board# "<<ib
             <<" BEFORE DECODING"
             <<" bitset size= "<<bitset.size()
             <<" first_word ="<<iwd
             <<" first_bit ="<<ibit
             <<std::endl;
#endif
    if (compressed)
      ok=compressedPBWords_to_bitset(raw, PB_OpticalLinks_size, iwd, ibit, bitset);
    else
      ok=notcompressedPBWords_to_bitset(raw, PB_OpticalLinks_size, iwd, bitset);
    if (ok<0) {
      //ERROR
#if _DEBUG_PROCDATA_ >0
      std::cout<<"L0Muon::ProcDataCnv::decodeBank ERROR PBWords_to_bitset returned "<<ok
               <<" OLs board# "<<ib
               <<" bitset size= "<<bitset.size()
               <<" last_word  ="<<iwd
               <<" last_bit  ="<<ibit
               <<std::endl;
#endif
      for (int jb=0; jb<12; ++jb)  m_errors[jb].decoding.set(decoding_error[jb]);
      return -1;
    }
#if _DEBUG_PROCDATA_ >0
    std::cout<<"L0Muon::ProcDataCnv::decodeBank OLs board# "<<ib
             <<" AFTER  DECODING"
             <<" bitset size= "<<bitset.size()
             <<" last_word  ="<<iwd
             <<" last_bit  ="<<ibit
             <<std::endl;
#endif
    
    ok=setRegisters_for_PB_OpticalLinks(ib,bitset);
    if (ok<0) {
      //ERROR
#if _DEBUG_PROCDATA_ >0
      std::cout<<"L0Muon::ProcDataCnv::decodeBank ERROR setRegisters_for_PB_OpticalLinks returned "<<ok
               <<" OLs board# "<<ib
               <<std::endl;
#endif
      for (int ib=0; ib<12; ++ib)  m_errors[ib].decoding.set(decoding_error[ib]);
      return -1;
    }

    // if there is no neighbour data, the decoding is complete for this board.
    // Thus, set the decoding error flag to FALSE.
    //if ((nOLwords+nheader)<=raw.size()) m_errors[ib].decoding.set(0);
    if ((nOLwords+nheader)<=raw.size()) decoding_error[ib]=0;
    
    if (((ib+1)%3)==0) 
    { // take care of padding bits at the end of each PP
      ibit=16*((ibit+16)/16)-1;
#if _DEBUG_PROCDATA_ >0
    std::cout<<"L0Muon::ProcDataCnv::decodeBank OLs board# "<<ib
             <<" AFTER   PADDING"
             <<" bitset size= "<<bitset.size()
             <<" last_word  ="<<iwd
             <<" last_bit  ="<<ibit
             <<std::endl;
#endif

    } // end padding
  } // End loop over processing boards
  
#if _DEBUG_PROCDATA_ >0
  std::cout<<"L0Muon::ProcDataCnv::decodeBank "
           <<" OL decoding done ..."
           <<" last_word  ="<<iwd
           <<" last_bit  ="<<ibit
           <<std::endl;
#endif
  
  // Skip the end of the word after first part of bank 
  if (ibit!=-1) { // if no compression, we are already at next word
    ++iwd;
    ibit=-1;
  }

  if (iwd!=(nOLwords+nheader)) {
    // ERROR 
#if _DEBUG_PROCDATA_ >0
    std::cout<<"L0Muon::ProcDataCnv::decodeBank ERROR  end of 1st part current word ("<<iwd
             <<") doesn't match 1st part size ("<<(nOLwords+nheader)<<")"
             <<std::endl;
#endif
    for (int jb=0; jb<12; ++jb)  m_errors[jb].decoding.set(decoding_error[jb]);
    return -1;
  }

  if (iwd==raw.size()) {
    // the 2nd part was not activated : stop here
    for (int jb=0; jb<12; ++jb)  m_errors[jb].decoding.set(decoding_error[jb]);
    return 1;
  }
  
  //
  //--- 2nd part 
  //
  
  for (int ib =0; ib<12; ++ib) 
  { // Loop over processing boards
    int ok;
    boost::dynamic_bitset<> bitset;
#if _DEBUG_PROCDATA_ >0
    std::cout<<"L0Muon::ProcDataCnv::decodeBank Neigh board# "<<ib
             <<" BEFORE DECODING"
             <<" bitset size= "<<bitset.size()
             <<" first_word ="<<iwd
             <<" first_bit ="<<ibit
             <<std::endl;
#endif
    if (compressed)
      ok=compressedPBWords_to_bitset(raw, PB_Neighbours_size, iwd, ibit, bitset);
    else
      ok=notcompressedPBWords_to_bitset(raw, PB_Neighbours_size, iwd, bitset);
    if (ok<0) {
      //ERROR
#if _DEBUG_PROCDATA_ >0
      std::cout<<"L0Muon::ProcDataCnv::decodeBank ERROR PBWords_to_bitset returned "<<ok
               <<" Neigh board# "<<ib
               <<" bitset size= "<<bitset.size()
               <<" last_word  ="<<iwd
               <<" last_bit  ="<<ibit
               <<std::endl;
#endif
      for (int jb=0; jb<12; ++jb)  m_errors[jb].decoding.set(decoding_error[jb]);
      return -2;
    }
#if _DEBUG_PROCDATA_ >0
    std::cout<<"L0Muon::ProcDataCnv::decodeBank Neigh board# "<<ib
             <<" AFTER  DECODING"
             <<" bitset size= "<<bitset.size()
             <<" last_word  ="<<iwd
             <<" last_bit  ="<<ibit
             <<std::endl;
#endif
    
    ok=setRegisters_for_PB_Neighbours(ib,bitset);

    if (ok<0) {
      //ERROR
#if _DEBUG_PROCDATA_ >0
      std::cout<<"L0Muon::ProcDataCnv::decodeBank ERROR setRegisters_for_PB_Neighbours returned "<<ok
               <<" OLs board# "<<ib
               <<std::endl;
#endif
      for (int jb=0; jb<12; ++jb)  m_errors[jb].decoding.set(decoding_error[jb]);
      return -2;
    }

    // The decoding is now complete for this board.
    // Thus, set the decoding error flag to FALSE.
    //m_errors[ib].decoding.set(0);
    decoding_error[ib]=0;
    
    if (((ib+1)%3)==0) 
    { // take care of padding bits at the end of each PP
      //ibit=32*((ibit+32)/32)-1;
      ibit=31;
#if _DEBUG_PROCDATA_ >0
    std::cout<<"L0Muon::ProcDataCnv::decodeBank Neigh board# "<<ib
             <<" AFTER   PADDING"
             <<" bitset size= "<<bitset.size()
             <<" last_word  ="<<iwd
             <<" last_bit  ="<<ibit
             <<std::endl;
#endif
    } // end padding
  } // Enf of loop over processing boards


#if _DEBUG_PROCDATA_ >0
  std::cout<<"L0Muon::ProcDataCnv::decodeBank "
           <<" Neigh decoding done ..."
           <<" last_word  ="<<iwd
           <<" last_bit  ="<<ibit
           <<std::endl;
#endif

  // Skip the end of the word after first part of bank 
  if (ibit!=-1) { // if no compression, we are already at next word
    ++iwd;
    ibit=-1;
  }

  if (iwd!=raw.size()) {
    //ERROR
#if _DEBUG_PROCDATA_ >0
    std::cout<<"L0Muon::ProcDataCnv::decodeBank ERROR  end of 2nd part current word ("<<iwd
             <<") does'nt match raw bank size ("<<raw.size()<<")"
             <<std::endl;
#endif
    for (int jb=0; jb<12; ++jb)  m_errors[jb].decoding.set(decoding_error[jb]);
    return -2;
  }

  for (int jb=0; jb<12; ++jb)  m_errors[jb].decoding.set(decoding_error[jb]);
  return 2;

}


int L0Muon::ProcDataCnv::rawBank(std::vector<unsigned int> &raw, int version, int mode, bool compression)
{
  switch (version){
  case 1:
    return rawBank_v1(raw,mode);
    break;
  case 2:
    return rawBank_v2(raw,mode, compression);
    break;
  };
  
  return 0;
}

int L0Muon::ProcDataCnv::rawBank_v1(std::vector<unsigned int> &raw, int mode)
{

  raw.clear();
  unsigned int header = 0;
  raw.push_back(header);

  if (mode>0) {
    
    boost::dynamic_bitset<> rawbitset;
    // Loop over processing boards
    for (int iboard = 0; iboard <12 ; iboard++) {
      
      boost::dynamic_bitset<> olbitset;
      
      // Loop over PUs
      for (int ipu = 0; ipu <4 ; ipu++) {

        boost::dynamic_bitset<> pubitset;
        if (m_ols[iboard][ipu]!=0) 
        {
          pubitset = m_ols[iboard][ipu]->getBitset();
          pubitset.resize(12*16);
          pubitset = pubitset<<(2*16); // add the 2 error report words
        } else {
          pubitset.resize(12*16);
        }

        int pusize = pubitset.size();
        int olsize = olbitset.size();
        olbitset.resize(pusize+olsize); 
        pubitset.resize(pusize+olsize);
        pubitset<<=(olsize);
        olbitset|=pubitset;
      }// End of loop over PUs
 
      olbitset = applyCompression(olbitset);
      
      int size = olbitset.size();
      int rawsize = rawbitset.size();
      olbitset.resize(size+rawsize);
      rawbitset.resize(size+rawsize);
      olbitset<<=rawsize;
      rawbitset|=olbitset;
      
      if ((iboard+1)%3==0) { // end of treatment of the 3 boards connected to the same PP-FPGA of the TELL1
        int rawsize = rawbitset.size();
        int padding = ( 4 - (rawsize%4) )%4;
        rawbitset.resize(rawsize+padding);
      }

    }// End of loop over processing boards
  
    unsigned int size = 0;
    while(size<rawbitset.size()) {
      boost::dynamic_bitset<> bitset = (rawbitset>>size);
      bitset.resize(32);
      size+=32;
      unsigned int word = bitset.to_ulong();
      raw.push_back(word);
    }
  
    // update sub-detector specific header 
    header = (raw.size()-1)*4;// length in bytes
    raw[0]=header;

  } 

  if (mode>1) {
    
    boost::dynamic_bitset<> rawbitset;
    // Loop over processing boards
    for (int iboard = 0; iboard <12 ; iboard++) {
      
      boost::dynamic_bitset<> neighbitset;
      
      // Loop over PUs
      for (int ipu = 0; ipu <4 ; ipu++) {

        boost::dynamic_bitset<> pubitset;
        if (m_neighs[iboard][ipu]!=0) 
        {
          pubitset = m_neighs[iboard][ipu]->getBitset();
        } else {
          pubitset.resize(17*16);
        }

        int pusize    = pubitset.size();
        int neighsize = neighbitset.size();
        neighbitset.resize(pusize+neighsize);
        pubitset.resize(pusize+neighsize);
        pubitset<<=(neighsize);
        neighbitset|=pubitset;
      }// End of loop over PUs

      neighbitset = applyCompression(neighbitset);
      
      int size = neighbitset.size();
      int rawsize = rawbitset.size();
      neighbitset.resize(size+rawsize);
      rawbitset.resize(size+rawsize);
      neighbitset<<=rawsize;
      rawbitset|=neighbitset;

      if ((iboard+1)%3==0) { // end of treatment of the 3 boards connected to the same PP-FPGA of the TELL1
        int rawsize = rawbitset.size();
        int padding = ( 4 - (rawsize%4) )%4;
        rawbitset.resize(rawsize+padding);
      }

    }// End of loop over processing boards
  

    unsigned int size = 0;
    while(size<rawbitset.size()) {
      boost::dynamic_bitset<> bitset = (rawbitset>>size);
      bitset.resize(32);
      size+=32;
      unsigned int word = bitset.to_ulong();
      raw.push_back(word);
    }
  
  }

  //   std::cout << "L0Muon::ProcDataCnv::rawBank  ... done\n";
  
  return 1;
}


boost::dynamic_bitset<> L0Muon::ProcDataCnv::applyCompression(boost::dynamic_bitset<> bitset_to_compress){
  // Compress the bit set in input
  
  int nbitsPrefix  = 5;
  int nbitsSequence= nbitsPrefix+1;
  int maximumLength = int(pow(2,nbitsPrefix));

  int n0;
  int leftover;
  int size;
  int nchar;
  int shift;
  int  newsize;
  unsigned long  sequence;

  boost::dynamic_bitset<> compressedBitset;
  boost::dynamic_bitset<> encodedSequence;
  boost::dynamic_bitset<> leftoverSequence;

  boost::dynamic_bitset<>::size_type lastPos=0;
  --lastPos;
  
  boost::dynamic_bitset<>::size_type currentPos=bitset_to_compress.find_first();

  while (currentPos<bitset_to_compress.size()) {
  
    // Code the sequence of 0
    nchar      = currentPos-lastPos-1;
    n0         = nchar/maximumLength;
    leftover   = nchar%maximumLength;
    size       = n0 + nbitsSequence;
    sequence = (leftover<<1)+1;
    leftoverSequence= boost::dynamic_bitset<> (size,sequence);
    encodedSequence.resize(size);
    encodedSequence|=leftoverSequence<<n0;

    // Add the encoded sequence to the compressed bitset
    shift   = compressedBitset.size();
    newsize = shift +encodedSequence.size();
    compressedBitset.resize(newsize);
    encodedSequence.resize(newsize);
    compressedBitset|=encodedSequence<<shift;

    lastPos= currentPos;
    currentPos=bitset_to_compress.find_next(lastPos); 
    encodedSequence.clear();

  }

  // Code the last sequence of 0
  nchar = bitset_to_compress.size()-lastPos-1;
  n0         = nchar/maximumLength;
  leftover   = nchar%maximumLength;
  size       = n0;
  if (leftover>0) { // if the file ends with a number of 0s which is a multiple of maximumLength, do not write the last sequence
    size+=nbitsSequence;
    sequence = (leftover<<1)+1;
    leftoverSequence= boost::dynamic_bitset<>(size,sequence);
    encodedSequence.resize(size);
    encodedSequence|=leftoverSequence<<n0;
  } else {
    encodedSequence.resize(size);
  }
  
  
  // Add the encoded sequence to the compressed bitset
  shift   = compressedBitset.size();
  newsize = shift + encodedSequence.size();
  compressedBitset.resize(newsize);
  encodedSequence.resize(newsize);
  compressedBitset|=encodedSequence<<shift;

  return compressedBitset;
}


boost::dynamic_bitset<> L0Muon::ProcDataCnv::unapplyCompression(boost::dynamic_bitset<> & compressed, unsigned int length){
  

  boost::dynamic_bitset<> uncompressed(0);  

  // if the compressed bistset is empty, return immediatly
  unsigned int uncompressedSize = 0;
  if (compressed.size()==0) {
    return uncompressed;    
  }
  
  int nbitsPrefix  = 5;
  int nbitsSequence= nbitsPrefix+1;
  int maximumLength = int(pow(2,nbitsPrefix));

  // if the no bits are set in the compressed bistset, return the corresponding sequence
  unsigned int compressedSize = compressed.size();
  if (compressed.count()==0) {
    compressed>>=(length/maximumLength);
    uncompressed.resize(length);
    return uncompressed;
  }

  boost::dynamic_bitset<>::size_type lastPos=0;
  boost::dynamic_bitset<>::size_type currentPos=compressed.find_first();

  while (currentPos<compressed.size()) {

    int n0 =  (currentPos - lastPos) * maximumLength;
    boost::dynamic_bitset<> number = (compressed>>(currentPos+1));
    // the number is
    number.resize(nbitsPrefix);
    int n0tot = n0 + number.to_ulong();

    if ( (n0+uncompressedSize) >= length) {

      currentPos = lastPos+(length-uncompressedSize)/maximumLength;
      compressed>>=(currentPos);
      compressed.resize(compressedSize-currentPos);

      uncompressed.resize(length); 
      return uncompressed;

    } else if ( (n0tot+uncompressedSize) == length || (n0tot+uncompressedSize) == (length+1) ) {

      compressed>>=(currentPos+nbitsSequence);
      compressed.resize(compressedSize-currentPos-nbitsSequence);

      uncompressed.resize(length); 
      return uncompressed;

    }

    uncompressedSize+=n0tot;

    uncompressed.resize(uncompressedSize);
    uncompressed.push_back(1);
    uncompressedSize+=1;

    currentPos+=nbitsPrefix+1;
    lastPos= currentPos;

    currentPos=compressed.find_next(lastPos-1);   

  }

  
  int n0 =  ( compressedSize - lastPos) * maximumLength;
  if ( (n0+uncompressedSize) >= length) {
    currentPos = lastPos+(length-uncompressedSize)/maximumLength;
    compressed>>=(currentPos);
    compressed.resize(compressedSize-currentPos);
  }
  
  uncompressed.resize(length); 
  return uncompressed;
}


int L0Muon::ProcDataCnv::notcompressedPBWords_to_bitset(const std::vector<unsigned int> & raw,const int size_in_words,
                                                        unsigned int &iwd,
                                                        boost::dynamic_bitset<> & bitset)
{
  int size=size_in_words*32;
  
  if ((iwd+size_in_words)>raw.size()) return -1;
  
  bitset.resize(size);
  for (int i=0; i<size_in_words; ++i) {
    boost::dynamic_bitset<> word_bitset(32,raw[iwd]);
    word_bitset.resize(size);
    word_bitset<<=(size-(i+1)*32);
    bitset|=word_bitset;
    ++iwd;
  }

  return 1;
}

int L0Muon::ProcDataCnv::next_bit(const std::vector<unsigned int> & raw, unsigned int &iwd, int &ibit)
{
//   if (ibit==0) {
//     ++iwd;
//     ibit=32;
//   }
//   --ibit;
//   if (iwd>=raw.size()) return -1;
//   return (raw[iwd]>>ibit)&1;
  if (ibit==31) {
    ++iwd;
    ibit=-1;
  }
  ++ibit;
  if (iwd>=raw.size()) return -1;
  return (raw[iwd]>>ibit)&1;
}

int L0Muon::ProcDataCnv::compressedPBWords_to_bitset(const std::vector<unsigned int> & raw,const int size_in_word,
                                                     unsigned int &iwd,int &ibit,
                                                     boost::dynamic_bitset<> & bitset)
{
  int bitset_size=size_in_word*32;

  bitset.clear();
  bitset.resize(bitset_size);
  int bitset_index=bitset_size;
  
  boost::dynamic_bitset<> bitset_of_32_ones(32); bitset_of_32_ones.flip();
  boost::dynamic_bitset<> bitset_of_16_ones(16); bitset_of_16_ones.flip();

  for (int iw=0; iw<size_in_word; ++iw) { // Loop over the number of original words
#if _DEBUG_PROCDATA_ >1
    std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset iwd= "<<iwd
             <<" word: 0x"<<std::hex<<raw[iwd]<<std::dec
             <<" ibit= "<<ibit<<std::endl;
#endif
    
    // Get the key
    int key=0; 
    for(int i=1;i>-1;--i){
      int bit = next_bit(raw,iwd,ibit);
      key|=(bit<<i);
    }
    
#if _DEBUG_PROCDATA_ >1
    std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset key= "<<key<<std::endl;
#endif
#if _DEBUG_PROCDATA_ >1
    std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset bitset size= "<<bitset.size()
             <<" bitset_index= "<<bitset_index<<std::endl;
#endif
   
    // According to the key, decode the following sequence of bits
    if (key==0) // '00' -> add 32 '0'
    {
#if _DEBUG_PROCDATA_ >1
      std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset case 00"<<std::endl;
#endif
      bitset_index-=32;
#if _DEBUG_PROCDATA_ >1
      std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset bitset size= "<<bitset.size()
               <<" bitset_index= "<<bitset_index<<std::endl;
#endif
    }

    else if (key==1) // '01' -> key is not yet complete, seek the next bit 
    {
      int bit = next_bit(raw,iwd,ibit);
      if (bit==1) // '011' -> add 32 '1'
      {
#if _DEBUG_PROCDATA_ >1
        std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset case 011"<<std::endl;
#endif
        bitset_index-=32;
        boost::dynamic_bitset<> b32ones=bitset_of_32_ones;
        b32ones.resize(bitset_size);
        b32ones<<=(bitset_index);
        bitset|=b32ones;
#if _DEBUG_PROCDATA_ >1
        std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset bitset size= "<<bitset.size()
                 <<" bitset_index= "<<bitset_index<<std::endl;
#endif
      }
      else // '010' -> add 16 '1' and the 16 next bits
      {
#if _DEBUG_PROCDATA_ >1
        std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset case 010"<<std::endl;
#endif
        bitset_index-=16;
        boost::dynamic_bitset<> b16ones=bitset_of_16_ones;
        b16ones.resize(bitset_size);
        b16ones<<=(bitset_index);
        bitset|=b16ones;
#if _DEBUG_PROCDATA_ >1
        unsigned int debug_word=0;
#endif
        for (int i=0; i<16; ++i){
          int bit = next_bit(raw,iwd,ibit);
          --bitset_index;
          if (bit==1) bitset.set(bitset_index);
#if _DEBUG_PROCDATA_ >1
          debug_word|=(bit<<(15-i));
#endif
        }
#if _DEBUG_PROCDATA_ >1
        std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset     0x"<<std::hex<<debug_word<<std::dec<<std::endl;
#endif
#if _DEBUG_PROCDATA_ >1
        std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset bitset size= "<<bitset.size()
                 <<" bitset_index= "<<bitset_index<<std::endl;
#endif

      }
    }

    else if (key==2) // '10' -> decode the position of the ones in the word
    {
#if _DEBUG_PROCDATA_ >1
      std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset case 10"<<std::endl;
#endif
      // The number of '1' in the 32 bits original word is encoded on 3 bits 
      int n_ones=0; 
      for (int i=2; i>-1; --i){
        int bit = next_bit(raw,iwd,ibit);
        n_ones|=(bit<<i);
      }
#if _DEBUG_PROCDATA_ >1
      std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset nb ones : "<<n_ones<<std::endl;
#endif
      if (n_ones==0) return -2;
      if (n_ones>5) return -3;

      // The position of each '1' is encoded on 5 bits
      for (int k=0; k<n_ones; ++k){
        int pos=0;
        for (int i=4; i>-1; --i){
          int bit = next_bit(raw,iwd,ibit);
          pos|=(bit<<i);
        }
        bitset.set(bitset_index-pos-1);
      }
      bitset_index-=32;
#if _DEBUG_PROCDATA_ >1
      std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset bitset size= "<<bitset.size()
               <<" bitset_index= "<<bitset_index<<std::endl;
#endif
    }

    else if (key==3) // '11' -> the next 32 bits are uncompressed
    {
#if _DEBUG_PROCDATA_ >1
      std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset case 11"<<std::endl;
#endif
      for (int i=0; i<32; ++i){
        int bit = next_bit(raw,iwd,ibit);
        --bitset_index;
        if (bit==1) bitset.set(bitset_index);
      }
#if _DEBUG_PROCDATA_ >1
      std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset bitset size= "<<bitset.size()
               <<" bitset_index= "<<bitset_index<<std::endl;
#endif

    } 
#if _DEBUG_PROCDATA_ >1
    boost::dynamic_bitset<> debug_bitset =bitset;
    debug_bitset>>=(bitset_index);
    debug_bitset.resize(32);
    unsigned int debug_word=(debug_bitset.to_ulong()&0xFFFFFFFF);
    std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset decompressed word= 0x"
             <<std::hex<<debug_word<<std::dec<<std::endl;
#endif

  } // End of loop over the number of original words
  
  // If the end of the word was reach, point to the next 
  if (ibit==31) {
    ++iwd;
    ibit=-1;
  }

  return 1;
}
// int L0Muon::ProcDataCnv::compressedPBWords_to_bitset(const std::vector<unsigned int> & raw,const int size_in_word,
//                                                      unsigned int &iwd,int &ibit,
//                                                      boost::dynamic_bitset<> & bitset)
// {

//   //int size_max=size_in_word*32;
//   int size=0;
  
//   boost::dynamic_bitset<> bitset_of_32_ones(32); bitset_of_32_ones.flip();
//   boost::dynamic_bitset<> bitset_of_16_ones(16); bitset_of_16_ones.flip();

//   for (int iw=0; iw<size_in_word; ++iw) { // Loop over the number of original words
// #if _DEBUG_PROCDATA_ >0
//     std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset iwd= "<<iwd
//              <<" word: 0x"<<std::hex<<raw[iwd]<<std::dec<<std::endl;
// #endif
    
//     // Get the key
//     int key=0; 
//     for(int i=1;i>-1;--i){
//       int bit = next_bit(raw,iwd,ibit);
//       key|=(bit<<i);
//     }
    
// #if _DEBUG_PROCDATA_ >0
//     std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset key= "<<key<<std::endl;
// #endif
   
//     // According to the key, decode the following sequence of bits
//     if (key==0) // '00' -> add 32 '0'
//     {
// #if _DEBUG_PROCDATA_ >0
//       std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset case 00"<<std::endl;
// #endif
//       size+=32;
//       bitset.resize(size);
//     }

//     else if (key==1) // '01' -> key is not yet complete, seek the next bit 
//     {
//       int bit = next_bit(raw,iwd,ibit);
//       if (bit==1) // '011' -> add 32 '1'
//       {
// #if _DEBUG_PROCDATA_ >0
//         std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset case 011"<<std::endl;
// #endif
//         size+=32;
//         bitset.resize(size);
//         boost::dynamic_bitset<> b32ones=bitset_of_32_ones;
//         b32ones.resize(size);
//         b32ones<<=(size-32);
//         bitset|=b32ones;
//       }
//       else // '010' -> add 16 '1' and the 16 next bits
//       {
// #if _DEBUG_PROCDATA_ >0
//         std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset case 010"<<std::endl;
// #endif
//         size+=32;
//         bitset.resize(size);
//         boost::dynamic_bitset<> b16ones=bitset_of_16_ones;
//         b16ones.resize(size);
//         b16ones<<=(size-16);
//         bitset|=b16ones;
//         for (int i=0; i<16; ++i){
//           int bit = next_bit(raw,iwd,ibit);
//           if (bit==1) bitset.set(size-i);
//         }
//       }
//     }

//     else if (key==2) // '10' -> decode the position of the ones in the word
//     {
// #if _DEBUG_PROCDATA_ >0
//       std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset case 10"<<std::endl;
// #endif
//       // The number of '1' in the 32 bits original word is encoded on 3 bits 
//       int n_ones=0; 
//       for (int i=2; i>-1; --i){
//         int bit = next_bit(raw,iwd,ibit);
//         n_ones|=(bit<<i);
//       }
//       if (n_ones==0) return -2;
//       if (n_ones>5) return -3;

//       size+=32;
//       bitset.resize(size);

//       // The position of each '1' is encoded on 5 bits
//       for (int k=0; k<n_ones; ++k){
//         int pos=0;
//         for (int i=4; i>-1; --i){
//           int bit = next_bit(raw,iwd,ibit);
//           pos|=(bit<<i);
//         }
//         bitset.set(size-pos);
//       }
//     }

//     else if (key==3) // '11' -> the next 32 bits are uncompressed
//     {
// #if _DEBUG_PROCDATA_ >0
//       std::cout<<"L0Muon::ProcDataCnv::compressedPBWords_to_bitset case 11"<<std::endl;
// #endif
//       size+=32;
//       bitset.resize(size);
//       for (int i=0; i<32; ++i){
//         int bit = next_bit(raw,iwd,ibit);
//         if (bit==1) bitset.push_back(size-i);
//       }
//     } 

//   } // End of loop over the number of original words
  
//   // If the end of the word was reach, point to the next 
//   if (ibit==31) {
//     ++iwd;
//     ibit=-1;
//   }

//   return 1;
// }
  

int L0Muon::ProcDataCnv::rawBank_v2(std::vector<unsigned int> &raw, int mode, bool compression)
{
  raw.clear();
  if (compression) {
    //
    // TO BE IMPLEMENTED
    //
    raw.push_back(0x80000000);
    return 0;
  }
  
  
  //
  //--- 1st part 
  //

  for (int ib =0; ib<12; ++ib) 
  { // Loop over processing boards
    for (int ipu=0; ipu<4; ++ipu) raw.push_back(0);
    for (int ipu=0; ipu<4; ++ipu) {
      for (unsigned int iwd=0; iwd<PU_OpticalLinks_size_16/2; ++iwd) {
        unsigned int word=m_ols[ib][ipu]->getulong(32,iwd*32);
        raw.push_back(word);
      }
    }
    //     boost::dynamic_bitset<> olbitset(PB_OpticalLinks_size*32);
    //     for (int ipu=0; ipu<4; ++ipu) {
    //       boost::dynamic_bitset<> pubitset = m_ols[ib][ipu]->getBitset();
    //       pubitset.resize(PB_OpticalLinks_size);
    //       olbitset|=( pubitset << ( (4*32) + (PU_OpticalLinks_size_16*16*ipu) ) );
    //     }
    //     if (compression) {
    //       // compress olbitset
    //     }
    //     unsigned int nwords=olbitset.size()/32;
    //     for (unsigned int iwd=0; iwd<nwords;++iwd) {
    //       unsigned int word=olbitset.to_ulong()&0xFFFFFFFF;
    //       raw.push_back(word);
    //       olbitset>>=32;
    //     }
    //     unsigned int remainder=olbitset.size()%32;
  }

  unsigned int header=raw.size()*2;
  raw.insert(raw.begin(),header);

  if (mode<2) return 1;
  
  //
  //--- 2nd part 

  //
  //
  // TO BE IMPLEMENTED
  //

  //   for (int ib =0; ib<12; ++ib) 
  //   { // Loop over processing boards
  //     unsigned int word=0;
  //     for (int ipu=0; ipu<4; ++ipu) {
  //       int shift=0;
  //       int msb=1;
  //       for (int iwd=0; iwd<PU_OpticalLinks_size_16){
  //         unsigned int word16 = m_neighs[ib][ipu]->getulong(16, shift);
  //         shift+=16;
  //         word|=(word16<<(msb*16));
  //         if (!msb) raw.push_back(0);
  //         msb=!msb;
  //       }
  //     }
  //   }
  return -2;
}

int L0Muon::ProcDataCnv::setRegisters_for_PB_OpticalLinks(int iboard, boost::dynamic_bitset<> & bitset )
{
    int current_size;
    
#if _DEBUG_PROCDATA_ >2
    unsigned int x=0xFFFF0000;
    boost::dynamic_bitset<> bitset1(32,x);
    unsigned int y=0x0000FFFF;
    boost::dynamic_bitset<> bitset2(32,y);
    std::cout << "L0Muon::ProcDataCnv::decodeBank bitset1 size= "<<bitset1.size()<<" "<<bitset1<<std::endl;
    std::cout << "L0Muon::ProcDataCnv::decodeBank bitset2 size= "<<bitset2.size()<<" "<<bitset2<<std::endl;
    std::cout << "L0Muon::ProcDataCnv::decodeBank bitset  size= "<<bitset.size() <<" "<<bitset <<std::endl;
#endif
    // Errors
    current_size=bitset.size();
    boost::dynamic_bitset<> errorbitset(current_size);
    errorbitset=bitset>>(current_size-4*32);
    errorbitset.resize(4*32);
    bitset.resize(current_size-4*32);
    
    for (int ipu = 0; ipu <4 ; ipu++) {// Loop over PUs errors
      boost::dynamic_bitset<> bitset = (errorbitset>>((3-ipu)*32));
      bitset.resize(32);
      unsigned int word = bitset.to_ulong();
#if _DEBUG_PROCDATA_ >2
      std::cout << "L0Muon::ProcDataCnv::decodeBank error word ipu= "<<ipu
                <<" 0x"<<std::hex<<word<<std::dec<<std::endl;
#endif
      m_errors[iboard].opt_link[ipu].set( (word>>16)&0xFF);
      m_errors[iboard].ser_link[ipu].set( (word>> 8)&0x3F);
      m_errors[iboard].par_link[ipu].set( (word>> 0)&0xFF);
    } // End of loop over PUs errors
    
    
    // Optical links
    current_size=bitset.size();
    boost::dynamic_bitset<> olbitset(current_size);
    olbitset=bitset>>(current_size-4*10*16);
    olbitset.resize(4*10*16);
    bitset.resize(current_size-4*10*16);
    
    for (int ipu = 0; ipu <4 ; ipu++) {// Loop over PUs
      
      current_size=olbitset.size();
      boost::dynamic_bitset<> pubitset(current_size);
      pubitset=olbitset>>(current_size-10*16);
      pubitset.resize(10*16);
      olbitset.resize(current_size-10*16);
      
      if (m_ols[iboard][ipu]!=0) 
        m_ols[iboard][ipu]->set(pubitset);

#if _DEBUG_PROCDATA_ >3
      std::cout << "L0Muon::ProcDataCnv::decodeBank OLs   cross check"<<std::endl;
      if (m_ols[iboard][ipu]!=0) {
        std::vector<LHCb::MuonTileID> mids = m_ols[iboard][ipu]->getTileVector();
        std::cout << "L0Muon::ProcDataCnv::decodeBank OLs   model"<<std::endl;
        for (int ii=10-1;ii>-1;--ii){
          for (int iii=16-1;iii>-1;--iii){
            if (mids[ii*16+iii].isValid())
              std::cout<<(mids[ii*16+iii].station()+1);
            else if (mids[ii*16+iii].layout().xGrid()!=0 && mids[ii*16+iii].layout().xGrid()!=0)
              std::cout<<"?";
            else
              std::cout<<"x";
          }
          std::cout<<std::endl;
        }
        std::cout << "L0Muon::ProcDataCnv::decodeBank OLs   observed"<<std::endl;
        for (int ii=10-1;ii>-1;--ii){
          for (int iii=16-1;iii>-1;--iii){
            if (m_ols[iboard][ipu]->test(ii*16+iii))
              std::cout<<"1";
            else
              std::cout<<"0";
          }
          std::cout<<std::endl;
        }
        unsigned int nexpected=0;
        for (int ii=10-1;ii>-1;--ii){
          for (int iii=16-1;iii>-1;--iii){
            if (mids[ii*16+iii].isValid()) ++nexpected;
          }
        }        
        for (int ii=10-1;ii>-1;--ii){
          for (int iii=16-1;iii>-1;--iii){
            if (mids[ii*16+iii].isValid()) {
              if (!m_ols[iboard][ipu]->test(ii*16+iii)){
                std::cout << "L0Muon::ProcDataCnv::decodeBank OLs   missing : "<<mids[ii*16+iii].toString()
                          <<" @ "<<ii*16+iii<<std::endl;
              }
            } 
          }
        }        
        std::vector<LHCb::MuonTileID> pads = m_ols[iboard][ipu]->firedTiles();
        for (std::vector<LHCb::MuonTileID>::iterator it=pads.begin(); it<pads.end();++it){
          if (!it->isValid())
            std::cout << "L0Muon::ProcDataCnv::decodeBank OLs    OL data :"
                      <<" tiles "<<it->toString()<<" valid ? "<<it->isValid()<<std::endl;
        }
        std::cout << "L0Muon::ProcDataCnv::decodeBank OLs   bits found in"
                  <<" buffer= "<<pubitset.count()
                  <<" register= "<<pads.size()
                  <<" model= "<<nexpected;
        if ( (pubitset.count()!=pads.size()) || (pubitset.count()!=nexpected) ) 
          std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!";
        std::cout<<std::endl;
      }
#endif      
    } // End of Loop over PUs
    
    return 1;
    
}
int L0Muon::ProcDataCnv::setRegisters_for_PB_Neighbours(int iboard, boost::dynamic_bitset<> & neighbitset )
{
    int current_size;

    for (int ipu = 0; ipu <4 ; ipu++) {  // Loop over PUs
      current_size=neighbitset.size();
      boost::dynamic_bitset<> pubitset(current_size);
      pubitset=neighbitset>>(current_size-17*16);
      pubitset.resize(17*16);
      neighbitset.resize(current_size-17*16);

      if (m_neighs[iboard][ipu]!=0)
        m_neighs[iboard][ipu]->set(pubitset);

#if _DEBUG_PROCDATA_ >0
      if (m_neighs[iboard][ipu]!=0) {
        std::cout<<"-- PB"<<iboard<<" PU"<<ipu<<std::endl; 
        // Cross check (only when all inputs forced to 1)
        std::vector<LHCb::MuonTileID> mids = m_neighs[iboard][ipu]->getTileVector();
//         for (int ii=17-1;ii>-1;--ii){
//           for (int iii=16-1;iii>-1;--iii){
//             if (mids[ii*16+iii].isValid()) {
//               if (!m_neighs[iboard][ipu]->test(ii*16+iii)){
//                 std::cout << "L0Muon::ProcDataCnv::decodeBank Neigh   !!! en moins : "<<mids[ii*16+iii].toString()
//                           <<" @ pos"<<ii*16+iii
//                           <<" word "<<ii<<" bit "<<iii<<std::endl;
//               }
//             } 
//             if (m_neighs[iboard][ipu]->test(ii*16+iii)){
//               if (!mids[ii*16+iii].isValid()) {
//                 std::cout << "L0Muon::ProcDataCnv::decodeBank Neigh   !!! en plus  : "<<mids[ii*16+iii].toString()
//                           <<" @ pos"<<ii*16+iii
//                           <<" word "<<ii<<" bit "<<iii<<std::endl;
//               }
//             }
//           }
//         }
        
        
        boost::dynamic_bitset<> model(17*16);
        for (int ii=17*16-1;ii>-1;--ii){
          if (mids[ii].isValid()) model.set(ii);
        }
        std::cout << "L0Muon::ProcDataCnv::decodeBank Neigh   PER LINK"<<std::endl;
        boost::dynamic_bitset<> link;
        boost::dynamic_bitset<> link2;
        
        link.resize(17*16);
        link=pubitset>>244;
        link.resize(28);
        link2.resize(17*16);
        link2=model>>244;
        link2.resize(28);
#if _DEBUG_PROCDATA_ >1
        std::cout <<"BP SER 1 : "<<link<<std::endl;
        std::cout <<"BP SER 1 - "<<link2<<std::endl;
#endif
        if (link2.count()>0) {
          std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP SER 1";
          std::cout<<" status "<<((m_errors[iboard].ser_link[ipu].value()>>0)&1);
          if (link!=link2) std::cout<<" missing";
          std::cout<<std::endl;
        } else {
          if (link.count()>0) std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP SER 1"<<" excess"<<std::endl;
        }
        
        link.resize(17*16);
        link=pubitset>>216;
        link.resize(28);
        link2.resize(17*16);
        link2=model>>216;
        link2.resize(28);
#if _DEBUG_PROCDATA_ >1
        std::cout <<"BP SER 2 : "<<link<<std::endl;
        std::cout <<"BP SER 2 - "<<link2<<std::endl;
#endif
        if (link2.count()>0) {
          std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP SER 2";
          std::cout<<" status "<<((m_errors[iboard].ser_link[ipu].value()>>1)&1);
          if (link!=link2) std::cout<<" missing";
          std::cout<<std::endl;
        } else {
          if (link.count()>0) std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP SER 2"<<" excess"<<std::endl;
        }
     
        link.resize(17*16);
        link=pubitset>>188;
        link.resize(28);
        link2.resize(17*16);
        link2=model>>188;
        link2.resize(28);
#if _DEBUG_PROCDATA_ >1
        std::cout <<"BP SER 3 : "<<link<<std::endl;
        std::cout <<"BP SER 3 - "<<link2<<std::endl;
#endif
        if (link2.count()>0) {
          std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP SER 3";
          std::cout<<" status "<<((m_errors[iboard].ser_link[ipu].value()>>2)&1);
          if (link!=link2) std::cout<<" missing";
          std::cout<<std::endl;
        } else {
          if (link.count()>0) std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP SER 3"<<" excess"<<std::endl;
        }
        

        link.resize(17*16);
        link=pubitset>>176;
        link.resize(12);
        link2.resize(17*16);
        link2=model>>176;
        link2.resize(12);
#if _DEBUG_PROCDATA_ >1
        std::cout <<"BP SER 4 : "<<link<<std::endl;
        std::cout <<"BP SER 4 - "<<link2<<std::endl;
#endif
        if (link2.count()>0) {
          std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP SER 4";
          std::cout<<" status "<<((m_errors[iboard].ser_link[ipu].value()>>3)&1);
          if (link!=link2) std::cout<<" missing";
          std::cout<<std::endl;
        } else {
          if (link.count()>0) std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP SER 4"<<" excess"<<std::endl;
        }
     
        link.resize(17*16);
        link=pubitset>>164;
        link.resize(12);
        link2.resize(17*16);
        link2=model>>164;
        link2.resize(12);
#if _DEBUG_PROCDATA_ >1
        std::cout <<"BP SER 5 : "<<link<<std::endl;
        std::cout <<"BP SER 5 - "<<link2<<std::endl;
#endif
        if (link2.count()>0) {
          std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP SER 5";
          std::cout<<" status "<<((m_errors[iboard].ser_link[ipu].value()>>4)&1);
          if (link!=link2) std::cout<<" missing";
          std::cout<<std::endl;
        } else {
          if (link.count()>0) std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP SER 5"<<" excess"<<std::endl;
        }
     
        link.resize(17*16);
        link=pubitset>>152;
        link.resize(12);
        link2.resize(17*16);
        link2=model>>152;
        link2.resize(12);
#if _DEBUG_PROCDATA_ >1
        std::cout <<"BP SER 6 : "<<link<<std::endl;
        std::cout <<"BP SER 6 - "<<link2<<std::endl;
#endif
        if (link2.count()>0) {
          std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP SER 6";
          std::cout<<" status "<<((m_errors[iboard].ser_link[ipu].value()>>5)&1);
          if (link!=link2) std::cout<<" missing";
          std::cout<<std::endl;
        } else {
          if (link.count()>0) std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP SER 6"<<" excess"<<std::endl;
        }
     
        link.resize(17*16);
        link=pubitset>>150;
        link.resize(2);
        link2.resize(17*16);
        link2=model>>150;
        link2.resize(2);
#if _DEBUG_PROCDATA_ >1
        std::cout <<"BP PAR 5 : "<<link<<std::endl;
        std::cout <<"BP PAR 5 - "<<link2<<std::endl;
#endif
        if (link2.count()>0) {
          std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP PAR 5";
          std::cout<<" status "<<((m_errors[iboard].par_link[ipu].value()>>4)&1);
          if (link!=link2) std::cout<<" missing";
          std::cout<<std::endl;
        } else {
          if (link.count()>0) std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP PAR 5"<<" excess"<<std::endl;
        }
     
        link.resize(17*16);
        link=pubitset>>148;
        link.resize(2);
        link2.resize(17*16);
        link2=model>>148;
        link2.resize(2);
#if _DEBUG_PROCDATA_ >1
        std::cout <<"BP PAR 4 : "<<link<<std::endl;
        std::cout <<"BP PAR 4 - "<<link2<<std::endl;
#endif
        if (link2.count()>0) {
          std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP PAR 4";
          std::cout<<" status "<<((m_errors[iboard].par_link[ipu].value()>>3)&1);
          if (link!=link2) std::cout<<" missing";
          std::cout<<std::endl;
        } else {
          if (link.count()>0) std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP PAR 4"<<" excess"<<std::endl;
        }
     
        link.resize(17*16);
        link=pubitset>>146;
        link.resize(2);
        link2.resize(17*16);
        link2=model>>146;
        link2.resize(2);
#if _DEBUG_PROCDATA_ >1
        std::cout <<"BP PAR 3 : "<<link<<std::endl;
        std::cout <<"BP PAR 3 - "<<link2<<std::endl;
#endif
        if (link2.count()>0) {
          std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP PAR 3";
          std::cout<<" status "<<((m_errors[iboard].par_link[ipu].value()>>2)&1);
          if (link!=link2) std::cout<<" missing";
          std::cout<<std::endl;
        } else {
          if (link.count()>0) std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP PAR 3"<<" excess"<<std::endl;
        }
     
        link.resize(17*16);
        link=pubitset>>144;
        link.resize(2);
        link2.resize(17*16);
        link2=model>>144;
        link2.resize(2);
#if _DEBUG_PROCDATA_ >1
        std::cout <<"BP PAR 2 : "<<link<<std::endl;
        std::cout <<"BP PAR 2 - "<<link2<<std::endl;
#endif
        if (link2.count()>0) {
          std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP PAR 2";
          std::cout<<" status "<<((m_errors[iboard].par_link[ipu].value()>>1)&1);
          if (link!=link2) std::cout<<" missing";
          std::cout<<std::endl;
        } else {
          if (link.count()>0) std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP PAR 2"<<" excess"<<std::endl;
        }
     
        link.resize(17*16);
        link=pubitset>>142;
        link.resize(2);
        link2.resize(17*16);
        link2=model>>142;
        link2.resize(2);
#if _DEBUG_PROCDATA_ >1
        std::cout <<"BP PAR 1 : "<<link<<std::endl;
        std::cout <<"BP PAR 1 - "<<link2<<std::endl;
#endif
        if (link2.count()>0) {
          std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP PAR 1";
          std::cout<<" status "<<((m_errors[iboard].par_link[ipu].value()>>0)&1);
          if (link!=link2) std::cout<<" missing";
          std::cout<<std::endl;
        } else {
          if (link.count()>0) std::cout<<"++Neigh :   PB"<<iboard<<" PU"<<ipu<<" : BP PAR 1"<<" excess"<<std::endl;
        }
     
        link.resize(17*16);
        link=pubitset>>130;
        link.resize(12);
        link2.resize(17*16);
        link2=model>>130;
        link2.resize(12); 
#if _DEBUG_PROCDATA_ >1
        std::cout <<"CROS     : "<<link<<std::endl;
        std::cout <<"CROS     - "<<link2<<std::endl;
#endif
     
        link.resize(17*16);
        link=pubitset>>48;
        link.resize(81);
        link2.resize(17*16);
        link2=model>>48;
        link2.resize(81);
#if _DEBUG_PROCDATA_ >1
        std::cout <<"HORI     : "<<link<<std::endl;
        std::cout <<"HORI     - "<<link2<<std::endl;
#endif
     
        link.resize(17*16);
        link=pubitset>>6;
        link.resize(42);
        link2.resize(17*16);
        link2=model>>6;
        link2.resize(42);
#if _DEBUG_PROCDATA_ >1
        std::cout <<"VERT     : "<<link<<std::endl;
        std::cout <<"VERT     - "<<link2<<std::endl;
#endif
     
        link.resize(17*16);
        link=pubitset>>0;
        link.resize(6);
        link2.resize(17*16);
        link2=model>>0;
        link2.resize(6);
#if _DEBUG_PROCDATA_ >1
        std::cout <<"EMPTY    : "<<link<<std::endl;
        std::cout <<"EMPTY    - "<<link2<<std::endl;
#endif
      }

      if (m_neighs[iboard][ipu]!=0) {
        
        std::cout << "L0Muon::ProcDataCnv::decodeBank Neigh   cross check"<<std::endl;
        std::vector<LHCb::MuonTileID> mids = m_neighs[iboard][ipu]->getTileVector();
//         std::cout << "L0Muon::ProcDataCnv::decodeBank Neigh   model"<<std::endl;
//         for (int ii=17-1;ii>-1;--ii){
//           int wd=0;
//           for (int iii=16-1;iii>-1;--iii){
//             if (mids[ii*16+iii].isValid()) wd|=1<<iii;
//             if (mids[ii*16+iii].isValid())
//               std::cout<<(mids[ii*16+iii].station()+1);
//             else if (mids[ii*16+iii].layout().xGrid()!=0 && mids[ii*16+iii].layout().xGrid()!=0)
//               std::cout<<"?";
//             else
//               std::cout<<"x";
//           }
//           std::cout<<" "<<std::hex<<wd<<std::dec<<" "<<(ii)<<" "<<(ii*16+15)<<" <- "<<(ii*16)<<std::endl;
//         }
        unsigned int nexpected=0;
        for (int ii=17-1;ii>-1;--ii){
          for (int iii=16-1;iii>-1;--iii){
            if (mids[ii*16+iii].isValid()) ++nexpected;
          }
        }
        
        std::vector<LHCb::MuonTileID> pads = m_neighs[iboard][ipu]->firedTiles();
        std::cout << "L0Muon::ProcDataCnv::decodeBank Neigh number of bits fired= "<<pads.size()<<std::endl;
        for (std::vector<LHCb::MuonTileID>::iterator it=pads.begin(); it<pads.end();++it){
          if (!it->isValid())
            std::cout << "L0Muon::ProcDataCnv::decodeBank   NOT VALID  "<<it->toString()<<std::endl;
        }
        
        std::cout << "L0Muon::ProcDataCnv::decodeBank Neigh   bits found in"
                  <<" buffer= "<<pubitset.count()
                  <<" register= "<<pads.size()
                  <<" model= "<<nexpected;
        if ( (pubitset.count()!=pads.size()) || (pubitset.count()!=nexpected) ) 
          std::cout << "!!!!!!!!!!!!!!!!!!!!!!!! ERROR -- PB"<<iboard<<" PU"<<ipu;
        std::cout<<std::endl; 
        std::cout<<"--"<<std::endl; 
      }
#endif
    } // End of Loop over PUs

    return 1;
}
