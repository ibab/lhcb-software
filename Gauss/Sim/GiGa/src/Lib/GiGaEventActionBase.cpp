// $Id: GiGaEventActionBase.cpp,v 1.8 2002-04-25 13:02:04 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// GiGa 
#include "GiGa/GiGaEventActionBase.h"

/** @file 
 *  implementation fo class GiGaEventActionBase
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */

// ============================================================================
// ============================================================================
GiGaEventActionBase::GiGaEventActionBase( const std::string& nick , 
                                          ISvcLocator* svc )
  : GiGaBase          ( nick , svc ) 
{};

// ============================================================================
/// virtual destructor
// ============================================================================
GiGaEventActionBase::~GiGaEventActionBase(){};

// ============================================================================
// ============================================================================
StatusCode GiGaEventActionBase::initialize() 
{
  StatusCode sc = GiGaBase::initialize() ; 
  if( sc.isFailure() ) { return Error("Could not initialize Base class!"); } 
  ///
  Print("GiGaEventActionBase initialized successfully" ,
        StatusCode::SUCCESS , MSG::DEBUG ) ;
  ///
  return StatusCode::SUCCESS;
}; 

// ============================================================================
// ============================================================================
StatusCode GiGaEventActionBase::finalize() 
{ 
  ///
  Print("GiGaEventActionBase finalization" ,
        StatusCode::SUCCESS , MSG::DEBUG ) ;
  ///
  return GiGaBase::finalize();  
  ///
};

// ============================================================================
// ============================================================================
StatusCode GiGaEventActionBase::queryInterface( const InterfaceID& iid , 
                                                void** ppI)
{
  if( 0 == ppI ) { return StatusCode::FAILURE; } 
  *ppI = 0; 
  if   ( IGiGaEventAction::interfaceID() == iid ) 
    { *ppI = static_cast<IGiGaEventAction*>        ( this ) ; } 
  else { return GiGaBase::queryInterface( iid , ppI ) ; } /// RETURN!!!
  addRef();
  return StatusCode::SUCCESS;
};

// ============================================================================
// The End 
// ============================================================================












