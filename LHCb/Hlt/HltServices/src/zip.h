/*
 * 19xx...2012 - GR: original design and the code
 * 9.2012 - AZ: write access, spec from
 *        http://web.archive.org/web/20011203085830/http://www.pkware.com/support/appnote.txt
 *        This implementation partially support the specification, with the following
 * limitations:
 *            1) only one file archives are supported
 *            2) separate files should be less than 4G in size (ZIP64 extra field is
 * not parsed)
 *            3) zip64 extensible data sector is not supported
 *            4) archives with more than 4G in total are not tested
 *        At the moment, I have DISABLED ZIP64 mode
 *        Directories should have specific attributes on creation, so not supported.
 */


#include <map>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include "boost/optional.hpp"
#include "boost/iostreams/slice.hpp"
#include "boost/iostreams/copy.hpp"
#include "boost/iostreams/filter/zlib.hpp"
#ifndef _WIN32
#include "boost/iostreams/filter/bzip2.hpp"
#endif
#include "boost/iostreams/filtering_stream.hpp"
#include "boost/iostreams/device/back_inserter.hpp"
#include "boost/regex.hpp"
#include "boost/assign/list_of.hpp"
#include "boost/filesystem/path.hpp"

#include "boost/integer_traits.hpp"
using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

#include "boost/date_time/posix_time/posix_time.hpp" // ZIP date/time
#include <boost/crc.hpp>                             // for boost::crc_32_type

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/StringKey.h"

using namespace std;
namespace io = boost::iostreams;
namespace fs = boost::filesystem;

namespace
{

constexpr struct {
    template <typename T> bool operator()( const T& ) const { return true; }
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


}

namespace ConfigZipFileAccessSvc_details
{

class ZipFile : boost::noncopyable
{
  public:
    ZipFile( const string& name, ios::openmode mode = ios::in )
        : m_erec()
        , m_mode( mode )
        , m_modified( false )
    {
        m_file.open(  name.c_str(), mode | ios::in | ios::binary );
        if ( !m_file && ( mode & ios::out ) ) { // try to create it
            m_file.close();
            m_file.clear();
            m_file.open( name.c_str(), ios::out | ios::binary );
            m_file.close();
            m_file.open( name.c_str(), mode | ios::in | ios::binary );
            if ( !m_file ) cerr << "Can not create the file " << name << endl;
        } else
            index();
    }
    ~ZipFile()
    {
        flush();
    }

    bool operator!() const
    {
        return !m_file;
    }
    bool writeable() const
    {
        return m_mode & ios::out;
    }
    vector<string> files() const
    {
        return files( all );
    }

    bool append( const string& name, const stringstream& is );

    template <typename SELECTOR>
    vector<string> files( const SELECTOR& selector ) const
    {
        vector<string> f;
        for ( const auto& i : m_index ) {
            if ( selector( i.first ) ) f.push_back( i.first );
        }
        return f;
    }

    bool setupStream( io::filtering_istream& s, const string& name );

    template <typename T>
    boost::optional<T> get( const std::string& name )
    {
        io::filtering_istream strm;
        if (!setupStream( strm, name ) ) return boost::optional<T>();
        T t;
        strm >> t;
        return !strm.fail() ? t : boost::optional<T>() ;
    }

  private:
    struct ZipInfo
    {
        /* uint32_t sig = 0x02014b50 */

        uint16_t vmade;          /* Version made by  (0x317) */
        uint16_t vneed;          /* Version need to extract (0x14) */
        uint16_t flag;           /* Bit flag (0x0) */
        uint16_t compress;       /* Compression method (0, 8 or 12) */
        uint16_t time;           /* File last mod time */
        uint16_t date;           /* File last mod date */
        uint32_t crc;            /* CRC-32 (of uncompressed file) */
        uint32_t data_size;      /* Compressed size */
        uint32_t file_size;      /* Uncompressed size */
        uint16_t name_length;    /* File name length */
        uint16_t extra_length;   /* Extra field length */
        uint16_t comment_length; /* Comment length */
        uint16_t disk;           /* Disk where file starts (0) */
        uint16_t iattr;          /* Internal file attributes (1 or 0 ) */
        uint32_t eattr;          /* External file attributes (0x81a40000 or 0) */
        uint32_t file_offset;    /* Relative offset of local file header */

