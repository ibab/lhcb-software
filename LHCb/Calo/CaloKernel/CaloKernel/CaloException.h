// $Id: CaloException.h,v 1.2 2003-01-17 14:15:18 sponce Exp $
// ===========================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2001/11/25 14:07:38  ibelyaev
// New Package: substitution of the  previous CaloGen package
//
// Revision 1.4  2001/07/02 17:49:10  ibelyaev
// update in CaloException and CaloVector
//
// Revision 1.3  2001/06/23 14:39:31  ibelyaev
//  fix CVS-keywords and remove long lines
// 
// ===========================================================================
#ifndef      CALOKERNEL_CALOEXCEPTION_H  
#define      CALOKERNEL_CALOEXCEPTION_H 1 
/// ===========================================================================
// GaudiKernel
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/GaudiException.h"

/** @class CaloException CaloException.h CaloKernel/CaloException.h
 *
 *  It is just a base class for all exceptions in the Calorimeter system     
 *  
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date    26/11/1999
 */

class CaloException : public GaudiException
{  
public:

  /** constructor 
   *  @param ex  exception message 
   *  @param sc  status code assiciated with exception
   */ 
  CaloException( const std::string& ex   = "unspecified exception", 
                 const StatusCode&  sc   =  StatusCode::FAILURE )
    : GaudiException( ex , "*CaloException*" , sc )  {}; 

  /** constructor
   *  @param ex          exception message 
   *  @param Exception   previous exception 
   *  @param sc          status code associated with the exception
   */  
  CaloException( const std::string   & ex                        , 
                 const GaudiException& Ex                        , 
                 const StatusCode&     sc =  StatusCode::FAILURE ) 
    : GaudiException( ex, "*CaloException*" , sc , Ex ) {};
  
  /** destructor
   */
  virtual ~CaloException() throw() {};
  
  /** clone (virtual constructor)
   */
  virtual GaudiException* clone() const { return new CaloException(*this); }
  
};

// ===========================================================================
// The End
// ===========================================================================
#endif  ///<   CALOKERNEL_CALOEXCEPTION_H
// ===========================================================================

