#include "TarFileAccess.h"

#include <sstream>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <time.h>

#include "GaudiKernel/ToolFactory.h"

#ifndef _WIN32
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#else
typedef int uid_t;
typedef int gid_t;
#endif
using namespace std;

#include "boost/regex.hpp"
#include "boost/assign/list_of.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/filesystem/path.hpp"
namespace fs = boost::filesystem;

#include "boost/iostreams/filtering_stream.hpp"
#include "boost/iostreams/copy.hpp"
#ifndef _WIN32
#include "boost/iostreams/filter/gzip.hpp"
#endif
#include "boost/iostreams/slice.hpp"
#include "boost/iostreams/operations.hpp"
#include "boost/iostreams/seek.hpp"
namespace io = boost::iostreams;

#include "GaudiKernel/StringKey.h"
#include "GaudiKernel/GaudiException.h"

namespace
{

struct DefaultFilenameSelector
{
    bool operator()( const std::string& /*fname*/ ) const
    {
        return true;
    }
};

struct PrefixFilenameSelector
{
    PrefixFilenameSelector( const std::string& _prefix ) : prefix( _prefix )
    {
    }
    bool operator()( const std::string& fname ) const
    {
        return fname.compare( 0, prefix.size(), prefix ) == 0;
    }
    std::string prefix;
};

/* POSIX tar Header Block, from POSIX 1003.1-1990  */
struct posix_header
{                       /* byte offset */
    char name[100];     /*   0-99 */
    char mode[8];       /* 100-107 */
    char uid[8];        /* 108-115 */
    char gid[8];        /* 116-123 */
    char size[12];      /* 124-135 */
    char mtime[12];     /* 136-147 */
    char chksum[8];     /* 148-155 */
    char typeflag;      /* 156-156 */
    char linkname[100]; /* 157-256 */
    char magic[6];      /* 257-262 */
    char version[2];    /* 263-264 */
    char uname[32];     /* 265-296 */
    char gname[32];     /* 297-328 */
    char devmajor[8];   /* 329-336 */
    char devminor[8];   /* 337-344 */
    char prefix[155];   /* 345-499 */
    char padding[12];   /* 500-512 (pad to exactly the TAR_BLOCK_SIZE) */
};

template <typename T>
std::string convert( const T& in )
{
    // take into account that eg. name and prefix do not have a
    // terminating '\0' in case they occupy the full length...
    std::string s( in, sizeof( in ) );
    std::string::size_type last = s.find( char( 0 ) );
    return ( last == std::string::npos ) ? s : s.substr( 0, last );
}

// size_t maxValWithDigits(unsigned digits, unsigned bitsPerDigit) {
//    return (1UL << (digits*bitsPerDigit)) - 1 ;
//}

template <typename T>
char* putString( const char* s, T& buffer )
{
    return strncpy( buffer, s, sizeof( buffer ) );
}
template <typename T, size_t Size>
bool putOctal( T val, char ( &buf )[Size] )
{
    // octal needs 3 bits per digit, and we need a terminal '\0'
    // uintmax_t m = maxValWithDigits( Size-1, 3)
    // TODO: check whether val fits within size octal characters...
    size_t size = Size;
    buf[--size] = '\0';
    do {
        buf[--size] = '0' + ( val & ( ( 1 << 3 ) - 1 ) );
        val >>= 3;
    } while ( size );
    return true;
}

bool isZero( const struct posix_header& h )
{
    const char* i = (const char*)( &h );
    const char* end = i + 512;
    while ( i < end && *i++ == 0 ) {/* nothing*/
    }
    return i == end;
}

void zero( struct posix_header& h )
{
    memset( &h, 0, sizeof( posix_header ) );
}

/* A nice enum with all the possible tar file content types */
enum TarFileType {
    REGTYPE = '0',   /* regular file */
    REGTYPE0 = '\0', /* regular file (ancient bug compat)*/
    GNUTYPE_LONGLINK = 'K',
    GNUTYPE_LONGNAME = 'L',
};
typedef enum TarFileType TarFileType;
}

