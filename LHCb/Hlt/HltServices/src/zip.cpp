#include "zip.h"
#include <boost/crc.hpp>                             // for boost::crc_32_type
#include "boost/regex.hpp"
#ifndef _WIN32
#include "boost/iostreams/filter/bzip2.hpp"
#endif
#include "boost/iostreams/filter/zlib.hpp"
#include "boost/iostreams/slice.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/iostreams/device/back_inserter.hpp"
#include "boost/iostreams/copy.hpp"
#include "boost/date_time/posix_time/posix_time.hpp" // ZIP date/time
using namespace std;

namespace
{
template <typename T>
T get( istream& is );
template <typename T>
T get( const unsigned char* buf );

template <typename I, typename J>
I _get2( istream& is )
{
    I x( get<J>( is ) );
    x |= I( get<J>( is ) ) << ( 8 * sizeof( J ) );
    return x;
}
template <typename I, typename J>
I _get2( const unsigned char* buf )
{
    I x( get<J>( buf ) );
    x |= I( get<J>( buf + sizeof( J ) ) ) << 8 * sizeof( J );
    return x;
}
template <>
uint8_t get<uint8_t>( istream& is )
{
    uint8_t x;
    is.read( (char*)&x, sizeof( uint8_t ) );
    // TODO: if not OK, throw exception...
    return x;
}
template <>
uint8_t get<uint8_t>( const unsigned char* buf )
{
    return uint8_t( buf[0] );
}

template <>
uint16_t get<uint16_t>( istream& is )
{
    return _get2<uint16_t, uint8_t>( is );
}
template <>
uint32_t get<uint32_t>( istream& is )
{
    return _get2<uint32_t, uint16_t>( is );
}
template <>
uint64_t get<uint64_t>( istream& is )
{
    return _get2<uint64_t, uint32_t>( is );
}

template <>
uint16_t get<uint16_t>( const unsigned char* buf )
{
    return _get2<uint16_t, uint8_t>( buf );
}
template <>
uint32_t get<uint32_t>( const unsigned char* buf )
{
    return _get2<uint32_t, uint16_t>( buf );
}
template <typename T>
void put( ostream& os, T value );
template <>
void put<uint8_t>( ostream& os, uint8_t value )
{
    os.put( value & 0xff );
}
template <>
void put<uint16_t>( ostream& os, uint16_t value )
{
    os.put( value & 0xff );
    os.put( ( value >> 8 ) & 0xff );
}
template <>
void put<uint32_t>( ostream& os, uint32_t value )
{
    os.put( value & 0xff );
    os.put( ( value >> 8 ) & 0xff );
    os.put( ( value >> 16 ) & 0xff );
    os.put( ( value >> 24 ) & 0xff );
}
template <>
void put<uint64_t>( ostream& os, uint64_t value )
{
    os.put( value & 0xff );
    os.put( ( value >> 8 ) & 0xff );
    os.put( ( value >> 16 ) & 0xff );
    os.put( ( value >> 24 ) & 0xff );
    os.put( ( value >> 32 ) & 0xff );
    os.put( ( value >> 40 ) & 0xff );
    os.put( ( value >> 48 ) & 0xff );
    os.put( ( value >> 56 ) & 0xff );
}

using namespace boost::posix_time;
using namespace boost::gregorian;

ptime ZipToTime( uint16_t zip_date, uint16_t zip_time )
{
    return {date( 1980 + ( ( zip_date >> 9 ) & 0x3f ), ( ( zip_date >> 5 ) & 0xf ),
                  zip_date & 0x1f ),
            hours( ( zip_time >> 11 ) & 0x1f ) +
                minutes( ( zip_time >> 5 ) & 0x1f ) +
                seconds( ( ( zip_time & 0x1f ) << 1 ) )};
}
void TimeToZip( uint16_t& zip_date, uint16_t& zip_time )
{
    ptime pt = second_clock::local_time();
    date d = pt.date();
    time_duration t = pt.time_of_day();

    zip_date = ( ( d.year() - 1980 ) << 9 ) + ( d.month() << 5 ) + d.day();
    zip_time = ( t.hours() << 11 ) + ( t.minutes() << 5 ) + ( t.seconds() >> 1 );
}
}

