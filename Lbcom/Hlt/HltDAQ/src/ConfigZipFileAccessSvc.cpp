#include "ConfigZipFileAccessSvc.h"

#include <map>
#include <iostream>
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
}


namespace ConfigZipFileAccessSvc_details {
    enum { sizeEndCentDir = 22 };

    class ZipFile  : boost::noncopyable {
    public:

      ZipFile(const string& name, ios::openmode mode = ios::in) 
        : m_name( name )
        , m_file( new fstream( name.c_str(), mode | ios::in | ios::binary ) )
      {
          index();
      } 
      ~ZipFile() { }

      bool good() const { return m_file->good(); }
      vector<string> files() const { return files(DefaultFilenameSelector()); }
      bool append(const string& /*name*/, std::stringstream& /*is*/) { return false; } //TODO: implement writing...

      template <typename SELECTOR> vector<string> files(const SELECTOR& selector) const {
          vector<string> f;
          for (map<Gaudi::StringKey,ZipInfo>::const_iterator i = m_index.begin(); i!= m_index.end();++i) {
            if( selector(i->first) ) f.push_back(i->first);
          }
          return f;
      }

      istream* open(const string& fname) {
            map<Gaudi::StringKey,ZipInfo>::const_iterator i = m_index.find(fname);
            if ( i == m_index.end() ) return 0;
            m_file->seekg( i->second.file_offset,ios::beg);
            if (get<uint32_t>(*m_file)!=0x04034B50)  return 0;
            m_file->seekg(22, ios::cur);
            uint32_t offset = i->second.file_offset+30+get<uint16_t>(*m_file)+get<uint16_t>(*m_file);
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
              uint16_t compress;
              uint16_t time;
              uint16_t date;
              uint32_t crc;
              uint32_t data_size;
              uint32_t file_size;
              uint32_t file_offset;
              string name;
      };

      struct EndRec {
            enum { signature = 0x06054b50 };
            uint16_t diskNumber;
            uint16_t diskStart;
            uint16_t entriesThisDisk ;
            uint16_t entriesTotal;
            uint32_t size;
            uint32_t offset;
            string   comment;
      };

      struct CentralDirectory {
            enum { signature = 0x02014b50 };
      };


      void index() {
          // first try for a ZIP file without archive comment
          // in which case the 'end of central directory' structure
          // is the last thing in the file
          m_file->seekg(0,ios::end);
          ios::streamoff filesize = m_file->tellg();
          m_file->seekg(-sizeEndCentDir,ios::end);
          ios::streampos header_position = m_file->tellg();
          if (get<uint32_t>(*m_file) != 0x06054B50) {
              // Check whether there is an 'archive comment' at the end of this file...
              // The comment is up to 64K in size, and we assume it does NOT contain
              // the signature of an 'ecd' record...
              for (  header_position  =     filesize                   -streamsize(sizeEndCentDir); 
                     header_position >= max(filesize-streamsize(0xffff)-streamsize(sizeEndCentDir),ios::streamoff(0));
                     header_position -= 1 ) {
                    m_file->seekg( header_position, ios::beg );
                    if ( get<uint8_t>( *m_file ) == 0x50 && get<uint8_t>( *m_file) == 0x4b  &&
                         get<uint8_t>( *m_file ) == 0x05 && get<uint8_t>( *m_file) == 0x06 ) {
                        break;
                    }
              }
              m_file->seekg(header_position,ios::beg);
              if (get<uint32_t>(*m_file) != 0x06054B50) {
                    cerr << "not a zip file??";
                    return;
              }
          }
          // skip disk number(2), disk with start of central dir(2), entries in cdir in this disk(2), total entries cdir(2)
          m_file->seekg(2+2+2+2, ios::cur); 
          uint32_t header_size   = get<uint32_t>(*m_file);
          uint32_t header_offset = get<uint32_t>(*m_file);
          uint32_t arc_offset    = ios::streamoff(header_position) - header_offset - header_size;
          header_offset += arc_offset;

          // pick up trailing comment if here...
          uint16_t comment_size  = get<uint16_t>(*m_file);
          string comment;
          io::copy(io::slice(*m_file,0,comment_size),io::back_inserter(comment));
           
          // try to read the zip64 end of central directory...


          for (;;) {
              ZipInfo info;
              m_file->seekg( header_offset, ios::beg);
              if (get<uint32_t>(*m_file)!= 0x02014B50) break;
              m_file->seekg( header_offset + 10, ios::beg);
              info.compress      = get<uint16_t>(*m_file);
              info.time          = get<uint16_t>(*m_file);
              info.date          = get<uint16_t>(*m_file);
              info.crc           = get<uint32_t>(*m_file);
              info.data_size     = get<uint32_t>(*m_file);
              info.file_size     = get<uint32_t>(*m_file);
              uint16_t name_size = get<uint16_t>(*m_file);
              header_size = 46 + name_size + get<uint16_t>(*m_file) + get<uint16_t>(*m_file);
              m_file->seekg( header_offset + 42, ios::beg);
              info.file_offset   = get<uint32_t>(*m_file) + arc_offset;
              stringstream fname;
              io::copy(io::slice(*m_file,0,name_size),fname );
              info.name          = fname.str();
              header_offset += header_size;
              m_index[ info.name ] = info;
          }
      }
      string  m_name; 
      std::auto_ptr<istream> m_file;
      map<Gaudi::StringKey,ZipInfo> m_index;
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
      if (m_mode!="ReadOnly") { // &&m_mode!="ReadWrite"&&m_mode!="Truncate") {
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
    error() <<"attempted write, but tarfile has been opened ReadOnly" << endmsg;
    return false;
  }
  std::stringstream s; s << object;
  error() << " Writing not implemented at this time " << endmsg;
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
