#include "L0MuonKernel/RawBufferUnit.h"
#include "L0MuonKernel/MuonTriggerUnit.h"

/**
   Constructor.
*/
L0Muon::RawBufferUnit::RawBufferUnit(){
};
/**
   Constructor.
*/
L0Muon::RawBufferUnit::RawBufferUnit(LHCb::MuonTileID id):L0MUnit(id){
  m_evtCounter=0;
  m_compress=true;
};

/**
   Constructor.
*/
L0Muon::RawBufferUnit::RawBufferUnit(DOMNode* pNode):L0MUnit(pNode){
};


/**
   Destructor
*/
L0Muon::RawBufferUnit::~RawBufferUnit(){
}

/**
   Initialize
*/
void L0Muon::RawBufferUnit::initialize(){

  L0MUnit::initialize();

  // Initialize event counter
  m_evtCounter=0;

  // Get properties from top unit
  MuonTriggerUnit * pmuontrigger = dynamic_cast<MuonTriggerUnit *>( parentByType("MuonTriggerUnit"));
  m_compress = pmuontrigger->getProperty("BufferCompression");

}

/**
   Execute
*/
void L0Muon::RawBufferUnit::execute(){
  L0MUnit::execute();

}

/**
   PostExecute
*/
void L0Muon::RawBufferUnit::postexecute(){
  L0MUnit::postexecute();
  // Increment event counter
  m_evtCounter++;
  
  // Release output register (set bit to 0)
  releaseOutputRegisters();
  
  // Clear output register (bitset size set to 0)
  std::map<std::string,L0Muon::Register*>::iterator itoutputs;
  for (itoutputs=m_outputs.begin();itoutputs!=m_outputs.end();itoutputs++) {
    (*itoutputs).second->clear();
  }
  

}
/**
   
*/

