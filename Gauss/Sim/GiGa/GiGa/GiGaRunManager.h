// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/GiGa/GiGaRunManager.h,v 1.3 2001-04-23 08:51:54 ibelyaev Exp $ 
#ifndef     GIGA_GIGARUNMANAGER_H
#define     GIGA_GIGARUNMANAGER_H  1

///
#include <string>
#include <list>
#include <vector> 
///
#include   "GaudiKernel/Kernel.h"
#include   "GaudiKernel/StatusCode.h"
#include   "GaudiKernel/IService.h"
#include   "GaudiKernel/ISvcLocator.h"
#include   "GaudiKernel/IMessageSvc.h"
#include   "GaudiKernel/IChronoStatSvc.h"
///
#include   "GiGa/IGiGaGeomCnvSvc.h"
#include   "GiGa/GiGaException.h"
///
#include   "G4RunManager.hh" 
///
class     IParticlePropertySvc          ;
class     IChronoStatSvc                ;
class     ISvcLocator                   ;
class     IGiGaGeomCnvSvc               ;
///
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
///
class     GiGaSvc                       ; 
///

/**    @class GiGaRunManager GiGaRunManager.h GiGa/GiGaRunManager.h

       an adaptation of generic G4RunManager to Gaudi   
       @author: Vanya Belyaev
*/

class GiGaRunManager: private G4RunManager
{
  
  /////////////////////////////////////////////////////////////////////////////////
  friend class GiGaSvc;
  /////////////////////////////////////////////////////////////////////////////////

 public: 

  /////////////////////////////////////////////////////////////////////////////////
  typedef std::vector<std::string>        Strings;                              ///
  /////////////////////////////////////////////////////////////////////////////////

 protected:

  /////////////////////////////////////////////////////////////////////////////////
  GiGaRunManager( const std::string& , ISvcLocator* );                          ///
  /////////////////////////////////////////////////////////////////////////////////
  virtual ~GiGaRunManager();                                                    ///
  /////////////////////////////////////////////////////////////////////////////////

 public:
  
  /////////////////////////////////////////////////////////////////////////////////
  virtual StatusCode declare( G4VUserPrimaryGeneratorAction  * ) ;              /// 
  virtual StatusCode declare( G4VPhysicalVolume              * ) ;              /// 
  virtual StatusCode declare( G4VUserDetectorConstruction    * ) ;              ///
  virtual StatusCode declare( G4VUserPhysicsList             * ) ;              /// 
  virtual StatusCode declare( G4UserRunAction                * ) ;              /// 
  virtual StatusCode declare( G4UserEventAction              * ) ;              ///
  virtual StatusCode declare( G4UserStackingAction           * ) ;              /// 
  virtual StatusCode declare( G4UserSteppingAction           * ) ;              ///
  virtual StatusCode declare( G4UserTrackingAction           * ) ;              ///
  virtual StatusCode declare( G4VisManager                   * ) ;              ///
  /////////////////////////////////////////////////////////////////////////////////
  virtual StatusCode  prepareTheEvent ( G4PrimaryVertex    * vertex = 0 ) ;     ///
  virtual StatusCode  processTheEvent (                                 ) ;     /// 
  virtual StatusCode  retrieveTheEvent( const G4Event      *& event     ) ;     /// 
  /////////////////////////////////////////////////////////////////////////////////
  void       set_UIsessions  ( const Strings& st ) { m_UIsessions = st; }       ///
  StatusCode createUIsession () ;                                               /// 
  /////////////////////////////////////////////////////////////////////////////////
  inline bool krn_Is_Initialized () const { return m_krn_st ; }                 ///
  inline bool run_Is_Initialized () const { return m_run_st ; }                 ///
  inline bool evt_Is_Prepared    () const { return m_pre_st ; }                 ///
  inline bool evt_Is_Processed   () const { return m_pro_st ; }                 ///
  inline bool vis_Is_Initialized () const { return m_vis_st ; }                 ///
  inline bool uis_Is_Started     () const { return m_uis_st ; }                 ///
  /////////////////////////////////////////////////////////////////////////////////
  inline const std::string& name      () const { return m_name      ; } ;       ///
  inline IMessageSvc*       msgSvc    () const { return m_msgSvc    ; } ;       ///
  inline IChronoStatSvc*    chronoSvc () const { return m_chronoSvc ; } ;       ///
  inline ISvcLocator*       svcLoc    () const { return m_svcLoc    ; } ;       ///
  inline IGiGaGeomCnvSvc*   cnvSvc    () const                          ;       ///
  /////////////////////////////////////////////////////////////////////////////////
  /// UI commands to be executed at start                                       ///
  inline const Strings&  startUIcommands      () const { return m_s  ; } ;      ///
  /// UI commands to be executed at start of each Event                         ///
  inline const Strings&  startOfEvtUIcommands () const { return m_sE ; } ;      ///
  /// UI commands to be executed at end   of each Event                         ///
  inline const Strings&  endOfEvtUIcommands   () const { return m_eE ; } ;      ///
  /// UI commands to be executed at start of each Run                           ///
  inline const Strings&  startOfRunUIcommands () const { return m_sR ; } ;      ///
  /// UI commands to be executed at end   of each Run                           ///
  inline const Strings&  endOfRunUIcommands   () const { return m_eR ; } ;      ///
  /// UI commands to be executed at end                                         ///
  inline const Strings&  endUIcommands        () const { return m_e  ; } ;      ///
  /// defined UI sessions:                                                      ///
  inline const Strings&  UIsessions           () const { return m_UIsessions ;} ///
  /////////////////////////////////////////////////////////////////////////////////

