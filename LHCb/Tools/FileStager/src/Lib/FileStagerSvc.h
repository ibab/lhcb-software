// $Id: $
#ifndef FILESTAGER_H
#define FILESTAGER_H 1

#include <vector>
#include <string>

// boost
#include <boost/filesystem.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/sequenced_index.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/foreach.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <boost/cstdint.hpp>

// Gaudi
#include <GaudiKernel/Service.h>
#include <GaudiUtils/IIODataManager.h>

// FileStager
#include <FileStager/IFileStagerSvc.h>

// local
#include "File.h"

/** @class FileStagerSvc FileStagerSvc.h
 *
 *
 *  @author Roel Aaij
 *  @date   2009-11-21
 */
class FileStagerSvc : public extends1< Service, IFileStagerSvc > {
public:

   FileStagerSvc( const std::string& name, ISvcLocator* svcLoc );

   virtual~ FileStagerSvc( );

   virtual StatusCode initialize();

   virtual StatusCode finalize();

   virtual StatusCode getLocal( const std::string& filename, std::string& local );

   virtual StatusCode addFiles( const std::vector< std::string >& files );

   virtual StatusCode clearFiles();

private:

   class FileWrapper {
   public:

      FileWrapper( File* file )
         : m_file( file )
      {

      }

      FileWrapper( const FileWrapper& other )
         : m_file( other.m_file )
      {

      }

      ~FileWrapper()
      {

      }

      File* file() const
      {
         return m_file;
      }

      const std::string& original() const
      {
         return m_file->original();
      }

   private:

      // Private methods
      FileWrapper& operator=( const FileWrapper& other );

      File* m_file;

   };

   struct originalTag{ };
   struct listTag{ };

   typedef boost::multi_index_container<
      FileWrapper,
      boost::multi_index::indexed_by<
         boost::multi_index::hashed_unique<
            boost::multi_index::tag< originalTag >,
            boost::multi_index::const_mem_fun< FileWrapper, const std::string&,
                                               &FileWrapper::original >
            >,
         boost::multi_index::sequenced<
            boost::multi_index::tag< listTag >
            >
         >
      > fileContainer_t;

   typedef fileContainer_t::index< originalTag >::type filesByOriginal_t;
   typedef fileContainer_t::index< listTag >::type filesByPosition_t;

   // iterator shortcuts
   typedef filesByOriginal_t::iterator original_iterator;
   typedef filesByPosition_t::iterator position_iterator;

   // const_iterator shortcuts
   typedef filesByOriginal_t::const_iterator const_original_iterator;
   typedef filesByPosition_t::const_iterator const_position_iterator;

   // Mutexes
   boost::recursive_mutex m_fileMutex;

   // Containers
   mutable fileContainer_t m_files;

   // Data
   boost::thread* m_thread;
   std::string m_stageStart;
   int m_garbagePID;
   const_original_iterator m_stageIt;

   // Services
   SmartIF< Gaudi::IIODataManager > m_dataManager;

   // Properties
   std::string m_tmpdir;
   size_t m_stageNFiles;
   size_t m_tries;
   size_t m_copyTries;
   boost::filesystem::path m_tempdir;
   bool m_initialized;
   bool m_retry;
   bool m_stageLocalFiles;
   std::string m_dataManagerName;
   std::string m_garbageCommand;
   bool m_keepFiles;

   // Helper Methods
   void stage();

   boost::uintmax_t diskspace() const;

   void restartStaging( const std::string& filename );

   void removeFile( const_original_iterator it );

   void removeFiles();

   void removePrevious( const_original_iterator it );
   
   StatusCode garbage();

   File* createFile( const std::string& filename );

   bool createPFN( std::string& remote, std::string& command );

   bool createLFN( std::string& remote, std::string& command );

};
#endif // FILESTAGER_H
