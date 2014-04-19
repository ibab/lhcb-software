#include "ConfigTarFileAccessSvc.h"
#include <sstream>
#include <string>
#include <cerrno>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;

#include "boost/filesystem/path.hpp"
namespace fs = boost::filesystem;


#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/System.h"

#include "tar.h"


using ConfigTarFileAccessSvc_details::TarFile;

// Factory implementation
DECLARE_SERVICE_FACTORY( ConfigTarFileAccessSvc )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ConfigTarFileAccessSvc::ConfigTarFileAccessSvc( const string& name,
                                                ISvcLocator* pSvcLocator )
    : Service( name, pSvcLocator ), m_file{ nullptr }
{
    string def( System::getEnv( "HLTTCKROOT" ) );
    if ( !def.empty() ) def += "/config.tar";
    declareProperty( "File", m_name = def );
    declareProperty( "Mode", m_mode = "ReadOnly" );
    declareProperty( "CompressOnWrite", m_compress = true );
}

//=============================================================================
// Destructor
//=============================================================================
ConfigTarFileAccessSvc::~ConfigTarFileAccessSvc()
{
}

//=============================================================================
// queryInterface
//=============================================================================
StatusCode ConfigTarFileAccessSvc::queryInterface( const InterfaceID& riid,
                                                   void** ppvUnknown )
{
    if ( IConfigAccessSvc::interfaceID().versionMatch( riid ) ) {
        *ppvUnknown = (IConfigAccessSvc*)this;
        addRef();
        return SUCCESS;
    }
    return Service::queryInterface( riid, ppvUnknown );
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ConfigTarFileAccessSvc::initialize()
{
    if ( msgLevel( MSG::DEBUG ) ) debug() << "Initialize" << endmsg;
    StatusCode status = Service::initialize();
    if ( !status.isSuccess() ) return status;
    status = setProperties();
    return status;
}

ConfigTarFileAccessSvc_details::TarFile* ConfigTarFileAccessSvc::file() const
{
    if ( m_file.get() == 0 ) {
        if ( m_mode != "ReadOnly" && m_mode != "ReadWrite" &&
             m_mode != "Truncate" ) {
            error() << "invalid mode: " << m_mode << endmsg;
            return 0;
        }
        ios::openmode mode = ( m_mode == "ReadWrite" )
                                 ? ( ios::in | ios::out | ios::ate )
                                 : ( m_mode == "Truncate" )
                                       ? ( ios::in | ios::out | ios::trunc )
                                       : ios::in;

        info() << " opening " << m_name << " in mode " << m_mode << endmsg;
        m_file.reset( new TarFile( m_name, mode, m_compress ) );
        if ( !*m_file ) {
            error() << " Failed to open " << m_name << " in mode " << m_mode
                    << endmsg;
            error() << string( strerror( errno ) ) << endmsg;
            m_file.reset( 0 );
        }
    }
    return m_file.get();
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode ConfigTarFileAccessSvc::finalize()
{
    m_file.reset( 0 ); // close file if still open
    return Service::finalize();
}

string ConfigTarFileAccessSvc::propertyConfigPath(
    const PropertyConfig::digest_type& digest ) const
{
    string sref = digest.str();
    return ( ( string( "PropertyConfigs/" ) += sref.substr( 0, 2 ) ) += "/" ) +=
           sref;
}

string ConfigTarFileAccessSvc::configTreeNodePath(
    const ConfigTreeNode::digest_type& digest ) const
{
    string sref = digest.str();
    return ( ( string( "ConfigTreeNodes/" ) += sref.substr( 0, 2 ) ) += "/" ) +=
           sref;
}

string ConfigTarFileAccessSvc::configTreeNodeAliasPath(
    const ConfigTreeNodeAlias::alias_type& alias ) const
{
    return string( "Aliases/" ) += alias.str();
}

template <typename T>
boost::optional<T> ConfigTarFileAccessSvc::read( const string& path ) const
{
    if ( msgLevel( MSG::DEBUG ) ) debug() << "trying to read " << path << endmsg;
    if ( !file() ) {
        debug() << "file " << m_name << " not found" << endmsg;
        return boost::optional<T>();
    }
    auto c = file()->get<T>(path);
    if ( !c &&  msgLevel( MSG::DEBUG ) ) {
            debug() << "file " << path << " not found in container " << m_name
                    << endmsg;
    }
    return c;
}

template <typename T>
bool ConfigTarFileAccessSvc::write( const string& path, const T& object ) const
{
    boost::optional<T> current = read<T>( path );
    if ( current ) {
        if ( object == current.get() ) return true;
        error() << " object @ " << path
                << "  already exists, but contents are different..." << endmsg;
        return false;
    }
    if ( !file()->writeable() ) {
        error() << "attempted write, but file has been opened ReadOnly" << endmsg;
        return false;
    }
    stringstream s;
    s << object;
    return file() && file()->append( path, s );
}

boost::optional<PropertyConfig>
ConfigTarFileAccessSvc::readPropertyConfig( const PropertyConfig::digest_type& ref )
{
    return this->read<PropertyConfig>( propertyConfigPath( ref ) );
}

boost::optional<ConfigTreeNode>
ConfigTarFileAccessSvc::readConfigTreeNode( const ConfigTreeNode::digest_type& ref )
{
    return this->read<ConfigTreeNode>( configTreeNodePath( ref ) );
}

boost::optional<ConfigTreeNode> ConfigTarFileAccessSvc::readConfigTreeNodeAlias(
    const ConfigTreeNodeAlias::alias_type& alias )
{
    string fnam = configTreeNodeAliasPath( alias );
    boost::optional<string> sref = this->read<string>( fnam );
    if ( !sref ) return boost::optional<ConfigTreeNode>();
    ConfigTreeNode::digest_type ref =
        ConfigTreeNode::digest_type::createFromStringRep( *sref );
    if ( !ref.valid() ) {
        error() << "content of " << fnam << " not a valid ref" << endmsg;
        return boost::optional<ConfigTreeNode>();
    }
    return readConfigTreeNode( ref );
}

vector<ConfigTreeNodeAlias> ConfigTarFileAccessSvc::configTreeNodeAliases(
    const ConfigTreeNodeAlias::alias_type& alias )
{
    vector<ConfigTreeNodeAlias> x;

    string basename( "Aliases" );
    if ( !file() ) return x;
    PrefixFilenameSelector selector( basename + "/" + alias.major() );
    for ( const auto& i : file()->files( selector )) {
        // TODO: this can be more efficient...
        if ( msgLevel( MSG::DEBUG ) )
            debug() << " configTreeNodeAliases: adding file " << i << endmsg;
        auto ref = file()->get<string>( i );
        string _alias = i.substr( basename.size() + 1 );
        stringstream str;
        str << "Ref: " << *ref << '\n' << "Alias: " << _alias << endl;
        ConfigTreeNodeAlias a;
        str >> a;
        if ( msgLevel( MSG::DEBUG ) )
            debug() << " configTreeNodeAliases: content:" << a << endmsg;
        x.push_back( a );
    }
    return x;
}

PropertyConfig::digest_type
ConfigTarFileAccessSvc::writePropertyConfig( const PropertyConfig& config )
{
    PropertyConfig::digest_type digest = config.digest();
    return this->write( propertyConfigPath( digest ), config )
               ? digest
               : PropertyConfig::digest_type::createInvalid();
}

ConfigTreeNode::digest_type
ConfigTarFileAccessSvc::writeConfigTreeNode( const ConfigTreeNode& config )
{
    ConfigTreeNode::digest_type digest = config.digest();
    return this->write( configTreeNodePath( digest ), config )
               ? digest
               : ConfigTreeNode::digest_type::createInvalid();
}

ConfigTreeNodeAlias::alias_type
ConfigTarFileAccessSvc::writeConfigTreeNodeAlias( const ConfigTreeNodeAlias& alias )
{
    // verify that we're pointing at something existing
    if ( !readConfigTreeNode( alias.ref() ) ) {
        error() << " Alias points at non-existing entry " << alias.ref()
                << "... refusing to create." << endmsg;
        return ConfigTreeNodeAlias::alias_type();
    }
    // now write alias...
    fs::path fnam = configTreeNodeAliasPath( alias.alias() );
    boost::optional<string> x = read<string>( fnam.string() );
    if ( !x ) {
        stringstream s;
        s << alias.ref();
        if ( !file() ) {
            error() << " container file not found during attempted write of "
                    << fnam.string() << endmsg;
            return ConfigTreeNodeAlias::alias_type();
        }
        if ( file()->append( fnam.string(), s ) ) {
            info() << " created " << fnam.string() << endmsg;
            return alias.alias();
        } else {
            error() << " failed to write " << fnam.string() << endmsg;
            return ConfigTreeNodeAlias::alias_type();
        }
    } else {
        //@TODO: decide policy: in which cases do we allow overwrites of existing
        //labels?
        // (eg. TCK aliases: no!, tags: maybe... , toplevel: impossible by
        // construction )
        // that policy should be common to all implementations, so move to a mix-in
        // class,
        // or into ConfigTreeNodeAlias itself
        if ( ConfigTreeNodeAlias::digest_type::createFromStringRep( *x ) ==
             alias.ref() )
            return alias.alias();
        error() << " Alias already exists, but contents differ... refusing to change"
                << endmsg;
        return ConfigTreeNodeAlias::alias_type();
    }
}

MsgStream& ConfigTarFileAccessSvc::msg( MSG::Level level ) const
{
    if ( m_msg.get() == 0 ) m_msg.reset( new MsgStream( msgSvc(), name() ) );
    return *m_msg << level;
}
