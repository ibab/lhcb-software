// $Id: GiGaCnvBase.cpp,v 1.18 2007-11-08 15:29:36 gcorti Exp $ 

// Include files 

// from STL 
#include <string> 
#include <vector> 

// from Gaudi
#include "GaudiKernel/Converter.h" 
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Stat.h"
#include "GaudiKernel/IChronoStatSvc.h"

// from GiGa 
#include "GiGa/IGiGaSvc.h" 
#include "GiGa/IGiGaSetUpSvc.h"
#include "GiGa/GiGaException.h"
#include "GiGa/GiGaUtil.h"
#include "GiGaCnv/IGiGaCnvSvc.h" 
#include "GiGaCnv/IGiGaGeomCnvSvc.h" 
#include "GiGaCnv/IGiGaKineCnvSvc.h" 
#include "GiGaCnv/IGiGaHitsCnvSvc.h" 
#include "GiGaCnv/GiGaCnvBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GiGaCnvBase
// base class for  converters from Geant4 to Gaudi and vice versa  
//
// 2001-02-21 : Vanya Belyaev
//-----------------------------------------------------------------------------

namespace GiGaCnvBaseLocal {

 class Counter {
  public:
    // constructor 
    Counter ( const std::string& msg = " Misbalance ")
      : m_map     ()
      , m_message ( msg ) 
    {};
    // destructor 
    ~Counter() { report() ; m_map.clear() ;}
    // make the increment 
    long increment ( const CLID& object ) { return ++m_map[object] ; }
    // make the decrement 
    long decrement ( const CLID& object ) { return --m_map[object] ; }
    // current count 
    long counts    ( const CLID& object ) { return   m_map[object] ; }
    // make a report
    void report() const 
    {
      for ( Map::const_iterator entry = m_map.begin() ;
            m_map.end() != entry ; ++entry )
      {
        if( 0 == entry->second ) { continue ; }
        std::cout << "GiGaCnvBase     WARNING  "          << m_message 
                  << "'" << entry->first << "' Counts = " << entry->second  
                  << std::endl ;
      }
    };
    
  private:
    typedef std::map<CLID,long> Map;
    Map         m_map     ;
    std::string m_message ;
  };
  
  /** @var   s_Counter
   *  static instance counter 
   */
  static GiGaUtil::InstanceCounter<GiGaCnvBase> s_Counter ;
  
  /** @var s_InstanceCounter 
   *  The instance counter for all 'GaudiTool' based classes
   *  @author Vanya BELYAEV Ivan.Belyaev@Ivan.Belyaev@itep.ru
   *  @date   2004-01-19
   */
  static Counter s_InstanceCounter ( " Create/Destroy      (mis)balance " ) ;
  
