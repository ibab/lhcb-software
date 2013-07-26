#include "ConfigTarFileAccessSvc.h"
#include <sstream>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <time.h>

#ifndef _WIN32
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#else
typedef int uid_t;
typedef int gid_t;
#endif

using namespace std;

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


#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/StringKey.h"

namespace {

#ifndef _WIN32
  class CleanupAtExit {
  public:
        static CleanupAtExit& instance() {
              static  CleanupAtExit theOne;
              return theOne;
        }

        void add(const std::string& name)    { m_files.insert(name); }
        void remove(const std::string& name) { m_files.erase(name);  }
  private :
        CleanupAtExit() {} 
        ~CleanupAtExit() {
                for (std::set<std::string>::const_iterator i=m_files.begin();i!=m_files.end();++i) {
                        cerr << "ConfigTarFileAccessSvc::CleanupAtExit: removing " << *i << endl;
                        unlink(i->c_str());
                        cerr << "ConfigTarFileAccessSvc::CleanupAtExit: removed " << *i << endl;
                }
        }
        std::set<std::string> m_files;
  };
#endif

  struct DefaultFilenameSelector {
    bool operator()(const std::string& /*fname*/) const { return true; }
  };

  struct PrefixFilenameSelector {
    PrefixFilenameSelector( const std::string& _prefix) : prefix(_prefix) {}
    bool operator()(const std::string& fname) const { return fname.compare(0,prefix.size(),prefix)==0; }
    std::string prefix;
  };


  /* POSIX tar Header Block, from POSIX 1003.1-1990  */
  struct posix_header
  {                               /* byte offset */
    char name[100];               /*   0-99 */
    char mode[8];                 /* 100-107 */
    char uid[8];                  /* 108-115 */
    char gid[8];                  /* 116-123 */
    char size[12];                /* 124-135 */
    char mtime[12];               /* 136-147 */
    char chksum[8];               /* 148-155 */
    char typeflag;                /* 156-156 */
    char linkname[100];           /* 157-256 */
    char magic[6];                /* 257-262 */
    char version[2];              /* 263-264 */
    char uname[32];               /* 265-296 */
    char gname[32];               /* 297-328 */
    char devmajor[8];             /* 329-336 */
    char devminor[8];             /* 337-344 */
    char prefix[155];             /* 345-499 */
    char padding[12];             /* 500-512 (pad to exactly the TAR_BLOCK_SIZE) */
  };

  template <typename T>
  std::string convert(const T& in) {
    // take into account that eg. name and prefix do not have a
    // terminating '\0' in case they occupy the full length...
    std::string s(in,sizeof(in));
    std::string::size_type last = s.find(char(0));
    return  (last==std::string::npos)? s : s.substr(0,last);
  }


    //size_t maxValWithDigits(unsigned digits, unsigned bitsPerDigit) {
    //    return (1UL << (digits*bitsPerDigit)) - 1 ;
    //}

  template <typename T>
  char* putString(const char* s, T& buffer ) {
    return strncpy(buffer, s, sizeof(buffer));
  }
  template <typename T, size_t Size>
  bool putOctal(T val, char (&buf)[Size] ) {
    // octal needs 3 bits per digit, and we need a terminal '\0'
    //uintmax_t m = maxValWithDigits( Size-1, 3)
    //TODO: check whether val fits within size octal characters...
    size_t size = Size;
    buf[--size] = '\0';
    do {
      buf[--size] = '0' + (val & ((1 << 3) - 1));
      val >>= 3;
    } while (size);
    return true;
  }

  bool isZero(const struct posix_header& h) {
    const char* i = (const char*)(&h);
    const char* end = i+512;
    while (i<end && *i++==0) { /* nothing*/  }
    return i==end;
  }

  void zero(struct posix_header& h) {
    memset(&h,0,sizeof(posix_header));
  }


