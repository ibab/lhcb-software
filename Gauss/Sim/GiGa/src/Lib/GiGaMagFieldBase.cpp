// $Id: GiGaMagFieldBase.cpp,v 1.8 2002-05-07 12:21:33 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
//  ===========================================================================
#define GIGA_GIGAMAGFIELDBASE_CPP 1 
///  ===========================================================================
/// Gaudi Kernel 
#include "GaudiKernel/IMagneticFieldSvc.h" 
#include "GaudiKernel/ISvcLocator.h" 
#include "GaudiKernel/PropertyMgr.h"
/// GiGa  
#include "GiGa/GiGaMagFieldBase.h"

// ============================================================================
/** @file 
 *  The implemenation of the class GiGaMagFieldBase 
 *  @author Vanya Belyaev
 */
// ============================================================================

// ============================================================================
/** standard constructor
 *  @param name name of the object
 *  @param loc  pointer to service locator 
 */
// ============================================================================
GiGaMagFieldBase::GiGaMagFieldBase 
( const std::string& type , 
  const std::string& nick , 
  const IInterface*  loc  )
  
  : GiGaBase( type ,nick , loc ) 
  , m_nameMFSvc ( "MagneticFieldSvc" )
  , m_mfSvc     ( 0 )
{ 
  declareInterface<IGiGaMagField> (this);
  declareProperty( "MagneticFieldService" , m_nameMFSvc ); 
};
// ============================================================================

// ============================================================================
/// virtual destructor 
// ============================================================================
GiGaMagFieldBase::~GiGaMagFieldBase(){};
// ============================================================================

// ============================================================================
/** initialize the object 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaMagFieldBase::initialize ()  
{
  /// initialize base class 
  StatusCode sc = GiGaBase::initialize (); 
  if( sc.isFailure() ) 
    { return Error("Could not initialize base class!", sc); }
  if( !m_nameMFSvc.empty() ) 
    {
      sc = svcLoc()->service( m_nameMFSvc, m_mfSvc );
      if( sc.isFailure() ) 
        { return Error("Could not locate MagneticField Service!", sc ) ; }
      if( 0 == mfSvc()   ) 
        { return Error("IMagneticFieldSvc* points to NULL") ; }
      mfSvc()->addRef();
    }
  else { Warning("Magnetic Field Service is not requested") ; }
  ///
  return Print("GiGaMagFieldBase initialized successfully" ,
               StatusCode::SUCCESS                         , MSG::VERBOSE ) ;
};
// ============================================================================

// ============================================================================
/** finalize the object  
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaMagFieldBase::finalize   ()  
{
  Print("GiGaMagFieldBase Finalization" ,
        StatusCode::SUCCESS             , MSG::VERBOSE ) ;
  // relese magnetic field service
  if( 0 != mfSvc() ) { mfSvc() -> release() ; m_mfSvc = 0 ; }
  // finalize the base class 
  return GiGaBase::finalize   (); 
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================














