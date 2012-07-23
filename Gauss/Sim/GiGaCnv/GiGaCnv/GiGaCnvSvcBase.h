// $Id: GiGaCnvSvcBase.h,v 1.15 2008-05-30 13:13:17 gcorti Exp $ 
#ifndef      GIGA_GIGACNVSVCBASE_H 
#define      GIGA_GIGACNVSVCBASE_H  1 

// Include files
// from STL 
#include <string> 
#include <vector> 
#include <exception> 
#include <map> 
// from Gaudi - base class and interafces 
#include "GaudiKernel/IIncidentListener.h" 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/IToolSvc.h" 
#include "GaudiKernel/System.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ConversionSvc.h"
// from GiGa 
#include "GiGaCnv/IGiGaCnvSvc.h" 
#include "GiGaCnv/GiGaLeaf.h" 
#include "GiGa/IGiGaSvc.h"
#include "GiGa/IGiGaSetUpSvc.h"
#include "GaudiKernel/GaudiException.h"

// Forward declarations
class IDataProviderSvc     ;
namespace LHCb { 
  class IParticlePropertySvc ; 
}
class IMagneticFieldSvc    ; 
class IChronoStatSvc       ; 
class IToolSvc             ; 
class IIncidentSvc         ; 
class ISvcLocator          ;
//class GaudiException       ;


/** @class GiGaCnvSvc GiGaCnvSvc.h GiGaCnv/GiGaCnvSvc.h
 *
 *   Base conversion service  for converting of Gaudi 
 *   structures into Geant4 structures and vice versa  
 *   
 *   @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *   @date    07/08/2000 
 */

