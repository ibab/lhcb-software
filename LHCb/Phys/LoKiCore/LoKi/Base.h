// $Id: Base.h,v 1.4 2007-06-03 20:38:24 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2007/06/01 11:35:26  ibelyaev
//  prepare for v3r0
//
// Revision 1.2  2006/05/02 14:29:09  ibelyaev
//  censored
//
// ============================================================================
#ifndef LOKI_BASE_H 
#define LOKI_BASE_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKiCore 
// ============================================================================
#include "LoKi/IReporter.h"
#include "LoKi/Interface.h"
// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-10
 */
// ============================================================================
namespace LoKi 
{ 
  /** @class Base Base.h LoKi/Base.h
   *  
   *  the base class for complex LoKi objects 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-09
   */
  class Base 
  {
  public: 
    /// get the name of base object 
    inline  const std::string& name     () const { return m_name ; }
    /// get the type of the base object 
    virtual const std::string& type     () const ;
    /// get the pair "type/name" of the object 
    inline  const std::string& typeName () const 
    {
      if ( m_typeName.empty() ) { m_typeName = type() + "/" + name() ; } 
      return m_typeName ;
    } ;
  protected:
    /// Standard constructor
    Base 
    ( const std::string& name     = "<Anonymous>" , 
      const IReporter*   reporter = 0             ) ;
    /// copy constructor 
    Base ( const Base& right ) ;    
    /// destructor: virtual and protected:
    virtual ~Base() ; ///< Destructor
    /// assignement 
    Base& operator=( const Base& ) ;
  protected:
    /** Print the error message and return with the given StatusCode.
     *
     *  Also performs statistical analysis of the error messages and
     *  suppression after the defined number of error instances.
     *
     *  @code
     *
     *   if( a < 0 ) { return Error("a is negative!") ;}
     *   if( b < 0 ) { return Error("b is illegal!" , StatusCode(25) );
     *   if( c < 0 )
     *      { return Error("c is negative" , StatusCode(35) , 50 );
     *
     *  @endcode
     *
     *  @see MsgStream
     *  @see IMessageSvc
     *  @see StatusCode
     *
     *  @param msg    Error message
     *  @param st     StatusCode to return
     *  @param mx     Maximum number of printouts for this message
     *  @return       StatusCode
     */
    StatusCode Error
    ( const std::string& msg ,
      const StatusCode   st  = StatusCode::FAILURE ,
      const size_t       mx  = 10                  ) const ;    
    /** Print the warning message and return with the given StatusCode.
     *
     *  Also performs statistical analysis of the error messages and
     *  suppression after the defined number of error instances.
     *
     *  @code
     *
     *   if( a < 0 ) { return Warning("a is negative!") ;}
     *   if( b < 0 ) { return Warning("b is illegal!" , StatusCode(25) );
     *   if( c < 0 )
     *      { return Warning("c is negative" , StatusCode(35) , 50 );
     *
     *  @endcode
     *
     *  @see MsgStream
     *  @see IMessageSvc
     *  @see StatusCode
     *
     *  @param msg    Warning message
     *  @param st     StatusCode to return
     *  @param mx     Maximum number of printouts for this message
     *  @return       The given StatusCode
     */
    StatusCode Warning
    ( const std::string& msg ,
      const StatusCode   st  = StatusCode::FAILURE ,
      const size_t       mx  = 10                  ) const ;
    /** Print the message and return status code 
     *  @see MsgStream
     *  @see IMessageSvc 
     *  @see StatusCode 
     *  @param msg    warning message 
     *  @param st     status code 
     *  @param lev    print level 
     */
    void Print     
    ( const std::string& msg , 
      const StatusCode   st  = StatusCode::SUCCESS ,
      const MSG::Level   lev = MSG::INFO           ) const ;    
    /** Assertion - throw exception, if condition is not fulfilled 
     *  @see CaloException
     *  @see GaudiException
     *  @exception CaloException for invalid condifition 
     *  @param ok           condition which should be "true"
     *  @param message      message to be associated with the exception 
     *  @param sc           status code to be returned (artificial) 
     *  @return             status code        
     */ 
    inline void Assert 
    ( const bool         ok                            , 
      const std::string& message = ""                  , 
      const StatusCode   sc      = StatusCode::FAILURE ) const 
    {
      if ( !ok ) { Exception ( message , sc ) ; } ; sc.ignore() ;
    } 
    /** Assertion - throw exception, if condition is not fulfilled 
     *  @see CaloException
     *  @see GaudiException
     *  @exception CaloException for invalid condifition 
     *  @param ok           condition which should be "true"
     *  @param message      message to be associated with the exception 
     *  @param sc           status code to be returned (artificial) 
     *  @return             status code        
     */ 
    inline void Assert 
    ( const bool         ok                            , 
      const char*        message                       , 
      const StatusCode   sc      = StatusCode::FAILURE ) const 
    {
      if ( !ok ) { Exception ( message , sc ) ; } ; sc.ignore() ;
    } 
    /** Create and (re)-throw the exception  
     *  @see GaudiException
     *  @exception CaudiException always!
     *  @param msg    exception message 
     *  @param exc    (previous) exception of type GaudiException
     *  @param sc     status code  
     */
    void Exception 
    ( const std::string    & msg                        ,  
      const GaudiException & exc                        , 
      const StatusCode       sc  = StatusCode::FAILURE  ) const ;
    /** Create and (re)-throw the exception  
     *  @see GaudiException
     *  @exception GaudiException always!
     *  @param msg    exception message 
     *  @param exc    (previous) exception of type std::exception
     *  @param sc     status code  
     */
    void Exception 
    ( const std::string    & msg                        ,  
      const std::exception & exc                        , 
      const StatusCode       sc  = StatusCode::FAILURE  ) const ;
    /** Create and throw the exception  
     *  @see GaudiException
     *  @exception GaudiException always!
     *  @param msg    exception message 
     *  @param sc     status code  
     */
    void Exception 
    ( const std::string& msg = "no message"        ,  
      const StatusCode   sc  = StatusCode::FAILURE ) const ;
  public:
    /// get the currect reference counter 
    long refCount   () const { return   m_refCount ; }
    /// increment the reference counter 
    long addRef     ()       { return ++m_refCount ; }
    /// decreemnt the reference counter 
    long release    ()   ; ///< ATTENTION! object is deleted if refCount==0
  private:
    // object name 
    std::string                      m_name     ; ///<  object name 
    // "type/name" pair 
    mutable std::string              m_typeName ; ///< "type/name" pair 
    // own problem reported 
    LoKi::Interface<LoKi::IReporter> m_reporter ; ///< own problem reported 
    // reference counter 
    long                             m_refCount ; ///< reference counter 
  } ;
}  // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_BASE_H
// ============================================================================

