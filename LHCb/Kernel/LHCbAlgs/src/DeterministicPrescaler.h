#include "GaudiAlg/GaudiAlgorithm.h"
#include "boost/dynamic_bitset.hpp"
#include "boost/cstdint.hpp"
#include <string>

namespace LHCb { class ODIN; };

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
  std::string             m_prescaleSpec; // pre-scale specification
  boost::dynamic_bitset<> m_prescale;     // bitmap determined from the above during 'initialize'

  bool accept(const LHCb::ODIN& odin) const ;
  bool accept(const ulonglong& evtNr) const;

};
