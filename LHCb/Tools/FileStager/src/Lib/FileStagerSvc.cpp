#ifndef WIN32
// system headers
#include <sys/statvfs.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#endif

// stdlib
#include <iostream>
#include <cstdio>
#include <limits>
#include <sstream>
#include <string>

// Gaudi
#include "GaudiKernel/SvcFactory.h"
#include <GaudiUtils/IIODataManager.h>

// STL
#include <vector>

#ifndef WIN32
// boost
#include <boost/range.hpp>
#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/iostreams/device/file_descriptor.hpp>
#include <boost/iostreams/stream.hpp>
#endif

// local
#include "File.h"
#include "FileStagerSvc.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FileStagerSvc
//
// 2009-11-21 : Roel Aaij
//-----------------------------------------------------------------------------

#ifndef WIN32
namespace fs = boost::filesystem;
namespace ba = boost::algorithm;
namespace pt = boost::posix_time;
namespace bio = boost::iostreams;
namespace {
   using namespace boost;
   using namespace assign;
}
#endif

namespace {
   using Gaudi::IIODataManager;
   using std::string;
   using std::stringstream;
   using std::vector;
   using std::istream;
   using std::getline;
}

DECLARE_SERVICE_FACTORY( FileStagerSvc );

//=============================================================================
FileStagerSvc::FileStagerSvc( const string& name, ISvcLocator* svcLoc )
   : base_class( name, svcLoc ), m_thread( 0 ), m_garbagePID( 0 ),
     m_initialized( false )
{

   declareProperty( "Tempdir", m_tmpdir, "The base of the temporary directory "
                    "where the files will be staged" );
   declareProperty( "StageNFiles", m_stageNFiles = 2, "The number of files to stage" );
   declareProperty( "KeepFiles", m_keepFiles = false, "Keep staged files" );
   declareProperty( "DiskspaceTries", m_tries = 10, "The number of times to retry "
                    "whether there is sufficient diskspace" );
   declareProperty( "RetryStaging", m_retry = false, "Retry staging once it's failed." );
   declareProperty( "CopyTries", m_copyTries = 5, "Retry copying if it fails." );
   declareProperty( "StageLocalFiles", m_stageLocalFiles = false,
                    "Stage files beginning with file:." );
   declareProperty( "DataManagerName",
                    m_dataManagerName = "Gaudi::StagedIODataManager/IODataManager" ,
                    "Name of the IODataManager to use for proper disconnection." );
   declareProperty( "GarbageCollectorCommand", m_garbageCommand = "garbage.exe" ,
                    "Command for the garbage collector." );

}

//=============================================================================
FileStagerSvc::~FileStagerSvc()
{

}

#ifndef WIN32
//=============================================================================
StatusCode FileStagerSvc::initialize()
{
   if ( m_initialized ) return StatusCode::SUCCESS;
   StatusCode sc = Service::initialize();

   // Remove trailing slashes
   ba::trim_right_if( m_tmpdir, ba::is_any_of( "/" ) );

   // Check if the base dir exists. FIXME create it if possible and clean up
   // correctly later.
   if ( !fs::exists( m_tmpdir ) ) {
      error() << "Base temp dir " << m_tmpdir << " does not exist." << endmsg;
      sc = StatusCode::FAILURE;
      return sc;
   }

   // Create the temporary directory.
   if ( !m_keepFiles ) {
      // Create a random directory, we're not keeping the files
      fs::path p( m_tmpdir + "/FileStagerSvc_XXXXXX" );
      const char* temp = p.string().c_str();
      char buf[ 200 ];
      snprintf( buf, 200, "%s", temp );
      const char* tmpdir = mkdtemp( buf );
      m_tmpdir = tmpdir;
   } else {
      // Create a standard directory, not pretty, but it works.
      fs::path p( m_tmpdir + "/FileStagerSvc" );
      fs::create_directories( p );
      m_tmpdir = p.string();
   }

   // Check if the temp dir is ok.
   if ( !fs::exists( m_tmpdir ) ) {
      error() << "Could not create temp dir " << m_tmpdir << endmsg;
      sc = StatusCode::FAILURE;
      return sc;
   }

   // Get the datamanager to be able to properly disconnect files when we
   // remove them.
   // m_dataManager = serviceLocator()->service( m_dataManagerName, false );
   // if ( !m_dataManager.isValid() ) {
   //    error() << "Error getting IODataManager Service!" << endmsg;
   //    return StatusCode::FAILURE;
   // }

   // Submit the garbage collector if not keeping files
   if ( !m_keepFiles ) sc = garbage();
   if ( sc.isSuccess() ) {
      m_initialized = true;
   }
   return sc;
}

