#include "GaudiAlg/GaudiAlgorithm.h"
#include "boost/cstdint.hpp"
#include <string>

namespace LHCb { class ODIN; };
class StatEntity;

class DeterministicPrescaler : public GaudiAlgorithm 
{

public:

  DeterministicPrescaler( const std::string& name, ISvcLocator* pSvcLocator );
  ~DeterministicPrescaler( );
  
  StatusCode initialize();
  StatusCode execute();
  
private:
  boost::uint32_t         m_initial;      // initial seed unique to this instance (computed from the name)
  double                  m_accFrac;      // fraction of input events to accept...
  StatEntity*             m_counter;

  bool accept(const LHCb::ODIN& odin) const ;

};