 protected:

  /////////////////////////////////////////////////////////////////////////////////
  virtual  void BeamOn( int         n_event        ,                            ///
                        const char* macroFile =  0 ,                            /// 
                        int         n_select  = -1 );                           ///
  /////////////////////////////////////////////////////////////////////////////////
  void       InitializeGeometry() ;                                             /// 
  void       Initialize()         ;                                             /// 
  /////////////////////////////////////////////////////////////////////////////////
  
 protected:

  /////////////////////////////////////////////////////////////////////////////////
  StatusCode initializeKernel   () ;                                            ///
  StatusCode initializeRun      () ;                                            ///
  StatusCode finalizeRunManager () ;                                            ///
  /////////////////////////////////////////////////////////////////////////////////
  
  /////////////////////////////////////////////////////////////////////////////////
  /// UI commands to be executed at start                                       ///
  inline void set_startUIcommands     ( const Strings& c ) { m_s   = c ; } ;    /// 
  /// UI commands to be executed at start of each Event                         ///
  inline void set_startOfEvtUIcommands( const Strings& c ) { m_sE  = c ; } ;    ///
  /// UI commands to be executed at end   of each Event                         ///
  inline void set_endOfEvtUIcommands  ( const Strings& c ) { m_eE  = c ; } ;    ///
  /// UI commands to be executed at start of each Run                           ///
  inline void set_startOfRunUIcommands( const Strings& c ) { m_sR  = c ; } ;    ///
  /// UI commands to be executed at end   of each Run                           ///
  inline void set_endOfRunUIcommands  ( const Strings& c ) { m_eR  = c ; } ;    ///
  /// UI commands to be executed at end                                         ///
  inline void set_endUIcommands       ( const Strings& c ) { m_e   = c ; } ;    ///
  /////////////////////////////////////////////////////////////////////////////////

 private: 

