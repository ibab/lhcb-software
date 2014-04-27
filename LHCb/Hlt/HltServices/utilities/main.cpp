//// TODO: use http://www.boost.org/doc/libs/1_55_0/libs/utility/doc/html/string_ref.html where ever possible!!!
//// TODO: fold class CDB back into ConfigCDBAccessSvc...
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "boost/iostreams/filter/zlib.hpp"
#include "boost/iostreams/filter/bzip2.hpp"
#include "boost/iostreams/filtering_stream.hpp"
#include "boost/iostreams/device/array.hpp"
#include "boost/iostreams/stream.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"
#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"
#include "boost/filesystem/fstream.hpp"

#include "../src/cdb.h"

namespace io = boost::iostreams;
namespace fs = boost::filesystem;


struct manifest_entry {
    template <typename TCKS > 
    manifest_entry( std::string toplevel, const TCKS& tcks, std::string com) 
    {
        std::vector<std::string> tokens;
        boost::algorithm::split(tokens,toplevel,boost::algorithm::is_any_of("/"));
        assert(tokens.size()==3);
        release = tokens[0];
        type = tokens[1];
        id = tokens[2];
        auto itck = tcks.find(id);
        tck = (itck!=std::end(tcks)) ? itck->second : "<NONE>" ;
        comment = com;
    }
    std::string release;
    std::string type;
    std::string tck;
    std::string id;
    std::string comment;

    bool operator<(const manifest_entry& rhs) const {
        // can we get MOORE_v9r1 prior to MOORE_v10r1 ???
        // if the string contains a digit at the point where they are different,
        // do a numerical <
        return release < rhs.release || 
              ( release == rhs.release && ( type < rhs.type || 
              ( type == rhs.type && ( tck < rhs.tck || 
              ( tck == rhs.tck && ( id < rhs.id || 
              ( comment < rhs.comment )))))));
    }
};

std::ostream& operator<<(std::ostream& os, const manifest_entry& e) {
    return os << e.release << " : " << e.type << " : " << e.tck << " : " << e.id << " : " << e.comment ;
}


std::string format_time(std::time_t t) {
            // gcc4.8 doesn't have std::put_time???
            static char mbstr[100];
            mbstr[0]=0;
            std::strftime(mbstr, sizeof(mbstr), "%A %c", std::localtime(&t));
            return { mbstr };
}

class CDB {
    mutable struct cdb m_db;
public:
    CDB( const std::string& fname ) {
        int fd = open( fname.c_str(), O_RDONLY );
        if (cdb_init( &m_db, fd ) < 0) cdb_fileno(&m_db)=-1;
    }
    ~CDB() {
        auto fd = cdb_fileno(&m_db);
        if (fd>=0) close(fd);
        cdb_free(&m_db);
    }

    class record {
        const void *m_kpos,*m_vpos;
        unsigned m_klen, m_vlen;
    public:
        record( const void *kpos, const void *vpos, unsigned klen, unsigned vlen) 
          : m_kpos{kpos}, m_vpos{vpos}, m_klen{klen}, m_vlen{vlen} { 
            assert(vlen>=12);
        }
        unsigned int valuePersistentSize() const { return m_vlen;}
        std::string key() const {
            return { static_cast<const char*>(m_kpos), m_klen };
        }
        unsigned int flags() const {
            return  *(static_cast<const unsigned char*>(m_vpos)+1);
        }
        uid_t uid() const {
            const unsigned char *base = static_cast<const unsigned char*>(m_vpos);
            auto f = [=](unsigned int i, unsigned int j) { return  uid_t( base[i] ) <<j; };
            return  f(4,0) | f(5,8) | f(6,16) | f(7,24) ;
        }
        std::time_t time() const {
            const unsigned char *base = static_cast<const unsigned char*>(m_vpos);
            auto f = [=](unsigned int i, unsigned int j) { return  std::time_t( base[i] ) <<j; };
            return  f(8,0) | f(9,8) | f(10,16) | f(11,24) ;
        }
        std::string value() const {
            io::stream<io::array_source> buffer(static_cast<const char*>(m_vpos)+12,m_vlen-12);
            io::filtering_istream s;
            switch ( flags() & 0x3  ) {
                    case 0 : break ; // do nothing...
                    case 2 : s.push(io::bzip2_decompressor()); break;
                    case 3 : { io::zlib_params params; params.noheader = true;
                               s.push(io::zlib_decompressor(params)); } break;
                    default : 
                            std::cerr << "unknown compression flag" << std::endl;
                            return 0;
            }
            s.push(buffer);
            std::string value;
            std::copy(std::istreambuf_iterator<char>(s), std::istreambuf_iterator<char>(), std::back_inserter(value));
            return value;
        }
    };

