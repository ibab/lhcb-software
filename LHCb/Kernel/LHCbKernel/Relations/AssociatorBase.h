// $Id: AssociatorBase.h,v 1.2 2002-04-25 08:44:02 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/04/08 14:26:00  ibelyaev
//  new version of 'Relations'-subpackage
//
// Revision 1.1  2002/04/03 15:35:18  ibelyaev
// essential update and redesing of all 'Relations' stuff
// 
// ============================================================================
#ifndef RELATIONS_AssociatorBase_H 
#define RELATIONS_AssociatorBase_H 1
// Include files
#include "Relations/PragmaWarnings.h"
// GaudiKernel
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/IIncidentListener.h"
// forward declarations
//
class IAlgorithm         ; // GaudiKernel 
class IToolSvc           ; // GaudiKernel
class IChronoStatSvc     ; // GaudiKernel
class IDataProviderSvc   ; // GaudiKernel
class IIncidentSvc       ; // GaudiKernel 
class GaudiException     ; // GaudiKernel 

namespace Relations
{
  
  /** @class AssociatorBase AssociatorBase.h Relations/AssociatorBase.h
   *   
   *  The base class for implementation of relation tools  
   * 
   *  @see  AlgTool 
   *  @see IAlgTool
   *  @see IIncidentListener 
   *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
   *  @date   24/03/2002
   */  
  class AssociatorBase:
    public                   AlgTool  ,
    public virtual IIncidentListener 
  {
  public:
    
    /** standard initialization method
     *  @see  AlgTool 
     *  @see IAlgTool 
     *  @return status code 
     */
    virtual StatusCode initialize () ;
    
    /** standard initialization method
     *  @see  AlgTool 
     *  @see IAlgTool 
     *  @return status code 
     */
    virtual StatusCode finalize   () ;
    
    /** handle the incident 
     *  @see IIncidentListener 
     *  @see  Incident 
     *  @see IIncidentSvc  
     *  @see incident incident to be handled 
     */
    virtual void handle( const Incident& incident ) ;
    
  protected:
    
    /** Standard constructor
     *  @param type   tool type (?) 
     *  @param name   tool name 
     *  @param parent tool parent
     */
    AssociatorBase
    ( const std::string& type   , 
      const std::string& name   ,
      const IInterface*  parent ) ;
    
    /// destructor (virtual and protected)
    virtual ~AssociatorBase () ;
  
  protected:
    
    /** The "base" method for access the relation data 
     *
     *  @attention it is virtual method ! One coudl 
     *  redefine it for "specific" behaviour 
     *
     *  'Default' behaviour:
     *  
     *   - locate relation table in Gaudi Event Transient Store 
     *   - call for Relation builder algorithm to build and 
     *     register  the relation table in Gaudi Event Transient Store
     *   - again locate relation table in Gaudi Event Transient Store 
     *
     *  @return status code 
     */
    virtual StatusCode locateOrBuild   () const;
    
    /** accessor to event data service 
     *  @return pointer to event data service 
     */
    inline IDataProviderSvc*  evtSvc    () const ;
    
    /** accessor to tool service 
     *  @return pointer to tool service 
     */
    inline IToolSvc*          toolSvc   () const ;
    
    /** accessor to Chrono & Stat  service 
     *  @return pointer to Chrono & Stat  service 
     */
    inline IChronoStatSvc*    chronoSvc () const ;
    
    /** accessor to Incident service 
     *  @see IIncidentSvc 
     *  @see IIncidentListener 
     *  @see  Incident 
     *  @return pointer to Incident  service 
     */
    inline IIncidentSvc*      incSvc    () const ;
    
    /** accessor to relations builder,
     *  locate algorthm, if not yet done 
     *  @see IAlgorithm
     *  @return poinere to relations builder
     */
    inline IAlgorithm*        algorithm () const ;
    
    /** accessor to relations table via interface, 
     *  locate or build the table, if not yet done 
     *  @see IInterface 
     *  @return poinere to relations table 
     */
    inline IInterface*        object    () const ;
    
    /** accessor to data location
     *  @return data locationin Gaudi ETS 
     */
    inline const std::string& location  () const ;
    
    /** Print the error  message, return status code
     *  and perform the statistics of error messages 
     *  @see MsgStream
     *  @see Stat 
     *  @see IChronoStatSvc 
     *  @see StatusCode 
     *  @param msg    error message 
     *  @param st     status code 
     *  @return       status code 
     */
    StatusCode Error     
    ( const std::string& msg                       , 
      const StatusCode & st  = StatusCode::FAILURE ) const ;
    
    /** Print the warning  message, return status code 
     *  and perform the statistics of warning  messages 
     *  @see MsgStream
     *  @see Stat 
     *  @see IChronoStatSvc 
     *  @see StatusCode 
     *  @param msg    warning message 
     *  @param st     statsu code 
     *  @return       status code 
     */
    StatusCode Warning   
    ( const std::string& msg                       , 
      const StatusCode & st  = StatusCode::FAILURE ) const ; 
    
    /** Print the message and return status code 
     *  @param msg    warning message 
     *  @param st     status code 
     *  @param lev    print level 
     *  @return       status code 
     */
    StatusCode Print     
    ( const std::string& msg                       , 
      const StatusCode & st  = StatusCode::FAILURE ,
      const MSG::Level & lev = MSG::INFO           ) const ;
    
    /** Assertion - throw exception, if condition is not fulfilled 
     *  @exception GaudiException if 'ok' is false 
     *  @param ok            condition which should be "true"
     *  @param mesage       message to eb associated with the exception 
     *  @return             status code        
     */
    inline StatusCode Assert 
    ( bool ok , const std::string& message = " ") const ;
    
    /** Assertion - throw exception, if condition is not fulfilled 
     *  @exception GaudiException if 'ok' is false 
     *  @param ok            condition which shoudl be "true"
     *  @param mesage       message to eb associated with the exception
     *  @return             status code        
     */
    inline StatusCode Assert 
    ( bool ok , const char*        message  ) const ;
    
    /** Create and (re)-throw the exception  
     *  @exception GaudiException always 
     *  @param msg    exception message 
     *  @param exc    (previous) exception of type GaudiException
     *  @param lvl    print level 
     *  @param sc     status code  
     *  @return       status code (fictive) 
     */
    StatusCode Exception 
    ( const std::string    & msg                        ,  
      const GaudiException & exc                        , 
      const MSG::Level     & lvl = MSG::FATAL           ,
      const StatusCode     & sc  = StatusCode::FAILURE ) const ;
    
    /** Create and (re)-throw the exception  
     *  @exception GaudiException always 
     *  @param msg    exception message 
     *  @param exc    (previous) exception of type std::exception
     *  @param lvl    print level 
     *  @param sc     status code  
     *  @return       status code (fictive) 
     */
    StatusCode Exception 
    ( const std::string    & msg                        ,  
      const std::exception & exc                        , 
      const MSG::Level     & lvl = MSG::FATAL           ,
      const StatusCode     & sc  = StatusCode::FAILURE ) const ;
    
    /** Create and throw the exception  
     *  @exception GaudiException always 
     *  @param msg    exception message 
     *  @param lvl    print level 
     *  @param sc     status code  
     *  @return       status code (fictive) 
     */
    StatusCode Exception 
    ( const std::string& msg = "no message"          ,  
      const MSG::Level & lvl = MSG::FATAL            ,
      const StatusCode & sc  = StatusCode::FAILURE   ) const ;
    
  private:
    
    /** locate the algorithm for building the relations
     *  @return status code 
     */
    StatusCode locateAlgorithm () const ;
    
  private:
    
    /// default constructor is private 
    AssociatorBase();
    /// copy    constructor is private 
    AssociatorBase           
    ( const AssociatorBase& );
    /// assignement operator is private  
    AssociatorBase& operator=
    ( const AssociatorBase& );
    
  protected: 
    
  private:
    
    // event data service 
    IDataProviderSvc*   m_evtSvc       ; ///< event data service 
    // Tool service 
    IToolSvc*           m_toolSvc      ; ///< tool service 
    // Chrono & Stat service 
    IChronoStatSvc*     m_chronoSvc    ; ///< Chrono & Stat service
    // Incident service 
    IIncidentSvc*       m_incSvc       ; ///< Incident  service
    // data location in Gaudi Event Transient Store 
    std::string         m_location     ; ///< data location in Gaudi ETS 
    // relation builder type 
    std::string         m_builderType  ; ///< relation builder type 
    // relation builder name 
    mutable std::string m_builderName  ; ///< relation builder name 
    // relation builder itself 
    mutable IAlgorithm* m_algorithm    ; ///< relation builder itself 
    // relation table!    
    mutable IInterface* m_object       ; ///< relation table
    
  };
  