  /////////////////////////////////////////////////////////////////////////////////
  inline void set_krn_Is_Initialized ( bool st ) { m_krn_st = st ; }            ///
  inline void set_run_Is_Initialized ( bool st ) { m_run_st = st ; }            ///
  inline void set_evt_Is_Prepared    ( bool st ) { m_pre_st = st ; }            ///
  inline void set_evt_Is_Processed   ( bool st ) { m_pro_st = st ; }            ///
  inline void set_vis_Is_Initialized ( bool st ) { m_vis_st = st ; }            ///
  inline void set_uis_Is_Started     ( bool st ) { m_uis_st = st ; }            ///
  /////////////////////////////////////////////////////////////////////////////////
  
  /////////////////////////////////////////////////////////////////////////////////
  inline void Assert( bool  assertion                              ,            ///
                      const std::string& msg = "GiGaSvc::unknown"  ,            ///                  
                      const StatusCode&  sc = StatusCode::FAILURE  ) ;          ///
  /////////////////////////////////////////////////////////////////////////////////
  
 private: 

  /////////////////////////////////////////////////////////////////////////////////
  ISvcLocator*               m_svcLoc       ;                                   ///
  /////////////////////////////////////////////////////////////////////////////////
  G4VPhysicalVolume*         m_rootGeo      ;                                   ///
  /////////////////////////////////////////////////////////////////////////////////
  mutable IGiGaGeomCnvSvc*   m_cnvSvc       ;                                   ///
  /////////////////////////////////////////////////////////////////////////////////
  G4UIsession*               m_g4UIsession  ;                                   ///
  /////////////////////////////////////////////////////////////////////////////////
  G4VisManager*              m_g4VisManager ;                                   ///
  /////////////////////////////////////////////////////////////////////////////////
  /////////////////////////////////////////////////////////////////////////////////
  bool                       m_krn_st ;                                         ///  
  bool                       m_run_st ;                                         /// 
  bool                       m_pre_st ;                                         ///
  bool                       m_pro_st ;                                         /// 
  bool                       m_vis_st ;                                         ///
  bool                       m_uis_st ;                                         /// 
  /////////////////////////////////////////////////////////////////////////////////
  std::string                m_name       ; 
  IMessageSvc*               m_msgSvc     ; 
  IChronoStatSvc*            m_chronoSvc  ;
  /////////////////////////////////////////////////////////////////////////////////
  Strings                    m_s          ;                                     ///
  Strings                    m_sE         ;                                     ///
  Strings                    m_eE         ;                                     ///
  Strings                    m_sR         ;                                     ///
  Strings                    m_eR         ;                                     ///
  Strings                    m_e          ;                                     ///
  /////////////////////////////////////////////////////////////////////////////////
  Strings                    m_UIsessions ;                                     ///
  /////////////////////////////////////////////////////////////////////////////////

}; 
///////////////////////////////////////////////////////////////////////////////////
inline void GiGaRunManager::Assert( bool  assertion        , 
                                    const std::string& msg , 
                                    const StatusCode &  sc ) 
{ if( !assertion ) { throw GiGaException("GiGaRunManager::"+msg , sc ) ; } };
///////////////////////////////////////////////////////////////////////////////////
inline IGiGaGeomCnvSvc*   GiGaRunManager::cnvSvc    () const 
{
  /// service is not located 
  if( 0 != m_cnvSvc ) { return m_cnvSvc; }                                 /// RETURN!!
  /// locate service
  {
    IService* iS = 0 ; 
    const std::string tmp("GiGaGeomCnvSvc");
    StatusCode sc = svcLoc()->getService( tmp , iS ) ;
    if( sc.isSuccess() && 0 != iS ) { m_cnvSvc = dynamic_cast<IGiGaGeomCnvSvc*> ( iS ); }  
    if( 0 != m_cnvSvc ) { m_cnvSvc->addRef() ;  return m_cnvSvc; }         /// RETURN   
    ///
    MsgStream log( msgSvc() , name() ) ; 
    log << MSG::ERROR << " Couldn't locate ConversionService="+tmp << endreq; 
  } 
  ///
  return 0;
};
///////////////////////////////////////////////////////////////////////////////////



#endif  //  GIGA_GIGARUNMANAGER_H






