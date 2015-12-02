#include "tar.h"
#include <sstream>
#include <set>
#include "boost/iostreams/copy.hpp"
#include "boost/iostreams/filter/gzip.hpp"
#include "boost/iostreams/slice.hpp"
#include "boost/iostreams/operations.hpp"
#include "boost/iostreams/seek.hpp"
#include "boost/algorithm/string/trim.hpp"

namespace
{
class CleanupAtExit
{
  public:
    static CleanupAtExit& instance()
    {
        static CleanupAtExit theOne;
        return theOne;
    }
    void add( std::string name )
    {
        m_files.emplace( std::move(name) );
    }
    void remove( const std::string& name )
    {
        m_files.erase( name );
    }

  private:
    CleanupAtExit() = default;
    ~CleanupAtExit()
    {
        for ( const auto& i : m_files ) {
            std::cerr << "ConfigTarFileAccessSvc::CleanupAtExit: removing " << i
                      << std::endl;
            unlink( i.c_str() );
            std::cerr << "ConfigTarFileAccessSvc::CleanupAtExit: removed " << i
                      << std::endl;
        }
    }
    std::set<std::string> m_files;
};

bool isOctal( const char& ch )
{
    return ( ( ch >= '0' ) && ( ch <= '7' ) );
}
long getOctal( const char* cp, int size )
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

long computeChecksum( const ConfigTarFileAccessSvc_details::posix_header& header )
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

template <typename T>
std::string convert( const T& in )
{
    // take into account that eg. name and prefix do not have a
    // terminating '\0' in case they occupy the full length...
    std::string s( in, sizeof( in ) );
    std::string::size_type last = s.find( char( 0 ) );
    return ( last == std::string::npos ) ? s : s.substr( 0, last );
}

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


/* A nice enum with all the possible tar file content types */
enum TarFileType {
    REGTYPE = '0',   /* regular file */
    REGTYPE0 = '\0', /* regular file (ancient bug compat)*/
    GNUTYPE_LONGLINK = 'K',
    GNUTYPE_LONGNAME = 'L'
};
typedef enum TarFileType TarFileType;
}

namespace ConfigTarFileAccessSvc_details
{
bool interpretHeader(std::fstream& file,  ConfigTarFileAccessSvc_details::posix_header& header, Info& info )
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

    if ( TarFileType( header.typeflag ) == TarFileType::GNUTYPE_LONGNAME ) {
        // current header is a dummy 'flag', followed by data blocks
        // with name as content (length of name is 'size' of data)
        assert( info.name == "././@LongLink" );
        // first read the real, untruncated name as data
        std::ostringstream fname;
        io::copy( io::slice( file, 0, info.size ), fname );
        size_t padding = info.size % 512;
        if ( padding != 0 ) file.seekg( 512 - padding, std::ios::cur );
        // and now get another header, which contains a truncated name
        // but which is otherwise the 'real' one
        file.read( (char*)&header, sizeof( header ) );
        if ( !interpretHeader( file, header, info ) ) return false;
        // so we overwrite the truncated one with the long one...
        info.name = fname.str();
        // removing trailing zero(s)
        boost::trim_right_if(info.name,[](const char& c) { return c==0; });
    }
    info.uid    = getOctal( header.uid, sizeof(header.uid) );
    info.mtime  = getOctal( header.mtime, sizeof(header.mtime) );
    info.offset = file.tellg(); // this goes here, as the longlink handling
                                  // reads an extra block...
    // if name ends in .gz, assume it is gzipped.
    // Strip the name down, and flag as compressed in info.
    info.compressed = ( info.name.size() > 3 &&
                        info.name.compare( info.name.size() - 3, 3, ".gz" ) == 0 );
    if ( info.compressed ) info.name = info.name.substr( 0, info.name.size() - 3 );

    return true;
}

bool TarFile::append( const std::string& name, std::stringstream& is )
{
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
    return _append( name, is );
}

