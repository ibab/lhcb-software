/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
/// Gaudi 
#include "GaudiKernel/AddrFactory.h" 
#include "GaudiKernel/SvcFactory.h" 
#include "GaudiKernel/MsgStream.h"
/// GiGaCnv 
#include "GiGaCnv/IGiGaKineCnvSvc.h"
/// local
#include "GiGaHitsCnvSvc.h" 

/// ===========================================================================
/// ===========================================================================
extern const IAddrFactory& GiGaHitsAddressFactory ; 

/// ===========================================================================
/// ===========================================================================
static const  SvcFactory<GiGaHitsCnvSvc>                         s_Factory ; 
const        ISvcFactory&                GiGaHitsCnvSvcFactory = s_Factory ; 

/// ===========================================================================
/** standard constructor 
 *  @param ServiceName     name of the service
 *  @param ServiceLocator  pointer to service locator
 */
/// ===========================================================================
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

/// ===========================================================================
/// destructor 
/// ===========================================================================
GiGaHitsCnvSvc::~GiGaHitsCnvSvc(){};

/// ===========================================================================
/** initialize the service
 *  @return status code
 */
/// ===========================================================================
StatusCode GiGaHitsCnvSvc::initialize() 
{ 
  StatusCode sc = GiGaCnvSvcBase::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class!");}
  /// locate KineCnv  service
  if( !m_kineSvcName.empty() )
    {
      sc = service( m_kineSvcName , m_kineSvc );
      if( sc.isFailure() )
        { return Error("Could Not Locate IGiGaKineCnvSvc* =" + 
                       m_kineSvcName , sc ) ; }
      if( 0 == kineSvc() ) 
        { return Error("IGiGaKineCnvSvc* points to NULL!" ) ; }
      kineSvc()->addRef();
    }
  else 
    { return Error("Empty name for Kine Conversion Service!!"); }
  ///
  return StatusCode::SUCCESS ;
};


/// ===========================================================================
/** finalize the service
 *  @return status code
 */ 
/// ===========================================================================
StatusCode GiGaHitsCnvSvc::finalize()   
{ 
  /// release kine service
  if( 0 != kineSvc() ) { kineSvc()->release() ;  m_kineSvc =  0 ; }
  ///
  return GiGaCnvSvcBase::finalize  (); 
};  


/// ===========================================================================
/** retrieve the relation table between Geant4 track/trajectory 
 *  identifiers and the converted MCParticle objects 
 *  @return the reference to relation table  
 */
/// ===========================================================================
GiGaKineRefTable& GiGaHitsCnvSvc::table()
{
  if( 0 == kineSvc() ) 
    { Exception ( "GiGaHitsCnvSvc::table(): IGiGaCnvSvc* points to NULL!" );}
  ///
  return kineSvc()->table();
};

/// ===========================================================================



















