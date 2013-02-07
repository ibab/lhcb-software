// $Id: GiGaRunManager.h,v 1.9 2007-03-18 18:25:05 gcorti Exp $ 
#ifndef   GIGA_GIGARUNMANAGER_H
#define   GIGA_GIGARUNMANAGER_H  1

// Include files
// from STD & STL 
#include <string>
#include <list>
#include <vector> 

// from Gaudi
#include   "GaudiKernel/Kernel.h"
#include   "GaudiKernel/StatusCode.h"
#include   "GaudiKernel/IService.h"
#include   "GaudiKernel/ISvcLocator.h"
#include   "GaudiKernel/IMessageSvc.h"
#include   "GaudiKernel/IChronoStatSvc.h"

// from GiGa 
#include   "GiGa/GiGaException.h"
#include   "GiGa/GiGaBase.h"
#include   "GiGa/IGiGaRunManager.h"
#include   "GiGa/IGiGaTool.h"

// from Geant4 
#include   "G4RunManager.hh" 

// forward declarations
namespace LHCb { 
  class IParticlePropertySvc; 
}
class     IChronoStatSvc                 ;
class     ISvcLocator                    ;
class     IGiGaGeoSrc                    ;
class     G4UIsession                    ;
class     G4UImanager                    ; 


/** @class GiGaRunManager GiGaRunManager.h src/component/GiGaRunManager.h
 *
 *  Customized version of Geant4 Run Manager 
 *
 *  @author Vanya Belyaev 
 *  @date xxxx-xx-xx 
 *  @author Gloria Corti 
 *  @date 2007-01-08 
 */

