/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
#ifndef GIGA_GIGACNVBASE_H 
#define GIGA_GIGACNVBASE_H  1 
/// ===========================================================================
// STL 
#include <string> 
#include <vector> 
#include <exception> 
// Gaudi base type  
#include "GaudiKernel/Converter.h" 
#include "GaudiKernel/MsgStream.h" 
// GiGa 
#include "GiGa/IGiGaCnvSvc.h"
#include "GiGa/IGiGaKineCnvSvc.h"
// GiGaCnv 
//
class IDataProviderSvc     ;
class IMessageSvc          ; 
class IChronoStatSvc       ; 
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
    
    Base class for  converters from Geant4 to Gaudi and vice versa  
    
    @author  Vanya Belyaev
    @date    21/02/2001
*/

class GiGaCnvBase: public Converter 
{
  ///  
 protected: 
  ///
  GiGaCnvBase( const unsigned char  StorageType , 
               const CLID&          ClassType   , 
               ISvcLocator*         Locator     );
  ///
  virtual ~GiGaCnvBase();
  ///
public: 
  ///
  virtual StatusCode initialize () ; 
  virtual StatusCode finalize   () ;
  ///  
protected: 
  ///
  inline IGiGaCnvSvc*          cnvSvc    () const 
  { return m_GiGaCnvSvc                               ; } 
  inline IGiGaGeomCnvSvc*      geoSvc    () const 
  { return m_GiGaGeomCnvSvc                           ; } 
  inline IGiGaKineCnvSvc*      kineSvc   () const 
  { return m_GiGaKineCnvSvc                           ; } 
  inline IGiGaHitsCnvSvc*      hitsSvc   () const 
  { return m_GiGaHitsCnvSvc                           ; } 
  inline IDataProviderSvc*     evtSvc    () const 
  { return m_evtSvc                                   ; }   
  inline IDataProviderSvc*     detSvc    () const 
  { return m_detSvc                                   ; }   
  inline IChronoStatSvc*       chronoSvc () const 
  { return m_chronoSvc                                ; } 
  inline IGiGaSvc*             gigaSvc   () const 
  { return 0 != cnvSvc () ? cnvSvc ()->gigaSvc () : 0 ; } 
  inline IGiGaSetUpSvc*        setupSvc  () const 
  { return 0 != cnvSvc () ? cnvSvc ()->setupSvc() : 0 ; } 
  inline IParticlePropertySvc* ppSvc     () const 
  { return 0 != kineSvc() ? kineSvc()->ppSvc   () : 0 ; } 
  ///  
  StatusCode Error     ( const std::string& Message , 
                         const StatusCode& status = StatusCode::FAILURE );
  ///
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
  inline const std::string&  name     () { return m_ConverterName; } 
  ///
  inline void setNameOfGiGaConversionService( const std::string& CnvSvc ) 
  { m_NameOfGiGaConversionService = CnvSvc ; }                     
  ///
  void setConverterName ( const std::string& ConverterName ) 
{ m_ConverterName  = ConverterName ; } 
  ///
  StatusCode declareObject( const std::string & Path       ,
                            const CLID        & Clid       ,
                            const std::string & Addr1 = "" ,
                            const std::string & Addr2 = "");
  /// 
protected: 
  ///
  std::string           m_NameOfGiGaConversionService ; 
  std::string           m_ConverterName               ;
  ///
  IGiGaCnvSvc::Leaves   m_leaves                      ;
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
  ///
};
///

 
/// ===========================================================================
#endif    //      GIGA_GIGACNVBASE_H 
/// ===========================================================================















