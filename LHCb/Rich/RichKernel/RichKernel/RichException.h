/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// ===========================================================================
#ifndef      RICHGEN_RICHEXCEPTION_H  
#define      RICHGEN_RICHEXCEPTION_H 1 
/// ===========================================================================

// GaudiKernel
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/GaudiException.h"

/** @class RichException RichException.h RichGen/RichException.h

    It is just a base class for all exceptions in the Rich system     
    
    @author  Andy Presland 
    @date    23/07/2001
*/

class RichException : public GaudiException
{  
  ///
public:
  /**  constructor 
       @param ex  exception message 
       @param sc  status code assiciated with exception
  */ 
  RichException( const std::string& ex   = "unspecified exception", 
                 const StatusCode&  sc   =  StatusCode::FAILURE )
    : GaudiException( ex , "*RichException*" , sc )  {}; 
  /**  constructor
       @param ex          exception message 
       @param Exception   previous exception 
       @param sc          status code associated with the exception
  */  
  RichException( const std::string   & ex                        , 
                 const GaudiException& Ex                        , 
                 const StatusCode&     sc =  StatusCode::FAILURE ) 
    : GaudiException( ex, "*RichException*" , sc , Ex ) {};
  /// destructor 
  virtual ~RichException(){};
  ///
  virtual GaudiException* clone() const 
  { return new RichException(*this); } 
  ///
private:
  ///
};

/// ===========================================================================
#endif  ///<   RICHGEN_RICHEXCEPTION_H
/// ===========================================================================

