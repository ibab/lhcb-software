/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef      GIGA_GIGACNVSVCBASE_H 
#define      GIGA_GIGACNVSVCBASE_H  1 
/// ===========================================================================
///  STL 
#include <string> 
#include <vector> 
#include <exception> 
#include <map> 
///  Gaudi - base class and interafces 
#include "GaudiKernel/IIncidentListener.h" 
#include "GaudiKernel/ConversionSvc.h"
/// GiGa 
#include "GiGa/IGiGaCnvSvc.h" 
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
 *   @author Vanya Belyaev 
 *   @date    07/08/2000 
 */

class GiGaCnvSvcBase: virtual public  IGiGaCnvSvc       ,
                      virtual public  IIncidentListener ,       
                      public          ConversionSvc
{
  ///
protected:
  ///
public:
  /// constructor 
  GiGaCnvSvcBase( const std::string&   ServiceName       , 
                  ISvcLocator*         ServiceLocator    ,
                  const unsigned int   StorageType       ) ;
  /// virtual destructor
  virtual ~GiGaCnvSvcBase();
  ///
public: 
  ///  
  virtual StatusCode initialize     (); 
  ///
  virtual StatusCode finalize       ();
  ///
  virtual StatusCode queryInterface ( const IID& , void**  );
  ///
  virtual StatusCode declareObject ( const IGiGaCnvSvc::Leaf& Leaf );
  ///
  virtual void       handle         ( const Incident& );
  ///
protected:
  ///
  /// "main" data provider         (mandatory) 
  inline IDataProviderSvc*       dpSvc     () { return m_dpSvc       ; }  
  ///  event data provider         (optional) 
  inline IDataProviderSvc*       evtSvc    () { return m_evtSvc      ; }  
  /// detector data provider       (optional)
  inline  IDataProviderSvc*       detSvc    () { return m_detSvc      ; }  
  /// GiGa service                 (mandatory)
  inline IGiGaSvc*               gigaSvc   () { return m_gigaSvc     ; }  
  /// GiGaSetUp service            (mandatory)
  inline  IGiGaSetUpSvc*          setupSvc  () { return m_setupSvc    ; } 
  /// chrono  service              (optional)
  inline  IChronoStatSvc*         chronoSvc () { return m_chronoSvc   ; } 
  /// object manager               (optiona)
  inline  IObjManager*            objMgr    () { return m_objMgr      ; } 
  /// incident service                          
  inline  IIncidentSvc*           incSvc    () { return m_incSvc      ; } 
  /// service locator                           
  inline  ISvcLocator*            svcLoc    () { return serviceLocator() ; } 
  ///
  StatusCode    setNameOfDataProviderSvc( const std::string& Name ) 
  { m_dpName = Name ; return StatusCode::SUCCESS ; } 
  ///
  StatusCode Error     ( const std::string & msg                       ,  
                         const StatusCode  & sc  = StatusCode::FAILURE );
  StatusCode Warning   ( const std::string & msg                       ,  
                         const StatusCode  & sc  = StatusCode::FAILURE );
  StatusCode Print     ( const std::string & msg                       ,  
                         const MSG::Level  & lvl = MSG::INFO           ,
                         const StatusCode  & sc  = StatusCode::FAILURE );
  StatusCode Exception ( const std::string    & msg                        ,  
                         const GaudiException & exc                        , 
                         const MSG::Level     & lvl = MSG::FATAL           ,
                         const StatusCode     & sc  = StatusCode::FAILURE );
  StatusCode Exception ( const std::string    & msg                        ,  
                         const std::exception & exc                        , 
                         const MSG::Level     & lvl = MSG::FATAL           ,
                         const StatusCode     & sc  = StatusCode::FAILURE );
  StatusCode Exception ( const std::string    & msg                        ,  
                         const MSG::Level     & lvl = MSG::FATAL           ,
                         const StatusCode     & sc  = StatusCode::FAILURE );
  ///
private: 
  ///
  GiGaCnvSvcBase(); ///< no default constructor  
  GiGaCnvSvcBase           ( const GiGaCnvSvcBase& ) ; //< no copy
  GiGaCnvSvcBase& operator=( const GiGaCnvSvcBase& ) ; //< no assignment 
  ///
protected:
  ///
  IGiGaCnvSvc::Leaves                  m_leaves     ; 
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
};        
///
 
/// ===========================================================================
#endif  ///<   GIGACNV_GIGACNVSVCBASE_H 
/// ===========================================================================
















