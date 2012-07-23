// $Id: GiGaIGiGaSvc.cpp,v 1.4 2009-10-14 13:50:02 gcorti Exp $ 
// Include files 
// STD & STL 
#include <string>
#include <list>
#include <vector> 
#include <algorithm>

// GaudiKernel
#include    "GaudiKernel/ISvcLocator.h"
#include    "GaudiKernel/IMessageSvc.h"
#include    "GaudiKernel/IChronoStatSvc.h"
#include    "GaudiKernel/SvcFactory.h"
#include    "GaudiKernel/MsgStream.h"

// LHCb
#include    "Kernel/ParticleProperty.h"

// GiGa 
#include    "GiGa/GiGaException.h"
#include    "GiGa/GiGaHitsByID.h"
#include    "GiGa/GiGaHitsByName.h"
// local 
#include    "GiGa.h"

// G4 
#include    "G4Event.hh"
#include    "G4VHitsCollection.hh"
#include    "G4HCofThisEvent.hh"
#include    "G4SDManager.hh"

/**  @file 
 *   Implementation of class GiGa  
 *   all methods from abstract interface IGiGaSvc 
 *
 *   @author: Vanya Belyaev 
 *   @date xx/xx/xxxx
 */

// ============================================================================
/** add  primary vertex into GiGa/G4 
 *                  implementation of IGiGaSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown
 * 
 *  @param  vertex pointer to primary vertex 
 *  @return  self-reference ot IGiGaSvc interface 
 */
// ============================================================================
IGiGaSvc&   GiGa::operator <<         ( G4PrimaryVertex * vertex   )
{
  ///
  StatusCode sc = prepareTheEvent( vertex ) ; 
  Assert( sc.isSuccess(), " operator<<(G4PrimaryVertex*) " , sc );   
  ///
  return *this; 
  ///
}; 
// ============================================================================

// ============================================================================
/** get the whole event  object from GiGa/G4 
 *                  implementation of IGiGaSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown
 * 
 *  @param   event pointer to whole event  
 *  @return  self-reference ot IGiGaSvc interface 
 */
// ============================================================================
IGiGaSvc& GiGa::operator >> ( const G4Event*         & event        )    
{
  ///
  StatusCode sc = retrieveTheEvent( event ) ; 
  Assert( sc.isSuccess(), "operator>>(G4Event*)" , sc );   
  return *this;
}; 
// ============================================================================

// ============================================================================
/** get the all hit collections from GiGa/G4 
 *                  implementation of IGiGaSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown
 * 
 *  @param   collections  pointer to all hit collections  
 *  @return  self-reference ot IGiGaSvc interface 
 */
// ============================================================================
IGiGaSvc& GiGa::operator >> ( G4HCofThisEvent*       & collections  )
{
  ///
  const G4Event* event = 0 ; 
  *this >> event           ; 
  collections = ( 0 != event )  ? event->GetHCofThisEvent() : 0 ; 
  ///
  return *this;   
};
// ============================================================================

// ============================================================================
/** get the concrete hit collection from GiGa/G4 
 *                  implementation of IGiGaSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown
 * 
 *  @param   collection  reference to collection pair   
 *  @return  self-reference ot IGiGaSvc interface 
 */
// ============================================================================
IGiGaSvc& GiGa::operator >> ( GiGaHitsByID& collection   )
{
  // reset the output 
  collection.setHits( (G4VHitsCollection*) 0  ) ;
  if( collection.id() < 0 ) 
    { Warning("Illegal (negative) collection ID"); return *this ; } // RETURN
  G4HCofThisEvent* collections = 0 ; 
  *this >> collections             ; 
  if( 0 == collections ) 
    { collection.setHits( (G4VHitsCollection*) 0                ) ; }
  else 
    { collection.setHits( collections->GetHC( collection.id() ) ) ; }
  ///
  return *this ;  
}; 
// ============================================================================

// ============================================================================
/** get the concrete hit collection from GiGa/G4 
 *                  implementation of IGiGaSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown
 * 
 *  @param   collection  reference to collection pair   
 *  @return  self-reference ot IGiGaSvc interface 
 */
// ============================================================================
IGiGaSvc& GiGa::operator >> ( GiGaHitsByName & collection   )
{
  // reset the output 
  collection.setHits( (G4VHitsCollection*) 0  ) ;
  // get the unique collection ID from collection name
  G4SDManager* sdm = G4SDManager::GetSDMpointer () ;
  Assert( 0 != sdm , " G4SDManager* poits to NULL ");
  const int colID = sdm -> GetCollectionID( collection.name() );
  if( colID < 0 ) { Warning("Illegal (negative) ID for collection '" + 
                            collection.name()+"'"); }
  GiGaHitsByID col( colID );
  *this >> col ;
  collection.setHits( col.hits() )  ;
  ///
  return *this ;  
}; 
// ============================================================================

