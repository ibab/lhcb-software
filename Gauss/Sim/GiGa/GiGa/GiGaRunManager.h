// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.10  2001/08/01 09:42:21  ibelyaev
/// redesign and reimplementation of GiGaRunManager class
///
/// Revision 1.9  2001/07/27 14:28:59  ibelyaev
/// bug fix
///
/// Revision 1.8  2001/07/25 17:18:07  ibelyaev
/// move all conversions from GiGa to GiGaCnv
///
/// Revision 1.7  2001/07/23 13:11:42  ibelyaev
/// the package restructurisation(II)
///
// ============================================================================
#ifndef   GIGA_GIGARUNMANAGER_H
#define   GIGA_GIGARUNMANAGER_H  1
// ============================================================================

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
#include   "GiGa/GiGaException.h"
#include   "GiGa/GiGaBase.h"
#include   "GiGa/IGiGaRunManager.h"
/// Geant4 
#include   "G4RunManager.hh" 
/// forward declarations (Gaudi)
class     IParticlePropertySvc          ;
class     IChronoStatSvc                ;
class     ISvcLocator                   ;
class     IGiGaGeoSrc                   ;
class     G4UIsession                   ;
class     G4UImanager                   ; 


/** @class GiGaRunManager GiGaRunManager.h GiGa/GiGaRunManager.h
 *
 *  Customized version of Genat4 Run Manager 
 *
 *  @author Vanya Belyaev 
 *  @date xx/xx/xxx 
 */

class GiGaRunManager: public  virtual IGiGaRunManager ,
                      private            G4RunManager ,
                      private          GiGaBase    
{
  ///
public:
  
  /** standard onstructor 
   *  @param name name of the run manager object
   *  @param svc  pointer to service locator 
   */
  GiGaRunManager( const std::string&  name , 
                  ISvcLocator*        svc  ) ;
  /// virtual destructor 
  virtual ~GiGaRunManager();  

public:

  /** identification 
   *  @return name of concrete inteface instance 
   */
  virtual const std::string& GiGaRunManager::name () const;
  
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

  /** declare the GiGa geometry source  
   *  @param obj pointer  to GiGa Geometry source   
   *  @return  status code 
   */
  virtual StatusCode declare( IGiGaGeoSrc                    * obj ) ;

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

  /** declare the Geant4 User Interface session 
   *  @param obj pointer  to Geant4 User Interface session  
   *  @return  status code 
   */
  virtual StatusCode declare( G4UIsession                    * obj     ) ;

  /** initialize the run manager 
   *  @return status code
   */ 
  virtual StatusCode initialize () ;
  
  /** finalize the run manager 
   *  @return status code
   */ 
  virtual StatusCode finalize   () ;

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

  /** retrieve the status of Geant4 User Interface Session 
   *  @return true if user interafce session is initializesd 
   */
  inline bool uis_Is_Started     () const { return m_uis_st ; } ;

  /** retrieve the pointer minimal geometry information source 
   *  @return pointer to minimal geometry information source  
   */
  inline IGiGaGeoSrc*       geoSrc    () const { return m_geoSrc    ; } ;

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
  
private:
  
  /// no default constructor 
  GiGaRunManager();
  /// no copy constructor 
  GiGaRunManager( const  GiGaRunManager& );
  /// no assignment 
  GiGaRunManager& operator=( const  GiGaRunManager& );
  
private:
  
  inline void set_krn_Is_Initialized ( bool st ) { m_krn_st = st ; }
  inline void set_run_Is_Initialized ( bool st ) { m_run_st = st ; }
  inline void set_evt_Is_Prepared    ( bool st ) { m_pre_st = st ; }
  inline void set_evt_Is_Processed   ( bool st ) { m_pro_st = st ; }
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

  bool                       m_krn_st       ;
  bool                       m_run_st       ;
  bool                       m_pre_st       ;
  bool                       m_pro_st       ;
  bool                       m_uis_st       ;
  
  G4VPhysicalVolume*         m_rootGeo      ;
  IGiGaGeoSrc*               m_geoSrc       ;
  G4UIsession*               m_g4UIsession  ;

};

// ============================================================================
/** Assertion
 *  @param assertion condition to be checked 
 *  @param msg       message
 *  @param sc        status code 
 */
// ============================================================================
inline void GiGaRunManager::Assert( bool  assertion        , 
                                    const std::string& msg , 
                                    const StatusCode &  sc ) 
{ if( !assertion ) { Exception(msg , MSG::FATAL , sc ) ; } };


// ============================================================================
#endif  ///< GIGA_GIGARUNMANAGER_H
// ============================================================================






