/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#define GIGACNV_GIGACNVBASE_CPP 1 
/// ===========================================================================
/// STL 
#include <string> 
#include <vector> 
/// GaudiKernel
#include "GaudiKernel/Converter.h" 
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Stat.h"
/// GiGa 
#include "GiGa/IGiGaSvc.h" 
#include "GiGa/IGiGaSetUpSvc.h"
#include "GiGa/IGiGaCnvSvc.h" 
#include "GiGa/GiGaException.h"
#include "GiGa/IGiGaGeomCnvSvc.h" 
#include "GiGa/IGiGaKineCnvSvc.h" 
#include "GiGa/IGiGaHitsCnvSvc.h" 
// GiGaCnv 
#include "GiGaCnv/GiGaCnvBase.h"


/// ===========================================================================
/// constructor 
/// ===========================================================================
GiGaCnvBase::GiGaCnvBase( const unsigned char  StorageType , 
                          const CLID&          ClassType   , 
                          ISvcLocator*         Locator     )  
  : Converter( StorageType ,  ClassType , Locator     ) 
  ///
  , m_NameOfGiGaConversionService   ( "NotYetDefined" ) 
  , m_ConverterName                 ( "NotYetDefined" )
  ///
  , m_leaves                        (                 )
  ///
  , m_GiGaCnvSvc                    (  0              ) 
  , m_GiGaGeomCnvSvc                (  0              ) 
  , m_GiGaKineCnvSvc                (  0              ) 
  , m_GiGaHitsCnvSvc                (  0              ) 
  , m_evtSvc                        (  0              ) 
  , m_detSvc                        (  0              ) 
  , m_chronoSvc                     (  0              ) 
  ///
{};

