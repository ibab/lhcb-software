// $Id: RichException.h,v 1.3 2004-05-13 09:21:17 jonrob Exp $
// ===========================================================================
// $Log: not supported by cvs2svn $
//
// ===========================================================================
#ifndef      RICHKERNEL_RICHEXCEPTION_H
#define      RICHKERNEL_RICHEXCEPTION_H 1

// GaudiKernel
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/GaudiException.h"

/** @class RichException RichException.h RichKernel/RichException.h
 *
 *  Base class for all exceptions in the RICH system
 *
 *  @author  Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date    09/05/2004
 */

class RichException : public GaudiException {

public:

  /** constructor
   *  @param ex  exception message
   *  @param sc  status code assiciated with exception
   */
  RichException( const std::string& ex   = "unspecified exception",
                 const StatusCode&  sc   =  StatusCode::FAILURE )
    : GaudiException( ex , "*RichException*" , sc )  {};

  /** constructor
   *  @param ex          exception message
   *  @param Exception   previous exception
   *  @param sc          status code associated with the exception
   */
  RichException( const std::string   & ex                        ,
                 const GaudiException& Ex                        ,
                 const StatusCode&     sc =  StatusCode::FAILURE )
    : GaudiException( ex, "*RichException*" , sc , Ex ) {};

  /** destructor
   */
  virtual ~RichException() throw() {};

  /** clone (virtual constructor)
   */
  virtual GaudiException* clone() const { return new RichException(*this); }

};

// ===========================================================================
// The End
// ===========================================================================
#endif  ///<   RICHKERNEL_RICHEXCEPTION_H
// ===========================================================================

