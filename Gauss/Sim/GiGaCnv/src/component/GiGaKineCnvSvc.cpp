/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================

/// from Gaudi 
#include "GaudiKernel/AddrFactory.h" 
#include "GaudiKernel/SvcFactory.h" 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IParticlePropertySvc.h"
/// from GiGa 
#include "GiGaCnv/GiGaCnvSvcBase.h" 
// local
#include "GiGaKineCnvSvc.h" 

/// ===========================================================================
/// Address factory 
/// ===========================================================================
extern const IAddrFactory& GiGaKineAddressFactory ;

/// ===========================================================================
/// service factory 
/// ===========================================================================
static const  SvcFactory<GiGaKineCnvSvc> s_Factory ; 
const        ISvcFactory&GiGaKineCnvSvcFactory = s_Factory ;

/// ===========================================================================
/** standard constructor
 *  @param name  name of the service 
 *  @param loc   pointer to service locator 
 */
/// ===========================================================================
GiGaKineCnvSvc::GiGaKineCnvSvc( const std::string&   ServiceName          , 
                                ISvcLocator*         ServiceLocator       ) 
  : GiGaCnvSvcBase(                                  ServiceName          , 
                                                     ServiceLocator       , 
                                                     GiGaKine_StorageType )
  , m_ppSvcName ("ParticlePropertySvc")
  , m_ppSvc     ( 0 )                
  ///
  , m_table     (   )
{
  setAddressFactory(&GiGaKineAddressFactory);
  setNameOfDataProviderSvc("EventDataSvc");
  ///
  declareProperty("ParticlePropertyService" , m_ppSvcName );
  ///
};

/// ===========================================================================
/// virtual destructor
/// ===========================================================================
GiGaKineCnvSvc::~GiGaKineCnvSvc()
{ 
  /// clear the reference table 
  m_table.clear(); 
};

/// ===========================================================================
/** initialization 
 *  @return status code 
 */
/// ===========================================================================
StatusCode GiGaKineCnvSvc::initialize() 
{ 
  StatusCode sc = GiGaCnvSvcBase::initialize(); 
  if( sc.isFailure() ) 
    { return Error("Could not initialize base class!", sc ); }
  ///
  sc = svcLoc()->service( m_ppSvcName , m_ppSvc );
  if( sc.isFailure() ) 
    { return Error("Could not locate ParticlePropertyService!", sc);}
  if( 0 == ppSvc  ()  )
    { return Error("IParticlePropertySvc* points to NULL!");}
  ///
  return StatusCode::SUCCESS;
};  

/// ===========================================================================
/** finalization 
 *  @return status code 
 */
/// ===========================================================================
StatusCode GiGaKineCnvSvc::finalize()   
{ 
  /// clear the reference table 
  m_table.clear();
  /// release particle property service
  if( 0 != ppSvc() ) 
    { ppSvc()->release() ; m_ppSvc = 0 ; }
  ///
  return GiGaCnvSvcBase::finalize(); 
};  

/// ===========================================================================




















