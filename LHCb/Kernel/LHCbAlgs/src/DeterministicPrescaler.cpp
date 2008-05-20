#include "DeterministicPrescaler.h"

#include <math.h>

// from Boost
#include "boost/static_assert.hpp"
#include "boost/integer/integer_mask.hpp"
#include "boost/integer_traits.hpp"
using boost::uint32_t;
using boost::uint64_t;

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// from LHCb core
#include "Event/ODIN.h"


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
  assert(s.size()%4==0);
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

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DeterministicPrescaler );

DeterministicPrescaler::DeterministicPrescaler(const std::string& name, ISvcLocator* pSvcLocator) :
    GaudiAlgorithm(name, pSvcLocator) 
  , m_initial(0)
  , m_seen(0)
  , m_passed(0)
{
  declareProperty( "AcceptPattern" ,  m_prescaleSpec = "" ) ;
  declareProperty( "AcceptFraction" , m_accFrac = -1 ) ;
}

DeterministicPrescaler::~DeterministicPrescaler( )
{
}

StatusCode
DeterministicPrescaler::initialize()
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( !sc) return sc;

  m_seen    = 0;
  m_passed  = 0;
  m_initial = mixString( name().size(), name() );

  debug() << " generated initial value " << m_initial << endmsg;
  
  // scan m_prescaleSpec and fill m_prescale accordingly
  // add support for 'repeats' at a later point...
  for (std::string::const_iterator  i=m_prescaleSpec.begin();
       i!=m_prescaleSpec.end(); ++i ) {
    if(*i!='A'&&*i!='R') {
      err()  << "Prescale pattern should be consists of a string of 'A' and 'R'"
             << " to indicate Accept and Reject" << endmsg;
      return StatusCode::FAILURE;
    }
    m_prescale.push_back(*i=='A');
  }

  if (!m_prescale.empty()) {
    info() << "Prescaling events according to the pattern "
           << m_prescale << "[evtnum % " << m_prescale.size() << "]" << endmsg;
  }
  if (m_accFrac>0) {
    info() << "Prescaling events; keeping " << m_accFrac << " of events " << endmsg;
  }

  if (m_accFrac>0 && !m_prescaleSpec.empty()) {
    fatal() << " either specify AcceptPattern, or AcceptFraction, not both " << endmsg;
    return StatusCode::FAILURE;
  }

  return sc;
}

bool 
DeterministicPrescaler::accept(const LHCb::ODIN& odin)  const
{
  uint32_t x = m_initial;
  x = mix64( x, odin.gpsTime() );
  x = mix32( x, odin.runNumber() );
  x = mix64( x, odin.eventNumber() );

  debug() << "  gpsTime: " << odin.gpsTime() 
          << "  run#: "    << odin.runNumber()
          << "  evt#: "    << odin.eventNumber()
          << "  -->  "     << x 
          << endmsg;

  // at this point, we assume 'x' to be uniformly distributed in [0,0xffffffff]
  // (and yes, this was verified to be sufficiently true on a sample of 10K MC events ;-)
  
  // TODO: fill a histogram of 'x'/0xffffffff
  //       compute mean, median, standard deviation on the fly
  //       to verify statistical properties...

  // note that an IEEE754 double has 57 bits of fraction, which is enough precision
  // to cover the entire dynamic range of an uint32_t
  return double(x) < m_accFrac*boost::integer_traits<uint32_t>::const_max;
}

bool 
DeterministicPrescaler::accept(const ulonglong& evtNr) const
{
  return  m_prescale.empty() || m_prescale[evtNr%m_prescale.size()];
}

StatusCode
DeterministicPrescaler::execute()
{
  LHCb::ODIN* odin = get<LHCb::ODIN> ( LHCb::ODINLocation::Default );

  bool acc = ( m_accFrac >= 0 ? accept(*odin)
                              : accept(odin->eventNumber()) );
  setFilterPassed(acc);
  debug() << " run # "   << odin->runNumber() 
          << " event # " << odin->eventNumber() << " : " << (acc?"Accepted":"Rejected") << endmsg ;

  ++m_seen; if (acc) ++m_passed;
  return StatusCode::SUCCESS;
}

StatusCode
DeterministicPrescaler::finalize()
{
  info() << " passed " << m_passed << " out of " << m_seen << " : " << double(m_passed)/double(m_seen) << endmsg;
  return GaudiAlgorithm::finalize();
}
