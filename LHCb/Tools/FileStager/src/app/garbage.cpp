// stdlig
#include <csignal>
#include <iostream>
#include <string>

#ifndef WIN32
// boost
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/thread/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace fs = boost::filesystem;
namespace po = boost::program_options;
using boost::posix_time::seconds;

// Need a global here to allow cleanup to be called using signal
std::string tmpdir;
pid_t pID = 0;

void cleanup( int )
{
   if ( kill( pID, 0 ) == 0 ) {
      // The main process still exists (and probably killed us), we don't do anything.
      exit( 0 );
   } else {
      fs::path p( tmpdir );
      if ( fs::exists(p) )
         fs::remove_all( tmpdir );
      exit( 0 );
   }
}
#endif

using namespace std ;

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
      ( "pid",  po::value< pid_t >(), "pid to watch" )
      ( "tmpdir", po::value< string >(), "tempdir" );

   po::options_description cmdline_options;
   cmdline_options.add( config ).add( hidden );

   po::positional_options_description p;
   p.add( "pid", 1 );
   p.add( "tmpdir", 1 );

   // Parse the command line
   po::variables_map vm;
   store( po::command_line_parser( argc, argv ).
          options( cmdline_options ).positional( p ).run(), vm );
   notify( vm );

   if ( vm.count( "help" ) ) {
      cout << "usage: garbage.exe pid dir" << endl;
      cout << config << endl;
      return 0;
   }

   try {
      tmpdir = vm[ "tmpdir" ].as< string >();
   } catch ( const boost::bad_any_cast& /* e */ ) {
      cout << "dir is not a correct string" << endl;
      cout << "usage: garbage.exe pid dir" << endl;
      return -2;
   }
   if ( !fs::exists( fs::path( tmpdir ) ) ) {
      cout << "Directory " << tmpdir << " does not exist!" << endl;
      return -2;
   }

   try {
      pID = vm[ "pid" ].as< pid_t >();
   } catch ( const boost::bad_any_cast& /* e */ ) {
      cout << "pid is not an integer" << endl;
      cout << "usage: garbage.exe pid dir" << endl;
      return -2;
   }

   // register the signals now, we know the tmpdir
   signal( SIGTERM, cleanup );

   // poll for the main process 
   while (1) {
      if ( kill( pID, 0 ) == 0 ) {
         boost::this_thread::sleep( seconds( 1 ) );
      } else {
         break;
      }
   }

   cleanup( 0 );

   return 0;
#endif
}

