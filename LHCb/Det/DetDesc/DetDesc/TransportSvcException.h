// $Id: TransportSvcException.h,v 1.3 2003-04-25 08:52:24 sponce Exp $
#ifndef     __DETDESC_TRANSPORTSVC_TRANSPORTSVCEXCEPTION_H__ 
#define     __DETDESC_TRANSPORTSVC_TRANSPORTSVCEXCEPTION_H__ 1


///
///
///
#include "GaudiKernel/GaudiException.h" 



///
///
///  class TransportSvcException: an exception class for Transport Service
///
///
///

class TransportSvcException: public GaudiException
{
  ///
  ///
  ///

  public: 

  ///
  /// constructor 
  ///
  
  TransportSvcException( const std::string& Message                           , 
                         const StatusCode&  statusCode = StatusCode::FAILURE  ) 
    : GaudiException( Message , "*TransportSvcException*" , statusCode ) 
    ///
    {};
  
  ///
  TransportSvcException( const std::string&    Message                             , 
                         const GaudiException& Exception                           ,
                         const StatusCode&     statusCode = StatusCode::FAILURE    ) 
    : GaudiException( Message , "*TransportSvcException*" , statusCode , Exception )
    {};
  
  ///
  /// nothing specific (yet) :-))) 
  ///
  
  virtual ~TransportSvcException() throw() {};

  ///
  ///

};

///
///
///



#endif  //  __DETDESC_TRANSPORTSVC_TRANSPORTSVCEXCEPTION_H__ 
