#include "ConfigTarFileAccessSvc.h"
#include <sstream>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
using namespace std;

#include "boost/lexical_cast.hpp"
#include "boost/filesystem/path.hpp"
namespace fs = boost::filesystem;

#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/System.h"


namespace {

    struct DefaultFilenameSelector {
        bool operator()(const std::string& fname) const { return true; }
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


/* A nice enum with all the possible tar file content types */
enum TarFileType 
{
    REGTYPE  = '0',            /* regular file */
    REGTYPE0 = '\0',           /* regular file (ancient bug compat)*/
};
typedef enum TarFileType TarFileType;

}


namespace ConfigTarFileAccessSvc_details {
class TarFile {
public:
    TarFile(const std::string& name) : m_name(name) {
            m_file.open(m_name.c_str(), ios::in | ios::binary );
            index();
    }
    bool dump(const std::string& name,ostream& os) {
        map<string,Info>::const_iterator i = m_index.find(name);
        if (i==m_index.end()) return false;
        m_file.seekg(i->second.offset);
        static std::vector<char> buf;
        buf.resize(i->second.size);
        m_file.read(&buf[0],buf.size());
        for (std::vector<char>::const_iterator j=buf.begin();j!=buf.end();++j) {
            os << *j ;
        }
        return true;
    }

    template <typename SELECTOR>
    std::vector<std::string> files(const SELECTOR& selector) const {
        std::vector<std::string> f;
        for (map<string,Info>::const_iterator i = m_index.begin(); i!= m_index.end();++i) {
            if( selector(i->first) ) { 
                f.push_back(i->first);
            }
        }
        return f;
    }
    std::vector<std::string> files() const { return files(DefaultFilenameSelector()); }

    ~TarFile() {
            m_file.close();
    }
private:
    struct Info {
        Info(size_t _offset) : size(0),type( TarFileType(0) ),offset(_offset) {}
        std::string      name;
        size_t           size;
        TarFileType      type;
        size_t           offset;
    };

    bool index();
    bool interpretHeader(const posix_header& header, struct Info& info);
    /* Read an octal value in a field of the specified width, with optional
     * spaces on both sides of the number and with an optional null character
     * at the end.  Returns -1 on an illegal format.  */
    bool is_octal(const char& ch)   { return ((ch >= '0') && (ch <= '7')); }
    long getOctal(const char *cp, int size)
    {
       long val = 0;
       for(;(size > 0) && (*cp == ' '); cp++, size--);
       if ((size == 0) || !is_octal(*cp)) return -1;
       for(; (size > 0) && is_octal(*cp); size--) val = val * 8 + *cp++ - '0';
       for (;(size > 0) && (*cp == ' '); cp++, size--);
       if ((size > 0) && *cp) return -1;
       return val;
    }

    std::string m_name;
    ifstream m_file;
    std::map<std::string,Info> m_index;
};

bool TarFile::interpretHeader(const posix_header& header, Info& info) {
            if (!strncmp(header.magic,"ustar",6)) return false;
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
            info.name  = header.name;
            info.type  = TarFileType(header.typeflag);
            info.size  = getOctal(header.size,sizeof(header.size));
            return true;
}

bool TarFile::index() {
        posix_header header;
        while (m_file.read( (char*) &header, sizeof(header) )) {
            Info info(m_file.tellg());
            if (!interpretHeader( header, info))  return false;
            if (info.name.empty()) break;
            if ( (info.type == REGTYPE || info.type == REGTYPE0 ) 
               && info.name[ info.name.size()-1 ] != '/' 
               && info.name.find("/CVS/") == string::npos )  {
                m_index.insert(make_pair(info.name,info));
            }
            // round up size to block size, and skip to next header...
            size_t skip = info.size;
            size_t padding = skip % 512;
            if (padding!=0 ) skip += 512 - padding;
            m_file.seekg(skip,ios::cur);
        }
        m_file.seekg(0, ios::beg);
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
