#include <string>
#include <map>

// Define the parser
#include "GaudiKernel/ParsersFactory.h"

namespace {
   using additionalIDs_t = std::map<std::string, std::map<std::string, unsigned int>>;
}

namespace Gaudi {
namespace Parsers {

// Parser grammar
template <typename Iterator, typename Skipper>
struct Grammar_<Iterator, additionalIDs_t, Skipper > {
   // In this case, the type is a mapping type, so it requires the MapGrammar.
   // For other grammars see GaudiKernel/GrammarsV2.h
   typedef MapGrammar<Iterator, ::additionalIDs_t, Skipper> Grammar;
};

// Additional parser for our property
StatusCode parse(additionalIDs_t& result, const std::string& input) {
    return Gaudi::Parsers::parse_(result, input);
}
}
}

// We also need to be able to print an object of our type as a string that both
// Python and our parser can understand,
#include "GaudiKernel/ToStream.h"
namespace std {
  // This is an example valid for any mapping type.
  ostream& operator<<(ostream& s, const additionalIDs_t& m) {
    bool first = true;
    s << '{';
    for(const auto& i: m) {
      if (first) first = false;
      else s << ", ";
      Gaudi::Utils::toStream(i.first, s) << ": ";
      Gaudi::Utils::toStream(i.second, s);
    }
    s << '}';
    return s;
  }
}

#include "GaudiKernel/Service.h"
#include "GaudiKernel/VectorMap.h"

#include "Kernel/IIndexedANNSvc.h"
#include "Kernel/IPropertyConfigSvc.h"
#include "Kernel/TCK.h"


class TCKANNSvc : public Service
                , virtual public IIndexedANNSvc  {

public:
  TCKANNSvc( const std::string& name, ISvcLocator* pSvcLocator);
  ~TCKANNSvc() = default ;

  StatusCode queryInterface(const InterfaceID& riid, void** ppvUnknown) override;
  StatusCode initialize() override;
  StatusCode finalize() override;

  GaudiUtils::VectorMap<unsigned int, Gaudi::StringKey> i2s(unsigned int index, const Gaudi::StringKey& major) const override;

private:

  // properties
  additionalIDs_t m_additionals;

  mutable IPropertyConfigSvc * m_propertyConfigSvc;
  std::string       m_propertyConfigSvcName;
  std::string       m_instanceName;
  mutable std::map<TCK,const PropertyConfig*> m_cache; // TODO: flush cache if m_instanceName changes
};

#include "GaudiKernel/SvcFactory.h"
DECLARE_SERVICE_FACTORY( TCKANNSvc )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TCKANNSvc::TCKANNSvc( const std::string& name, ISvcLocator* pSvcLocator)
  : Service( name , pSvcLocator )
  , m_propertyConfigSvc{nullptr}
{
  declareProperty("IPropertyConfigSvcInstance", m_propertyConfigSvcName = "PropertyConfigSvc");
  declareProperty("InstanceName", m_instanceName = "HltANNSvc");
  declareProperty("AdditionalIDs", m_additionals);
}

StatusCode
TCKANNSvc::initialize()
{
  StatusCode sc = Service::initialize();
  if (sc.isFailure()) {
    fatal() << "Service::initialize() failed!!!"<< endmsg;
    return sc;
  }

  if( msgLevel(MSG::VERBOSE) ) verbose() << "==> Initialize" << endmsg;

  if (!service( m_propertyConfigSvcName, m_propertyConfigSvc).isSuccess()) {
    fatal() << "TCKANNSvc failed to get the IConfigAccessSvc." << endmsg;
    return StatusCode::FAILURE;
  }

  if (!m_additionals.empty()) {
     warning() << "Additional IDs specified for " << m_instanceName << "." << endmsg
               << "This is an expert options, and not for general use." << endmsg;
  }
  return sc;
}

StatusCode
TCKANNSvc::finalize(  )
{
  if (m_propertyConfigSvc) { m_propertyConfigSvc->release(); m_propertyConfigSvc=nullptr; }
  return Service::finalize();
}


