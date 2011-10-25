// $Id: $
// Include files 
#include <cmath>
#include <string>

// from Boost
#include "boost/integer/integer_mask.hpp"
#include "boost/integer_traits.hpp"
using boost::uint32_t;
using boost::uint64_t;

namespace Random {
uint32_t mix(uint32_t state)
{
   // note: the constants below are _not_ arbitrary, but are picked
   //       carefully such that the bit shuffling has a large 'avalanche' effect...
   //       See http://bretm.home.comcast.net/~bretm/hash/
   // 
   // note: as a result, you might call this a quasi-random (not to be confused
   //       with psuedo-random!) number generator, in that it generates an output
   //       which satisfies a requirement on the uniformity of its output distribution.
   //       (and not on the predictability of the next number in the sequence,
   //       based on knowledge of the preceding numbers)
   //
   // note: another way to look at this is is as an (approximation of an) evaporating 
   //       black hole: whatever you dump in to it, you get something uniformly 
   //       distributed back ;-)
   //
   state += (state << 16); state ^= (state >> 13);
   state += (state <<  4); state ^= (state >>  7);
   state += (state << 10); state ^= (state >>  5);
   state += (state <<  8); state ^= (state >> 16);
   return state;
}

// mix some 'extra' entropy into 'state' and return result
uint32_t mix32(uint32_t state, uint32_t extra)
{ 
   return mix( state + extra ); 
}

// mix some 'extra' entropy into 'state' and return result
uint32_t mix64(uint32_t state, uint64_t extra)
 { 
   typedef boost::low_bits_mask_t<32>  mask_t;
   state = mix32( state , uint32_t( extra        & mask_t::sig_bits_fast) );
   return  mix32( state , uint32_t((extra >> 32) & mask_t::sig_bits_fast) );
}

// mix some 'extra' entropy into 'state' and return result
uint32_t mixString(uint32_t state, const std::string& extra)
{ 
   // prefix name with ' ' until the size is a multiple of 4.
   std::string s = std::string((4-extra.size()%4)%4,' ') + extra;
   for (size_t i=0; i < s.size()/4; ++i) {
      //FIXME: this might do something different on big endian vs. small endian machines...
      uint32_t x = uint32_t(s[i*4  ]) 
         | uint32_t(s[i*4+1])<< 8 
         | uint32_t(s[i*4+2])<<16 
         | uint32_t(s[i*4+3])<<24 ;
      state = mix32( state , x );
   }
   return state;
}

}
