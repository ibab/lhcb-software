/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.3  2001/07/15 20:54:35  ibelyaev
/// package restructurisation
///
/// ===========================================================================
#ifndef       GIGA_GIGASVC_H
#define       GIGA_GIGASVC_H   1 
/// ===========================================================================

/// STD & STL 
#include <string>
#include <list>
#include <vector> 
/// GaudiKernel 
#include  "GaudiKernel/Kernel.h"
#include  "GaudiKernel/StatusCode.h"
#include  "GaudiKernel/Service.h" 
#include  "GaudiKernel/MsgStream.h" 
#include  "GaudiKernel/Stat.h" 
/// GiGa 
#include  "GiGa/IGiGaSvc.h"
#include  "GiGa/IGiGaSetUpSvc.h"
#include  "GiGa/GiGaException.h" 
/// forwad declarations  from Gaudi 
class     IChronoStatSvc                  ;
class     ISvcLocator                     ;
class     IObjManager                     ;
template   <class TYPE> class SvcFactory  ;
/// forwad declarations  from GiGa  
class     GiGaRunManager     ; 
/// forwad declarations  from G4
class     G4UImanager        ; 
class     G4VisManager       ;
class     IGiGaPhysList      ;
class     IGiGaStackAction   ;
class     IGiGaTrackAction   ;
class     IGiGaStepAction    ;
class     IGiGaEventAction   ;

/**  @class GiGaSvc GiGaSvc.h 
     
     implementation of abstract Interfaces IGiGaSvc          
     (for event-by-event communications with Geant4)         
     and IGiGaSetUpSvc (for configuration of Geant4)         
     
     @author: Vanya Belyaev                                                   
*/

