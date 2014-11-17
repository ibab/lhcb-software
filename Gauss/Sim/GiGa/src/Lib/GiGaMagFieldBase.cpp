// $Id: GiGaMagFieldBase.cpp,v 1.11 2006-02-17 14:34:18 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.10  2003/04/06 18:49:47  ibelyaev
//  see $GIGAROOT/doc/release.notes
//
//  ===========================================================================
#define GIGA_GIGAMAGFIELDBASE_CPP 1 
///  ===========================================================================
/// Gaudi Kernel 
#include "GaudiKernel/IMagneticFieldSvc.h" 
#include "GaudiKernel/ISvcLocator.h" 
#include "GaudiKernel/PropertyMgr.h"
/// GiGa  
#include "GiGa/GiGaMagFieldBase.h"
#include "GiGa/GiGaUtil.h"

// ============================================================================
/** @file 
 *  The implemenation of the class GiGaMagFieldBase 
 *  @author Vanya Belyaev
 */
// ============================================================================

namespace GiGaMagFieldBaseLocal
{
#ifdef GIGA_DEBUG
  /** @var   s_Counter
   *  static instance counter 
   */
  static GiGaUtil::InstanceCounter<GiGaMagFieldBase> s_Counter ;
#endif   
}


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
  : GiGaFieldMgrBase( type ,nick , loc ) 
  , m_nameMFSvc ( "MagneticFieldSvc" )
  , m_mfSvc     ( 0 )
  , m_self      ( 0 )
{ 
  declareInterface<IGiGaMagField> (this);
  declareProperty( "MagneticFieldService" , m_nameMFSvc ); 
#ifdef GIGA_DEBUG
  GiGaMagFieldBaseLocal::s_Counter.increment () ;
#endif 
}
// ============================================================================

// ============================================================================
/// virtual destructor 
// ============================================================================
GiGaMagFieldBase::~GiGaMagFieldBase()
{
#ifdef GIGA_DEBUG
  GiGaMagFieldBaseLocal::s_Counter.decrement () ;
#endif
}
// ============================================================================

// ============================================================================
/** initialize the object 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaMagFieldBase::initialize ()  
{
  /// initialize base class 
  StatusCode sc = GiGaFieldMgrBase::initialize (); 
  if( sc.isFailure() ) 
    { return Error("Could not initialize base class!", sc); }
  if( !m_nameMFSvc.empty() ) 
    {
      sc = svcLoc()->service( m_nameMFSvc, m_mfSvc, true );
      if( sc.isFailure() ) 
        { return Error("Could not locate MagneticField Service!", sc ) ; }
      if( 0 == mfSvc()   ) 
        { return Error("IMagneticFieldSvc* points to NULL") ; }
    }
  else { Warning("Magnetic Field Service is not requested") ; }
  ///
  m_self = this ;
  //
  return Print("GiGaMagFieldBase initialized successfully" ,
               StatusCode::SUCCESS                         , MSG::VERBOSE ) ;
}
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
  return GiGaFieldMgrBase::finalize   (); 
}
// ============================================================================

// ============================================================================
/** accessor to magnetic field 
 *  @see G4MagneticField 
 *  @return pointer to the magnetic field object
 */
// ============================================================================
G4MagneticField*        GiGaMagFieldBase::field    () const
{
  if( 0 != m_self ) { return m_self ; }
  const G4MagneticField* mag = this ;
  m_self = const_cast<G4MagneticField*> ( mag );
  return m_self ;
}
// ============================================================================


// ============================================================================
// The END 
// ============================================================================














