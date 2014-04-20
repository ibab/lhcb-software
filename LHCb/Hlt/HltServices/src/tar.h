#ifndef TAR_IMPL_H
#define TAR_IMPL_H
#include "IArchive.h"
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
class TarFile : public IArchive, private boost::noncopyable
{
  public:
    TarFile( const std::string& name, std::ios::openmode mode = std::ios::in,
             bool compressOnWrite = true );
    bool operator!() const override
    {
        return !m_file;
    }
    bool writeable() const override
    {
        return m_lock >= 0;
    }
    bool setupStream( io::filtering_istream& s, const std::string& name ) const  override;


    bool exists( const std::string& path )
    {
        const auto& myIndex = getIndex();
        return myIndex.find( path ) != myIndex.end();
    }

    std::vector<std::string> contents() const override
    {
        std::vector<std::string> f;
        for ( auto& i : getIndex() ) f.push_back(i.first) ;
        return f;
    }
    bool append( const std::string& name, std::stringstream& is ) override;

    ~TarFile();

  private:
    struct Info
    {
        Info() : size( 0 ), offset( 0 ), compressed( false ), mtime(0),uid(0)
        { }
        std::string name;
        size_t size;
        size_t offset;
        bool compressed;
        time_t mtime;
        uid_t uid;
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
