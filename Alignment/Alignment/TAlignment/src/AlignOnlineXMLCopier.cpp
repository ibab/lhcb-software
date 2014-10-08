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
  return m_aligndir + "/" + m_condname + "/current.xml" ;
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
    boost::filesystem::copy_file(origin,target, boost::filesystem::copy_option::overwrite_if_exists) ;
    if( boost::filesystem::exists(target) ){
      m_time = boost::filesystem::last_write_time(target) ;
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
    boost::filesystem::copy( origin, target ) ;
    if( !boost::filesystem::exists(target) ) {
      sc = StatusCode::FAILURE ;
    }
  }
  return sc ;
}

