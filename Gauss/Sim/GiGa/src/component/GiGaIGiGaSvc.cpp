// $Id: GiGaIGiGaSvc.cpp,v 1.1 2002-12-07 14:27:52 ibelyaev Exp $ 
// ================================`============================================
// CVS tag $Name: not supported by cvs2svn $ 
// ================================`============================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2002/05/01 18:23:39  ibelyaev
//  import errors/warnings/exception counterf from LHCb Calo software
//
// ============================================================================
#define GIGA_GIGASVCIGIGASVC_CPP 1 
// ============================================================================
/// includes
/// STD & STL 
#include <string>
#include <list>
#include <vector> 
#include <algorithm>
/// GaudiKernel
#include    "GaudiKernel/ISvcLocator.h"
#include    "GaudiKernel/IMessageSvc.h"
#include    "GaudiKernel/IChronoStatSvc.h"
#include    "GaudiKernel/IObjManager.h"
#include    "GaudiKernel/SvcFactory.h"
#include    "GaudiKernel/MsgStream.h"
#include    "GaudiKernel/ParticleProperty.h"
/// GiGa 
#include    "GiGa/GiGaException.h"
/// local 
#include    "GiGa.h"
// G4 
#include    "G4Event.hh"
#include    "G4VHitsCollection.hh"
#include    "G4HCofThisEvent.hh"
#include    "G4SDManager.hh"

// ============================================================================
/**  @file 
 *   Implementation of class GiGa  
 *   all methods from abstract interface IGiGaSvc 
 *
 *   @author: Vanya Belyaev 
 *   @date xx/xx/xxxx
 */
// ============================================================================

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
IGiGaSvc& GiGa::operator >> ( CollectionPair         & collection   )
{
  G4HCofThisEvent* collections = 0 ; 
  *this >> collections             ; 
  collection.second = 
    ( 0 != collections)  ? collections->GetHC( collection.first ) : 0 ; 
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
IGiGaSvc& GiGa::operator >> ( CollectionNamePair         & collection   )
{
  // get the unique collection ID from collection name 
  G4SDManager* sdm = G4SDManager::GetSDMpointer () ;
  Assert( 0 != sdm , " G4SDManager* poits to NULL ");
  const int colID = sdm -> GetCollectionID( collection.first );
  CollectionPair col( colID , (G4VHitsCollection *) 0 );
  *this >> col ;
  collection.second = col.second ;
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
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),"addVtx",msgSvc(),chronoSvc(),sc ) ; 
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
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),"getEvt",msgSvc(),chronoSvc(),sc ) ; 
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
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),"getHits",msgSvc(),chronoSvc(),sc); 
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
StatusCode GiGa::retrieveHitCollection  ( CollectionPair & collection   )
{
  ///
  StatusCode sc( StatusCode::FAILURE ); 
  ///
  ___GIGA_TRY___
    { *this >> collection       ; } 
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),"getHits",msgSvc(),chronoSvc(),sc); 
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
StatusCode GiGa::retrieveHitCollection  ( CollectionNamePair & collection   )
{
  ///
  StatusCode sc( StatusCode::FAILURE ); 
  ///
  ___GIGA_TRY___
    { *this >> collection       ; } 
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),"getHits",msgSvc(),chronoSvc(),sc); 
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
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),"getTrjs",msgSvc(),chronoSvc(),sc); 
  ///
  return StatusCode::SUCCESS; 
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
















