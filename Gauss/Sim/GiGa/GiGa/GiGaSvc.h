#ifndef     __GIGA_GIGASVC_GIGASVC_H__
#define     __GIGA_GIGASVC_GIGASVC_H__ 1 

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *   base classes         (from STL )                                      * ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <list>
#include <vector> 

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *   base classes         (from Gaudi framework)                           * ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////

#include  "GaudiKernel/Kernel.h"
#include  "GaudiKernel/StatusCode.h"
#include  "GaudiKernel/Service.h" 
#include  "GaudiKernel/IGiGaSvc.h"
#include  "GaudiKernel/IGiGaSetUpSvc.h"

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *   base classes         (from GiGa)                                      * ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////

#include "GiGa/GiGaException.h" 

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *   base classes         (from Geant4)                                    * ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////

  
///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *   forward declarations (from Gaudi framework)                           * ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////

class    IChronoStatSvc                  ;
class    ISvcLocator                     ;
template   <class TYPE> class SvcFactory ; 

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *   forward declarations (from GiGa)                                      * ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////

class    GiGaRunManager      ; 

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *   forward declarations (from Geant4 ToolKit)                            * ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////

class     G4UImanager        ; 
class     G4VisManager       ; 

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *************************************************************************** ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
///////////////////////////////////////////////////////////////////////////////////
///                                                                             ///
/// *************************************************************************** ///
/// *                                                                         * ///
/// * * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * * ///
/// *                                                                         * ///
/// *            Geant4 Interface for Gaudi  Applications                     * ///
/// *                                                                         * ///
/// * * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * * ///
/// *                                                                         * ///
/// *            Gaudi  Interface for Geant4 Applications                     * ///
/// *                                                                         * ///
/// * * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * * ///
/// *                                                                         * ///
/// *************************************************************************** ///
///                                                                             ///
///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *************************************************************************** ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////

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
  /// ************************************************************************* ///
  /// *           friend factory                                              * ///
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                           ///
  friend class SvcFactory<GiGaSvc>;                                             ///
  ///                                                                           ///
  /////////////////////////////////////////////////////////////////////////////////

 public:

  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *           useful definitions                                          * ///
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                           ///
  typedef std::vector<std::string>        Strings;                              /// 
  ///                                                                           ///
  /////////////////////////////////////////////////////////////////////////////////
  
 protected: 

  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *           constructor                                                 * ///
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                           ///
  GiGaSvc( const std::string& name, ISvcLocator* svc );                         ///
  ///                                                                           ///
  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *           (virtual) destructor                                        * ///
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                           ///
  virtual ~GiGaSvc();                                                           ///
  ///                                                                           ///
  /////////////////////////////////////////////////////////////////////////////////

 public:

  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// ************************************************************************* ///
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *           implementation of IGiGaSvc interface                        * ///
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////
  
  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *           set the initial kinematics for Geant4                       * ///
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *      add one more primary vertex or primary particle                  * /// 
  /// ************************************************************************* ///
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                           ///
  virtual IGiGaSvc&   operator <<         ( G4PrimaryVertex  * vertex   )     ; /// 
  ///                                                                           ///
  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                           ///
  virtual StatusCode  addPrimaryKinematics( G4PrimaryVertex  * vertex   )     ; /// 
  ///                                                                           ///
  /////////////////////////////////////////////////////////////////////////////////
  
  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* /// 
  /// *                retrieve results from Geant4                           * ///
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* /// 
  /// * retrieve the whole event, all hit collections,                        * ///
  /// *   specific hit colelctions  and trajectory containers                 * /// 
  /// ************************************************************************* ///
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                           ///
  virtual IGiGaSvc& operator >> ( const G4Event*         & event        )     ; /// 
  virtual IGiGaSvc& operator >> ( G4HCofThisEvent*       & collections  )     ; /// 
  virtual IGiGaSvc& operator >> ( CollectionPair         & collection   )     ; ///
  virtual IGiGaSvc& operator >> ( G4TrajectoryContainer* & trajectories )     ; ///
  ///                                                                           ///
  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                           ///
  virtual StatusCode retrieveEvent          ( const G4Event*          & )     ; ///
  virtual StatusCode retrieveHitCollections ( G4HCofThisEvent*        & )     ; ///
  virtual StatusCode retrieveHitCollection  ( CollectionPair          & )     ; ///
  virtual StatusCode retrieveTrajectories   ( G4TrajectoryContainer*  & )     ; ///
  ///                                                                           ///
  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// ************************************************************************* ///
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *           implementation of IGiGaSetUpSvc Interface                   * ///
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////
  
  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *           set detector construction                                   * ///
  /// ************************************************************************* ///
  /// *           define primary generator action                             * ///
  /// ************************************************************************* ///
  /// *           define Physics List                                         * ///
  /// ************************************************************************* ///
  /// *           define Run Action                                           * ///
  /// ************************************************************************* ///
  /// *           define Event Action                                         * ///
  /// ************************************************************************* ///
  /// *           define Stacking Action                                      * ///
  /// ************************************************************************* ///
  /// *           define Tracking Action                                      * ///
  /// ************************************************************************* ///
  /// *           define Stepping Action                                      * ///
  /// ************************************************************************* ///
  /// *           define Visualisation Manager                                * ///
  /// ************************************************************************* ///
  /// *           define Root of Geometry Tree                                * ///
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                           /// 
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
  ///                                                                           /// 
  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                           /// 
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
  ///                                                                           /// 
  /////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *             mandatory functions for any Service                       * /// 
  /// ************************************************************************* ///
  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *             initialize & finalize methods                             * /// 
  /// ************************************************************************* /// 
  ///                                                                           ///
  virtual StatusCode                    initialize()     ;                      ///
  virtual StatusCode                    finalize  ()     ;                      ///
  ///                                                                           ///
  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *             Query the interface                                       * ///                     
  /// ************************************************************************* /// 
  ///                                                                           /// 
  virtual StatusCode queryInterface ( const IID& riid, void** ppvI)    ;        ///
  ///                                                                           /// 
  /////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// ************************************************************************* ///
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *    end of public interface                                            * ///
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////

 protected: 
  
  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *    Internal protected functions                                       * ///
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                           ///
  StatusCode      createGiGaRunManager    () ;                                  ///
  StatusCode      prepareTheEvent  ( G4PrimaryVertex *  vertex );               ///
  StatusCode      retrieveTheEvent ( const G4Event   *& event  );               ///
  ///                                                                           ///
  /////////////////////////////////////////////////////////////////////////////////                
  
 private: 

  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *    Internal private functions                                         * ///
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                           ///
  inline  IChronoStatSvc*        chronoSvc () const { return m_chronoSvc ; } ;  ///
  ///                                                                           ///
  /////////////////////////////////////////////////////////////////////////////////


  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *    Internal private functions                                         * ///
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                           ///
  inline void Assert( bool  assertion                              ,            /// 
                      const std::string& msg = "GiGaSvc::unknown"  ,            ///                      
		      const StatusCode&  sc = StatusCode::FAILURE  ) ;          ///
  ///                                                                           ///
  inline void Assert( bool  assertion                              ,            /// 
                      const char*        msg                       ,            ///                      
		      const StatusCode&  sc  = StatusCode::FAILURE ) ;          ///
  ///                                                                           ///
  /////////////////////////////////////////////////////////////////////////////////

  
 private:

  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *            GiGaRunManager                                             * ///                     
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                           ///
  GiGaRunManager*                  m_GiGaRunManager;                            ///
  ///                                                                           ///
  /////////////////////////////////////////////////////////////////////////////////
  
  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *            Services                  (Gaudi)                          * ///                     
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                           ///    
  mutable IChronoStatSvc*          m_chronoSvc      ;                           ///
  ///                                                                           ///
  /////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *            UIcommands for G4RunManager                                * ///                     
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                           ///    
  Strings                          m_startUIcommands           ;                ///
  Strings                          m_startOfEvtUIcommands      ;                ///
  Strings                          m_endOfEvtUIcommands        ;                ///
  Strings                          m_startOfRunUIcommands      ;                ///
  Strings                          m_endOfRunUIcommands        ;                ///
  Strings                          m_endUIcommands             ;                ///
  ///                                                                           ///
  /////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////
  /// ************************************************************************* ///
  /// *            Available UIsessions                                       * ///                     
  /// ************************************************************************* /// 
  /////////////////////////////////////////////////////////////////////////////////
  ///                                                                           ///    
  Strings                m_UIsessions                ;                          ///
  ///                                                                           ///
  /////////////////////////////////////////////////////////////////////////////////

};

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *************************************************************************** ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////
inline void GiGaSvc::Assert( bool               assertion , 
			     const std::string& msg       , 
			     const StatusCode&  sc        ) 
{ 
  if( !assertion ) { throw GiGaException( name() + msg , sc ) ; } 
};
///
///
inline void GiGaSvc::Assert( bool               assertion , 
			     const char*        msg       , 
			     const StatusCode&  sc        ) 
{ 
  if( !assertion ) { throw GiGaException( name() + msg , sc ) ; } 
};
///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *************************************************************************** ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
///////////////////////////////////////////////////////////////////////////////////
///                                                                             ///
/// *************************************************************************** ///
/// *                                                                         * ///
/// * * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * * ///
/// *                                                                         * ///
/// *            Geant4 Interface for Gaudi  Applications                     * ///
/// *                                                                         * ///
/// * * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * * ///
/// *                                                                         * ///
/// *            Gaudi  Interface for Geant4 Applications                     * ///
/// *                                                                         * ///
/// * * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * GiGa * * ///
/// *                                                                         * ///
/// *************************************************************************** ///
///                                                                             ///
///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
///////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////
/// *************************************************************************** ///
/// *************************************************************************** ///
/// *************************************************************************** /// 
///////////////////////////////////////////////////////////////////////////////////


 
#endif  //  __GIGA_GIGASVC_GIGASVC_H__

