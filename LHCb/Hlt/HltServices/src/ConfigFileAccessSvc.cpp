#include "ConfigFileAccessSvc.h"
#include <sstream>
#include <string>
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/convenience.hpp"
namespace fs = boost::filesystem;


#include "GaudiKernel/System.h"

// Factory implementation
DECLARE_COMPONENT(ConfigFileAccessSvc)


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ConfigFileAccessSvc::ConfigFileAccessSvc( const std::string& name, ISvcLocator* pSvcLocator)
  : base_class ( name , pSvcLocator )
{
  declareProperty("Directory", m_dir = "");
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

bool ConfigFileAccessSvc::create_directories( fs::path dir ) const {
   try {
        return fs::create_directories(dir);
   }
   catch ( const fs::filesystem_error& x )  {
        error() << x.what() << endmsg;
   }
   return false;
}

fs::path ConfigFileAccessSvc::dir() const {
  if ( m_dir.empty() ) {
     std::string def( System::getEnv("HLTTCKROOT") );
     if ( !def.empty() ) {
        if (!def.empty()) def += "/config";
     } else {
        throw GaudiException("Environment variable HLTTCKROOT not specified and no explicit "
                             "filename given; cannot obtain location of config.tar.",
                             name(), StatusCode::FAILURE);
     }
     m_dir = def;
  }
  return fs::path(m_dir);
}
   
fs::path
ConfigFileAccessSvc::propertyConfigPath( const PropertyConfig::digest_type& digest ) const {
    std::string sref=digest.str();
    return dir() / "PropertyConfigs" / sref.substr(0,2) / sref;
}

fs::path
ConfigFileAccessSvc::configTreeNodePath( const ConfigTreeNode::digest_type& digest)  const{
    std::string sref=digest.str();
    return dir() / "ConfigTreeNodes" / sref.substr(0,2) / sref;
}

fs::path
ConfigFileAccessSvc::configTreeNodeAliasPath( const ConfigTreeNodeAlias::alias_type& alias ) const {
    return dir() / "Aliases" / alias.str();
}

boost::optional<PropertyConfig>
ConfigFileAccessSvc::readPropertyConfig(const PropertyConfig::digest_type& ref) {
   fs::path fnam = propertyConfigPath(ref);
   if (!fs::exists(fnam)) {
        debug() << "file " << fnam.string() << " does not exist" << endmsg;
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
        debug() << "file " << fnam.string() << " does not exist" << endmsg;
        return boost::optional<ConfigTreeNode>();
   }
   ConfigTreeNode c;
   fs::ifstream s( fnam );
   s >> c;
   return c;
}

boost::optional<ConfigTreeNode>
ConfigFileAccessSvc::readConfigTreeNodeAlias(const ConfigTreeNodeAlias::alias_type& alias) {
   fs::path fnam = configTreeNodeAliasPath(alias);
   if (!fs::exists(fnam)) {
        debug() << "file " << fnam.string() << " does not exist" << endmsg;
        return boost::optional<ConfigTreeNode>();
   }
   std::string sref;
   fs::ifstream s( fnam );
   s >> sref;
   ConfigTreeNode::digest_type ref = ConfigTreeNode::digest_type::createFromStringRep(sref);
   if (!ref.valid()) {
        error() << "content of " << fnam.string() << " not a valid ref" << endmsg;
        return boost::optional<ConfigTreeNode>();
   }
   return readConfigTreeNode(ref);
}

PropertyConfig::digest_type
ConfigFileAccessSvc::writePropertyConfig(const PropertyConfig& config) {

   PropertyConfig::digest_type digest = config.digest();

   fs::path fnam = propertyConfigPath(digest);
   fs::path fdir = fnam.branch_path();
   if (!fs::exists(fdir) && !create_directories(fdir)) {
            error() << " directory " << fdir.string() << " does not exist, and could not create... " << endmsg;
            return PropertyConfig::digest_type::createInvalid();
   }
   if (!fs::exists(fnam)) {
            fs::ofstream s( fnam );
            s << config;
            debug() << " created " << fnam.string() << " for " << config.name() << endmsg;
            return digest;
   } else {
            PropertyConfig x;
            fs::ifstream s( fnam );
            s >> x;
            if ( x==config ) return digest;
            error() << " config already exists, but contents are different..." << endmsg;
            return PropertyConfig::digest_type::createInvalid();
    }
}

ConfigTreeNode::digest_type
ConfigFileAccessSvc::writeConfigTreeNode(const ConfigTreeNode& config) {

   ConfigTreeNode::digest_type digest = config.digest();

   fs::path fnam = configTreeNodePath(digest);
   fs::path fdir = fnam.branch_path();
   if (!fs::exists(fdir) && !create_directories(fdir)) {
            error() << " directory " << fdir.string() << " does not exist, and could not create... " << endmsg;
            return ConfigTreeNode::digest_type::createInvalid();
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
            return ConfigTreeNode::digest_type::createInvalid();
    }
}

// TODO: replace fs::path with a concrete config...
bool
ConfigFileAccessSvc::isCompatible(const ConfigTreeNodeAlias& /*alias*/, const fs::path& /*dirName*/ ) const {
    // TODO: only allow write of TOPLEVEL alias if _consistent_ with other configs in same directory...
    // (nasty: we wrote everything, only to find out in the end that
    // should be common to all implementations -- mix-in ??
    return true;
}

ConfigTreeNodeAlias::alias_type
ConfigFileAccessSvc::writeConfigTreeNodeAlias(const ConfigTreeNodeAlias& alias) {
   // verify that we're pointing at something existing
   if ( !readConfigTreeNode(alias.ref()) ) {
            error() << " Alias points at non-existing entry " << alias.ref() << "... refusing to create." << endmsg;
            return ConfigTreeNodeAlias::alias_type();
   }
   // now write alias...
   fs::path fnam = configTreeNodeAliasPath(alias.alias());
   fs::path fdir = fnam.branch_path();
   if (fs::exists(fdir)) {
            if (!isCompatible(alias,fdir) ) {
                error() << " current TOPLEVEL is not compatible with other config in " << fdir.string() << " refusing to write TOPLEVEL alias " << endmsg;
                return ConfigTreeNodeAlias::alias_type();
            }
   } else if ( !create_directories(fdir) ) {
            error() << " directory " << fdir.string() << " does not exist, and could not create... " << endmsg;
            return ConfigTreeNodeAlias::alias_type();
   }
   if (!fs::exists(fnam)) {
            fs::ofstream s( fnam );
            s << alias.ref();
            info() << " created " << fnam.string() << endmsg;
            return alias.alias();
   } else {
   //@TODO: decide policy: in which cases do we allow overwrites of existing labels?
   // (eg. TCK aliases: no!, tags: maybe... , toplevel: impossible by construction )
   // that policy should be common to all implementations, so move to a mix-in class,
   // or into ConfigTreeNodeAlias itself
            std::string x;
            fs::ifstream s( fnam );
            s >> x;
            if ( ConfigTreeNodeAlias::digest_type::createFromStringRep(x)==alias.ref() ) return alias.alias();
            error() << " Alias already exists, but contents differ... refusing to change" << endmsg;
            return ConfigTreeNodeAlias::alias_type();
   }
}

std::vector<ConfigTreeNodeAlias>
ConfigFileAccessSvc::configTreeNodeAliases(const ConfigTreeNodeAlias::alias_type& alias)
{
    std::vector<ConfigTreeNodeAlias> x;

    // use std::list as iterators are not invalidated when extending list...
    fs::path basedir = dir() / "Aliases" ;
    std::list<fs::path> dirs; dirs.push_back( basedir / alias.major() );

    for (const auto&  dir : dirs ) {

        if ( !fs::exists( dir) ) continue; //@todo: add warning about this...

        fs::directory_iterator end; // default construction yields past-the-end
        for ( fs::directory_iterator i( dir ); i != end; ++i) {
            if ( fs::is_directory(i->status()) ) {
              std::string fn = i->path().filename().string();
              if ( fn  == "CVS" || fn == ".svn" ) continue;
              // push back on stack of directories...
              dirs.push_back(*i);
              debug() << " configTreeNodeAliases: adding dir " << *i << endmsg;
            } else {
              debug() << " configTreeNodeAliases: adding file " << *i << endmsg;
              std::string ref;
              fs::ifstream s( *i );
              s >> ref;
              std::string _alias = i->path().string().substr( basedir.string().size()+1 );
              std::stringstream str;
              str << "Ref: " << ref << '\n' << "Alias: " << _alias << std::endl;
              ConfigTreeNodeAlias a;
              str >> a;
              debug() << " configTreeNodeAliases: content:" << a << endmsg;
              x.push_back(a);
            }
        }
    }
    return x;
}

MsgStream& ConfigFileAccessSvc::msg(MSG::Level level) const {
     if (!m_msg) m_msg.reset( new MsgStream( msgSvc(), name() ));
     return *m_msg << level;
}
