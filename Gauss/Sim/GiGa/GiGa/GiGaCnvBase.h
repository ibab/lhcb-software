#ifndef           GIGA_GIGACNVBASE_H 
#define           GIGA_GIGACNVBASE_H  1 
/// STL 
#include <string> 
#include <vector> 
#include <exception> 
/// Gaudi base type  
#include "GaudiKernel/Converter.h" 
#include "GaudiKernel/MsgStream.h" 
/// GiGa 
#include "GiGa/IGiGaCnvSvc.h"
#include "GiGa/IGiGaKineCnvSvc.h"
///
class IDataProviderSvc     ;
class IMessageSvc          ; 
class IChronoStatSvc       ; 
class IParticlePropertySvc ; 
class IGiGaSvc             ;   
class IGiGaSetUpSvc        ;
///
class GaudiException       ; 
///
class IGiGaGeomCnvSvc      ;
class IGiGaHitsCnvSvc      ; 
///

/** @class IGiGaStackAction IGiGaStackAction.h GiGa/IGiGaStackAction.h

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
  virtual ~GiGaCnvBase();
  ///
public: 
  ///
  virtual StatusCode initialize () ; 
  virtual StatusCode finalize   () ;
  ///  
protected: 
  ///
  IGiGaCnvSvc*          cnvSvc    () { return m_GiGaCnvSvc                               ; } 
  IGiGaGeomCnvSvc*      geoSvc    () { return m_GiGaGeomCnvSvc                           ; } 
  IGiGaKineCnvSvc*      kineSvc   () { return m_GiGaKineCnvSvc                           ; } 
  IGiGaHitsCnvSvc*      hitsSvc   () { return m_GiGaHitsCnvSvc                           ; } 
  IDataProviderSvc*     evtSvc    () { return m_evtSvc                                   ; }   
  IDataProviderSvc*     detSvc    () { return m_detSvc                                   ; }   
  IChronoStatSvc*       chronoSvc () { return m_chronoSvc                                ; } 
  IGiGaSvc*             gigaSvc   () { return 0 != cnvSvc () ? cnvSvc ()->gigaSvc () : 0 ; } 
  IGiGaSetUpSvc*        setupSvc  () { return 0 != cnvSvc () ? cnvSvc ()->setupSvc() : 0 ; } 
  IParticlePropertySvc* ppSvc     () { return 0 != kineSvc() ? kineSvc()->ppSvc   () : 0 ; } 
  ///  
  StatusCode Error     ( const std::string& Message , const StatusCode& status = StatusCode::FAILURE );
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
  void setNameOfGiGaConversionService( const std::string& NameOfGiGaConversionService ) 
  { m_NameOfGiGaConversionService = NameOfGiGaConversionService ; }                     
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

 
#endif    //      GIGA_GIGACNVBASE_H 