  /** accessor to relations builder,
   *  locate algorthm, if not yet done 
   *  @see IAlgorithm
   *  @see AssociatorBase 
   *  @return poinere to relations builder
   */
  inline IAlgorithm*        AssociatorBase::algorithm () const 
  { 
    if( 0 == m_algorithm ) { locateAlgorithm() ; }
    return m_algorithm ;
  };
  
  /** accessor to relations table via interface, 
   *  locate or build the table, if not yet done 
   *  @see IInterface 
   *  @see AssociatorBase 
   *  @return poinere to relations table 
   */
  inline IInterface*        AssociatorBase::object    () const 
  {
    if( 0 == m_object    ) { locateOrBuild () ; } 
    return m_object ; 
  };
  
  /** accessor to data location
   *  @see AssociatorBase 
   *  @return data locationin Gaudi ETS 
   */
  inline const std::string& AssociatorBase::location  () const 
  { return m_location ; }
  
  /** accessor to event data service 
   *  @see IDataProviderSvc 
   *  @see AssociatorBase 
   *  @return pointer to event data service 
   */
  inline IDataProviderSvc*  AssociatorBase::evtSvc    () const 
  { return m_evtSvc    ; }
  
  /** accessor to tool service 
   *  @see IToolSvc 
   *  @see IAlgTool 
   *  @see AssociatorBase 
   *  @return pointer to tool service 
   */
  inline IToolSvc*          AssociatorBase::toolSvc   () const 
  { return m_toolSvc   ; }  
  