unsigned long  L0Muon::RawBufferUnit::formattedCandidate(CandRegisterHandler candRegHandler, int icand, int quarter)
{
  
  unsigned long cand=0;
  if (m_debug) std::cout << "*!* RawBufferUnit::formattedCandidate IN "<<std::endl;

  int  pt      = candRegHandler.getCandPT(icand)     ;
  int  charge  = candRegHandler.getCandCharge(icand) ;
  int  addM3   = candRegHandler.getCandAddM3(icand)  ;
  int  offM2   = candRegHandler.getCandOffM2(icand)  ;
  int  offM1   = candRegHandler.getCandOffM1(icand)  ;
  int  pu      = candRegHandler.getCandPU(icand)     ;
  int  board   = candRegHandler.getCandBoard(icand)  ;

  //   if (m_debug) std::cout << "*!* RawBufferUnit::formattedCandidate pt    = "<< pt    
  //                          << " = " << (unsigned long) ((pt     << RelShiftRawBufPT)     &  RelMaskRawBufPT)
  //                          << " = " << (unsigned long) ((pt     << RelShiftRawBufPT)    )
  //                          <<" ; bits: "  << BitsRawBufPT   
  //                          <<" ; mask: "  << RelMaskRawBufPT   
  //                          <<" ; shift: " << RelShiftRawBufPT 
  //                          <<std::endl;  
  cand +=  (pt     << RelShiftRawBufPT)     &  RelMaskRawBufPT;
  //   if (m_debug) std::cout << "*!* RawBufferUnit::formattedCandidate cand    = "<< cand  <<std::endl;
  //   if (m_debug) std::cout << "*!* RawBufferUnit::formattedCandidate charge= "<< charge
  //                          << " = " << (unsigned long) ((charge << RelShiftRawBufCHARGE) & RelMaskRawBufCHARGE)
  //                          << " = " << (unsigned long) ((charge << RelShiftRawBufCHARGE) )
  //                          <<" ; bits: "  << BitsRawBufCHARGE   
  //                          <<" ; mask: "  << RelMaskRawBufCHARGE   
  //                          <<" ; shift: " << RelShiftRawBufCHARGE 
  //                          <<std::endl;
  cand +=  (charge << RelShiftRawBufCHARGE) & RelMaskRawBufCHARGE;                             
  //   if (m_debug) std::cout << "*!* RawBufferUnit::formattedCandidate cand    = "<< cand  <<std::endl;
  //   if (m_debug) std::cout << "*!* RawBufferUnit::formattedCandidate addM3 = "<< addM3 
  //                          << " = " <<  (unsigned long) ((addM3  << RelShiftRawBufADDM3)  & RelMaskRawBufADDM3)
  //                          << " = " <<  (unsigned long) ((addM3  << RelShiftRawBufADDM3) )
  //                          <<" ; bits: "  << BitsRawBufADDM3   
  //                          <<" ; mask: "  << RelMaskRawBufADDM3  
  //                          <<" ; shift: " << RelShiftRawBufADDM3 
  //                          <<std::endl;
  cand +=  (addM3  << RelShiftRawBufADDM3)  & RelMaskRawBufADDM3;                             
  //   if (m_debug) std::cout << "*!* RawBufferUnit::formattedCandidate cand    = "<< cand  <<std::endl;
  //   if (m_debug) std::cout << "*!* RawBufferUnit::formattedCandidate offM2 = "<< offM2 
  //                          << " = " << (unsigned long) ((offM2  << RelShiftRawBufOFFM2)  & RelMaskRawBufOFFM2)
  //                          << " = " << (unsigned long) ((offM2  << RelShiftRawBufOFFM2)  )
  //                          <<" ; bits: "  << BitsRawBufOFFM2   
  //                          <<" ; mask: "  << RelMaskRawBufOFFM2   
  //                          <<" ; shift: " << RelShiftRawBufOFFM2 
  //                          <<std::endl;
  cand +=  (offM2  << RelShiftRawBufOFFM2)  & RelMaskRawBufOFFM2;                             
  //   if (m_debug) std::cout << "*!* RawBufferUnit::formattedCandidate cand    = "<< cand  <<std::endl;
  //   if (m_debug) std::cout << "*!* RawBufferUnit::formattedCandidate offM1 = "<< offM1 
  //                          << " = " << (unsigned long) ((offM1  << RelShiftRawBufOFFM1)  & RelMaskRawBufOFFM1)
  //                          << " = " << (unsigned long) ((offM1  << RelShiftRawBufOFFM1)  )
  //                          <<" ; bits: "  << BitsRawBufOFFM1   
  //                          <<" ; mask: "  << RelMaskRawBufOFFM1   
  //                          <<" ; shift: " << RelShiftRawBufOFFM1 
  //                          <<std::endl;
  cand +=  (offM1  << RelShiftRawBufOFFM1)  & RelMaskRawBufOFFM1;                             
  //   if (m_debug) std::cout << "*!* RawBufferUnit::formattedCandidate cand    = "<< cand  <<std::endl;
  //   if (m_debug) std::cout << "*!* RawBufferUnit::formattedCandidate pu    = "<< pu     
  //                          << " = " << (unsigned long) ((pu     << RelShiftRawBufPU)     & RelMaskRawBufPU)
  //                          << " = " << (unsigned long) ((pu     << RelShiftRawBufPU)     )
  //                          <<" ; bits: "  << BitsRawBufPU   
  //                          <<" ; mask: "  << RelMaskRawBufPU   
  //                          <<" ; shift: " << RelShiftRawBufPU 
  //                          <<std::endl;
  cand +=  (pu     << RelShiftRawBufPU)     & RelMaskRawBufPU;                             
  //   if (m_debug) std::cout << "*!* RawBufferUnit::formattedCandidate cand    = "<< cand  <<std::endl;
  //   if (m_debug) std::cout << "*!* RawBufferUnit::formattedCandidate board = "<< board  
  //                          << " = " <<  (unsigned long) ((board  << RelShiftRawBufBOARD)  &RelMaskRawBufBOARD)
  //                          << " = " <<  (unsigned long) ((board  << RelShiftRawBufBOARD)  )
  //                          <<" ; bits: "  << BitsRawBufBOARD 
  //                          <<" ; mask: "  << RelMaskRawBufBOARD 
  //                          <<" ; shift: " << RelShiftRawBufBOARD
  //                          <<std::endl;
  cand +=  (board  << RelShiftRawBufBOARD)  &RelMaskRawBufBOARD;
  //   if (m_debug) std::cout << "*!* RawBufferUnit::formattedCandidate cand    = "<< cand  <<std::endl;
  if (quarter>0) 
    cand +=  (quarter  << RelShiftRawBufQUARTER)  &RelMaskRawBufQUARTER;

      
  return cand;
  
}

