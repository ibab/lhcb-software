// $Id: GiGaTrackActionBase.cpp,v 1.7 2002-05-07 12:21:34 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// GiGa
#include "GiGa/GiGaTrackActionBase.h"

/** @file 
 *  implementation of class GiGaTrackActionBAse 
 * 
 *  @author Vanya Belyaev 
 */

// ============================================================================
/** standard constructor 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaTrackActionBase::GiGaTrackActionBase
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : GiGaBase( type , name , parent )
{ declareInterface<IGiGaTrackAction> (this); }
// ============================================================================

// ============================================================================
// destructor
// ============================================================================
GiGaTrackActionBase::~GiGaTrackActionBase(){};
// ============================================================================

// ============================================================================
/** initialize the track action  
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaTrackActionBase::initialize() 
{
  StatusCode sc = GiGaBase::initialize() ; 
  if( sc.isFailure() ) { return Error("Could not initialize Base class!"); } 
  ///
  return Print("GiGaTrackActionBase initialized successfully" ,
               StatusCode::SUCCESS                            , MSG::VERBOSE) ;
}; 
// ============================================================================

// ============================================================================
/** finalize the track action
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaTrackActionBase::finalize() 
{ 
  Print("GiGaTrackActionBase finalization" ,
        StatusCode::SUCCESS                , MSG::VERBOSE ) ;
  // finalize the base class 
  return GiGaBase::finalize();  
};

// ============================================================================
// ============================================================================
void GiGaTrackActionBase::PreUserTrackingAction  ( const G4Track* /* track */ ) 
{ Print("'PreAction' is invoked" , StatusCode::SUCCESS , MSG::DEBUG ); };

// ============================================================================
// ============================================================================
void GiGaTrackActionBase::PostUserTrackingAction ( const G4Track* /* track */ ) 
{ Print("'PostAction' is invoked" , StatusCode::SUCCESS , MSG::DEBUG ); };

// ============================================================================
// The END 
// ============================================================================










