#include "GaudiKernel/MsgStream.h"

#include "AlignOnlineXMLCopier.h"
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <fstream>

namespace {
   namespace fs = boost::filesystem;
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

std::string AlignOnlineXMLCopier::onlinedirname() const
{
  return m_onlinedir + "/" + m_condname;
}

std::string AlignOnlineXMLCopier::onlinefilename( FileVersion v ) const
{
  return onlinedirname() + "/v" + std::to_string(v) + ".xml" ;
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
   // Regex and lambda to extract version from filename vN.xml
   boost::regex re("^v([0-9]+)\\.xml$");
   auto version = [re, this](const std::string& filename) {
      boost::smatch matches;
      auto flags = boost::match_default;
      if (!fs::is_regular_file(fs::path{onlinedirname()} / fs::path{filename}))
         return 0u;
      if (boost::regex_match(begin(filename), end(filename), matches, re, flags)) {
         std::cout << "dir " << onlinedirname() << " filename " << filename << " matches [1] " << matches[1] << std::endl;
         return boost::lexical_cast<unsigned int>(matches[1]);
      } else {
         return 0u;
      }
   };

   std::vector<unsigned int> versions;
   // Store versions
   std::transform(fs::directory_iterator{fs::path{onlinedirname()}},
                  fs::directory_iterator{}, std::inserter(versions, end(versions)),
                  [version](const fs::directory_entry& e) {
                     return version(e.path().filename().string());
                  });

   // Find highest version
   auto v = std::max_element(begin(versions), end(versions));
   if (v == end(versions)) {
      m_version = 0;
      return StatusCode::FAILURE;
   } else {
      m_version = *v;
      m_filename = onlinefilename(m_version) ;
      fs::path origin(m_filename) ;
      fs::path target(alignfilename()) ;
      fs::path aligndir(aligndirname());
      fs::create_directories(aligndir);
      fs::copy_file(origin, target, fs::copy_option::overwrite_if_exists) ;
      if( fs::exists(target) ){
         m_time = fs::last_write_time(target) ;
         msg(MSG::INFO) << "AlignOnlineXMLCopier: copied " << origin.native()
                        << " to " << target.native() << endmsg;
         return StatusCode::SUCCESS ;
      } else {
         msg(MSG::WARNING) << "AlignOnlineXMLCopier: failed to copy " << origin.native()
                           << " to " << target.native() << endmsg;
         return StatusCode::FAILURE ;
      }
   }
}

StatusCode AlignOnlineXMLCopier::copyToOnlineArea()
{
  StatusCode sc= StatusCode::SUCCESS ;
  fs::path origin(alignfilename()) ;
  if( fs::exists(origin) &&
      fs::last_write_time(origin) > m_time ) {
    // first check that the new file is not identical to the old one
    static size_t headersize = 5 ;
    if( !filesAreIdentical( origin.c_str(), onlinefilename(m_version).c_str(), headersize ) ) {
      // update the version number
      m_version += 1 ;
      fs::path target(onlinefilename(m_version)) ;
      msg(MSG::INFO) << "AlignOnlineXMLCopier: copying " << origin << " to "
                     << target << "." << endmsg;
      fs::copy( origin, target ) ;
      if( !fs::exists(target) ) {
        sc = StatusCode::FAILURE ;
        m_version -= 1 ;
      }
    }
    m_newfilename = onlinefilename(m_version) ;
  }
  return sc ;
}
