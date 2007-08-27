#include "L0MuonKernel/ProcDataCnv.h"
#include "ProcessorKernel/RegisterFactory.h"
#include "Kernel/MuonLayout.h"

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
  int length = (header & 0x3FFF)/4; // length in bytes
//   std::cout << "L0Muon::ProcDataCnv::decodeBank  length= "<<length<<std::endl;
  int count = 0;

  // Optical links
  boost::dynamic_bitset<> rawbitset;
  for (int iwd=0; iwd<length; ++iwd){
    boost::dynamic_bitset<> bitset(32,raw[iwd+1]);
    int rawsize = rawbitset.size();
    rawbitset.resize(rawsize+32);
    bitset.resize(rawsize+32);
    bitset<<=rawsize;
    rawbitset|=bitset;
  }

//   std::cout << "L0Muon::ProcDataCnv::decodeBank  compressed bitset size= "<<rawbitset.size()<<std::endl;
  rawbitset = unapplyCompression(rawbitset);
//   std::cout << "L0Muon::ProcDataCnv::decodeBank  uncompressed bitset size= "<<rawbitset.size()<<std::endl;

  count = 0;
  for (int iboard = 0; iboard <12 ; iboard++) {
    for (int ipu = 0; ipu <4 ; ipu++) {
      boost::dynamic_bitset<> bitset = rawbitset>>(count*10*16);
      bitset.resize(10*16);
      if (m_ols[iboard][ipu]!=0)
        m_ols[iboard][ipu]->set(bitset);
      ++count;
    }
  }
