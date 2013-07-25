/*
 * 19xx...2012 - GR: original design and the code
 * 9.2012 - AZ: write access, spec from
 *        http://web.archive.org/web/20011203085830/http://www.pkware.com/support/appnote.txt
 *        This implementation partially support the specification, with the following limitations:
 *            1) only one file archives are supported
 *            2) separate files should be less than 4G in size (ZIP64 extra field is not parsed)
 *            3) zip64 extensible data sector is not supported
 *            4) archives with more than 4G in total are not tested
 *        At the moment, I have DISABLED ZIP64 mode
 *        Directories should have specific attributes on creation, so not supported.
 */

#include "ConfigZipFileAccessSvc.h"

#include <map>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
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

#include <boost/crc.hpp>  // for boost::crc_32_type


#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/StringKey.h"

using namespace std;
namespace io = boost::iostreams;
namespace fs = boost::filesystem;

namespace {

    struct DefaultFilenameSelector {
      bool operator()(const string& /*fname*/) const { return true; }
    };

    struct PrefixFilenameSelector {
      PrefixFilenameSelector( const string& _prefix) : prefix(_prefix) {}
      bool operator()(const string& fname) const { return fname.compare(0,prefix.size(),prefix)==0; }
      string prefix;
    };

    template <typename T> T get(istream& is);
    template <typename T> T get(const unsigned char* buf);

    template <typename I, typename J> I _get2(istream& is) {
         I x( get<J>(is) ); x |= I( get<J>(is) )<<(8*sizeof(J)); return x;
    }
    template <typename I, typename J> I _get2(const unsigned char* buf) { 
        I x( get<J>(buf) ) ; x |= I(get<J>(buf+sizeof(J)))<<8*sizeof(J) ; return x;
    }
    template <> uint8_t get<uint8_t>(istream& is) {
        uint8_t x; is.read((char*)&x,sizeof(uint8_t));
        // TODO: if not OK, throw exception...
        return x;
    }
    template <>  uint8_t get< uint8_t>(const unsigned char* buf) { return uint8_t( buf[0] ); }

    template <> uint16_t get<uint16_t>(istream& is) { return _get2<uint16_t, uint8_t>(is); }
    template <> uint32_t get<uint32_t>(istream& is) { return _get2<uint32_t,uint16_t>(is); }
    template <> uint64_t get<uint64_t>(istream& is) { return _get2<uint64_t,uint32_t>(is); }
        
    template <> uint16_t get<uint16_t>(const unsigned char* buf) { return _get2<uint16_t,uint8_t>(buf); }
    template <> uint32_t get<uint32_t>(const unsigned char* buf) { return _get2<uint32_t,uint16_t>(buf); }

    template <typename T> void put(ostream &os, T value);
    template <> void put<uint8_t>(ostream &os, uint8_t value) { os.put(value&0xff); }
    template <> void put<uint16_t>(ostream &os, uint16_t value) { os.put(value&0xff); os.put((value>>8)&0xff); }
    template <> void put<uint32_t>(ostream &os, uint32_t value) { 
      os.put(value&0xff); os.put((value>>8)&0xff); os.put((value>>16)&0xff); os.put((value>>24)&0xff);
    }
    template <> void put<uint64_t>(ostream &os, uint64_t value) { 
      os.put(value&0xff); os.put((value>>8)&0xff); os.put((value>>16)&0xff); os.put((value>>24)&0xff);
      os.put((value>>32)&0xff); os.put((value>>40)&0xff); os.put((value>>48)&0xff); os.put((value>>56)&0xff);
    }

    using namespace boost::posix_time;
    using namespace boost::gregorian;

    void TimeToZip(uint16_t &zip_date, uint16_t &zip_time){
      ptime pt = second_clock::local_time();
      date  d = pt.date();
      time_duration t = pt.time_of_day();

      zip_date = ((d.year()-1980)<<9)+(d.month()<<5)+d.day();
      zip_time = (t.hours()<<11)+(t.minutes()<<5)+(t.seconds()>>1);
    }

    ptime ZipToTime(uint16_t zip_date, uint16_t zip_time){
      ptime t(date(1980+((zip_date>>9)&0x3f),
		   ((zip_date>>5)&0xf),zip_date&0x1f),
	      hours((zip_time>>11)&0x1f)+
	      minutes((zip_time>>5)&0x1f)+
	      seconds(((zip_time&0x1f)<<1))
	      );
      return t;
    }

}

