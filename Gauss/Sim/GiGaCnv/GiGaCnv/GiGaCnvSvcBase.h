// $Id: GiGaCnvSvcBase.h,v 1.7 2002-05-04 20:39:34 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2002/05/01 18:33:18  ibelyaev
//  import error/warning/exception counter technique from LHCb Calo
//
// Revision 1.5  2002/01/22 18:24:41  ibelyaev
//  Vanya: update for newer versions of Geant4 and Gaudi
//
// Revision 1.4  2001/08/12 17:24:49  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.3  2001/07/25 17:19:30  ibelyaev
// all conversions now are moved from GiGa to GiGaCnv
//
// Revision 1.2  2001/07/24 11:13:54  ibelyaev
// package restructurization(III) and update for newer GiGa
// 
// ============================================================================
#ifndef      GIGA_GIGACNVSVCBASE_H 
#define      GIGA_GIGACNVSVCBASE_H  1 
// ============================================================================
///  STL 
#include <string> 
#include <vector> 
#include <exception> 
#include <map> 
///  Gaudi - base class and interafces 
#include "GaudiKernel/IIncidentListener.h" 
#include "GaudiKernel/ConversionSvc.h"
/// GiGaCnv 
#include "GiGaCnv/IGiGaCnvSvc.h" 
#include "GiGaCnv/GiGaLeaf.h" 
///
class IDataProviderSvc     ; 
class IParticlePropertySvc ; 
class IMagneticFieldSvc    ; 
class IObjManager          ; 
class IChronoStatSvc       ; 
class IIncidentSvc         ; 
class IGiGaSvc             ;
class IGiGaSetUpSvc        ;
class ISvcLocator          ;
///
class GaudiException       ;


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
  ///
  typedef std::vector<GiGaLeaf> Leaves;
  ///
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
  ///
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
  virtual StatusCode queryInterface ( const IID& , void**  );
  ///
  virtual StatusCode declareObject  ( const GiGaLeaf& Leaf );
  ///
  virtual void       handle         ( const Incident&      );
  ///
  /// GiGa service                 (mandatory)
  virtual IGiGaSvc*         gigaSvc   ()  const { return m_gigaSvc     ; }  
  /// GiGaSetUp service            (mandatory)
  virtual IGiGaSetUpSvc*    setupSvc  ()  const { return m_setupSvc    ; } 
  
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
  ( unsigned char         svc_type ,
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
  /// object manager               (optiona)
  inline IObjManager*      objMgr    ()  const { return m_objMgr      ; } 
  /// incident service                          
  inline IIncidentSvc*     incSvc    ()  const { return m_incSvc      ; } 
  /// service locator                           
  inline ISvcLocator*      svcLoc    ()  const { return serviceLocator() ; } 
  ///
  StatusCode    setNameOfDataProviderSvc( const std::string& Name ) 
  { m_dpName = Name ; return StatusCode::SUCCESS ; } 
  ///
  StatusCode locateOwnCnvs();
  
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
  IGiGaSvc*                            m_gigaSvc    ; 
  ///
  std::string                          m_setupName  ; 
  IGiGaSetUpSvc*                       m_setupSvc   ; 
  ///
  std::string                          m_chronoName ; 
  IChronoStatSvc*                      m_chronoSvc  ;
  ///
  std::string                          m_omName     ; 
  IObjManager*                         m_objMgr     ;
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
















