#ifndef           __GIGA_CONVERSION_GIGACNV_H__ 
#define           __GIGA_CONVERSION_GIGACNV_H__  1 


///
/// from STL 
#include <string> 

///
/// from Gaudi base type 
#include "GaudiKernel/Converter.h" 
#include "GaudiKernel/IGiGaSvc.h" 
#include "GaudiKernel/IGiGaSetUpSvc.h" 
#include "GaudiKernel/IParticlePropertySvc.h" 


#include "GaudiKernel/MsgStream.h"

#include "GiGa/IGiGaCnvSvc.h" 


class GiGaCnv: public Converter 
{
  
 protected: 
  
  ///
  GiGaCnv( const unsigned char  StorageType , 
	   const CLID&          ClassType   , 
	   ISvcLocator*         Locator     )
    : Converter(                StorageType , 
                                ClassType   , 
                                Locator     ) 
    ///
    , m_NameOfGiGaConversionService   ( "NotDefined"  ) 
    , m_GiGaCnvSvc                    (  0            ) 
    ///
    {}; 
  
  ///  
  virtual ~GiGaCnv() {}; 
  
  ///
 public: 
  ///
  
  virtual inline StatusCode initialize () ; 
  virtual inline StatusCode finalize   () { return Converter::finalize() ; }  
  
  virtual StatusCode createObj(IOpaqueAddress* /* Address */, DataObject*&     /* Object  */) { return StatusCode::SUCCESS; } ;
  virtual StatusCode updateObj(IOpaqueAddress* /* Address */, DataObject*      /* Object  */) { return StatusCode::SUCCESS; } ; 
  virtual StatusCode createRep(DataObject*     /* Object  */, IOpaqueAddress*& /* Address */) { return StatusCode::SUCCESS; } ;
  virtual StatusCode updateRep(DataObject*     /* Object  */, IOpaqueAddress*  /* Address */) { return StatusCode::SUCCESS; } ; 
  
  ///  
 protected: 
  ///
  
  IGiGaCnvSvc*               cnvSvc   () { return m_GiGaCnvSvc; } 
  IGiGaSvc*                  gigaSvc  () { return ( 0 != cnvSvc() ) ? cnvSvc()->gigaSvc  () : 0 ; } 
  IGiGaSetUpSvc*             setupSvc () { return ( 0 != cnvSvc() ) ? cnvSvc()->setupSvc () : 0 ; }
  IParticlePropertySvc*      ppSvc    () { return ( 0 != cnvSvc() ) ? cnvSvc()->ppSvc    () : 0 ; } 
  ///  
  inline StatusCode          Error       ( const std::string& Message , const StatusCode& status = StatusCode::FAILURE );
  ///
  ///
  inline const std::string&  ConverterName  () { return m_ConverterName; } 
  ///
  ///
  void setNameOfGiGaConversionService( const std::string& NameOfGiGaConversionService ) 
    { m_NameOfGiGaConversionService = NameOfGiGaConversionService ; }                     
  ///
  ///
  void setConverterName                    ( const std::string& ConverterName ) { m_ConverterName  = ConverterName ; } 
  ///
 protected: 
  ///
  
  ///
  std::string                   m_NameOfGiGaConversionService ; 
  ///
  
  ///
  std::string                   m_ConverterName ;
  ///
  
 private:
  ///
  mutable IGiGaCnvSvc*          m_GiGaCnvSvc                  ; 
  ///  
  
};

///
///
///

inline StatusCode GiGaCnv::initialize () 
{
  StatusCode status = Converter::initialize() ; 
  if( status.isFailure() ) { return Error("Initialize::uanle to initialize Converter base class"); } 
  ///
  status = serviceLocator()->getService( m_NameOfGiGaConversionService , 
					 IID_IGiGaCnvSvc               , 
					 (IInterface*&) m_GiGaCnvSvc   ) ; 
  ///
  if ( status.isFailure() ) {return Error("Initialize::unable to locate IGiGaCnvSvs="+m_NameOfGiGaConversionService, status );} 
  if ( 0 == cnvSvc()      ) {return Error("Initialize::unable to locate IGiGaCnvSvs="+m_NameOfGiGaConversionService         );} 
  if ( 0 == gigaSvc()     ) {return Error("Initialize::unable to locate IGiGaSvc"                                           );} 
  if ( 0 == setupSvc()    ) {return Error("Initialize::unable to locate IGiGaSvc"                                           );} 
  if ( 0 == ppSvc()       ) {return Error("Initialize::unable to locate IParticlePropertySvc"                               );} 
  ///
  return StatusCode::SUCCESS ; 
};

///
///
///

inline StatusCode GiGaCnv::Error( const std::string& Message , const StatusCode& status )
{
  MsgStream log( msgSvc() , ConverterName() ); log << MSG::ERROR << Message << endreq; 
  return status; 
};

///
///
///


#endif    //      __GIGA_CONVERSION_GIGACNV_H__ 