/// ===========================================================================
/// destructor 
/// ===========================================================================
GiGaCnvBase::~GiGaCnvBase(){};

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaCnvBase::Exception( const std::string    & Message , 
                                   const GaudiException & Excp    ,
                                   const MSG::Level     & level   , 
                                   const StatusCode     & Status )
{
  Stat stat( chronoSvc() , Excp.tag() );
  MsgStream log( msgSvc() , name() ); 
  log << level << "Exception:" 
      << Message << ":" << Excp << endreq ; 
  return  Status;
};  

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaCnvBase::Exception( const std::string    & Message , 
                                   const std::exception & Excp    ,
                                   const MSG::Level     & level   , 
                                   const StatusCode     & Status )
{
  Stat stat( chronoSvc() , "std::exception" );
  MsgStream log( msgSvc() , name() ); 
  log << level << " Exception:" 
      << Message << ":" << Excp.what() << endreq ; 
  return  Status;
};  

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaCnvBase::Exception( const std::string    & Message , 
                                   const MSG::Level     & level   , 
                                   const StatusCode     & Status )
{
  Stat stat( chronoSvc() , "*UNKNOWN* exception" );
  MsgStream log( msgSvc() , name() ); 
  log << level << Message << ": UNKNOWN exception"  << endreq ; 
  return  Status;
};  

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaCnvBase::Error( const std::string& Message , 
                               const StatusCode& status )
{
  Stat stat( chronoSvc() , name() + ":Error" );
  MsgStream log( msgSvc() , name() ); log << MSG::ERROR << Message << endreq; 
  return status; 
};

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaCnvBase::initialize () 
{
  StatusCode st = Converter::initialize() ; 
  if( st.isFailure() ) 
    { return Error("Initialize::uanle to initialize Converter base class"); } 
  ///
  {
    StatusCode sc = 
      serviceLocator()->
      service( m_NameOfGiGaConversionService , m_GiGaCnvSvc ) ;
    if ( st.isFailure() ) 
      { return Error("Initialize::unable to locate IGiGaCnvSvs=" + 
                     m_NameOfGiGaConversionService, sc );} 
    if ( 0 == cnvSvc()  ) 
      { return Error("Initialize::unable to locate IGiGaCnvSvs=" + 
                     m_NameOfGiGaConversionService     );} 
    cnvSvc()->addRef() ; 
  }
  ///
  m_GiGaGeomCnvSvc = dynamic_cast<IGiGaGeomCnvSvc*> ( m_GiGaCnvSvc ); 
  m_GiGaKineCnvSvc = dynamic_cast<IGiGaKineCnvSvc*> ( m_GiGaCnvSvc ); 
  m_GiGaHitsCnvSvc = dynamic_cast<IGiGaHitsCnvSvc*> ( m_GiGaCnvSvc ); 
  ///
  if( 0 != geoSvc  () ) { geoSvc  ()->addRef() ; } 
  if( 0 != kineSvc () ) { kineSvc ()->addRef() ; } 
  if( 0 != hitsSvc () ) { hitsSvc ()->addRef() ; } 
  if( 0 == geoSvc() && 0 == kineSvc() && 0 == hitsSvc() )
    { return Error("Initialize::neither Geom,Hits or Kine CnvSvc located!");} 
  ///
  {
    const std::string evtName("EventDataSvc");
    StatusCode sc = serviceLocator()->service( evtName , m_evtSvc) ;
    if ( st.isFailure() ) 
      { return Error("Initialize::unable to locate IDataProviderSvs=" + 
                     evtName, sc );} 
    if ( 0 == evtSvc()  ) 
      { return Error("Initialize::unable to locate IDataProviderSvs=" + 
                     evtName     );} 
    evtSvc()->addRef();
  }
  ///
  {
    const std::string detName("DetectorDataSvc");
    StatusCode sc = serviceLocator()->service( detName , m_detSvc ) ;
    if ( st.isFailure() )
      { return Error("Initialize::unable to locate IDataProviderSvs=" + 
                     detName, sc );} 
    if ( 0 == detSvc()  ) 
      { return Error("Initialize::unable to locate IDataProviderSvs=" + 
                     detName     );}
    detSvc()->addRef(); 
  }
  ///
  {
    const std::string chronoName("ChronoStatSvc");
    StatusCode sc = serviceLocator()->service( chronoName , m_chronoSvc ) ;
    if ( st.isFailure()    ) 
      { return Error("Initialize::unable to locate IChronoStatSvs=" + 
                     chronoName, sc );} 
    if ( 0 == chronoSvc()  ) 
      { return Error("Initialize::unable to locate IChronoStatSvs=" + 
                     chronoName  );}
    chronoSvc()->addRef(); 
  }
  ///
  {
    for( IGiGaCnvSvc::Leaves::const_iterator it = m_leaves.begin() ; 
         m_leaves.end() != it ; ++it )
      { cnvSvc()->declareObject( *it ); }
  }
  ///
  return StatusCode::SUCCESS ; 
};

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaCnvBase::finalize () 
{
  /// release (in reverse order)
  if( 0 != chronoSvc () ) { chronoSvc ()->release() ; m_chronoSvc      = 0 ; } 
  if( 0 != detSvc    () ) { detSvc    ()->release() ; m_detSvc         = 0 ; } 
  if( 0 != evtSvc    () ) { evtSvc    ()->release() ; m_evtSvc         = 0 ; } 
  if( 0 != kineSvc   () ) { kineSvc   ()->release() ; m_GiGaKineCnvSvc = 0 ; } 
  if( 0 != geoSvc    () ) { geoSvc    ()->release() ; m_GiGaGeomCnvSvc = 0 ; } 
  if( 0 != cnvSvc    () ) { cnvSvc    ()->release() ; m_GiGaCnvSvc     = 0 ; } 
  ///
  m_leaves.clear();
  ///
  return Converter::finalize() ; 
  ///
};

/// ===========================================================================
/// ===========================================================================
StatusCode GiGaCnvBase::declareObject( const std::string & Path  ,
                                       const CLID        & Clid  ,
                                       const std::string & Addr1 ,
                                       const std::string & Addr2 )
{ 
  m_leaves.push_back( IGiGaCnvSvc::Leaf( Path ,  Clid , Addr1 , Addr2 ) );
  return StatusCode::SUCCESS; 
};

/// ===========================================================================











