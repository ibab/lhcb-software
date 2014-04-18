//// TODO: use http://www.boost.org/doc/libs/1_55_0/libs/utility/doc/html/string_ref.html where ever possible!!!
//// TODO: fold class CDB back into ConfigCDBAccessSvc...
#include <iostream>
#include <string>
#include <map>
#include <set>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include "boost/iostreams/filter/zlib.hpp"
#include "boost/iostreams/filter/bzip2.hpp"
#include "boost/iostreams/filtering_stream.hpp"
#include "boost/iostreams/device/array.hpp"
#include "boost/iostreams/stream.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/classification.hpp"

#include "../src/cdb.h"

namespace io = boost::iostreams;

struct manifest_entry {
    template <typename TCKS, typename COMMENT> 
    manifest_entry( std::string toplevel, const TCKS& tcks, const COMMENT& comments ) 
    {
        std::vector<std::string> tokens;
        boost::algorithm::split(tokens,toplevel,boost::algorithm::is_any_of("/"));
        assert(tokens.size()==3);
        release = tokens[0];
        type = tokens[1];
        id = tokens[2];
        auto itck = tcks.find(id);
        if (itck!=std::end(tcks)) tck = itck->second;
        auto ilab = comments.find(id);
        if (ilab!=std::end(comments))  comment = ilab->second;
    }
    std::string release;
    std::string type;
    std::string tck;
    std::string id;
    std::string comment;

    bool operator<(const manifest_entry& rhs) const {
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
            static char mbstr[100];
            mbstr[0]=0;
            std::strftime(mbstr, sizeof(mbstr), "%A %c", std::localtime(&t));
            return { mbstr };
}

class CDB {
public:
    CDB( std::string fname, int mode ) {
        int fd = open( fname.c_str(), mode );
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
        struct cdb* m_db = nullptr;
        unsigned m_cpos  = 0;
        bool atEnd() const { return !m_db; }
        void next() { 
            if (m_db && !( cdb_seqnext(&m_cpos, m_db) > 0 ) ) m_db = nullptr ;
        }
    public:
        iterator(struct cdb *parent = nullptr) : m_db{parent} {
            if (m_db) cdb_seqinit(&m_cpos, m_db);
            next();
        }
        iterator& operator++() { next(); return *this; }
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
        return { cdb_find(&m_db, key.c_str(), key.size())>0 ? &m_db  : nullptr };
    }

    bool ok() const { return cdb_fileno(&m_db)>=0; }
private:
    mutable struct cdb m_db;
};



void dump_manifest(CDB& db) {
    std::multiset< manifest_entry > manifest;
    std::map< std::string , std::string > tck;     // id -> TCK
    std::map< std::string , std::string > comment; // id -> comment

    // first: get TCK & labels
    for ( auto record : db )  { // TODO: allow loop with predicate on key...
        auto key = record.key() ; 
        if (key.compare(0,6,"AL/TCK")!=0) continue;
        auto id  = record.value();
        tck.emplace( id, key.substr(7));
        key = "TN/";  key+=id;
        auto i = db.find( key );
        if ( i != std::end(db) ) {
            auto value = (*i).value();
            if (value.compare(0,6,"Label:")==0) {
                   auto x = value.find('\n',6);
                   comment.emplace(id, value.substr(6, x!=std::string::npos ? x-6 : x ));
            }
        }
    }
    // next: create manifest
    for ( auto record : db ) { // TODO: allow loop with predicate on key...
        auto key = record.key();
        if (key.compare(0,11,"AL/TOPLEVEL")!=0) continue;
        manifest.emplace( key.substr(12), tck, comment );
    }

    for (auto& m : manifest ) std::cout << m <<   std::endl;
}

void dump_keys(CDB& db) {
    for ( auto record : db ) {
        std::cout << record.uid() << "     " << format_time( record.time() ) << "     "  << record.key() << "\n";
    }
    std::cout << std::flush;
}

void dump_records(CDB& db) {
    for ( auto record : db ) {
        std::cout << record.uid() << "     " << format_time( record.time() ) << "     "  << record.key() << "\n";
        std::cout << record.value() <<"\n";
    }
    std::cout << std::flush;
}

int main(int argc, char* argv[]) {
    // TODO: add cmdline arguments for database, Moore version...

    if (argc<2) return 0;
    CDB db(argv[1],O_RDONLY );
    if (!db.ok()) return 1;

    // dump_manifest(db);
    // dump_keys(db);
    dump_records(db);

    return 0;
}

// TODO: add method for config.tar -> config.cdb conversion, which maintains timestamp, uid, order...
