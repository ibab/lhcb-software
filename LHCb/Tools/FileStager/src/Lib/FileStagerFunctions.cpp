// stdlib
#include <string>

// boost
#include <boost/range.hpp>
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

// local
#include <FileStager/FileStagerFunctions.h>

#ifndef WIN32
namespace ba = boost::algorithm;
#endif

namespace {
   using std::string;
}

namespace FileStager {
//=============================================================================
bool createPFN( string& remote, string& command, bool stageLocal )
{
   boost::match_flag_type flags = boost::match_default;
   boost::match_results< string::iterator > match;
   boost::regex re( "^(root:)((?:root:)|(?:dcap:))" );

   boost::iterator_range< string::iterator > result;
   if ( ( result = ba::find_first( remote, "gfal:" ) ) ) {
      // strip gfal:
      ba::erase_range( remote, result );
   } else if ( regex_search( remote.begin(), remote.end(), match, re, flags ) ) {
      // in case of root:root: or root:dcap, remove the first root
      boost::iterator_range< string::iterator >
         range( match[ 1 ].first, match[ 1 ].second );
      ba::erase_range( remote, range );
   } else if ( ( result = ba::find_first( remote, "mdf:" ) ) ) {
      // strip mdf:
      ba::erase_range( remote, result );
   }

   re = "^((?:rfio)|(?:castor):)//(?:\\w\\.\\w\\.\\w:\\d+/+castor)";
   if ( regex_search( remote.begin(), remote.end(), match, re, flags ) ) {
      boost::iterator_range< string::iterator >
         range( match[ 1 ].first, match[ 1 ].second );
      ba::replace_range( remote, range, "rfio:" );
      command = "rfcp";
      return true;
   } else if ( ( result = ba::find_first( remote, "castor:" ) ) ) {
      // erase castor
      ba::erase_range( remote, result );
      command = "rfcp";
      return true;
   } else if ( ( result = ba::find_first( remote, "rfio:" ) ) ) {
      // erase rfio
      ba::erase_range( remote, result );
      command = "rfcp";
      return true;
   } else if ( ( result = ba::find_first( remote, "root:" ) ) ) {
      // xrootd needs no changes, command is xrdcp
      command = "xrdcp -s";
      return true;
   } else if ( ( result = ba::find_first( remote, "srm:" ) ) ) {
      // srm does not need changes, command is lcg-cp
      command = "lcg-cp -V lhcb";
      return true;
   } else if ( ( result = ba::find_first( remote, "dcap:" ) ) ) {
      // gsidcap or dcap needs no changes, command is dccp
      command = "dccp -A";
      return true;
   } else if ( ( result = ba::find_first( remote, "/castor" ) ) ) {
      // castor file, no protocol specification
      command = "rfcp";
      return true;
   } else if ( stageLocal ) {
      if ( ( result = ba::find_first( remote, "file:" ) ) ) {
         // local file, perhaps nfs or other networked filesystem
         ba::erase_range( remote, result );
         ba::trim_left_if( remote, ba::is_any_of("/") );
         remote = "/" + remote;
      }
      command = "cp";
      return true;
   } else {
      return false;
   }
}

//=============================================================================
bool createLFN( string& remote, string& command )
{
   // The input are only lfns which are not in the catalog.

   boost::iterator_range< string::iterator > result;
   if ( ( result = ba::find_first( remote, "gfal:" ) ) ) {
      // strip gfal
      ba::erase_range( remote, result );
   }

   // Add /grid to start of lfn path
   if ( remote.substr( 0, 5 ) != "/grid" ) {
      remote = "/grid" + remote;
   }
   // add lfn: to remote
   remote = "lfn:" + remote;

   command = "lcg-cp -V lhcb";
   return true;
}
}