  /* A nice enum with all the possible tar file content types */
  enum TarFileType
    {
      REGTYPE  = '0',            /* regular file */
      REGTYPE0 = '\0',           /* regular file (ancient bug compat)*/
      GNUTYPE_LONGLINK = 'K',
      GNUTYPE_LONGNAME = 'L'
    };
  typedef enum TarFileType TarFileType;

}


namespace ConfigTarFileAccessSvc_details {
  class TarFile {
  public:
    TarFile(const std::string& name, ios::openmode mode = ios::in, bool compressOnWrite = false )
      : m_name(name)
      , m_lock(-1)
      , m_leof(0)
      , m_indexUpToDate(false)
      , m_myUid(0)
      , m_myGid(0)
      , m_compressOnWrite( compressOnWrite )
    {
#ifndef _WIN32
      if (mode&ios::out) {
            std::string lckname = m_name + ".lock";
            // From http://pubs.opengroup.org/onlinepubs/7908799/xsh/open.html:
            //
            // O_CREAT and O_EXCL are set, open() will fail if the file exists. The check for 
            // the existence of the file and the creation of the file if it does not exist will 
            // be atomic with respect to other processes executing open() naming the same 
            // filename in the same directory with O_EXCL and O_CREAT set. If O_CREAT is not set, 
            // the effect is undefined.
            //
            m_lock = open(lckname.c_str(), O_RDWR|O_CREAT|O_EXCL,S_IRUSR|S_IWUSR);
            if (m_lock < 0) { 
                cerr << "trying to open  " << m_name << " for writing, but lockfile " << lckname << " already exists." << endl;
                cerr << "This (most likely) means that some other process intends to write into " << m_name << endl;
                cerr << "Refusing to continue to preserve integrity of " << m_name << "... goodbye" << endl;
                ::abort();
            }
            cerr << "succesfully created lock file " << lckname << endl;
            CleanupAtExit::instance().add(lckname);
            // TODO: write some text to the lockfile to identify this process: machine name, pid...
            //       which can be used in the above failure case to point out the source of the clash...
      }
#endif
      m_file.open(m_name.c_str(), mode | ios::in | ios::binary );
    }
    bool good() const { return m_file.good(); }
    bool dump(const std::string& name,ostream& os) {
      const map<Gaudi::StringKey,Info>& myIndex = getIndex();
      map<Gaudi::StringKey,Info>::const_iterator i = myIndex.find(name);
      if (i!=myIndex.end()) {
        // slice works relative to the current file offset, as it works on an istream...
        m_file.seekg(0,std::ios_base::beg);
        if (i->second.compressed) {
#ifndef _WIN32
            io::filtering_istream in;
#ifdef __INTEL_COMPILER         // Disable ICC warning
  #pragma warning(disable:279)  // gzip.hpp(551) controlling expression constant
  #pragma warning(push)
#endif
            in.push(io::gzip_decompressor());
#ifdef __INTEL_COMPILER         // Re-enable ICC warning 279
  #pragma warning(pop)
#endif
            in.push(io::slice(m_file,i->second.offset,i->second.size));
            // suggest an 8K buffer size as hint -- most config items are smaller than that...
            io::copy(in, os, 8192);
#else 
            return false;
#endif // _WIN32
        } else {
            // suggest an 8K buffer size as hint -- most config items are smaller than that...
            io::copy(io::slice(m_file,i->second.offset,i->second.size), os, 8192);
        }
        return true;
      }
      return false;
    }

    bool exists(const std::string& path) {
      const map<Gaudi::StringKey,Info>& myIndex = getIndex();
      return myIndex.find(path)!=myIndex.end();

    }
    template <typename SELECTOR>
    std::vector<std::string> files(const SELECTOR& selector) const {
      std::vector<std::string> f;
      const map<Gaudi::StringKey,Info>& myIndex = getIndex();
      for (map<Gaudi::StringKey,Info>::const_iterator i = myIndex.begin(); i!= myIndex.end();++i) {
        if( selector(i->first) ) f.push_back(i->first);
      }
      return f;
    }
    std::vector<std::string> files() const { return files(DefaultFilenameSelector()); }
    bool append(const string& name, std::stringstream& is);