GaudiUtils::VectorMap<unsigned int, Gaudi::StringKey>
TCKANNSvc::i2s(unsigned int tck, const Gaudi::StringKey& major) const
{
    TCK _tck(tck); _tck.normalize();
    auto entry = m_cache.find(_tck);
    if (entry == std::end(m_cache) ) {
        // grab properties of child from config database...
        const ConfigTreeNode* tree = m_propertyConfigSvc->resolveConfigTreeNode( ConfigTreeNodeAlias::alias_type{ std::string("TCK/") +  _tck.str()  } );
        if (!tree) {
          //If we could not resolve the (non-zero) TCK we have a problem
          error() << "Requested TCK " << _tck <<  " could not resolved. Returning an empty map... " << endmsg;
          return {};
        }
        PropertyConfig::digest_type child = m_propertyConfigSvc->findInTree(tree->digest(), m_instanceName);
        if (child.invalid()) {
          error() << "Error finding configuration of " << m_instanceName << " for TCK " << _tck
                  <<  " Returning an empty map... " << endmsg;
          return {};
        }
        const PropertyConfig *config = m_propertyConfigSvc->resolvePropertyConfig(child);
        if (!config) {
          error() << "Error reading configuration of " << m_instanceName << " for TCK " << _tck
                  <<  " Returning an empty map... " << endmsg;
          return {};
        }
        auto status = m_cache.insert( { _tck, config } );
        if (!status.second) {
          error() << "Error updating cache for TCK " << _tck <<  " Returning an empty map... " << endmsg;
          return {};
        }
        entry = status.first;
    }
    auto prop = std::find_if( std::begin(entry->second->properties()), std::end( entry->second->properties() ),
                              [&](const std::pair<std::string,std::string>& p ) { return major.str() == p.first ; } );
    if (prop == std::end(entry->second->properties())) {
      error() << "Error finding requested major " << major <<" in  configuration of " << m_instanceName
              << " for TCK " << _tck <<  " Returning an empty map... " << endmsg;
      return {};
    }
    // now use the property parser to do the hard work of converting the string rep into a map...
    std::map<std::string,int> map;
    // or extend the PropertyConfig interface so it takes a property, invokes 'fromString' on it, and returns StatusCode ?
    // or an templated interface, which wraps a PropertyWithValue around it, then invokes the above, and drops the PropertyWithValue?
    // boost::optional< map<string,int> > = config->assign< map<string,int> >( major );
    // PropertyWithValue<std::map<std::string,int>>{  major.str(), &map, false }.fromString( prop->second );
    // auto status = prp.fromString( prop->second );
    auto status = Gaudi::Parsers::parse(map , prop->second);
    if (status.isFailure()) {
      error() << "Error interpreting requested major " << major <<" in  configuration of " << m_instanceName
              << " for TCK " << _tck <<  " Returning an empty map... " << endmsg;
      return {};
    }
    GaudiUtils::VectorMap<unsigned int, Gaudi::StringKey> result;
    auto additionals = m_additionals.find(major.str());
    bool ok = true;
    if (additionals != end(m_additionals)) {
       for (const auto& item : additionals->second) {
          ok = ok && result.insert(item.second, item.first).second;
       }
    }
    if (!ok) {
      error() << "Duplicate entries in additional map for  major " << major <<" in  configuration of "
              << m_instanceName << " for TCK "
              << _tck <<  " Returning an empty map... " << endmsg;
      return {};
    }
    for ( const auto& item : map )  {
        ok = ok && result.insert( item.second, item.first ).second;
    }
    if (!ok) {
      error() << "Duplicate entries in map for  major " << major <<" in  configuration of " << m_instanceName
              << " for TCK " << _tck <<  " Returning an empty map... " << endmsg;
      return {};
    }
    return result;
}



//=============================================================================
// queryInterface
//=============================================================================
StatusCode TCKANNSvc::queryInterface(const InterfaceID& riid,
                                     void** ppvUnknown) {
  if ( IIndexedANNSvc::interfaceID().versionMatch(riid) )   {
    *ppvUnknown = dynamic_cast<IIndexedANNSvc*>(this);
    addRef();
    return SUCCESS;
  }
  return Service::queryInterface(riid,ppvUnknown);
}
