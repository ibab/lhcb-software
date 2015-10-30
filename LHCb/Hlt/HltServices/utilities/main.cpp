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
#include "boost/iostreams/filter/gzip.hpp"
#include "boost/iostreams/filtering_stream.hpp"
#include "boost/iostreams/device/array.hpp"
#include "boost/iostreams/slice.hpp"
#include "boost/iostreams/stream.hpp"
#include "boost/iostreams/copy.hpp"
#include "boost/algorithm/string/split.hpp"
#include "boost/algorithm/string/join.hpp"
#include "boost/algorithm/string/classification.hpp"
#include "boost/program_options.hpp"
#include "boost/filesystem.hpp"
#include "boost/filesystem/fstream.hpp"
#include "boost/algorithm/string/predicate.hpp"
#include "boost/io/detail/quoted_manip.hpp"

#include "Kernel/ConfigTreeNode.h"
#include "Kernel/PropertyConfig.h"

template <typename T> std::string convert(const std::string& s) {
    std::istringstream in(s);
    std::ostringstream out;
    T t; in >> t; out << t;
    return out.str();
}

constexpr struct to_json_t {
    std::string operator()(const std::string& s) const;
    using r2t_t = std::map<std::string,std::string>;
    std::string operator()(const std::tuple<std::string,r2t_t,std::string>& e) const;

    template <typename K, typename V> std::string operator()(const std::pair<K,V>& p) const ;
    template <typename V> std::string operator()(const std::map<std::string,V>& m) const ;
} to_json {};

std::string to_json_t::operator()(const std::string& s) const {
    // TODO: replace with C++14 std::quoted
    std::ostringstream out;
    out << boost::io::quoted(s);
    return out.str();
}

template <typename K, typename V>
std::string to_json_t::operator()(const std::pair<K,V>& p) const {
    return to_json(p.first)+":"+to_json(p.second);
}

template <typename V>
std::string to_json_t::operator()(const std::map<std::string,V>& m) const {
    std::vector<std::string> buf; buf.reserve( m.size() );
    std::transform( std::begin(m), std::end(m), std::back_inserter(buf), to_json );
    return std::string{"{"} + boost::algorithm::join(buf,",") + "}";
}

std::string to_json_t::operator()(const std::tuple<std::string,to_json_t::r2t_t,std::string>& e) const {
    return std::string{"{"}+to_json(std::make_pair("TCK",std::get<0>(e)))
                     + "," +to_json(std::make_pair("Release2Type",std::get<1>(e)))
                     + "," +to_json(std::make_pair("label",std::get<2>(e)))
                     + "}";
}

#include "../src/cdb.h"
#include "../src/tar.h"

namespace io = boost::iostreams;
namespace fs = boost::filesystem;

int compress( std::string& str )
{
    // compress and check if worthwhile...
    std::string compressed;
    compressed.reserve( str.size() );
    io::filtering_streambuf<io::output> filter;
    io::zlib_params params;
    params.noheader = true;
    filter.push( io::zlib_compressor( params ) );
    filter.push( io::back_inserter( compressed ) );
    io::copy( boost::make_iterator_range( str ), filter, 8192 );
    bool ok = compressed.size() < str.size(); // yes, it's better!
    if ( ok ) str.swap( compressed );
    return ok ? 3 : 0;
}

std::vector<unsigned char> make_cdb_record( std::string str, uid_t uid, std::time_t t )
{
    auto flags = compress( str );
    std::vector<unsigned char> buffer( 12 + str.size(), 0 );
    auto buf = std::begin( buffer );
    *buf++ = 0;     // version number
    *buf++ = flags; // compression
    *buf++ = 0;     // reserved;
    *buf++ = 0;     // reserved;
    assert( sizeof( uid_t ) == 4 );
    *buf++ = ( uid & 0xff );
    *buf++ = ( ( uid >> 8 ) & 0xff );
    *buf++ = ( ( uid >> 16 ) & 0xff );
    *buf++ = ( ( uid >> 24 ) & 0xff );
    assert( sizeof( t ) == 4 ); // seconds since 1970  ( ok for 136 years )
    *buf++ = ( t & 0xff );
    *buf++ = ( ( t >> 8 ) & 0xff );
    *buf++ = ( ( t >> 16 ) & 0xff );
    *buf++ = ( ( t >> 24 ) & 0xff );
    if ( std::distance( std::begin( buffer ), buf ) != 12 ) {
        std::cerr << "ERROR" << std::endl;
    }
    auto e = std::copy_n( std::begin( str ), str.size(), buf );
    if ( e != std::end( buffer ) ) {
        std::cerr << "ERROR" << std::endl;
    }
    return buffer;
}


