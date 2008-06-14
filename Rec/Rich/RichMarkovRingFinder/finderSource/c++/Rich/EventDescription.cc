

#include "EventDescription.h"

namespace Lester
{

  void EventDescription::fill ( GenRingF::GenericResults & results ) const
  {
    results.rings.clear();
    results.rings.reserve(circs.size());
    unsigned int i=0;
    for ( Circs::const_iterator it = circs.begin();
          it != circs.end(); ++it, ++i )
    {
      results.rings.push_back(GenRingF::GenericRing( GenRingF::GenericRingIndex(i),
                                                     it->centre().x(),
                                                     it->centre().y(),
                                                     it->radius()) );
    }
  }

  std::ostream & EventDescription::printMeTo(std::ostream & os) const
  {
    int i = 0;
    os << "EventDescription[ ";
    for ( Circs::const_iterator it = circs.begin();
          it != circs.end(); ++it, ++i )
    {
      os << " circ-" << i << "=" << *it;
    }
    return os << " ]";
  }

  //--------------------------------------------------------------------------------------------

}
