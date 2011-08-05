// stdlib
#include <string>
#include <vector>
#include <sstream>

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

   boost::uintmax_t get_size( const string& command, const unsigned int lineno );
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
      } catch( const GaudiException& ) {
         return false;
      }
   }
   return true;
}

//=============================================================================
boost::uintmax_t  File::size() const
{
   if ( !m_size ) {
      if ( m_command.find( "rfcp" ) != string::npos ) {
         stringstream command;
         command << "rfstat " << m_remote;
         m_size = get_size( command.str(), 7 );
      } else {
         stringstream command;
         command << "gfal_teststat " << m_remote;
         m_size = get_size( command.str(), 5 );
      }
   }
   return *m_size;
}

namespace {

#ifndef WIN32
boost::uintmax_t get_size( const string& command, const unsigned int lineno ) {
   FILE* pipe = 0;
   vector< string > lines;
   int ret = -1;
   if ( ( pipe = ( FILE* )popen( command.c_str(), "r" ) ) ) {
      bio::stream_buffer< bio::file_descriptor_source > fpstream( fileno( pipe ), bio::never_close_handle );
      istream in( &fpstream  );
      string line;
      while ( in ) {
         getline( in, line );
         lines.push_back( line );
      }
      ret = pclose( pipe );
   }
   if ( ret < 0 ) {
      string error = "Executing " + command + " failed.";
      throw GaudiException( error, "Error getting size.", StatusCode::FAILURE );
   } else {
      boost::match_flag_type flags = boost::match_default;
      boost::smatch match;
      boost::regex re( "(\\d+)" );
      const std::string& line = lines[ lineno ];
      boost::regex_search( line.begin(), line.end(), match, re, flags );
      return boost::lexical_cast< boost::uintmax_t >( match[ 1 ].str() ) / 1024;
   }
}
#else
boost::uintmax_t get_size( const string& , const unsigned int ) {
   return 0;
}
#endif

} // anonymous namespace