int L0Muon::RawBufferUnit::compress(Register *reg, int start, int stop){

  // Compress the bitset in the register between start and stop

  //   std::cout << "L0Muon::RawBufferUnit::compress "<<"IN - "<<reg<< " " <<start<< " "  << stop<<"\n";
  if (start==stop) return 1;
  //   std::cout << "L0Muon::RawBufferUnit::compress type = "<<type() << "m_mid"<<m_mid.toString() <<"\n";

  int totalSize =  reg->size();
  //std::cout << "L0Muon::RawBufferUnit::compress  totalSize= "<<totalSize <<"\n";

  // Extract the bit set to be compressed
  boost::dynamic_bitset<> bitset_to_compress = reg->getBitset(stop-start,start);
  int originalSize =  bitset_to_compress.size();
  //std::cout << "L0Muon::RawBufferUnit::compress  originalSize= "<<originalSize <<"\n";
  
  // Apply compression
  boost::dynamic_bitset<> compressed= apply0Counting(bitset_to_compress);
  //boost::dynamic_bitset<> compressed= applyRLE(bitset_to_compress);
  int compressedSize = compressed.size();
  //std::cout << "L0Muon::RawBufferUnit::compress  compressedSize= "<<compressedSize <<"\n";
   
  // Update the register
  reg->set(compressed,compressedSize,start);
  reg->set(reg->getBitset(totalSize-stop,stop),totalSize-stop,start+compressedSize);
  int newSize = totalSize - originalSize + compressedSize;
  //std::cout << "L0Muon::RawBufferUnit::compress  newSize= "<< newSize<<"\n";
  reg->resize(newSize);
  
//   std::cout << "L0Muon::RawBufferUnit::compress  OUT - = "
//             <<  " reg->getBitset().size()= "  << reg->getBitset().size()
//             <<  " reg->getBitset().count()= " << reg->getBitset().count() <<"\n";

  // Return compression factor
  return compressedSize/(stop-start);
}


