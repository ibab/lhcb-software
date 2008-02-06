#include "HltBase/ANNSvc.h"
#include <string>
#include "boost/assign/list_of.hpp"

class HltANNSvc : public ANNSvc {
public:
    HltANNSvc( const std::string& name, ISvcLocator *svcLocator )
    :ANNSvc(name, svcLocator, 
            boost::assign::list_of( std::string("SelectionID") )
                                  ( std::string("InfoID")      )
           ) {}
};

#include "GaudiKernel/SvcFactory.h"
DECLARE_SERVICE_FACTORY( HltANNSvc );
