/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// ===========================================================================
#ifndef   GIGA_GIGARUNMANAGER_H
#define   GIGA_GIGARUNMANAGER_H  1
/// ===========================================================================

/// STD & STL 
#include <string>
#include <list>
#include <vector> 
/// GaudiKernel
#include   "GaudiKernel/Kernel.h"
#include   "GaudiKernel/StatusCode.h"
#include   "GaudiKernel/IService.h"
#include   "GaudiKernel/ISvcLocator.h"
#include   "GaudiKernel/IMessageSvc.h"
#include   "GaudiKernel/IChronoStatSvc.h"
/// GiGa 
#include   "GiGa/IGiGaGeoCnvSvc.h"
#include   "GiGa/GiGaException.h"
/// Geant4 
#include   "G4RunManager.hh" 
/// forward declarations (Gaudi)
class     IParticlePropertySvc          ;
class     IChronoStatSvc                ;
class     ISvcLocator                   ;
class     IGiGaGeomCnvSvc               ;
/// forward declarations (Geant4) 
class     G4VUserPrimaryGeneratorAction ;
class     G4VUserDetectorConstruction   ;
class     G4VUserPhysicsList            ;
class     G4UserRunAction               ;
class     G4UserEventAction             ;
class     G4UserStackingAction          ;
class     G4UserSteppingAction          ;
class     G4UserTrackingAction          ;
class     G4VisManager                  ; 
class     G4UIsession                   ;
class     G4UImanager                   ; 
/// GiGa
class     GiGaSvc                       ; 


/** @class GiGaRunManager GiGaRunManager.h GiGa/GiGaRunManager.h
 *
 *  Customized version of Genat4 Run Manager 
 *
 *  @author Vanya Belyaev 
 *  @date xx/xx/xxx 
 */

class GiGaRunManager: private G4RunManager
{
  /// only GiGa Svc can instantiate the run manager! 
  friend class GiGaSvc;
  ///
public:
  /// useful typedef 
  typedef std::vector<std::string>        Strings;
  ///
 protected:
  
  /** standard onstructor 
   *  @param name name of the run manager object
   *  @param svc  pointer to service locator 
   */
  GiGaRunManager( const std::string&  name , 
                  ISvcLocator*        svc  ) ;
  /// virtual destructor 
  virtual ~GiGaRunManager();  

 public:
  
  /** declare the Geant4 Primary Generator Action 
   *  @param obj pointer  to Geant4 Primary Generator Action 
   *  @return  status code 
   */
  virtual StatusCode declare( G4VUserPrimaryGeneratorAction  * obj ) ;

  /** declare the top level ("world") physical volume 
   *  @param obj pointer  to top level ("world") physical volume  
   *  @return  status code 
   */
  virtual StatusCode declare( G4VPhysicalVolume              * obj ) ;

  /** declare the Geant4 Detector Construction Action
   *  @param obj pointer  to Geant4 Detector Construction Action  
   *  @return  status code 
   */
  virtual StatusCode declare( G4VUserDetectorConstruction    * obj ) ;

  /** declare the Geant4 Physics List 
   *  @param obj pointer  to Geant4 Physics List  
   *  @return  status code 
   */
  virtual StatusCode declare( G4VUserPhysicsList             * obj ) ;

  /** declare the Geant4 Run Action 
   *  @param obj pointer  to Geant4 Run action  
   *  @return  status code 
   */
  virtual StatusCode declare( G4UserRunAction                * obj ) ;

  /** declare the Geant4 Event Action 
   *  @param obj pointer  to Geant4 Event  
   *  @return  status code 
   */
  virtual StatusCode declare( G4UserEventAction              * obj ) ;

  /** declare the Geant4 Stacking Action 
   *  @param obj pointer  to Geant4 Stacking Action 
   *  @return  status code 
   */
  virtual StatusCode declare( G4UserStackingAction           * obj ) ;

  /** declare the Geant4 Stepping  Action 
   *  @param obj pointer  to Geant4 Stepping Action 
   *  @return  status code 
   */
  virtual StatusCode declare( G4UserSteppingAction           * obj ) ;

