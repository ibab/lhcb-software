// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.4  2001/08/12 15:42:46  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.3  2001/08/01 09:42:23  ibelyaev
/// redesign and reimplementation of GiGaRunManager class
///
/// Revision 1.2  2001/07/23 13:11:45  ibelyaev
/// the package restructurisation(II)
///
/// Revision 1.1  2001/07/15 20:53:57  ibelyaev
/// package restructurisation
///
// ============================================================================
#ifndef GIGA_IGIGASETUPSVC_H
#define GIGA_IGIGASETUPSVC_H
// ============================================================================
// Include files
#include  "GaudiKernel/Kernel.h"
#include  "GaudiKernel/StatusCode.h"
#include  "GaudiKernel/IService.h"
/// GiGa 
/// Forward declarations (from Geant4)
class       G4VUserDetectorConstruction   ;
class       G4VUserPrimaryGeneratorAction ;
class       G4VUserPhysicsList            ;
class       G4UserRunAction               ;
class       G4UserEventAction             ;
class       G4UserStackingAction          ;
class       G4UserSteppingAction          ; 
class       G4UserTrackingAction          ; 
class       G4VisManager                  ; 
class       G4VPhysicalVolume             ; 

/** @class IGiGaSetUpSvc IGiGaSetUpSvc.h GiGa/IGiGaSetUpSvc.h
 *
 *  definition of abstract interface to Geant4 Service for 
 *  configuration of Geant4 
 *
 *  @author Vanya Belyaev
*/

class IGiGaSetUpSvc : virtual public IService
{
public:
  
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() ;
  
  /** initialize method 
   *  @return status code 
   */
  virtual StatusCode                    initialize() = 0 ;

  /** finalize  method 
   *  @return status code 
   */
  virtual StatusCode                    finalize  () = 0 ;
  
public:   
  
  /** set detector constructon module 
   *
   *  @param  detector pointer to detector construction module  
   *  @return self-reference ot IGiGaSetUpSvc interface 
   */
  virtual IGiGaSetUpSvc& operator << ( G4VUserDetectorConstruction   * ) = 0 ;

  
  /** set new world wolume 
   * 
   *  @param  world  pointer to  new world volume   
   *  @return self-reference ot IGiGaSetUpSvc interface 
   */
  virtual IGiGaSetUpSvc& operator << ( G4VPhysicalVolume             * ) = 0 ;
 
  /** set new generator 
   *
   *  @param  generator  pointer to new generator   
   *  @return self-reference ot IGiGaSetUpSvc interface 
   */
  virtual IGiGaSetUpSvc& operator << ( G4VUserPrimaryGeneratorAction * ) = 0 ;
  
  /** set new physics list 
   * 
   *  @param  physics  pointer to physics list    
   *  @return self-reference ot IGiGaSetUpSvc interface 
   */
  virtual IGiGaSetUpSvc& operator << ( G4VUserPhysicsList            * ) = 0 ;
 
  /** set new run action 
   * 
   *  @param  action  pointer to new run action     
   *  @return self-reference ot IGiGaSetUpSvc interface 
   */
  virtual IGiGaSetUpSvc& operator << ( G4UserRunAction               * ) = 0 ;
  
  /** set new event action 
   * 
   *  @param  action  pointer to new event action     
   *  @return self-reference ot IGiGaSetUpSvc interface 
   */
  virtual IGiGaSetUpSvc& operator << ( G4UserEventAction             * ) = 0 ;

  /** set new stacking action 
   * 
   *  @param  action  pointer to new stacking action     
   *  @return self-reference ot IGiGaSetUpSvc interface 
   */
  virtual IGiGaSetUpSvc& operator << ( G4UserStackingAction          * ) = 0 ;

  /** set new tracking  action 
   * 
   *  @param  action  pointer to new tracking action     
   *  @return self-reference ot IGiGaSetUpSvc interface 
   */
  virtual IGiGaSetUpSvc& operator << ( G4UserTrackingAction          * ) = 0 ; 
  
  /** set new stepping  action 
   * 
   *  @param  action  pointer to new stepping action     
   *  @return self-reference ot IGiGaSetUpSvc interface 
   */
  virtual IGiGaSetUpSvc& operator << ( G4UserSteppingAction          * ) = 0 ;

  /** set detector constructon module 
   *
   *  @param  detector pointer to detector construction module  
   *  @return self-reference ot IGiGaSetUpSvc interface 
   */
  virtual StatusCode setConstruction ( G4VUserDetectorConstruction   * ) = 0 ;
  
  /** set new world wolume 
   *
   *  @param  world  pointer to  new world volume   
   *  @return status code  
   */
  virtual StatusCode setDetector     ( G4VPhysicalVolume             * ) = 0 ; 
  
  /** set new generator 
   *
   *  @param  generator  pointer to new generator   
   *  @return status code  
   */
  virtual StatusCode setGenerator    ( G4VUserPrimaryGeneratorAction * ) = 0 ;
  
  /** set new physics list 
   *
   *  @param  physics  pointer to physics list    
   *  @return status code  
   */
  virtual StatusCode setPhysics      ( G4VUserPhysicsList            * ) = 0 ;
  
  /** set new run action 
   *
   *  @param  action  pointer to new run action     
   *  @return status code  
   */
  virtual StatusCode setRunAction    ( G4UserRunAction               * ) = 0 ;
  
  /** set new event action 
   *
   *  @param  action  pointer to new event action     
   *  @return status code  
   */
  virtual StatusCode setEvtAction    ( G4UserEventAction             * ) = 0 ;
  
  /** set new stacking action 
   *
   *  @param  action  pointer to new stacking action     
   *  @return status code  
   */
  virtual StatusCode setStacking     ( G4UserStackingAction          * ) = 0 ;

  /** set new tracking  action 
   *
   *  @param  action  pointer to new tracking action     
   *  @return status code  
   */
  virtual StatusCode setTracking     ( G4UserTrackingAction          * ) = 0 ;
  
  /** set new stepping  action 
   *
   *  @param  action  pointer to new stepping action     
   *  @return status code  
   */
  virtual StatusCode setStepping     ( G4UserSteppingAction          * ) = 0 ;

protected:
  
  /// virtual destructor
  virtual ~IGiGaSetUpSvc() ;
  

};

// ============================================================================
#endif ///< GIGA_IGIGASETUPSVC_H
// ============================================================================
