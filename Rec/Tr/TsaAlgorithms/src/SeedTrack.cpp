#include "SeedTrack.h"
#include "Kernel/LHCbID.h"

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>

using namespace boost::lambda;

bool SeedTrack::onTrack(const LHCb::LHCbID id) const{

  bool found = false;

  // x projection
  std::vector<SeedPnt>::const_iterator iterX = std::find_if(m_xPnts.begin(),
                                                      m_xPnts.end(), 
                                                      bind(&SeedPnt::sameID, _1, id));
  if (iterX == m_xPnts.end() ){    
    // x projection
    std::vector<SeedPnt>::const_iterator iterY = std::find_if(m_yPnts.begin(),
                                                        m_yPnts.end(), 
                                                        bind(&SeedPnt::sameID, _1, id));
    if (iterY != m_yPnts.end()) found = true;
  }
  else {
    found = true;
  }
  
  return found;
}