struct manifest_entry {
    template <typename TCKS > 
    manifest_entry(const std::string& toplevel, const TCKS& tcks, std::string com) 
    {
        std::vector<std::string> tokens;
        boost::algorithm::split(tokens,toplevel,boost::algorithm::is_any_of("/"));
        assert(tokens.size()==3);
        release = tokens[0];
        type = tokens[1];
        id = tokens[2];
        auto itck = tcks.find(id);
        tck = (itck!=std::end(tcks)) ? itck->second : "<NONE>" ;
        comment = std::move(com);
    }
    std::string release,type,tck,id,comment;

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

class TAR {
    std::fstream m_file;
    std::map<std::string,std::streamoff> m_index;
public:

    TAR(const std::string& fname) {
        m_file.open( fname.c_str(), std::ios::in | std::ios::binary );
    }

    bool ok() const { return m_file.good(); }

    class record {
        ConfigTarFileAccessSvc_details::Info m_info;
        std::fstream* m_file;
    public:
        record() = default;
        record( std::fstream* file, ConfigTarFileAccessSvc_details::Info info ) : m_info(std::move(info)), m_file(file) {};
        uid_t uid() const { return m_info.uid; }
        std::time_t time() const { return m_info.mtime; }
        const std::string& key() const { return m_info.name; }
        std::string value() const { 
            m_file->seekg( 0, std::ios_base::beg );
            io::filtering_istream s;
            if ( m_info.compressed ) s.push( io::gzip_decompressor() );
            s.push( io::slice( *m_file, m_info.offset, m_info.size ) );
            std::string value;
            std::copy(std::istreambuf_iterator<char>(s), std::istreambuf_iterator<char>(), std::back_inserter(value));
            return value;
        }
        unsigned int valuePersistentSize() const { return m_info.size; }
        bool isTCK() { return key().size()==22 && key().compare(0,12,"Aliases/TCK/")==0 ; }
        bool isTopLevel() { return key().back()!='/' && key().compare(0,17,"Aliases/TOPLEVEL/")==0; }
        std::string TCK() { return isTCK() ? key().substr(12) : std::string{} ; }
        std::string topLevel() { return isTopLevel() ? key().substr(17) : std::string{}; }

    };

    class iterator {
        friend class TAR;
        std::fstream*  m_file;
        std::streamoff m_pos;

        ConfigTarFileAccessSvc_details::Info read_info() { // WARNING: advances the position in the file to just after the header...
            ConfigTarFileAccessSvc_details::Info info;
            if (!m_file) return info;
            m_file->seekg( m_pos, std::ios::beg );
            static ConfigTarFileAccessSvc_details::posix_header header;
            if (!m_file->read( (char*)&header, sizeof( header ) )) {
                m_file = nullptr;
                std::cerr << "unexpected EOF" << std::endl;
            }
            if ( !interpretHeader(*m_file, header, info ) ) {
                // check whether we're at the end of the file: (at least) two all-zero
                // headers)
                if ( !isZero( header ) ) { 
                    std::cerr << "failed to interpret header" << std::endl;
                } else {
                    m_file->read( (char*)&header, sizeof( header ) );
                    if ( !isZero( header ) ) {
                        std::cerr << "missing 2nd zero header " << std::endl;
                    }
                }
                m_file = nullptr;
            }
            return info;
        }
        void next() {
            if (!m_file) return;
            auto info = read_info();  // read current header, and position at the end of it...
            if (!m_file) return;
            size_t skip = info.size;
            size_t padding = skip % 512;
            if ( padding != 0 ) skip += 512 - padding;
            m_pos = m_file->tellg(); // read_info put us at the end of the CURRENT header
            m_pos += skip;           // m_pos now points at the start of the NEXT header...
        }
        bool is_current_valid() {
            auto info = read_info();
            return m_file && !info.name.empty() && info.name.back()!='/';
        }
        void skip_invalid() {
            while (m_file && !is_current_valid()) next();
        }
    public:
        iterator( ) : m_file{ nullptr }, m_pos{0}  { }
        iterator( std::fstream* file, std::streamoff pos = 0 ) : m_file{file}, m_pos{pos} { 
            skip_invalid();
        }

