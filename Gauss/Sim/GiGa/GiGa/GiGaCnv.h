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
  IGiGaSvc*                  gigaSvc   () { return 0 != cnvSvc () ? cnvSvc ()->gigaSvc () : 0 ; } 
  IGiGaSetUpSvc*             setupSvc  () { return 0 != cnvSvc () ? cnvSvc ()->setupSvc() : 0 ; } 
  IParticlePropertySvc*      ppSvc     () { return 0 != kineSvc() ? kineSvc()->ppSvc   () : 0 ; } 
  ///  
  inline StatusCode          Error       ( const std::string& Message , const StatusCode& status = StatusCode::FAILURE )
    {
      MsgStream log( msgSvc() , name() ); log << MSG::ERROR << Message << endreq; 
      return status; 
    };
  ///
  inline const std::string&  name     () { return m_ConverterName; } 
  ///
  void setNameOfGiGaConversionService( const std::string& NameOfGiGaConversionService ) 
    { m_NameOfGiGaConversionService = NameOfGiGaConversionService ; }                     
  ///
  void setConverterName                    ( const std::string& ConverterName ) { m_ConverterName  = ConverterName ; } 
  ///
 protected: 
  ///
  std::string           m_NameOfGiGaConversionService ; 
  std::string           m_ConverterName ;
  //  
 private:
  ///
  IGiGaCnvSvc*          m_GiGaCnvSvc                  ; 
  IGiGaGeomCnvSvc*      m_GiGaGeomCnvSvc              ; 
  IGiGaKineCnvSvc*      m_GiGaKineCnvSvc              ; 
  ///  
  IDataProviderSvc*     m_evtSvc                      ; 
  IDataProviderSvc*     m_detSvc                      ; 
  ///
};
///


#endif    //      __GIGA_CONVERSION_GIGACNV_H__ 















