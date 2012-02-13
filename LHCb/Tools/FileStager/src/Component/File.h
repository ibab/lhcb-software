#ifndef FILE_H
#define FILE_H 1

// boost
#include <boost/optional.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>
#include <boost/cstdint.hpp>

/** @class File File.h
 *
 *
 *  @author Roel Aaij
 *  @date   2009-11-22
 */
class File {
public: 
   
   File( const std::string& original, const std::string& command,
         const std::string& remote, const std::string& temp );

   virtual ~File();

   const std::string& original() const
   {
      return m_original;
   }

   const std::string& command() const
   {
      return m_command;
   }

   const std::string& remote() const
   {
      return m_remote;
   }

   const std::string& temporary() const
   {
      return m_temporary;
   }

   boost::mutex& openMutex()
   {
      return m_openMutex;
   }

   const boost::mutex& openMutex() const
   {
      return m_openMutex;
   }

   boost::mutex& closeMutex()
   {
      return m_closeMutex;
   }

   const boost::mutex& closeMutex() const
   {
      return m_closeMutex;
   }

   bool staged() const
   {
      boost::lock_guard< boost::mutex > lock( m_internalMutex );
      return m_staged;
   }

   void setStaged( bool staged )
   {
      boost::lock_guard< boost::mutex > lock( m_internalMutex );
      m_staged = staged;
   }

   bool good() const
   {
      boost::lock_guard< boost::mutex > lock( m_internalMutex );
      return m_good;
   }

   void setGood( bool good )
   {
      boost::lock_guard< boost::mutex > lock( m_internalMutex );
      m_good = good;
   }

   boost::condition_variable& openCondition()
   {
      boost::lock_guard< boost::mutex > lock( m_internalMutex );
      return m_openCondition;
   }

   const boost::condition_variable& openCondition() const
   {
      boost::lock_guard< boost::mutex > lock( m_internalMutex );
      return m_openCondition;
   }

   boost::condition_variable& closeCondition()
   {
      boost::lock_guard< boost::mutex > lock( m_internalMutex );
      return m_closeCondition;
   }

   const boost::condition_variable& closeCondition() const
   {
      boost::lock_guard< boost::mutex > lock( m_internalMutex );
      return m_closeCondition;
   }

   bool exists() const;

   boost::uintmax_t size() const;

private:

   File& operator=( const File& other );
   File( const File& other );

   // Data members
   // The original name of the file
   const std::string m_original;
   // The command to be executed to stage this file
   const std::string m_command;
   // The filename as passed to lcg-cp
   const std::string m_remote;
   // The name of the temporary file
   const std::string m_temporary;

   bool m_staged;
   bool m_good;
   mutable boost::optional< boost::uintmax_t > m_size;

   mutable boost::mutex m_internalMutex;
   boost::mutex m_openMutex;
   boost::mutex m_closeMutex;
   boost::condition_variable m_openCondition;
   boost::condition_variable m_closeCondition;

};
#endif // STAGER_FILE_H
