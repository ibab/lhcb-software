#ifndef DETDESCCNV_XMLCNVSVC_XMLCNVEXCEPTION_H
#define DETDESCCNV_XMLCNVSVC_XMLCNVEXCEPTION_H 1
/// Include files
#include <string>
/// GaudiKernel
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/GaudiException.h"


/// Error codes used for XmlCnvException handling
enum Status 
{  
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

///
class XmlCnvException: public GaudiException
{
public: 
  ///
  inline XmlCnvException( const std::string    & message                       , 
                          const StatusCode     & sc      = StatusCode::FAILURE );
  inline XmlCnvException( const std::string    & message                       ,
                          const GaudiException & ge                            ,
                          const StatusCode     & sc      = StatusCode::FAILURE );
  inline XmlCnvException( const XmlCnvException& ) ;
  ///
  virtual inline ~XmlCnvException() throw(); 
  ///
  inline const char*       getMessage () const;
  inline const char*       getType    () const;
  inline const StatusCode& status     () const;
  ///
  virtual inline GaudiException* clone() const; 
  ///
};
///
#include "DetDescCnv/XmlCnvException.icpp"
///


#endif // DETDESCCNV_XMLCNVSVC_XMLCNVEXCEPTION_H














