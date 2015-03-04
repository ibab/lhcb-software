#include "GaudiKernel/MsgStream.h"

#include "AlignOnlineXMLCopier.h"
#include <boost/filesystem.hpp>
#include <fstream>

namespace {
  // check that two files are identical. numlinestoskip is the number
  // of lines in the header, which is ignored from the comparison
  bool filesAreIdentical( const char* filename1, const char* filename2, size_t numlinestoskip )
  {
    bool identical = false ;
    
    std::ifstream file1(filename1) ;
    std::ifstream file2(filename2) ;
    if( file1 && file2 ) {
    
      // compare every line, but skip the header
      size_t numlines(0) ;
      identical = true ;
      bool ready = false ;
      std::string line1,line2;
      while(identical && !ready) {
        getline(file1,line1) ;
        getline(file2,line2) ;
        // flag ready when both files are finished
        ready = file1.eof() && file2.eof() ;
        if(!ready) {
          identical = 
            // flag false if we have reached the end of one of the two files
            !file1.eof() && !file2.eof() 
            // flag false if lines differ
            && (++numlines <= numlinestoskip || line1==line2) ;
        }
      }
    }
    return identical ;
  }
}

AlignOnlineXMLCopier::AlignOnlineXMLCopier( const std::string& onlinedir,
                                            const std::string& aligndir,
                                            const std::string& condname,
                                            MsgStream* msg_stream)
  : m_condname(condname),
    m_onlinedir(onlinedir),
    m_aligndir(aligndir),
    m_msg_stream(msg_stream),
    m_version(0)
{}

std::string AlignOnlineXMLCopier::onlinefilename( FileVersion v ) const
{
  return m_onlinedir + "/" + m_condname + "/v" + std::to_string(v) + ".xml" ;
}

std::string AlignOnlineXMLCopier::alignfilename() const
{
  return m_aligndir + "/" + m_condname + ".xml" ;
}

std::string AlignOnlineXMLCopier::aligndirname() const
{
  return m_aligndir + "/" + m_condname;
}

StatusCode AlignOnlineXMLCopier::copyFromOnlineArea()
{
  StatusCode sc = StatusCode::SUCCESS ;
  FileVersion v = 0 ;
  while(1) {
    // check that the file exists
    boost::filesystem::path fn(onlinefilename( v )) ;
    if( boost::filesystem::exists(fn) ) ++v ;
    else break ;
  }
  if( v==0 ) {
    m_version = v ;
    sc = StatusCode::FAILURE ;
  } else {
    m_version = v-1 ;
    m_filename = onlinefilename(m_version) ;
    boost::filesystem::path origin(m_filename) ;
    boost::filesystem::path target(alignfilename()) ;
    boost::filesystem::path aligndir(aligndirname());
    boost::filesystem::create_directories(aligndir);
    boost::filesystem::copy_file(origin, target, boost::filesystem::copy_option::overwrite_if_exists) ;
    if( boost::filesystem::exists(target) ){
      m_time = boost::filesystem::last_write_time(target) ;
      msg(MSG::INFO) << "AlignOnlineXMLCopier: copied " << origin.native()
                     << " to " << target.native() << endmsg;
    } else {
      msg(MSG::WARNING) << "AlignOnlineXMLCopier: failed to copy " << origin.native()
                        << " to " << target.native() << endmsg;
      sc = StatusCode::FAILURE ;
    }
  }
  return sc ;
}

StatusCode AlignOnlineXMLCopier::copyToOnlineArea()
{
  StatusCode sc= StatusCode::SUCCESS ;
  boost::filesystem::path origin(alignfilename()) ;
  if( boost::filesystem::exists(origin) &&
      boost::filesystem::last_write_time(origin) > m_time ) {
    // first check that the new file is not identical to the old one
    static size_t headersize = 5 ;
    if( !filesAreIdentical( origin.c_str(), onlinefilename(m_version).c_str(), headersize ) ) {
      // update the version number
      m_version += 1 ;
      boost::filesystem::path target(onlinefilename(m_version)) ;
      msg(MSG::INFO) << "AlignOnlineXMLCopier: copying " << origin << " to "
                     << target << "." << endmsg;
      boost::filesystem::copy( origin, target ) ;
      if( !boost::filesystem::exists(target) ) {
        sc = StatusCode::FAILURE ;
        m_version -= 1 ;
      } 
    }
    m_newfilename = onlinefilename(m_version) ;
  }
  return sc ;
}


