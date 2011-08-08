#include "GaudiAlg/GaudiAlgorithm.h"
#include "boost/cstdint.hpp"
#include <string>

// from Boost
#include "boost/integer/integer_mask.hpp"
#include "boost/integer_traits.hpp"
using boost::uint32_t;
using boost::uint64_t;

namespace LHCb { class RecHeader; }
class StatEntity;

class OfflineDeterministicPrescaler : public GaudiAlgorithm 
{

public:

  OfflineDeterministicPrescaler( const std::string& name, ISvcLocator* pSvcLocator );
  ~OfflineDeterministicPrescaler( );
  
  StatusCode initialize();
  StatusCode execute();
  
protected:
  double                  m_accFrac;      // fraction of input events to accept...
  boost::uint32_t         m_acc;          // integer representation of the accept rate
  bool accept() const ;
  bool accept(const LHCb::RecHeader& header) const ;
  inline void update(){
    m_acc = ( m_accFrac<=0 ? 0 
              : m_accFrac>=1 ? boost::integer_traits<uint32_t>::const_max 
              : boost::uint32_t( m_accFrac*boost::integer_traits<uint32_t>::const_max ) 
              );
  };
  StatEntity*             m_counter;

private:
  boost::uint32_t         m_initial;      // initial seed unique to this instance (computed from the name)

  void update(Property&) ;

};

