/** @file RichException.h
 *
 *  Header file for utility class : RichException
 *
 *  CVS Log :-
 *  $Id: RichException.h,v 1.6 2005-01-18 10:49:40 cattanem Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.5  2004/07/26 17:53:17  jonrob
 *  Various improvements to the doxygen comments
 *
 *  Revision 1.4  2004/07/15 15:36:53  jonrob
 *  Doxygen improvements
 *
 *  Revision 1.3  2004/05/13 09:21:17  jonrob
 *  add RichException
 *
 *  @author  Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date    09/05/2004
 */

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

  /** Default Constructor
   *
   *  @param ex  exception message
   *  @param sc  status code associated with the exception
   */
  RichException( const std::string& ex   = "unspecified exception",
                 const StatusCode&  sc   =  StatusCode::FAILURE )
    : GaudiException( ex , "*RichException*" , sc )  {};

  /** Constructor from another GaudiException
   *
   *  @param ex          exception message
   *  @param Ex          previous exception
   *  @param sc          status code associated with the exception
   */
  RichException( const std::string    & ex                        ,
                 const GaudiException & Ex                        ,
                 const StatusCode&      sc =  StatusCode::FAILURE )
    : GaudiException( ex, "*RichException*" , sc , Ex ) {};

  /** destructor
   */
  virtual ~RichException() throw() {};

  /** clone (virtual constructor)
   */
  virtual GaudiException* clone() const { return new RichException(*this); }

};

#endif  ///<   RICHKERNEL_RICHEXCEPTION_H


