/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.11  2001/07/27 15:06:14  ibelyaev
/// printout improvements in base class GiGaBase
///
/// Revision 1.10  2001/07/23 13:11:41  ibelyaev
/// the package restructurisation(II)
/// 
/// ===========================================================================
#ifndef     GIGA_GIGABASE_H
#define     GIGA_GIGABASE_H 1 
/// ===========================================================================
/// STL
#include <string>
#include <exception>
/// GaudiKernel 
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ISerialize.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/MsgStream.h"
///
class IMessageSvc          ;
class ISvcLocator          ;
class IChronoStatSvc       ;
class IDataProviderSvc     ;
class IIncidentSvc         ;
class IObjManager          ;
class IGiGaSvc             ; 
class IGiGaSetUpSvc        ; 
///
class PropertyMgr          ;
class MsgStream            ;
class StreamBuffer         ;
class GaudiException       ;
///

/** @class GiGaBase GiGaBase.h GiGa/GiGaBase.h
 *    
 *  Helper class for implementation of some GiGa classes. 
 *  It allows an easy configuration of properties and services location  
 *  Implement almost all virtual "technical functions".
 *
 *  @author  Vanya Belyaev
 *  @date    23/01/2001
 */

class  GiGaBase: virtual public IProperty         , 
                 virtual public ISerialize        , 
                 virtual public IIncidentListener 
{
  ///
protected:

  /** standard constructor 
   *  @param name object name 
   *  @param loc  pointer to servcie locator 
   */
  GiGaBase( const std::string& , ISvcLocator* );
  /// virtual destructor 
  virtual ~GiGaBase();
  ///
public:

  /** object name 
   *  @return object name 
   */
  inline virtual const std::string& name   () const ;
  
  /// Increment the reference count of Interface instance
  virtual unsigned long          addRef  () ;
  /// Release Interface instance
  virtual unsigned long          release () ;

  /** query the interface
   *  @param id unique interface identifier 
   *  @param I  placeholder for returning interface 
   *  @return status code 
   */ 
  virtual StatusCode queryInterface( const InterfaceID& id , 
                                     void**             I  ) ;
  /** initialize the object
   *  @return status code 
   */
  virtual StatusCode initialize() ;

  /** finalize the object 
   *  @return status code 
   */
  virtual StatusCode finalize  () ;

  /// serialize object for reading 
  virtual StreamBuffer& serialize( StreamBuffer& S )       ;

  /// serialize object for writing 
  virtual StreamBuffer& serialize( StreamBuffer& S ) const ; 

  /** set the property by property
   *  @param p property 
   *  @return status code 
   */
  virtual StatusCode      setProperty   ( const Property& p       )       ;

  /** set the property from input stream
   *  @param s reference to input stream 
   *  @return status code 
   */
  virtual StatusCode      setProperty   ( std::istream& s         )       ;
  
  /** set the property from the string 
   *  @param n property name 
   *  @param s string property 
   *  @return status code 
   */
  virtual StatusCode      setProperty   ( const std::string& n    , 
                                          const std::string& s    )       ;

  /** get the property by property
   *  @param p pointer to property 
   *  @return status code 
   */
  virtual StatusCode      getProperty   ( Property* p             ) const ;
  
  /** get the property by name
   *  @param name property name 
   *  @return status code 
   */
  virtual const Property& getProperty   ( const std::string& name ) const ; 

  /** get the property by std::string
   *  @param s property name 
   *  @param n property string 
   *  @return status code 
   */
  virtual StatusCode      getProperty   ( const std::string& n    , 
                                          std::string&       s    ) const ;

  /** get list of all properties 
   *  @return list of all proeprties 
   */
  virtual const std::vector<Property*>& getProperties ( )  const ;

  /** handle the incident
   *  @param i reference to the incident
   */ 
  virtual void handle( const Incident& i ) ;
  /// 
protected:
  ///
  
  /** object type 
   *  @return object type 
   */
  inline const std::string& myType   () const ;

  /** is the base is initialized properly?
   *  @return true if it is initialized 
   */
  inline bool              init      () const { return m_init      ; };

  /** accessor to service locator 
   *  @return pointer to service locator 
   */
  inline ISvcLocator*      svcLoc    () const { return m_svcLoc    ; };  

  /** accessor to GiGa Service 
   *  @return pointer to GiGa Service 
   */
  inline IGiGaSvc*         gigaSvc   () const { return m_gigaSvc   ; }; 

  /** accessor to GiGa SetUp Service 
   *  @return pointer to GiGa SetUp Service 
   */
  inline IGiGaSetUpSvc*    setupSvc  () const { return m_setupSvc  ; };

  /** accessor to Message Service 
   *  @return pointer to Message Service 
   */
  inline IMessageSvc*      msgSvc    () const { return m_msgSvc    ; };

  /** accessor to Chrono & Stat Service 
   *  @return pointer to Chrono & Stat Service 
   */
  inline IChronoStatSvc*   chronoSvc () const { return m_chronoSvc ; };

  /** accessor to Event Data  Service 
   *  @return pointer to Event Data  Service 
   */
  inline IDataProviderSvc* evtSvc    () const { return m_evtSvc    ; }; 

  /** accessor to Detector Data Service 
   *  @return pointer to Detector Data  Service 
   */
  inline IDataProviderSvc* detSvc    () const { return m_detSvc    ; }; 

  /** accessor to Incident  Service 
   *  @return pointer to Message Service 
   */
  inline IIncidentSvc*     incSvc    () const { return m_incSvc    ; }; 

  /** accessor to Property Manager 
   *  @return pointer to Property Manager  
   */
  inline PropertyMgr*      propMgr   () const { return m_propMgr   ; };  

  /** accessor to Object Manager
   *  @return pointer to Object Manager  
   */
  inline IObjManager*      objMgr    () const { return m_objMgr    ; };  
  ///
protected: 
  
  /** Methods for declaring properties to the property manager
   *  @param name property name 
   *  @param reference property reference 
   *  @return status code
   */
  template <class TYPE>
  StatusCode declareProperty( const std::string& name , TYPE& reference )
  {
    if( 0 != propMgr() ) { propMgr()->declareProperty( name , reference ); } 
    return 0 != propMgr() ? StatusCode::SUCCESS : StatusCode::FAILURE ; 
  };

  /** set own properties 
   *  @return status code
   */
  StatusCode setProperties  () ; 
  ///

  /** set the type of the object
   *  @return object type 
   */
  const std::string&  setMyType() const;
  
protected:
  
  /** Print the error    message and return status code 
   *  @param mgs message to be printed 
   *  @param sc  status code 
   *  @return status code 
   */
  StatusCode Error   ( const std::string& msg , 
                       const StatusCode & sc  = StatusCode::FAILURE ) const ;  
  
  /** Print the warning  message and return status code 
   *  @param mgs message to be printed 
   *  @param sc  status code 
   *  @return status code 
   */
  StatusCode Warning ( const std::string& msg , 
                       const StatusCode & sc  = StatusCode::FAILURE ) const ;
  
  /** Print the message and return status code 
   *  @param mgs message to be printed 
   *  @param sc  status code 
   *  @param lvl print level  
   *  @return status code 
   */
  inline StatusCode 
  Print   ( const std::string& msg , 
            const StatusCode & sc  = StatusCode::SUCCESS ,
            const MSG::Level & lvl = MSG::INFO           ) const ;
  
  /** re-throw the exception and print 
   *  @param msg exception message  
   *  @param exc previous exception 
   *  @param lvl print level 
   *  @param status code
   *  @return status code 
   */
  StatusCode Exception ( const std::string    & msg                        ,  
                         const GaudiException & exc                        , 
                         const MSG::Level     & lvl = MSG::FATAL           ,
                         const StatusCode     & sc  = StatusCode::FAILURE );
  
  /** re-throw the exception and print 
   *  @param msg exception message  
   *  @param exc previous exception 
   *  @param lvl print level 
   *  @param status code
   *  @return status code 
   */
  StatusCode Exception ( const std::string    & msg                        ,  
                         const std::exception & exc                        , 
                         const MSG::Level     & lvl = MSG::FATAL           ,
                         const StatusCode     & sc  = StatusCode::FAILURE );
  
  /** re-throw the exception and print 
   *  @param msg exception message  
   *  @param lvl print level 
   *  @param status code
   *  @return status code 
   */
  StatusCode Exception ( const std::string    & msg                        ,  
                         const MSG::Level     & lvl = MSG::FATAL           ,
                         const StatusCode     & sc  = StatusCode::FAILURE );

private: 
  ///
  GiGaBase();                              ///< no default 
  GiGaBase           ( const GiGaBase& );  ///< no copy 
  GiGaBase& operator=( const GiGaBase& );  ///< no assignment 
  ///
private:
  ///
  /// the reference count 
  unsigned long         m_count      ;
  /// name of the object 
  std::string           m_name       ; 
  /// type of the object 
  mutable std::string   m_myType     ;
  /// name of GiGa Service   
  std::string           m_gigaName   ; 
  /// name of GiGa SetUp Service   
  std::string           m_setupName  ; 
  /// name of Message Service 
  std::string           m_msgName    ; 
  /// name of Chrono & Stat service
  std::string           m_chronoName ;
  /// name of Event Data Provider Service  
  std::string           m_evtName    ; 
  /// name of Detector Data Provider Service  
  std::string           m_detName    ; 
  /// name of Incident Service 
  std::string           m_incName    ; 
  /// name of Object Manager 
  std::string           m_omName     ; 
  /// output level 
  int                   m_output     ; 
  //// pointer to Service Locator 
  ISvcLocator*          m_svcLoc     ; 
  /// pointer to Property Manager 
  PropertyMgr*          m_propMgr    ; 
  /// pointer to GiGa Service 
  IGiGaSvc*             m_gigaSvc    ;
  /// pointer to GiGa SetUp Service 
  IGiGaSetUpSvc*        m_setupSvc   ;
  /// pointer to Message  Service 
  IMessageSvc*          m_msgSvc     ; 
  /// pointer to Chrono& Stat  Service 
  IChronoStatSvc*       m_chronoSvc  ; 
  /// pointer to Event Data  Service 
  IDataProviderSvc*     m_evtSvc     ; 
  /// pointer to Detector Data  Service 
  IDataProviderSvc*     m_detSvc     ; 
  /// pointer to Incident  Service 
  IIncidentSvc*         m_incSvc     ; 
  /// pointer to Object Manager  
  IObjManager*          m_objMgr     ;
  /// "init" flag 
  bool                  m_init       ;
};
///

/// ===========================================================================
/** Print the message and return status code 
 *  @param mgs message to be printed 
 *  @param sc  status code 
 *  @param lvl print level  
 *  @return status code 
 */
/// ===========================================================================
inline StatusCode GiGaBase::Print( const std::string& Message , 
                                   const StatusCode & Status  , 
                                   const MSG::Level & level   ) const 
{
  ///
  MsgStream log( msgSvc() , name() ); 
  log << level 
      << myType() 
      << " "   
      << Message 
      << endreq  ; 
  ///
  return  Status;
  ///
};

/// ===========================================================================
/** object name 
 *  @return object name 
 */
/// ===========================================================================
inline const std::string& GiGaBase::name () const { return m_name ; };

/// ===========================================================================
/** object type 
 *  @return object type 
 */
/// ===========================================================================
inline const std::string& GiGaBase::myType() const 
{ return m_myType.empty() ? setMyType() : m_myType ; }

/// ===========================================================================
#endif ///<   GIGA_GIGABASE_H
/// ===========================================================================












