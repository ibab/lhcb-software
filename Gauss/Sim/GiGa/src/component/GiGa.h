// $Id: GiGa.h,v 1.9 2009-10-14 13:50:02 gcorti Exp $ 
#ifndef GIGA_GIGASVC_H
#define GIGA_GIGASVC_H   1 

// Include files 
// from STD & STL 
#include <string>
#include <list>
#include <vector> 
#include <map> 

// from Gaudi 
#include  "GaudiKernel/Kernel.h"
#include  "GaudiKernel/StatusCode.h"
#include  "GaudiKernel/Service.h" 
#include  "GaudiKernel/MsgStream.h" 
#include  "GaudiKernel/Stat.h" 
#include  "GaudiKernel/IToolSvc.h" 


// from GiGa 
#include  "GiGa/IGiGaSvc.h"
#include  "GiGa/IGiGaSetUpSvc.h"
#include  "GiGa/GiGaException.h" 

// Forwad declarations
// from Gaudi 
class     IChronoStatSvc                  ;
class     ISvcLocator                     ;
class     IRndmGenSvc                     ;
template   <class TYPE> class SvcFactory  ;

// from GiGa  
class     IGiGaRunManager                 ; 
class     IGiGaGeoSrc                     ; 
class     IGiGaPhysicsList                ;
class     IGiGaStackAction                ;
class     IGiGaTrackAction                ;
class     IGiGaStepAction                 ;
class     IGiGaEventAction                ;
class     IGiGaRunAction                  ;
class     IGiGaUIsession                  ;
class     IGiGaVisManager                 ;

// from G4
class     G4UImanager                     ; 
class     G4VVisManager                   ;
class     G4VExceptionHandler             ;

/**  @class GiGa GiGa.h 
 *    
 *   Implementation of abstract Interfaces IGiGaSvc          
 *   (for event-by-event communications with Geant4)         
 *    and IGiGaSetUpSvc (for configuration of Geant4)         
 *   
 *    @author: Vanya Belyaev Ivan.Belyaev@itep.ru
 */