namespace ConfigZipFileAccessSvc_details {

    class ZipFile  : boost::noncopyable {
    public:

      ZipFile(const string& name, ios::openmode mode = ios::in) 
        : m_name( name )
        , m_file( new fstream( name.c_str(), mode | ios::in | ios::binary ) )
	, m_erec( )
	, modified( false )
      {
	if(!m_file->good() && (mode & ios::out)){ // try to create it
	  m_file->close();
	  m_file->clear();
	  m_file->open(name.c_str(),ios::out | ios::binary);
	  m_file->close();
	  m_file->open(name.c_str(), mode | ios::in | ios::binary);
	  if(!m_file->good())
	    cerr << "Can not create the file " << name << endl;
	} else
	  index();
      } 
      ~ZipFile() { 
	flush();
      }

      bool good() const { return m_file->good(); }
      vector<string> files() const { return files(DefaultFilenameSelector()); }

      bool append(const string& name, const std::stringstream& is) {
	// AZ: for many reasons just a buffer is prefered 
	string uncompressed;
	io::copy(is,io::back_inserter(uncompressed),8192);

	ZipInfo info;
	info.make_from(name, m_erec.offset, uncompressed);

	// Decide either compress or not (brute force, just try and compare)
	string compressed;
	{
	  io::filtering_streambuf<io::output> filter;
	  io::zlib_params params; 
	  params.noheader = true;
	  filter.push(io::zlib_compressor(params));
	  filter.push(io::back_inserter(compressed));
	  io::copy(boost::make_iterator_range(uncompressed), filter, 8192);
	}
	if( uncompressed.size() >  compressed.size()) {
	  info.data_size = compressed.size();
	  info.compress = 8;
	}

	// info.print();

	m_file->seekp( info.file_offset, ios::beg );
	info.write_hdr(*m_file);
	if( uncompressed.size() >  compressed.size())
	  m_file->write(compressed.c_str(),compressed.size());
	else
	  m_file->write(uncompressed.c_str(),uncompressed.size());
	m_erec.offset = m_file->tellp();

	++m_erec.entriesThisDisk;
	
	/* AZ: ZIP64 is disabled
	if(m_erec.offset > 0xfffffffe || m_erec.entriesThisDisk > 0xfffe)
	  m_erec.zip64 = true;
	*/

	m_index[ info.name ] = info;
	modified = true;
	return true;
      }

      template <typename SELECTOR> vector<string> files(const SELECTOR& selector) const {
          vector<string> f;
          for (map<Gaudi::StringKey,ZipInfo>::const_iterator i = m_index.begin(); i!= m_index.end();++i) {
            if( selector(i->first) ) f.push_back(i->first);
          }
          return f;
      }

      istream* open(const string& fname) {
            map<Gaudi::StringKey,ZipInfo>::const_iterator i = m_index.find(fname);
            if ( i == m_index.end() ) 
	      return 0;

            m_file->seekg( i->second.file_offset,ios::beg);
            if (get<uint32_t>(*m_file)!=0x04034B50)  // That is just a safety test, can be removed for speed
	      return 0;
	    
	    // AZ: original code read values from the file header, I think it is not required since should be redundant in consistent file
	    // In practice, it is not... at least for extra field.	    
	    // i->second.print();
            m_file->seekg( i->second.file_offset+28,ios::beg);
	    uint16_t hdr_extra_length = get<uint16_t>(*m_file);
	    uint32_t offset = i->second.file_offset+30+i->second.name_length+hdr_extra_length;
            m_file->seekg( 0, ios::beg );
            std::auto_ptr<io::filtering_istream> s(new io::filtering_istream());
            switch( i->second.compress ) {
                case 0: 
                    break;
                case 8:
                    { io::zlib_params params; params.noheader = true;
                      s->push(io::zlib_decompressor(params));
                    }
                    break;
#ifndef _WIN32
                case 12 : 
                    s->push(io::bzip2_decompressor());
                    break;
#endif
                default:
                    cerr << " unknown compression algorithm " << i->second.compress << endl;
                    return 0;
            }
            s->push(io::slice(*m_file,offset,i->second.data_size));
            //TODO: we should keep track of outstanding open streams, by using ios_base::register_callback...
            return s.release();
      }

    private:

      struct ZipInfo {
	/* uint32_t sig = 0x02014b50 */

