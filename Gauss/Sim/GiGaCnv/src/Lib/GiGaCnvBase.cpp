// $Id: GiGaCnvBase.cpp,v 1.7 2002-05-01 18:33:18 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2002/01/22 18:24:42  ibelyaev
//  Vanya: update for newer versions of Geant4 and Gaudi
//
// Revision 1.5  2001/08/12 17:24:50  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.4  2001/07/25 17:19:31  ibelyaev
// all conversions now are moved from GiGa to GiGaCnv
//
// Revision 1.3  2001/07/24 11:13:54  ibelyaev
// package restructurization(III) and update for newer GiGa
//
// Revision 1.2  2001/07/15 20:45:08  ibelyaev
// the package restructurisation
//
//=============================================================================
#define GIGACNV_GIGACNVBASE_CPP 1 
///============================================================================
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
#include "GiGa/GiGaException.h"
// GiGaCnv 
#include "GiGaCnv/IGiGaCnvSvc.h" 
#include "GiGaCnv/IGiGaGeomCnvSvc.h" 
#include "GiGaCnv/IGiGaKineCnvSvc.h" 
#include "GiGaCnv/IGiGaHitsCnvSvc.h" 
#include "GiGaCnv/GiGaCnvBase.h"


// ============================================================================
/// constructor 
// ============================================================================
GiGaCnvBase::GiGaCnvBase
( const unsigned char  StorageType , 
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
  , m_errors     ()
  , m_warnings   ()
  , m_exceptions ()
  ///
{};

// ============================================================================
/// destructor 
// ============================================================================
GiGaCnvBase::~GiGaCnvBase(){};

// ============================================================================
/** (re)-throw exception and print error message 
 *  @param msg  error message 
 *  @param exc  previous exception 
 *  @param lvl  print level
 *  @param sc   status code
 *  @return statsu code 
 */
// ============================================================================
StatusCode GiGaCnvBase::Exception
( const std::string    & Message , 
  const GaudiException & Excp    ,
  const MSG::Level     & level   , 
  const StatusCode     & Status  ) const 
{
  Stat stat( chronoSvc() , Excp.tag() );
  Print( "GaudiExceptions: catch and re-throw " + Message ,  level , Status );
  // increase counter of errrors 
  m_exceptions [ Message ] += 1 ;
  throw GiGaException( name() + "::" + Message , Excp , Status );
  return  Status;
};

// ============================================================================
/** (re)-throw exception and print error message 
 *  @param msg  error message 
 *  @param exc  previous exception 
 *  @param lvl  print level
 *  @param sc   status code
 *  @return statsu code 
 */
// ============================================================================
StatusCode GiGaCnvBase::Exception
( const std::string    & Message , 
  const std::exception & Excp    ,
  const MSG::Level     & level   , 
  const StatusCode     & Status  ) const 
{
  Stat stat( chronoSvc() , "std::exception" );
  Print( "std::exception: catch and re-thrwo " + Message ,  level , Status );
  // increase counter of errrors 
  m_exceptions [ Message ] += 1 ;
  throw GiGaException( name() + "::" + Message + " (" + 
                       Excp.what() + ")", Status );
  return  Status;
};

// ============================================================================
/** throw exception and print error message 
 *  @param msg  error message 
 *  @param lvl  print level
 *  @param sc   status code
 *  @return statsu code 
 */
// ============================================================================
StatusCode GiGaCnvBase::Exception
( const std::string    & Message , 
  const MSG::Level     & level   , 
  const StatusCode     & Status  ) const 
{
  Stat stat( chronoSvc() , "*UNKNOWN Exception*" );
  Print( "GiGaException throw " + Message ,  level , Status );
  // increase counter of errrors 
  m_exceptions [ Message ] += 1 ;
  throw GiGaException( name() + "::" + Message , Status );
  return  Status;
};

// ============================================================================
/** print and return the error
 *  @param Message message to be printed 
 *  @param status  status code to be returned 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaCnvBase::Error
( const std::string& Message , 
  const StatusCode&  status  ) const 
{  
  Stat stat( chronoSvc() ,  name() + ":Error" );
  // increase counter of errrors 
  m_errors [ Message ] += 1 ;
  return  Print( Message , MSG::ERROR  , status  ) ; 
};  

// ============================================================================
/** print warning message and return status code
 *  @param msg warning message 
 *  @param sc  status code
 *  @return statsu code 
 */
// ============================================================================
StatusCode GiGaCnvBase::Warning
( const std::string& Message , 
  const StatusCode & Status  ) const 
{
  Stat stat( chronoSvc() ,  name() + ":Warning" );
  // increase counter of errrors 
  m_warnings [ Message ] += 1 ;
  return  Print( Message , MSG::WARNING , Status ) ; 
};

// ============================================================================
/** print the  message and return status code
 *  @param msg error message 
 *  @param lvl print level 
 *  @param sc  status code
 *  @return statsu code 
 */
// ============================================================================
StatusCode GiGaCnvBase::Print
( const std::string& Message , 
  const MSG::Level & level   , 
  const StatusCode & Status  ) const 
{ 
  MsgStream log( msgSvc() , name() ); 
  log << level << Message << endreq   ; 
  return  Status; 
};

// ============================================================================
/** initialization
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaCnvBase::initialize () 
{
  StatusCode st = Converter::initialize() ; 
  if( st.isFailure() ) 
    { return Error("Initialize::uanle to initialize Converter base class"); } 
  ///
  {
    StatusCode sc = 
      serviceLocator()->
      service( m_NameOfGiGaConversionService , m_GiGaCnvSvc , true ) ;
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
    StatusCode sc = 
      serviceLocator()->service( evtName , m_evtSvc , true ) ;
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
    StatusCode sc = 
      serviceLocator()->service( detName , m_detSvc , true ) ;
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
    StatusCode sc = 
      serviceLocator()->service( chronoName , m_chronoSvc , true ) ;
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
    for( Leaves::const_iterator it = m_leaves.begin() ; 
         m_leaves.end() != it ; ++it )
      { cnvSvc()->declareObject( *it ); }
  }
  ///
  return StatusCode::SUCCESS ; 
};

// ============================================================================
/** finalization 
 *  @return status code 
 */
// ============================================================================
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
  return Converter::finalize() ; 
  ///
};

// ============================================================================
/** declare the object to conversion service 
 *  @param Path path/address in Transoent Store 
 *  @param Clid object class identifier 
 *  @param Addr1 major GiGa address 
 *  @param Addr2 minor GiGa address 
 *  @return statsu code 
 */
// ============================================================================
StatusCode GiGaCnvBase::declareObject( const GiGaLeaf& leaf )
{ 
  m_leaves.push_back( leaf );
  return StatusCode::SUCCESS; 
};

// ============================================================================
// The END 
// ============================================================================