boost::dynamic_bitset<>  L0Muon::RawBufferUnit::apply0Counting(boost::dynamic_bitset<> bitset_to_compress){
  // Compress the bit set in input
//   std::cout << "L0Muon::RawBufferUnit::apply0Counting IN - "
//             <<" bitset_to_compress.size()= "<< bitset_to_compress.size()
//             <<" bitset_to_compress.count()= "<< bitset_to_compress.count()<<"\n";

  int nbitsPrefix  =L0Muon::encodingPrefixSizeRawBuf;
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
//     std::cout << "L0Muon::RawBufferUnit::apply0Counting 0:.."<<"  currentPos= "<<currentPos<<"\n";
  
    // Code the sequence of 0
    nchar = currentPos-lastPos;
//     std::cout << "L0Muon::RawBufferUnit::apply0Counting 0:  "<<"  nchar= "<<nchar<<"\n";
    n0         = nchar/maximumLength;
//     std::cout << "L0Muon::RawBufferUnit::apply0Counting 0:  "<<"  n0= "<<n0<<"\n";
    leftover   = nchar%maximumLength;
//     std::cout << "L0Muon::RawBufferUnit::apply0Counting 0:  "<<"  leftover= "<<leftover<<"\n";
    size       = n0 + nbitsSequence;
//     std::cout << "L0Muon::RawBufferUnit::apply0Counting 0:  "
//               <<"  size= "<<size<<"\n";
    sequence = (leftover<<1)+1;
//     std::cout << "L0Muon::RawBufferUnit::apply0Counting 0:  "
//               <<"  sequence= "<<sequence<<"\n";
    boost::dynamic_bitset<> tmpbitset(size,sequence);
//     std::cout << "L0Muon::RawBufferUnit::apply0Counting 0:  "
//               <<"  tmpbitset= "<<tmpbitset<<"\n";
    leftoverSequence= tmpbitset;
//     std::cout << "L0Muon::RawBufferUnit::apply0Counting 0:  "
//               <<"  leftoverSequence= "<<leftoverSequence<<"\n";
    encodedSequence.resize(size);
    encodedSequence|=leftoverSequence<<n0;
//     std::cout << "L0Muon::RawBufferUnit::apply0Counting 0:  "
//               <<"  encodedSequence= "<<encodedSequence<<"\n";
  

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
//   std::cout << "L0Muon::RawBufferUnit::apply0Counting last 0:last"
//             <<" currentPos= "<<currentPos<<" ;lastPos= "<<lastPos<<"\n";
  nchar = bitset_to_compress.size()-lastPos;
//   std::cout << "L0Muon::RawBufferUnit::apply0Counting last 0:  "<<" nchar= "<<nchar<<"\n";
  n0         = nchar/maximumLength;
//   std::cout << "L0Muon::RawBufferUnit::apply0Counting 0:  "<<"  n0= "<<n0<<"\n";
  leftover   = nchar%maximumLength;
//   std::cout << "L0Muon::RawBufferUnit::apply0Counting 0:  "<<"  leftover= "<<leftover<<"\n";
  size       = n0;
  size+=nbitsSequence;
  sequence = (leftover<<1)+1;
//   std::cout << "L0Muon::RawBufferUnit::apply0Counting 0:  "<<"  sequence= "<<sequence<<"\n";
  leftoverSequence= boost::dynamic_bitset<>(size,sequence);
//   std::cout << "L0Muon::RawBufferUnit::apply0Counting 0:  "<<"  leftoverSequence= "<<leftoverSequence<<"\n";
  encodedSequence.resize(size);
  encodedSequence|=leftoverSequence<<n0;
//   std::cout << "L0Muon::RawBufferUnit::apply0Counting 0:  "<<"  encodedSequence= "<<encodedSequence<<"\n";

  // Add the encoded sequence to the compressed bitset
  shift   = compressedBitset.size();
  newsize = shift + encodedSequence.size();
  compressedBitset.resize(newsize);
  encodedSequence.resize(newsize);
  compressedBitset|=encodedSequence<<shift;

//   std::cout << "L0Muon::RawBufferUnit::apply0Counting "<<"  compressedBitset= "<<compressedBitset<<"\n";
//   std::cout << "L0Muon::RawBufferUnit::apply0Counting "<<"OUT  - "
//             << " compressedBitset.size = "<<compressedBitset.size() 
//             << " compressedBitset.count = "<<compressedBitset.count() <<"\n";
  return compressedBitset;
}


