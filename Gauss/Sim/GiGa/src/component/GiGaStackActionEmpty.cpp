// $Id: GiGaStackActionEmpty.cpp,v 1.7 2002-04-25 13:02:05 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
//
#include "G4Step.hh"
//
#include "GaudiKernel/MsgStream.h"
// GiGa 
#include "GiGa/GiGaStackActionFactory.h"
// local
#include "GiGaStackActionEmpty.h"

/** @file 
 * 
 * Implementation fo class GiGaStackActionEmpty
 *  
 * @author Vanya Belyaev 
 */

// ============================================================================
// Factory
// ============================================================================
IMPLEMENT_GiGaStackAction( GiGaStackActionEmpty ) ;

// ============================================================================
// ============================================================================
GiGaStackActionEmpty::GiGaStackActionEmpty( const std::string& Name , 
                                            ISvcLocator* Loc ) 
  : GiGaStackActionBase( Name , Loc ) 
{};

// ============================================================================
// ============================================================================
GiGaStackActionEmpty::~GiGaStackActionEmpty(){};

// ============================================================================
// ============================================================================
StatusCode GiGaStackActionEmpty::initialize () 
{ 
  /// initialize the base class 
  StatusCode sc = GiGaStackActionBase::initialize() ; 
  if( sc.isFailure() )
    { return Error("Could not initialize the base class ",sc); }
  ///
  Print("initialized succesfully");
  ///
  return StatusCode::SUCCESS;
} ;

// ============================================================================
// ============================================================================
StatusCode GiGaStackActionEmpty::finalize   () 
{ 
  ///
  Print("finalization");
  ///
  return GiGaStackActionBase::initialize() ; 
} ;

// ============================================================================
// ============================================================================
G4ClassificationOfNewTrack 
GiGaStackActionEmpty::ClassifyNewTrack ( const G4Track* /* track */ )
{
  ///
  Print("ClassifyNewTrack method in invoked", 
        StatusCode::SUCCESS , MSG::DEBUG ) ;
  ///
  return fUrgent; 
};

// ============================================================================
// ============================================================================
void GiGaStackActionEmpty::NewStage         ()
{ Print("New Stage method in invoked"       , 
        StatusCode::SUCCESS , MSG::DEBUG ) ; }; 

// ============================================================================
// ============================================================================
void GiGaStackActionEmpty::PrepareNewEvent  ()
{  Print("PrepareNewEvent method is invoked" , 
         StatusCode::SUCCESS , MSG::DEBUG ) ; };

// ============================================================================
// The END 
// ============================================================================
