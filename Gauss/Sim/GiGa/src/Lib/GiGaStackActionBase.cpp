// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGa/src/Lib/GiGaStackActionBase.cpp,v 1.1 2001-04-23 08:52:03 ibelyaev Exp $ 

// GiGa 
#include "GiGa/GiGaStackActionBase.h"


////////////////////////////////////////////////////////////////////////////////////////////////
GiGaStackActionBase::GiGaStackActionBase( const std::string& nick , ISvcLocator* svc )
  : G4UserStackingAction () 
  , GiGaBase             ( nick , svc ) 
{};
/// virtual de//////structor ///////////////////////////////////////////////////////////////////
GiGaStackActionBase::~GiGaStackActionBase(){};
////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaStackActionBase::initialize() 
{
  StatusCode sc = GiGaBase::initialize() ; 
  if( sc.isFailure() ) { return Error("Could not initialize Base class!"); } 
  ///
  return StatusCode::SUCCESS;
}; 
////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaStackActionBase::finalize() { return GiGaBase::finalize();  };
///////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaStackActionBase::queryInterface( const InterfaceID& iid , void** ppI)
{
  if( 0 == ppI ) { return StatusCode::FAILURE; } 
  *ppI = 0; 
  if   ( IGiGaStackAction::interfaceID() == iid ) 
    { *ppI = static_cast<IGiGaStackAction*>        ( this ) ; } 
  else                                            
    { return GiGaBase::queryInterface( iid , ppI ) ; } /// RETURN!!!
  addRef();
  return StatusCode::SUCCESS;
};
////////////////////////////////////////////////////////////////////////////////////////////////
G4ClassificationOfNewTrack GiGaStackActionBase::ClassifyNewTrack ( const G4Track* /* track */ )
{ return fUrgent; }
////////////////////////////////////////////////////////////////////////////////////////////////
void GiGaStackActionBase::NewStage         (){};
////////////////////////////////////////////////////////////////////////////////////////////////
void GiGaStackActionBase::PrepareNewEvent  (){};
////////////////////////////////////////////////////////////////////////////////////////////////