boost::dynamic_bitset<>  L0Muon::RawBufferUnit::applyRLE(boost::dynamic_bitset<> bitset_to_compress){
  // Compress the bit set in input
  //std::cout << "L0Muon::RawBufferUnit::applyRLE "
  //          <<"IN bitset_to_compress.size()= "
  //          << bitset_to_compress.size()<<"\n";

  boost::dynamic_bitset<> compressedBitset;
  boost::dynamic_bitset<> encodedSequence;

  boost::dynamic_bitset<>::size_type lastPos=0;
  boost::dynamic_bitset<>::size_type currentPos=bitset_to_compress.find_next(lastPos);
  boost::dynamic_bitset<>::size_type nextPos=0;

  int nchar;
  int shift;
  int  newsize;
  while (currentPos<bitset_to_compress.size()) {
    //std::cout << "L0Muon::RawBufferUnit::applyRLE 0:"<<"currentPos= "<<currentPos<<"\n";

    // Code the sequence of 0
    nchar = currentPos-lastPos;
    //std::cout << "L0Muon::RawBufferUnit::applyRLE 0:"<<"nchar= "<<nchar<<"\n";
    encodedSequence = encodeRLE(nchar, false);

    // Add the encoded sequence to the compressed bitset
    shift   = compressedBitset.size();
    newsize = shift +encodedSequence.size();
    compressedBitset.resize(newsize);
    encodedSequence.resize(newsize);
    compressedBitset|=encodedSequence<<shift;

    // Code the sequence of 1
    nextPos = currentPos + 1;
    while( bitset_to_compress.test(nextPos) ){
      nextPos++;
    }
    nchar= nextPos-currentPos;
    //std::cout << "L0Muon::RawBufferUnit::applyRLE 1:"<<"nchar= "<<nchar<<"\n";
    encodedSequence = encodeRLE(nchar, true);
    
    // Add the encoded sequence to the compressed bitset
    shift   = compressedBitset.size();
    newsize = shift +encodedSequence.size();
    compressedBitset.resize(newsize);
    encodedSequence.resize(newsize);
    compressedBitset|=encodedSequence<<shift;

    lastPos= currentPos;
    currentPos=bitset_to_compress.find_next(lastPos);    
  }

  // Code the last sequence of 0
  //std::cout << "L0Muon::RawBufferUnit::applyRLE last 0:"<<" currentPos= "<<currentPos<<" ;lastPos= "<<lastPos<<"\n";
  nchar = bitset_to_compress.size()-lastPos;
  //std::cout << "L0Muon::RawBufferUnit::applyRLE last 0:"<<" nchar= "<<nchar<<"\n";
  encodedSequence = encodeRLE(nchar, false);

  // Add the encoded sequence to the compressed bitset
  shift   = compressedBitset.size();
  newsize = shift + encodedSequence.size();
  compressedBitset.resize(newsize);
  encodedSequence.resize(newsize);
  compressedBitset|=encodedSequence<<shift;

  //std::cout << "L0Muon::RawBufferUnit::applyRLE "<<"OUT  -"
  //          << "- compressedBitset.size = "<<compressedBitset.size() <<"\n";
  return compressedBitset;
}

boost::dynamic_bitset<> L0Muon::RawBufferUnit::encodeRLE( int nchar, bool ch){

  //std::cout << "L0Muon::RawBufferUnit::encodeRLE "<<"IN "<< nchar<< " " << ch<<"\n";
  // Encode a sequence of 'nchar' identical character ('ch=0/1')

  int nbitsPrefix   = L0Muon::encodingPrefixSizeRawBuf;
  int nbitsSequence = nbitsPrefix+1;
  int maximumLength = int(pow(2,nbitsPrefix));
  
  int nfullparts = nchar/maximumLength;
  int leftover   = nchar%maximumLength;
  int nparts     = nfullparts;
  if (leftover>0) nparts++;
  int size = nparts*(nbitsSequence);
  //std::cout << "L0Muon::RawBufferUnit::encodeRLE "<<"nfullparts ="<<nfullparts  <<"\n";
  //std::cout << "L0Muon::RawBufferUnit::encodeRLE "<<"leftover   ="<<leftover    <<"\n";
  //std::cout << "L0Muon::RawBufferUnit::encodeRLE "<<"nparts     ="<<nparts      <<"\n";
  //std::cout << "L0Muon::RawBufferUnit::encodeRLE "<<"size       ="<<size        <<"\n";

  boost::dynamic_bitset<> encodedSequence(size);

  for (int ipart=0; ipart<nfullparts; ipart++) {
    boost::dynamic_bitset<> part(size, (unsigned long) maximumLength-1);
    part.set(nbitsPrefix,ch);
    encodedSequence|=part<<ipart*(nbitsSequence);
  }
  if (leftover>0) {
    boost::dynamic_bitset<> part(size, (unsigned long) leftover-1);
    part.set(nbitsPrefix,ch);
    encodedSequence|=part<<(nparts-1)*(nbitsSequence);    
  }

  //std::cout << "L0Muon::RawBufferUnit::encodeRLE "<<"OUT  -"
  //          << "- compressedBitset.size = "<<encodedSequence.size() <<"\n";
  return encodedSequence;
  
}
