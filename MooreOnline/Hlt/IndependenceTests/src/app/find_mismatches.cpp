// stdlib
#include <iostream>
#include <string>
#include <cstdlib>

// STL
#include <list>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>

// Root
#include <TDirectoryFile.h>
#include <TFile.h>
#include <TTree.h>
#include <TBranch.h>
#include <TTreeCache.h>

// boost
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/program_options.hpp>
#include <boost/assign/list_of.hpp>

// local
#include <Independence/DecisionMap.h>

using namespace std;

using namespace boost::assign;
namespace bf = boost::filesystem;
namespace ba = boost::algorithm;
namespace po = boost::program_options;

typedef pair< unsigned int, unsigned int > mkey_t;
typedef boost::unordered_map< mkey_t, set< string > > mismatches_t;
typedef boost::unordered_set< string > select_t;

int process_file( const string& filename,
                  const DecisionMap& decisionMap,
                  mismatches_t& mismatches );

void add_branch( TTree* tree, const std::string& branch_nane, int* d, vector< TBranch* >& branches );

int main( int argc, char* argv[] ) {

   // strings to contain commandline options
   string lines;
   string ignore;
   string output_filename;

   // Setup the command line option parser
   po::options_description config("Possible options");
   config.add_options()
      ( "help,h", "print help message" )
      ( "select", po::value< string >( &lines )->default_value( "all" ),
        "lines to record mismatches for")
      ( "ignore", po::value< string >( &ignore )->default_value( "none" ),
        "lines to ignore mismatches for")
      ( "output,o", po::value< string >( &output_filename )->default_value( "mismatches.lst" ),
        "file in which mismatches will be stored");

   po::options_description hidden( "Hidden options" );
   hidden.add_options()
      ( "input", po::value< string >(), "input file" );

   po::options_description cmdline_options;
   cmdline_options.add(config).add(hidden);

   po::positional_options_description p;
   p.add( "input", 1 );

   // Parse the command line
   po::variables_map vm;
   store( po::command_line_parser( argc, argv ).
          options( cmdline_options ).positional( p ).run(), vm );
   notify( vm );

   if ( vm.count( "help" ) || !vm.count( "input" ) ) {
      cout << "usage: find_mismatches [options] input" << endl;
      cout << config << endl;
      return 0;
   }

   // Open the input file
   string filename = vm[ "input" ].as< string >();
   bf::path path( filename );
   if ( !bf::exists( path ) ) {
      cout << "Input file " << filename << " does not exist." << endl;
      return 1;
   }
      
   ifstream file;
   file.open( path.string().c_str() );
   if ( !file.is_open() ) {
      cerr << "Could not open file: " << filename << endl;
      return 1;
   }

   typedef list< pair< string, string > > files_t;
   files_t files;
   string read_line;
   while( getline( file, read_line ) ) {
      ba::trim( read_line );
      if ( read_line.empty() || read_line[ 0 ] == '#' ) continue;
      vector< string > v;
      ba::split( v, read_line, ba::is_any_of( " " ) );
      files.push_back( make_pair( v[ 0 ], v[ 1 ] ) );
   }

   // Find the file with all lines in it
   files_t::iterator it_file = files.begin(),
      files_end = files.end();
   for ( ; it_file != files_end; ++it_file ) {
      if ( it_file->first == "allLines" ) break;
   }
   if ( it_file == files_end ) {
      cout << "Error, no allLines tuple in the list" << endl;
      return 2;
   }
   string allLinesFile = it_file->second;

   // Make sure we don't open the file with all lines again later
   files.erase( it_file );

   // Parse the selected lines
   select_t select;
   if ( lines != "all" ) {
      // Lines to select specified on command line
      list< string > l;
      ba::split( l, lines, ba::is_any_of( ";" ) );
      BOOST_FOREACH( const std::string line, l ) {
         select.insert( line );
      }
   } else {
      // Use all available lines
      BOOST_FOREACH( const files_t::value_type& entry, files ) {
         select.insert( entry.first );
      }
   }

   // Check whether the selected lines exist
   BOOST_FOREACH( const string& line, select ) {
      for ( it_file = files.begin(); it_file != files_end; ++it_file ) {
         if ( it_file->first == line ) break;
      }
      if ( it_file == files_end ) {
         cout << "Selected line " << line << " has no file listed." << endl;
         return 2;
      }
   }

   select_t remove;
   if ( ignore != "none" ) {
      // Lines to ignore specified on command line
      list< string > l;
      ba::split( l, ignore, ba::is_any_of( ";" ) );
      BOOST_FOREACH( const std::string& line, l ) {
         remove.insert( line );
      }
   }

   // Remove lines to be ignored
   BOOST_FOREACH( const string& line, remove ) {
      if ( select.count( line ) ) {
         select.erase( line );
      }
   }

   // Open the root file
   TFile* rootFile = TFile::Open( allLinesFile.c_str() );
   if ( !rootFile || !rootFile->IsOpen() ) {
      cout << "Error, could not open " << allLinesFile << endl;
      return 2;
   }
   
   // Get the directory and the tree
   TObject* tobj = rootFile->Get( "TupleHltDecReports" );
   TDirectoryFile* dir = static_cast< TDirectoryFile* >( tobj );
   tobj = dir->Get( "HltDecReportsTuple" );
   TTree* tree = static_cast< TTree* >( tobj );

   // Use cache
   int cachesize = 10000000;
   tree->SetCacheSize( cachesize );
   tree->AddBranchToCache( "*", true );

   // Set the branch addresses for run and event number
   int runNumber = 0, eventNumber = 0;
   tree->SetBranchAddress( "runNumber", &runNumber );
   tree->SetBranchAddress( "eventNumber", &eventNumber );

   // See which decisions are in the tree
   TObjArray* branches = tree->GetListOfBranches();
   
   typedef vector< pair< string, int > > decisions_t;
   decisions_t dv;

   for ( int i = 0; i < branches->GetEntries(); ++i ) {
      tobj = branches->At( i );
      string branchName( tobj->GetName() );
      size_t pos = branchName.find( "Decision" );
      if ( pos != string::npos ) {
         dv.push_back( make_pair( branchName.substr( 0, pos ), 0 ) );
      }
   }

   // Set the branch addresses for the decisions
   BOOST_FOREACH( decisions_t::value_type& entry, dv ) {
      string decision = entry.first + "Decision";
      tree->SetBranchAddress( decision.c_str(), &(entry.second) );
   }

   // Container to store all lines decisions in memory
   DecisionMap dm;

   cout << "Processing allLines" << endl;

   // Process the tree
   unsigned int entries = tree->GetEntries();
   for ( unsigned int i = 0; i < entries; ++i ) {
      tree->GetEntry( i );
      BOOST_FOREACH( decisions_t::value_type& entry, dv ) {
         string line = entry.first;
         if ( select.count( line ) ) {
            int value = entry.second;
            dm.addDecision( runNumber, eventNumber, line, value );
         }
      }
   }

   // Container to store mismatches
   mismatches_t mismatches;

   // Process the single line file
   BOOST_FOREACH( const files_t::value_type& entry, files ) {
      bf::path p( entry.second );
      if ( !bf::exists( p ) ) {
         cout << "Error, file does not exist:" << p << endl;
         return 2;
      }
      if ( !select.count( entry.first ) ) continue;
      int status = process_file( p.string(), dm, mismatches );
      if ( status != 0 ) return status;
   }

   // Put the mismatches in a file
   bf::path output_path( output_filename );

   ofstream of;
   of.open( output_path.string().c_str(), ios::out );

   if ( mismatches.size() ) {
      cout << "Found mismatches:" << endl;
      BOOST_FOREACH( mismatches_t::value_type& entry, mismatches ) {
         const mkey_t& key = entry.first;
         set< string >& lines = entry.second;
         stringstream lineStream;
         const string& first = *( lines.begin() );
         size_t pos = first.find( "Decision" );
         lineStream << first.substr( 0, pos );
         lines.erase( first );
         BOOST_FOREACH( const string& line, lines ) {
            pos = line.find( "Decision" );
            lineStream << ", " << line.substr( 0, pos );
         }
         cout << key.first << " " << key.second << " " << lineStream.str() << endl;
         of << key.first << " " << key.second << endl;
      }
   } else {
      cout << "No mismatches found." << endl;
   }
   of.close();
   return 0;
}