    ~TarFile() {
      m_file.close();
#ifndef _WIN32
      if (! (m_lock<0) )  { 
            cerr << "releasing lock " << endl;
            close(m_lock);
            unlink( ( m_name + ".lock").c_str() );
            cerr << "removed lock file " << ( m_name+".lock" )  << endl;
            CleanupAtExit::instance().remove(m_name+".lock");

      }
#endif
    }
  private:
    struct Info {
      Info() : size(0),type( TarFileType(0) ),offset(0), compressed(false) {}
      std::string      name;
      size_t           size;
      TarFileType      type;
      size_t           offset;
      bool             compressed;
    };

    bool _append(const string& name, std::stringstream& is);
    bool index(std::streamoff start=0) const;
    const std::map<Gaudi::StringKey,Info>&  getIndex() const {
      if (!m_indexUpToDate) m_indexUpToDate = index();
      if (!m_indexUpToDate) {
        throw GaudiException("Failed to index tarfile ",m_name,StatusCode::FAILURE);
      }
      return m_index;
    }
    long computeChecksum(const posix_header& header) const;
    bool interpretHeader(posix_header& header, struct Info& info) const;
    // Read an octal value in a field of the specified width, with optional
    // spaces on both sides of the number and with an optional null character
    // at the end.  Returns -1 on an illegal format.
    bool isOctal(const char& ch) const   { return ((ch >= '0') && (ch <= '7')); }
    long getOctal(const char *cp, int size) const
    {
      long val = 0;
      for(;(size > 0) && (*cp == ' '); cp++, size--);
      if ((size == 0) || !isOctal(*cp)) return -1;
      for(; (size > 0) && isOctal(*cp); size--) val = (val<<3)  + (*cp++ - '0');
      for (;(size > 0) && (*cp == ' '); cp++, size--);
      if ((size > 0) && *cp) return -1;
      return val;
    }
    uid_t getUid() const {
#ifndef _WIN32
      if (m_myUid==0) m_myUid = getuid();
#endif
      return m_myUid;
    }
    gid_t getGid() const {
#ifndef _WIN32
      if (m_myGid==0) m_myGid = getgid();
#endif
      return m_myGid;
    }
    const char *getUname() const {
#ifndef _WIN32
      if (m_uname.empty()) {
        struct passwd *passwd = getpwuid(getUid());
        m_uname = ( passwd ? passwd->pw_name : "" );
      }
#endif
      return m_uname.c_str();
    }
    const char *getGname() const {
#ifndef _WIN32
      if (m_gname.empty()) {
        struct group *group = getgrgid(getGid());
        m_gname = ( group ? group->gr_name : "" );
      }
#endif
      return m_gname.c_str();
    }

    std::string m_name;
    mutable fstream m_file;
    mutable int m_lock;
    mutable std::map<Gaudi::StringKey,Info> m_index;
    mutable long m_leof;
    mutable bool m_indexUpToDate;
    mutable std::string m_gname;
    mutable std::string m_uname;
    mutable uid_t m_myUid;
    mutable gid_t m_myGid;
    bool m_compressOnWrite;
  };

  bool TarFile::append(const string& name, std::stringstream& is) {
#ifndef _WIN32
    if (m_compressOnWrite && is.str().size()>512 && (name.size()<3||name.compare(name.size()-3,3,".gz")!=0) ) {
        std::stringstream out;
        io::filtering_istream in;
        in.push(io::gzip_compressor(9));
        in.push(is);
        io::copy(in,out, is.str().size());
        // TODO: check that the compressed version actually occupies less blocks
        //       if not, it's useless to compress..
        return _append(name+".gz",out);
    } 
#endif
    return _append(name,is);
  }