class GiGaRunManager: public  virtual IGiGaRunManager  ,
                      public  virtual  GiGaBase        ,    
                      private virtual G4RunManager 
{
public:

  /** standard constructor
   *  @see  GiGaBase 
   *  @see   AlgTool
   *  @param type type of the run manager object
   *  @param name name of the run manager object
   *  @param parent pointer to parent object  
   */
  GiGaRunManager
  ( const std::string&  type   , 
    const std::string&  name   , 
    const IInterface*   parent ) ;
  
  /// virtual destructor 
  virtual ~GiGaRunManager();  

  /** declare the Geant4 Primary Generator Action 
   *  @see IGiGaRunManager 
   *  @param obj pointer  to Geant4 Primary Generator Action 
   *  @return  status code 
   */
  virtual StatusCode declare( G4VUserPrimaryGeneratorAction  * obj ) ;

  /** declare the top level ("world") physical volume 
   *  @see IGiGaRunManager 
   *  @param obj pointer  to top level ("world") physical volume  
   *  @return  status code 
   */
  virtual StatusCode declare( G4VPhysicalVolume              * obj ) ;

  /** declare the Geant4 Detector Construction Action
   *  @see IGiGaRunManager 
   *  @param obj pointer  to Geant4 Detector Construction Action  
   *  @return  status code 
   */
  virtual StatusCode declare( G4VUserDetectorConstruction    * obj ) ;

  /** declare the Geant4 Physics List 
   *  @see IGiGaRunManager 
   *  @param obj pointer  to Geant4 Physics List  
   *  @return  status code 
   */
  virtual StatusCode declare( G4VUserPhysicsList             * obj ) ;

  /** declare the GiGa geometry source  
   *  @see IGiGaRunManager 
   *  @param obj pointer  to GiGa Geometry source   
   *  @return  status code 
   */
  virtual StatusCode declare( IGiGaGeoSrc                    * obj ) ;

  /** declare the Geant4 Run Action 
   *  @see IGiGaRunManager 
   *  @param obj pointer  to Geant4 Run action  
   *  @return  status code 
   */
  virtual StatusCode declare( G4UserRunAction                * obj ) ;

  /** declare the Geant4 Event Action 
   *  @see IGiGaRunManager 
   *  @param obj pointer  to Geant4 Event  
   *  @return  status code 
   */
  virtual StatusCode declare( G4UserEventAction              * obj ) ;

  /** declare the Geant4 Stacking Action 
   *  @see IGiGaRunManager 
   *  @param obj pointer  to Geant4 Stacking Action 
   *  @return  status code 
   */
  virtual StatusCode declare( G4UserStackingAction           * obj ) ;

  /** declare the Geant4 Stepping  Action 
   *  @see IGiGaRunManager 
   *  @param obj pointer  to Geant4 Stepping Action 
   *  @return  status code 
   */
  virtual StatusCode declare( G4UserSteppingAction           * obj ) ;

  /** declare the Geant4 Tracking Action 
   *  @see IGiGaRunManager 
   *  @param obj pointer  to Geant4 Tracking Action 
   *  @return  status code 
   */
  virtual StatusCode declare( G4UserTrackingAction           * obj ) ;

  /** Prepare the event 
   *  @see IGiGaRunManager 
   *  @param vertex pointer to (main) primary vertex 
   *  @return status code 
   */
  virtual StatusCode  prepareTheEvent ( G4PrimaryVertex    * vertex = 0 ) ; 

  /** Process the prepared event 
   *  @see IGiGaRunManager 
   *  @return status code 
   */
  virtual StatusCode  processTheEvent (                                 ) ; 

  /** Retrieve the processed event 
   *  @see IGiGaRunManager 
   *  @param  event pointer to processed event  
   *  @return status code 
   */
  virtual StatusCode  retrieveTheEvent( const G4Event      *& event     ) ;

  /** declare the Geant4 User Interface session 
   *  @see IGiGaRunManager 
   *  @param obj pointer  to Geant4 User Interface session  
   *  @return  status code 
   */
  virtual StatusCode declare( G4UIsession                    * obj     ) ;

  /** initialize the run manager 
   *  @see    GiGaBase 
   *  @see     AlgBase 
   *  @see    IAlgBase 
   *  @return status code
   */ 
  virtual StatusCode initialize () ;
  
  /** finalize the run manager 
   *  @see    GiGaBase 
   *  @see     AlgBase 
   *  @see    IAlgBase 
   *  @return status code
   */ 
  virtual StatusCode finalize   () ;

  virtual unsigned long release()
  {
    unsigned long count = addRef() ;
    if ( count >= 2 )
    {
      AlgTool::release() ;
      return AlgTool::release() ;
    }
    return 0 ;
  };
  
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

  /// default constructor  is private
  GiGaRunManager() ;
  /// copy    constructor  is private
  GiGaRunManager           ( const GiGaRunManager& ) ;
  /// assignement operator is private
  GiGaRunManager& operator=( const GiGaRunManager& ) ;
  
private:
  
  inline void set_krn_Is_Initialized ( bool st ) { m_krn_st = st ; }
  inline void set_run_Is_Initialized ( bool st ) { m_run_st = st ; }
  inline void set_evt_Is_Prepared    ( bool st ) { m_pre_st = st ; }
  inline void set_evt_Is_Processed   ( bool st ) { m_pro_st = st ; }
  inline void set_uis_Is_Started     ( bool st ) { m_uis_st = st ; }
  
private: 
  
  bool                       m_krn_st          ;
  bool                       m_run_st          ;
  bool                       m_pre_st          ;
  bool                       m_pro_st          ;
  bool                       m_uis_st          ;
  
  G4VPhysicalVolume*         m_rootGeo         ;
  IGiGaGeoSrc*               m_geoSrc          ;
  G4UIsession*               m_g4UIsession     ;

  bool                       m_delDetConstr    ;
  bool                       m_delPrimGen      ;
  bool                       m_delPhysList     ;
  bool                       m_delRunAction    ;
  bool                       m_delEventAction  ;
  bool                       m_delStackAction  ;
  bool                       m_delTrackAction  ;
  bool                       m_delStepAction   ;

  typedef std::vector<std::string> Names ;
  typedef std::vector<IGiGaTool*>  Tools ;
  
  Names                      m_runToolsList  ;
  Tools                      m_runTools      ;

  int                        m_verbosity       ;
  
};
#endif  ///< GIGA_GIGARUNMANAGER_H






