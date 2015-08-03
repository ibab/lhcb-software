#include "ConfigCDBAccessSvc.h"
#include "cdb.h"

#include <map>
#include <ctime>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include "boost/iostreams/copy.hpp"
#include "boost/iostreams/filter/zlib.hpp"
#ifndef _WIN32
#include "boost/iostreams/filter/bzip2.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif
#include "boost/iostreams/filtering_stream.hpp"
#include "boost/iostreams/device/back_inserter.hpp"
#include "boost/iostreams/device/array.hpp"
#include "boost/iostreams/stream.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"

#include "boost/integer_traits.hpp"
using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

#include "GaudiKernel/System.h"
#include "GaudiKernel/StringKey.h"

using namespace std;
namespace io = boost::iostreams;
namespace fs = boost::filesystem;

namespace
{
struct DefaultFilenameSelector
{
    bool operator()( const string& /*fname*/ ) const
    {
        return true;
    }
};

struct PrefixFilenameSelector
{
    PrefixFilenameSelector( const string& _prefix ) : prefix( _prefix )
    {
    }
    bool operator()( const string& fname ) const
    {
        return fname.compare( 0, prefix.size(), prefix ) == 0;
    }
    string prefix;
};

uint8_t read8( std::istream& s )
{
    union
    {
        unsigned int ui;
        int i;
    } u;
    u.i = s.get(); // stream will typically return an 'int', as it is a 'char' stream
    uint8_t r = u.ui;
    ;
    return r;
}
uint16_t read16( std::istream& s )
{
    uint16_t r;
    r = uint16_t( read8( s ) );
    r |= uint16_t( read8( s ) ) << 8;
    return r;
}
uint32_t read32( std::istream& s )
{
    uint32_t r;
    r = uint32_t( read16( s ) );
    r |= uint32_t( read16( s ) ) << 16;
    return r;
}
uid_t read_uid( std::istream& s )
{
    uid_t uid = read32( s );
    // std::cout << "uid : " <<  uid << std::endl ;
    return uid;
}
std::time_t read_time( std::istream& s )
{
    std::time_t tm = read32( s );
    // std::cout << "time : " << tm  << " " ;
    // char mbstr[100];
    // if (std::strftime(mbstr, sizeof(mbstr), "%A %c", std::localtime(&tm)))
    // std::cout << mbstr ;
    // std::cout << endl;
    return tm;
}
}

