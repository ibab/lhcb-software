#include "OfflineDeterministicPrescaler.h"

#include <math.h>

// from LHCb core
#include "Event/RecHeader.h"

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
DECLARE_ALGORITHM_FACTORY( OfflineDeterministicPrescaler )

OfflineDeterministicPrescaler::OfflineDeterministicPrescaler(const std::string& name, ISvcLocator* pSvcLocator) :
    GaudiAlgorithm(name, pSvcLocator) 
  , m_acc(boost::integer_traits<uint32_t>::const_max)
  , m_counter(0)
  , m_initial(0)
{
  declareProperty( "AcceptFraction" , m_accFrac = 1 )->declareUpdateHandler( &OfflineDeterministicPrescaler::update, this);
}

OfflineDeterministicPrescaler::~OfflineDeterministicPrescaler( )
{
}

void OfflineDeterministicPrescaler::update(Property&) 
{
  update();
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "frac: " << m_accFrac << " acc: 0x" << std::hex << m_acc << endmsg;
}

StatusCode
OfflineDeterministicPrescaler::initialize()
{
  const StatusCode sc = GaudiAlgorithm::initialize();
  if ( !sc) return sc;

  m_initial = mixString( name().size(), name() );
  m_counter = &counter("#accept");

  if (msgLevel(MSG::DEBUG)) debug() << " generated initial value " << m_initial << endmsg;
  
  if (m_acc!=boost::integer_traits<uint32_t>::const_max)  {
      info() << "Prescaling events; keeping " << m_accFrac << " of events " << endmsg;
  }
  return sc;
}

bool OfflineDeterministicPrescaler::accept(const LHCb::RecHeader& header)  const
{
  uint32_t x = m_initial;
  x = mix64( x, header.gpsTime() );
  x = mix32( x, header.runNumber() );
  x = mix64( x, (ulonglong)header.evtNumber() );

  if (msgLevel(MSG::DEBUG)) debug() << "  gpsTime: " << header.gpsTime() 
                                    << "  run#: "    << header.runNumber()
                                    << "  evt#: "    << header.evtNumber()
                                    << "  -->  "     << x 
                                    << endmsg;

  // at this point, we assume 'x' to be uniformly distributed in [0,0xffffffff]
  // (and yes, this was verified to be sufficiently true on a sample of 10K MC events ;-)
  return x < m_acc;
}

bool OfflineDeterministicPrescaler::accept()  const{
  return (    ( m_acc == boost::integer_traits<uint32_t>::const_max ) 
              || ( m_acc !=0 && accept( *get<LHCb::RecHeader> ( LHCb::RecHeaderLocation::Default )))
              );
}


StatusCode OfflineDeterministicPrescaler::execute(){
  bool acc = accept();
  setFilterPassed(acc);
  *m_counter += acc;
  if (msgLevel(MSG::DEBUG)) debug() << (acc?"Accepted":"Rejected") << endmsg ;
  return StatusCode::SUCCESS;
}