  bool TarFile::_append(const string& name, std::stringstream& is) {
    //TODO: check if file is open in read/write mode...
    m_indexUpToDate = false;
    std::string bstring = is.str();
    size_t size = bstring.size();
    const char* buffer = bstring.c_str();
    if (m_file.tellp()>=0 && m_leof!=m_file.tellp() ) {
      m_file.seekp(m_leof,ios::beg); // where do we start writing?
    }
    posix_header header;
    memset(&header,0,sizeof(header));
    if (name.size()>sizeof(header.name)) {
      // try to split it on a '/'
      // otherwise, go for the GNU LongLink solution...

      // generate longlink header...
      putString("././@LongLink",header.name);
      header.typeflag =  GNUTYPE_LONGNAME;
      putOctal( name.size(), header.size);
      putString( "ustar ",header.magic);
      putString("00", header.version);
      // and write header
      putOctal(computeChecksum(header),header.chksum);
      m_file.write((const char*)&header,sizeof(header));
      // followed by name, padded up to 512 byte block..
      std::string nameBuffer(name);
      if (nameBuffer.size()%512) nameBuffer.resize( ((nameBuffer.size()/512)+1)*512 ,'\0');
      m_file.write(nameBuffer.c_str(), nameBuffer.size());

      // clear and continue with a header with truncated name...
      memset(&header,0,sizeof(header));
    }
    putString(name.c_str(),header.name);
#ifdef _WIN32
    putOctal(33060u, header.mode);
#else
    putOctal( S_IFREG|S_IRUSR|S_IRGRP|S_IROTH, header.mode);
#endif
    putString( "ustar ",header.magic);
    putString("00", header.version);
    putOctal( getUid(), header.uid);
    putOctal( getGid(), header.gid);
    putOctal( size, header.size);
    putOctal( time(0), header.mtime);
    header.typeflag =  REGTYPE ;
    putString(getUname(),header.uname);
    putString(getGname(),header.gname);
    putOctal(0,header.devmajor);
    putOctal(0,header.devminor);

    // last but not least, compute checksum...
    putOctal(computeChecksum(header),header.chksum);
    m_file.write((const char*)&header,sizeof(header));


    // and write the file contents, padded with zeros
    m_file.write(buffer,size);
    static const size_t blockingFactor = 20;
    static char zeros[blockingFactor*512]={0};
    if (size%512) m_file.write(zeros,512-size%512);
    // update the logical EOF
    streamoff leof = m_file.tellp();
    // write end-of-file marker : 2 zero blocks, padded up to the blocking factor
    m_file.write(zeros,2*512);
    if (m_leof%(blockingFactor*512)) {
      m_file.write(zeros,blockingFactor*512-m_leof%(blockingFactor*512));
    }
    m_file.flush();
    m_indexUpToDate = index( m_leof );
    if (m_leof!=leof) {
      cerr << "oops: read-back eof not what was predicted: " << m_leof << " vs. " << leof << endl;
      return false;
    }
    return true;
  }

  long TarFile::computeChecksum(const posix_header& header)  const {
    /* Check the checksum */
    long sum=0;
    unsigned char *s = (unsigned char *)&header;
    for (int i = sizeof(header); i-- != 0;) sum += *s++;
    /* Remove the effects of the checksum field (replace
     * with blanks for the purposes of the checksum) */
    for (int i = sizeof header.chksum; i-- != 0;) {
      sum -= (unsigned char) header.chksum[i];
    }
    sum += ' ' * sizeof header.chksum;
    return sum;
  }

