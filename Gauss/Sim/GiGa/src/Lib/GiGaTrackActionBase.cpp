/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
// GiGa
#include "GiGa/GiGaTrackActionBase.h"

/** implementation of class GiGaTrackActionBAse 
 * 
 *  @author Vanya Belyaev 
 */

/// ===========================================================================
/// ===========================================================================
GiGaTrackActionBase::GiGaTrackActionBase( const std::string& nick ,
                                          ISvcLocator* svc )
  : G4UserTrackingAction() 
  , GiGaBase            ( nick     ,   svc     ) 
{ };

/// ===========================================================================
/// ===========================================================================
GiGaTrackActionBase::~GiGaTrackActionBase(){};

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaTrackActionBase::initialize() 
{
  StatusCode sc = GiGaBase::initialize() ; 
  if( sc.isFailure() ) { return Error("Could not initialize Base class!"); } 
  ///
  return StatusCode::SUCCESS;
}; 

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaTrackActionBase::finalize() { return GiGaBase::finalize();  };

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaTrackActionBase::queryInterface( const InterfaceID& iid , 
                                                void** ppI)
{
  if( 0 == ppI ) { return StatusCode::FAILURE; } 
  *ppI = 0; 
  if   ( IGiGaTrackAction::interfaceID()  == iid ) 
    { *ppI = static_cast<IGiGaTrackAction*>        ( this ) ; } 
  else                                             
    { return GiGaBase::queryInterface( iid , ppI ) ; } /// RETURN!!!
  addRef();
  return StatusCode::SUCCESS;
};

/// ===========================================================================
/// ===========================================================================
void GiGaTrackActionBase::PreUserTrackingAction  ( const G4Track* /* track */ ) 
{};

/// ===========================================================================
/// ===========================================================================
void GiGaTrackActionBase::PostUserTrackingAction ( const G4Track* /* track */ ) 
{};

/// ===========================================================================