namespace TarFileAccess_details
{
class TarFile
{
  public:
    TarFile( const std::string& name, ios::openmode mode = ios::in,
             bool compressOnWrite = false )
        : m_name( name )
        , m_leof( 0 )
        , m_indexUpToDate( false )
        , m_myUid( 0 )
        , m_myGid( 0 )
        , m_compressOnWrite( compressOnWrite )
    {
        m_file.open( m_name.c_str(), mode | ios::in | ios::binary );
    }
    bool good() const
    {
        return m_file.good();
    }
    istream* open( const std::string& name )
    {
        const map<Gaudi::StringKey, Info>& myIndex = getIndex();
        map<Gaudi::StringKey, Info>::const_iterator i = myIndex.find( name );
        if ( i != myIndex.end() ) {
            // slice works relative to the current file offset, as it works on an
            // istream...
            m_file.seekg( 0, std::ios_base::beg );
            // cerr << "got file " << i->second.name << " of size " << i->second.size
            // <<  " at offset " << i->second.offset << (i->second.compressed?"
            // (compressed) ":"") << endl;
            io::filtering_istream* in = new io::filtering_istream();
            if ( i->second.compressed ) {
#ifdef _WIN32
                return 0;
#else
                in->push( io::gzip_decompressor() );
#endif
            }
            in->push( io::slice( m_file, i->second.offset, i->second.size ) );
            return in;
        }
        return 0;
    }

    bool exists( const std::string& path )
    {
        const map<Gaudi::StringKey, Info>& myIndex = getIndex();
        return myIndex.find( path ) != myIndex.end();
    }
    template <typename SELECTOR>
    std::vector<std::string> files( const SELECTOR& selector ) const
    {
        std::vector<std::string> f;
        const map<Gaudi::StringKey, Info>& myIndex = getIndex();
        for ( map<Gaudi::StringKey, Info>::const_iterator i = myIndex.begin();
              i != myIndex.end(); ++i ) {
            if ( selector( i->first ) ) f.push_back( i->first );
        }
        return f;
    }
    std::vector<std::string> files() const
    {
        return files( DefaultFilenameSelector() );
    }
    bool append( const string& name, std::stringstream& is );

    ~TarFile()
    {
        m_file.close();
    }

  private:
    struct Info
    {
        Info()
            : size( 0 ), type( TarFileType( 0 ) ), offset( 0 ), compressed( false )
        {
        }
        std::string name;
        size_t size;
        TarFileType type;
        size_t offset;
        bool compressed;
    };

    bool _append( const string& name, std::stringstream& is );
    bool index( std::streamoff start = 0 ) const;
    const std::map<Gaudi::StringKey, Info>& getIndex() const
    {
        if ( !m_indexUpToDate ) m_indexUpToDate = index();
        if ( !m_indexUpToDate ) {
            throw GaudiException( "Failed to index tarfile ", m_name,
                                  StatusCode::FAILURE );
        }
        return m_index;
    }
    long computeChecksum( const posix_header& header ) const;
    bool interpretHeader( posix_header& header, struct Info& info ) const;
    // Read an octal value in a field of the specified width, with optional
    // spaces on both sides of the number and with an optional null character
    // at the end.  Returns -1 on an illegal format.
    bool isOctal( const char& ch ) const
    {
        return ( ( ch >= '0' ) && ( ch <= '7' ) );
    }
    long getOctal( const char* cp, int size ) const
    {
        long val = 0;
        for ( ; ( size > 0 ) && ( *cp == ' ' ); cp++, size-- )
            ;
        if ( ( size == 0 ) || !isOctal( *cp ) ) return -1;
        for ( ; ( size > 0 ) && isOctal( *cp ); size-- )
            val = ( val << 3 ) + ( *cp++ - '0' );
        for ( ; ( size > 0 ) && ( *cp == ' ' ); cp++, size-- )
            ;
        if ( ( size > 0 ) && *cp ) return -1;
        return val;
    }
    uid_t getUid() const
    {
#ifndef _WIN32
        if ( m_myUid == 0 ) m_myUid = getuid();
#endif
        return m_myUid;
    }
    gid_t getGid() const
    {
#ifndef _WIN32
        if ( m_myGid == 0 ) m_myGid = getgid();
#endif
        return m_myGid;
    }
    const char* getUname() const
    {
#ifndef _WIN32
        if ( m_uname.empty() ) {
            struct passwd* passwd = getpwuid( getUid() );
            m_uname = ( passwd ? passwd->pw_name : "" );
        }
#endif
        return m_uname.c_str();
    }
    const char* getGname() const
    {
#ifndef _WIN32
        if ( m_gname.empty() ) {
            struct group* group = getgrgid( getGid() );
            m_gname = ( group ? group->gr_name : "" );
        }
#endif
        return m_gname.c_str();
    }

