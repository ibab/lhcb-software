#ifndef TALIGNMENT_ALIGNONLINEXMLCOPIER_H
#define TALIGNMENT_ALIGNONLINEXMLCOPIER_H

#include <string>
#include <ctime>
#include "GaudiKernel/StatusCode.h"

class AlignOnlineXMLCopier
{
public:
  typedef int FileVersion ;
  AlignOnlineXMLCopier( const std::string& onlinedir,
			const std::string& aligndir,
			const std::string& condname ) ;
  StatusCode copyFromOnlineArea() ;
  StatusCode copyToOnlineArea() ;

  std::string alignfilename() const ;
  std::string aligndirname() const ;
  FileVersion version() const { return m_version ; }
  std::string onlinefilename() const { return onlinefilename(m_version) ; }
  std::string copybackfilename() const {return m_newfilename;}
  std::string condition () const {return m_condname;}
private:
  std::string onlinefilename( FileVersion v ) const ;

private:
  std::string m_condname ;  // e.g. "Velo/VeloGlobal"
  std::string m_onlinedir ;
  std::string m_aligndir ;
  FileVersion m_version ;
  std::time_t m_time ;
  std::string m_newfilename;
  std::string m_filename;
} ;

#endif
