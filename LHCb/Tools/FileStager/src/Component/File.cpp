// stdlib
#include <string>
#include <vector>
#include <sstream>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

// boost
#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>

// boost
#include <boost/algorithm/string.hpp>

// Gaudi
#include "GaudiKernel/GaudiException.h"

// local
#include "File.h"

//-----------------------------------------------------------------------------
// Implementation file for class : File
//
// 2009-11-22 : Roel Aaij
//-----------------------------------------------------------------------------

namespace bio = boost::iostreams;
namespace ba = boost::algorithm;
namespace {
   using std::string;
   using std::cout;
   using std::istream;
   using std::endl;
   using std::vector;
   using std::stringstream;
   using std::endl;

   boost::uintmax_t get_size( const string& command, const unsigned int lineno,
                              const unsigned int matchno );
}

//=============================================================================
File::File( const string& original,
            const string& command,
            const string& remote,
            const string& temporary )
   : m_original( original ), m_command( command ), m_remote( remote ),
     m_temporary( temporary ), m_staged( false ), m_good( false )
{

}

//=============================================================================
File::~File()
{

}

//=============================================================================
bool File::exists() const
{
   if ( !m_size ) {
      try {
         size();
      } catch( const GaudiException& e ) {
         m_errorMessage = e.message();
         return false;
      }
   }
   return true;
}

//=============================================================================
boost::uintmax_t  File::size() const
{
   if ( !m_size ) {
      vector<string> args;
      ba::split(args, m_command, ba::is_any_of(" \t\n"));
      if ( args[0] == "rfcp" ) {
         stringstream command;
         command << "rfstat " << m_remote;
         m_size = get_size( command.str(), 7, 0 );
      } else if ( args[0] == "xrdcp" ) {
         stringstream command;
         boost::regex re_xrd( "root://([a-zA-z0-9\\.-]+)(?::([0-9]+))?" );
         boost::smatch matches;
         boost::match_flag_type flags = boost::match_default;
         boost::regex_search( m_remote.begin(), m_remote.end(), matches, re_xrd, flags );
         command << "xrd -DIport " << matches[2] << " " << matches[1] << " \"stat " 
                 << matches.suffix() << "\" 2>&1";
         m_size = get_size( command.str(), 1, 1 );
      } else if ( args[0] == "cp" ) {
         struct stat buf;
         int r = ::stat(m_remote.c_str(), &buf);
         if (r == -1) {
            string msg = strerror(errno);
            throw GaudiException( "Call to stat failed", msg, StatusCode::FAILURE );
         } else {
            m_size = uintmax_t(buf.st_size) / 1024.;
         }
      } else {
         stringstream command;
         command << "gfal_teststat " << m_remote;
         m_size = get_size( command.str(), 5, 0 );
      }
   }
   return *m_size;
}

namespace {

#ifndef WIN32
boost::uintmax_t get_size( const string& command, const unsigned int lineno,
                           const unsigned int matchno ) {
   FILE* pipe = 0;
   vector< string > lines;
   int ret = -1;
   if ( ( pipe = ( FILE* )popen( command.c_str(), "r" ) ) ) {
      bio::stream_buffer< bio::file_descriptor_source > fpstream( fileno( pipe ), bio::never_close_handle );
      istream in( &fpstream  );
      string line;
      while ( in ) {
         getline( in, line );
         if ( lines.size() < 20 ) lines.push_back( line );
      }
      ret = pclose( pipe );
   }
   if ( ret != 0 ) {
      string error = "Executing " + command + " failed.";
      throw GaudiException( error, "Error getting size.", StatusCode::FAILURE );
   } else {
      boost::match_flag_type flags = boost::match_default;
      boost::smatch match;
      boost::regex re( "(\\d+)" );
      if (lineno >= lines.size()) {
         string error = "Invalid output from " + command;
         stringstream s;
         s << "Could not get size from command output line " 
           << lineno << ".\n";
         for (vector<string>::const_iterator it = lines.begin(), end = lines.end();
              it != end; ++ it) {
            s << *it << endl;
         }
         throw GaudiException( error, s.str(), StatusCode::FAILURE );
      }
      const std::string& line = lines[ lineno ];
      std::vector<std::string> numbers;
      std::string::const_iterator start = line.begin(), end = line.end();
      while ( boost::regex_search(start, end, match, re, flags ) ) {
         numbers.push_back(match[0].str());
         start = match[0].second;
      }
      if (matchno >= numbers.size()) {
         string error = "Invalid output from " + command;
         stringstream s;
         s << "Could not get size from command output line " << line
           << " as the number at position " << matchno << ".\n";
         throw GaudiException( error, s.str(), StatusCode::FAILURE );
      }
      try {
         return boost::lexical_cast< boost::uintmax_t >( numbers[ matchno ] ) / 1024;
      } catch (const boost::bad_lexical_cast&) {
         string error = "Invalid output from " + command;
         stringstream s;
         s << "Could not get size from command output line " << line
           << " as the number at position " << matchno << ":" 
           << numbers[matchno] << ".\n";
         throw GaudiException( error, s.str(), StatusCode::FAILURE );
      }
   }
}
#else
boost::uintmax_t get_size( const string& , const unsigned int, const unsigned int ) {
   return 0;
}
#endif

} // anonymous namespace
