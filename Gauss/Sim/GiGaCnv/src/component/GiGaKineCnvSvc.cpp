// $Id: GiGaKineCnvSvc.cpp,v 1.6 2002-01-22 18:24:43 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2001/08/12 17:24:53  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.4  2001/07/25 17:19:32  ibelyaev
// all conversions now are moved from GiGa to GiGaCnv
//
// Revision 1.3  2001/07/24 11:13:55  ibelyaev
// package restructurization(III) and update for newer GiGa
// 
// ============================================================================

/// from Gaudi 
#include "GaudiKernel/SvcFactory.h" 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IParticlePropertySvc.h"
/// from GiGa 
#include "GiGaCnv/GiGaCnvSvcBase.h" 
// local
#include "GiGaKineCnvSvc.h" 

// ============================================================================
/// service factory 
// ============================================================================
static const  SvcFactory<GiGaKineCnvSvc> s_Factory ; 
const        ISvcFactory&GiGaKineCnvSvcFactory = s_Factory ;

// ============================================================================
/** standard constructor
 *  @param name  name of the service 
 *  @param loc   pointer to service locator 
 */
// ============================================================================
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
  setNameOfDataProviderSvc("EventDataSvc");
  ///
  declareProperty("ParticlePropertyService" , m_ppSvcName );
  ///
};

// ============================================================================
/// virtual destructor
// ============================================================================
GiGaKineCnvSvc::~GiGaKineCnvSvc()
{ 
  /// clear the reference table 
  m_table.clear(); 
};

// ============================================================================
/** initialization 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaKineCnvSvc::initialize() 
{ 
  StatusCode sc = GiGaCnvSvcBase::initialize(); 
  if( sc.isFailure() ) 
    { return Error("Could not initialize base class!", sc ); }
  ///
  sc = svcLoc()->service( m_ppSvcName , m_ppSvc , true );
  if( sc.isFailure() ) 
    { return Error("Could not locate ParticlePropertyService!", sc);}
  if( 0 == ppSvc  ()  )
    { return Error("IParticlePropertySvc* points to NULL!");}
  ///
  return StatusCode::SUCCESS;
};  

// ============================================================================
/** finalization 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaKineCnvSvc::finalize()   
{ 
  /// clear the reference table 
  m_table.clear();
  /// release particle property service
  if( 0 != ppSvc() ) { ppSvc()->release() ; m_ppSvc = 0 ; }
  ///
  return GiGaCnvSvcBase::finalize(); 
};  

// ============================================================================
/** query the interface
 *  @param ID unique interface identifier 
 *  @param II placeholder for interface 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaKineCnvSvc::queryInterface( const InterfaceID& ID , 
                                           void**             II ) 
{
  if( 0 == II ) { return StatusCode::FAILURE; }
  /// reset interface
  *II = 0;
  if     ( IGiGaKineCnvSvc ::interfaceID () == ID )
    { *II = static_cast<IGiGaKineCnvSvc*>  ( this ) ; }
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




















