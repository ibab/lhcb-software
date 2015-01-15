// $Id: Prescale.cpp,v 1.1 2010-02-12 14:17:28 ibelyaev Exp $
// =============================================================================
// Include files 
// =============================================================================
// GauidKernel
// =============================================================================
#include "GaudiKernel/ToStream.h"
// =============================================================================
// LoKiHlt 
// =============================================================================
#include "LoKi/Prescale.h"
// =============================================================================
// Boost
// =============================================================================
#include "boost/integer/integer_mask.hpp"
#include "boost/integer_traits.hpp"
// =============================================================================
/** @file 
 *  Implementation file for class LoKi::Odin::Prescale
 *  @see LoKi::Cuts::ODIN_PRESCALE
 *  @see DeterministicPrescaler 
 *  The idea belongs to Gerhard The Great Raven
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2010-02-12
 */
// =============================================================================
namespace 
{
  // ===========================================================================
  using boost::uint32_t ;
  using boost::uint64_t ; 
  // =============================================================================
  /// stollen form Gerhard The Great 
  inline uint32_t mix(uint32_t state)
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
  // ===========================================================================
  // mix some 'extra' entropy into 'state' and return result
  inline uint32_t mix32(uint32_t state, uint32_t extra)
  { 
    return mix( state + extra ); 
  }
  // mix some 'extra' entropy into 'state' and return result
  inline uint32_t mix64(uint32_t state, uint64_t extra)
  { 
    typedef boost::low_bits_mask_t<32>  mask_t;
    state = mix32( state , uint32_t( extra        & mask_t::sig_bits_fast) );
    return  mix32( state , uint32_t((extra >> 32) & mask_t::sig_bits_fast) );
  }
  // mix some 'extra' entropy into 'state' and return result
  inline uint32_t mixString(uint32_t state, const std::string& extra)
  { 
    // prefix name with ' ' until the size is a multiple of 4.
    std::string s = std::string((4-extra.size()%4)%4,' ') + extra;
    for (size_t i=0; i < s.size()/4; ++i) 
    {
      // FIXME: this might do something different on big endian 
      // vs. small endian machines...
      uint32_t x = uint32_t(s[i*4  ]) 
        | uint32_t(s[i*4+1])<< 8 
        | uint32_t(s[i*4+2])<<16 
        | uint32_t(s[i*4+3])<<24 ;
      state = mix32( state , x );
    }
    return state;
  }
  // =========================================================================== 
}
// =============================================================================
// constructor from the accept fraction and the seed 
// =============================================================================
LoKi::Odin::Prescale::Prescale 
( const double       accept , 
  const std::string& seed   ) 
  : LoKi::AuxFunBase ( std::tie ( accept , seed ) ) 
  , LoKi::BasicFunctors<const LHCb::ODIN*>::Predicate () 
  , m_accept  ( accept ) 
  , m_seed    ( seed   )
  , m_initial () 
{
  m_initial = mixString ( seed.size() , seed ) ;
  if ( 0 >= m_accept ) 
  { Warning ( "Non-positive 'AcceptFraction is specified" ) ; }
  if ( 1 <  m_accept ) 
  { Warning ( "'AcceptFraction exceeds 1"                 ) ; }
}
// =============================================================================
// MANDATORY: virtual destructor 
// =============================================================================
LoKi::Odin::Prescale::~Prescale() {}
// =============================================================================  
// MANDATORY: clone method ("virtual constructor")
// =============================================================================
LoKi::Odin::Prescale*
LoKi::Odin::Prescale::clone() const { return new Prescale ( *this ) ; }
// =============================================================================
// MANDATORY: the only essential method 
// =============================================================================
LoKi::Odin::Prescale::result_type 
LoKi::Odin::Prescale::operator() 
  ( LoKi::Odin::Prescale::argument a ) const 
{
  Assert ( 0 != a , "LHCb::ODIN* point to NULL!" ) ;
  //
  uint32_t x = m_initial;
  x = mix64 ( x , a -> gpsTime     () ) ;
  x = mix32 ( x , a -> runNumber   () ) ;
  x = mix64 ( x , a -> eventNumber () ) ;
  //
  // at this point, we assume 'x' to be uniformly 
  // distributed in [0,0xffffffff]
  // (and yes, this was verified to be sufficiently 
  // true on a sample of 10K MC events ;-)
  
  // note that an IEEE754 double has 57 bits of fraction, 
  // which is enough precision
  // to cover the entire dynamic range of an uint32_t
  return double(x) < m_accept * boost::integer_traits<uint32_t>::const_max;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& 
LoKi::Odin::Prescale::fillStream ( std::ostream& s ) const 
{
  s << " ODIN_PRESCALE( " << m_accept ;
  if ( !m_seed.empty() ) 
  { s << " , " ; Gaudi::Utils::toStream ( m_seed , s ) ; }
  return s << " ) " ;   
}
// =============================================================================
// The END 
// =============================================================================