    class iterator {
        struct cdb* m_db;
        unsigned m_cpos;
        bool atEnd() const { return !m_db; }
    public:
        iterator(struct cdb *parent = nullptr, unsigned cpos = 0) : m_db{parent}, m_cpos(cpos) {
            if ( m_db && !m_cpos ) cdb_seqinit(&m_cpos, m_db);
            ++*this;
        }
        iterator& operator++() { 
            if (m_db && !( cdb_seqnext(&m_cpos, m_db) > 0 ) ) m_db = nullptr ;
            return *this; 
        }
        bool operator==(const iterator& rhs) const {
            return atEnd() ? rhs.atEnd() : m_cpos == rhs.m_cpos;
        }
        bool operator!=(const iterator& rhs) const {
            return atEnd() ? !rhs.atEnd() : m_cpos != rhs.m_cpos;
        }
        record operator*() const { // Not quite canonical -- ideally should be record&...
            return { cdb_getkey(m_db), cdb_getdata(m_db), cdb_keylen(m_db), cdb_datalen(m_db) };
        }
    };
    
    iterator begin() const { return { &m_db  }; }
    iterator end()   const { return {        }; }

    iterator find(const std::string& key) const { 
        if ( cdb_find(&m_db, key.c_str(), key.size())>0 ) {
            // Hrmpf. Use inside knowledge of the layout of the cdb structure... 
            //        ... so that 'next' will do the right thing...
            unsigned int offset = std::distance(m_db.cdb_mem,static_cast<const unsigned char*>(cdb_getkey(&m_db)));
            return { &m_db, offset-8 };
        }
        return end();
    }

    bool ok() const { return cdb_fileno(&m_db)>=0; }
};



void dump_manifest(CDB& db) {
    std::multiset< manifest_entry > manifest;

    // first: get TCK -> id map, and invert.
    // needed so that we can make immutable manifest_entries later...
    std::map< std::string , std::string > tck;     // id -> TCK
    for ( auto record : db )  { // TODO: allow loop with predicate on key...
        auto key = record.key() ; 
        if (key.compare(0,6,"AL/TCK")!=0) continue;
        tck.emplace( record.value() , key.substr(7));
    }
    // next: create manifest
    for ( auto record : db ) { // TODO: allow loop with predicate on key... 
        auto key = record.key();
        if (key.compare(0,11,"AL/TOPLEVEL")!=0) continue;
        // the 'comment' is in the 'label' member of the treenode this alias "points" at
        std::string comment;
        auto i = db.find( std::string("TN/")+key.substr(key.rfind("/")+1) );
        if ( i != std::end(db) ) {
            auto value = (*i).value();
            if (value.compare(0,6,"Label:")==0) {
                   auto x = value.find('\n',6);
                   comment = value.substr(6, x!=std::string::npos ? x-6 : x );
            }
        }
        manifest.emplace( key.substr(12), tck, comment );
    }
    for (auto& m : manifest ) std::cout << m <<   std::endl;
}

void dump_keys(CDB& db) {
    for ( auto record : db ) {
        std::cout << record.uid() << "     " << record.valuePersistentSize()  << "      " << format_time( record.time() ) << "     "  << record.key() << "\n";
    }
    std::cout << std::flush;
}

void dump_records(CDB& db) {
    for ( auto record : db ) {
        std::cout << record.uid() << "     " << record.valuePersistentSize()  << "      "<< format_time( record.time() ) << "     "  << record.key() << "\n";
        std::cout << record.value() <<"\n";
    }
    std::cout << std::flush;
}

void extract_records(CDB& db) {
    struct timespec t[2]; t[0].tv_nsec = 0; t[1].tv_nsec = 0;
    for ( auto record : db ) {
        auto key = record.key();
        if (key.compare(0,3,"TN/")==0)        key = std::string("ConfigTreeNodes/") + key.substr(3,2) + "/" + key.substr(3);
        else if (key.compare(0,3,"PC/")==0 )  key = std::string("PropertyConfigs/") + key.substr(3,2) + "/" + key.substr(3);
        else if (key.compare(0,3,"AL/")==0 )  key = std::string("Aliases/") + key.substr(3);
        std::cout <<  " extracting    " << record.key() << " -> " << key << std::endl;
        fs::path fname( key );
        fs::create_directories( fname.parent_path() );
        fs::ofstream fn( fname );
        fn << record.value();
        fn.close();
        t[0].tv_sec = record.time();
        t[1].tv_sec = record.time();
        utimensat( AT_FDCWD,  fname.c_str(), t, 0 );
    }
}

namespace po = boost::program_options;

int main(int argc, char* argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options() ("list-manifest", "dump manifest ")
                       ("list-keys", "list keys")
                       ("list-records", "list keys and records")
                       ("extract-records", "extract records")
                       ("input-file",  po::value<std::string>()->default_value(std::string("config.cdb")),"input file");
    po::positional_options_description p;
    p.add("input-file", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);

    
    CDB db(vm["input-file"].as<std::string>() );
    if (!db.ok()) return 1;

    if (vm.count("list-manifest")) dump_manifest(db);
    if (vm.count("list-keys"))     dump_keys(db);
    if (vm.count("list-records"))  dump_records(db);
    if (vm.count("extract-records"))  extract_records(db);

    return 0;
}

// TODO: add method for config.tar -> config.cdb conversion, which maintains timestamp, uid, order...
