// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/src/component/GiGaSvcIGiGaSvc.cpp,v 1.2 2001-04-23 19:24:24 ibelyaev Exp $ 
///
#include <string>
#include <list>
#include <vector> 
#include <algorithm> 
///
#include    "GaudiKernel/ISvcLocator.h"
#include    "GaudiKernel/IMessageSvc.h"
#include    "GaudiKernel/IChronoStatSvc.h"
#include    "GaudiKernel/IObjManager.h"
#include    "GaudiKernel/SvcFactory.h"
#include    "GaudiKernel/MsgStream.h"
#include    "GaudiKernel/ParticleProperty.h"
///
#include    "GiGa/GiGaException.h"

// local 
#include    "GiGaSvc.h"
#include    "GiGaRunManager.h" 

///////////////////////////////////////////////////////////////////////////////////
IGiGaSvc&   GiGaSvc::operator <<         ( G4PrimaryVertex * vertex   )
{
  ///
  StatusCode sc = prepareTheEvent( vertex ) ; 
  Assert( sc.isSuccess(), " operator<<(G4PrimaryVertex*) " , sc );   
  ///
  return *this; 
  ///
}; 
///////////////////////////////////////////////////////////////////////////////////
StatusCode  GiGaSvc::addPrimaryKinematics( G4PrimaryVertex  * vertex   ) 
{
  ///
  StatusCode sc(StatusCode::FAILURE);
  ///
  ___GIGA_TRY___
    { *this << vertex           ; } 
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),"addPrimaryKinematics",msgSvc(),chronoSvc(),sc ) ; 
  ///
  return StatusCode::SUCCESS;  
};
///////////////////////////////////////////////////////////////////////////////////
IGiGaSvc& GiGaSvc::operator >> ( const G4Event*         & event        )    
{
  ///
  StatusCode sc = retrieveTheEvent( event ) ; 
  Assert( sc.isSuccess(), "operator>>(G4Event*)" , sc );   
  return *this;
}; 
///////////////////////////////////////////////////////////////////////////////////
IGiGaSvc& GiGaSvc::operator >> ( G4HCofThisEvent*       & collections  )
{
  ///
  const G4Event* event = 0 ; 
  *this >> event           ; 
  collections = 
    ( 0 != event )  ? event->GetHCofThisEvent() : 0 ; 
  ///
  return *this;   
};
///////////////////////////////////////////////////////////////////////////////////
IGiGaSvc& GiGaSvc::operator >> ( CollectionPair         & collection   )
{
  G4HCofThisEvent* collections = 0 ; 
  *this >> collections             ; 
  collection.second = 
    ( 0 != collections)  ? collections->GetHC( collection.first ) : 0 ; 
  ///
  return *this ;  
}; 
///////////////////////////////////////////////////////////////////////////////////
IGiGaSvc& GiGaSvc::operator >> ( G4TrajectoryContainer* & trajectories )
{
  ///
  const G4Event* event = 0 ; 
  *this >> event ;
  trajectories = 
    ( 0 != event ) ? event->GetTrajectoryContainer() : 0 ; 
  /// 
  return *this ; 
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSvc::retrieveEvent  ( const G4Event*          & event )
{ 
  ///
  StatusCode sc( StatusCode::FAILURE ); 
  ///
  ___GIGA_TRY___
    { *this >> event            ; } 
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),"retrieveEvent",msgSvc(),chronoSvc(),sc ) ; 
  ///
  return StatusCode::SUCCESS ;  
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSvc::retrieveHitCollections  ( G4HCofThisEvent*        & collections  )
{
  ///
  StatusCode sc( StatusCode::FAILURE ); 
  ///
  ___GIGA_TRY___
    { *this >> collections      ; } 
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),"retrieveHitCollections",msgSvc(),chronoSvc(),sc ) ; 
  ///
  return StatusCode::SUCCESS; 
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSvc::retrieveHitCollection  ( CollectionPair          & collection   )
{
  ///
  StatusCode sc( StatusCode::FAILURE ); 
  ///
  ___GIGA_TRY___
    { *this >> collection       ; } 
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),"retrieveHitCollection",msgSvc(),chronoSvc(),sc ) ; 
  ///
  return StatusCode::SUCCESS; 
};
///////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaSvc::retrieveTrajectories  ( G4TrajectoryContainer*  & trajectories )
{
  ///
  StatusCode sc( StatusCode::FAILURE ); 
  ///
  ___GIGA_TRY___
    { *this >> trajectories     ; } 
  ___GIGA_CATCH_PRINT_AND_RETURN___(name(),"retrieveTrajectories",msgSvc(),chronoSvc(),sc ) ; 
  ///
  return StatusCode::SUCCESS; 
};
///////////////////////////////////////////////////////////////////////////////////
















