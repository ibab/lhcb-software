
#include <string>

#include "GaudiKernel/ISvcLocator.h"

#include "GiGa/GiGaCnv.h"
#include "GiGa/IGiGaCnvSvc.h"
#include "GiGa/IGiGaGeomCnvSvc.h"
#include "GiGa/IGiGaKineCnvSvc.h"



/// constructor 
GiGaCnv::GiGaCnv( const unsigned char  StorageType , 
		  const CLID&          ClassType   , 
		  ISvcLocator*         Locator     )
  : Converter(                StorageType , 
			      ClassType   , 
			      Locator     ) 
  ///
  , m_NameOfGiGaConversionService   ( "NotDefined"       ) 
  , m_GiGaCnvSvc                    (  0                 ) 
  , m_GiGaGeomCnvSvc                (  0                 ) 
  , m_GiGaKineCnvSvc                (  0                 ) 
  , m_evtSvc                        (  0                 ) 
  , m_detSvc                        (  0                 ) 
  ///
{};   
///  
GiGaCnv::~GiGaCnv() {}; 
///
StatusCode GiGaCnv::initialize () 
{
  StatusCode st = Converter::initialize() ; 
  if( st.isFailure() ) { return Error("Initialize::uanle to initialize Converter base class"); } 
  ///
  {
    StatusCode sc = serviceLocator()->service( m_NameOfGiGaConversionService , m_GiGaCnvSvc ) ;
    if ( st.isFailure() ) { return Error("Initialize::unable to locate IGiGaCnvSvs="+m_NameOfGiGaConversionService, sc );} 
    if ( 0 == cnvSvc()  ) { return Error("Initialize::unable to locate IGiGaCnvSvs="+m_NameOfGiGaConversionService     );} 
    cnvSvc()->addRef() ; 
  }
  ///
  m_GiGaGeomCnvSvc = dynamic_cast<IGiGaGeomCnvSvc*> ( m_GiGaCnvSvc ); 
  m_GiGaKineCnvSvc = dynamic_cast<IGiGaKineCnvSvc*> ( m_GiGaCnvSvc ); 
  ///
  if( 0 != geoSvc  () ) { geoSvc  ()->addRef() ; } 
  if( 0 != kineSvc () ) { kineSvc ()->addRef() ; } 
  if( 0 == geoSvc() && 0 == kineSvc() )
    { return Error("Initialize:: neither GeomCnvSvc nor GeomKineCnvSvc is located!") ; } 
  ///
  {
    const std::string evtName("EventDataSvc");
    StatusCode sc = serviceLocator()->service( evtName , m_evtSvc) ;
    if ( st.isFailure() ) { return Error("Initialize::unable to locate IDataProviderSvs="+evtName, sc );} 
    if ( 0 == evtSvc()  ) { return Error("Initialize::unable to locate IDataProviderSvs="+evtName     );} 
    evtSvc()->addRef();
  }
  ///
  {
    const std::string detName("DetectorDataSvc");
    StatusCode sc = serviceLocator()->service( detName , m_detSvc ) ;
    if ( st.isFailure() ) { return Error("Initialize::unable to locate IDataProviderSvs="+detName, sc );} 
    if ( 0 == detSvc()  ) { return Error("Initialize::unable to locate IDataProviderSvs="+detName     );}
    detSvc()->addRef(); 
  }
  ///
  return StatusCode::SUCCESS ; 
};
///
StatusCode GiGaCnv::finalize () 
{
  /// release (in reverse order)
  if( 0 != detSvc () ) { detSvc ()->release() ; m_detSvc         = 0 ; } 
  if( 0 != evtSvc () ) { evtSvc ()->release() ; m_evtSvc         = 0 ; } 
  if( 0 != kineSvc() ) { kineSvc()->release() ; m_GiGaKineCnvSvc = 0 ; } 
  if( 0 != geoSvc () ) { geoSvc ()->release() ; m_GiGaGeomCnvSvc = 0 ; } 
  if( 0 != cnvSvc () ) { cnvSvc ()->release() ; m_GiGaCnvSvc     = 0 ; } 
  ///
  return Converter::finalize() ; 
  ///
};
///