  /** declare the Geant4 Tracking Action 
   *  @param obj pointer  to Geant4 Tracking Action 
   *  @return  status code 
   */
  virtual StatusCode declare( G4UserTrackingAction           * obj ) ;

  /** declare the Geant4 Visual Manager  
   *  @param obj pointer  to Geant4 Visual Manager
   *  @return  status code 
   */
  virtual StatusCode declare( G4VisManager                   * obj ) ;

  /** Prepare the event 
   *  @param vertex pointer to (main) primary vertex 
   *  @return status code 
   */
  virtual StatusCode  prepareTheEvent ( G4PrimaryVertex    * vertex = 0 ) ; 

  /** Process the prepared event 
   *  @return status code 
   */
  virtual StatusCode  processTheEvent (                                 ) ; 

  /** Retrieve the processed event 
   *  @param  event pointer to processed event  
   *  @return status code 
   */
  virtual StatusCode  retrieveTheEvent( const G4Event      *& event     ) ;


  /** set user interface session 
   *  @param st    ordered list of sessions 
   */
  void       set_UIsessions  ( const Strings& st ) { m_UIsessions = st; }  

  /** create  user interface session 
   *  @return  status code 
   */
  StatusCode createUIsession () ;
  
  /** retrieve the status of Geant4 kernel
   *  @return true if kernel is initialized properly 
   */
  inline bool krn_Is_Initialized () const { return m_krn_st ; } ;

  /** retrieve the status of Geant4 Run
   *  @return true if Run is initialized properly 
   */
  inline bool run_Is_Initialized () const { return m_run_st ; } ;

  /** retrieve the status of Geant4 Event preparation
   *  @return true if Event is prepared 
   */
  inline bool evt_Is_Prepared    () const { return m_pre_st ; } ;

  /** retrieve the status of Geant4 Event processing
   *  @return true if Event is processed 
   */
  inline bool evt_Is_Processed   () const { return m_pro_st ; } ;

  /** retrieve the status of Geant4 vis manager 
   *  @return true if Vis Manager is initializesd 
   */
  inline bool vis_Is_Initialized () const { return m_vis_st ; } ;

  /** retrieve the status of Geant4 User Interface Session 
   *  @return true if user interafce session is initializesd 
   */
  inline bool uis_Is_Started     () const { return m_uis_st ; } ;

  /** retrieve the name of run manager 
   *  @return name of run manager 
   */
  inline const std::string& name      () const { return m_name      ; } ;

  /** retrieve the pointer to message service 
   *  @return pointer to message service 
   */
  inline IMessageSvc*       msgSvc    () const { return m_msgSvc    ; } ;

  /** retrieve the pointer to Chrono & Stats service  
   *  @return pointer to Chrono & Stat service   
   */
  inline IChronoStatSvc*    chronoSvc () const { return m_chronoSvc ; } ;

  /** retrieve the pointer to Service Locator 
   *  @return pointer to Service Locator    
   */
  inline ISvcLocator*       svcLoc    () const { return m_svcLoc    ; } ;

  /** retrieve the pointer minimal geometry conversion service 
   *  @return pointer to minimal geometry conversion service 
   */
  IGiGaGeoCnvSvc*           cnvSvc    () const                          ;

  /// UI commands to be executed at start
  inline const Strings&  startUIcommands      () const { return m_s  ; } ;
  /// UI commands to be executed at start of each Event                   
  inline const Strings&  startOfEvtUIcommands () const { return m_sE ; } ;
  /// UI commands to be executed at end   of each Event                   
  inline const Strings&  endOfEvtUIcommands   () const { return m_eE ; } ;
  /// UI commands to be executed at start of each Run                     
  inline const Strings&  startOfRunUIcommands () const { return m_sR ; } ;
  /// UI commands to be executed at end   of each Run                     
  inline const Strings&  endOfRunUIcommands   () const { return m_eR ; } ;
  /// UI commands to be executed at end                                   
  inline const Strings&  endUIcommands        () const { return m_e  ; } ;
  /// defined UI sessions:                                                
  inline const Strings&  UIsessions           () const { return m_UIsessions ;}