    std::string m_name;
    mutable fstream m_file;
    mutable std::map<Gaudi::StringKey, Info> m_index;
    mutable long m_leof;
    mutable bool m_indexUpToDate;
    mutable std::string m_gname;
    mutable std::string m_uname;
    mutable uid_t m_myUid;
    mutable gid_t m_myGid;
    bool m_compressOnWrite;
};

bool TarFile::append( const string& name, std::stringstream& is )
{
#ifndef _WIN32
    if ( m_compressOnWrite && is.str().size() > 512 &&
         ( name.size() < 3 || name.compare( name.size() - 3, 3, ".gz" ) != 0 ) ) {
        std::stringstream out;
        io::filtering_istream in;
        in.push( io::gzip_compressor( 9 ) );
        in.push( is );
        io::copy( in, out, is.str().size() );
        // TODO: check that the compressed version actually occupies less blocks
        //       if not, it's useless to compress..
        return _append( name + ".gz", out );
    }
#endif
    return _append( name, is );
}

bool TarFile::_append( const string& name, std::stringstream& is )
{
    // TODO: check if file is open in read/write mode...
    m_indexUpToDate = false;
    std::string bstring = is.str();
    size_t size = bstring.size();
    const char* buffer = bstring.c_str();
    if ( m_file.tellp() >= 0 && m_leof != m_file.tellp() ) {
        m_file.seekp( m_leof, ios::beg ); // where do we start writing?
    }
    posix_header header;
    memset( &header, 0, sizeof( header ) );
    if ( name.size() > sizeof( header.name ) ) {
        // try to split it on a '/'
        // otherwise, go for the GNU LongLink solution...

        // generate longlink header...
        putString( "././@LongLink", header.name );
        header.typeflag = GNUTYPE_LONGNAME;
        putOctal( name.size(), header.size );
        putString( "ustar ", header.magic );
        putString( "00", header.version );
        // and write header
        putOctal( computeChecksum( header ), header.chksum );
        m_file.write( (const char*)&header, sizeof( header ) );
        // followed by name, padded up to 512 byte block..
        std::string nameBuffer( name );
        if ( nameBuffer.size() % 512 )
            nameBuffer.resize( ( ( nameBuffer.size() / 512 ) + 1 ) * 512, '\0' );
        m_file.write( nameBuffer.c_str(), nameBuffer.size() );

        // clear and continue with a header with truncated name...
        memset( &header, 0, sizeof( header ) );
    }
    putString( name.c_str(), header.name );
#ifdef _WIN32
    putOctal( 33060u, header.mode );
#else
    putOctal( S_IFREG | S_IRUSR | S_IRGRP | S_IROTH, header.mode );
#endif
    putString( "ustar ", header.magic );
    putString( "00", header.version );
    putOctal( getUid(), header.uid );
    putOctal( getGid(), header.gid );
    putOctal( size, header.size );
    putOctal( time( 0 ), header.mtime );
    header.typeflag = REGTYPE;
    putString( getUname(), header.uname );
    putString( getGname(), header.gname );
    putOctal( 0, header.devmajor );
    putOctal( 0, header.devminor );

    // last but not least, compute checksum...
    putOctal( computeChecksum( header ), header.chksum );
    m_file.write( (const char*)&header, sizeof( header ) );

    // and write the file contents, padded with zeros
    m_file.write( buffer, size );
    static const size_t blockingFactor = 20;
    static char zeros[blockingFactor * 512] = {0};
    if ( size % 512 ) {
        m_file.write( zeros, 512 - size % 512 );
    }
    // update the logical EOF
    streamoff leof = m_file.tellp();
    // write end-of-file marker : 2 zero blocks, padded up to the blocking factor
    m_file.write( zeros, 2 * 512 );
    if ( m_leof % ( blockingFactor * 512 ) ) {
        m_file.write( zeros,
                      blockingFactor * 512 - m_leof % ( blockingFactor * 512 ) );
    }
    m_file.flush();
    m_indexUpToDate = index( m_leof );
    if ( m_leof != leof ) {
        cerr << "oops: read-back eof not what was predicted: " << m_leof << " vs. "
             << leof << endl;
        return false;
    }

    return true;
}

long TarFile::computeChecksum( const posix_header& header ) const
{
    /* Check the checksum */
    long sum = 0;
    unsigned char* s = (unsigned char*)&header;
    for ( int i = sizeof( header ); i-- != 0; ) sum += *s++;
    /* Remove the effects of the checksum field (replace
     * with blanks for the purposes of the checksum) */
    for ( int i = sizeof header.chksum; i-- != 0; ) {
        sum -= (unsigned char)header.chksum[i];
    }
    sum += ' ' * sizeof header.chksum;
    return sum;
}

bool TarFile::interpretHeader( posix_header& header, Info& info ) const
{
    if ( strncmp( header.magic, "ustar ", 6 ) &&
         strncmp( header.magic, "ustar\0", 6 ) ) {
        return false;
    }
    long chksum = getOctal( header.chksum, sizeof( header.chksum ) );
    /* Check the checksum */
    long sum = computeChecksum( header );
    if ( sum != chksum ) {
        std::cerr << " bad chksum " << sum << " vs. " << chksum << std::endl;
        return false;
    }
    info.type = TarFileType( header.typeflag );
    long size = getOctal( header.size, sizeof( header.size ) );
    if ( size < 0 ) {
        std::cerr << " got negative file size: " << info.size << std::endl;
        return false;
    }
    info.size = size;
    info.name = convert( header.name );
    if ( header.prefix[0] != 0 ) {
        std::string prefix = convert( header.prefix );
        info.name = prefix + "/" + info.name;
    }

    if ( info.type == GNUTYPE_LONGNAME ) {
        // current header is a dummy 'flag', followed by data blocks
        // with name as content (length of name is 'size' of data)
        assert( info.name == "././@LongLink" );
        // first read the real, untruncated name as data
        std::ostringstream fname;
        io::copy( io::slice( m_file, 0, info.size ), fname );
        size_t padding = info.size % 512;
        if ( padding != 0 ) m_file.seekg( 512 - padding, std::ios::cur );
        // and now get another header, which contains a truncated name
        // but which is otherwise the 'real' one
        m_file.read( (char*)&header, sizeof( header ) );
        if ( !interpretHeader( header, info ) ) return false;
        // so we overwrite the truncated one with the long one...
        info.name = fname.str();
    }
    info.offset = m_file.tellg(); // this goes here, as the longlink handling
                                  // reads an extra block...
    // if name ends in .gz, assume it is gzipped.
    // Strip the name down, and flag as compressed in info.
    info.compressed = ( info.name.size() > 3 &&
                        info.name.compare( info.name.size() - 3, 3, ".gz" ) == 0 );
    if ( info.compressed ) info.name = info.name.substr( 0, info.name.size() - 3 );

    return true;
}

bool TarFile::index( std::streamoff offset ) const
{
    posix_header header;
    if ( offset == 0 ) m_index.clear();
    m_file.seekg( offset, ios::beg );
    while ( m_file.read( (char*)&header, sizeof( header ) ) ) {
        Info info;
        if ( !interpretHeader( header, info ) ) {
            // check whether we're at the end of the file: (at least) two all-zero
            // headers)
            if ( isZero( header ) ) {
                m_file.read( (char*)&header, sizeof( header ) );
                if ( isZero( header ) ) {
                    m_leof = m_file.tellg() - 2 * std::streamoff( sizeof( header ) );
                    m_file.seekg( 0, std::ios::beg );
                    return true;
                }
            }
            std::cerr << "failed to interpret header preceeding  " << m_file.tellg()
                      << " in tarfile " << m_name << std::endl;
            m_file.seekg( 0, std::ios::beg );
            return false;
        }
        if ( info.name.empty() ) {
            std::cerr << " got empty name " << std::endl;
            break;
        }
        if ( ( info.type == REGTYPE || info.type == REGTYPE0 ) &&
             info.name[info.name.size() - 1] != '/' &&
             info.name.find( "/CVS/" ) == string::npos &&
             info.name.find( "/.svn/" ) == string::npos ) {
            // TODO: check for duplicates!!
            m_index.insert( make_pair( Gaudi::StringKey( info.name ), info ) );
        }
        // round up size to block size, and skip to next header...
        size_t skip = info.size;
        size_t padding = skip % 512;
        if ( padding != 0 ) skip += 512 - padding;
        m_file.seekg( skip, std::ios::cur );
    }
    m_leof = 0;
    m_file.clear();
    m_file.seekg( 0, std::ios::beg );
    return true;
}
}