    bool TarFile::interpretHeader(posix_header& header, Info& info) const {
                if (strncmp(header.magic,"ustar ",6)&&strncmp(header.magic,"ustar\0",6)) { 
                    return false;
                }
                long chksum       = getOctal(header.chksum,sizeof(header.chksum));
                /* Check the checksum */
                long sum=computeChecksum(header);
                if (sum!=chksum) {
                    std::cerr << " bad chksum " << sum << " vs. " << chksum << std::endl;
                    return false;
                }
                info.type  = TarFileType(header.typeflag);
                long size  = getOctal(header.size,sizeof(header.size));
                if (size<0) {
                    std::cerr << " got negative file size: " << info.size << std::endl;
                    return false;
                }
                info.size  = size;
                info.name  = convert(header.name);
                if (header.prefix[0]!=0) {
                    std::string prefix = convert(header.prefix);
                    info.name = prefix + "/" + info.name;
                }

                if ( info.type == GNUTYPE_LONGNAME ) { 
                    // current header is a dummy 'flag', followed by data blocks 
                    // with name as content (length of name is 'size' of data)
                    assert(info.name == "././@LongLink");
                    // first read the real, untruncated name as data
                    std::ostringstream fname;
                    io::copy(io::slice(m_file,0,info.size), fname);
                    size_t padding = info.size % 512;
                    if (padding!=0) m_file.seekg(512-padding,std::ios::cur);
                    // and now get another header, which contains a truncated name
                    // but which is otherwise the 'real' one
                    m_file.read( (char*) &header, sizeof(header) ) ;
                    if (!interpretHeader( header, info)) return false;
                    // so we overwrite the truncated one with the long one...
                    info.name = fname.str();
                }
                info.offset = m_file.tellg(); // this goes here, as the longlink handling
                                              // reads an extra block...
                // if name ends in .gz, assume it is gzipped.
                // Strip the name down, and flag as compressed in info.
                info.compressed = (info.name.size()>3 && info.name.compare(info.name.size()-3,3,".gz") == 0 );
                if (info.compressed) info.name = info.name.substr( 0, info.name.size()-3 ) ; 

                return true;
    }

    bool TarFile::index(std::streamoff offset) const {
            posix_header header;
            if (offset==0) m_index.clear();
            m_file.seekg(offset,ios::beg);
            while (m_file.read( (char*) &header, sizeof(header) )) {
                Info info;
                if (!interpretHeader( header, info))  {
                    // check whether we're at the end of the file: (at least) two all-zero headers)
                    if (isZero(header)) {
                        m_file.read( (char*) &header, sizeof(header) ) ;
                        if (isZero(header)){
                            m_leof = m_file.tellg()-2*std::streamoff(sizeof(header));
                            m_file.seekg(0,std::ios::beg);
                            return true;
                        }
                    }
                    std::cerr << "failed to interpret header preceeding " << m_file.tellg() << " in tarfile " << m_name << std::endl;
                    m_file.seekg(0,std::ios::beg);
                    return false;
                }
                if (info.name.empty()) {
                    std::cerr << " got empty name " << std::endl;
                    break;
                }
                if ( (info.type == REGTYPE || info.type == REGTYPE0 ) 
                   && info.name[ info.name.size()-1 ] != '/' 
                   && info.name.find("/CVS/")  == string::npos 
                   && info.name.find("/.svn/") == string::npos )  {
                    //TODO: check for duplicates!!
                    m_index.insert(make_pair(Gaudi::StringKey(info.name),info));
                }
                // round up size to block size, and skip to next header...
                size_t skip = info.size;
                size_t padding = skip % 512;
                if (padding!=0 ) skip += 512 - padding;
                m_file.seekg(skip,std::ios::cur);
            }
            m_leof = 0;
            m_file.clear();
            m_file.seekg(0,std::ios::beg);
            return true;
    }
}

using namespace ConfigTarFileAccessSvc_details;

// Factory implementation
DECLARE_SERVICE_FACTORY(ConfigTarFileAccessSvc)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  ConfigTarFileAccessSvc::ConfigTarFileAccessSvc( const std::string& name, ISvcLocator* pSvcLocator)
    : Service ( name , pSvcLocator )
    , m_tarfile(0)
{
  std::string def( System::getEnv("HLTTCKROOT") );
  if (!def.empty()) def += "/config.tar";
  declareProperty("File", m_name = def);
  declareProperty("Mode", m_mode = "ReadOnly");
  declareProperty("CompressOnWrite", m_compress = true );
}

//=============================================================================
// Destructor
//=============================================================================
ConfigTarFileAccessSvc::~ConfigTarFileAccessSvc() {}

