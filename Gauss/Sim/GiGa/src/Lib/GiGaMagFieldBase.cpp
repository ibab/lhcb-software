// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.6  2001/08/12 15:42:49  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.5  2001/07/27 17:03:18  ibelyaev
/// improved printout
///
/// Revision 1.4  2001/07/23 13:12:11  ibelyaev
/// the package restructurisation(II)
/// 
//  ===========================================================================
#define GIGA_GIGAMAGFIELDBASE_CPP 1 
///  ===========================================================================
/// Gaudi Kernel 
#include "GaudiKernel/IMagneticFieldSvc.h" 
#include "GaudiKernel/ISvcLocator.h" 
#include "GaudiKernel/PropertyMgr.h"
/// GiGa  
#include "GiGa/GiGaMagFieldBase.h"

/** The implemenation of the class GiGaMagFieldBase 
 *  @author Vanya Belyaev
 */

// ============================================================================
/** standard constructor
 *  @param name name of the object
 *  @param loc  pointer to service locator 
 */
// ============================================================================
GiGaMagFieldBase::GiGaMagFieldBase( const std::string& nick , 
                                    ISvcLocator*       loc ) 
  : GiGaBase( nick , loc ) 
  , m_nameMFSvc ( "MagneticFieldSvc" )
  , m_mfSvc     ( 0 )
{ declareProperty( "MagneticFieldService" , m_nameMFSvc ); };

// ============================================================================
/// virtual destructor 
// ============================================================================
GiGaMagFieldBase::~GiGaMagFieldBase(){};

// ============================================================================
/** query the interface
 *  @param id   uniqie interface identifier 
 *  @param ppI  placeholder for returned interface 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaMagFieldBase::queryInterface( const InterfaceID& id , 
                                             void** ppI) 
{
  if( 0 == ppI ) { return StatusCode::FAILURE; } 
  *ppI = 0 ; 
  if   ( IGiGaMagField::interfaceID() == id ) 
    { *ppI = static_cast<IGiGaMagField*> (this) ; } 
  else                                        
    { return GiGaBase::queryInterface( id , ppI ); } /// RETURN ;
  addRef();
  return StatusCode::SUCCESS; 
};

// ============================================================================
/// identification  
// ============================================================================
const std::string& GiGaMagFieldBase::name () const 
{ return GiGaBase::name() ; }; 

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
  Print("GiGaMagFieldBase initialized successfully" ,
        StatusCode::SUCCESS , MSG::DEBUG ) ;
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** finalize the object  
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaMagFieldBase::finalize   ()  
{
  /// relese magnetic field service
  if( 0 != mfSvc() ) { mfSvc() -> release() ; m_mfSvc = 0 ; }
  ///
  Print("GiGaMagFieldBase finalization" ,
        StatusCode::SUCCESS , MSG::DEBUG ) ;
  ///
  return GiGaBase::finalize   (); 
};

// ============================================================================













