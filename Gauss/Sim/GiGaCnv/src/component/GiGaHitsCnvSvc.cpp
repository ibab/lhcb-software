// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.5  2001/07/27 18:50:35  ibelyaev
///  bug fix
///
/// Revision 1.4  2001/07/25 17:19:31  ibelyaev
/// all conversions now are moved from GiGa to GiGaCnv
///
/// Revision 1.3  2001/07/24 11:13:55  ibelyaev
/// package restructurization(III) and update for newer GiGa
/// 
// ============================================================================
/// Gaudi 
#include "GaudiKernel/AddrFactory.h" 
#include "GaudiKernel/SvcFactory.h" 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/ISvcLocator.h"
/// GiGa
#include "GiGa/GiGaUtil.h"
#include "GiGa/IGiGaSvc.h"
/// GiGaCnv 
#include "GiGaCnv/IGiGaKineCnvSvc.h"
#include "GiGaCnv/IGiGaGeomCnvSvc.h"
/// local
#include "GiGaHitsCnvSvc.h" 

// ============================================================================
// ============================================================================
extern const IAddrFactory& GiGaHitsAddressFactory ; 

// ============================================================================
// ============================================================================
static const  SvcFactory<GiGaHitsCnvSvc>                         s_Factory ; 
const        ISvcFactory&                GiGaHitsCnvSvcFactory = s_Factory ; 

// ============================================================================
/** standard constructor 
 *  @param ServiceName     name of the service
 *  @param ServiceLocator  pointer to service locator
 */
// ============================================================================
GiGaHitsCnvSvc::GiGaHitsCnvSvc( const std::string&   ServiceName          , 
                                ISvcLocator*         ServiceLocator       ) 
  : GiGaCnvSvcBase(                                  ServiceName          , 
                                                     ServiceLocator       , 
                                                     GiGaHits_StorageType )
  , m_kineSvcName ("GiGaKineCnvSvc" ) ///< default name for KineCnv Service 
  , m_kineSvc     ( 0 )
{
  ///
  declareProperty( "KineConversionService" , m_kineSvcName );
  ///
  setAddressFactory(&GiGaHitsAddressFactory);
  setNameOfDataProviderSvc("EventDataSvc");
}

// ============================================================================
/// destructor 
// ============================================================================
GiGaHitsCnvSvc::~GiGaHitsCnvSvc(){};

// ============================================================================
/** initialize the service
 *  @return status code
 */
// ============================================================================
StatusCode GiGaHitsCnvSvc::initialize() 
{ 
  StatusCode sc = GiGaCnvSvcBase::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!");}
  /// locate KineCnv  service
  if( !m_kineSvcName.empty() )
    {
      sc = svcLoc()->service( m_kineSvcName , m_kineSvc );
      if( sc.isFailure() )
        { return Error("Could Not Locate IGiGaKineCnvSvc* ='" + 
                       m_kineSvcName + "'", sc ) ; }
      if( 0 == kineSvc() ) 
        { return Error("IGiGaKineCnvSvc* points to NULL!" ) ; }
      kineSvc()->addRef();
    }
  else 
    { return Error("Empty name for Kine Conversion Service!!"); }
  ///
  return StatusCode::SUCCESS;
};


// ============================================================================
/** finalize the service
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaHitsCnvSvc::finalize()   
{ 
  /// release kine service
  if( 0 != kineSvc() ) { kineSvc()->release() ;  m_kineSvc =  0 ; }
  ///
  return GiGaCnvSvcBase::finalize  (); 
};  


// ============================================================================
/** retrieve the relation table between Geant4 track/trajectory 
 *  identifiers and the converted MCParticle objects 
 *  @return the reference to relation table  
 */
// ============================================================================
GiGaKineRefTable& GiGaHitsCnvSvc::table()
{
  if( 0 == kineSvc() ) 
    { Exception ( "GiGaHitsCnvSvc::table(): IGiGaCnvSvc* points to NULL!" );}
  ///
  return kineSvc()->table();
};


// ============================================================================
/** query the interface
 *  @param ID unique interface identifier 
 *  @param II placeholder for interface 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaHitsCnvSvc::queryInterface( const InterfaceID& ID , 
                                           void**             II ) 
{
  if( 0 == II ) { return StatusCode::FAILURE; }
  /// reset interface
  *II = 0;
  if     ( IGiGaHitsCnvSvc ::interfaceID () == ID )
    { *II = static_cast<IGiGaHitsCnvSvc*>  ( this ) ; }
  else if( IGiGaCnvSvc     ::interfaceID () == ID )
    { *II = static_cast<IGiGaCnvSvc*>      ( this ) ; }
  else if( IConversionSvc  ::interfaceID () == ID )
    { *II = static_cast<IConversionSvc*>   ( this ) ; }
  else if( IService        ::interfaceID () == ID )
    { *II = static_cast<IService*>         ( this ) ; }
  else if( IInterface      ::interfaceID () == ID )
    { *II = static_cast<IInterface*>       ( this ) ; }
  else 
    { return GiGaCnvSvcBase::queryInterface( ID , II ) ; }
  ///
  addRef();
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
















