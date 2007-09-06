#include "L0MuonKernel/ProcDataCnv.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "Kernel/MuonLayout.h"
#include <math.h>

/**
   Constructor.
*/
L0Muon::ProcDataCnv::ProcDataCnv(){
};
/**
   Constructor.
*/
L0Muon::ProcDataCnv::ProcDataCnv(int quarter, int compressionParameter){
  m_quarter=quarter;
  m_compressionParameter = compressionParameter;

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


void L0Muon::ProcDataCnv::release(){
  for (int iboard = 0; iboard <12 ; iboard++) {
    for (int ipu = 0; ipu <4 ; ipu++) { 
      if (m_ols[iboard][ipu]!=0) m_ols[iboard][ipu]->reset();
      if (m_neighs[iboard][ipu]!=0) m_neighs[iboard][ipu]->reset();
    }    
  }
}

// std::vector<LHCb::MuonTileID> L0Muon::ProcDataCnv::ols(int board, int pu)
// {
//   return m_ols[board][pu]->firedTiles();
// }
    
// std::vector<LHCb::MuonTileID> L0Muon::ProcDataCnv::neighs(int board, int pu)   
// {
//   return m_neighs[board][pu]->firedTiles();
// }

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


void L0Muon::ProcDataCnv::decodeBank(std::vector<unsigned int> raw, int version)
{
  if (version<1) return;

  // Clear the registers first
  release();

//   std::cout << "L0Muon::ProcDataCnv::decodeBank  bank size= "<<raw.size()<<" words"<<std::endl;
//   std::cout.setf(std::ios::uppercase) ;
//   std::cout<<"\t=> L0Muon::ProcDataCnv::decodeBank -- dump raw bank of size: "<<raw.size()<<std::hex<<std::endl;
//   for (std::vector<unsigned int>::iterator itraw = raw.begin(); itraw<raw.end(); ++itraw){
//     std::cout <<"\t0x"<<std::setw(8)<<(*itraw)<<std::endl;
//   }
//   std::cout<<std::dec;
//   std::cout.unsetf(std::ios::uppercase);
  
  int header = raw[0];
//   std::cout.setf(std::ios::uppercase) ;
//   std::cout << "L0Muon::ProcDataCnv::decodeBank  header= 0x"<<std::setw(8)<<std::hex<<header<<std::dec<<std::endl;
//   std::cout.unsetf(std::ios::uppercase);
  int nOLwords = (header & 0x3FFF)/4; // in words of 32 bytes
//   std::cout << "L0Muon::ProcDataCnv::decodeBank  nOLwords= "<<nOLwords<<std::endl;

  //
  // Optical links
  //
 
  // Read the first nOLwords words of raw into a single bitset
  boost::dynamic_bitset<> rawbitset;
  for (int iwd=0; iwd<nOLwords; ++iwd){
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
  
  if (nOLwords+1==raw.size()) return; // raw bank stops here
  
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
      boost::dynamic_bitset<> bitset = rawbitset>>(ipu*17*16);
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

}

std::vector<unsigned int> L0Muon::ProcDataCnv::rawBank(int version, int mode)
{


  std::vector<unsigned int> raw;

  if (version<1) return raw;

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
  
  return raw;
}


boost::dynamic_bitset<> L0Muon::ProcDataCnv::applyCompression(boost::dynamic_bitset<> bitset_to_compress){
  // Compress the bit set in input
  
  int nbitsPrefix  = m_compressionParameter;
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
  
  int nbitsPrefix  = m_compressionParameter;
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






  