bool TarFile::_append( const std::string& name, std::stringstream& is )
{
    if ( m_lock < 0 ) return false;
    m_indexUpToDate = false;
    std::string bstring = is.str();
    size_t size = bstring.size();
    const char* buffer = bstring.c_str();
    if ( m_file.tellp() >= 0 && m_leof != m_file.tellp() ) {
        m_file.seekp( m_leof, std::ios::beg ); // where do we start writing?
    }
    ConfigTarFileAccessSvc_details::posix_header header;
    memset( &header, 0, sizeof( header ) );
    if ( name.size() > sizeof( header.name ) ) {
        // generate GNU longlink header...
        putString( "././@LongLink", header.name );
        header.typeflag = TarFileType::GNUTYPE_LONGNAME;
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
    putOctal( S_IFREG | S_IRUSR | S_IRGRP | S_IROTH, header.mode );
    putString( "ustar ", header.magic );
    putString( "00", header.version );
    putOctal( getUid(), header.uid );
    putOctal( getGid(), header.gid );
    putOctal( size, header.size );
    putOctal( time( nullptr ), header.mtime );
    header.typeflag = TarFileType::REGTYPE;
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
    if ( size % 512 ) m_file.write( zeros, 512 - size % 512 );
    // update the logical EOF
    std::streamoff leof = m_file.tellp();
    // write end-of-file marker : 2 zero blocks, padded up to the blocking factor
    m_file.write( zeros, 2 * 512 );
    if ( m_leof % ( blockingFactor * 512 ) ) {
        m_file.write( zeros,
                      blockingFactor * 512 - m_leof % ( blockingFactor * 512 ) );
    }
    m_file.flush();
    m_indexUpToDate = index( m_leof );
    if ( m_leof != leof ) {
        std::cerr << "oops: read-back eof not what was predicted: " << m_leof
                  << " vs. " << leof << std::endl;
        return false;
    }
    return true;
}


bool TarFile::index( std::streamoff offset ) const
{
    ConfigTarFileAccessSvc_details::posix_header header;
    if ( offset == 0 ) m_index.clear();
    // check whether file is empty
    m_file.seekg( offset, std::ios::end );
    if ( m_file.tellg() == 0 ) return true;
    // otherwise, start at the begin...
    m_file.seekg( offset, std::ios::beg );
    while ( m_file.read( (char*)&header, sizeof( header ) ) ) {
        Info info;
        if ( !interpretHeader(m_file, header, info ) ) {
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
            std::cerr << "failed to interpret header preceeding " << m_file.tellg()
                      << " in tarfile " << m_name << std::endl;
            m_file.seekg( 0, std::ios::beg );
            return false;
        }
        if ( info.name.empty() ) {
            std::cerr << " got empty name " << std::endl;
            break;
        }
        if ( ( TarFileType( header.typeflag ) == TarFileType::REGTYPE ||
               TarFileType( header.typeflag ) == TarFileType::REGTYPE0 ) &&
             info.name[info.name.size() - 1] != '/' &&
             info.name.find( "/CVS/" ) == std::string::npos &&
             info.name.find( "/.svn/" ) == std::string::npos ) {
            // TODO: check for duplicates!!
            auto r = m_index.emplace( info.name, info );
            if (!r.second) {
                 std::cerr << "error inserting " << info.name << " into index!!! " << std::endl;
            }
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

bool TarFile::setupStream( io::filtering_istream& s, const std::string& name ) const
{
    const auto& myIndex = getIndex();
    auto i = myIndex.find( name );
    if ( i == myIndex.end() ) return false;
    // slice works relative to the current file offset, as it works on an
    // istream...
    m_file.seekg( 0, std::ios_base::beg );
    if ( i->second.compressed ) {
#ifdef __INTEL_COMPILER          // Disable ICC warning
#pragma warning( disable : 279 ) // gzip.hpp(551) controlling expression constant
#pragma warning( push )
#endif
        s.push( io::gzip_decompressor() );
#ifdef __INTEL_COMPILER // Re-enable ICC warning 279
#pragma warning( pop )
#endif
    }
    s.push( io::slice( m_file, i->second.offset, i->second.size ) );
    return true;
}
TarFile::TarFile( const std::string& name, std::ios::openmode mode,
                  bool compressOnWrite )
    : m_name( name )
    , m_lock( -1 )
    , m_leof( 0 )
    , m_indexUpToDate( false )
    , m_myUid( 0 )
    , m_myGid( 0 )
    , m_compressOnWrite( compressOnWrite )
{
    if ( mode & std::ios::out ) {
        std::string lckname = m_name + ".lock";
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
        //
        m_lock =
            open( lckname.c_str(), O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR );
        if ( m_lock < 0 ) {
            std::cerr << "trying to open  " << m_name
                      << " for writing, but lockfile " << lckname
                      << " already exists." << std::endl;
            std::cerr << "This (most likely) means that some other process intends "
                         "to write into " << m_name << std::endl;
            std::cerr << "Refusing to continue to preserve integrity of " << m_name
                      << "... goodbye" << std::endl;
            ::abort();
        }
        std::cerr << "succesfully created lock file " << lckname << std::endl;
        CleanupAtExit::instance().add( lckname );
// TODO: write some text to the lockfile to identify this process: machine name,
// pid...
//       which can be used in the above failure case to point out the source of the
// clash...
    }
    m_file.open( m_name.c_str(), mode | std::ios::in | std::ios::binary );
}
TarFile::~TarFile()
{
    m_file.close();
    if ( !( m_lock < 0 ) ) {
        std::cerr << "releasing lock " << std::endl;
        close( m_lock );
        unlink( ( m_name + ".lock" ).c_str() );
        std::cerr << "removed lock file " << ( m_name + ".lock" ) << std::endl;
        CleanupAtExit::instance().remove( m_name + ".lock" );
    }
}
}
