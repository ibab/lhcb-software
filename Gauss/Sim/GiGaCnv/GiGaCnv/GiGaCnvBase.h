// $Id: GiGaCnvBase.h,v 1.8 2002-05-07 12:24:50 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef GIGA_GIGACNVBASE_H 
#define GIGA_GIGACNVBASE_H  1 
// ============================================================================
// STL 
#include <string> 
#include <vector> 
#include <map> 
#include <exception> 
// Gaudi base type  
#include "GaudiKernel/Converter.h" 
#include "GaudiKernel/MsgStream.h" 
// GiGaCnv 
#include "GiGaCnv/IGiGaCnvSvc.h" 
#include "GiGaCnv/IGiGaKineCnvSvc.h" 
#include "GiGaCnv/GiGaLeaf.h" 
//
class IDataProviderSvc     ;
class IMessageSvc          ; 
class IChronoStatSvc       ; 
class IToolSvc             ; 
class IParticlePropertySvc ; 
class IGiGaSvc             ;   
class IGiGaSetUpSvc        ;
//
class GaudiException       ; 
//
class IGiGaGeomCnvSvc      ;
class IGiGaHitsCnvSvc      ; 
//

/** @class GiGaCnvBase  GiGaCnvBase.h GiGaCnv/GiGaCnvBase.h
 *
 *  Base class for  converters from Geant4 to Gaudi and vice versa  
 *  
 *  @author  Vanya Belyaev
 *   @date    21/02/2001
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
  ( const unsigned char  StorageType , 
    const CLID&          ClassType   , 
    ISvcLocator*         Locator     );

  /// virtual destructor 
  virtual ~GiGaCnvBase();
  ///
public: 
  
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
  inline IParticlePropertySvc* ppSvc     () const 
  { return 0 != kineSvc() ? kineSvc()->ppSvc   () : 0 ; }
  
  /** print the  message and return status code
   *  @param msg error message 
   *  @param lvl print level 
   *  @param sc  status code
   *  @return statsu code 
   */
  StatusCode Print
  ( const std::string& Message , 
    const MSG::Level & level   , 
    const StatusCode & Status  ) const ;
  
  /** print and return the error
   *  @param Message message to be printed 
   *  @param status  status code to be returned 
   *  @return status code 
   */
  StatusCode Error     
  ( const std::string& Message , 
    const StatusCode& status = StatusCode::FAILURE ) const ;
  
  /** print and return warning
   *  @param Message message to be printed 
   *  @param status  status code to be returned 
   *  @return status code 
   */
  StatusCode Warning     
  ( const std::string& Message , 
    const StatusCode& status = StatusCode::FAILURE ) const ;
  
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















