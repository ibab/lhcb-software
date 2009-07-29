#include "TarFileAccess.h"

#include "boost/assign/list_of.hpp"
#include "boost/regex.hpp"
#include <fstream>
#include <sstream>

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


namespace TarFileAccess_details {
    class TarFile {
    public:
        TarFile(const std::string& name) : m_name(name) {
            m_file.open(m_name.c_str(), std::ios::in | std::ios::binary );
            index();
        }
        bool fillStream(const std::string& name,std::ostream& os) {
            std::map<std::string,Info>::const_iterator i = m_index.find(name);
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
            for (std::map<std::string,Info>::const_iterator i = m_index.begin(); i!= m_index.end();++i) {
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
           for ( ; (size > 0) && (*cp == ' ') ; cp++, size--) ;
           if ((size == 0) || !is_octal(*cp)) return -1;
           for ( ; (size > 0) && is_octal(*cp) ; size--) val = val * 8 + *cp++ - '0' ;
           for ( ; (size > 0) && (*cp == ' ') ; cp++, size--) ;
           if ((size > 0) && *cp) return -1;
           return val;
        }

        std::string m_name;
        std::ifstream m_file;
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
                // skip directories and CVS (sub)trees...
                if ( (info.type == REGTYPE || info.type == REGTYPE0 ) 
                   && info.name[ info.name.size()-1 ] != '/' 
                   && info.name.find("/CVS/") == std::string::npos )  {
                    m_index.insert(make_pair(info.name,info));
                }
                // round up size to block size, and skip to next header...
                size_t skip = info.size;
                size_t padding = skip % 512;
                if (padding!=0 ) skip += 512 - padding;
                m_file.seekg(skip,std::ios::cur);
            }
            m_file.seekg(0, std::ios::beg);
            return true;
    };
}

//=============================================================================
// implementation of TarFileAccess
//=============================================================================
TarFileAccess::TarFileAccess() {}

TarFileAccess::~TarFileAccess() {
    for (container_t::iterator i = m_tarFiles.begin(); i!=m_tarFiles.end();++i) {
        delete i->second; // this closes the files
    }
    m_tarFiles.clear();
}


const std::vector<std::string>&
TarFileAccess::protocols() const {
    static const std::vector<std::string> s_proto = boost::assign::list_of( std::string("TarFile") )
                                                                          ( std::string("Tarfile") )
                                                                          ( std::string("tarfile") )
                                                                          ( std::string("tarFile") );
    return s_proto;
}

std::auto_ptr<std::istream> 
TarFileAccess::open(const std::string &url) {
    std::auto_ptr<std::istream> stream;
    // parse tarfile:/foo/bar/xxx.tar/some/file 
    //                                ^       ^ file name inside tar file
    //               ^              ^ tar name
    static boost::regex re("^[tT]ar[fF]ile:(.*.tar)/(.*)$");
    boost::smatch what;
    if(!boost::regex_match(url, what, re)) {
        return stream;
    }
    // check if tarname already known; if not, open and index
    container_t::iterator tarFile =  m_tarFiles.find(what.str(1));
    if (tarFile == m_tarFiles.end() ) { 
        tarFile = m_tarFiles.insert( tarFile, make_pair( what.str(1), new TarFileAccess_details::TarFile(what.str(1)) ) );
        // TODO: keep access statistics on open files,
        // and if too many, close the oldest
        // also, maybe we want to move the most recently used
        // item to the front, and start to search from there...
        // then we can drop things at the back if we become
        // too long...
    }

    // if exists, dump into stringstream
    // TODO: use streambuf to implement zero-copy.. but beware of parent lifetime!
    std::stringstream *ss = new std::stringstream(std::stringstream::in|std::stringstream::out);
    stream.reset(ss);
    if (!tarFile->second->fillStream( what.str(2), *ss)) stream.reset(0);
    return stream;
}