	uint16_t vmade; /* Version made by  (0x317) */
	uint16_t vneed; /* Version need to extract (0x14) */
	uint16_t flag; /* Bit flag (0x0) */
	uint16_t compress; /* Compression method (0, 8 or 12) */
	uint16_t time; /* File last mod time */
	uint16_t date; /* File last mod date */
	uint32_t crc; /* CRC-32 (of uncompressed file) */
	uint32_t data_size; /* Compressed size */
	uint32_t file_size; /* Uncompressed size */
	uint16_t name_length; /* File name length */
	uint16_t extra_length; /* Extra field length */
	uint16_t comment_length; /* Comment length */
	uint16_t disk; /* Disk where file starts (0) */
	uint16_t iattr; /* Internal file attributes (1 or 0 ) */
	uint32_t eattr; /* External file attributes (0x81a40000 or 0) */
	uint32_t file_offset; /* Relative offset of local file header */

	string name;
	string extra;
	string comment;

	void make_from(const string &a_name, uint32_t a_file_offset, const string &is){ /* here 'is' is the body of the 'file' */
	  vmade = 0x317;
	  vneed = 0x14;
	  flag = 0;
	  TimeToZip(date,time);
	  name_length = a_name.size();
	  name = a_name;
	  extra_length = comment_length = 0;
	  disk = iattr = 0;
	  eattr = 0x81240001; // Read for all
	  file_offset = a_file_offset;

	  boost::crc_32_type  crc32;
	  crc32.process_bytes(is.c_str(),is.size());
	  crc = crc32.checksum();
	  file_size = is.size();

	  data_size = file_size; // that should be set by caller if different
	  compress = 0; 
	}

        bool read_from(istream &is){ /* here 'is' is the ZIP file positioned to the start of the entry */
	  if (get<uint32_t>(is)!= 0x02014B50)
	    return false;

	  vmade         = get<uint16_t>(is);
	  vneed         = get<uint16_t>(is);
	  flag          = get<uint16_t>(is);
	  compress      = get<uint16_t>(is);
	  time          = get<uint16_t>(is);
	  date          = get<uint16_t>(is);
	  crc           = get<uint32_t>(is);
	  data_size     = get<uint32_t>(is);
	  file_size     = get<uint32_t>(is);
	  name_length   = get<uint16_t>(is);
	  extra_length  = get<uint16_t>(is);
	  comment_length= get<uint16_t>(is);
	  disk          = get<uint16_t>(is);
	  iattr         = get<uint16_t>(is);
	  eattr         = get<uint32_t>(is);
	  file_offset   = get<uint32_t>(is);

	  io::copy(io::slice(is,0,name_length),io::back_inserter(name));
	  io::copy(io::slice(is,0,extra_length),io::back_inserter(extra));
	  io::copy(io::slice(is,0,comment_length),io::back_inserter(comment));
	  return true;
	}

	bool write_cd(ostream &os) const {
	  put<uint32_t>(os,0x02014B50);
	  put<uint16_t>(os,vmade);
	  put<uint16_t>(os,vneed);
	  put<uint16_t>(os,flag);
	  put<uint16_t>(os,compress);
	  put<uint16_t>(os,time);
	  put<uint16_t>(os,date);
	  put<uint32_t>(os,crc);
	  put<uint32_t>(os,data_size);
	  put<uint32_t>(os,file_size);
	  put<uint16_t>(os,name_length);
	  put<uint16_t>(os,extra_length);
	  put<uint16_t>(os,comment_length);
	  put<uint16_t>(os,disk);
	  put<uint16_t>(os,iattr);
	  put<uint32_t>(os,eattr);
	  put<uint32_t>(os,file_offset);

	  os.write(name.c_str(),name.size());
	  os.write(extra.c_str(),extra.size());
	  return true;
	}

	bool write_hdr(ostream &os) const {
	  put<uint32_t>(os,0x04034b50);
	  put<uint16_t>(os,vneed);
	  put<uint16_t>(os,flag);
	  put<uint16_t>(os,compress);
	  put<uint16_t>(os,time);
	  put<uint16_t>(os,date);
	  put<uint32_t>(os,crc);
	  put<uint32_t>(os,data_size);
	  put<uint32_t>(os,file_size);
	  put<uint16_t>(os,name_length);
	  put<uint16_t>(os,extra_length);
	  os.write(name.c_str(),name.size());
	  os.write(extra.c_str(),extra.size());
	  return true;
	}
	
