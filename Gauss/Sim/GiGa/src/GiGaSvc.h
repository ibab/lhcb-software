#ifndef     __GIGA_GIGASVC_GIGASVC_H__
#define     __GIGA_GIGASVC_GIGASVC_H__ 1 

///////////////////////////////////////////////////////////////////////////////////
#include <string>
#include <list>
#include <vector> 
///////////////////////////////////////////////////////////////////////////////////
#include  "GaudiKernel/Kernel.h"
#include  "GaudiKernel/StatusCode.h"
#include  "GaudiKernel/Service.h" 
#include  "GaudiKernel/IGiGaSvc.h"
#include  "GaudiKernel/IGiGaSetUpSvc.h"
///////////////////////////////////////////////////////////////////////////////////
#include "GiGa/GiGaException.h" 
///////////////////////////////////////////////////////////////////////////////////
class     IChronoStatSvc                  ;
class     ISvcLocator                     ;
class     IObjManager                     ;
template   <class TYPE> class SvcFactory ; 
///////////////////////////////////////////////////////////////////////////////////
class     GiGaRunManager      ; 
///////////////////////////////////////////////////////////////////////////////////
class     G4UImanager        ; 
class     G4VisManager       ; 

class     IGiGaPhysList;
class     IGiGaStackAction;
class     IGiGaTrackAction;
class     IGiGaStepAction;

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///                                                                             ///
///    class GiGaSvc : implementation of abstract Interfaces IGiGaSvc           ///      
///                    (for event-by-event communications with Geant4)          ///
///                    and IGiGaSetUpSvc (for configuration of Geant4)          ///
///                                                                             ///
///    Author: Vanya Belyaev                                                    ///
///                                                                             ///
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////


