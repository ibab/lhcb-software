#include "GaudiAlg/GaudiAlgorithm.h"
#include "boost/cstdint.hpp"
#include <string>

namespace LHCb { class RecHeader; }
class StatEntity;

class OfflineDeterministicPrescaler : public GaudiAlgorithm 
{

public:

  OfflineDeterministicPrescaler( const std::string& name, ISvcLocator* pSvcLocator );
  ~OfflineDeterministicPrescaler( );
  
  StatusCode initialize();
  StatusCode execute();
  
private:
  double                  m_accFrac;      // fraction of input events to accept...
  boost::uint32_t         m_acc;          // integer representation of the above
  boost::uint32_t         m_initial;      // initial seed unique to this instance (computed from the name)
  StatEntity*             m_counter;

  inline bool accept(const LHCb::RecHeader& header) const ;
  void update(Property&) ;

};

