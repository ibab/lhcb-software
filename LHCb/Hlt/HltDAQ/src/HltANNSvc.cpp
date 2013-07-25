#include "ANNSvc.h"
#include <string>
#include "boost/assign/list_of.hpp"
#include <boost/functional/hash.hpp>

class HltANNSvc : public ANNSvc {
public:
    HltANNSvc( const std::string& name, ISvcLocator *svcLocator )
    :ANNSvc(name, svcLocator, 
            boost::assign::list_of( std::string("Hlt1SelectionID") )
                                  ( std::string("Hlt2SelectionID") )
                                  ( std::string("InfoID")          )
                                  ( std::string("RoutingBits")     )
           ) 
    {
        declareProperty("allowUndefined",m_allowUndefined=true,"do we allow undefined, on-demand generated, key/value pairs?");
    }
    using ANNSvc::handleUndefined;
    virtual boost::optional<minor_value_type> handleUndefined(const major_key_type& major, const std::string& minor) const;
private:
    bool              m_allowUndefined;
};

#include "GaudiKernel/SvcFactory.h"
DECLARE_SERVICE_FACTORY( HltANNSvc )

boost::optional<IANNSvc::minor_value_type> 
HltANNSvc::handleUndefined(const major_key_type& major, const std::string& minor) const
{
   if (!m_allowUndefined)  return minor_value_type();

   static boost::hash<std::string> hasher; 
   log() << MSG::DEBUG << "handleUndefined called for " << major << " : " << minor
                       << " -->  " << hasher(minor) << endmsg;
   return minor_value_type(minor,hasher(minor));
}
