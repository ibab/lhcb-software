// $Id: GiGaTrackActionBase.cpp,v 1.8 2002-12-07 14:27:51 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.7  2002/05/07 12:21:34  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
// GiGa
#include "GiGa/GiGaTrackActionBase.h"
#include "GiGa/GiGaUtil.h"

/** @file 
 *  implementation of class GiGaTrackActionBAse 
 * 
 *  @author Vanya Belyaev 
 */

namespace GiGaTrackActionBaseLocal
{
#ifdef GIGA_DEBUG
  /** @var   s_Counter
   *  static instance counter 
   */
  static GiGaUtil::InstanceCounter<GiGaTrackActionBase> s_Counter ;
#endif   
};

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
{ 
  declareInterface<IGiGaTrackAction> (this); 
#ifdef GIGA_DEBUG
  GiGaTrackActionBaseLocal::s_Counter.increment () ;
#endif 
}
// ============================================================================

// ============================================================================
// destructor
// ============================================================================
GiGaTrackActionBase::~GiGaTrackActionBase()
{
#ifdef GIGA_DEBUG
  GiGaTrackActionBaseLocal::s_Counter.decrement () ;
#endif 
};
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










