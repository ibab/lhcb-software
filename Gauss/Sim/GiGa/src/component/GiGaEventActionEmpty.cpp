// $Id: GiGaEventActionEmpty.cpp,v 1.7 2002-04-25 13:02:04 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// GiGa 
#include "GiGa/GiGaEventActionFactory.h"
// local
#include "GiGaEventActionEmpty.h"

/** @file 
 *
 * implementation of class GiGaEventActionEmpty
 * 
 *  @author Vanya Belyaev 
 */

// ============================================================================
// Factory
// ============================================================================
IMPLEMENT_GiGaEventAction( GiGaEventActionEmpty );

// ============================================================================
// ============================================================================
GiGaEventActionEmpty::GiGaEventActionEmpty( const std::string& Name , 
                                            ISvcLocator* Loc ) 
  : GiGaEventActionBase( Name , Loc ) 
{};

// ============================================================================
// ============================================================================
GiGaEventActionEmpty::~GiGaEventActionEmpty(){};

// ============================================================================
// ============================================================================
StatusCode GiGaEventActionEmpty::initialize () 
{
  /// initialize the base class 
  StatusCode sc = GiGaEventActionBase::initialize() ; 
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class",sc); }
  ///
  Print("initialized succesfully");
  ///
  return StatusCode::SUCCESS;
} ;

// ============================================================================
// ============================================================================
StatusCode GiGaEventActionEmpty::finalize   () 
{ 
  Print("finalization");
  return GiGaEventActionBase::finalize() ; 
} ;

// ============================================================================
// ============================================================================
void GiGaEventActionEmpty::BeginOfEventAction ( const G4Event* /* event */ ) 
{ Print(" BeginOfEventAction() method is invoked"); };

// ============================================================================
// ============================================================================
void GiGaEventActionEmpty::EndOfEventAction   ( const G4Event* /* event */ ) 
{ Print(" EndOfEventAction() method is invoked"); };

// ============================================================================
// The END 
// ============================================================================