class GiGa: public         Service, 
            virtual public IGiGaSvc,
            virtual public IGiGaSetUpSvc 
{
  /// friend factory 
  friend class SvcFactory<GiGa>;

 public:

  /// useful typedef 
  typedef std::vector<std::string>        Strings;

 protected: 
  
  /** standard constructor 
   *  @see Service
   *  @param name instrance name 
   *  @param svc  pointer to service locator 
   */
  GiGa( const std::string& name , 
        ISvcLocator*       svc  );
  
  /// (virtual destructor)
  virtual ~GiGa();


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
  virtual IGiGaSvc& operator >> ( GiGaHitsByID         & collection   ); 

  /** get the concrete hit collection from GiGa/G4 
   *                  implementation of IGiGaSvc abstract interface 
   *
   *  NB: errors are reported through exception thrown
   * 
   *  @param   collection  reference to collection pair   
   *  @return  self-reference ot IGiGaSvc interface 
   */
  virtual IGiGaSvc& operator >> ( GiGaHitsByName       & collection   ); 
  
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
  retrieveHitCollection  ( GiGaHitsByID          & collection   );
  
  /** get the concrete hit collection from GiGa/G4 
   *                  implementation of IGiGaSvc abstract interface 
   *
   *  @param   collection  reference to collection pair   
   *  @return  status code 
   */
  virtual StatusCode 
  retrieveHitCollection  ( GiGaHitsByName       & collection   );
  
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

  /** service initialization 
   *  @see  Service 
   *  @see IService 
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
  
  /** retrieve IGiGaRunManager 
   *  @see IGiGaRunManager
   *  @return status code
   */
  StatusCode      retrieveRunManager() ;
  
private:
  
  /// accessor to IGiGaRunManager object
  inline IGiGaRunManager* runMgr    () const { return m_runMgr         ; } ;
  /// accessor to GiGa Geometry Source  
  inline IGiGaGeoSrc*     geoSrc    () const { return m_geoSrc         ; } ;
  /// accessor to Chrono & Stat  service 
  inline IChronoStatSvc*  chronoSvc () const { return m_chronoSvc      ; } ;
  /// accessor to Tool Service 
  inline IToolSvc*        toolSvc   () const { return m_toolSvc        ; } ;
  /// accessor to Random Number Service 
  inline IRndmGenSvc*     rndmSvc   () const { return m_rndmSvc        ; } ;
  /// accessor to Service Locator 
  inline ISvcLocator*     svcLoc    () const { return serviceLocator() ; } ;
  
  /** assertion 
   *  @param assertion   assertion condition
   *  @param msg         assertion message 
   *  @param sc          assertion status code 
   *  @return status code 
   */
  inline StatusCode Assert
  ( bool  assertion                              , 
    const std::string& msg = "GiGaSvc::unknown"  , 
    const StatusCode&  sc = StatusCode::FAILURE  ) const ;
  
  /** assertion 
   *  @param assertion   assertion condition
   *  @param msg         assertion message 
   *  @param sc          assertion status code 
   *  @return status code 
   */
  inline StatusCode Assert
  ( bool  assertion                              ,
    const char*        msg                       ,
    const StatusCode&  sc  = StatusCode::FAILURE ) const ;
  
  /** error printout 
   *  @param msg         error message 
   *  @param sc          error status code
   *  @return status code 
   */
  StatusCode Error  
  ( const std::string & msg                       ,  
    const StatusCode  & sc  = StatusCode::FAILURE ) const ;
  
  /** warning printout 
   *  @param msg         warning message 
   *  @param sc          warning status code
   *  @return status code 
   */
  StatusCode Warning
  ( const std::string & msg                       ,  
    const StatusCode  & sc  = StatusCode::FAILURE ) const ;
  
  /** printout 
   *  @param msg            message 
   *  @param lvl            printout level 
   *  @param sc             status code
   *  @return status code 
   */
  StatusCode Print  
  ( const std::string & msg                       ,  
    const MSG::Level  & lvl = MSG::INFO           ,
    const StatusCode  & sc  = StatusCode::FAILURE ) const ;
  
  /** exception
   *  @param msg            exception message 
   *  @param exc            reference to "previous" exception
   *  @param lvl            exception printout level 
   *  @param sc             exception status code
   *  @return status code 
   */
  StatusCode Exception 
  ( const std::string    & msg                       ,  
    const GaudiException & exc                       , 
    const MSG::Level     & lvl = MSG::FATAL          ,
    const StatusCode     & sc  = StatusCode::FAILURE ) const ;
  
  /** exception
   *  @param msg            exception message 
   *  @param exc            reference to "previous" exception
   *  @param lvl            exception printout level 
   *  @param sc             exception status code
   *  @return status code 
   */
  StatusCode Exception 
  ( const std::string    & msg                       ,  
    const std::exception & exc                       , 
    const MSG::Level     & lvl = MSG::FATAL          ,
    const StatusCode     & sc  = StatusCode::FAILURE ) const ;
  
  /** exception
   *  @param msg            exception message 
   *  @param lvl            exception printout level 
   *  @param sc             exception status code
   *  @return status code 
   */
  StatusCode Exception 
  ( const std::string    & msg                       ,  
    const MSG::Level     & lvl = MSG::FATAL          ,
    const StatusCode     & sc  = StatusCode::FAILURE ) const ;

  /** the useful method for location of tools. 
   *  for empty "name" delegates to another method  
   *  @see IToolSvc
   *  @see IAlgTool
   *  @attention do not forget to 'release' tool after the usage!
   *  @exception GiGaException for invalid Tool Service 
   *  @exception GiGaException for error from Tool Service 
   *  @exception GiGaException for invalid tool 
   *  @param type   tool type 
   *  @param name   tool name
   *  @param Tool   tool itself (return) 
   *  @param parent tool parent
   *  @param create flag for creation of nonexisting tools 
   *  @return pointer to the tool
   */
  template<class TOOL>
  TOOL* 
  tool
  ( const std::string& type           , 
    const std::string& name           , 
    TOOL*&             Tool           , 
    const IInterface*  parent  = 0    , 
    bool               create  = true ) const 
  {
    // for empty names delegate to another method 
    if( name.empty() ) { return tool( type , Tool , parent , create ) ; }
    Assert( 0 != toolSvc() , "IToolSvc* points toNULL!" );
    // get the tool from Tool Service 
    StatusCode sc = toolSvc()->retrieveTool( type , name , Tool, parent, 
                                             create );
    Assert( sc.isSuccess() , 
            "Could not retrieve Tool'" + type + "'/'" + name + "'", sc ) ;
    Assert( 0 != Tool      , 
            "Could not retrieve Tool'" + type + "'/'" + name + "'"     ) ;
    // debug printout 
    Print( " The Tool of type '" + Tool->type() + 
           "'/'"                 + Tool->name() + 
           "' is retrieved from IToolSvc " , MSG::DEBUG  , sc ) ;
    // return located tool 
    return Tool ;
  };
  
  /** the useful method for location of tools.
   *  @see IToolSvc
   *  @see IAlgTool
   *  @attention do not forget to 'release' tool after the usage!
   *  @exception CaloException for invalid Tool Service 
   *  @exception CaloException for error from Tool Service 
   *  @exception CaloException for invalid tool 
   *  @param type   tool type, could be of "Type/Name" format 
   *  @param Tool   tool itself (return)
   *  @param parent tool parent
   *  @param create flag for creation of nonexisting tools 
   *  @return pointer to the tool
   */
  template<class TOOL>
  TOOL* 
  tool
  ( const std::string& type          , 
    TOOL*&             Tool          , 
    const IInterface*  parent = 0    , 
    bool               create = true ) const
  {
    // check the environment 
    Assert( 0 != toolSvc() , "IToolSvc* points toNULL!" );
    // "type" or "type/name" ?
    std::string::const_iterator it = 
      std::find( type.begin() , type.end () , '/' );
    // "type" is compound!
    if( type.end() != it ) 
      {
        std::string::size_type pos = it - type.begin()   ;
        const std::string ntype( type , 0       , pos               );
        const std::string nname( type , pos + 1 , std::string::npos );
        return tool( ntype , // new type 
                     nname , // new name 
                     Tool , parent , create            ) ;
      }
    // retrieve the tool from Tool Service 
    StatusCode sc = toolSvc () 
      -> retrieveTool ( type , Tool, parent , create   );
    Assert( sc.isSuccess() , "Could not retrieve Tool'" + type + "'" , sc ) ;
    Assert( 0 != Tool      , "Could not retrieve Tool'" + type + "'"      ) ;
    // debug printout 
    Print( " The Tool of type '" + Tool->type() + 
           "'/'"                 + Tool->name() + 
           "' is retrieved from IToolSvc " , MSG::DEBUG , sc ) ;
    // return located tool 
    return Tool ;
  };
  
  
private:
  
  IChronoStatSvc*   m_chronoSvc           ; ///< pointer to Chrono&Stat Service 
  IToolSvc*         m_toolSvc             ; ///< pointer to Tool Service  

  std::string       m_runMgrName          ; ///< name of Run manager  
  IGiGaRunManager*  m_runMgr              ; ///< pointer to IGiGaRunManager  

  std::string       m_geoSrcName          ; ///< name of geoemtry source    
  IGiGaGeoSrc*      m_geoSrc              ; ///< pointer to geometry source  

  std::string       m_GiGaPhysListName    ; ///< type/name of Physics List 
  IGiGaPhysicsList* m_GiGaPhysList        ; ///< pointer to Physics List  
  std::string       m_GiGaStackActionName ; ///< type/name of Stacking Action   
  IGiGaStackAction* m_GiGaStackAction     ; ///< pointer to   Stacking Action 
  std::string       m_GiGaTrackActionName ; ///< type/name of Tracking Action 
  IGiGaTrackAction* m_GiGaTrackAction     ; ///< pointer to   Tracking Action 
  std::string       m_GiGaStepActionName  ; ///< type/name of Stepping Action  
  IGiGaStepAction*  m_GiGaStepAction      ; ///< pointer to   Stepping Action  
  std::string       m_GiGaEventActionName ; ///< type/name of Event    Action  
  IGiGaEventAction* m_GiGaEventAction     ; ///< pointer to   Event    Action  
  std::string       m_GiGaRunActionName   ; ///< type/name of Run      Action  
  IGiGaRunAction*   m_GiGaRunAction       ; ///< pointer to   Run      Action 
  
  std::string       m_uiSessionName       ; ///< GiGa UI session type/name 
  IGiGaUIsession*   m_uiSession           ; ///< GiGa UI session              
  std::string       m_visManagerName      ; ///< GiGa Vis manager type/name   
  IGiGaVisManager*  m_visManager          ; ///< GiGa Vis manager             
  
  std::string       m_rndmSvcName         ; ///< name for random number service 
  IRndmGenSvc*      m_rndmSvc             ; ///< Gaudi random number service    

  bool              m_printParticles       ; ///< control print of G4Particles 
  
  typedef std::map<std::string,unsigned int> Counter;
  /// counter of errors 
  mutable Counter m_errors     ;
  /// counter of warning 
  mutable Counter m_warnings   ; 
  /// counter of exceptions
  mutable Counter m_exceptions ; 
  /// pointer to custom exception handler
  G4VExceptionHandler * m_exceptionHandler ;
};

// ============================================================================
/** assertion 
 *  @param assertion   assertion condition
 *  @param msg         assertion message 
 *  @param sc          assertion status code 
 */
// ============================================================================
inline StatusCode  GiGa::Assert
( bool               assertion , 
  const std::string& msg       , 
  const StatusCode&  sc        ) const 
{  
  StatusCode status = StatusCode::SUCCESS ;
  return (assertion) ? status : Exception( msg , MSG::FATAL , sc ) ;
};

// ============================================================================
/** assertion 
 *  @param assertion   assertion condition
 *  @param msg         assertion message 
 *  @param sc          assertion status code 
 */
// ============================================================================
inline StatusCode  GiGa::Assert
( bool               assertion , 
  const char*        msg       , 
  const StatusCode&  sc        ) const 
{ 
  StatusCode status = StatusCode::SUCCESS ;
  return (assertion) ? status : Exception( msg , MSG::FATAL , sc ) ;
};


#endif  ///<  GIGA_GIGASVC_H