//=============================================================================
// queryInterface
//=============================================================================
StatusCode ConfigTarFileAccessSvc::queryInterface(const InterfaceID& riid,
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
StatusCode ConfigTarFileAccessSvc::initialize() {
  if( msgLevel(MSG::DEBUG) ) debug() << "Initialize" << endmsg;
  StatusCode status = Service::initialize();
  if ( !status.isSuccess() )   return status;
  status = setProperties();
  return status;
}

ConfigTarFileAccessSvc_details::TarFile* ConfigTarFileAccessSvc::file() const {
  if (m_tarfile.get()==0) { 

      if (m_mode!="ReadOnly"&&m_mode!="ReadWrite"&&m_mode!="Truncate") {
        error() << "invalid mode: " << m_mode << endmsg;
        return 0;
      }

      ios::openmode mode =  (m_mode == "ReadWrite") ? ( ios::in | ios::out | ios::ate   )
        :                   (m_mode == "Truncate" ) ? ( ios::in | ios::out | ios::trunc )
        :                                               ios::in ;

      info() << " opening " << m_name << " in mode " << m_mode << endmsg;
      m_tarfile.reset( new TarFile(m_name,mode,m_compress) );
      if (!m_tarfile->good()) {
        error() << " Failed to open " << m_name << " in mode " << m_mode << endmsg;
      }
  }
  return m_tarfile.get();
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode ConfigTarFileAccessSvc::finalize() {
  m_tarfile.reset(0);  // (TarFile*)0); // close file if still open
  return Service::finalize();
}

std::string
ConfigTarFileAccessSvc::propertyConfigPath( const PropertyConfig::digest_type& digest ) const {
  std::string sref=digest.str();
  return  ((std::string("PropertyConfigs/")+= sref.substr(0,2))+="/")+= sref;
}

std::string
ConfigTarFileAccessSvc::configTreeNodePath( const ConfigTreeNode::digest_type& digest)  const{
  std::string sref=digest.str();
  return  ((std::string("ConfigTreeNodes/")+= sref.substr(0,2) )+="/")+= sref;
}

std::string
ConfigTarFileAccessSvc::configTreeNodeAliasPath( const ConfigTreeNodeAlias::alias_type& alias ) const {
  return  std::string("Aliases/") +=  alias.str();
}

template <typename T>
boost::optional<T>
ConfigTarFileAccessSvc::read(const std::string& path) const {
  stringstream content(stringstream::in | stringstream::out);
  if( msgLevel(MSG::DEBUG) ) debug() << "trying to read " << path << endmsg;
  if (file()==0 || !file()->dump(path,content)) {
    if( msgLevel(MSG::DEBUG) ) debug() << "file " << path << " not found" << endmsg;
    return boost::optional<T>();
  }
  T c;
  content >> c;
  return c;

}

template <typename T>
bool
ConfigTarFileAccessSvc::write(const std::string& path,const T& object) const {
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
  return file()!=0 && file()->append( path, s );
}

boost::optional<PropertyConfig>
ConfigTarFileAccessSvc::readPropertyConfig(const PropertyConfig::digest_type& ref) {
  return this->read<PropertyConfig>(propertyConfigPath(ref));
}

boost::optional<ConfigTreeNode>
ConfigTarFileAccessSvc::readConfigTreeNode(const ConfigTreeNode::digest_type& ref) {
  return this->read<ConfigTreeNode>(configTreeNodePath(ref));
}

boost::optional<ConfigTreeNode>
ConfigTarFileAccessSvc::readConfigTreeNodeAlias(const ConfigTreeNodeAlias::alias_type& alias) {
  stringstream content(stringstream::in | stringstream::out);
  std::string fnam = configTreeNodeAliasPath(alias);
  if (file()==0 || !file()->dump(fnam,content)) {
    if( msgLevel(MSG::DEBUG) ) debug() << "file " << fnam << " does not exist" << endmsg;
    return boost::optional<ConfigTreeNode>();
  }
  std::string sref;
  content >> sref;
  ConfigTreeNode::digest_type ref = ConfigTreeNode::digest_type::createFromStringRep(sref);
  if (!ref.valid()) {
    error() << "content of " << fnam << " not a valid ref" << endmsg;
    return boost::optional<ConfigTreeNode>();
  }
  return readConfigTreeNode(ref);
}

std::vector<ConfigTreeNodeAlias>
ConfigTarFileAccessSvc::configTreeNodeAliases(const ConfigTreeNodeAlias::alias_type& alias)
{
  std::vector<ConfigTreeNodeAlias> x;

  std::string basename("Aliases");
  if (file()==0) { return x ; }
  std::vector<std::string> aliases = file()->files( PrefixFilenameSelector(basename+"/"+alias.major()) );

  for (std::vector<std::string>::const_iterator i  = aliases.begin(); i!=aliases.end(); ++i ) {
    //TODO: this can be more efficient...
    if( msgLevel(MSG::DEBUG) ) debug() << " configTreeNodeAliases: adding file " << *i << endmsg;
    std::string ref;
    stringstream content(stringstream::in | stringstream::out);
    file()->dump(*i,content);
    content >> ref;
    std::string _alias = i->substr( basename.size()+1 );
    std::stringstream str;
    str << "Ref: " << ref << '\n' << "Alias: " << _alias << std::endl;
    ConfigTreeNodeAlias a;
    str >> a;
    if( msgLevel(MSG::DEBUG) ) debug() << " configTreeNodeAliases: content:" << a << endmsg;
    x.push_back(a);
  }
  return x;
}

PropertyConfig::digest_type
ConfigTarFileAccessSvc::writePropertyConfig(const PropertyConfig& config) {
  PropertyConfig::digest_type digest = config.digest();
  return this->write(propertyConfigPath(digest), config) ? digest
    : PropertyConfig::digest_type::createInvalid();
}

ConfigTreeNode::digest_type
ConfigTarFileAccessSvc::writeConfigTreeNode(const ConfigTreeNode& config) {
  ConfigTreeNode::digest_type digest = config.digest();
  return this->write(configTreeNodePath(digest), config) ? digest
    : ConfigTreeNode::digest_type::createInvalid();
}

ConfigTreeNodeAlias::alias_type
ConfigTarFileAccessSvc::writeConfigTreeNodeAlias(const ConfigTreeNodeAlias& alias ) {
  // verify that we're pointing at something existing
  if ( !readConfigTreeNode(alias.ref()) ) {
    error() << " Alias points at non-existing entry " << alias.ref() << "... refusing to create." << endmsg;
    return ConfigTreeNodeAlias::alias_type();
  }
  // now write alias...
  fs::path fnam = configTreeNodeAliasPath(alias.alias());
  //fs::path fdir = fnam.branch_path();
  //if (fs::exists(fdir)) {
  //         if (!isCompatible(alias,fdir) ) {
  //             error() << " current TOPLEVEL is not compatible with other config in " << fdir.string() << " refusing to write TOPLEVEL alias " << endmsg;
  //             return ConfigTreeNodeAlias::alias_type();
  //         }
  //} else if ( !create_directories(fdir) ) {
  //         error() << " directory " << fdir.string() << " does not exist, and could not create... " << endmsg;
  //         return ConfigTreeNodeAlias::alias_type();
  //}
  boost::optional<string> x = read<string>(fnam.string());
  if (!x) {
    std::stringstream s; s << alias.ref();
    if (file()==0) return ConfigTreeNodeAlias::alias_type();
    file()->append(fnam.string(),s);
    info() << " created " << fnam.string() << endmsg;
    return alias.alias();
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

MsgStream& ConfigTarFileAccessSvc::msg(MSG::Level level) const {
  if (m_msg.get()==0) m_msg.reset( new MsgStream( msgSvc(), name() ));
  return *m_msg << level;
}