	void print(void) const {
	  cout << hex << vmade << ' ' << vneed << ' ' 
	       << flag << ' ' << compress << ' ' 
	       << time << ' ' << date << " (" << ZipToTime(date,time) << ") " 
	       << crc << ' ' 
	       << dec << data_size << ' ' << file_size << ' '
	       << name_length << ' ' << extra_length << ' ' 
	       << comment_length << ' ' << disk << ' ' 
	       << hex << iattr << ' ' << eattr << ' ' << file_offset << ' '
	       << " '" << name << "' "
	       << " '" << extra << "' "
	       << " '" << comment << "' "
	       << endl;
	}

      };

      struct EndRec {
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
	string   comment;

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
	uint64_t size; // size of the central directory
	uint64_t offset; // offset of the central directory
	/* ... zip64_extra === "" */

	void print(void) const {
	  cout << dec << entriesThisDisk << ' '
	       << size << ' ' << offset << ' ' 
	       << comment_length << " '" << comment << "' "
	       << endl;
	}

	EndRec() : zip64(false), comment_length(0), comment(""),
		   zip64_offset(0),
		   vmade(0x317), vneed(0x14), entriesThisDisk(0), size(0), offset(0) {}

	bool write_zip64_ecd(ostream &os){
	  put<uint32_t>(os,0x06064b50);
	  put<uint64_t>(os,56);
	  put<uint16_t>(os,vmade);
	  put<uint16_t>(os,vneed);
	  put<uint32_t>(os,0);
	  put<uint32_t>(os,0);
	  put<uint64_t>(os,entriesThisDisk);
	  put<uint64_t>(os,size);
	  put<uint64_t>(os,offset);
	  return true;
	}

	bool write_zip64_cdl(ostream &os){
	  put<uint32_t>(os,0x07064b50);
	  put<uint32_t>(os,0);
	  put<uint64_t>(os,zip64_offset);
	  put<uint32_t>(os,1);
	  return true;
	}

	bool write_ecd(ostream &os){
	  zip64_offset = os.tellp();
	  put<uint32_t>(os,0x06054b50);
	  put<uint16_t>(os,0);
	  put<uint16_t>(os,0);
	  if(entriesThisDisk > 0xfffe && zip64){
	    put<uint16_t>(os,0xffff);
	    put<uint16_t>(os,0xffff);
	  } else {
	    put<uint16_t>(os,uint16_t(entriesThisDisk));
	    put<uint16_t>(os,uint16_t(entriesThisDisk));
	  }
	  if(size > 0xfffffffe && zip64)
	    put<uint32_t>(os,0xffffffff);
	  else
	    put<uint32_t>(os,size);
	  if(offset > 0xfffffffe && zip64)
	    put<uint32_t>(os,0xffffffff);
	  else
	    put<uint32_t>(os,offset);
	  put<uint16_t>(os,comment_length);
	  os.write(comment.c_str(),comment.size());
	  return true;
	}

	bool write_to(ostream &os){
	  bool result = true;
	  if(zip64)
	    result &= write_zip64_ecd(os) & write_zip64_cdl(os);
	  return result & write_ecd(os);
	}

	bool read_zip64_ecd(istream &is){
	  is.seekg(zip64_offset,ios::beg);
	  if (get<uint32_t>(is) != 0x06064b50) {
	    cerr << "ZIP: ZIP64 ECD is not found: not a zip file??" << endl;
	    return false;
	  }
	  uint64_t ecd_size = get<uint64_t>(is);
	  vmade = get<uint16_t>(is);
	  vneed = get<uint16_t>(is);
	  uint32_t ecdDisk = get<uint32_t>(is);
	  uint32_t cdDisk  = get<uint32_t>(is);
	  entriesThisDisk = get<uint64_t>(is);
	  uint64_t entriesTotal = get<uint64_t>(is);
	  size = get<uint64_t>(is);
	  offset = get<uint64_t>(is);

	  if(ecdDisk!=0 || cdDisk!=0){
	    cerr << "ZIP: multifile archives are not supported" << endl;
	    return false;
	  }
	  if(ecd_size != 56){
	    cerr << "ZIP: unsupported ZIP64 ECD size" << endl;
	    return false;
	  }
	  if(entriesTotal != entriesThisDisk){
	    cerr << "ZIP: entries counter mismatch" << endl;
	    return false;
	  }
	  return true;
	}