//=============================================================================
StatusCode FileStagerSvc::finalize()
{
   StatusCode sc = clearFiles();
   if ( !m_keepFiles && fs::exists( m_tmpdir ) ) {
      fs::remove( m_tmpdir );
   }

   if ( !m_keepFiles ) {
      verbose() << "Killing garbage collector" << endmsg;
      kill( m_garbagePID, SIGTERM );
      int err = 0;
      while ( waitpid( m_garbagePID, &err, 0) == -1 )
         if ( errno != EINTR ) {
            warning() << "Interrupted while waiting for the garbage collector " 
                      << "to exit." << endmsg;
            break;
         }
      if ( err < 0 ) {
         warning() << "The garbage collector exited with an error: " 
                   << strerror( err ) << endmsg;
      }
   }
   if ( !sc.isSuccess() ) {
      return sc;
   } else {
      return Service::finalize();
   }
}

//=============================================================================
StatusCode FileStagerSvc::getLocal( const string& filename, string& local )
{
   if ( outputLevel() <= MSG::DEBUG ) {
      debug() <<  "Get local filename for: " << filename << endmsg;
   }

   File* openFile = 0;
   const_original_iterator openIt;
   {
      lock_guard< recursive_mutex > fileLock( m_fileMutex );
      filesByOriginal_t& originals = m_files.get< originalTag >();
      openIt = originals.find( filename );
      openFile = openIt->file();
      if ( openIt == originals.end() ) {
         debug() << filename << " is not listed to be staged." << endmsg;
         return StatusCode::FAILURE;
      } else if ( openFile->good() && openFile->staged() ) {
         local = openFile->temporary();
         // Delete previous file if necessary
         removePrevious( openIt );
         return StatusCode::SUCCESS;
      }
   }

   if ( !m_thread ) {
      // retry
      if ( m_retry ) {
         restartStaging( filename );
      } else {
         local = filename;
         warning() << "Not retrying staging, returning original filename" << endmsg;
         return StatusCode::SUCCESS;
      }
   }

   // Delete previous file if necessary
   removePrevious( openIt );

   // Wait for the file to be staged
   unique_lock< mutex > lock( openFile->openMutex() );
   while( !openFile->staged() ) {
      openFile->openCondition().wait( lock );
      if ( !openFile->good() ) break;
   }

   // Check if everything is ok
   if ( openFile->good() ) {
      info() << "Staged file: " << openFile->remote() << " as "
             << openFile->temporary() << endmsg;
      local = openFile->temporary();
      return StatusCode::SUCCESS;
   } else {
      warning() << "Error occurred while staging:" << openFile->remote()
                << " as " << openFile->temporary() << endmsg;
      local = filename;
      m_thread->join();
      delete m_thread;
      m_thread = 0;
      removeFiles();
      return StatusCode::FAILURE;
   }
}