        string name;
        string extra;
        string comment;

        void make_from( const string& a_name, uint32_t a_file_offset,
                        const string& is );

        bool read_from( istream& is );

        bool write_cd( ostream& os ) const;

        bool write_hdr( ostream& os ) const;
        void print( void ) const;

    };

    struct EndRec
    {
        enum {
            sizeEndCentDir = 22,
            sizeEndCentDirLoc = 160
        };

        bool zip64; /* Set if we detect such a need */

        /* End of central directory record */
        /* uint32_t sig = 0x06054b50 */
        /* uint16_t diskNumber === 0 */
        /* uint16_t diskStart === 0 */
        /* uint16_t entriesThisDisk ; lower part from ZIP64 ECD or 0xffff */
        /* uint16_t entriesTotal === entriesThisDisk */
        /* uint32_t size;   lower part from ZIP64 ECD or 0xffffffff */
        /* uint32_t offset; lower part from ZIP64 ECD or 0xffffffff */
        uint16_t comment_length;
        string comment;

        /* Zip64 end of central directory locator */
        /* uint32_t zip64_loc_sig = 0x07064b50 */
        /* uint32_t zip64_diskStart === 0 */
        uint64_t zip64_offset; // zip64 end of the central directory
        /* uint32_t disks_total === 0 */

        /* Zip64 end of central directory record */
        /* uint32_t zip64_ecd_sig = 0x06064b50 */
        /* uint64_t zip64_ecd_size === 56 */
        uint16_t vmade; /* Version made by  (0x317) */
        uint16_t vneed; /* Version need to extract (0x14 for not ZIP64) */
        /* uint32_t zip64_ecd_disk === 0 */
        /* uint32_t zip64_cd_disk === 0 */
        uint64_t entriesThisDisk;
        /* uint64_t entriesTotal === entriesThisDisk */
        uint64_t size;   // size of the central directory
        uint64_t offset; // offset of the central directory
        /* ... zip64_extra === "" */

        void print() const
        {
            cout << dec << entriesThisDisk << ' ' << size << ' ' << offset << ' '
                 << comment_length << " '" << comment << "' " << endl;
        }

        EndRec()
            : zip64( false )
            , comment_length( 0 )
            , comment( "" )
            , zip64_offset( 0 )
            , vmade( 0x317 )
            , vneed( 0x14 )
            , entriesThisDisk( 0 )
            , size( 0 )
            , offset( 0 )
        {
        }

        bool write_zip64_ecd( ostream& os );
        bool write_zip64_cdl( ostream& os );
        bool write_ecd( ostream& os );
        bool write_to( ostream& os );
        bool read_zip64_ecd( istream& is );
        bool read_zip64_cdl( istream& is, ios::streampos cdl_offset );
        bool read_from( istream& is );
    };

    void flush()
    {
        if ( !m_modified ) return;

        m_file.seekp( m_erec.offset, ios::beg );
        for ( const auto& i : m_index ) i.second.write_cd( m_file );
        m_erec.size = m_file.tellp() - streamoff( m_erec.offset );
        m_erec.write_to( m_file );
        m_modified = false;
    }

    void index()
    {
        if ( !m_erec.read_from( m_file ) ) return;

        // m_erec.print();
        m_file.seekg( m_erec.offset, ios::beg );

        for ( ;; ) {
            ZipInfo info;
            if ( !info.read_from( m_file ) ) break;
            // info.print();
            m_index[info.name] = info;
        }
    }
    fstream m_file;
    map<Gaudi::StringKey, ZipInfo> m_index;

    EndRec m_erec;
    ios::openmode m_mode;
    bool m_modified;
};
}

