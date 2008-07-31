#include "HltBase/ANNSvc.h"
#include <string>
#include "boost/assign/list_of.hpp"

class LumiANNSvc : public ANNSvc {
public:
  LumiANNSvc( const std::string& name, ISvcLocator *svcLocator )
    :ANNSvc(name, svcLocator, 
            boost::assign::list_of( std::string("LumiCounters") )
	    ) 
  {    }
};

#include "GaudiKernel/SvcFactory.h"
DECLARE_SERVICE_FACTORY( LumiANNSvc );
