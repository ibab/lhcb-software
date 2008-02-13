#include "ConfigFileAccessSvc.h"
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
  declareProperty("Directory", m_dir = "/user/graven/tmp/config/");
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


fs::path 
ConfigFileAccessSvc::propertyConfigPath( const PropertyConfig::digest_type& digest ) const {
     std::string sref=digest.str();
     return fs::path(m_dir) / "PropertyConfigs" / sref.substr(0,2) / sref;
}

fs::path 
ConfigFileAccessSvc::configTreeNodePath( const ConfigTreeNode::digest_type& digest)  const{
     std::string sref=digest.str();
     return fs::path(m_dir) / "ConfigTreeNodes" / sref.substr(0,2) / sref;
}

boost::optional<PropertyConfig> 
ConfigFileAccessSvc::readPropertyConfig(const PropertyConfig::digest_type& ref) {
   fs::path fnam = propertyConfigPath(ref);
   if (!fs::exists(fnam)) {
        error() << "file " << fnam.string() << " does not exist" << endmsg;
        return boost::optional<PropertyConfig>();
   }
   PropertyConfig c;
   fs::ifstream s( fnam );
   s >> c;
   return c;
}

boost::optional<ConfigTreeNode> 
ConfigFileAccessSvc::readConfigTreeNode(const ConfigTreeNode::digest_type& ref) {
   fs::path fnam = configTreeNodePath(ref);
   if (!fs::exists(fnam)) {
        error() << "file " << fnam.string() << " does not exist" << endmsg;
        return boost::optional<ConfigTreeNode>();
   }
   ConfigTreeNode c;
   fs::ifstream s( fnam );
   s >> c;
   return c;
}

PropertyConfig::digest_type
ConfigFileAccessSvc::writePropertyConfig(const PropertyConfig& config) {

   PropertyConfig::digest_type digest = config.digest();

   fs::path fnam = propertyConfigPath(digest);
   fs::path fdir = fnam.branch_path();
   if (!fs::exists(fdir) && !fs::create_directory(fdir)) {
            error() << " directory " << fdir.string() << " does not exist, and could not create... " << endmsg;
            return MD5::createInvalidDigest();
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
            return MD5::createInvalidDigest();
    }
}

ConfigTreeNode::digest_type
ConfigFileAccessSvc::writeConfigTreeNode(const ConfigTreeNode& config) {

   ConfigTreeNode::digest_type digest = config.digest();

   fs::path fnam = configTreeNodePath(digest);
   fs::path fdir = fnam.branch_path();
   if (!fs::exists(fdir) && !fs::create_directory(fdir)) {
            error() << " directory " << fdir.string() << " does not exist, and could not create... " << endmsg;
            return MD5::createInvalidDigest();
   }
   if (!fs::exists(fnam)) {
            fs::ofstream s( fnam );
            s << config;
            info() << " created " << fnam.string() << endmsg;
            return digest;
   } else {
            ConfigTreeNode x; 
            fs::ifstream s( fnam );
            s >> x;
            if ( x==config ) return digest;
            error() << " TreeNode already exists, but contents are different..." << endmsg;
            return MD5::createInvalidDigest();
    }
}

MsgStream& ConfigFileAccessSvc::msg(MSG::Level level) const {
     if (m_msg.get()==0) m_msg.reset( new MsgStream( msgSvc(), name() ));
     return *m_msg << level;
}