class GiGaSvc: public         Service       , 
	       virtual public IGiGaSvc      ,
               virtual public IGiGaSetUpSvc 
{
  /////////////////////////////////////////////////////////////////////////////////
  friend class SvcFactory<GiGaSvc>;                                             ///
  /////////////////////////////////////////////////////////////////////////////////

 public:

  /////////////////////////////////////////////////////////////////////////////////
  typedef std::vector<std::string>        Strings;                              /// 
  /////////////////////////////////////////////////////////////////////////////////
  
 protected: 

  /////////////////////////////////////////////////////////////////////////////////
  GiGaSvc( const std::string& name, ISvcLocator* svc );                         ///
  /////////////////////////////////////////////////////////////////////////////////
  virtual ~GiGaSvc();                                                           ///
  /////////////////////////////////////////////////////////////////////////////////

 public:

  /////////////////////////////////////////////////////////////////////////////////
  virtual IGiGaSvc&   operator <<         ( G4PrimaryVertex  * vertex   )     ; /// 
  /////////////////////////////////////////////////////////////////////////////////
  virtual StatusCode  addPrimaryKinematics( G4PrimaryVertex  * vertex   )     ; /// 
  /////////////////////////////////////////////////////////////////////////////////
  virtual IGiGaSvc& operator >> ( const G4Event*         & event        )     ; /// 
  virtual IGiGaSvc& operator >> ( G4HCofThisEvent*       & collections  )     ; /// 
  virtual IGiGaSvc& operator >> ( CollectionPair         & collection   )     ; ///
  virtual IGiGaSvc& operator >> ( G4TrajectoryContainer* & trajectories )     ; ///
  /////////////////////////////////////////////////////////////////////////////////
  virtual StatusCode retrieveEvent          ( const G4Event*          & )     ; ///
  virtual StatusCode retrieveHitCollections ( G4HCofThisEvent*        & )     ; ///
  virtual StatusCode retrieveHitCollection  ( CollectionPair          & )     ; ///
  virtual StatusCode retrieveTrajectories   ( G4TrajectoryContainer*  & )     ; ///
  /////////////////////////////////////////////////////////////////////////////////
  virtual IGiGaSetUpSvc& operator << ( G4VUserDetectorConstruction   * )     ;  ///
  virtual IGiGaSetUpSvc& operator << ( G4VPhysicalVolume             * )     ;  ///
  virtual IGiGaSetUpSvc& operator << ( G4VUserPrimaryGeneratorAction * )     ;  ///
  virtual IGiGaSetUpSvc& operator << ( G4VUserPhysicsList            * )     ;  ///
  virtual IGiGaSetUpSvc& operator << ( G4UserRunAction               * )     ;  ///
  virtual IGiGaSetUpSvc& operator << ( G4UserEventAction             * )     ;  ///
  virtual IGiGaSetUpSvc& operator << ( G4UserStackingAction          * )     ;  ///
  virtual IGiGaSetUpSvc& operator << ( G4UserTrackingAction          * )     ;  ///
  virtual IGiGaSetUpSvc& operator << ( G4UserSteppingAction          * )     ;  ///
  virtual IGiGaSetUpSvc& operator << ( G4VisManager                  * )     ;  ///
  /////////////////////////////////////////////////////////////////////////////////
  virtual StatusCode setConstruction ( G4VUserDetectorConstruction   * )     ;  /// 
  virtual StatusCode setDetector     ( G4VPhysicalVolume             * )     ;  /// 
  virtual StatusCode setGenerator    ( G4VUserPrimaryGeneratorAction * )     ;  /// 
  virtual StatusCode setPhysics      ( G4VUserPhysicsList            * )     ;  /// 
  virtual StatusCode setRunAction    ( G4UserRunAction               * )     ;  /// 
  virtual StatusCode setEvtAction    ( G4UserEventAction             * )     ;  /// 
  virtual StatusCode setStacking     ( G4UserStackingAction          * )     ;  /// 
  virtual StatusCode setTracking     ( G4UserTrackingAction          * )     ;  /// 
  virtual StatusCode setStepping     ( G4UserSteppingAction          * )     ;  /// 
  virtual StatusCode setVisManager   ( G4VisManager                  * )     ;  /// 
  /////////////////////////////////////////////////////////////////////////////////
  virtual StatusCode                    initialize()     ;                      ///
  virtual StatusCode                    finalize  ()     ;                      ///
  /////////////////////////////////////////////////////////////////////////////////
  virtual StatusCode queryInterface ( const IID& riid, void** ppvI)    ;        ///
  /////////////////////////////////////////////////////////////////////////////////

 protected: 
  
  /////////////////////////////////////////////////////////////////////////////////
  StatusCode      createGiGaRunManager    () ;                                  ///
  StatusCode      prepareTheEvent  ( G4PrimaryVertex *  vertex );               ///
  StatusCode      retrieveTheEvent ( const G4Event   *& event  );               ///
  /////////////////////////////////////////////////////////////////////////////////                
  
 private: 

  /////////////////////////////////////////////////////////////////////////////////
  inline  IChronoStatSvc*        chronoSvc () const { return m_chronoSvc ; } ;  ///
  inline  IObjManager*           objMgr    () const { return m_objMgr    ; } ;  ///
  /////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////
  inline void Assert( bool  assertion                              ,            /// 
                      const std::string& msg = "GiGaSvc::unknown"  ,            ///                      
		      const StatusCode&  sc = StatusCode::FAILURE  ) ;          ///
  /////////////////////////////////////////////////////////////////////////////////
  inline void Assert( bool  assertion                              ,            /// 
                      const char*        msg                       ,            ///                      
		      const StatusCode&  sc  = StatusCode::FAILURE ) ;          ///
  /////////////////////////////////////////////////////////////////////////////////
  inline StatusCode Error     ( const std::string    & msg ,  
				const StatusCode     & sc  = StatusCode::FAILURE );
  inline StatusCode Warning   ( const std::string    & msg ,  
				const StatusCode     & sc  = StatusCode::FAILURE );
  inline StatusCode Print     ( const std::string    & msg ,  
				const MSG::Level     & lvl = MSG::INFO           ,
				const StatusCode     & sc  = StatusCode::FAILURE );
  inline StatusCode Exception ( const std::string    & msg  ,  
                                const GaudiException & exc  , 
				const MSG::Level     & lvl = MSG::FATAL           ,
				const StatusCode     & sc  = StatusCode::FAILURE );
  inline StatusCode Exception ( const std::string    & msg  ,  
                                const std::exception & exc  , 
				const MSG::Level     & lvl = MSG::FATAL           ,
				const StatusCode     & sc  = StatusCode::FAILURE );
  inline StatusCode Exception ( const std::string    & msg  ,  
				const MSG::Level     & lvl = MSG::FATAL           ,
				const StatusCode     & sc  = StatusCode::FAILURE );
  /////////////////////////////////////////////////////////////////////////////////
  StatusCode physList      ( const std::string& TypeAndName , 
			     IGiGaPhysList*&    PhisicsList ) ;
  /////////////////////////////////////////////////////////////////////////////////
  StatusCode stackAction   ( const std::string& TypeAndName , 
			     IGiGaStackAction*& StackAction ) ;
  /////////////////////////////////////////////////////////////////////////////////
  StatusCode trackAction   ( const std::string& TypeAndName , 
			     IGiGaTrackAction*& TrackAction ) ;
  /////////////////////////////////////////////////////////////////////////////////
  StatusCode stepAction    ( const std::string& TypeAndName , 
			     IGiGaStepAction*&  StepAction  ) ;
  /////////////////////////////////////////////////////////////////////////////////
  
 private:
  
  /////////////////////////////////////////////////////////////////////////////////
  GiGaRunManager*                  m_GiGaRunManager            ;               
  /////////////////////////////////////////////////////////////////////////////////
  IChronoStatSvc*                  m_chronoSvc                 ;                
  IObjManager*                     m_objMgr                    ;
  std::string                      m_objMgrName                ; 
  /////////////////////////////////////////////////////////////////////////////////
  Strings                          m_startUIcommands           ;                ///
  Strings                          m_startOfEvtUIcommands      ;                ///
  Strings                          m_endOfEvtUIcommands        ;                ///
  Strings                          m_startOfRunUIcommands      ;                ///
  Strings                          m_endOfRunUIcommands        ;                ///
  Strings                          m_endUIcommands             ;                ///
  /////////////////////////////////////////////////////////////////////////////////
  Strings                          m_UIsessions                ;                ///
  /////////////////////////////////////////////////////////////////////////////////
  /// PhysicsList Object
  std::string                      m_GiGaPhysList                    ;   /// type/name 
  /// Stacking Action Object 
  std::string                      m_GiGaStackAction                 ;   /// type/name 
  /// Tracking Action Object 
  std::string                      m_GiGaTrackAction                 ;   /// type/name 
  /// Stepping Action Object 
  std::string                      m_GiGaStepAction                  ;   /// type/name 
  /////////////////////////////////////////////////////////////////////////////////
  bool                             m_UseVisManager                   ; 
  /////////////////////////////////////////////////////////////////////////////////
};
///////////////////////////////////////////////////////////////////////////////////
inline void GiGaSvc::Assert( bool               assertion , 
			     const std::string& msg       , 
			     const StatusCode&  sc        ) 
{ if( !assertion ) { throw GiGaException( name() + msg , sc ) ; } };
///
inline void GiGaSvc::Assert( bool               assertion , 
			     const char*        msg       , 
			     const StatusCode&  sc        ) 
{ if( !assertion ) { throw GiGaException( name() + msg , sc ) ; } };
///////////////////////////////////////////////////////////////////////////////////
inline StatusCode GiGaSvc::Error( const std::string& Message , const StatusCode & Status )
{ return  Print( Message , MSG::ERROR  , Status  ) ; };  
///////////////////////////////////////////////////////////////////////////////////
inline StatusCode GiGaSvc::Warning( const std::string& Message , const StatusCode & Status )
{ return  Print( Message , MSG::WARNING , Status ) ; };  
///////////////////////////////////////////////////////////////////////////////////
inline StatusCode GiGaSvc::Print( const std::string& Message , 
				  const MSG::Level & level   , 
				  const StatusCode & Status )
{
  MsgStream log( msgSvc() , name() ); log << level << Message << endreq ; 
  return  Status;
};  
/////////////////////////////////////////////////////////////////////////////////// 
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
/////////////////////////////////////////////////////////////////////////////////// 
inline StatusCode GiGaSvc::Exception( const std::string    & Message , 
                                      const std::exception & Excp    ,
				      const MSG::Level     & level   , 
				      const StatusCode     & Status )
{
  Stat stat( chronoSvc() , "std::exception" );
  MsgStream log( msgSvc() , name() + ":std::exception" ); 
  log << level << Message << ":" << Excp.what() << endreq ; 
  return  Status;
};  
///////////////////////////////////////////////////////////////////////////////////
inline StatusCode GiGaSvc::Exception( const std::string    & Message , 
				      const MSG::Level     & level   , 
				      const StatusCode     & Status )
{
  Stat stat( chronoSvc() , "*UNKNOWN* exception" );
  MsgStream log( msgSvc() , name() + ":UNKNOWN exception" ); 
  log << level << Message << ": UNKNOWN exception"  << endreq ; 
  return  Status;
};  
///////////////////////////////////////////////////////////////////////////////////
 

#endif  //  __GIGA_GIGASVC_GIGASVC_H__











