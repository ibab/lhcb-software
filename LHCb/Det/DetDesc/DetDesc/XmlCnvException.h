/// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/XmlCnvException.h,v 1.1.1.1 2000-11-23 13:44:44 ranjard Exp $
#ifndef DETDESC_XMLCNVSVC_XMLCNVEXCEPTION_H
#define DETDESC_XMLCNVSVC_XMLCNVEXCEPTION_H 1

/// Include files
#include "GaudiKernel/Kernel.h"

#include <string>

/// Error codes used for XmlCnvException handling
enum Status {  
  CANT_QUERY_INTERFACE = 2,
  CANT_RETRIEVE_OBJECT,
  INVALID_CLASS_ID,
  CORRUPTED_DATA,
  STOP_REQUEST,
  EXPRESSION_ERROR,
  WRONG_DTD_VERSION,
  ERROR_ADDING_TO_TS,
  LAST
};

class XmlCnvException
{
public:

  XmlCnvException( const char* msg, StatusCode& sc )
  : m_status( sc )
  {
    m_msg  = msg;
    m_type = "XmlCnvException";
  }

  const char* getMessage() const
  {
    return( m_msg.c_str() );
  }
  
  const char* getType() const
  {
    return( m_type.c_str() );
  }

  StatusCode status() const
  {
    return m_status;
  }

private:

  StatusCode  m_status;
  std::string m_msg;
  std::string m_type;
};

#endif // DETDESC_XMLCNVSVC_XMLCNVEXCEPTION_H
