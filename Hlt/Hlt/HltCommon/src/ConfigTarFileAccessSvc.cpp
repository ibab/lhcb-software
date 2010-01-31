#include "ConfigTarFileAccessSvc.h"
#include <sstream>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

#include "boost/lexical_cast.hpp"
#include "boost/filesystem/path.hpp"
namespace fs = boost::filesystem;

#include "boost/iostreams/filtering_stream.hpp"
#include "boost/iostreams/copy.hpp"
#include "boost/iostreams/filter/gzip.hpp"
#include "boost/iostreams/slice.hpp"
#include "boost/iostreams/operations.hpp"
#include "boost/iostreams/seek.hpp"
namespace bio = boost::iostreams;



#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/StringKey.h"


namespace {

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
        GNUTYPE_LONGNAME = 'L',
    };
    typedef enum TarFileType TarFileType;

}


namespace ConfigTarFileAccessSvc_details {
class TarFile {
public:
    TarFile(const std::string& name) : m_name(name), m_leof(0), m_indexUpToDate(false) {
            m_file.open(m_name.c_str(), ios::in | ios::binary );
    }
    bool dump(const std::string& name,ostream& os) {
        const map<Gaudi::StringKey,Info>& myIndex = getIndex();
        map<Gaudi::StringKey,Info>::const_iterator i = myIndex.find(name);
        if (i!=myIndex.end()) {
            // slice works relative to the current file offset, as it works on an istream...
            bio::seek(m_file,0,std::ios_base::beg);
            // suggest an 8K buffer size as hint -- most config items are smaller than that...
            bio::copy(bio::slice(m_file,i->second.offset,i->second.size), os, 8192);
            return true;
        }
        // try to read a gzipped version of the filename
        i = myIndex.find(name+".gz");
        if (i!=myIndex.end()) {
            // slice works relative to the current file offset, as it works on an istream...
            bio::seek(m_file,0,std::ios_base::beg);
            bio::filtering_istream in;
            in.push(bio::gzip_decompressor());
            in.push(bio::slice(m_file,i->second.offset,i->second.size));
            // suggest an 8K buffer size as hint -- most config items are smaller than that...
            bio::copy(in, os, 8192);
            return true;
        }
        return false;
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

    ~TarFile() {
            m_file.close();
    }
private:
    struct Info {
        Info() : size(0),type( TarFileType(0) ),offset(0) {}
        std::string      name;
        size_t           size;
        TarFileType      type;
        size_t           offset;
    };

    bool index() const;
    const std::map<Gaudi::StringKey,Info>&  getIndex() const {
        if (!m_indexUpToDate) m_indexUpToDate = index();
        if (!m_indexUpToDate) {
            throw GaudiException("Failed to index tarfile ",m_name,StatusCode::FAILURE);
        }
        return m_index;
    }
    bool interpretHeader(posix_header& header, struct Info& info) const;
    /* Read an octal value in a field of the specified width, with optional
     * spaces on both sides of the number and with an optional null character
     * at the end.  Returns -1 on an illegal format.  */
    bool isOctal(const char& ch) const   { return ((ch >= '0') && (ch <= '7')); }
    long getOctal(const char *cp, int size) const
    {
       long val = 0;
       for(;(size > 0) && (*cp == ' '); cp++, size--);
       if ((size == 0) || !isOctal(*cp)) return -1;
       for(; (size > 0) && isOctal(*cp); size--) val = val * 8 + *cp++ - '0';
       for (;(size > 0) && (*cp == ' '); cp++, size--);
       if ((size > 0) && *cp) return -1;
       return val;
    }

    std::string m_name;
    mutable ifstream m_file;
    mutable std::map<Gaudi::StringKey,Info> m_index;
    mutable unsigned long m_leof;
    mutable bool m_indexUpToDate;
};

bool TarFile::interpretHeader(posix_header& header, Info& info) const {
            if (strncmp(header.magic,"ustar ",6)&&strncmp(header.magic,"ustar\0",6)) { 
                return false;
            }
            long chksum       = getOctal(header.chksum,sizeof(header.chksum));
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
            if (sum!=chksum) return false;
            info.type  = TarFileType(header.typeflag);
            info.size  = getOctal(header.size,sizeof(header.size));
            if (info.size<0) {
                std::cerr << " got negative file size: " << info.size << std::endl;
                return false;
            }
            info.name  = convert(header.name);
            if (header.prefix[0]!=0) {
                std::string prefix = convert(header.prefix);
                info.name = prefix + "/" + info.name;
            }
            info.offset = m_file.tellg();

            if ( info.type == GNUTYPE_LONGNAME ) { 
                // current header is a dummy 'flag', followed by data blocks 
                // with name as content (length of name is 'size' of data)
                assert(info.name == "././@LongLink");
                // first read the real, untruncated name as data
                std::ostringstream fname;
                bio::copy(bio::slice(m_file,0,info.size), fname);
                size_t padding = info.size % 512;
                if (padding!=0) bio::seek(m_file,512-padding,std::ios::cur);
                // and now get another header, which contains a truncated name
                // but which is otherwise the 'real' one
                m_file.read( (char*) &header, sizeof(header) ) ;
                if (!interpretHeader( header, info)) return false;
                // so we overwrite the truncated one with the long one...
                info.name = fname.str();
            }
            return true;
}

bool TarFile::index() const {
        posix_header header;
        bio::seek(m_file,0,std::ios::beg);
        m_index.clear();
        while (m_file.read( (char*) &header, sizeof(header) )) {
            std::streamoff offset = m_file.tellg()-std::streamoff(sizeof(header));
            Info info;
            if (!interpretHeader( header, info))  {
                // see if we're at the end of the file: (at least) two all-zero headers)
                if (isZero(header)) {
                    m_file.read( (char*) &header, sizeof(header) ) ;
                    if (isZero(header)){
                        m_leof = info.offset;
                        bio::seek(m_file,0,std::ios::beg);
                        return true;
                    }
                }
                std::cerr << "failed to interpret header @ " << offset << " in tarfile " << m_name << std::endl;
                bio::seek(m_file,0,std::ios::beg);
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
                m_index.insert(make_pair(Gaudi::StringKey(info.name),info));
            }
            // round up size to block size, and skip to next header...
            size_t skip = info.size;
            size_t padding = skip % 512;
            if (padding!=0 ) skip += 512 - padding;
            bio::seek(m_file,skip,std::ios::cur);
        }
        bio::seek(m_file,0,std::ios::beg);
        return true;
};
}

using namespace ConfigTarFileAccessSvc_details;

// Factory implementation
DECLARE_SERVICE_FACTORY(ConfigTarFileAccessSvc)

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ConfigTarFileAccessSvc::ConfigTarFileAccessSvc( const std::string& name, ISvcLocator* pSvcLocator)
  : Service ( name , pSvcLocator )
{
  std::string def( System::getEnv("HLTTCKROOT") );
  if (!def.empty()) def += "/config.tar";
  declareProperty("File", m_name = def);
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
  info() << "Initialize" << endmsg;
  StatusCode status = Service::initialize();
  if ( !status.isSuccess() )   return status;
  status = setProperties();

  m_file.reset( new TarFile(m_name) );
  info() << " opened " << m_name << endmsg;

  return status;
}

//=============================================================================
// Finalization
//=============================================================================
StatusCode ConfigTarFileAccessSvc::finalize() {
  m_file.reset(0);  // (TarFile*)0);
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
   debug() << "trying to read " << path << endmsg;
   if (!m_file->dump(path,content)) {
        debug() << "file " << path << " not found" << endmsg;
        return boost::optional<T>();
   }
   T c;
   content >> c;
   return c;

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
   if (!m_file->dump(fnam,content)) {
        debug() << "file " << fnam << " does not exist" << endmsg;
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
    std::vector<std::string> aliases = m_file->files( PrefixFilenameSelector(basename+"/"+alias.major()) );
     
    for (std::vector<std::string>::const_iterator i  = aliases.begin(); i!=aliases.end(); ++i ) { 
              //TODO: this can be more efficient...
              debug() << " configTreeNodeAliases: adding file " << *i << endmsg;
              std::string ref;
              stringstream content(stringstream::in | stringstream::out);
              m_file->dump(*i,content);
              content >> ref;
              std::string alias = i->substr( basename.size()+1 );
              std::stringstream str; 
              str << "Ref: " << ref << '\n' << "Alias: " << alias << std::endl;
              ConfigTreeNodeAlias a;
              str >> a;
              debug() << " configTreeNodeAliases: content:" << a << endmsg;
              x.push_back(a);
    }
    return x;
}

PropertyConfig::digest_type
ConfigTarFileAccessSvc::writePropertyConfig(const PropertyConfig& ) {
   // write is _NOT_ supported to tar files...
   error() <<" Writing to tar files is not supported " << endmsg;
   return PropertyConfig::digest_type::createInvalid();
}

ConfigTreeNode::digest_type
ConfigTarFileAccessSvc::writeConfigTreeNode(const ConfigTreeNode& ) {
   // write is _NOT_ supported to tar files...
   error() <<" Writing to tar files is not supported " << endmsg;
   return ConfigTreeNode::digest_type::createInvalid();
}

ConfigTreeNodeAlias::alias_type 
ConfigTarFileAccessSvc::writeConfigTreeNodeAlias(const ConfigTreeNodeAlias& ) {
   // write is _NOT_ supported to tar files...
   error() <<" Writing to tar files is not supported " << endmsg;
   return ConfigTreeNodeAlias::alias_type();
}

MsgStream& ConfigTarFileAccessSvc::msg(MSG::Level level) const {
     if (m_msg.get()==0) m_msg.reset( new MsgStream( msgSvc(), name() ));
     return *m_msg << level;
}
