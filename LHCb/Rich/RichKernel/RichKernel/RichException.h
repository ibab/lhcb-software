
//-----------------------------------------------------------------------------
/** @file RichException.h
 *
 *  Header file for utility class : Rich::Exception
 *
 *
 *  @author  Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date    09/05/2004
 */
//-----------------------------------------------------------------------------

#ifndef      RICHKERNEL_RICHEXCEPTION_H
#define      RICHKERNEL_RICHEXCEPTION_H 1

// GaudiKernel
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/GaudiException.h"

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class Exception RichException.h RichKernel/RichException.h
   *
   *  Base class for all exceptions in the RICH system
   *
   *  @author  Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date    09/05/2004
   */
  //-----------------------------------------------------------------------------

  class Exception : public GaudiException
  {

  public:

    /** Default Constructor
     *
     *  @param ex  exception message
     *  @param sc  status code associated with the exception
     */
    Exception( const std::string& ex   = "unspecified exception",
               const StatusCode&  sc   =  StatusCode::FAILURE )
      : GaudiException( ex , "*RichException*" , sc )  {};

    /** Constructor from another GaudiException
     *
     *  @param ex          exception message
     *  @param Ex          previous exception
     *  @param sc          status code associated with the exception
     */
    Exception( const std::string    & ex                        ,
               const GaudiException & Ex                        ,
               const StatusCode&      sc =  StatusCode::FAILURE )
      : GaudiException( ex, "*RichException*" , sc , Ex ) {};

    /** destructor
     */
    virtual ~Exception() throw() {};

    /** clone (virtual constructor)
     */
    virtual GaudiException* clone() const { return new Exception(*this); }

  };

}

#endif  ///<   RICHKERNEL_RICHEXCEPTION_H


