/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.4  2001/07/23 13:12:12  ibelyaev
/// the package restructurisation(II)
/// 
/// ===========================================================================
// GiGa 
#include "GiGa/GiGaStackActionBase.h"

/** implementation of class GiGaStackActionBase
 *
 * @author Vanya Belyaev 
 */

/// ===========================================================================
/// ===========================================================================
GiGaStackActionBase::GiGaStackActionBase( const std::string& nick , 
                                          ISvcLocator* svc )
  : G4UserStackingAction () 
  , GiGaBase             ( nick , svc ) 
{};

/// ===========================================================================
/// ===========================================================================
GiGaStackActionBase::~GiGaStackActionBase(){};

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaStackActionBase::initialize() 
{
  StatusCode sc = GiGaBase::initialize() ; 
  if( sc.isFailure() ) { return Error("Could not initialize Base class!"); } 
  ///
  Print("GiGaStackActionBase initialized successfully" ,
        StatusCode::SUCCESS , MSG::DEBUG ) ;
  ///
  return StatusCode::SUCCESS;
}; 

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaStackActionBase::finalize() 
{ 
  ///
  Print("GiGaStackActionBase finalization" ,
        StatusCode::SUCCESS , MSG::DEBUG ) ;
  ///
  return GiGaBase::finalize();  
};

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaStackActionBase::queryInterface( const InterfaceID& iid , 
                                                void** ppI)
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

/// ===========================================================================
/// ===========================================================================
G4ClassificationOfNewTrack 
GiGaStackActionBase::ClassifyNewTrack ( const G4Track* /* track */ )
{ return fUrgent; }

/// ===========================================================================
/// ===========================================================================
void GiGaStackActionBase::NewStage         (){};

/// ===========================================================================
/// ===========================================================================
void GiGaStackActionBase::PrepareNewEvent  (){};

/// ===========================================================================














