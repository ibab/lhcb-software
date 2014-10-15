#include "AlignOnlineXMLCopier.h"
#include <boost/filesystem.hpp>

AlignOnlineXMLCopier::AlignOnlineXMLCopier( const std::string& onlinedir,
					    const std::string& aligndir,
					    const std::string& condname)
  : m_condname(condname),
    m_onlinedir(onlinedir),
    m_aligndir(aligndir),
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
    boost::filesystem::path origin(onlinefilename(m_version)) ;
    boost::filesystem::path target(alignfilename()) ;
    boost::filesystem::path aligndir(aligndirname());
    boost::filesystem::create_directories(aligndir);
    boost::filesystem::copy_file(origin,target, boost::filesystem::copy_option::overwrite_if_exists) ;
    if( boost::filesystem::exists(target) ){
      m_time = boost::filesystem::last_write_time(target) ;
      printf("+++++++++++++++++++++++++++ AlignOnlineXMLCopier: copied %s to %s\n",origin.c_str(),target.c_str());
    } else {
      sc = StatusCode::FAILURE ;
    }
  }
  return sc ;
}

StatusCode AlignOnlineXMLCopier::copyToOnlineArea() const
{
  StatusCode sc= StatusCode::SUCCESS ;
  boost::filesystem::path origin(alignfilename()) ;
  if( boost::filesystem::exists(origin) &&
      boost::filesystem::last_write_time(origin) > m_time ) {
    boost::filesystem::path target(onlinefilename(m_version+1)) ;
    printf("++++++++++++++++++++++++++++ AlignOnlineXMLCopier: copying %s to %s\n",origin.c_str(),target.c_str());
    boost::filesystem::copy( origin, target ) ;
    if( !boost::filesystem::exists(target) ) {
      sc = StatusCode::FAILURE ;
    }
  }
  return sc ;
}


#ifdef LATER
namespace {
  bool filesAreIdentical( const std::string& filename1, const std::string& filename2, size_t numlinestoskip )
  {
    bool identical = false ;
    
    std::ifstream file1(filename1.c_str()) ;
    std::ifstream file2(filename2.c_str()) ;
    if( file1 && file2 ) {

      // compare every line, but skip the header
      const headerlength = 5 ;
      size_t numlines(0) ;
      identical = true ;
      
      std::string line1,line2;
      while(!file1.eof() && identical) {
	getline(file1,line1) ;
	getline(file2,line2) ;
	if(!file1.eof()) {
	  if( ++numlines > numlinestoskip ) {
	    identical = identical && line1 == line2 ;
	  }
	}
      }
    }
    return identical ;
  }
}

#endif