//   std::cout << "L0Muon::ProcDataCnv::decodeBank  -> registers filled. "<<std::endl;

  // Neighbours
  rawbitset.clear();
  for (unsigned int iwd=length+1; iwd<raw.size(); ++iwd){
    boost::dynamic_bitset<> bitset(32,raw[iwd]);
    int rawsize = rawbitset.size();
    rawbitset.resize(rawsize+32);
    bitset.resize(rawsize+32);
    bitset<<=rawsize;
    rawbitset|=bitset;
  }

  rawbitset = unapplyCompression(rawbitset);

  count = 0;
  for (int iboard = 0; iboard <12 ; iboard++) {
    for (int ipu = 0; ipu <4 ; ipu++) {
      boost::dynamic_bitset<> bitset = rawbitset>>(count*17*16);
      bitset.resize(17*16);
      if (m_neighs[iboard][ipu]!=0)
        m_neighs[iboard][ipu]->set(bitset);
      ++count;
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
    for (int iboard = 0; iboard <12 ; iboard++) {
      for (int ipu = 0; ipu <4 ; ipu++) {

        boost::dynamic_bitset<> bitset;
        if (m_ols[iboard][ipu]!=0) 
        {
          bitset = m_ols[iboard][ipu]->getBitset();
//           std::cout << "L0Muon::ProcDataCnv::rawBank  got bitset for board "
//                     <<iboard<<" and pu "<<ipu
//                     <<" -- size is : "<<bitset.size()<<std::endl;
        } else {
          bitset.clear();
          bitset.resize(10*16);
          std::cout << "L0Muon::ProcDataCnv::rawBank  set empty bitset for board "
                    <<iboard<<" and pu "<<ipu<<std::endl;
        }
        bitset = applyCompression(bitset);

        int size = bitset.size();
        int rawsize = rawbitset.size();
//         std::cout << "L0Muon::ProcDataCnv::rawBank  adding "<<size<<" bits to the "<<rawsize<<" existing bits"<<std::endl;
        rawbitset.resize(rawsize+size); 
        bitset.resize(rawsize+size);
        bitset<<=(rawsize);
        rawbitset|=bitset;
      }
    }
//     std::cout << "L0Muon::ProcDataCnv::rawBank  bitset is ready; size= "<<rawbitset.size()<<std::endl;
  
    unsigned int size = 0;
    while(size<rawbitset.size()) {
      boost::dynamic_bitset<> bitset = (rawbitset>>size);
      bitset.resize(32);
      size+=32;
      unsigned int word = bitset.to_ulong();
      raw.push_back(word);
    }
//     std::cout << "L0Muon::ProcDataCnv::rawBank  rawbank size is "<<raw.size()<<" words"<<std::endl;
  
    // update sub-detector specific header 
    header = (raw.size()-1)*4;// length in bytes
    raw[0]=header;

  } 

  if (mode>1) {
    
    boost::dynamic_bitset<> rawbitset;
    for (int iboard = 0; iboard <12 ; iboard++) {
      for (int ipu = 0; ipu <4 ; ipu++) {

        boost::dynamic_bitset<> bitset;
        if (m_neighs[iboard][ipu]!=0) 
        {
          bitset = m_neighs[iboard][ipu]->getBitset();
        } else {
          bitset.resize(17*16);
        }
        bitset = applyCompression(bitset);

        int size = bitset.size();
        int rawsize = rawbitset.size();
        rawbitset.resize(rawsize+size);
        bitset.resize(rawsize+size);
        bitset<<=(rawsize);
        rawbitset|=bitset;
      }
    }
  
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
//   std::cout << "L0Muon::ProcDataCnv::applyCompression\tIN -"
//             <<" bitset_to_compress.size()= "<< bitset_to_compress.size()
//             <<" ; bitset_to_compress.count()= "<< bitset_to_compress.count()<<std::endl;
//   std::cout << "L0Muon::ProcDataCnv::applyCompression\tbitset: "<<bitset_to_compress<<std::endl;

  int nbitsPrefix  = m_compressionParameter;
  int nbitsSequence=nbitsPrefix+1;
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
  boost::dynamic_bitset<>::size_type currentPos=bitset_to_compress.find_first();

  while (currentPos<bitset_to_compress.size()) {
//     std::cout << "L0Muon::ProcDataCnv::applyCompression\t:.."<<"  currentPos= "<<currentPos<<std::endl;
  
    // Code the sequence of 0
    nchar = currentPos-lastPos;
//     std::cout << "L0Muon::ProcDataCnv::applyCompression\t:  "<<"  nchar= "<<nchar<<std::endl;
    n0         = nchar/maximumLength;
//     std::cout << "L0Muon::ProcDataCnv::applyCompression\t:  "<<"  n0= "<<n0<<std::endl;
    leftover   = nchar%maximumLength;
//     std::cout << "L0Muon::ProcDataCnv::applyCompression\t:  "<<"  leftover= "<<leftover<<std::endl;
    size       = n0 + nbitsSequence;
//     std::cout << "L0Muon::ProcDataCnv::applyCompression\t:  "<<"  size= "<<size<<std::endl;
    sequence = (leftover<<1)+1;
//     std::cout << "L0Muon::ProcDataCnv::applyCompression\t:  "
//               <<"  sequence= "<<sequence<<std::endl;
    leftoverSequence= boost::dynamic_bitset<> (size,sequence);
//     std::cout << "L0Muon::ProcDataCnv::applyCompression\t:  "<<"  leftoverSequence= "<<leftoverSequence<<std::endl;
    encodedSequence.resize(size);
    encodedSequence|=leftoverSequence<<n0;
//     std::cout << "L0Muon::ProcDataCnv::applyCompression\t:  "<<"  encodedSequence= "<<encodedSequence<<std::endl;
  

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
//   std::cout << "L0Muon::ProcDataCnv::applyCompression\tlast ......."<<std::endl;
//   std::cout << "L0Muon::ProcDataCnv::applyCompression\t:.."
//             <<"  currentPos= "<<currentPos<<" ;lastPos= "<<lastPos<<std::endl;
  nchar = bitset_to_compress.size()-lastPos;
//   std::cout << "L0Muon::ProcDataCnv::applyCompression\t:  "<<"  nchar= "<<nchar<<std::endl;
  n0         = nchar/maximumLength;
//   std::cout << "L0Muon::ProcDataCnv::applyCompression\t:  "<<"  n0= "<<n0<<std::endl;
  leftover   = nchar%maximumLength;
//   std::cout << "L0Muon::ProcDataCnv::applyCompression\t:  "<<"  leftover= "<<leftover<<std::endl;
  size       = n0;
  size+=nbitsSequence;
  sequence = (leftover<<1)+1;
//   std::cout << "L0Muon::ProcDataCnv::applyCompression\t:  "<<"  sequence= "<<sequence<<std::endl;
  leftoverSequence= boost::dynamic_bitset<>(size,sequence);
//   std::cout << "L0Muon::ProcDataCnv::applyCompression\t:  "<<"  leftoverSequence= "<<leftoverSequence<<std::endl;
  encodedSequence.resize(size);
  encodedSequence|=leftoverSequence<<n0;
//   std::cout << "L0Muon::ProcDataCnv::applyCompression\t:  "<<"  encodedSequence= "<<encodedSequence<<std::endl;

  // Add the encoded sequence to the compressed bitset
  shift   = compressedBitset.size();
  newsize = shift + encodedSequence.size();
  compressedBitset.resize(newsize);
  encodedSequence.resize(newsize);
  compressedBitset|=encodedSequence<<shift;

//   std::cout << "L0Muon::ProcDataCnv::applyCompression\t"<<"compressedBitset= "<<compressedBitset<<std::endl;
//   std::cout << "L0Muon::ProcDataCnv::applyCompression\t"<<"OUT  -"
//             << " compressedBitset.size = "<<compressedBitset.size() 
//             << " ; compressedBitset.count = "<<compressedBitset.count() <<std::endl;
  return compressedBitset;
}


boost::dynamic_bitset<> L0Muon::ProcDataCnv::unapplyCompression(boost::dynamic_bitset<> compressed){
  
//   std::cout << "L0Muon::ProcDataCnv::unapplyCompression\tIN -" 
//             << " compressedSize= "<< compressed.size()
//             << " ; # of bits set = "<< compressed.count()<<std::endl;
//   std::cout << "L0Muon::ProcDataCnv::unapplyCompression\tcompressed bitset: "
//             << compressed <<std::endl;

  boost::dynamic_bitset<> uncompressed;  
  unsigned int uncompressedSize = uncompressed.size();

  int nbitsPrefix  = m_compressionParameter;
  int maximumLength = int(pow(2,nbitsPrefix));
//   std::cout << "L0Muon::ProcDataCnv::unapplyCompression\t"<<"nbitsPrefix= "<<nbitsPrefix<<std::endl;

  unsigned int compressedSize = compressed.size();
  if (compressed.count()==0) {
    uncompressed.resize(compressedSize*maximumLength);
    return uncompressed;
  }
//   std::cout << "L0Muon::ProcDataCnv::unapplyCompression\t"<<"compressedSize= "<<compressedSize<<std::endl;

  boost::dynamic_bitset<>::size_type lastPos=0;
  boost::dynamic_bitset<>::size_type currentPos=compressed.find_first();

  while (currentPos<compressed.size()) {
//     std::cout << "L0Muon::ProcDataCnv::unapplyCompression\t:.."<<" currentPos= "<<currentPos<<std::endl;

    // Decode a sequence.
    // A sequence is made of 3 parts:
    // - a series of 0 eventually
    // - a 1
    // - a number coded on nbitsPrefix

    // the series of 0 contains n0
    int n0 =  currentPos - lastPos;
//     std::cout << "L0Muon::ProcDataCnv::unapplyCompression\t:  "<<" n0= "<< n0<<std::endl;

    // the number is
    boost::dynamic_bitset<> number = (compressed>>currentPos+1);
    number.resize(nbitsPrefix);
//     std::cout << "L0Muon::ProcDataCnv::unapplyCompression\t:  "<<"number= "<<number.to_ulong()<<std::endl;

    uncompressedSize+=n0*maximumLength + number.to_ulong();

//     std::cout << "L0Muon::ProcDataCnv::unapplyCompression\t:  adding: "<< n0*maximumLength + number.to_ulong()<<" 0\n";
//     std::cout << "L0Muon::ProcDataCnv::unapplyCompression\t:  uncompressedSize:"<<"= "<<uncompressedSize<<std::endl; 
    uncompressed.resize(uncompressedSize);
    uncompressed.push_back(1);

    currentPos+=nbitsPrefix+1;
    lastPos= currentPos;
//     std::cout << "L0Muon::ProcDataCnv::unapplyCompression\t:  "
//               <<" compressed.size()"<<"= "<<compressed.size()
//               <<" currentPos"<<"= "<<currentPos
//               <<std::endl;
    currentPos=compressed.find_next(lastPos-1);   

  }

  // Remove the ending 1
  uncompressed.resize(uncompressedSize);
//   std::cout << "L0Muon::ProcDataCnv::unapplyCompression\tOUT - " 
//             <<  " uncompressed.size()= "<< uncompressed.size()
//             <<  " uncompressed.count()= "<< uncompressed.count() <<std::endl;

  return uncompressed;
}