int process_file( const std::string& filename,
                  const DecisionMap& decisionMap,
                  mismatches_t& mismatches )
{
   // Open the specified file
   TFile* rootFile = TFile::Open( filename.c_str() );
   if ( !rootFile || !rootFile->IsOpen() ) {
      cout << "Could not open file " << filename << endl;
      return 1;
   }

   // Get the tree from the directory
   TObject* tobj = 0;
   tobj = rootFile->Get( "TupleHltDecReports" );
   TDirectoryFile* dir = static_cast< TDirectoryFile* >( tobj );
   tobj = dir->Get( "HltDecReportsTuple" );
   TTree* tree = static_cast< TTree* >( tobj );
   int cachesize = 10000000;
   tree->SetCacheSize( cachesize );

   // Setup branch addresses for run and event number
   int runNumber = 0;
   int eventNumber = 0;

   // Initialise run and event number branches and add them to the cache.
   vector< TBranch* > read_branches;
   add_branch( tree, "runNumber", &runNumber, read_branches );
   add_branch( tree, "eventNumber", &eventNumber, read_branches );

   // Get the branches which contain decisions
   TObjArray* branches = tree->GetListOfBranches();
   typedef boost::unordered_map< string, set< string > > decisions_t;
   decisions_t decisions = map_list_of( "Hlt1", set< string >() )
      ( "Hlt2", set< string >() );
   string line;
   for ( int i = 0; i < branches->GetEntries(); ++i ) {
      line = branches->At( i )->GetName();
      size_t pos = line.find( "Decision" );
      if ( pos == string::npos ) continue;
      string level = line.substr( 0, 5 );
      line = line.substr( 0, pos );
      decisions[ level ].insert( line );
   }

   // Make sure we are only dealing with a single decision/line
   vector< string > levels = list_of( "Hlt2" )( "Hlt1" );
   BOOST_FOREACH( const string& level, levels ) {
      set< string > lines = decisions[ level ];
      // There are Hlt2 decisions
      if ( lines.size() > 1 ) {
         cout << "Error, more than one " << level << " decision in this file." << endl;
         BOOST_FOREACH( const string& dec, lines ) {
            cout << dec << endl;
         }
      } else if ( !lines.empty() ) {
         line = *( lines.begin() );
         break;
      }
   }
   
   if ( line.empty() ) {
      cout << "No decision in this file" << endl;
      return 1;
   } else {
      // Notify the user
      cout << "Processing " << line << endl;
   }

   // Set the branch address for the decision
   int decision = 0;
   string branch_name = line + "Decision";
   add_branch( tree, branch_name, &decision, read_branches );

   // Process the tree and store any mismatches
   unsigned int entries = tree->GetEntries();
   for ( unsigned int i = 0; i < entries; ++i ) {
      tree->LoadTree( i );
      BOOST_FOREACH( TBranch* branch, read_branches ) {
         branch->GetEntry( i );
      }
      int d = decisionMap.decision( runNumber, eventNumber, line );
      if ( d != decision ) {
         mkey_t key( runNumber, eventNumber );
         mismatches_t::iterator it = mismatches.find( key );
         if ( it == mismatches.end() ) {
            set< string > s;
            s.insert( line );
            mismatches.insert( make_pair( key, s ) );
         } else {
            set< string >& s = it->second;
            s.insert( line );
         }
      }
   }
   return 0;
}

void add_branch( TTree* tree, const std::string& branch_name, int* d, vector< TBranch* >& branches ) {
   TBranch* branch = tree->GetBranch( branch_name.c_str() );
   tree->AddBranchToCache( branch, true );
   branch->SetAddress( d );
   branches.push_back( branch );
}