	bool read_zip64_cdl(istream &is,ios::streampos cdl_offset){
	  is.seekg(cdl_offset,ios::beg);
	  if (get<uint32_t>(is) != 0x07064b50) {
	    cerr << "ZIP: ZIP64 CDL is not found: not a zip file??" << endl;
	    return false;
	  }
	  uint32_t diskStart = get<uint32_t>(is);
	  zip64_offset = get<uint64_t>(is);
	  uint32_t diskTotal = get<uint32_t>(is);

	  if(diskStart!=0 || diskTotal!=1){
	    cerr << "ZIP: multifile archives are not supported" << endl;
	    return false;
	  }
	  return true;
	}

	bool read_from(istream &is){
          // first try for a ZIP file without archive comment
          // in which case the 'end of central directory' structure
          // is the last thing in the file.
	  // Then  check whether there is an 'archive comment' at the end of this file...
	  // The comment is up to 64K in size, and we assume it does NOT contain
	  // the signature of an 'ecd' record...
	  
          is.seekg(0,ios::end);
          ios::streamoff filesize = is.tellg();
	  ios::streampos er_offset;
	  
	  for (  er_offset  =     filesize                   -streamsize(sizeEndCentDir); 
		 er_offset >= max(filesize-streamsize(0xffff)-streamsize(sizeEndCentDir),ios::streamoff(0));
		 er_offset -= 1 ) {
	    is.seekg( er_offset, ios::beg );
	    if ( get<uint8_t>(is) == 0x50 && get<uint8_t>(is) == 0x4b  &&
		 get<uint8_t>(is) == 0x05 && get<uint8_t>(is) == 0x06 ) {
	      break;
	    }
	  }
	  is.seekg(er_offset,ios::beg);
	  if (get<uint32_t>(is) != 0x06054B50) {
	    cerr << "ZIP: CDE is not found: not a zip file??" << endl;
	    return false;
	  }
	  uint16_t diskNumber    = get<uint16_t>(is);
	  uint16_t diskStart     = get<uint16_t>(is);
	  entriesThisDisk = get<uint16_t>(is);
	  uint16_t entriesTotal  = get<uint16_t>(is);
	  size          = get<uint32_t>(is);
	  offset        = get<uint32_t>(is);
	  comment_length= get<uint16_t>(is);
	  io::copy(io::slice(is,0,comment_length),io::back_inserter(comment));

	  if(diskNumber!=0 || diskStart !=0){
	    cerr << "ZIP: multifile archives are not supported" <<endl;
	    return false;
	  }

	  if(entriesTotal!=entriesThisDisk){
	    cerr << "ZIP: entries counter mismatch" << endl;
	    return false;
	  }
	  
	  /* AZ: ZIP64 is disabled
	  if(entriesThisDisk==0xffff || size==0xffffffff || offset==0xffffffff){
	    zip64 = true;
	    return read_zip64_cdl(is,er_offset-streamsize(sizeEndCentDirLoc)) & read_zip64_ecd(is);
	  }
	  */
	  return true;
	}
	
      };

      void flush() {
	if(!modified)
	  return;

	m_file->seekp( m_erec.offset, ios::beg );
	for (map<Gaudi::StringKey,ZipInfo>::const_iterator i = m_index.begin(); i!= m_index.end();++i)
	  i->second.write_cd(*m_file);
	
	m_erec.size = m_file->tellp()-streamoff(m_erec.offset);

	m_erec.write_to(*m_file);

	modified = false;
      }


      void index() {
	  if( !m_erec.read_from( *m_file ) )
	    return;

	  // m_erec.print();
	  m_file->seekg( m_erec.offset, ios::beg);

          for (;;) {
              ZipInfo info;
	      if(!info.read_from(*m_file))
		break;
	      //info.print();
              m_index[ info.name ] = info;
          }
      }
      string  m_name; 
      std::auto_ptr<fstream> m_file;
      map<Gaudi::StringKey,ZipInfo> m_index;

      EndRec m_erec;
      bool   modified;
    };
}

using namespace ConfigZipFileAccessSvc_details;

