// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
// ============================================================================
/// Gaudi 
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
#include "GiGaCnv/IGiGaCnvSvcLocation.h"
/// local
#include "GiGaHits.h" 

// ============================================================================
// ============================================================================
static const  SvcFactory<GiGaHits>         s_Factory ; 
const        ISvcFactory&GiGaHitsFactory = s_Factory ; 

// ============================================================================
/** standard constructor 
 *  @param ServiceName     name of the service
 *  @param ServiceLocator  pointer to service locator
 */
// ============================================================================
GiGaHits::GiGaHits( const std::string&   ServiceName          , 
                                ISvcLocator*         ServiceLocator       ) 
  : GiGaCnvSvcBase(                                  ServiceName          , 
                                                     ServiceLocator       , 
                                                     GiGaHits_StorageType )
  // default name for KineCnv Service 
  , m_kineSvcName ( IGiGaCnvSvcLocation::Kine )
  , m_kineSvc     ( 0 )
{
  ///
  declareProperty( "KineConversionService" , m_kineSvcName );
  ///
  setNameOfDataProviderSvc("EventDataSvc");
}

// ============================================================================
/// destructor 
// ============================================================================
GiGaHits::~GiGaHits(){};

// ============================================================================
/** initialize the service
 *  @return status code
 */
// ============================================================================
StatusCode GiGaHits::initialize() 
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
StatusCode GiGaHits::finalize()   
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
GiGaKineRefTable& GiGaHits::table()
{
  if( 0 == kineSvc() ) 
    { Exception ( "GiGaHits::table(): IGiGaCnvSvc* points to NULL!" );}
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
StatusCode GiGaHits::queryInterface( const InterfaceID& ID , 
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
