//=============================================================================
StatusCode FileStagerSvc::addFiles( const vector< string >& files )
{
   if ( files.empty() ) {
      warning() << "No files added to stager" << endmsg;
      return StatusCode::SUCCESS;
   }

   if ( m_thread ) {
      error() << "Staging has already started, adding more files is "
              << "currently not supported." << endmsg;
      return StatusCode::FAILURE;
   }

   BOOST_FOREACH( const string& filename, files ) {
      File* file = createFile( filename );
      // Bad files are skipped. This probably means that gfal_stat failed.
      if ( file->good() ) m_files.insert( FileWrapper( file ) );
   }

   if ( !m_files.empty() ) {
      filesByPosition_t& filesByPosition = m_files.get< listTag >();
      m_stageStart = filesByPosition.begin()->original();
      m_thread = new thread( bind( &FileStagerSvc::stage, this ) );
   }

   if ( outputLevel() <=  MSG::DEBUG ) {
      debug() << "Files queued for staging:" << endmsg;
      BOOST_FOREACH( const FileWrapper& wrapper, m_files.get< listTag >() ) {
         debug() << wrapper.original() << endmsg;
      }
   }
   return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode FileStagerSvc::clearFiles()
{
   // Check if there is still a staging thread
   if ( m_thread ) {
      m_thread->interrupt();
      m_thread->join();
      delete m_thread;
      m_thread = 0;
   }

   // Cleanup staged files
   removeFiles();

   // Clear the internal container
   BOOST_FOREACH( const FileWrapper& wrapper, m_files.get< listTag >() ) {
      File* file = wrapper.file();
      if ( file ) delete file;
   }
   m_files.clear();
   return StatusCode::SUCCESS;
}

//=============================================================================
void FileStagerSvc::stage()
{
   {
      lock_guard< recursive_mutex > fileLock( m_fileMutex );
      if ( m_files.size() == 0 ) {
         return;
      } else {
         m_stageIt = m_files.get< originalTag >().find( m_stageStart );
      }
   }

   while ( 1 ) {
      try {
         string temp;
         fs::path temporaryPath;

         File* stageFile = 0;
         {
            // Locate the file that will be staged
            stageFile = m_stageIt->file();
            temporaryPath = stageFile->temporary();

            // Check if file exists
            bool err = false;
            
            if ( !stageFile->exists() && !fs::exists( temporaryPath ) ) {
               error() << "Error: " << stageFile->remote() << " does not exists" << endmsg;
               err = true;
            }

            // Check available diskspace if we need it
            if ( !fs::exists( temporaryPath ) ) {
               uintmax_t space = diskspace();
               size_t tries = 0;
               while ( space < stageFile->size() + 1 && tries < m_tries ) {
                  warning() << "No enough diskspace in " << m_tmpdir
                            << " sleeping 60 seconds before retrying" << endmsg;
                  boost::this_thread::sleep( pt::seconds( 60 ) );
                  ++tries;
               }
               if ( tries >= m_tries ) {
                  error() << "Still not enough space to stage: " << stageFile->remote()
                          << " after " << tries << " tries, giving up." << endmsg;
                  err = true;
               }
            }

            if ( err ) {
               // Set correct variables and notify conditions.
               stageFile->setStaged( true );
               stageFile->setGood( false );
               stageFile->openCondition().notify_one();
               break;
            }
         }

         const_position_iterator pos;
         bool waitForClose = true;
         {
            lock_guard< recursive_mutex > lock( m_fileMutex );
            const filesByPosition_t& filesByPosition = m_files.get< listTag >();
            pos = m_files.project< listTag >( m_stageIt );

            for ( size_t i = 0; i < m_stageNFiles; ++i ) {
               if ( pos == filesByPosition.begin() ) {
                  waitForClose = false;
                  break;
               }
               do {
                  --pos;
               } while( !pos->file()->good() && !pos->file()->staged() 
                        && pos != filesByPosition.begin() );
            }
         }

         if ( waitForClose ) {
            File* closeFile = pos->file();
            unique_lock< mutex > lock( closeFile->closeMutex() );
            debug() << "Waiting for close of " << closeFile->remote() << endmsg;
            while( closeFile->staged() ) {
               closeFile->closeCondition().wait( lock );
            }
         }

         info() <<  "Staging file " << stageFile->remote() << endmsg;
         
         bool err = false;
         unsigned int tries = 0;
         while ( tries < m_copyTries ) {
            ++tries;
            // If the file is already there, no need to copy
            fs::path temporary( stageFile->temporary() );
            if ( fs::exists( temporary ) 
                 && fs::file_size( temporary ) == stageFile->size() ) 
               break;

            // try to copy a few times
            FILE* pipe = 0;
            vector< string > lines;
            int ret = 0;
            stringstream command;
            command << stageFile->command() << " \"" << stageFile->remote() 
                    << "\" \"" << stageFile->temporary() << "\"";
            verbose() << "Calling command: " << command.str() << endmsg;
            if ( !( pipe = ( FILE* )popen( command.str().c_str(), "w" ) ) ) {
               // problems setting up the shell
               error() << "Error creating the pipe"  << endmsg;
               err = true;
            } else {
               bio::stream_buffer< bio::file_descriptor_source > fpstream( fileno( pipe ), bio::never_close_handle );
               istream in( &fpstream  );
               string line;
               while ( in ) {
                  getline( in, line );
                  lines.push_back( line );
               }
               ret = pclose( pipe );
            }

            // Check if the file is actually there.

            if ( ret != 0 ) {
               error() << "Staging failed error output: " << endmsg;
               BOOST_FOREACH( const string& line, lines ) {
                  error() << line;
               }
               error() << endmsg;
               err = true;
               boost::this_thread::sleep( pt::seconds( 10 ) );
            } else if ( !fs::exists( temporary ) ) {
               warning() << "Staging command returned, but the file is not there, retrying." 
                         << endmsg;
               err = true;
               boost::this_thread::sleep( pt::seconds( 10 ) );
            } else {
               err = false;
               break;
            }
         }
            
         if ( err ) {
            // Handle errors
            lock_guard< recursive_mutex > lock( m_fileMutex );
            // Set correct variables and notify conditions.
            stageFile->setStaged( true );
            stageFile->setGood( false );
            stageFile->openCondition().notify_one();
            break;
         } else {
            // We're good
            lock_guard< recursive_mutex > lock( m_fileMutex );
            info() << "Staging successful: " << stageFile->remote() << " staged." << endmsg;
            stageFile->setStaged( true );
            stageFile->setGood( true );
            stageFile->openCondition().notify_one();

            // Move to next file
            const_position_iterator pos = m_files.project< listTag >( m_stageIt );
            const_position_iterator end = m_files.get< listTag >().end();
            ++pos;
            if ( pos == end ) {
               break;
            } else {
               m_stageIt = m_files.project< originalTag >( pos );
            }
         }
      } catch ( const thread_interrupted& interrupt ) {
         break;
      }
   }
}

//=============================================================================
boost::uintmax_t FileStagerSvc::diskspace() const
{
   fs::path tmp( m_tmpdir );
   fs::space_info space = fs::space( tmp );
   if ( space.available != std::numeric_limits< uintmax_t >::max() ) {
      return space.available / 1024;
   } else {
      throw GaudiException( "Checking space in " + m_tmpdir + " failed.",
                            "filesystem exception", StatusCode::FAILURE );
      return 0;
   }
}

//=============================================================================
void FileStagerSvc::restartStaging( const string& filename )
{
   if ( m_thread ) {
      m_thread->interrupt();
      m_thread->join();
      delete m_thread;
      m_thread = 0;
   }
   // Remove remaining files
   removeFiles();

   // Restart staging
   m_stageStart = filename;
   m_thread = new thread( bind( &FileStagerSvc::stage, this ) );
}

//=============================================================================
void FileStagerSvc::removeFile( const_original_iterator it )
{
   lock_guard< recursive_mutex > lock( m_fileMutex );

   // Remove the file
   File* file = it->file();
   string temporary = file->temporary();

   // Disconnect the file
   // StatusCode sc = StatusCode::SUCCESS;
   // IIODataManager::Connection* con = m_dataManager->connection( file->original() );
   // if ( con ) {
   //    con->disconnect();
   // }

   fs::path p( temporary );
   if ( !m_keepFiles && fs::exists( p ) ) {
      // Remove the file
      fs::remove( p );
   }
   file->setStaged( false );
   file->setGood( false );

   if ( outputLevel() <=  MSG::VERBOSE ) {
      verbose() << "Deleted file " << temporary << endmsg;
   }

}

//=============================================================================
void FileStagerSvc::removeFiles()
{
   if ( m_thread ) {
      throw GaudiException( "Files should not be removed while staging.",
                            "staging", StatusCode::FAILURE );
   }

   const filesByPosition_t& files = m_files.get< listTag >();
   for ( const_position_iterator it = files.begin(), end = files.end();
         it != end; ++it ) {
      removeFile( m_files.project< originalTag>( it ) );
   }
}

//=============================================================================
void FileStagerSvc::removePrevious( const_original_iterator it )
{
   lock_guard< recursive_mutex > lock( m_fileMutex );
   filesByPosition_t& filesByPosition = m_files.get< listTag >();
   const_position_iterator pos = m_files.project< listTag >( it );

   // Delete files if possible
   if ( pos == filesByPosition.begin() ) {
      return;
   } else {
      --pos;
   }

   File* closeFile = pos->file();
   if ( closeFile->staged() ) {
      const_original_iterator rIt = m_files.project< originalTag >( pos );
      removeFile( rIt );
      closeFile->closeCondition().notify_one();
   }
}

//=============================================================================
StatusCode FileStagerSvc::garbage()
{
   int pipefds[ 2 ];

   if ( pipe( pipefds ) ) {
      error() << "Failed to create the required pipes" << endmsg;
      return StatusCode::FAILURE;
   }
   if ( fcntl( pipefds[ 1 ], F_SETFD, fcntl( pipefds[ 1 ], F_GETFD ) | FD_CLOEXEC ) ) {
      error() << "Failed to set the required flags on the pipe" << endmsg;
      return StatusCode::FAILURE;
   }

   // Create the arguments
   int ppid = getpid();

   vector< string > arguments;
   arguments += m_garbageCommand, lexical_cast< string >( ppid ), m_tmpdir;
    
   // Put the arguments into the correct format for execvp
   size_t n = arguments.size();
   char** args = new char*[ n + 1 ];
   for ( size_t i = 0; i < n; ++i ) args[ i ] = new char[ 1024 ];
   args[ n ] = 0;
   for ( size_t i = 0; i < arguments.size(); ++i ) {
      const std::string& arg = arguments.at( i );
      snprintf( args[ i ], 1024, "%s", arg.c_str() );
   }

   // pass parent pid to stagemonitor for monitoring
   m_garbagePID = fork();
   if ( m_garbagePID < 0 ) {
      error() << "Failed to fork the garbage collector" << endmsg;

      // delete memory reserved for args
      for ( size_t i = 0; i < n; ++i ) delete[] args[ i ];
      delete[] args;

      return StatusCode::FAILURE;
   } else if ( m_garbagePID == 0 ) {

      // close the wrong end of the pipe
      close( pipefds[ 0 ] );

      // The setsid() function creates a new session;
      if( setsid() < 0 ) {
         write( pipefds[ 1 ], &errno, sizeof( int ) );
         _exit( 0 );
      }

      execvp( m_garbageCommand.c_str(), args );

      // exec should not return, but we got something.
      // delete memory reserved for args
      for ( size_t i = 0; i < n; ++i ) delete[] args[ i ];
      delete[] args;

      // send what we go to the parent.
      write( pipefds[ 1 ], &errno, sizeof( int ) );
      _exit( 0 );
   } else {
      // delete memory reserved for args
      for ( size_t i = 0; i < n; ++i ) delete[] args[ i ];
      delete[] args;

      int count = 0, err = 0;
       
      // Close the wrong end of the pipe.
      close( pipefds[ 1 ] );

      // Check if we have something
      while ( ( count = read( pipefds[ 0 ], &err, sizeof( errno ) ) ) == -1 ) {
         if ( errno != EAGAIN && errno != EINTR ) break;
      }
      if ( count ) {
         error() << "execvp failed with error: " << strerror( err ) << endmsg;
         return StatusCode::FAILURE;
      }
      close( pipefds[ 0 ] );
      return StatusCode::SUCCESS;
   }
}

//=============================================================================
File* FileStagerSvc::createFile( const string& filename )
{
   string file( filename );
   string remote( filename );
   string command;
   bool success = false;
   boost::iterator_range< string::iterator > result;
   if ( result = ba::find_first( file, "LFN:" ) ) {
      // convert LFN to lowercase
      ba::erase_range( file, result );
      remote = file;
      success = createLFN( remote, command );
   } else if ( result = ba::find_first( file, "PFN:" ) ) {
      // strip PFN
      ba::erase_range( file, result );
      remote = file;
      success = createPFN( remote, command );
   } else {
      success = createPFN( remote, command );
   }

   if ( !success ) {
      warning() << "Error manipulating the original descriptor: " << filename 
                << " into a suitable remote filename" << endmsg;
      File* f = new File( filename, "", "", "" );
      f->setGood( false );
      return f;
   }

   boost::hash< string > hash;
   fs::path p( remote );
   stringstream temp;
   string extension = p.extension();
   if ( result = ba::find_first( extension, "?" ) ) {
      boost::iterator_range< string::iterator > range( result.begin(), extension.end() );
      ba::erase_range( extension, range );
   }

   temp << m_tmpdir << "/" << boost::format( "%|x|" ) % hash( remote ) << extension;

   File* f = new File( file, command, remote, temp.str() );
   if ( !f->exists() ) {
      warning() << "stat failed for: " << remote << endmsg
                << "Either the remote file does not exist, or there are"
                << " grid related problems, proxy for example" << endmsg;
      f->setGood( false );
   } else {
      verbose() << "Created file: " << file << " " << command 
                << " " << remote << " " << temp.str() << endmsg;
      f->setGood( true );
   }
   return f;
}

//=============================================================================
bool FileStagerSvc::createPFN( string& remote, string& command )
{
   boost::match_flag_type flags = match_default;
   boost::match_results< string::iterator > match;
   boost::regex re( "^(root:)((?:root:)|(?:dcap:))" );

   boost::iterator_range< string::iterator > result;
   if ( result = ba::find_first( remote, "gfal:" ) ) {
      // strip gfal:
      ba::erase_range( remote, result );
   } else if ( regex_search( remote.begin(), remote.end(), match, re, flags ) ) {
      // in case of root:root: or root:dcap, remove the first root
      boost::iterator_range< string::iterator >
         range( match[ 1 ].first, match[ 1 ].second );
      ba::erase_range( remote, range );
   } else if ( result = ba::find_first( remote, "mdf:" ) ) {
      // strip mdf:
      ba::erase_range( remote, result );
   }
   
   re = "^((?:rfio)|(?:castor)|(?:root):)//(?:\\w\\.\\w\\.\\w:\\d+/+castor)";
   if ( regex_search( remote.begin(), remote.end(), match, re, flags ) ) {
      boost::iterator_range< string::iterator >
         range( match[ 1 ].first, match[ 1 ].second );
      ba::replace_range( remote, range, "rfio:" );
      command = "rfcp";
      return true;
   } else if ( result = ba::find_first( remote, "castor:" ) ) {
      // erase castor
      ba::erase_range( remote, result );
      command = "rfcp";
      return true;
   } else if ( result = ba::find_first( remote, "rfio:" ) ) {
      // erase rfio
      ba::erase_range( remote, result );
      command = "rfcp";
      return true;
   } else if ( result = ba::find_first( remote, "/castor" ) ) {
      // castor file, no protocol specification
      command = "rfcp";
      return true;
   } else if ( result = ba::find_first( remote, "srm:" ) ) {
      // srm does not need changes, command is lcg-cp
      command = "lcg-cp -V lhcb";
      return true;
   } else if ( result = ba::find_first( remote, "dcap:" ) ) {
      // gsidcap or dcap needs no changes, command is dccp
      command = "dccp -A";
      return true;
   } else if ( m_stageLocalFiles && ( result = ba::find_first( remote, "file:" ) ) ) {
      // local file, perhaps nfs or other networked filesystem
      ba::erase_range( remote, result );
      command = "cp";
      return true;
   } else if ( m_stageLocalFiles ) {
      // assume local file, perhaps nfs or other networked filesystem
      command = "cp";
      return true;
   }
   return false;
}

//=============================================================================
bool FileStagerSvc::createLFN( string& remote, string& command )
{
   // The input are only lfns which are not in the catalog.

   boost::iterator_range< string::iterator > result;
   if ( result = ba::find_first( remote, "gfal:" ) ) {
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

#else
// Dummy implementations for compilation on Windows
//=============================================================================
StatusCode FileStagerSvc::initialize()
{
   error() << "The FileStager does not work on Windows, please disable it." << endmsg;
   return StatusCode::FAILURE;
}

//=============================================================================
StatusCode FileStagerSvc::finalize()
{
   return Service::finalize();
}

//=============================================================================
StatusCode FileStagerSvc::getLocal( const string&, string& )
{
   return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode FileStagerSvc::addFiles( const vector< string >& )
{
   return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode FileStagerSvc::clearFiles()
{
   return StatusCode::SUCCESS;
}

//=============================================================================
void FileStagerSvc::stage()
{

}

//=============================================================================
boost::uintmax_t FileStagerSvc::diskspace() const
{
   return 0;
}

//=============================================================================
void FileStagerSvc::restartStaging( const string& )
{

}

//=============================================================================
void FileStagerSvc::removeFile( const_original_iterator )
{

}

//=============================================================================
void FileStagerSvc::removeFiles()
{

}

//=============================================================================
void FileStagerSvc::removePrevious( const_original_iterator )
{

}

//=============================================================================
StatusCode FileStagerSvc::garbage()
{
   return StatusCode::SUCCESS;
}

//=============================================================================
File* FileStagerSvc::createFile( const string& )
{
   return 0;
}

//=============================================================================
bool FileStagerSvc::createPFN( string&, string& )
{
   return false;
}

//=============================================================================
bool FileStagerSvc::createLFN( string&, string& )
{
   return false;
}
#endif
