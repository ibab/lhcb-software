// $Id: GiGaEventActionBase.cpp,v 1.9 2002-05-07 12:21:33 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2002/04/25 13:02:04  ibelyaev
//  small update
//
// ============================================================================
// GiGa 
#include "GiGa/GiGaEventActionBase.h"

// ============================================================================
/** @file 
 *  implementation fo class GiGaEventActionBase
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 6 May 2002 
 */
// ============================================================================

// ============================================================================
/** standard  constructor
 *  @see GiGaBase
 *  @see AlgTool
 *  @param type tool type (?)
 *  @param name  tool name 
 *  @param parent 
 */
// ============================================================================
GiGaEventActionBase::GiGaEventActionBase
( const std::string& type , 
  const std::string& nick , 
  const IInterface*  svc  )
  : GiGaBase          ( type , nick , svc ) 
{ declareInterface<IGiGaEventAction> (this); };
// ============================================================================

// ============================================================================
// virtual destructor
// ============================================================================
GiGaEventActionBase::~GiGaEventActionBase(){};
// ============================================================================

// ============================================================================
/** initialize the object 
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaEventActionBase::initialize ()  
{
  /// initialize base class
  StatusCode sc = GiGaBase::initialize (); 
  if( sc.isFailure() )
    { return Error("Could not initialize GiGaBase class!", sc); }
  ///
  return Print("GiGaEventActionBase initialized succesfully" ,
               StatusCode::SUCCESS                           , MSG::VERBOSE );
};
// ============================================================================

// ============================================================================
/** finalize initialize the object 
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaEventActionBase::finalize   ()  
{ 
  Print("GiGaEventActionBase Finalization" ,
        StatusCode::SUCCESS                , MSG::VERBOSE );
  // finalze the base class 
  return GiGaBase::finalize   (); 
};
// ============================================================================

// ============================================================================
// The End 
// ============================================================================












