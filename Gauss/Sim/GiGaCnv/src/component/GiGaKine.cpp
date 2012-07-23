// $Id: GiGaKine.cpp,v 1.4 2007-07-02 08:51:07 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2007/01/12 15:45:56  ranjard
// v18r0 - use GAUDI v19r0
//
// Revision 1.2  2002/12/13 14:25:22  ibelyaev
//  few trivial bug fixes
//
// Revision 1.1  2002/12/07 14:36:26  ibelyaev
//  see $GIGACNVROOT/doc/release.notes
//
// ============================================================================

/// from Gaudi 
#include "GaudiKernel/SvcFactory.h" 
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
/// from LHCb
#include "Kernel/IParticlePropertySvc.h"
/// from GiGa 
#include "GiGaCnv/GiGaCnvSvcBase.h" 
// local
#include "GiGaKine.h" 

// ============================================================================
/// service factory 
// ============================================================================
DECLARE_SERVICE_FACTORY( GiGaKine );

// ============================================================================
/** standard constructor
 *  @param name  name of the service 
 *  @param loc   pointer to service locator 
 */
// ============================================================================
GiGaKine::GiGaKine( const std::string&   ServiceName          , 
                                ISvcLocator*         ServiceLocator       ) 
  : GiGaCnvSvcBase(                                  ServiceName          , 
                                                     ServiceLocator       , 
                                                     GiGaKine_StorageType )
  , m_ppSvcName ("LHCb::ParticlePropertySvc")
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
GiGaKine::~GiGaKine(){};

// ============================================================================
/** initialization 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaKine::initialize() 
{ 
  StatusCode sc = GiGaCnvSvcBase::initialize(); 
  if( sc.isFailure() ) 
    { return Error("Could not initialize base class!", sc ); }
  ///
  sc = svcLoc()->service( m_ppSvcName , m_ppSvc , true );
  if( sc.isFailure() ) 
    { return Error("Could not locate LHCb::ParticlePropertyService!", sc);}
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
StatusCode GiGaKine::finalize()   
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
StatusCode GiGaKine::queryInterface( const InterfaceID& ID , 
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
  else if ( INamedInterface ::interfaceID ()  == ID )
    { *II  = static_cast<INamedInterface*>   ( this )   ; }
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




