namespace ConfigCDBAccessSvc_details
{

class CDB
{
  public:
    CDB( const std::string& name, ios::openmode mode = ios::in ) : m_fname( name )
    {
        cdb_make_start( &m_ocdb, -1 );
        // if open 'readwrite' we construct a fresh copy of the input
        // database, then extend it, and on 'close' replace the original one
        // with the new one.

        // Note that while appending, we need to keep a 'shadow' copy to satisfy
        // reads
        // to the newly written items...

        int fd = open( m_fname.c_str(), O_RDONLY );

        // if not exist, forget about copying...

        if ( cdb_init( &m_icdb, fd ) < 0 ) cdb_fileno( &m_icdb ) = -1;

        if ( mode & ios::out ) {
            m_oname = fs::unique_path( m_fname.string() + "-%%%%-%%%%-%%%%-%%%%" );
        }

        if ( writing() ) {
            // From http://pubs.opengroup.org/onlinepubs/7908799/xsh/open.html:
            //
            // O_CREAT and O_EXCL are set, open() will fail if the file exists. The
            // check for
            // the existence of the file and the creation of the file if it does not
            // exist will
            // be atomic with respect to other processes executing open() naming the
            // same
            // filename in the same directory with O_EXCL and O_CREAT set. If O_CREAT
            // is not set,
            // the effect is undefined.

            int ofd = open( m_oname.c_str(), O_RDWR | O_CREAT | O_EXCL,
                            S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
            std::cerr << " opened new database " << m_oname << ", fd = " << ofd << std::endl;
            if ( ofd < 0 ) m_error = true;
            cdb_make_start( &m_ocdb, ofd );

            if ( fd >= 0 && ofd >= 0 ) {
                std::cerr << "copying original database entries" << std::endl;
                // copy everything into a 'shadow' database -- basically, a (k,v)
                // vector
                // which preserves insertion order, augmented with a map for fast
                // searches
                // so we can (quickly) avoid duplicate entries...
                unsigned cpos;
                cdb_seqinit( &cpos, &m_icdb );
                unsigned long nrec = 0;
                while ( cdb_seqnext( &cpos, &m_icdb ) > 0 ) {
                    ++nrec;
                    if ( cdb_make_add( &m_ocdb, static_cast<const unsigned char*>(
                                                    cdb_getkey( &m_icdb ) ),
                                       cdb_keylen( &m_icdb ),
                                       static_cast<const unsigned char*>(
                                           cdb_getdata( &m_icdb ) ),
                                       cdb_datalen( &m_icdb ) ) != 0 ) {
                        // handle error...
                        cerr << " failure to put key "
                             << string( static_cast<const char*>(
                                            cdb_getkey( &m_icdb ) ),
                                        cdb_keylen( &m_icdb ) ) << " : " << errno
                             << " = " << strerror( errno ) << endl;
                        m_error = true;
                    }
                }
                std::cerr << "copied " << nrec << " records " << std::endl;
            }
        }
    }

    ~CDB()
    {
        close( cdb_fileno( &m_icdb ) );
        cdb_free( &m_icdb );
        if ( writing() ) {
            auto fd = cdb_fileno( &m_ocdb );
            cdb_make_finish( &m_ocdb );
            close( fd );
            if ( !m_error ) {
                std::cerr << "renaming " << m_oname << ", to " << m_fname << endl;
                fs::rename( m_oname, m_fname );
            } else {
                std::cerr << "encountered an error; leaving original " << m_fname
                     << " untouchted, removing temporary " << m_oname << endl;
                fs::remove( m_oname );
            }
        }
    }

    bool writing() const
    {
        return !m_oname.empty();
    };

    template <typename T>
    bool readObject( T& t, const std::string& key )
    {
        // first check input database...
        if ( cdb_find( &m_icdb, key.c_str(), key.size() ) > 0 ) {
            io::stream<io::array_source> value(
                static_cast<const char*>( cdb_getdata( &m_icdb ) ),
                cdb_datalen( &m_icdb ) );
// 12 bytes of header information...
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
            {
                unsigned int version = read8( value );
                assert( version == 0 );
            }
            unsigned int flags = read8( value );
            assert( flags < 4 );
            {
                unsigned int reserved = read16( value );
                assert( reserved == 0 );
                auto uid = read_uid( value );
                auto tm = read_time( value );
            }
#pragma GCC diagnostic pop

            assert( value.tellg() == 12 );
            io::filtering_istream s;
            switch ( flags & 0x3 ) {
            case 0:
                break; // do nothing...
            case 2:
                s.push( io::bzip2_decompressor() );
                break;
            case 3: {
                io::zlib_params params;
                params.noheader = true;
                s.push( io::zlib_decompressor( params ) );
            } break;
            default:
                std::cerr << "unknown compression flag" << std::endl;
                return 0;
            }
            s.push( value );
            s >> t;
            assert( value.tellg() == cdb_datalen( &m_icdb ) );
            return true;
        }
        // not in input, maybe it's in the write cache?
        if ( writing() ) {
            auto i = m_shadow.find( key );
            if ( i != m_shadow.end() ) {
                std::stringstream s( i->second );
                s >> t;
                return true;
            }
        }
        return false;
    }

    template <typename SELECTOR>
    vector<string> files( const SELECTOR& selector ) const
    {
        vector<string> _keys;
        if ( cdb_fileno( &m_icdb ) >= 0 ) {
            unsigned cpos;
            cdb_seqinit( &cpos, &m_icdb );
            while ( cdb_seqnext( &cpos, &m_icdb ) > 0 ) {
                string key( static_cast<const char*>( cdb_getkey( &m_icdb ) ),
                            cdb_keylen( &m_icdb ) );
                if ( selector( key ) ) _keys.push_back( key );
            }
        }
        if ( writing() ) { // then also check write cache...
            for ( auto& i : m_shadow ) {
                if ( selector( i.first ) ) _keys.push_back( i.first );
            }
        }
        return _keys;
    }

    uid_t getUid() const
    {
#ifndef _WIN32
        if ( m_myUid == 0 ) m_myUid = getuid();
#endif
        return m_myUid;
    }

    int compress( string& str )
    {
        // compress and check if worthwhile...
        string compressed;
        compressed.reserve( str.size() );
        io::filtering_streambuf<io::output> filter;
        io::zlib_params params;
        params.noheader = true;
        filter.push( io::zlib_compressor( params ) );
        filter.push( io::back_inserter( compressed ) );
        io::copy( boost::make_iterator_range( str ), filter, 8192 );
        bool ok = compressed.size() < str.size(); // yes, it's better!
        if ( ok ) str.swap( compressed );
        return ok ? 3 : 0;
    }

    std::vector<unsigned char> make_cdb_record( std::string str, uid_t uid, std::time_t t )
    {
        auto flags = compress( str );
        std::vector<unsigned char> buffer( 12 + str.size(), 0 );
        auto buf = std::begin( buffer );
        *buf++ = 0;     // version number
        *buf++ = flags; // compression
        *buf++ = 0;     // reserved;
        *buf++ = 0;     // reserved;
        assert( sizeof( uid_t ) == 4 );
        *buf++ = ( uid & 0xff );
        *buf++ = ( ( uid >> 8 ) & 0xff );
        *buf++ = ( ( uid >> 16 ) & 0xff );
        *buf++ = ( ( uid >> 24 ) & 0xff );
        assert( sizeof( t ) == 4 ); // seconds since 1970  ( ok for 136 years )
        *buf++ = ( t & 0xff );
        *buf++ = ( ( t >> 8 ) & 0xff );
        *buf++ = ( ( t >> 16 ) & 0xff );
        *buf++ = ( ( t >> 24 ) & 0xff );
        if ( std::distance( std::begin( buffer ), buf ) != 12 ) {
            std::cerr << "ERROR" << std::endl;
        }
        auto e = std::copy_n( std::begin( str ), str.size(), buf );
        if ( e != std::end( buffer ) ) {
            std::cerr << "ERROR" << std::endl;
        }
        return buffer;
    }

    std::vector<unsigned char> make_cdb_record( std::string str )
    {
        return make_cdb_record( std::move(str), getUid(), std::time(nullptr) );
    }

    bool append( const string& key, std::stringstream& is )
    {
        if ( !writing() ) return false;

        // first, look in input database
        std::string rd;
        if ( readObject( rd, key ) ) {
            bool ok = rd == is.str();
            if ( !ok ) {
                std::cerr << "append error: entry present in input , but not equal!"
                          << std::endl;
            }
            return ok;
        }
        // if not there, look in write cache
        auto i = m_shadow.find( key );
        if ( i != m_shadow.end() ) {
            bool ok = i->second == is.str();
            if ( !ok ) {
                std::cerr << "append error: entry present in output , but not equal!"
                          << std::endl;
            }
            return ok;
        }

        // aha, this is an as yet unknown key... insert it!
        m_shadow.emplace( key, is.str() );

        auto record = make_cdb_record( is.str() );
        if ( cdb_make_add( &m_ocdb,
                           reinterpret_cast<const unsigned char*>( key.data() ),
                           key.size(), record.data(), record.size() ) != 0 ) {
            // handle error...
            cerr << " failure to put key " << key << " : " << errno << " = "
                 << strerror( errno ) << endl;
            m_error = true;
            return false;
        }
        // cout << " appended key " << key << " onto shadow -- now " <<
        // m_shadow.size() << " entries " << endl;
        return true;
    }
    bool operator!() const
    {
        return false;
    } // TODO: FIXME: properly implement error checking...
  private:
    mutable struct cdb m_icdb;
    mutable struct cdb_make m_ocdb;
    fs::path m_fname;
    fs::path m_oname;
    map<Gaudi::StringKey, string> m_shadow; // write cache..
    mutable uid_t m_myUid = 0;
    mutable bool m_error = false;
};
}
using namespace ConfigCDBAccessSvc_details;

// Factory implementation
DECLARE_SERVICE_FACTORY( ConfigCDBAccessSvc )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ConfigCDBAccessSvc::ConfigCDBAccessSvc( const string& name,
                                        ISvcLocator* pSvcLocator )
    : Service( name, pSvcLocator ), m_file{ nullptr }
{
    declareProperty( "File", m_name = "" );
    declareProperty( "Mode", m_mode = "ReadOnly" );
}


//=============================================================================
// queryInterface
//=============================================================================
StatusCode ConfigCDBAccessSvc::queryInterface( const InterfaceID& riid,
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
StatusCode ConfigCDBAccessSvc::initialize()
{
    if ( msgLevel( MSG::DEBUG ) ) debug() << "Initialize" << endmsg;
    StatusCode status = Service::initialize();
    if ( !status.isSuccess() ) return status;
    status = setProperties();
    return status;
}

ConfigCDBAccessSvc_details::CDB* ConfigCDBAccessSvc::file() const
{
    if ( !m_file ) {
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
        if ( m_name.empty() ) {
            std::string def( System::getEnv( "HLTTCKROOT" ) );
            if ( !def.empty() ) {
                def += "/config.cdb";
            } else {
               throw GaudiException("Environment variable HLTTCKROOT not specified and no explicit "
                                    "filename given; cannot obtain location of config.cdb.",
                                    name(), StatusCode::FAILURE);
            }
            m_name = def;
        }
        info() << " opening " << m_name << " in mode " << m_mode << endmsg;
        m_file.reset( new CDB( m_name, mode ) );
        if ( !*m_file ) {
            error() << " Failed to open " << m_name << " in mode " << m_mode
                    << endmsg;
            error() << string( strerror( errno ) ) << endmsg;
            m_file.reset( nullptr );
        }
    }
    return m_file.get();
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode ConfigCDBAccessSvc::finalize()
{
    m_file.reset( nullptr ); // close file if still open
    return Service::finalize();
}

string ConfigCDBAccessSvc::propertyConfigPath(
    const PropertyConfig::digest_type& digest ) const
{
    return string( "PC/" ) += digest.str();
}

string ConfigCDBAccessSvc::configTreeNodePath(
    const ConfigTreeNode::digest_type& digest ) const
{
    return string( "TN/" ) += digest.str();
}

string ConfigCDBAccessSvc::configTreeNodeAliasPath(
    const ConfigTreeNodeAlias::alias_type& alias ) const
{
    return string( "AL/" ) += alias.str();
}

template <typename T>
boost::optional<T> ConfigCDBAccessSvc::read( const string& path ) const
{
    if ( msgLevel( MSG::DEBUG ) ) debug() << "trying to read " << path << endmsg;
    if ( file() == 0 ) {
        debug() << "file " << m_name << " not found" << endmsg;
        return boost::optional<T>();
    }
    T c;
    if ( !file()->readObject( c, path ) ) {
        if ( msgLevel( MSG::DEBUG ) )
            debug() << "file " << path << " not found in container " << m_name
                    << endmsg;
        return boost::optional<T>();
    }
    return c;
}

template <typename T>
bool ConfigCDBAccessSvc::write( const string& path, const T& object ) const
{
    boost::optional<T> current = read<T>( path );
    if ( current ) {
        if ( object == current.get() ) return true;
        error() << " object @ " << path
                << "  already exists, but contents are different..." << endmsg;
        return false;
    }
    if ( m_mode == "ReadOnly" ) {
        error() << "attempted write, but file has been opened ReadOnly" << endmsg;
        return false;
    }
    stringstream s;
    s << object;
    return file() && file()->append( path, s );
}

boost::optional<PropertyConfig>
ConfigCDBAccessSvc::readPropertyConfig( const PropertyConfig::digest_type& ref )
{
    return this->read<PropertyConfig>( propertyConfigPath( ref ) );
}

boost::optional<ConfigTreeNode>
ConfigCDBAccessSvc::readConfigTreeNode( const ConfigTreeNode::digest_type& ref )
{
    return this->read<ConfigTreeNode>( configTreeNodePath( ref ) );
}

boost::optional<ConfigTreeNode> ConfigCDBAccessSvc::readConfigTreeNodeAlias(
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

vector<ConfigTreeNodeAlias> ConfigCDBAccessSvc::configTreeNodeAliases(
    const ConfigTreeNodeAlias::alias_type& alias )
{
    vector<ConfigTreeNodeAlias> x;

    string basename( "AL" );
    if ( !file() ) return x;
    vector<string> aliases =
        file()->files( PrefixFilenameSelector( basename + "/" + alias.major() ) );
    for ( const auto& i : aliases ) {
        // TODO: this can be more efficient...
        if ( msgLevel( MSG::DEBUG ) )
            debug() << " configTreeNodeAliases: adding file " << i << endmsg;
        string ref;
        file()->readObject( ref, i );
        stringstream str;
        str << "Ref: " << ref << '\n' << "Alias: " << i.substr( basename.size() + 1 )
            << endl;
        ConfigTreeNodeAlias a;
        str >> a;
        if ( msgLevel( MSG::DEBUG ) )
            debug() << " configTreeNodeAliases: content:" << a << endmsg;
        x.push_back( a );
    }
    return x;
}

PropertyConfig::digest_type
ConfigCDBAccessSvc::writePropertyConfig( const PropertyConfig& config )
{
    PropertyConfig::digest_type digest = config.digest();
    return this->write( propertyConfigPath( digest ), config )
               ? digest
               : PropertyConfig::digest_type::createInvalid();
}

ConfigTreeNode::digest_type
ConfigCDBAccessSvc::writeConfigTreeNode( const ConfigTreeNode& config )
{
    ConfigTreeNode::digest_type digest = config.digest();
    return this->write( configTreeNodePath( digest ), config )
               ? digest
               : ConfigTreeNode::digest_type::createInvalid();
}

ConfigTreeNodeAlias::alias_type
ConfigCDBAccessSvc::writeConfigTreeNodeAlias( const ConfigTreeNodeAlias& alias )
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

MsgStream& ConfigCDBAccessSvc::msg( MSG::Level level ) const
{
    if ( !m_msg ) m_msg.reset( new MsgStream( msgSvc(), name() ) );
    return *m_msg << level;
}