  /** accessor to Chrono & Stat  service 
   *  @see AssociatorBase 
   *  @see IChronoStatSvc
   *  @see Chrono
   *  @see Stat
   *  @return pointer to Chrono & Stat  service 
   */
  inline IChronoStatSvc*    AssociatorBase::chronoSvc () const 
  { return m_chronoSvc ; }
  
  /** accessor to Incident service 
   *  @see IIncidentSvc 
   *  @see IIncidentListener 
   *  @see  Incident 
   *  @see AssociatorBase 
   *  @return pointer to Incident  service 
   */
  inline IIncidentSvc*      AssociatorBase::incSvc    () const 
  { return m_incSvc    ; }
  
  /** Assertion - throw exception, if condition is not fulfilled 
   *  @see AssociatorBase 
   *  @param ok            condition which should be "true"
   *  @param mesage       message to eb associated with the exception 
   *  @return             status code        
   */
  inline StatusCode AssociatorBase::Assert 
  ( bool ok , const std::string& message  ) const
  {
    StatusCode OK ( StatusCode::SUCCESS );
    return ok ? OK : Exception( message , MSG::ERROR ) ; 
  };
  
  /** Assertion - throw exception, if condition is not fulfilled 
   *  @see AssociatorBase 
   *  @param ok            condition which shoudl be "true"
   *  @param mesage       message to eb associated with the exception
   *  @return             status code        
   */
  inline StatusCode AssociatorBase::Assert 
  ( bool ok , const char*        message  ) const 
  { 
    StatusCode OK ( StatusCode::SUCCESS ) ;
    return ok ? OK : Exception( message , MSG::ERROR ) ; 
  };
  
}; ///< end of namespace Relations 

// ============================================================================
// The End 
// ============================================================================
#endif // RELATIONS_AssociatorBase_H
// ============================================================================
