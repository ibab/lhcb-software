#include <string>
#include <iostream>

// boost
#include <boost/range.hpp>
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

// local
#include <FileStager/FileStagerFunctions.h>

namespace {
   using std::string;
   using std::stringstream;
   using std::cout;
   using std::endl;
   using std::cerr;
}

#ifndef WIN32
namespace ba = boost::algorithm;
namespace po = boost::program_options;
namespace fs = boost::filesystem;
#endif

int main( int argc, char* argv[] )
{

#ifdef WIN32
   cout << "Windows is not supported." << endl;
   return -1;
#else
   // Setup the command line option parser
   po::options_description config("Possible options");
   config.add_options()
      ( "help,h", "print help message" );

   po::options_description hidden( "Hidden options" );
   hidden.add_options()
      ( "descriptor", po::value< string >(), "tempdir" );

   po::options_description cmdline_options;
   cmdline_options.add( config ).add( hidden );

   po::positional_options_description pos;
   pos.add( "descriptor", 1 );

   // Parse the command line
   po::variables_map vm;
   store( po::command_line_parser( argc, argv ).
          options( cmdline_options ).positional( pos ).run(), vm );
   notify( vm );

   if ( vm.count( "help" ) ) {
      cout << "usage: hash_filename.exe descriptor" << endl;
      cout << config << endl;
      return 0;
   }

   std::string descriptor;
   try {
      descriptor = vm[ "descriptor" ].as< string >();
   } catch ( const boost::bad_any_cast& /* e */ ) {
      cout << "descriptor is not a correct string" << endl;
      cout << "usage: garbage.exe descriptor" << endl;
      return -2;
   }

   string file(descriptor);
   string remote(descriptor);
   string command;
   bool success = false;
   boost::iterator_range< string::iterator > result;
   if ( result = ba::find_first( file, "LFN:" ) ) {
      // convert LFN to lowercase
      ba::erase_range( file, result );
      remote = file;
      success = FileStager::createLFN( remote, command );
   } else if ( result = ba::find_first( file, "PFN:" ) ) {
      // strip PFN
      ba::erase_range( file, result );
      remote = file;
      success = FileStager::createPFN( remote, command, true );
   } else {
      success = FileStager::createPFN( remote, command, true);
   }

   if ( !success ) {
      cerr << "Error manipulating the original descriptor: " << descriptor
           << " into a suitable remote filename" << endl;
      exit(-2);
   }

   boost::hash< string > hash;
   fs::path p(remote);
   stringstream temp;
   string extension = p.extension().string();
   if ( result = ba::find_first( extension, "?" ) ) {
      boost::iterator_range< string::iterator > range( result.begin(), extension.end() );
      ba::erase_range( extension, range );
   }
   temp << boost::format( "%|x|" ) % hash( remote ) << extension;
   
   cout << temp.str() << endl;
   return 0;
}

#endif