class GiGaSvc: public         Service       , 
               virtual public IGiGaSvc      ,
               virtual public IGiGaSetUpSvc 
{
  /// friend factory 
  friend class SvcFactory<GiGaSvc>;
  ///
 public:
  /// useful typedef 
  typedef std::vector<std::string>        Strings;
  ///
 protected: 
  
  /** standard constructor 
      @param name instrance name 
      @param svc  pointer to service locator 
  */
  GiGaSvc( const std::string& name , 
           ISvcLocator*       svc  );

  /// (virtual destructor)
  virtual ~GiGaSvc();
  ///

 public:
  
  /** add  primary vertex into GiGa/G4 
   *                  implementation of IGiGaSvc abstract interface 
   *
   *  NB: errors are reported through exception thrown
   * 
   *  @param  vertex pointer to primary vertex 
   *  @return  self-reference ot IGiGaSvc interface 
   */
  virtual IGiGaSvc&   operator <<         ( G4PrimaryVertex  * vertex   )     ;
  
  /** get the whole event  object from GiGa/G4 
   *                  implementation of IGiGaSvc abstract interface 
   *
   *  NB: errors are reported through exception thrown
   * 
   *  @param   event pointer to whole event  
   *  @return  self-reference ot IGiGaSvc interface 
   */
  virtual IGiGaSvc& operator >> ( const G4Event*         & event        );
  
  /** get the all hit collections from GiGa/G4 
   *                  implementation of IGiGaSvc abstract interface 
   *
   *  NB: errors are reported through exception thrown
   * 
   *  @param   collections  pointer to all hit collections  
   *  @return  self-reference ot IGiGaSvc interface 
   */
  virtual IGiGaSvc& operator >> ( G4HCofThisEvent*       & collections  );
  
  /** get the concrete hit collection from GiGa/G4 
   *                  implementation of IGiGaSvc abstract interface 
   *
   *  NB: errors are reported through exception thrown
   * 
   *  @param   collection  reference to collection pair   
   *  @return  self-reference ot IGiGaSvc interface 
   */
  virtual IGiGaSvc& operator >> ( CollectionPair         & collection   ); 
  
  /** get all trajectories(trajectory container) from GiGa/G4 
   *                  implementation of IGiGaSvc abstract interface 
   *
   *  NB: errors are reported through exception thrown
   * 
   *  @param   trajectories  pointer to trajectory conatiner   
   *  @return  self-reference ot IGiGaSvc interface 
   */
  virtual IGiGaSvc& operator >> ( G4TrajectoryContainer* & trajectories );
  
  /** add  primary vertex into GiGa/G4  
   *                  implementation of IGiGaSvc abstract interface 
   *
   *  @param  vertex pointer to primary vertex 
   *  @return status code  
   */
  virtual StatusCode  addPrimaryKinematics( G4PrimaryVertex  * vertex   );
  
  /** get the whole event  object from GiGa/G4 
   *                  implementation of IGiGaSvc abstract interface 
   *
   *  @param  event pointer to whole event 
   *  @return status code  
   */
  virtual StatusCode retrieveEvent( const G4Event*& event )     ;

  /** get the all hit collections from GiGa/G4 
   *                  implementation of IGiGaSvc abstract interface 
   * 
   *  @param   collections  pointer to all hit collections  
   *  @return  status code  
   */
  virtual StatusCode 
  retrieveHitCollections ( G4HCofThisEvent*        & collections  );

  /** get the concrete hit collection from GiGa/G4 
   *                  implementation of IGiGaSvc abstract interface 
   *
   *  @param   collection  reference to collection pair   
   *  @return  status code 
   */
  virtual StatusCode 
  retrieveHitCollection  ( CollectionPair          & collection   );
  
  /** get all trajectories(trajectory container) from GiGa/G4 
   *                  implementation of IGiGaSvc abstract interface 
   *
   *  NB: errors are reported throw exception
   * 
   *  @param   trajectories  pointer to trajectory conatiner   
   *  @return  self-reference ot IGiGaSvc interface 
   */
  virtual StatusCode 
  retrieveTrajectories   ( G4TrajectoryContainer*  & trajectories );


  /** set detector constructon module 
   *               implementation of IGiGaSetUpSvc abstract interface 
   *
   *  NB: errors are reported through exception thrown 
   *
   *  @param  detector pointer to detector construction module  
   *  @return self-reference ot IGiGaSetUpSvc interface 
   */
  virtual IGiGaSetUpSvc& 
  operator << ( G4VUserDetectorConstruction   * detector      ) ;
  
  /** set new world wolume 
   *               implementation of IGiGaSetUpSvc abstract interface 
   *
   *  NB: errors are reported through exception thrown 
   * 
   *  @param  world  pointer to  new world volume   
   *  @return self-reference ot IGiGaSetUpSvc interface 
   */
  virtual IGiGaSetUpSvc& 
  operator << ( G4VPhysicalVolume             * world         ) ;

  /** set new generator 
   *               implementation of IGiGaSetUpSvc abstract interface 
   *
   *  NB: errors are reported throw exception
   * 
   *  @param  generator  pointer to new generator   
   *  @return self-reference ot IGiGaSetUpSvc interface 
   */
  virtual IGiGaSetUpSvc& 
  operator << ( G4VUserPrimaryGeneratorAction * generator     ) ;

  /** set new physics list 
   *               implementation of IGiGaSetUpSvc abstract interface 
   *
   *  NB: errors are reported through  exception thrown 
   * 
   *  @param  physics  pointer to physics list    
   *  @return self-reference ot IGiGaSetUpSvc interface 
   */
  virtual IGiGaSetUpSvc& 
  operator << ( G4VUserPhysicsList            * physics       ) ;

  /** set new run action 
   *               implementation of IGiGaSetUpSvc abstract interface 
   *
   *  NB: errors are reported through exception thrown 
   * 
   *  @param  action  pointer to new run action     
   *  @return self-reference ot IGiGaSetUpSvc interface 
   */
  virtual IGiGaSetUpSvc& 
  operator << ( G4UserRunAction               * action        ) ;

  /** set new event action 
   *               implementation of IGiGaSetUpSvc abstract interface 
   *
   *  NB: errors are reported through exception thrown 
   * 
   *  @param  action  pointer to new event action     
   *  @return self-reference ot IGiGaSetUpSvc interface 
   */
  virtual IGiGaSetUpSvc& 
  operator << ( G4UserEventAction             * action        ) ;

  /** set new stacking action 
   *               implementation of IGiGaSetUpSvc abstract interface 
   *
   *  NB: errors are reported through  exception thrown 
   * 
   *  @param  action  pointer to new stacking action     
   *  @return self-reference ot IGiGaSetUpSvc interface 
   */
  virtual IGiGaSetUpSvc&
  operator << ( G4UserStackingAction          * action        ) ;

  /** set new tracking  action 
   *               implementation of IGiGaSetUpSvc abstract interface 
   *
   *  NB: errors are reported through exception thrown 
   * 
   *  @param  action  pointer to new tracking action     
   *  @return self-reference ot IGiGaSetUpSvc interface 
   */
  virtual IGiGaSetUpSvc& 
  operator << ( G4UserTrackingAction          * action        ) ;
  
  /** set new stepping  action 
   *               implementation of IGiGaSetUpSvc abstract interface 
   *
   *  NB: errors are reported through exception thrown 
   * 
   *  @param  action  pointer to new stepping action     
   *  @return self-reference ot IGiGaSetUpSvc interface 
   */
  virtual IGiGaSetUpSvc& 
  operator << ( G4UserSteppingAction          * action        ) ; 
  
  /** set new visualization manager  
   *               implementation of IGiGaSetUpSvc abstract interface 
   *
   *  NB: errors are reported through exception thrown 
   * 
   *  @param  visualization  pointer to new visualization manager      
   *  @return self-reference ot IGiGaSetUpSvc interface 
   */
  virtual IGiGaSetUpSvc& 
  operator << ( G4VisManager                  * visualization ) ;

  /** set detector constructon module 
   *               implementation of IGiGaSetUpSvc abstract interface 
   *
   *  @param  detector pointer to detector construction module  
   *  @return self-reference ot IGiGaSetUpSvc interface 
   */
  virtual StatusCode 
  setConstruction ( G4VUserDetectorConstruction   * detector      ) ;

  /** set new world wolume 
   *               implementation of IGiGaSetUpSvc abstract interface 
   *
   *  @param  world  pointer to  new world volume   
   *  @return status code  
   */
  virtual StatusCode 
  setDetector     ( G4VPhysicalVolume             * volume        ) ;

  /** set new generator 
   *               implementation of IGiGaSetUpSvc abstract interface 
   *
   *  @param  generator  pointer to new generator   
   *  @return status code  
   */
  virtual StatusCode 
  setGenerator    ( G4VUserPrimaryGeneratorAction * generator     ) ;

  /** set new physics list 
   *               implementation of IGiGaSetUpSvc abstract interface 
   *
   *  @param  physics  pointer to physics list    
   *  @return status code  
   */
  virtual StatusCode
  setPhysics      ( G4VUserPhysicsList            * physics       ) ;

  /** set new run action 
   *               implementation of IGiGaSetUpSvc abstract interface 
   *
   *  @param  action  pointer to new run action     
   *  @return status code  
   */
  virtual StatusCode 
  setRunAction    ( G4UserRunAction               * action        ) ;

  /** set new event action 
   *               implementation of IGiGaSetUpSvc abstract interface 
   *
   *  @param  action  pointer to new event action     
   *  @return status code  
   */
  virtual StatusCode 
  setEvtAction    ( G4UserEventAction             * action        ) ;

  /** set new stacking action 
   *               implementation of IGiGaSetUpSvc abstract interface 
   *
   *  @param  action  pointer to new stacking action     
   *  @return status code  
   */
  virtual StatusCode 
  setStacking     ( G4UserStackingAction          * action        ) ;

  /** set new tracking  action 
   *               implementation of IGiGaSetUpSvc abstract interface 
   *
   *  @param  action  pointer to new tracking action     
   *  @return status code  
   */
  virtual StatusCode
  setTracking     ( G4UserTrackingAction          * action        ) ;

  /** set new stepping  action 
   *               implementation of IGiGaSetUpSvc abstract interface 
   *
   *  @param  action  pointer to new stepping action     
   *  @return status code  
   */
  virtual StatusCode 
  setStepping     ( G4UserSteppingAction          * action        ) ;

  /** set new visualization manager  
   *               implementation of IGiGaSetUpSvc abstract interface 
   *
   *  @param  visualization  pointer to new visualization manager      
   *  @return status code  
   */
  virtual StatusCode 
  setVisManager   ( G4VisManager                  * visualization ) ;
  
  /** service initialization 
   *  @return status code 
   */
  virtual StatusCode                    initialize() ; 
  
  /** service finalization  
   *  @return status code 
   */
  virtual StatusCode                    finalize  () ;
  
  /** query interface 
   *  @param   iid   InterfaceID 
   *  @param   pI    placeholder for returned interface
   *  @return status code
   */   
  virtual StatusCode queryInterface ( const InterfaceID& iid , 
                                      void**             pI  );
protected: 
  
  /** create GiGa Run Manager 
   *  @return status code 
   */
  StatusCode      createGiGaRunManager    () ;  
  
  /** prepare the event 
   *  @param  vertex pointer to primary vertex 
   *  @return status code 
   */
  StatusCode      prepareTheEvent  ( G4PrimaryVertex *  vertex ); 
  
  /** retrieve the event 
   *  @param  event pointer to event 
   *  @return status code 
   */
  StatusCode      retrieveTheEvent ( const G4Event   *& event  ); 
  
private: 
  
  /// accessor to Chrono & Stat  service 
  inline  IChronoStatSvc*        chronoSvc () const { return m_chronoSvc ; } ;
  /// accessor to object manager 
  inline  IObjManager*           objMgr    () const { return m_objMgr    ; } ;
  /// accessor to Service Locator 
  inline  ISvcLocator*           svcLoc    () const 
  { return serviceLocator() ; }
  
  /** assertion 
   *  @param assertion   assertion condition
   *  @param msg         assertion message 
   *  @param sc          assertion status code 
   */
  inline void Assert( bool  assertion                              , 
                      const std::string& msg = "GiGaSvc::unknown"  , 
                      const StatusCode&  sc = StatusCode::FAILURE  ) ;
  
  /** assertion 
   *  @param assertion   assertion condition
   *  @param msg         assertion message 
   *  @param sc          assertion status code 
   */
  inline void Assert( bool  assertion                              ,
                      const char*        msg                       ,
                      const StatusCode&  sc  = StatusCode::FAILURE ) ;
  
  /** error printout 
   *  @param msg         error message 
   *  @param sc          error status code
   *  @return status code 
   */
  inline StatusCode Error  ( const std::string & msg ,  
                             const StatusCode  & sc  = StatusCode::FAILURE );

  /** warning printout 
   *  @param msg         warning message 
   *  @param sc          warning status code
   *  @return status code 
   */
  inline StatusCode Warning( const std::string & msg ,  
                             const StatusCode  & sc  = StatusCode::FAILURE );

  /** printout 
   *  @param msg            message 
   *  @param lvl            printout level 
   *  @param sc             status code
   *  @return status code 
   */
  inline StatusCode Print  ( const std::string & msg ,  
                             const MSG::Level  & lvl = MSG::INFO           ,
                             const StatusCode  & sc  = StatusCode::FAILURE );
  
  /** exception
   *  @param msg            exception message 
   *  @param exc            reference to "previous" exception
   *  @param lvl            exception printout level 
   *  @param sc             exception status code
   *  @return status code 
   */
  inline StatusCode 
  Exception ( const std::string    & msg  ,  
              const GaudiException & exc  , 
              const MSG::Level     & lvl = MSG::FATAL           ,
              const StatusCode     & sc  = StatusCode::FAILURE );
  
  /** exception
   *  @param msg            exception message 
   *  @param exc            reference to "previous" exception
   *  @param lvl            exception printout level 
   *  @param sc             exception status code
   *  @return status code 
   */
  inline StatusCode 
  Exception ( const std::string    & msg  ,  
              const std::exception & exc  , 
              const MSG::Level     & lvl = MSG::FATAL           ,
              const StatusCode     & sc  = StatusCode::FAILURE );

  /** exception
   *  @param msg            exception message 
   *  @param lvl            exception printout level 
   *  @param sc             exception status code
   *  @return status code 
   */
  inline StatusCode 
  Exception ( const std::string    & msg  ,  
              const MSG::Level     & lvl = MSG::FATAL           ,
              const StatusCode     & sc  = StatusCode::FAILURE );
  
  
  /** instantiate new physics list object using abstract factory technique 
   *  @param TypeAndName    "Type/Name" of physics list object 
   *  @param PhysicsList    reference to new phisics list object 
   *  @return status code 
   */
  StatusCode physList      ( const std::string& TypeAndName , 
                             IGiGaPhysList*&    PhisicsList ) ;
  
  /** instantiate new stacking action object using abstract factory technique 
   *  @param TypeAndName    "Type/Name" of stacking action object 
   *  @param StackAction    reference to new stacking action object 
   *  @return status code 
   */
  StatusCode stackAction   ( const std::string& TypeAndName , 
                             IGiGaStackAction*& StackAction ) ;
  
  /** instantiate new tracking action object using abstract factory technique 
   *  @param TypeAndName    "Type/Name" of tracking action object 
   *  @param TrackAction    reference to new tracking action object 
   *  @return status code 
   */
  StatusCode trackAction   ( const std::string& TypeAndName , 
                             IGiGaTrackAction*& TrackAction ) ;

  /** instantiate new stepping action object using abstract factory technique 
   *  @param TypeAndName    "Type/Name" of stepping action object 
   *  @param StepAction    reference to new stepping action object 
   *  @return status code 
   */
  StatusCode stepAction    ( const std::string& TypeAndName , 
                             IGiGaStepAction*&  StepAction  ) ;
  
  /** instantiate new event action object using abstract factory technique 
   *  @param TypeAndName    "Type/Name" of event  action object 
   *  @param eventAction     reference to new event action object 
   *  @return status code 
   */
  StatusCode eventAction   ( const std::string& TypeAndName , 
			     IGiGaEventAction*& EventAction  ) ;

 private:
  
  ///
  GiGaRunManager* m_GiGaRunManager ; ///< pointer to GiGaRunManager  object
  ///
  IChronoStatSvc* m_chronoSvc   ; ///< pointer to Chtono&Stat service
  IObjManager*    m_objMgr      ; ///< pointer to ObjectManager object 
  std::string     m_objMgrName  ; ///< name of object manager object  
  ///
  Strings m_startUIcommands           ; ///< command list 
  Strings m_startOfEvtUIcommands      ; ///< command list 
  Strings m_endOfEvtUIcommands        ; ///< command list 
  Strings m_startOfRunUIcommands      ; ///< command list 
  Strings m_endOfRunUIcommands        ; ///< command list 
  Strings m_endUIcommands             ; ///< command list
  ///
  Strings m_UIsessions                ; ///< list of sessions 
  ///
  std::string m_GiGaPhysList    ; ///< type/name of PhysicsList Object
  std::string m_GiGaStackAction ; ///< type/name of  Stacking Action Object 
  std::string m_GiGaTrackAction ; ///< type/name of Tracking Action Object 
  std::string m_GiGaStepAction  ; ///< type/name of Stepping Action Object 
  std::string m_GiGaEventAction ; ///< type/name of Event    Action Object 
  ///
  bool        m_UseVisManager ;  ///< flag to use vis manager 
  ///
};

