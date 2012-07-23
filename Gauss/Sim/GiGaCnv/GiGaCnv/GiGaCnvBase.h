// $Id: GiGaCnvBase.h,v 1.14 2007-11-08 15:29:04 gcorti Exp $ 
#ifndef GIGA_GIGACNVBASE_H 
#define GIGA_GIGACNVBASE_H  1 

// Include files
// from STL 
#include <string> 
#include <vector> 
#include <map> 
#include <exception> 

// from Gaudi 
#include "GaudiKernel/IDataProviderSvc.h" 
#include "GaudiKernel/IToolSvc.h" 
#include "GaudiKernel/Converter.h" 
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h" 
#include "GaudiKernel/System.h" 

// from GiGa
#include "GiGaCnv/IGiGaCnvSvc.h" 
#include "GiGaCnv/IGiGaCnvSvcLocation.h" 
#include "GiGaCnv/IGiGaKineCnvSvc.h" 
#include "GiGaCnv/GiGaLeaf.h" 

// forward declarations
class IDataProviderSvc     ;
class IMessageSvc          ; 
class IChronoStatSvc       ; 
class IToolSvc             ; 
namespace LHCb {
  class IParticlePropertySvc ;
}
class IGiGaSvc             ;   
class IGiGaSetUpSvc        ;
//
class GaudiException       ; 
//
class IGiGaGeomCnvSvc      ;
class IGiGaHitsCnvSvc      ; 


/** @class GiGaCnvBase  GiGaCnvBase.h GiGaCnv/GiGaCnvBase.h
 *
 *  Base class for  converters from Geant4 to Gaudi and vice versa  
 *  
 *  @author  Vanya Belyaev
 *  @date    21/02/2001
 */

class GiGaCnvBase: public Converter 
{
public:
  typedef std::vector<GiGaLeaf> Leaves;
  
protected: 
  
  /** standard constructor 
   *  @param StorageType    type identifier of external storage
   *  @param ClassType      class identifier 
   *  @param Locatopr       pointer to service locator 
   */
  GiGaCnvBase
  ( const long  StorageType , 
    const CLID&          ClassType   , 
    ISvcLocator*         Locator     );
  
  /// virtual destructor 
  virtual ~GiGaCnvBase();
  ///
public: 
  long repSvcType() const
  { return i_repSvcType(); }
  
  /** initialization
   *  @see  Converter 
   *  @see IConverter 
   *  @return status code 
   */
  virtual StatusCode initialize () ; 
  
  /** finalization 
   *  @see  Converter 
   *  @see IConverter 
   *  @return status code 
   */
  virtual StatusCode finalize   () ;
  
protected: 

  /** acessor to teh service locator 
   *  @return pointer to service locator
   */
  inline ISvcLocator*          svcLoc    () const 
  { return serviceLocator ()                          ; }
  
  /** accessor to own conversion service 
   *  @return pointer to own conversion service 
   */
  inline IGiGaCnvSvc*          cnvSvc    () const 
  { return m_GiGaCnvSvc                               ; } 
  
  /** accessor to geometry conversion service 
   *  @return pointer to geometry conversion service 
   */
  inline IGiGaGeomCnvSvc*      geoSvc    () const 
  { return m_GiGaGeomCnvSvc                           ; } 
  
  /** accessor to kinematics  conversion service 
   *  @return pointer to kinematics conversion service 
   */
  inline IGiGaKineCnvSvc*      kineSvc   () const 
  { return m_GiGaKineCnvSvc                           ; } 
  
  /** accessor to hits conversion service 
   *  @return pointer to hits conversion service 
   */
  inline IGiGaHitsCnvSvc*      hitsSvc   () const 
  { return m_GiGaHitsCnvSvc                           ; }
  
  /** accessor to event data service 
   *  @return pointer to event data service 
   */ 
  inline IDataProviderSvc*     evtSvc    () const 
  { return m_evtSvc                                   ; }   
  
  /** accessor to detector  data service 
   *  @return pointer to detector  data service 
   */ 
  inline IDataProviderSvc*     detSvc    () const 
  { return m_detSvc                                   ; }
  
  
  /** accessor to Chrono & Stat service 
   *  @return pointer to Chrono & Stat service 
   */  
  inline IChronoStatSvc*       chronoSvc () const 
  { return m_chronoSvc                                ; } 

  /** accessor to Tool Service 
   *  @return pointer to Tool Service 
   */  
  inline IToolSvc*             toolSvc   () const 
  { return m_toolSvc                                  ; } 
  
  /** accessor to GiGa service
   *  @return pointer to GiGa service 
   */   
  inline IGiGaSvc*             gigaSvc   () const 
  { return 0 != cnvSvc () ? cnvSvc ()->gigaSvc () : 0 ; }

  /** accessor to GiGa SetUp service
   *  @return pointer to GiGa SetUp  service 
   */                     
  inline IGiGaSetUpSvc*        setupSvc  () const 
  { return 0 != cnvSvc () ? cnvSvc ()->setupSvc() : 0 ; } 
  
