#include "GaudiAlg/GaudiAlgorithm.h"
#include <boost/dynamic_bitset.hpp>
#include <string>

class DeterministicPrescaler:public GaudiAlgorithm 
{

public:

  DeterministicPrescaler( const std::string& name, ISvcLocator* pSvcLocator );
  ~DeterministicPrescaler( );
  
  StatusCode initialize();
  StatusCode execute();
  
private:
  std::string             m_prescaleSpec; // pre-scale specification
  boost::dynamic_bitset<> m_prescale;     // bitmap determined from the above during 'initialize'
};