/// ===========================================================================
/// ===========================================================================
inline void GiGaSvc::Assert( bool               assertion , 
                             const std::string& msg       , 
                             const StatusCode&  sc        ) 
{ if( !assertion ) { throw GiGaException( name() + msg , sc ) ; } };

/// ===========================================================================
/** assertion 
 *  @param assertion   assertion condition
 *  @param msg         assertion message 
 *  @param sc          assertion status code 
 */
/// ===========================================================================
inline void GiGaSvc::Assert( bool               assertion , 
                             const char*        msg       , 
                             const StatusCode&  sc        ) 
{ if( !assertion ) { throw GiGaException( name() + msg , sc ) ; } };

/// ===========================================================================
/** error printout 
 *  @param msg         error message 
 *  @param sc          error status code
 *  @return status code 
 */
/// ===========================================================================
inline StatusCode GiGaSvc::Error( const std::string& Message , 
                                  const StatusCode & Status )
{ 
  Stat stat( chronoSvc() , name() + ":Error" ); 
  return  Print( Message , MSG::ERROR  , Status  ) ; 
};  

/// ===========================================================================
/** warning printout 
 *  @param msg         warning message 
 *  @param sc          warning status code
 *  @return status code 
 */
/// ===========================================================================
inline StatusCode GiGaSvc::Warning( const std::string& Message , 
                                    const StatusCode & Status )
{ 
  Stat stat( chronoSvc() , name() + ":Warning" ); 
  return  Print( Message , MSG::WARNING , Status ) ; 
};  