 protected:

  /** overriden method from G4RunManager
   *  ONE SHOULD NOT USE IT!!!
   */
  virtual  void BeamOn( int         n_event        ,  
                        const char* macroFile =  0 , 
                        int         n_select  = -1 );
  /// initialize teh geometry 
  void       InitializeGeometry() ;
  /// initialize 
  void       Initialize()         ; 
  
 protected:

  /** initialize the Geant4 kernel
   *  @return status code 
   */
  StatusCode initializeKernel   () ;

  /** initialize the Geant4 Run
   *  @return status code 
   */
  StatusCode initializeRun      () ;

  /** finalize run manager 
   *  @return status code 
   */
  StatusCode finalizeRunManager () ;
  
  /// UI commands to be executed at start                                   
  inline void set_startUIcommands     ( const Strings& c ) { m_s   = c ; } ;
  /// UI commands to be executed at start of each Event                     
  inline void set_startOfEvtUIcommands( const Strings& c ) { m_sE  = c ; } ;
  /// UI commands to be executed at end   of each Event                     
  inline void set_endOfEvtUIcommands  ( const Strings& c ) { m_eE  = c ; } ;
  /// UI commands to be executed at start of each Run                       
  inline void set_startOfRunUIcommands( const Strings& c ) { m_sR  = c ; } ;
  /// UI commands to be executed at end   of each Run                       
  inline void set_endOfRunUIcommands  ( const Strings& c ) { m_eR  = c ; } ;
  /// UI commands to be executed at end                                     
  inline void set_endUIcommands       ( const Strings& c ) { m_e   = c ; } ;

 private:

  inline void set_krn_Is_Initialized ( bool st ) { m_krn_st = st ; }
  inline void set_run_Is_Initialized ( bool st ) { m_run_st = st ; }
  inline void set_evt_Is_Prepared    ( bool st ) { m_pre_st = st ; }
  inline void set_evt_Is_Processed   ( bool st ) { m_pro_st = st ; }
  inline void set_vis_Is_Initialized ( bool st ) { m_vis_st = st ; }
  inline void set_uis_Is_Started     ( bool st ) { m_uis_st = st ; }
  
  /** Assertion
   *  @param assertion condition to be checked 
   *  @param msg       message
   *  @param sc        status code 
   */
  inline void Assert( bool  assertion                              ,  
                      const std::string& msg = "GiGaSvc::unknown"  ,  
                      const StatusCode&  sc = StatusCode::FAILURE  ) ;
 private: 

  ISvcLocator*               m_svcLoc       ; 
  G4VPhysicalVolume*         m_rootGeo      ;
  mutable IGiGaGeoCnvSvc*    m_cnvSvc       ;
  G4UIsession*               m_g4UIsession  ;
  G4VisManager*              m_g4VisManager ;

  bool                       m_krn_st ;
  bool                       m_run_st ;
  bool                       m_pre_st ;
  bool                       m_pro_st ;
  bool                       m_vis_st ;
  bool                       m_uis_st ;

  std::string                m_name       ; 
  IMessageSvc*               m_msgSvc     ; 
  IChronoStatSvc*            m_chronoSvc  ;

  Strings                    m_s          ;
  Strings                    m_sE         ;
  Strings                    m_eE         ;
  Strings                    m_sR         ;
  Strings                    m_eR         ;
  Strings                    m_e          ;

  Strings                    m_UIsessions ;

};

/// ===========================================================================
/** Assertion
 *  @param assertion condition to be checked 
 *  @param msg       message
 *  @param sc        status code 
 */
/// ===========================================================================
inline void GiGaRunManager::Assert( bool  assertion        , 
                                    const std::string& msg , 
                                    const StatusCode &  sc ) 
{ if( !assertion ) { throw GiGaException("GiGaRunManager::"+msg , sc ) ; } };



/// ===========================================================================
#endif  ///< GIGA_GIGARUNMANAGER_H
/// ===========================================================================






