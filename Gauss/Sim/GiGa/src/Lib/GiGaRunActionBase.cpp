// $Id: GiGaRunActionBase.cpp,v 1.6 2002-05-07 12:21:33 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
/// GiGa 
#include "GiGa/GiGaRunActionBase.h"

// ============================================================================
/** @file
 *
 *  Implementation file for class : GiGaRunActionBase
 * 
 *  @date 26/07/2001 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ============================================================================

// ============================================================================
/** standard constructor
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaRunActionBase::GiGaRunActionBase
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : GiGaBase( type , name , parent )
{ declareInterface<IGiGaRunAction> ( this ); };
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
GiGaRunActionBase::~GiGaRunActionBase() {};
// ============================================================================

// ============================================================================
/** initialization method 
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaRunActionBase::initialize  ()
{ 
  StatusCode sc = GiGaBase::initialize();   
  if( sc.isFailure() ) 
    { return Error("Could not initialize base class GiGaBase!",sc); }
  ///
  return Print("GiGaRunActionBase initialized successfully" ,
               StatusCode::SUCCESS                          , MSG::VERBOSE );
};
// ============================================================================

// ============================================================================
/** finalization method 
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaRunActionBase::finalize  ()
{
  Print("GiGaRunActionBase finalization" ,
        StatusCode::SUCCESS              , MSG::VERBOSE ) ;
  ///
  return GiGaBase::finalize(); 
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