  /** accessor to Particle Properties  service
   *  @return pointer to Particle Properties   service 
   */
  inline LHCb::IParticlePropertySvc* ppSvc     () const 
  { return 0 != kineSvc() ? kineSvc()->ppSvc   () : 0 ; }


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
           StatusCode::SUCCESS ,  MSG::DEBUG ) ;
    Print( " [ The actual data type is '" + type     + 
           "' ] "                         ,
           StatusCode::SUCCESS  , MSG::DEBUG ) ;
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

  /** print and return the error
   *  @param Message message to be printed 
   *  @param status  status code to be returned 
   *  @return status code 
   */
  StatusCode Error     
  ( const std::string& Message                       , 
    const StatusCode& status   = StatusCode::FAILURE , 
    const size_t      mx       = 10                  ) const ;
  
  /** print and return warning
   *  @param Message message to be printed 
   *  @param status  status code to be returned 
   *  @return status code 
   */
  StatusCode Warning     
  ( const std::string& Message                       , 
    const StatusCode&  status  = StatusCode::FAILURE , 
    const size_t       mx      = 10                  ) const ;
  
  /** print the  message and return status code
   *  @param msg error message 
   *  @param lvl print level 
   *  @param sc  status code
   *  @return statsu code 
   */
  StatusCode Print
  ( const std::string& Message                       , 
    const StatusCode & Status  = StatusCode::SUCCESS ,
    const MSG::Level & level   = MSG::INFO           ) const  ;
    
  /** (re)-throw exception and print error message 
   *  @param msg  error message 
   *  @param exc  previous exception 
   *  @param lvl  print level
   *  @param sc   status code
   *  @return statsu code 
   */
  StatusCode Exception 
  ( const std::string    & msg                       ,   
    const GaudiException & exc                       , 
    const MSG::Level     & lvl = MSG::FATAL          ,
    const StatusCode     & sc  = StatusCode::FAILURE ) const ;
  
  /** (re)-throw exception and print error message 
   *  @param msg  error message 
   *  @param exc  previous exception 
   *  @param lvl  print level
   *  @param sc   status code
   *  @return statsu code 
   */
  StatusCode Exception 
  ( const std::string    & msg                       ,  
    const std::exception & exc                       , 
    const MSG::Level     & lvl = MSG::FATAL          ,
    const StatusCode     & sc  = StatusCode::FAILURE ) const ;
  
  /** throw exception and print error message 
   *  @param msg  error message 
   *  @param lvl  print level
   *  @param sc   status code
   *  @return statsu code 
   */
  StatusCode Exception 
  ( const std::string    & msg                       ,  
    const MSG::Level     & lvl = MSG::FATAL          ,
    const StatusCode     & sc  = StatusCode::FAILURE ) const ;
  
  /** assertion 
   *  @param assertion   assertion condition
   *  @param msg         assertion message 
   *  @param sc          assertion status code 
   *  @return status code 
   */
  inline StatusCode Assert
  ( bool  assertion                                  , 
    const std::string& msg = "GiGaCnvBase::unknown"  , 
    const StatusCode&  sc  = StatusCode::FAILURE     ) const ;
  
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
  

  /** Retrieve name of converter 
   *  @return converter name
   */
  inline const std::string&  name     () const { return m_ConverterName; } 

  /// set name of own conversion service  
  inline void setNameOfGiGaConversionService
  ( const std::string& CnvSvc ) 
  { m_NameOfGiGaConversionService = CnvSvc ; }                     
  
  /// set own name 
  void setConverterName 
  ( const std::string& ConverterName ) 
  { m_ConverterName  = ConverterName ; } 
  
  /** declare the object to conversion service 
   *  @param leaf object leaf 
   *  @return status code 
   */
  StatusCode declareObject ( const GiGaLeaf&   leaf );
  /// 
protected: 
  ///
  std::string           m_NameOfGiGaConversionService ; 
  std::string           m_ConverterName               ;
  ///
  Leaves                m_leaves                      ;
  ///
private:
  ///
  IGiGaCnvSvc*          m_GiGaCnvSvc                  ; 
  IGiGaGeomCnvSvc*      m_GiGaGeomCnvSvc              ; 
  IGiGaKineCnvSvc*      m_GiGaKineCnvSvc              ; 
  IGiGaHitsCnvSvc*      m_GiGaHitsCnvSvc              ; 
  ///  
  IDataProviderSvc*     m_evtSvc                      ; 
  IDataProviderSvc*     m_detSvc                      ; 
  IChronoStatSvc*       m_chronoSvc                   ; 
  IToolSvc*             m_toolSvc                     ; 
  ///
  CLID                  m_local ;
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
inline StatusCode  GiGaCnvBase::Assert
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
inline StatusCode  GiGaCnvBase::Assert
( bool               assertion , 
  const char*        msg       , 
  const StatusCode&  sc        ) const 
{ 
  StatusCode status = StatusCode::SUCCESS ;
  return (assertion) ? status : Exception( msg , MSG::FATAL , sc ) ;
};
// ============================================================================

// ============================================================================
// End 
// ============================================================================
#endif    //      GIGA_GIGACNVBASE_H 
// ============================================================================















