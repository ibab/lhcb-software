// $Id: CaloAlgorithm.h,v 1.1.1.1 2001-11-25 14:07:38 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2001/11/13 09:34:41  ibelyaev
//  add accessor to IToolSvc interface in CaloAlgorithm base class
//
// Revision 1.4  2001/11/12 19:01:02  ibelyaev
//  minor  reorganization of header files
//
// ============================================================================
#ifndef CALOKERNEL_CALOALGORITHM_H 
#define CALOKERNEL_CALOALGORITHM_H 1
// ============================================================================
// Include files
// from STL
#include <string>
// from Gaudi
#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IMessageSvc.h"
// From CaloKernel 
#include "CaloKernel/CaloException.h"

template<class TYPE>
class         AlgFactory     ;
class         IToolSvc       ;

/** @class  CaloAlgorithm CaloAlgorithm.h CaloKernel/CaloAlgorithm.h
 *   
 *  The useful base class for calo algorithm.
 *  small externsion of ordinary algorithm base class
 *  Extensions:
 *  useful protected print methods, exceptions and assertions
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   30/06/2001
 */

class CaloAlgorithm : public Algorithm 
{
public:
  
  /** standard initialization method
   *  @return status code 
   */
  virtual StatusCode initialize(); 
  
  /** standard execution method
   *  @return status code 
   */
  virtual StatusCode execute   (); 
  
  /** standard finalization method
   *  @return status code 
   */
  virtual StatusCode finalize  (); 
  
  /** accessor to Tool Service
   *  @return pointer to Tool Service 
   */
  inline IToolSvc* toolSvc() const 
  { return m_toolSvc; }

protected:
  
  /** Standard constructor (protected)
   *  @param name           name of the algorithm
   *  @param pSvcLocator    poinetr to Service Locator 
   */
  CaloAlgorithm( const std::string& name        , 
                 ISvcLocator*       pSvcLocator );
  
  /** destructor, virtual and protected 
   */  
  virtual ~CaloAlgorithm();
  
protected: ///< "technical" methods 
  
  /** Print the error  message, return status code
   *  and perform the statistics of error messages 
   *  @param msg    error message 
   *  @param st     status code 
   *  @return       status code 
   */
  StatusCode 
  Error     ( const std::string& msg , 
              const StatusCode & st  = StatusCode::FAILURE ) const ;
  
  /** Print the warning  message, return status code 
   *  and perform the statistics of warning  messages 
   *  @param msg    warning message 
   *  @param st     statsu code 
   *  @return       status code 
   */
  StatusCode 
  Warning   ( const std::string& msg , 
              const StatusCode & st  = StatusCode::FAILURE ) const ; 
  
  /** Print the message and return status code 
   *  @param msg    warning message 
   *  @param st     status code 
   *  @param lev    print level 
   *  @return       status code 
   */
  StatusCode 
  Print     ( const std::string& msg , 
              const StatusCode & st  = StatusCode::FAILURE ,
              const MSG::Level & lev = MSG::INFO           ) const ;
  
  /** Assertion - throw exception, if condition is not fulfilled 
   *  @param ok            condition which should be "true"
   *  @param mesage       message to eb associated with the exception 
   *  @return             status code        
   */ 
  inline StatusCode 
  Assert ( bool ok , const std::string& message = " ") const;
  
  /** Assertion - throw exception, if condition is not fulfilled 
   *  @param ok            condition which shoudl be "true"
   *  @param mesage       message to eb associated with the exception 
   *  @return             status code        
  */ 
  inline StatusCode
  Assert ( bool ok , const char*        message  ) const ;
  
  /** Create and (re)-throw the exception  
   *  @param msg    exception message 
   *  @param exc    (previous) exception of type GaudiException
   *  @param lvl    print level 
   *  @param sc     status code  
   *  @return       status code (fictive) 
   */
  StatusCode 
  Exception ( const std::string    & msg                        ,  
              const GaudiException & exc                        , 
              const MSG::Level     & lvl = MSG::FATAL           ,
              const StatusCode     & sc  = StatusCode::FAILURE ) const ;
  
  /** Create and (re)-throw the exception  
   *  @param msg    exception message 
   *  @param exc    (previous) exception of type std::exception
   *  @param lvl    print level 
   *  @param sc     status code  
   *  @return       status code (fictive) 
   */
  StatusCode 
  Exception ( const std::string    & msg                        ,  
              const std::exception & exc                        , 
              const MSG::Level     & lvl = MSG::FATAL           ,
              const StatusCode     & sc  = StatusCode::FAILURE ) const ;
  
  /** Create and throw the exception  
   *  @param msg    exception message 
   *  @param lvl    print level 
   *  @param sc     status code  
   *  @return       status code (fictive) 
   */
  StatusCode 
  Exception ( const std::string& msg = "no message"        ,  
              const MSG::Level & lvl = MSG::FATAL          ,
              const StatusCode & sc  = StatusCode::FAILURE ) const ;
  
  /** address/location/name in Transient Store of input  data container 
   */
  const std::string& inputData  () const { return m_inputData  ; }
  
  /** address/location/name in Transient Store of output data container 
   */
  const std::string& outputData () const { return m_outputData ; }
  
  /** address/location/name in Transient Store of detector data  
   */
  const std::string& detData    () const { return m_detData    ; }
  
  /** set address/location/name in Transient Store of input  data container 
   *  @param  addr address/location/name in Transient Store of input data 
   */
  void setInputData ( const std::string& addr ) { m_inputData  = addr; }
  
  /** set address/location/name in Transient Store of output  data container 
   *  @param  addr address/location/name in Transient Store of output data 
   */
  void setOutputData( const std::string& addr ) { m_outputData = addr; }  
  
  /** set address/location/name in Transient Store of detector data  
   *  @param  addr address/location/name in Transient Store of detector 
   */
  void setDetData   ( const std::string& addr ) { m_inputData  = addr; }
  
private:
  
  /// no default constructor 
  CaloAlgorithm();
  /// no copy constructor 
  CaloAlgorithm( const CaloAlgorithm& );
  /// no assignment oprtator  
  CaloAlgorithm& operator=( const CaloAlgorithm& );
  
private:
  
  /// address/location/name in Transient Store of input  data container 
  std::string      m_inputData  ;
  /// address/location/name in Transient Store of output data container 
  std::string      m_outputData ;
  /// address/location/name in Transient Store of detector data  
  std::string      m_detData    ;

  /// pointer to Tool Service 
  IToolSvc*        m_toolSvc    ;
  
};

// ============================================================================
/** Assertion - throw exception, if condition is not fulfilled 
 *  @param OK           condition which should be "true"
 *  @param mesage       message to eb associated with the exception 
 *  @return             status code        
 */ 
// ============================================================================
inline StatusCode 
CaloAlgorithm::Assert ( bool OK , const std::string& msg ) const
{
  StatusCode ok ( StatusCode::SUCCESS );
  return OK ? ok : Exception( msg , MSG::FATAL ) ; 
};

// ============================================================================
/** Assertion - throw exception, if condition is not fulfilled 
 *  @param OK           condition which should be "true"
 *  @param mesage       message to eb associated with the exception 
 *  @return             status code        
 */ 
// ============================================================================
inline StatusCode
CaloAlgorithm::Assert ( bool OK , const char*        msg  ) const
{ 
  StatusCode ok ( StatusCode::SUCCESS ) ;
  return OK ? ok : Exception( msg , MSG::FATAL ) ; 
};


// ============================================================================
#endif // CALOALGORITHM_H 
// ============================================================================