// ============================================================================
/** get all trajectories(trajectory container) from GiGa/G4 
 *                  implementation of IGiGaSvc abstract interface 
 *
 *  NB: errors are reported through exception thrown
 * 
 *  @param   trajectories  pointer to trajectory conatiner   
 *  @return  self-reference ot IGiGaSvc interface 
 */
// ============================================================================
IGiGaSvc& GiGa::operator >> ( G4TrajectoryContainer* & trajectories )
{
  ///
  const G4Event* event = 0 ; 
  *this >> event ;
  trajectories = ( 0 != event ) ? event->GetTrajectoryContainer() : 0 ; 
  /// 
  return *this ; 
};
// ============================================================================

// ============================================================================
/** add  primary vertex into GiGa/G4  
 *                  implementation of IGiGaSvc abstract interface 
 *
 *  @param  vertex pointer to primary vertex 
 *  @return status code  
 */
// ============================================================================
StatusCode  GiGa::addPrimaryKinematics( G4PrimaryVertex  * vertex   ) 
{
  ///
  StatusCode sc(StatusCode::FAILURE);
  ///
  ___GIGA_TRY___
    { *this << vertex           ; } 
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),std::string("addVtx"),msgSvc(),chronoSvc(),sc ) ; 
  ///
  return StatusCode::SUCCESS;  
};
// ============================================================================

// ============================================================================
/** get the whole event  object from GiGa/G4 
 *                  implementation of IGiGaSvc abstract interface 
 *
 *  @param  event pointer to whole event 
 *  @return status code  
 */
// ============================================================================
StatusCode GiGa::retrieveEvent  ( const G4Event*          & event )
{ 
  ///
  StatusCode sc( StatusCode::FAILURE ); 
  ///
  ___GIGA_TRY___
    { *this >> event            ; } 
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),std::string("getEvt"),msgSvc(),chronoSvc(),sc ) ; 
  ///
  return StatusCode::SUCCESS ;  
};
// ============================================================================

// ============================================================================
/** get the all hit collections from GiGa/G4 
 *                  implementation of IGiGaSvc abstract interface 
 * 
 *  @param   collections  pointer to all hit collections  
 *  @return  status code  
 */
// ============================================================================
StatusCode GiGa::retrieveHitCollections  ( G4HCofThisEvent* & collections  )
{
  ///
  StatusCode sc( StatusCode::FAILURE ); 
  ///
  ___GIGA_TRY___
    { *this >> collections      ; } 
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),std::string("getHits"),msgSvc(),chronoSvc(),sc); 
  ///
  return StatusCode::SUCCESS; 
};
// ============================================================================

// ============================================================================
/** get the concrete hit collection from GiGa/G4 
 *                  implementation of IGiGaSvc abstract interface 
 *
 *  @param   collection  reference to collection pair   
 *  @return  status code 
 */
// ============================================================================
StatusCode GiGa::retrieveHitCollection  ( GiGaHitsByID & collection   )
{
  ///
  StatusCode sc( StatusCode::FAILURE ); 
  ///
  ___GIGA_TRY___
    { *this >> collection       ; } 
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),std::string("getHits"),msgSvc(),chronoSvc(),sc); 
  ///
  return StatusCode::SUCCESS; 
};
// ============================================================================

// ============================================================================
/** get the concrete hit collection from GiGa/G4 
 *                  implementation of IGiGaSvc abstract interface 
 *
 *  @param   collection  reference to collection pair   
 *  @return  status code 
 */
// ============================================================================
StatusCode GiGa::retrieveHitCollection  ( GiGaHitsByName & collection   )
{
  ///
  StatusCode sc( StatusCode::FAILURE ); 
  ///
  ___GIGA_TRY___
    { *this >> collection       ; } 
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),std::string("getHits"),msgSvc(),chronoSvc(),sc); 
  ///
  return StatusCode::SUCCESS; 
};
// ============================================================================

// ============================================================================
/** get all trajectories(trajectory container) from GiGa/G4 
 *                  implementation of IGiGaSvc abstract interface 
 *
 *  NB: errors are reported throw exception
 * 
 *  @param   trajectories  pointer to trajectory conatiner   
 *  @return  self-reference ot IGiGaSvc interface 
 */
// ============================================================================
StatusCode GiGa::retrieveTrajectories( G4TrajectoryContainer*& trajectories)
{
  ///
  StatusCode sc( StatusCode::FAILURE ); 
  ///
  ___GIGA_TRY___
    { *this >> trajectories     ; } 
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),std::string("getTrjs"),msgSvc(),chronoSvc(),sc); 
  ///
  return StatusCode::SUCCESS; 
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
