        record operator*() { // Not quite canonical -- ideally should be record&... and const
            return { m_file, read_info() };
        }
        iterator& operator++() { 
            next(); skip_invalid();
            return *this; 
        }

        friend bool operator==( const iterator& lhs, const iterator& rhs) {
             return lhs.m_file == rhs.m_file && ( !lhs.m_file || lhs.m_pos == rhs.m_pos );
        }
        friend bool operator!=( const iterator& lhs, const iterator& rhs) {
             return lhs.m_file != rhs.m_file || ( lhs.m_file && lhs.m_pos != rhs.m_pos );
        }
    };

    iterator begin() { return { &m_file }; }
    iterator end() const { return { }; }

    iterator find(const std::string& key) { 
         if (m_index.empty()) index();
         auto i = m_index.find(key);
         if (i==std::end(m_index)) return {};
         return { &m_file, i->second };
    }

    iterator findTreeNode(const std::string& hash) {
        return find( std::string("ConfigTreeNodes/")+hash.substr(0,2)+"/"+hash );
    }

    void index() {
        for ( auto i = begin(), e = end(); i!=e ; ++i) {
            m_index.emplace( (*i).key(), i.m_pos );
        }
    }

};

class CDB {
    mutable struct cdb m_db;
public:
    CDB( const std::string& fname ) {
        auto fd = open( fname.c_str(), O_RDONLY );
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
            auto base = static_cast<const unsigned char*>(m_vpos);
            auto f = [=](unsigned int i, unsigned int j) { return  uid_t( base[i] ) <<j; };
            return  f(4,0) | f(5,8) | f(6,16) | f(7,24) ;
        }
        std::time_t time() const {
            auto base = static_cast<const unsigned char*>(m_vpos);
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
        bool isTCK() { return key().compare(0,7,"AL/TCK/")==0; }
        bool isTopLevel() { return key().compare(0,12,"AL/TOPLEVEL/")==0; }
        std::string TCK() { return isTCK() ? key().substr(7) : std::string{} ; }
        std::string topLevel() { return isTopLevel() ? key().substr(12) : std::string{}; }
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

    iterator findTreeNode(const std::string& hash) const {
        return find( std::string("TN/")+hash );
    }

    bool ok() const { return cdb_fileno(&m_db)>=0; }
};


template <typename DB>
std::multiset< manifest_entry > create_manifest(DB& db) {
    std::multiset< manifest_entry > manifest;

    // first: get TCK -> id map, and invert.
    // needed so that we can make immutable manifest_entries later...
    std::map< std::string , std::string > tck;     // id -> TCK
    for ( auto record : db ) { // TODO: allow loop with predicate on key...
        if (!record.isTCK() ) continue;
        auto key = record.key() ; 
        tck.emplace( record.value() , record.TCK());
    }
    // next: create manifest
    for ( auto record : db ) { // TODO: allow loop with predicate on key... 
        if (!record.isTopLevel()) continue;
        auto key = record.key();
        // the 'comment' is in the 'label' member of the treenode this alias "points" at
        std::string comment;
        auto i = db.findTreeNode( key.substr(key.rfind("/")+1) );
        if ( i != std::end(db) ) {
            std::istringstream in( (*i).value() );
            ConfigTreeNode ctn; in >> ctn;
            comment = ctn.label();
            if (comment.empty()) {
                std::cerr << "could not locate Label part of " << std::endl;  
                std::cerr << (*i).value() << std::endl;
            }
        } else {
            std::cerr << "WARNING: could not locate treenode " <<  key.substr(key.rfind("/")+1) << " for key "  << key << std::endl;
        }
        manifest.emplace( record.topLevel(), tck, comment );
    }
    return manifest;
}

void dump_manifest( const std::multiset< manifest_entry > & manifest) {
    std::copy( std::begin(manifest), std::end(manifest),
               std::ostream_iterator<manifest_entry>(std::cout,"\n") );
}

void dump_manifest_as_json( const std::multiset< manifest_entry > & manifest) {
    // build the equivalent to the old-style python _dict used in createTCKManifest:
    // id ->  TCK, { release -> type }, label
    using r2t_t  = std::map< std::string, std::string >;
    using dict_t = std::map< std::string, std::tuple<std::string,r2t_t,std::string> >;
    dict_t dict;
    std::for_each( std::begin(manifest), std::end(manifest), [&dict](const manifest_entry& e) {
        auto i = dict.find( e.id );
        if ( i != std::end( dict ) ) {
            std::get<1>(i->second).emplace( e.release, e.type ) ;
        } else {
            dict.emplace( e.id, std::make_tuple( e.tck, r2t_t{{ e.release, e.type }} , e.comment ));
        }
    } );
    std::cout << to_json(dict) << std::endl;
}

template <typename DB>
void dump_keys(DB& db) {
    for ( auto record : db ) {
        std::cout << std::setw(5) << record.uid() 
                  << "   " << std::setw(6) <<  record.valuePersistentSize()  
                  << "   " << std::setw(34) << format_time( record.time() ) 
                  << "   " << record.key() << "\n";
    }
    std::cout << std::flush;
}

template <typename DB>
void dump_records(DB& db) {
    for ( auto record : db ) {
        std::cout << std::setw(5) << record.uid() 
                  << "   " << std::setw(6) << record.valuePersistentSize()  
                  << "   " << std::setw(34) << format_time( record.time() ) 
                  << "   " << record.key() << "\n";
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

void extract_records(TAR& db) {
    struct timespec t[2]; t[0].tv_nsec = 0; t[1].tv_nsec = 0;
    for ( auto record : db ) {
        auto key = record.key();
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

// TODO: add option to 'repack' the content of ConfigTreeNode and PropertyConfig... 
void convert_records( TAR& in, const std::string& oname ) {
    int ofd = open( oname.c_str(), 
                    O_RDWR  | O_CREAT | O_EXCL,
                    S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
    bool error = ofd < 0;
    struct cdb_make ocdb;
    cdb_make_start( &ocdb, ofd );

    for ( auto record : in ) {
        auto key = record.key();
        if (key.compare(0,16,"ConfigTreeNodes/")==0)  key.replace(0,18, "TN" ) ;
        if (key.compare(0,16,"PropertyConfigs/")==0)  key.replace(0,18 ,"PC");
        if (key.compare(0,8,"Aliases/")==0)           key.replace(0,7,"AL");
        auto v = record.value();
        if      (key.compare(0,2,"TN")==0) { v = convert<ConfigTreeNode>(v); }
        else if (key.compare(0,2,"PC")==0) { v = convert<PropertyConfig>(v); }

        auto val = make_cdb_record( v, record.uid(), record.time() );
        if ( cdb_make_add( &ocdb,
                           reinterpret_cast<const unsigned char*>( key.data() ),
                           key.size(), val.data(), val.size() ) != 0 ) {
            // handle error...
            std::cerr << " failure to put key " << key << " : " << errno << " = "
                 << strerror( errno ) << std::endl;
            error = true;
        }
    }
    cdb_make_finish( &ocdb );
    close(ofd);
    if (error) fs::remove( oname );
}

namespace po = boost::program_options;

template <typename DB>
void dispatch(const po::variables_map& vm, DB& db) {
    if (vm.count("list-manifest"))   dump_manifest( create_manifest(db));
    if (vm.count("list-manifest-as-json")) dump_manifest_as_json( create_manifest(db));
    if (vm.count("list-keys"))       dump_keys(db);
    if (vm.count("list-records"))    dump_records(db);
    if (vm.count("extract-records")) extract_records(db);
}

int main(int argc, char* argv[]) {
    po::options_description desc("Allowed options");
    desc.add_options() ("list-manifest", "dump manifest ")
                       ("list-manifest-as-json", "dump manifest in json format ")
                       ("list-keys", "list keys")
                       ("list-records", "list keys and records")
                       ("extract-records", "extract records")
                       ("convert-to-cdb", "convert to cdb")
                       ("input-file",  po::value<std::string>()->default_value("config.cdb"),"input file");
    po::positional_options_description p;
    p.add("input-file", -1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);

    std::string fname = vm["input-file"].as<std::string>();
    std::cerr << "opening " << fname << std::endl;
    
    if (boost::algorithm::ends_with(fname,".cdb")) {
        CDB db(fname) ;
        if (!db.ok()) return 1;
        dispatch(vm,db);

    }
    if (boost::algorithm::ends_with(fname,".tar")) {
        TAR db(fname) ;
        if (!db.ok()) return 1;
        dispatch(vm,db);
        if (vm.count("convert-to-cdb"))  convert_records(db, fname.substr(0,fname.size()-3)+"cdb" );
    }
    return 0;
}
