#ifndef TAR_IMPL_H
#define TAR_IMPL_H
#ifndef _WIN32
#include <unistd.h>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#else
typedef int uid_t;
typedef int gid_t;
#endif

#include "boost/optional.hpp"
#include "boost/iostreams/filtering_stream.hpp"
namespace io = boost::iostreams;

#include "GaudiKernel/StringKey.h"
#include "GaudiKernel/GaudiException.h"

namespace
{

constexpr struct
{
    template <typename T>
    bool operator()( const T& ) const
    {
        return true;
    }
} all;

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
}

namespace ConfigTarFileAccessSvc_details
{
class TarFile
{
  public:
    TarFile( const std::string& name, std::ios::openmode mode = std::ios::in,
             bool compressOnWrite = true );
    bool operator!() const
    {
        return !m_file;
    }
    bool writeable() const
    {
        return m_lock >= 0;
    }
    bool setupStream( io::filtering_istream& s, const std::string& name );

    template <typename T>
    boost::optional<T> get( const std::string& name )
    {
        io::filtering_istream strm;
        if ( !setupStream( strm, name ) ) return boost::optional<T>();
        T t;
        strm >> t;
        return !strm.fail() ? t : boost::optional<T>();
    }

    bool exists( const std::string& path )
    {
        const auto& myIndex = getIndex();
        return myIndex.find( path ) != myIndex.end();
    }
    template <typename SELECTOR>
    std::vector<std::string> files( const SELECTOR& selector = all ) const
    {
        std::vector<std::string> f;
        for ( auto& i : getIndex() ) {
            if ( selector( i.first ) ) f.push_back( i.first );
        }
        return f;
    }
    std::vector<std::string> files() const
    {
        return files( all );
    }
    bool append( const std::string& name, std::stringstream& is );

    ~TarFile();

  private:
    struct Info
    {
        /* A nice enum with all the possible tar file content types */
        enum TarFileType {
            REGTYPE = '0',   /* regular file */
            REGTYPE0 = '\0', /* regular file (ancient bug compat)*/
            GNUTYPE_LONGLINK = 'K',
            GNUTYPE_LONGNAME = 'L'
        };
        typedef enum TarFileType TarFileType;

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

    bool _append( const std::string& name, std::stringstream& is );
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
    bool interpretHeader( posix_header& header, struct Info& info ) const;
    // Read an octal value in a field of the specified width, with optional
    // spaces on both sides of the number and with an optional null character
    // at the end.  Returns -1 on an illegal format.
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
    mutable std::fstream m_file;
    mutable int m_lock;
    mutable std::map<Gaudi::StringKey, Info> m_index;
    mutable long m_leof;
    mutable bool m_indexUpToDate;
    mutable std::string m_gname;
    mutable std::string m_uname;
    mutable uid_t m_myUid;
    mutable gid_t m_myGid;
    bool m_compressOnWrite;
};
}
#endif
