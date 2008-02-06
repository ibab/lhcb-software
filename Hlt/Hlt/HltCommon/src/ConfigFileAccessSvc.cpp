#include "ConfigFileAccessSvc.h"
#include "md5.h"
#include <sstream>

#include "boost/lexical_cast.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/filesystem/operations.hpp"
namespace fs = boost::filesystem;


#include "GaudiKernel/SvcFactory.h"


// Factory implementation
DECLARE_SERVICE_FACTORY(ConfigFileAccessSvc)


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ConfigFileAccessSvc::ConfigFileAccessSvc( const std::string& name, ISvcLocator* pSvcLocator)
  : Service ( name , pSvcLocator )
{
  declareProperty("Directory", m_dir = "/user/graven/tmp/config/PropertyConfig");
}

//=============================================================================
// Destructor
//=============================================================================
ConfigFileAccessSvc::~ConfigFileAccessSvc() {}

//=============================================================================
// queryInterface
//=============================================================================
StatusCode ConfigFileAccessSvc::queryInterface(const InterfaceID& riid,
                                               void** ppvUnknown) {
  if ( IConfigAccessSvc::interfaceID().versionMatch(riid) )   {
    *ppvUnknown = (IConfigAccessSvc*)this;
    addRef();
    return SUCCESS;
  }
  return Service::queryInterface(riid,ppvUnknown);
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ConfigFileAccessSvc::initialize() {
  info() << "Initialize" << endmsg;
  StatusCode status = Service::initialize();
  if ( !status.isSuccess() )   return status;
  return setProperties();
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode ConfigFileAccessSvc::finalize() {
  return Service::finalize();
}


boost::optional<PropertyConfig> 
ConfigFileAccessSvc::read(const PropertyConfig::digest_type& ref) {
   std::string sref = ref.str();
   fs::path fnam = fs::path(m_dir) / sref.substr(0,2) / sref;
   if (!fs::exists(fnam)) {
        error() << "file " << fnam.string() << " does not exist" << endmsg;
        return boost::optional<PropertyConfig>();
   }
   PropertyConfig c;
   fs::ifstream s( fnam );
   s >> c;
   return c;
}

PropertyConfig::digest_type
ConfigFileAccessSvc::write(const PropertyConfig& config) {

   PropertyConfig::digest_type digest = config.digest();
   std::string sref=digest.str();

   fs::path fnam = fs::path(m_dir) / sref.substr(0,2) / sref;
   fs::path fdir = fnam.branch_path();
   if (!fs::exists(fdir) && !fs::create_directory(fdir)) {
            error() << " directory " << fdir.string() << " does not exist, and could not create... " << endmsg;
            return PropertyConfig::createNullDigest();
   }
   if (!fs::exists(fnam)) {
            fs::ofstream s( fnam );
            s << config;
            info() << " created " << fnam.string() << " for " << config.name() << endmsg;
            return digest;
   } else {
            PropertyConfig x; 
            fs::ifstream s( fnam );
            s >> x;
            if ( x==config ) return digest;
            error() << " config already exists, but contents are different..." << endmsg;
            return PropertyConfig::createNullDigest();
    }
}

MsgStream& ConfigFileAccessSvc::msg(MSG::Level level) const {
     if (m_msg.get()==0) m_msg.reset( new MsgStream( msgSvc(), name() ));
     return *m_msg << level;
}
