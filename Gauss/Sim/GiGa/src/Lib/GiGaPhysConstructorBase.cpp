// ============================================================================
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
/// GiGa
#include "GiGa/GiGaPhysConstructorBase.h"

// ============================================================================
/** @file 
 *  implementation of class GiGaPhysConstructorBase
 *
 *  @author Witek Pokorski   Witold.Pokorski@cern.ch 
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
GiGaPhysConstructorBase::GiGaPhysConstructorBase
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : GiGaBase          ( type , name , parent  )
{ 
  declareInterface<IGiGaPhysConstructor> (this);
};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
GiGaPhysConstructorBase::~GiGaPhysConstructorBase(){};
// ============================================================================

// ============================================================================
/** initialize the object 
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaPhysConstructorBase::initialize ()  
{
  /// initialize base class
  StatusCode sc = GiGaBase::initialize (); 
  if( sc.isFailure() )
    { return Error("Could not initialize GiGaBase class!", sc); }
  MsgStream log( msgSvc() , name() ); 
  ///
  return Print("GiGaPhysConstructorBase initialized succesfully" ,
               StatusCode::SUCCESS                        , MSG::VERBOSE );
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
StatusCode GiGaPhysConstructorBase::finalize   ()  
{ 
  Print("GiGaPhysConstructorBase Finalization" ,
        StatusCode::SUCCESS             , MSG::VERBOSE );
  // finalze the base class 
  return GiGaBase::finalize   (); 
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
