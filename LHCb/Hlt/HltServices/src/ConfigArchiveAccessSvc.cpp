#include "ConfigArchiveAccessSvc.h"
#include "IArchive.h"
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


#include "GaudiKernel/System.h"


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ConfigArchiveAccessSvc::ConfigArchiveAccessSvc( const std::string& name,
                                                ISvcLocator* pSvcLocator )
    : base_class( name, pSvcLocator )
{ }

//=============================================================================
// Initialization
//=============================================================================
StatusCode ConfigArchiveAccessSvc::initialize()
{
    if ( msgLevel( MSG::DEBUG ) ) debug() << "Initialize" << endmsg;
    StatusCode status = Service::initialize();
    if ( !status.isSuccess() ) return status;
    status = setProperties();
    return status;
}


string ConfigArchiveAccessSvc::propertyConfigPath(
    const PropertyConfig::digest_type& digest ) const
{
    string sref = digest.str();
    return ( ( string( "PropertyConfigs/" ) += sref.substr( 0, 2 ) ) += "/" ) +=
           sref;
}

string ConfigArchiveAccessSvc::configTreeNodePath(
    const ConfigTreeNode::digest_type& digest ) const
{
    string sref = digest.str();
    return ( ( string( "ConfigTreeNodes/" ) += sref.substr( 0, 2 ) ) += "/" ) +=
           sref;
}

string ConfigArchiveAccessSvc::configTreeNodeAliasPath(
    const ConfigTreeNodeAlias::alias_type& alias ) const
{
    return string( "Aliases/" ) += alias.str();
}

template <typename T>
boost::optional<T> ConfigArchiveAccessSvc::read( const string& path ) const
{
    if ( msgLevel( MSG::DEBUG ) ) debug() << "trying to read " << path << endmsg;
    if ( !file() ) return boost::none;
    auto c = file()->get<T>(path);
    if ( !c &&  msgLevel( MSG::DEBUG ) ) {
        debug() << "file " << path << " not found in container " << endmsg;
    }
    return c;
}

template <typename T>
bool ConfigArchiveAccessSvc::write( const string& path, const T& object ) const
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
    s << object.str(); // use the 'old fashioned' format when writing the 'old' archives (for backwards compatibility)...
    return file() && file()->append( path, s );
}

boost::optional<PropertyConfig>
ConfigArchiveAccessSvc::readPropertyConfig( const PropertyConfig::digest_type& ref )
{
    return this->read<PropertyConfig>( propertyConfigPath( ref ) );
}

boost::optional<ConfigTreeNode>
ConfigArchiveAccessSvc::readConfigTreeNode( const ConfigTreeNode::digest_type& ref )
{
    return this->read<ConfigTreeNode>( configTreeNodePath( ref ) );
}

boost::optional<ConfigTreeNode> ConfigArchiveAccessSvc::readConfigTreeNodeAlias(
    const ConfigTreeNodeAlias::alias_type& alias )
{
    string fnam = configTreeNodeAliasPath( alias );
    boost::optional<string> sref = this->read<string>( fnam );
    if ( !sref ) return boost::none;
    ConfigTreeNode::digest_type ref =
        ConfigTreeNode::digest_type::createFromStringRep( *sref );
    if ( !ref.valid() ) {
        error() << "content of " << fnam << " not a valid ref" << endmsg;
        return boost::none;
    }
    return readConfigTreeNode( ref );
}

vector<ConfigTreeNodeAlias> ConfigArchiveAccessSvc::configTreeNodeAliases(
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
ConfigArchiveAccessSvc::writePropertyConfig( const PropertyConfig& config )
{
    PropertyConfig::digest_type digest = config.digest();
    return this->write( propertyConfigPath( digest ), config )
               ? digest
               : PropertyConfig::digest_type::createInvalid();
}

ConfigTreeNode::digest_type
ConfigArchiveAccessSvc::writeConfigTreeNode( const ConfigTreeNode& config )
{
    ConfigTreeNode::digest_type digest = config.digest();
    return this->write( configTreeNodePath( digest ), config )
               ? digest
               : ConfigTreeNode::digest_type::createInvalid();
}

ConfigTreeNodeAlias::alias_type
ConfigArchiveAccessSvc::writeConfigTreeNodeAlias( const ConfigTreeNodeAlias& alias )
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

MsgStream& ConfigArchiveAccessSvc::msg( MSG::Level level ) const
{
    if ( !m_msg ) m_msg.reset( new MsgStream( msgSvc(), name() ) );
    return *m_msg << level;
}
