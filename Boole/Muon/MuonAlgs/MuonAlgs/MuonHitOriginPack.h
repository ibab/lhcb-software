#ifndef MUONALGS_MUONHITORIGINPACK_H 
#define MUONALGS_MUONHITORIGINPACK_H 1
 
 
namespace MuonHitOriginPack{
 
static const unsigned int bitHitOrigin=3;
static const unsigned int bitBX=3;


static const unsigned int shiftHitOrigin  = 0;  
static const unsigned int shiftBX=shiftHitOrigin+bitHitOrigin ;

static const unsigned int maskHitOrigin =  ( ( ( (unsigned int) 1 ) << bitHitOrigin  ) - 1  ) << shiftHitOrigin ;  
static const unsigned int maskBX   =  ( ( ( (unsigned int) 1 ) << bitBX ) - 1  ) << shiftBX ;

inline unsigned int getBit(unsigned int  packedValue, unsigned int shift,
unsigned int mask){
 return ((packedValue&mask)>>shift);
}

inline void  setBit(unsigned int &origValue,
                     unsigned int shift, unsigned int mask,
                     unsigned int bitValue){
 unsigned int                           store   = origValue;
 unsigned int       result=(((bitValue<<shift)&mask)| (store&~mask));
  origValue=result;
} 

}


 
#endif
