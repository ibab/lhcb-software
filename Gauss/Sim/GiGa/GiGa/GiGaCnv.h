#ifndef           __GIGA_CONVERSION_GIGACNV_H__ 
#define           __GIGA_CONVERSION_GIGACNV_H__  1 


///
/// from STL 
#include <string> 
#include <vector> 

///
/// from Gaudi base type 
 
#include "GaudiKernel/Converter.h" 
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IService.h"
#include "GaudiKernel/IGiGaSvc.h" 
#include "GaudiKernel/IGiGaSetUpSvc.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Stat.h"
///
#include "GiGa/IGiGaCnvSvc.h" 
#include "GiGa/IGiGaGeomCnvSvc.h" 
#include "GiGa/IGiGaKineCnvSvc.h" 

class GiGaCnv: public Converter 
{
  ///  
protected: 
  ///
  GiGaCnv( const unsigned char  StorageType , 
	   const CLID&          ClassType   , 
	   ISvcLocator*         Locator     ) ; 
  ///
  virtual ~GiGaCnv();
  ///
  
public: 
  ///
  
  virtual StatusCode initialize () ; 
  virtual StatusCode finalize   () ;
  ///  
  virtual StatusCode createObj( IOpaqueAddress* /* Address */, DataObject*&     /* Object  */) { return StatusCode::SUCCESS; } ;
  virtual StatusCode updateObj( IOpaqueAddress* /* Address */, DataObject*      /* Object  */) { return StatusCode::SUCCESS; } ; 
  virtual StatusCode createRep( DataObject*     /* Object  */, IOpaqueAddress*& /* Address */) { return StatusCode::SUCCESS; } ;
  virtual StatusCode updateRep( DataObject*     /* Object  */, IOpaqueAddress*  /* Address */) { return StatusCode::SUCCESS; } ; 
  ///  
protected: 
  ///
  IGiGaCnvSvc*               cnvSvc    () { return m_GiGaCnvSvc                               ; } 
  IGiGaGeomCnvSvc*           geoSvc    () { return m_GiGaGeomCnvSvc                           ; } 
  IGiGaKineCnvSvc*           kineSvc   () { return m_GiGaKineCnvSvc                           ; } 
  IDataProviderSvc*          evtSvc    () { return m_evtSvc                                   ; }   
  IDataProviderSvc*          detSvc    () { return m_detSvc                                   ; }   
  IChronoStatSvc*            chronoSvc () { return m_chronoSvc                                ; } 
  IGiGaSvc*                  gigaSvc   () { return 0 != cnvSvc () ? cnvSvc ()->gigaSvc () : 0 ; } 
  IGiGaSetUpSvc*             setupSvc  () { return 0 != cnvSvc () ? cnvSvc ()->setupSvc() : 0 ; } 
  IParticlePropertySvc*      ppSvc     () { return 0 != kineSvc() ? kineSvc()->ppSvc   () : 0 ; } 
  ///  
  inline StatusCode          Error       ( const std::string& Message , const StatusCode& status = StatusCode::FAILURE )
  {
    Stat stat( chronoSvc() , name() + ":Error" );
    MsgStream log( msgSvc() , name() ); log << MSG::ERROR << Message << endreq; 
    return status; 
  };
  ///
  inline StatusCode Exception ( const std::string    & msg                        ,   
                                const GaudiException & exc                        , 
                                const MSG::Level     & lvl = MSG::FATAL           ,
                                const StatusCode     & sc  = StatusCode::FAILURE );
  inline StatusCode Exception ( const std::string    & msg                        ,  
                                const std::exception & exc                        , 
                                const MSG::Level     & lvl = MSG::FATAL           ,
                                const StatusCode     & sc  = StatusCode::FAILURE );
  inline StatusCode Exception ( const std::string    & msg                        ,  
                                const MSG::Level     & lvl = MSG::FATAL           ,
                                const StatusCode     & sc  = StatusCode::FAILURE );
  ///
  inline const std::string&  name     () { return m_ConverterName; } 
  ///
  void setNameOfGiGaConversionService( const std::string& NameOfGiGaConversionService ) 
  { m_NameOfGiGaConversionService = NameOfGiGaConversionService ; }                     
  ///
  void setConverterName                    ( const std::string& ConverterName ) { m_ConverterName  = ConverterName ; } 
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
private:
  ///
  IGiGaCnvSvc*          m_GiGaCnvSvc                  ; 
  IGiGaGeomCnvSvc*      m_GiGaGeomCnvSvc              ; 
  IGiGaKineCnvSvc*      m_GiGaKineCnvSvc              ; 
  ///  
  IDataProviderSvc*     m_evtSvc                      ; 
  IDataProviderSvc*     m_detSvc                      ; 
  IChronoStatSvc*       m_chronoSvc                   ; 
  ///
};
///
inline StatusCode GiGaCnv::Exception( const std::string    & Message , 
				      const GaudiException & Excp    ,
				      const MSG::Level     & level   , 
				      const StatusCode     & Status )
{
  Stat stat( chronoSvc() , Excp.tag() );
  MsgStream log( msgSvc() , name() + ":"+Excp.tag() ); 
  log << level << Message << ":" << Excp << endreq ; 
  return  Status;
};  
/////////////////////////////////////////////////////////////////////////////////// 
inline StatusCode GiGaCnv::Exception( const std::string    & Message , 
				      const std::exception & Excp    ,
				      const MSG::Level     & level   , 
				      const StatusCode     & Status )
{
  Stat stat( chronoSvc() , "std::exception" );
  MsgStream log( msgSvc() , name() + ":std::exception" ); 
  log << level << Message << ":" << Excp.what() << endreq ; 
  return  Status;
};  
///////////////////////////////////////////////////////////////////////////////////
inline StatusCode GiGaCnv::Exception( const std::string    & Message , 
				      const MSG::Level     & level   , 
				      const StatusCode     & Status )
{
  Stat stat( chronoSvc() , "*UNKNOWN* exception" );
  MsgStream log( msgSvc() , name() + ":UNKNOWN exception" ); 
  log << level << Message << ": UNKNOWN exception"  << endreq ; 
  return  Status;
};  
///////////////////////////////////////////////////////////////////////////////////
 
#endif    //      __GIGA_CONVERSION_GIGACNV_H__ 















