#include <string>
#include <vector>

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/IGiGaSvc.h"
#include "GaudiKernel/IGiGaSetUpSvc.h"
#include "GaudiKernel/IChronoStatSvc.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/IObjManager.h"

#include "GaudiKernel/System.h"

/// from GiGa 
#include "GiGa/GiGaCnvSvc.h"

///
GiGaCnvSvc::GiGaCnvSvc( const std::string&   ServiceName       , 
			ISvcLocator*         ServiceLocator    ,
			const unsigned int   StorageType       )
  : ConversionSvc(                           ServiceName       , 
				             ServiceLocator    , 
				             StorageType       )  
  ///
  , m_dpName      ( "UndefinedName"       )
  , m_dpSvc       (     0                 )
  /// 
  , m_evtName     ( "EventDataSvc"        )
  , m_evtSvc      (     0                 )
  ///
  , m_detName     ( "DetectorDataSvc"     )
  , m_detSvc      (     0                 )
  ///
  , m_gigaName    ( "GiGaSvc"             ) 
  , m_gigaSvc     (     0                 ) 
  ///
  , m_setupName   ( "GiGaSvc"             ) 
  , m_setupSvc    (     0                 ) 
  ///
  , m_ppName      ( "ParticlePropertySvc" )  
  , m_ppSvc       (     0                 ) 
  ///
  , m_chronoName  ( "ChronoStatSvc"       )  
  , m_chronoSvc   (     0                 ) 
  ///
  , m_mfName      ( "MagneticFieldSvc"    )  
  , m_mfSvc       (     0                 ) 
  ///
  , m_omName      ( "ApplicationMgr"      )  
  , m_objMgr      (     0                 ) 
  ///
{ 
  declareProperty   ( "EventDataProviderService"        , m_evtName     );
  declareProperty   ( "DetectorDataProviderService"     , m_detName     );
  declareProperty   ( "GiGaService"                     , m_gigaName    ); 
  declareProperty   ( "GiGaSetUpService"                , m_setupName   ); 
  declareProperty   ( "ParticlePropertyService"         , m_ppName      );
  declareProperty   ( "MagneticFieldService"            , m_mfName      );
  declareProperty   ( "ChronoStatService"               , m_chronoName  );
  declareProperty   ( "ObjectManager"                   , m_omName      );
};
///
GiGaCnvSvc::~GiGaCnvSvc(){}; 
///
StatusCode GiGaCnvSvc::initialize()
{
  ///
  StatusCode st = ConversionSvc::initialize() ; 
  if( st.isFailure()     )  { return Error("Initialize::Could not initialize base class ConversionSvc", st); } 
  ///
  {
    StatusCode status = serviceLocator()->service( m_dpName , m_dpSvc );
    if( status.isFailure() ) { return Error("Initialize::Could not locate IDataProvider="+m_dpName, status );}      
    if( 0 == dpSvc()       ) { return Error("Initialize::Could not locate IDataProvider="+m_dpName         );}
    dpSvc()->addRef(); 
    setStore( dpSvc() ); 
    Print( " Located DataProvider="+m_dpName, MSG::VERBOSE ); 
  } 
  ///
  {
    StatusCode status = serviceLocator()->service( m_gigaName , m_gigaSvc ) ;
    if( status.isFailure() ) { return Error("Initialize::Could not locate IGiGaSvc="+m_gigaName, status );}      
    if( 0 == gigaSvc()     ) { return Error("Initialize::Could not locate IGiGaSvc="+m_gigaName         );}
    gigaSvc()->addRef(); 
    Print( " Located GiGa Service="+m_gigaName, MSG::VERBOSE ); 
  } 
  ///
  {
    StatusCode status = serviceLocator()->service( m_setupName , m_setupSvc ) ;
    if( status.isFailure() ) { return Error("Initialize::Could not locate IGiGaSetUpSvc="+m_setupName, status );}      
    if( 0 == setupSvc()    ) { return Error("Initialize::Could not locate IGiGaSetUpSvc="+m_setupName         );}
    setupSvc()->addRef(); 
    Print( " Located GiGa SetUp Service="+m_setupName, MSG::VERBOSE ); 
  } 
  ///
  if( !m_evtName.empty() ) 
    {
      StatusCode status = serviceLocator()->service( m_evtName , m_evtSvc ) ; 
      if( status.isFailure() ) { return Error("Initialize::Could not locate IDataProvider="+m_evtName, status );}      
      if( 0 == evtSvc()      ) { return Error("Initialize::Could not locate IDataProvider="+m_evtName         );}
      evtSvc()->addRef(); 
      Print( " Located Event Data  Service="+m_evtName, MSG::VERBOSE ); 
    } 
  else { Warning(" Event Data Service is not requested to be located!") ;} 
  ///
  if( !m_detName.empty() ) 
    {
      StatusCode status = serviceLocator()->service( m_detName , m_detSvc ) ; 
      if( status.isFailure() ) { return Error("Initialize::Could not locate IDataProvider="+m_detName, status );}      
      if( 0 == detSvc()      ) { return Error("Initialize::Could not locate IDataProvider="+m_detName         );}
      detSvc()->addRef(); 
      Print( " Located Detector Data  Service="+m_detName, MSG::VERBOSE ); 
    } 
  else { Warning(" Detector Data Service is not requested to be located!") ;} 
  ///
  if( !m_ppName.empty() ) 
    {
      StatusCode status = serviceLocator()->service( m_ppName , m_ppSvc  ) ; 
      if( status.isFailure() ) { return Error("Initialize::Could not locate IParticlePropertySvc="+m_ppName, status );}      
      if( 0 == ppSvc()       ) { return Error("Initialize::Could not locate IParticlePropertySvc="+m_ppName         );}
      ppSvc()->addRef(); 
      Print( " Located Particle Properties  Service="+m_ppName, MSG::VERBOSE ); 
    } 
  else { Warning(" Particle Property Service is not requested to be located!") ;} 
  ///
  if( !m_mfName.empty() ) 
    {
      StatusCode status = serviceLocator()->service( m_mfName , m_mfSvc ) ; 
      if( status.isFailure() ) { return Error("Initialize::Could not locate IMagneticFieldSvc="+m_mfName, status );}      
      if( 0 == mfSvc()       ) { return Error("Initialize::Could not locate IMagneticFieldSvc="+m_mfName         );}
      mfSvc()->addRef(); 
      Print( " Located Magnetic Field Service="+m_mfName, MSG::VERBOSE ); 
    } 
  else { Warning(" Magnetic Field Service is not requested to be located!") ;} 
  ///
  if( !m_chronoName.empty() ) 
    {
      StatusCode status = serviceLocator()->service( m_chronoName , m_chronoSvc  ) ; 
      if( status.isFailure() ) { return Error("Initialize::Could not locate IChronoStatSvc="+m_chronoName, status );}      
      if( 0 == chronoSvc()   ) { return Error("Initialize::Could not locate IChronoStatSvc="+m_chronoName         );}
      chronoSvc()->addRef(); 
      Print( " Located Chrono & Stat Service="+m_chronoName, MSG::VERBOSE ); 
    } 
  else { Warning(" Chrono & Stat Service is not requested to be located!") ;} 
  ///
  if( !m_omName.empty() ) 
    {
      StatusCode status = serviceLocator()->service( m_omName , m_objMgr  ) ; 
      if( status.isFailure() ) { return Error("Initialize::Could not locate IObjManager="+m_omName, status );}      
      if( 0 == objMgr()      ) { return Error("Initialize::Could not locate IObjManager="+m_omName         );}
      objMgr()->addRef(); 
      Print( " Located ObjectManager "+m_chronoName, MSG::VERBOSE ); 
    } 
  else { Warning(" Object Manager is not requested to be located!") ;} 
  ///
  return StatusCode::SUCCESS; 
  ///
};
///
StatusCode GiGaCnvSvc::finalize()
{
  ///
  if ( 0 != dpSvc     () ) { dpSvc     ()->release() ; m_dpSvc     = 0 ; } 
  if ( 0 != evtSvc    () ) { evtSvc    ()->release() ; m_evtSvc    = 0 ; } 
  if ( 0 != detSvc    () ) { detSvc    ()->release() ; m_detSvc    = 0 ; } 
  if ( 0 != gigaSvc   () ) { gigaSvc   ()->release() ; m_gigaSvc   = 0 ; } 
  if ( 0 != setupSvc  () ) { setupSvc  ()->release() ; m_setupSvc  = 0 ; } 
  if ( 0 != ppSvc     () ) { ppSvc     ()->release() ; m_ppSvc     = 0 ; } 
  if ( 0 != mfSvc     () ) { mfSvc     ()->release() ; m_mfSvc     = 0 ; } 
  if ( 0 != chronoSvc () ) { chronoSvc ()->release() ; m_chronoSvc = 0 ; } 
  if ( 0 != objMgr    () ) { objMgr    ()->release() ; m_objMgr    = 0 ; } 
  ///
  StatusCode st = ConversionSvc::finalize(); 
  ///
  return st; 
  ///
};
///
StatusCode GiGaCnvSvc::queryInterface( const IID& iid , void** ppI )
{ 
  if( 0 == ppI                ) { return StatusCode::FAILURE                        ; } 
  *ppI = 0 ;
  if( IID_IGiGaCnvSvc  == iid ) { *ppI = static_cast<IGiGaCnvSvc*>(this)            ; } 
  else                          { return ConversionSvc::queryInterface( iid , ppI ) ; } 
  addRef();
  return StatusCode::SUCCESS;  
};
///
