// Factory implementation
DECLARE_SERVICE_FACTORY(ConfigZipFileAccessSvc)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  ConfigZipFileAccessSvc::ConfigZipFileAccessSvc( const string& name, ISvcLocator* pSvcLocator)
    : Service ( name , pSvcLocator )
    , m_zipfile(0)
{
  string def( System::getEnv("HLTTCKROOT") );
  if (!def.empty()) def += "/config.zip";
  declareProperty("File", m_name = def);
  declareProperty("Mode", m_mode = "ReadOnly");
}

//=============================================================================
// Destructor
//=============================================================================
ConfigZipFileAccessSvc::~ConfigZipFileAccessSvc() {}

//=============================================================================
// queryInterface
//=============================================================================
StatusCode ConfigZipFileAccessSvc::queryInterface(const InterfaceID& riid,
                                                  void** ppvUnknown) {
  if ( IConfigAccessSvc::interfaceID().versionMatch(riid) )   {
    *ppvUnknown = (IConfigAccessSvc*)this;
    addRef();
    return SUCCESS;
  }
  return Service::queryInterface(riid,ppvUnknown);
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode ConfigZipFileAccessSvc::initialize() {
  debug() << "Initialize" << endmsg;
  StatusCode status = Service::initialize();
  if ( !status.isSuccess() )   return status;
  status = setProperties();
  return status;
}

ConfigZipFileAccessSvc_details::ZipFile* ConfigZipFileAccessSvc::file() const {
  if (m_zipfile.get()==0) {
      if (m_mode!="ReadOnly" &&m_mode!="ReadWrite"&&m_mode!="Truncate") {
        error() << "invalid mode: " << m_mode << endmsg;
        return 0;
      }
      ios::openmode mode =  (m_mode == "ReadWrite") ? ( ios::in | ios::out | ios::ate   )
        :                   (m_mode == "Truncate" ) ? ( ios::in | ios::out | ios::trunc )
        :                                               ios::in ;

      info() << " opening " << m_name << " in mode " << m_mode << endmsg;
      m_zipfile.reset( new ZipFile(m_name,mode) );
      if (!m_zipfile->good()) {
        error() << " Failed to open " << m_name << endmsg;
        return 0;
      }
  }
  return m_zipfile.get();
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode ConfigZipFileAccessSvc::finalize() {
  m_zipfile.reset(0);  // (ZipFile*)0); // close file if still open
  return Service::finalize();
}

string
ConfigZipFileAccessSvc::propertyConfigPath( const PropertyConfig::digest_type& digest ) const {
  string sref=digest.str();
  return  ((string("PropertyConfigs/")+= sref.substr(0,2))+="/")+= sref;
}

string
ConfigZipFileAccessSvc::configTreeNodePath( const ConfigTreeNode::digest_type& digest)  const{
  string sref=digest.str();
  return  ((string("ConfigTreeNodes/")+= sref.substr(0,2) )+="/")+= sref;
}

string
ConfigZipFileAccessSvc::configTreeNodeAliasPath( const ConfigTreeNodeAlias::alias_type& alias ) const {
  return  string("Aliases/") +=  alias.str();
}

template <typename T>
boost::optional<T>
ConfigZipFileAccessSvc::read(const string& path) const {
  debug() << "trying to read " << path << endmsg;
  if (file()==0 ) {
    debug() << "file " << m_name << " not found" << endmsg;
    return boost::optional<T>();
  }
  auto_ptr<istream> content( file()->open(path) );
  if (content.get()==0) {
    debug() << "file " << path << " not found" << endmsg;
    return boost::optional<T>();
  }
  T c;
  *content >> c;
  return c;

}

template <typename T>
bool
ConfigZipFileAccessSvc::write(const string& path ,const T& object) const {
  boost::optional<T> current = read<T>(path);
  if (current) {
    if (object==current) return true;
    error() << " object @ " << path << "  already exists, but contents are different..." << endmsg;
    return false;
  }
  if (m_mode=="ReadOnly") {
    error() <<"attempted write, but zipfile has been opened ReadOnly" << endmsg;
    return false;
  }
  std::stringstream s; s << object;
  return file()!=0 && file()->append( path, s );
}

boost::optional<PropertyConfig>
ConfigZipFileAccessSvc::readPropertyConfig(const PropertyConfig::digest_type& ref) {
  return this->read<PropertyConfig>(propertyConfigPath(ref));
}

boost::optional<ConfigTreeNode>
ConfigZipFileAccessSvc::readConfigTreeNode(const ConfigTreeNode::digest_type& ref) {
  return this->read<ConfigTreeNode>(configTreeNodePath(ref));
}

boost::optional<ConfigTreeNode>
ConfigZipFileAccessSvc::readConfigTreeNodeAlias(const ConfigTreeNodeAlias::alias_type& alias) {
  string fnam = configTreeNodeAliasPath(alias);
  boost::optional<string> sref = this->read<string>( fnam );
  if (!sref) return boost::optional<ConfigTreeNode>();
  ConfigTreeNode::digest_type ref = ConfigTreeNode::digest_type::createFromStringRep(*sref);
  if (!ref.valid()) {
    error() << "content of " << fnam << " not a valid ref" << endmsg;
    return boost::optional<ConfigTreeNode>();
  }
  return readConfigTreeNode(ref);
}

vector<ConfigTreeNodeAlias>
ConfigZipFileAccessSvc::configTreeNodeAliases(const ConfigTreeNodeAlias::alias_type& alias)
{
  vector<ConfigTreeNodeAlias> x;

  string basename("Aliases");
  if (file()==0) { return x ; }
  vector<string> aliases = file()->files( PrefixFilenameSelector(basename+"/"+alias.major()) );

  for (vector<string>::const_iterator i  = aliases.begin(); i!=aliases.end(); ++i ) {
    //TODO: this can be more efficient...
    debug() << " configTreeNodeAliases: adding file " << *i << endmsg;
    string ref;
    auto_ptr<istream> content( file()->open(*i) );
    *content >> ref;
    string _alias = i->substr( basename.size()+1 );
    stringstream str;
    str << "Ref: " << ref << '\n' << "Alias: " << _alias << endl;
    ConfigTreeNodeAlias a;
    str >> a;
    debug() << " configTreeNodeAliases: content:" << a << endmsg;
    x.push_back(a);
  }
  return x;
}

PropertyConfig::digest_type
ConfigZipFileAccessSvc::writePropertyConfig(const PropertyConfig& config) {
  PropertyConfig::digest_type digest = config.digest();
  return this->write(propertyConfigPath(digest), config) ? digest
    : PropertyConfig::digest_type::createInvalid();
}

ConfigTreeNode::digest_type
ConfigZipFileAccessSvc::writeConfigTreeNode(const ConfigTreeNode& config) {
  ConfigTreeNode::digest_type digest = config.digest();
  return this->write(configTreeNodePath(digest), config) ? digest
    : ConfigTreeNode::digest_type::createInvalid();
}

ConfigTreeNodeAlias::alias_type
ConfigZipFileAccessSvc::writeConfigTreeNodeAlias(const ConfigTreeNodeAlias& alias) {
  // verify that we're pointing at something existing
  if ( !readConfigTreeNode(alias.ref()) ) {
    error() << " Alias points at non-existing entry " << alias.ref() << "... refusing to create." << endmsg;
    return ConfigTreeNodeAlias::alias_type();
  }
  // now write alias...
  fs::path fnam = configTreeNodeAliasPath(alias.alias());
  boost::optional<string> x = read<string>(fnam.string());
  if (!x) {
    //TODO: this is where we actually write...
    std::stringstream s; s << alias.ref();
    if (file()==0) { 
        error() << " container file not found during attempted write of " << fnam.string() << endmsg;
        return ConfigTreeNodeAlias::alias_type();
    }
    if ( file()->append(fnam.string(),s)) { 
        info() << " created " << fnam.string() << endmsg;
        return alias.alias();
    } else  {
        error() << " Writing not implemented at this time " << endmsg;
        error() << " failed to write " << fnam.string() << endmsg;
        return ConfigTreeNodeAlias::alias_type();
    }
  } else {
    //@TODO: decide policy: in which cases do we allow overwrites of existing labels?
    // (eg. TCK aliases: no!, tags: maybe... , toplevel: impossible by construction )
    // that policy should be common to all implementations, so move to a mix-in class,
    // or into ConfigTreeNodeAlias itself
    if ( ConfigTreeNodeAlias::digest_type::createFromStringRep(*x)==alias.ref() ) return alias.alias();
    error() << " Alias already exists, but contents differ... refusing to change" << endmsg;
    return ConfigTreeNodeAlias::alias_type();
  }
}

MsgStream& ConfigZipFileAccessSvc::msg(MSG::Level level) const {
  if (m_msg.get()==0) m_msg.reset( new MsgStream( msgSvc(), name() ));
  return *m_msg << level;
}
