//  $ID:$ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2004/02/20 18:58:17  ibelyaev
//  update in IGiGaXXX and IIDIGiGaXXX
//
// Revision 1.3  2002/05/07 12:21:31  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// Revision 1.2  2001/08/12 15:42:46  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.1  2001/08/01 09:42:23  ibelyaev
// redesign and reimplementation of GiGaRunManager class
// 
// ============================================================================
#ifndef GIGA_IGIGARUNMANAGER_H 
#define GIGA_IGIGARUNMANAGER_H 1
// ============================================================================
// GiGa 
#include "GiGa/IGiGaInterface.h"
// forward declarations from GiGa 
class     IGiGaGeoSrc                   ;
// forward declarations form Geant4 
class     G4VUserPrimaryGeneratorAction ;
class     G4VUserDetectorConstruction   ;
class     G4VUserPhysicsList            ;
class     G4UserRunAction               ;
class     G4UserEventAction             ;
class     G4UserStackingAction          ;
class     G4UserSteppingAction          ;
class     G4UserTrackingAction          ;
class     G4VPhysicalVolume             ;
class     G4PrimaryVertex               ;
class     G4Event                       ;
class     G4UIsession                   ;

/** @class IGiGaRunManager IGiGaRunManager.h GiGa/IGiGaRunManager.h
 *  
 *  An abstract interface to GiGa Run Manager  
 *
 *  @author Ivan Belyaev
 *  @date   31/07/2001
 */

class IGiGaRunManager: virtual public IGiGaInterface
{  
  
public:
  
  /** retrieve the uniques interface ID
   *  @return uniqie interface ID 
   */
  static const InterfaceID& interfaceID () ;
  
  /** declare the Geant4 Primary Generator Action 
   *  @param obj pointer  to Geant4 Primary Generator Action 
   *  @return  status code 
   */
  virtual StatusCode declare( G4VUserPrimaryGeneratorAction  * obj )  = 0 ;
  
  /** declare the top level ("world") physical volume 
   *  @param obj pointer  to top level ("world") physical volume  
   *  @return  status code 
   */
  virtual StatusCode declare( G4VPhysicalVolume              * obj ) = 0 ;

  /** declare the Geant4 Detector Construction Action
   *  @param obj pointer  to Geant4 Detector Construction Action  
   *  @return  status code 
   */
  virtual StatusCode declare( G4VUserDetectorConstruction    * obj ) = 0 ;

  /** declare the Geant4 Physics List 
   *  @param obj pointer  to Geant4 Physics List  
   *  @return  status code 
   */
  virtual StatusCode declare( G4VUserPhysicsList             * obj ) = 0 ;

  /** declare the GiGa geometry source  
   *  @param obj pointer  to GiGa Geometry source   
   *  @return  status code 
   */
  virtual StatusCode declare( IGiGaGeoSrc                    * obj ) = 0 ;

  /** declare the Geant4 Run Action 
   *  @param obj pointer  to Geant4 Run action  
   *  @return  status code 
   */
  virtual StatusCode declare( G4UserRunAction                * obj ) = 0 ;

  /** declare the Geant4 Event Action 
   *  @param obj pointer  to Geant4 Event  
   *  @return  status code 
   */
  virtual StatusCode declare( G4UserEventAction              * obj ) = 0 ;

  /** declare the Geant4 Stacking Action 
   *  @param obj pointer  to Geant4 Stacking Action 
   *  @return  status code 
   */
  virtual StatusCode declare( G4UserStackingAction           * obj ) = 0 ;

  /** declare the Geant4 Stepping  Action 
   *  @param obj pointer  to Geant4 Stepping Action 
   *  @return  status code 
   */
  virtual StatusCode declare( G4UserSteppingAction           * obj ) = 0 ;

  /** declare the Geant4 Tracking Action 
   *  @param obj pointer  to Geant4 Tracking Action 
   *  @return  status code 
   */
  virtual StatusCode declare( G4UserTrackingAction           * obj ) = 0 ;

  /** Prepare the event 
   *  @param vertex pointer to (main) primary vertex 
   *  @return status code 
   */
  virtual StatusCode  prepareTheEvent ( G4PrimaryVertex    * vertex = 0 ) = 0 ; 

  /** Process the prepared event 
   *  @return status code 
   */
  virtual StatusCode  processTheEvent (                                 ) = 0 ; 

  /** Retrieve the processed event 
   *  @param  event pointer to processed event  
   *  @return status code 
   */
  virtual StatusCode  retrieveTheEvent( const G4Event      *& event     ) = 0 ;

  /** declare the Geant4 User Interface session 
   *  @param obj pointer  to Geant4 User Interface session  
   *  @return  status code 
   */
  virtual StatusCode declare( G4UIsession                    * obj     ) = 0 ;
  
  
  /** get the instance of IGiGaRunManager  
   *  @return pointer to IGiGaRinManager 
   */
  static IGiGaRunManager* instance () ;
  
  /** destroy the static instance 
   *  @treturn status code 
   */
  static StatusCode       destroy  () ;
  
protected:
  
  /// destructor 
  virtual ~IGiGaRunManager() ; 
  
protected: 
  
  /** set the istatic instance of IGiGaRunManager
   *  @param value pointer to the static instance of IGiGaRunManager 
   */
  static StatusCode setInstance( IGiGaRunManager* value );
  
private:
  
  static IGiGaRunManager* s_manager ;
  
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif ///< GIGA_IGIGARUNMANAGER_H
// ============================================================================