  /** @var s_FinalizeCounter
   *  The initialize/finalize mismatch counter for all 'GaudiTool' based classes
   *  @author Vanya BELYAEV Ivan.Belyaev@Ivan.Belyaev@itep.ru
   *  @date   2004-01-19
   */
  static Counter s_FinalizeCounter ( " Initialize/Finalize (mis)balance " ) ;

}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GiGaCnvBase::GiGaCnvBase
//( const unsigned char  StorageType , 
( const long StorageType , 
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
  , m_toolSvc                       (  0              )
  //
  , m_local                         ( ClassType       )
  //
  , m_errors     ()
  , m_warnings   ()
  , m_exceptions ()
  ///
{
  GiGaCnvBaseLocal::s_Counter.increment () ;
  GiGaCnvBaseLocal::s_InstanceCounter.increment ( m_local ) ;
}

//=============================================================================
// Destructor 
//=============================================================================
GiGaCnvBase::~GiGaCnvBase()
{
  GiGaCnvBaseLocal::s_Counter.decrement () ;
  GiGaCnvBaseLocal::s_InstanceCounter.decrement ( m_local ) ;
}

//=============================================================================
// (re)-throw exception and print error message 
//=============================================================================
StatusCode GiGaCnvBase::Exception
( const std::string    & Message , 
  const GaudiException & Excp    ,
  const MSG::Level     & level   , 
  const StatusCode     & Status  ) const 
{
  Print( "GaudiExceptions: catch and re-throw " + Message , Status , level  );
  // increase counter of errrors 
  m_exceptions [ Message ] += 1 ;
  throw GiGaException( name() + "::" + Message , Excp , Status );
  return  Status;
}

//=============================================================================
// (re)-throw exception and print error message 
//=============================================================================
StatusCode GiGaCnvBase::Exception
( const std::string    & Message , 
  const std::exception & Excp    ,
  const MSG::Level     & level   , 
  const StatusCode     & Status  ) const 
{
  Print( "std::exception: catch and re-thrwo " + Message , Status , level );
  // increase counter of errrors 
  m_exceptions [ Message ] += 1 ;
  throw GiGaException( name() + "::" + Message + " (" + 
                       Excp.what() + ")", Status );
  return  Status;
}

//=============================================================================
// throw exception and print error message 
//=============================================================================
StatusCode GiGaCnvBase::Exception
( const std::string    & Message , 
  const MSG::Level     & level   , 
  const StatusCode     & Status  ) const 
{
  Print( "GiGaException throw " + Message ,  Status , level );
  // increase counter of errrors 
  m_exceptions [ Message ] += 1 ;
  throw GiGaException( name() + "::" + Message , Status );
  return  Status;
}

//=============================================================================
// print and return the error
//=============================================================================
StatusCode GiGaCnvBase::Error
( const std::string& msg   , 
  const StatusCode&  st    , 
  const size_t       mx   ) const 
{  
  // increase local counter of errors  
  return ( ++m_errors[msg] < mx ) ? Print( msg , st , MSG::ERROR ) : st ;
}  

//=============================================================================
// print warning message and return status code
//=============================================================================
StatusCode GiGaCnvBase::Warning
( const std::string& msg  , 
  const StatusCode & st   ,
  const size_t       mx   ) const 
{
  // increase local counter of errors  
  return ( ++m_warnings[msg] < mx ) ? Print( msg , st , MSG::WARNING ) : st ;
}

//=============================================================================
// print the  message and return status code
//=============================================================================
StatusCode GiGaCnvBase::Print
( const std::string& Message , 
  const StatusCode & Status  ,  
  const MSG::Level & level   ) const
{
  MsgStream log( msgSvc() , name() ); 
  log << level << Message << endreq   ; 
  return  Status; 
}

//=============================================================================
// initialization
//=============================================================================
StatusCode GiGaCnvBase::initialize () 
{
  StatusCode st = Converter::initialize() ; 
  if( st.isFailure() ) 
    { return Error("Can not initialize Converter base class"); } 
  ///
  {
    StatusCode sc = 
      serviceLocator() ->
      service( m_NameOfGiGaConversionService , m_GiGaCnvSvc , true ) ;
    if ( st.isFailure() ) 
      { return Error("Initialize::unable to locate IGiGaCnvSvs=" + 
                     m_NameOfGiGaConversionService, sc );} 
    if ( 0 == cnvSvc()  ) 
      { return Error("Initialize::unable to locate IGiGaCnvSvs=" + 
                     m_NameOfGiGaConversionService     );} 
  }
  ///
  m_GiGaGeomCnvSvc = dynamic_cast<IGiGaGeomCnvSvc*> ( m_GiGaCnvSvc );  
  m_GiGaKineCnvSvc = dynamic_cast<IGiGaKineCnvSvc*> ( m_GiGaCnvSvc ); 
  m_GiGaHitsCnvSvc = dynamic_cast<IGiGaHitsCnvSvc*> ( m_GiGaCnvSvc ); 
  ///
  if( 0 == geoSvc() && 0 == kineSvc() && 0 == hitsSvc() )
    { return Error("Initialize::neither Geom,Hits or Kine CnvSvc located!");} 
  ///
  if( 0 !=  geoSvc () ) {  geoSvc  () -> addRef () ; }
  if( 0 != kineSvc () ) { kineSvc  () -> addRef () ; }
  if( 0 != hitsSvc () ) { hitsSvc  () -> addRef () ; }
  ///
  {
    const std::string evtName("EventDataSvc");
    StatusCode sc = 
      serviceLocator()->service( evtName , m_evtSvc , true ) ;
    if ( st.isFailure() ) 
      { return Error("Initialize::unable to locate IDataProviderSvs=" + 
                     evtName, sc );} 
    if ( 0 == evtSvc()  ) 
      { return Error("Initialize::unable to locate IDataProviderSvs=" + 
                     evtName     );} 
  }
  ///
  {
    const std::string detName("DetectorDataSvc");
    StatusCode sc = 
      serviceLocator()->service( detName , m_detSvc , true ) ;
    if ( st.isFailure() )
      { return Error("Initialize::unable to locate IDataProviderSvs=" + 
                     detName, sc );} 
    if ( 0 == detSvc()  ) 
      { return Error("Initialize::unable to locate IDataProviderSvs=" + 
                     detName     );}
  }
  ///
  {
    const std::string chronoName("ChronoStatSvc");
    StatusCode sc = 
      serviceLocator()->service( chronoName , m_chronoSvc , true ) ;
    if ( st.isFailure()    ) 
      { return Error("Initialize::unable to locate IChronoStatSvs=" + 
                     chronoName, sc );} 
    if ( 0 == chronoSvc()  ) 
      { return Error("Initialize::unable to locate IChronoStatSvs=" + 
                     chronoName  );}
  }
  ///
  {
    const std::string toolName("ToolSvc");
    StatusCode sc = 
      serviceLocator()->service( toolName , m_toolSvc , true ) ;
    if ( st.isFailure()    ) 
      { return Error("Initialize::unable to locate IToolSvs=" + 
                     toolName, sc );} 
    if ( 0 == toolSvc()  ) 
      { return Error("Initialize::unable to locate IToolSvs=" + 
                     toolName  );}
  }
  ///
  {
    for( Leaves::const_iterator it = m_leaves.begin() ; 
         m_leaves.end() != it ; ++it )
      { cnvSvc()->declareObject( *it ); }
  }
  ///
  GiGaCnvBaseLocal::s_FinalizeCounter.increment ( m_local ) ;
  
  return StatusCode::SUCCESS ; 
}

//=============================================================================
// finalization 
//=============================================================================
StatusCode GiGaCnvBase::finalize () 
{
  /// release (in reverse order)
  if( 0 != toolSvc   () ) { toolSvc   ()->release() ; m_toolSvc        = 0 ; } 
  if( 0 != chronoSvc () ) { chronoSvc ()->release() ; m_chronoSvc      = 0 ; } 
  if( 0 != detSvc    () ) { detSvc    ()->release() ; m_detSvc         = 0 ; } 
  if( 0 != evtSvc    () ) { evtSvc    ()->release() ; m_evtSvc         = 0 ; } 
  if( 0 != cnvSvc    () ) { cnvSvc    ()->release() ; m_GiGaCnvSvc     = 0 ; } 
  ///
  if( 0 !=  geoSvc   () ) 
    {  geoSvc   () -> release () ; m_GiGaGeomCnvSvc = 0 ; }
  if( 0 != kineSvc   () ) 
    { kineSvc   () -> release () ; m_GiGaKineCnvSvc = 0 ; }
  if( 0 != hitsSvc   () ) 
    { hitsSvc   () -> release () ; m_GiGaHitsCnvSvc = 0 ; }
  ///
  m_leaves.clear();
  ///
  /// error printout 
  if( 0 != m_errors     .size() || 
      0 != m_warnings   .size() || 
      0 != m_exceptions .size()   ) 
  {      
      MsgStream log( msgSvc() , name() );
      // format printout 
      log << MSG::ALWAYS 
          << " Exceptions/Errors/Warnings statistics:  " 
          << m_exceptions .size () << "/"
          << m_errors     .size () << "/"
          << m_warnings   .size () << endreq ; 
      // print exceptions counter 
      for( Counter::const_iterator excp = m_exceptions.begin() ;
           excp != m_exceptions.end() ; ++excp )
        {
          log << MSG::ALWAYS 
              << " #EXCEPTIONS= " << excp->second  
              << " Message='"     << excp->first    << "'" << endreq ; 
        }  
      // print errors counter 
      for( Counter::const_iterator error = m_errors.begin() ;
           error != m_errors.end() ; ++error )
        {
          log << MSG::ALWAYS 
              << " #ERRORS    = " << error->second  
              << " Message='"     << error->first    << "'" << endreq ; 
        }  
      // print warnings
      for( Counter::const_iterator warning = m_warnings.begin() ;
           warning != m_warnings.end() ; ++warning )
        {
          log << MSG::ALWAYS 
              << " #WARNINGS  = " << warning->second  
              << " Message='"     << warning->first  << "'" << endreq ; 
        }  
    }
  ///
  m_errors      .clear();
  m_warnings    .clear();
  m_exceptions  .clear();
  ///
  GiGaCnvBaseLocal::s_FinalizeCounter.decrement ( m_local ) ;
  //
  return Converter::finalize() ; 
  ///
}

//=============================================================================
// declare the object to conversion service 
//=============================================================================
StatusCode GiGaCnvBase::declareObject( const GiGaLeaf& leaf )
{ 
  m_leaves.push_back( leaf );
  return StatusCode::SUCCESS; 
}

//=============================================================================