class GiGaCnvSvcBase: virtual public  IGiGaCnvSvc       ,
                      virtual public  IIncidentListener ,       
                      public          ConversionSvc
{
public:

  typedef std::vector<GiGaLeaf> Leaves;

protected:
  
  /** standard constructor
   *  @param ServiceName service name 
   *  @param ServiceLocator pointer to service locator 
   *  @param StorageType storage type 
   */
  GiGaCnvSvcBase( const std::string&   ServiceName       , 
                  ISvcLocator*         ServiceLocator    ,
                  const unsigned int   StorageType       ) ;

  /// virtual destructor
  virtual ~GiGaCnvSvcBase();

public: 

  /** standard initialization method 
   *  @return status code 
   */
  virtual StatusCode initialize     (); 

  /** standard finalization method 
   *  @return status code 
   */
  virtual StatusCode finalize       ();
  ///
  virtual StatusCode queryInterface ( const InterfaceID& , void**  );
  ///
  virtual StatusCode declareObject  ( const GiGaLeaf& Leaf );
  ///
  virtual void       handle         ( const Incident&      );
  ///
  /// GiGa service                 (mandatory)
  virtual IGiGaSvc* gigaSvc   ()  const {
    if( m_gigaSvc == 0 ) {
      StatusCode status = serviceLocator()->service( m_gigaName, m_gigaSvc, 
                                                     true );
      if( status.isFailure() ) { 
        //FIXME Throw exception
        Error("Initialize::Could not locate IGiGaSvc=" + m_gigaName, status );
      }       
      if( 0 == gigaSvc() ) {
        Error("Initialize::Could not locate IGiGaSvc=" + m_gigaName );
      }
      Print( " Located GiGa Service="+m_gigaName, MSG::VERBOSE ); 
    } 
    return m_gigaSvc; 
  }  
  
  /// GiGaSetUp service            (mandatory)
  virtual IGiGaSetUpSvc*    setupSvc  ()  const {
    
    if( m_setupSvc == 0 ) {
      StatusCode status = serviceLocator()->service( m_setupName, m_setupSvc, 
                                                     true );
      if( status.isFailure() ){ 
        Error("Initialize::Could not locate IGiGaSetUpSvc=" + 
              m_setupName, status );
      }      
      if( 0 == setupSvc() ) { 
        Error("Initialize::Could not locate IGiGaSetUpSvc=" + 
              m_setupName );
      }
      Print( " Located GiGa SetUp Service="+m_setupName, MSG::VERBOSE ); 
    } 
    return m_setupSvc;
  } 
  
  /** register all declared leaves 
   *  @return status code 
   */
  virtual StatusCode registerGiGaLeaves() ;
  
  /** Create a Generic address using explicit arguments to 
   *                              identify a single object.
   *  @param      svc_type    Technology identifier encapsulated 
   *                          in this address.
   *  @param      clid        Class identifier of the DataObject 
   *                          represented by the opaque address
   *  @param      par         Array of strings needed to 
   *                          construct the opaque address.
   *  @param      ipar        Array of integers needed to 
   *                          construct the opaque address.
   *  @param      address     Reference to pointer to the address 
   *                          where the created Address should be stored.
   *  @return     Status code indicating success or failure.
   */
  virtual StatusCode createAddress
  ( long                  svc_type ,
    const CLID&           clid     ,
    const std::string*    par      , 
    const unsigned long*  ip       ,
    IOpaqueAddress*&      address  ) ;
  
protected:
  ///
  /// "main" data provider         (mandatory) 
  inline IDataProviderSvc* dpSvc     ()  const { return m_dpSvc       ; }  
  ///  event data provider         (optional) 
  inline IDataProviderSvc* evtSvc    ()  const { return m_evtSvc      ; }  
  /// detector data provider       (optional)
  inline IDataProviderSvc* detSvc    ()  const { return m_detSvc      ; }  
  /// chrono  service              (optional)
  inline IChronoStatSvc*   chronoSvc ()  const { return m_chronoSvc   ; } 
  /// tool    service              (optional)
  inline IToolSvc*         toolSvc   ()  const { return m_toolSvc     ; } 
  /// incident service                          
  inline IIncidentSvc*     incSvc    ()  const { return m_incSvc      ; } 
  /// service locator                           
  inline ISvcLocator*      svcLoc    ()  const { return serviceLocator() ; } 
  ///
  StatusCode    setNameOfDataProviderSvc( const std::string& Name ) 
  { m_dpName = Name ; return StatusCode::SUCCESS ; } 
  ///
//   StatusCode locateOwnCnvs();

  /** @brief templated  access to the data in Gaudi transient store 
   * 
   *  Quick and safe access to the data in Gaudi transient store.
   *  The method located the data at given address and perform the 
   *  debug printout about located data
   *    
   *  Usage:
   *
   *  - 
   *  MCCaloDigits*        digits = get( evtSvc () , inputData () , digits );
   *  if( 0 == digits ) { return StatusCode::FAILURE ;}
   *
   *  -
   *  const DeCalorimeter* det    = get( detSvc () , detData   () , det    );
   *  if( 0 == det ) { return StatusCode::FAILURE ;}
   *
   *  @warning the third argument is artificial to please MicroSoft
   *           stupid compiler!
   *
   *  @see IDataProviderSvc
   *  @see SmartDataPtr
   *  @exception GiGaException for Invalid Data Provider Service 
   *  @exception GiGaException for invalid/unavailable  data  
   *  @param svc pointer to data service (data provider)
   *  @param location data location/address in Gaudi Transient Store 
   *  @param type artificial algument (to please MicroSoft compiler)
   */
  template<class TYPE>
  TYPE* 
  get
  ( IDataProviderSvc*  svc        , 
    const std::string& location   ,
    const TYPE*        /* type */ ) const 
  {
    // check the environment 
    Assert( 0 != svc   , "  get():: IDataProvider* points to NULL!"      );
    SmartDataPtr<TYPE> object( svc, location ) ;
    Assert( !(!object) ,  " get():: No valid data at '" + location + "'" );
    TYPE* aux = object ;
    Assert( 0 != aux   ,  " get():: No valid data at '" + location + "'" );
    const std::string type( System::typeinfoName( typeid( *aux ) ) );
    // debug printout 
    // return located valid data 
    Print( " The data from address '"     + location + 
           "' are retrieved from TS "     , 
           MSG::DEBUG     ,     StatusCode::SUCCESS  ) ;
    Print( " [ The actual data type is '" + type     + 
           "' ] "                         ,
           MSG::VERBOSE   ,     StatusCode::SUCCESS  ) ;
    return aux ;
  };

  /** the useful method for location of tools. 
   *  for empty "name" delegates to another method  
   *  @see IToolSvc
   *  @see IAlgTool
   *  @attention do not forget to 'release' tool after the usage!
   *  @exception GiGaException for invalid Tool Service 
   *  @exception GiGaException for error from Tool Service 
   *  @exception GiGaException for invalid tool 
   *  @param type   tool type 
   *  @param name   tool name
   *  @param Tool   tool itself (return) 
   *  @param parent tool parent
   *  @param create flag for creation of nonexisting tools 
   *  @return pointer to the tool
   */
  template<class TOOL>
  TOOL* 
  tool
  ( const std::string& type           , 
    const std::string& name           , 
    TOOL*&             Tool           , 
    const IInterface*  parent  = 0    , 
    bool               create  = true ) const 
  {
    // for empty names delegate to another method 
    if( name.empty() ) { return tool( type , Tool , parent , create ) ; }
    Assert( 0 != toolSvc() , "IToolSvc* points toNULL!" );
    // get the tool from Tool Service 
    StatusCode sc = toolSvc () 
      -> retrieveTool ( type , name , Tool, parent , create );
    Assert( sc.isSuccess() , 
            "Could not retrieve Tool'" + type + "'/'" + name + "'", sc ) ;
    Assert( 0 != Tool      , 
            "Could not retrieve Tool'" + type + "'/'" + name + "'"     ) ;
    // debug printout 
    Print( " The Tool of type '" + Tool->type() + 
           "'/'"                 + Tool->name() + 
           "' is retrieved from IToolSvc " , MSG::DEBUG  , sc ) ;
    // return located tool 
    return Tool ;
  };

  /** the useful method for location of tools.
   *  @see IToolSvc
   *  @see IAlgTool
   *  @attention do not forget to 'release' tool after the usage!
   *  @exception GiGaException for invalid Tool Service 
   *  @exception GiGaException for error from Tool Service 
   *  @exception GiGaException for invalid tool 
   *  @param type   tool type, could be of "Type/Name" format 
   *  @param Tool   tool itself (return)
   *  @param parent tool parent
   *  @param create flag for creation of nonexisting tools 
   *  @return pointer to the tool
   */
  template<class TOOL>
  TOOL* 
  tool
  ( const std::string& type          , 
    TOOL*&             Tool          , 
    const IInterface*  parent = 0    , 
    bool               create = true ) const
  {
    // check the environment 
    Assert( 0 != toolSvc() , "IToolSvc* points toNULL!" );
    // "type" or "type/name" ?
    std::string::const_iterator it = 
      std::find( type.begin() , type.end () , '/' );
    // "type" is compound!
    if( type.end() != it ) 
      {
        std::string::size_type pos = it - type.begin()   ;
        const std::string ntype( type , 0       , pos               );
        const std::string nname( type , pos + 1 , std::string::npos );
        return tool( ntype , // new type 
                     nname , // new name 
                     Tool , parent , create            ) ;
      }
    // retrieve the tool from Tool Service 
    StatusCode sc = toolSvc () 
      -> retrieveTool ( type , Tool, parent , create   );
    Assert( sc.isSuccess() , "Could not retrieve Tool'" + type + "'" , sc ) ;
    Assert( 0 != Tool      , "Could not retrieve Tool'" + type + "'"      ) ;
    // debug printout 
    Print( " The Tool of type '" + Tool->type() + 
           "'/'"                 + Tool->name() + 
           "' is retrieved from IToolSvc " , MSG::DEBUG , sc ) ;
    // return located tool 
    return Tool ;
  };
  
  /** helpful method for error count and printout 
   *  @param msg message to be printed 
   *  @param sc  status code to be returned 
   *  @return status code 
   */
  StatusCode Error     
  ( const std::string    & msg                       ,  
    const StatusCode     & sc  = StatusCode::FAILURE ) const ;

  /** helpful method for warning count and printout 
   *  @param msg message to be printed 
   *  @param sc  status code to be returned 
   *  @return status code 
   */
  StatusCode Warning   
  ( const std::string    & msg                       ,  
    const StatusCode     & sc  = StatusCode::FAILURE ) const ;
  
  /** helpful method for  printout 
   *  @param msg message to be printed 
   *  @param lvl printout level 
   *  @param sc  status code to be returned 
   *  @return status code 
   */
  StatusCode Print     
  ( const std::string    & msg                       ,  
    const MSG::Level     & lvl = MSG::INFO           ,
    const StatusCode     & sc  = StatusCode::FAILURE ) const ;
  
  /** assertion 
   *  @param assertion   assertion condition
   *  @param msg         assertion message 
   *  @param sc          assertion status code 
   *  @return status code 
   */
  inline StatusCode Assert
  ( bool  assertion                                    , 
    const std::string& msg = "GiGaCnvSvcBase::unknown" , 
    const StatusCode&  sc  = StatusCode::FAILURE       ) const ;
  
  /** assertion 
   *  @param assertion   assertion condition
   *  @param msg         assertion message 
   *  @param sc          assertion status code 
   *  @return status code 
   */
  inline StatusCode Assert
  ( bool  assertion                              ,
    const char*        msg                       ,
    const StatusCode&  sc  = StatusCode::FAILURE ) const ;
  
  /** exception handling 
   *  @param msg message to be assosiated with the exception 
   *  @param exc "previous" exception
   *  @param lvl printout level 
   *  @param sc  status code to be "returned" (fictive)
   *  @return status code (fictive)
   */
  StatusCode Exception 
  ( const std::string    & msg                        ,  
    const GaudiException & exc                        , 
    const MSG::Level     & lvl = MSG::FATAL           ,
    const StatusCode     & sc  = StatusCode::FAILURE ) const ;

  /** exception handling 
   *  @param msg message to be assosiated with the exception 
   *  @param exc "previous" exception
   *  @param lvl printout level 
   *  @param sc  status code to be "returned" (fictive)
   *  @return status code (fictive)
   */
  StatusCode Exception 
  ( const std::string    & msg                        ,  
    const std::exception & exc                        , 
    const MSG::Level     & lvl = MSG::FATAL           ,
    const StatusCode     & sc  = StatusCode::FAILURE ) const ;
  
  /** exception handling 
   *  @param msg message to be assosiated with the exception 
   *  @param lvl printout level 
   *  @param sc  status code to be "returned" (fictive)
   *  @return status code (fictive)
   */
  StatusCode Exception 
  ( const std::string    & msg                        ,  
    const MSG::Level     & lvl = MSG::FATAL           ,
    const StatusCode     & sc  = StatusCode::FAILURE ) const ;
  ///
private: 
  ///
  GiGaCnvSvcBase() ; ///< no default constructor  
  GiGaCnvSvcBase           ( const GiGaCnvSvcBase& ) ; //< no copy
  GiGaCnvSvcBase& operator=( const GiGaCnvSvcBase& ) ; //< no assignment 
  ///
protected:
  ///
  Leaves                               m_leaves     ; 
  ///
private: 
  ///
  std::string                          m_dpName     ; 
  IDataProviderSvc*                    m_dpSvc      ; 
  ///
  std::string                          m_evtName    ; 
  IDataProviderSvc*                    m_evtSvc     ; 
  ///
  std::string                          m_detName    ; 
  IDataProviderSvc*                    m_detSvc     ; 
  ///
  std::string                          m_gigaName   ; 
  mutable IGiGaSvc*                    m_gigaSvc    ; 
  ///
  std::string                          m_setupName  ; 
  mutable IGiGaSetUpSvc*               m_setupSvc   ; 
  ///
  std::string                          m_chronoName ; 
  IChronoStatSvc*                      m_chronoSvc  ;
  ///
  std::string                          m_toolName   ; 
  IToolSvc*                            m_toolSvc    ;
  ///
  std::string                          m_inName     ; 
  IIncidentSvc*                        m_incSvc     ;
  /// 
  typedef std::map<std::string,unsigned int> Counter;
  /// counter of errors 
  mutable Counter m_errors     ;
  /// counter of warning 
  mutable Counter m_warnings   ; 
  /// counter of exceptions
  mutable Counter m_exceptions ;
};        

// ============================================================================
/** assertion 
 *  @param assertion   assertion condition
 *  @param msg         assertion message 
 *  @param sc          assertion status code 
 */
// ============================================================================
inline StatusCode  GiGaCnvSvcBase::Assert
( bool               assertion , 
  const std::string& msg       , 
  const StatusCode&  sc        ) const 
{  
  StatusCode status = StatusCode::SUCCESS ;
  return (assertion) ? status : Exception( msg , MSG::FATAL , sc ) ;
};
// ============================================================================

// ============================================================================
/** assertion 
 *  @param assertion   assertion condition
 *  @param msg         assertion message 
 *  @param sc          assertion status code 
 */
// ============================================================================
inline StatusCode  GiGaCnvSvcBase::Assert
( bool               assertion , 
  const char*        msg       , 
  const StatusCode&  sc        ) const 
{ 
  StatusCode status = StatusCode::SUCCESS ;
  return (assertion) ? status : Exception( msg , MSG::FATAL , sc ) ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif  ///<   GIGACNV_GIGACNVSVCBASE_H 
// ============================================================================
