// Factory implementation
DECLARE_TOOL_FACTORY( TarFileAccess )

//=============================================================================
// implementation of TarFileAccess
//=============================================================================
TarFileAccess::TarFileAccess( const std::string& type, const std::string& name,
                              const IInterface* parent )
    : base_class( type, name, parent )
{
}

TarFileAccess::~TarFileAccess()
{
    for ( container_t::iterator i = m_tarFiles.begin(); i != m_tarFiles.end();
          ++i ) {
        delete i->second; // this closes the files
    }
    m_tarFiles.clear();
}

const std::vector<std::string>& TarFileAccess::protocols() const
{
    static const std::vector<std::string> s_proto =
        boost::assign::list_of( std::string( "TarFile" ) )( std::string(
            "Tarfile" ) )( std::string( "tarfile" ) )( std::string( "tarFile" ) );
    return s_proto;
}

std::pair<TarFileAccess_details::TarFile*, std::string>
TarFileAccess::resolve( const std::string& url )
{
    // parse tarfile:///foo/bar/xxx.tar/some/file
    //                                  ^       ^ file name inside tar file
    //                 ^              ^ tar name
    // VFSSvc dictates a :// as seperator between protocol and rest
    static boost::regex tar( "^[tT]ar[fF]ile://(.*.tar)/+(.*)$" );
    boost::smatch what;
    if ( !boost::regex_match( url, what, tar ) ) {
        error() << " could not parse url " << url << endmsg;
        // could not parse url, invalid url
        return make_pair( (TarFileAccess_details::TarFile*)0, url );
    }
    debug() << " resolved url into tarfile " << what.str( 1 ) << " and file "
            << what.str( 2 ) << endmsg;

    // check if tarname already known; if not, open and index
    container_t::iterator tarFile = m_tarFiles.find( what.str( 1 ) );
    if ( tarFile == m_tarFiles.end() ) {
        tarFile = m_tarFiles.insert(
            tarFile,
            std::make_pair( what.str( 1 ),
                            new TarFileAccess_details::TarFile( what.str( 1 ) ) ) );
        // TODO: keep access statistics on open files,
        // and if too many, close the oldest
        // also, maybe we want to move the most recently used
        // item to the front, and start to search from there...
        // then we can drop things at the back if we become
        // too long...
    }

    return make_pair( tarFile->second, what.str( 2 ) );
}

std::auto_ptr<std::istream> TarFileAccess::open( const std::string& url )
{
    std::pair<TarFileAccess_details::TarFile*, std::string> resolved =
        resolve( url );
    return std::auto_ptr<std::istream>(
        resolved.first != 0 ? resolved.first->open( resolved.second ) : 0 );
    // std::auto_ptr<std::istream> is( resolved.first!=0 ?
    // resolved.first->open(resolved.second) : 0 ) ;
    // if (is->get()==0) error() << "Failed to resolve url " << url << endmsg;
    // return is;
}

MsgStream& TarFileAccess::msg( MSG::Level level ) const
{
    if ( m_msg.get() == 0 ) m_msg.reset( new MsgStream( msgSvc(), name() ) );
    return *m_msg << level;
}