namespace ConfigZipFileAccessSvc_details
{

bool ZipFile::setupStream( io::filtering_istream& s, const string& name ) const
{
    auto i = m_index.find( name );
    if ( i == m_index.end() ) return false;
    m_file.seekg( i->second.file_offset, ios::beg );
    if ( ::get<uint32_t>( m_file ) != 0x04034B50 ) return false;
    // AZ: original code read values from the file header, I think it is not
    // required since should be redundant in consistent file
    // In practice, it is not... at least for extra field.
    // i->second.print();
    m_file.seekg( i->second.file_offset + 28, ios::beg );
    uint16_t hdr_extra_length = ::get<uint16_t>( m_file );
    uint32_t offset =
        i->second.file_offset + 30 + i->second.name_length + hdr_extra_length;
    m_file.seekg( 0, ios::beg );
    switch ( i->second.compress ) {
    case 0:
        break;
    case 8: {
        io::zlib_params params;
        params.noheader = true;
        s.push( io::zlib_decompressor( params ) );
    } break;
#ifndef _WIN32
    case 12:
        s.push( io::bzip2_decompressor() );
        break;
#endif
    default:
        cerr << " unknown compression algorithm " << i->second.compress << endl;
        return false;
    }
    s.push( io::slice( m_file, offset, i->second.data_size ) );
    return true;
}

bool ZipFile::append( const string& name, stringstream& is )
{
    // AZ: for many reasons just a buffer is prefered
    string uncompressed;
    io::copy( is, io::back_inserter( uncompressed ), 8192 );

    ZipInfo info;
    info.make_from( name, m_erec.offset, uncompressed );

    // Decide either compress or not (brute force, just try and compare)
    string compressed;
    {
        io::filtering_streambuf<io::output> filter;
        io::zlib_params params;
        params.noheader = true;
        filter.push( io::zlib_compressor( params ) );
        filter.push( io::back_inserter( compressed ) );
        io::copy( boost::make_iterator_range( uncompressed ), filter, 8192 );
    }
    if ( uncompressed.size() > compressed.size() ) {
        info.data_size = compressed.size();
        info.compress = 8;
    }

    // info.print();

    m_file.seekp( info.file_offset, ios::beg );
    info.write_hdr( m_file );
    if ( uncompressed.size() > compressed.size() )
        m_file.write( compressed.c_str(), compressed.size() );
    else
        m_file.write( uncompressed.c_str(), uncompressed.size() );
    m_erec.offset = m_file.tellp();

    ++m_erec.entriesThisDisk;

    /* AZ: ZIP64 is disabled
    if(m_erec.offset > 0xfffffffe || m_erec.entriesThisDisk > 0xfffe)
      m_erec.zip64 = true;
    */

    m_index[info.name] = info;
    m_modified = true;
    return true;
}

void ZipFile::ZipInfo::print( void ) const
{
    cout << hex << vmade << ' ' << vneed << ' ' << flag << ' ' << compress << ' '
         << time << ' ' << date << " (" << ZipToTime( date, time ) << ") " << crc
         << ' ' << dec << data_size << ' ' << file_size << ' ' << name_length << ' '
         << extra_length << ' ' << comment_length << ' ' << disk << ' ' << hex
         << iattr << ' ' << eattr << ' ' << file_offset << ' ' << " '" << name
         << "' "
         << " '" << extra << "' "
         << " '" << comment << "' " << endl;
}

void ZipFile::ZipInfo::make_from( const string& a_name, uint32_t a_file_offset,
                                  const string& is )
{/* here 'is' is the body of the 'file' */
    vmade = 0x317;
    vneed = 0x14;
    flag = 0;
    TimeToZip( date, time );
    name_length = a_name.size();
    name = a_name;
    extra_length = comment_length = 0;
    disk = iattr = 0;
    eattr = 0x81240001; // Read for all
    file_offset = a_file_offset;

    boost::crc_32_type crc32;
    crc32.process_bytes( is.c_str(), is.size() );
    crc = crc32.checksum();
    file_size = is.size();

    data_size = file_size; // that should be set by caller if different
    compress = 0;
}
bool ZipFile::ZipInfo::read_from( istream& is )
{/* here 'is' is the ZIP file
    positioned to the start of
    the entry */
    if ( ::get<uint32_t>( is ) != 0x02014B50 ) return false;

    vmade = ::get<uint16_t>( is );
    vneed = ::get<uint16_t>( is );
    flag = ::get<uint16_t>( is );
    compress = ::get<uint16_t>( is );
    time = ::get<uint16_t>( is );
    date = ::get<uint16_t>( is );
    crc = ::get<uint32_t>( is );
    data_size = ::get<uint32_t>( is );
    file_size = ::get<uint32_t>( is );
    name_length = ::get<uint16_t>( is );
    extra_length = ::get<uint16_t>( is );
    comment_length = ::get<uint16_t>( is );
    disk = ::get<uint16_t>( is );
    iattr = ::get<uint16_t>( is );
    eattr = ::get<uint32_t>( is );
    file_offset = ::get<uint32_t>( is );

    io::copy( io::slice( is, 0, name_length ), io::back_inserter( name ) );
    io::copy( io::slice( is, 0, extra_length ), io::back_inserter( extra ) );
    io::copy( io::slice( is, 0, comment_length ), io::back_inserter( comment ) );
    return true;
}
bool ZipFile::ZipInfo::write_cd( ostream& os ) const
{
    put<uint32_t>( os, 0x02014B50 );
    put<uint16_t>( os, vmade );
    put<uint16_t>( os, vneed );
    put<uint16_t>( os, flag );
    put<uint16_t>( os, compress );
    put<uint16_t>( os, time );
    put<uint16_t>( os, date );
    put<uint32_t>( os, crc );
    put<uint32_t>( os, data_size );
    put<uint32_t>( os, file_size );
    put<uint16_t>( os, name_length );
    put<uint16_t>( os, extra_length );
    put<uint16_t>( os, comment_length );
    put<uint16_t>( os, disk );
    put<uint16_t>( os, iattr );
    put<uint32_t>( os, eattr );
    put<uint32_t>( os, file_offset );

    os.write( name.c_str(), name.size() );
    os.write( extra.c_str(), extra.size() );
    return true;
}

bool ZipFile::ZipInfo::write_hdr( ostream& os ) const
{
    put<uint32_t>( os, 0x04034b50 );
    put<uint16_t>( os, vneed );
    put<uint16_t>( os, flag );
    put<uint16_t>( os, compress );
    put<uint16_t>( os, time );
    put<uint16_t>( os, date );
    put<uint32_t>( os, crc );
    put<uint32_t>( os, data_size );
    put<uint32_t>( os, file_size );
    put<uint16_t>( os, name_length );
    put<uint16_t>( os, extra_length );
    os.write( name.c_str(), name.size() );
    os.write( extra.c_str(), extra.size() );
    return true;
}

bool ZipFile::EndRec::write_zip64_ecd( ostream& os )
{
    put<uint32_t>( os, 0x06064b50 );
    put<uint64_t>( os, 56 );
    put<uint16_t>( os, vmade );
    put<uint16_t>( os, vneed );
    put<uint32_t>( os, 0 );
    put<uint32_t>( os, 0 );
    put<uint64_t>( os, entriesThisDisk );
    put<uint64_t>( os, size );
    put<uint64_t>( os, offset );
    return true;
}

bool ZipFile::EndRec::write_zip64_cdl( ostream& os )
{
    put<uint32_t>( os, 0x07064b50 );
    put<uint32_t>( os, 0 );
    put<uint64_t>( os, zip64_offset );
    put<uint32_t>( os, 1 );
    return true;
}

bool ZipFile::EndRec::write_ecd( ostream& os )
{
    zip64_offset = os.tellp();
    put<uint32_t>( os, 0x06054b50 );
    put<uint16_t>( os, 0 );
    put<uint16_t>( os, 0 );
    if ( entriesThisDisk > 0xfffe && zip64 ) {
        put<uint16_t>( os, 0xffff );
        put<uint16_t>( os, 0xffff );
    } else {
        put<uint16_t>( os, uint16_t( entriesThisDisk ) );
        put<uint16_t>( os, uint16_t( entriesThisDisk ) );
    }
    put<uint32_t>( os, (   size > 0xfffffffe && zip64 ) ? 0xffffffff :   size );
    put<uint32_t>( os, ( offset > 0xfffffffe && zip64 ) ? 0xffffffff : offset );
    put<uint16_t>( os, comment_length );
    os.write( comment.c_str(), comment.size() );
    return true;
}

bool ZipFile::EndRec::write_to( ostream& os )
{
    bool result = true;
    if ( zip64 ) result &= write_zip64_ecd( os ) & write_zip64_cdl( os );
    return result & write_ecd( os );
}

bool ZipFile::EndRec::read_zip64_ecd( istream& is )
{
    is.seekg( zip64_offset, ios::beg );
    if ( ::get<uint32_t>( is ) != 0x06064b50 ) {
        cerr << "ZIP: ZIP64 ECD is not found: not a zip file??" << endl;
        return false;
    }
    uint64_t ecd_size = ::get<uint64_t>( is );
    vmade = ::get<uint16_t>( is );
    vneed = ::get<uint16_t>( is );
    uint32_t ecdDisk = ::get<uint32_t>( is );
    uint32_t cdDisk = ::get<uint32_t>( is );
    entriesThisDisk = ::get<uint64_t>( is );
    uint64_t entriesTotal = ::get<uint64_t>( is );
    size = ::get<uint64_t>( is );
    offset = ::get<uint64_t>( is );

    if ( ecdDisk != 0 || cdDisk != 0 ) {
        cerr << "ZIP: multifile archives are not supported" << endl;
        return false;
    }
    if ( ecd_size != 56 ) {
        cerr << "ZIP: unsupported ZIP64 ECD size" << endl;
        return false;
    }
    if ( entriesTotal != entriesThisDisk ) {
        cerr << "ZIP: entries counter mismatch" << endl;
        return false;
    }
    return true;
}

bool ZipFile::EndRec::read_zip64_cdl( istream& is, ios::streampos cdl_offset )
{
    is.seekg( cdl_offset, ios::beg );
    if ( ::get<uint32_t>( is ) != 0x07064b50 ) {
        cerr << "ZIP: ZIP64 CDL is not found: not a zip file??" << endl;
        return false;
    }
    uint32_t diskStart = ::get<uint32_t>( is );
    zip64_offset = ::get<uint64_t>( is );
    uint32_t diskTotal = ::get<uint32_t>( is );

    if ( diskStart != 0 || diskTotal != 1 ) {
        cerr << "ZIP: multifile archives are not supported" << endl;
        return false;
    }
    return true;
}

bool ZipFile::EndRec::read_from( istream& is )
{
    // first try for a ZIP file without archive comment
    // in which case the 'end of central directory' structure
    // is the last thing in the file.
    // Then  check whether there is an 'archive comment' at the end of this
    // file...
    // The comment is up to 64K in size, and we assume it does NOT contain
    // the signature of an 'ecd' record...

    is.seekg( 0, ios::end );
    ios::streamoff filesize = is.tellg();
    ios::streampos er_offset;

    for ( er_offset = filesize - streamsize( sizeEndCentDir );
          er_offset >=
              max( filesize - streamsize( 0xffff ) - streamsize( sizeEndCentDir ),
                   ios::streamoff( 0 ) );
          er_offset -= 1 ) {
        is.seekg( er_offset, ios::beg );
        if ( ::get<uint8_t>( is ) == 0x50 && ::get<uint8_t>( is ) == 0x4b &&
             ::get<uint8_t>( is ) == 0x05 && ::get<uint8_t>( is ) == 0x06 ) {
            break;
        }
    }
    is.seekg( er_offset, ios::beg );
    if ( ::get<uint32_t>( is ) != 0x06054b50 ) {
        cerr << "ZIP: CDE is not found: not a zip file??" << endl;
        return false;
    }
    uint16_t diskNumber = ::get<uint16_t>( is );
    uint16_t diskStart = ::get<uint16_t>( is );
    entriesThisDisk = ::get<uint16_t>( is );
    uint16_t entriesTotal = ::get<uint16_t>( is );
    size = ::get<uint32_t>( is );
    offset = ::get<uint32_t>( is );
    comment_length = ::get<uint16_t>( is );
    io::copy( io::slice( is, 0, comment_length ), io::back_inserter( comment ) );

    if ( diskNumber != 0 || diskStart != 0 ) {
        cerr << "ZIP: multifile archives are not supported" << endl;
        return false;
    }

    if ( entriesTotal != entriesThisDisk ) {
        cerr << "ZIP: entries counter mismatch" << endl;
        return false;
    }

    /* AZ: ZIP64 is disabled
    if(entriesThisDisk==0xffff || size==0xffffffff || offset==0xffffffff){
      zip64 = true;
      return read_zip64_cdl(is,er_offset-streamsize(sizeEndCentDirLoc)) &
    read_zip64_ecd(is);
    }
    */
    return true;
}
}