/// ===========================================================================
/** printout 
 *  @param msg            message 
 *  @param lvl            printout level 
 *  @param sc             status code
 *  @return status code 
 */
/// ===========================================================================
inline StatusCode GiGaSvc::Print( const std::string& Message , 
                                  const MSG::Level & level   , 
                                  const StatusCode & Status )
{ 
  MsgStream log( msgSvc() , name() ); 
  log << level << Message << endreq ; 
  return  Status; 
};  

/// ===========================================================================
/** exception
 *  @param Message        exception message 
 *  @param Excp           reference to "previous" exception
 *  @param level          exception printout level 
 *  @param Status         exception status code
 *  @return status code 
 */
/// ===========================================================================
inline StatusCode GiGaSvc::Exception( const std::string    & Message , 
                                      const GaudiException & Excp    ,
                                      const MSG::Level     & level   , 
                                      const StatusCode     & Status )
{
  Stat stat( chronoSvc() , Excp.tag() );
  MsgStream log( msgSvc() , name() + ":"+Excp.tag() ); 
  log << level << Message << ":" << Excp << endreq ; 
  return  Status;
};  

/// ===========================================================================
/** exception
 *  @param Message        exception message 
 *  @param Excp           reference to "previous" exception
 *  @param level          exception printout level 
 *  @param Status         exception status code
 *  @return status code 
 */
/// ===========================================================================
inline StatusCode GiGaSvc::Exception( const std::string    & Message , 
                                      const std::exception & Excp    ,
                                      const MSG::Level     & level   , 
                                      const StatusCode     & Status )
{
  Stat stat( chronoSvc() , ":std::exception" );
  MsgStream log( msgSvc() , name() ); 
  log << level << Message << ":" << Excp.what() << endreq ; 
  return  Status;
};  

/// ===========================================================================
/** exception
 *  @param Message        exception message 
 *  @param level          exception printout level 
 *  @param Status         exception status code
 *  @return status code 
 */
/// ===========================================================================
inline StatusCode GiGaSvc::Exception( const std::string    & Message , 
                                      const MSG::Level     & level   , 
                                      const StatusCode     & Status )
{
  Stat stat( chronoSvc() , ":*UNKNOWN* exception" );
  MsgStream log( msgSvc() , name() ); 
  log << level << Message << ": UNKNOWN exception"  << endreq ; 
  return  Status;
};  

 
/// ===========================================================================


/// ===========================================================================
#endif  ///<  GIGA_GIGASVC_H
/// ===========================================================================











