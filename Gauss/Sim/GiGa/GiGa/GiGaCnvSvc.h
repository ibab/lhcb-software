#ifndef      __GIGA_CONVERSION_GIGACNVSVC_H__ 
#define      __GIGA_CONVERSION_GIGACNVSVC_H__  1 

///
/// from STL 
#include <string> 


///
/// from Gaudi - base class 
#include "GaudiKernel/ConversionSvc.h"
///
/// from Gaudi - interfaces 
#include "GaudiKernel/IGiGaSvc.h" 
#include "GaudiKernel/IGiGaSetUpSvc.h" 
#include "GaudiKernel/IParticlePropertySvc.h" 
#include "GaudiKernel/ISvcLocator.h" 
#include "GaudiKernel/IDataProviderSvc.h" 



///
///
#include "GiGa/IGiGaCnvSvc.h" 

///
/// forward declarations
///

template <class SERVICE> class SvcFactory; 

class    IGiGaSvc; 
class    IParticlePropertySvc; 
class    IDataSelector; 

///
///  GiGaCnvSvc: base conversion service  for converting of Gaudi 
///                  structures into Geant4 primary event record  
///  
///  Author: Vanya Belyaev 
///  Date    7 Aug 2000 


class GiGaCnvSvc:  public          ConversionSvc, 
                   virtual public  IGiGaCnvSvc      
{
  ///
  /// 
  ///  
 public:
  
  ///   
  /// constructor 
  ///
  GiGaCnvSvc( const std::string&   ServiceName       , 
	      ISvcLocator*         ServiceLocator    ,
	      const unsigned int   StorageType       )
    : ConversionSvc(               ServiceName       , 
		                   ServiceLocator    , 
		                   StorageType       )  
    ///
    , m_NameOfDataProviderSvc     ( "EventDataSvc"        )
    , m_DataProviderSvc           (     0                 )
    ///
    , m_NameOfGiGaSvc             ( "GiGaSvc"             ) 
    , m_GiGaSvc                   (     0                 ) 
    ///
    , m_NameOfGiGaSetUpSvc        ( "GiGaSvc"             ) 
    , m_GiGaSetUpSvc              (     0                 ) 
    ///
    , m_NameOfParticlePropertySvc ( "ParticlePropertySvc" )  
    , m_ParticlePropertySvc       (            0          ) 
    ///
    { 
      declareProperty   ( "NameOfDataProviderService"       , m_NameOfDataProviderSvc     );
      declareProperty   ( "NameOfGiGaService"               , m_NameOfGiGaSvc             ); 
      declareProperty   ( "NameOfGiGaSetUpService"          , m_NameOfGiGaSetUpSvc        ); 
      declareProperty   ( "NameOfParticlePropertyService"   , m_NameOfParticlePropertySvc );
    };
  
  /// 
  /// virtual destructor
  ///
  
  virtual ~GiGaCnvSvc(){} 
  
 public: 
  
  inline virtual StatusCode              initialize (); 
  inline virtual StatusCode              finalize   ();
  inline virtual StatusCode              queryInterface( const IID& , void** );

  inline virtual StatusCode              createReps(IDataSelector* pSelector) ;
 
  virtual        IGiGaSvc*               gigaSvc  () { return m_GiGaSvc             ; }  
  virtual        IGiGaSetUpSvc*          setupSvc () { return m_GiGaSetUpSvc        ; } 
  virtual        IParticlePropertySvc*   ppSvc    () { return m_ParticlePropertySvc ; } 

 protected:

  inline StatusCode Error( const std::string& Message , const StatusCode& status = StatusCode::FAILURE ); 

  void setNameOfDataProviderSvc( const std::string& NameOfDataProviderSvc ) 
    { m_NameOfDataProviderSvc = NameOfDataProviderSvc; }  
   
 private: 
  
  ///
  ///
  ///
  
  std::string                          m_NameOfDataProviderSvc       ; 
  IDataProviderSvc*                    m_DataProviderSvc             ; 

  std::string                          m_NameOfGiGaSvc               ; 
  IGiGaSvc*                            m_GiGaSvc                     ; 
  
  std::string                          m_NameOfGiGaSetUpSvc          ; 
  IGiGaSetUpSvc*                       m_GiGaSetUpSvc                ; 
  
  std::string                          m_NameOfParticlePropertySvc   ; 
  IParticlePropertySvc*                m_ParticlePropertySvc         ;
  
  ///
  ///  
};        


///
///
///

inline StatusCode GiGaCnvSvc::initialize()
{
  ///
  StatusCode status = ConversionSvc::initialize() ; 
  ///  
  if( status.isFailure()     )  { return Error("Initialize::Could not initialize base class ConversionSvc", status); } 
  ///
  { MsgStream log(msgSvc() , name() ); log << MSG::VERBOSE << "Initialize::begin" << endreq; }
  ///
  IDataProviderSvc* m_DataProviderSvc = 0 ; 
  status = serviceLocator()->getService( m_NameOfDataProviderSvc          , 
					 IID_IDataProviderSvc             , 
					 (IInterface*&) m_DataProviderSvc ) ; 
  if( status.isFailure()     ) { return Error("Initialize::Could not locate IDataProvider="+m_NameOfDataProviderSvc, status );}      
  if( 0 == m_DataProviderSvc ) { return Error("Initialize::Could not locate IDataProvider="+m_NameOfDataProviderSvc         );}      
  ///
  setStore( m_DataProviderSvc ); 
  ///
  status = serviceLocator()->getService( m_NameOfGiGaSvc          , 
					 IID_IGiGaSvc             , 
					 (IInterface*&) m_GiGaSvc ) ; 
  ///
  if( status.isFailure()     ) { return Error("Initialize::Could not locate IGiGaSvc="+m_NameOfGiGaSvc, status );}      
  if( 0 == m_GiGaSvc         ) { return Error("Initialize::Could not locate IGiGaSvc="+m_NameOfGiGaSvc         );}      
  ///
  ///
  status = serviceLocator()->getService( m_NameOfGiGaSetUpSvc          , 
					 IID_IGiGaSetUpSvc             , 
					 (IInterface*&) m_GiGaSetUpSvc ) ; 
  ///
  if( status.isFailure()     ) { return Error("Initialize::Could not locate IGiGaSetUpSvc="+m_NameOfGiGaSetUpSvc, status );}      
  if( 0 == m_GiGaSetUpSvc    ) { return Error("Initialize::Could not locate IGiGaSetUpSvc="+m_NameOfGiGaSetUpSvc         );}      
  ///
  ///
  status = serviceLocator()->getService( m_NameOfParticlePropertySvc          , 
					 IID_IParticlePropertySvc             , 
					 (IInterface*&) m_ParticlePropertySvc ) ; 
  ///
  if( status.isFailure()     ) { return Error("Initialize::Could not locate IParticlePropertySvc="+m_NameOfParticlePropertySvc, status );}      
  if( 0 == m_GiGaSetUpSvc    ) { return Error("Initialize::Could not locate IParticlePropertySvc="+m_NameOfParticlePropertySvc         );}      
  ///
  { MsgStream log(msgSvc() , name() ); log << MSG::VERBOSE << "Initialize::end" << endreq; }
  ///
  return StatusCode::SUCCESS; 
  ///
};

///
///
///

inline StatusCode GiGaCnvSvc::finalize()
{
  ///
  StatusCode st = ConversionSvc::finalize(); 
  ///
  return st; 
  ///
};

///
///
///

StatusCode GiGaCnvSvc::queryInterface( const IID& iid , void** ppInterface )
{ 
  ///
  if( 0 == ppInterface        ) { return StatusCode::FAILURE                                ; } 
  if( IID_IGiGaCnvSvc  == iid ) { *ppInterface = static_cast<IGiGaCnvSvc*>(this)            ; } 
  else                          { return ConversionSvc::queryInterface( iid , ppInterface ) ; } 
  ///
  addRef();
  ///
  return StatusCode::SUCCESS;  
  ///
};

///
///
///

StatusCode GiGaCnvSvc::createReps(IDataSelector* pSelector)
{
  ///
  { MsgStream log(msgSvc() , name() ); log << MSG::VERBOSE << "CreateReps::begin" << endreq; }
  ///
  if( 0 == gigaSvc  ()   ) { return Error("CreateReps::IGiGaSvc* points to NULL!"            ) ; } 
  if( 0 == setupSvc ()   ) { return Error("CreateReps::IGiGaSetUpSvc* points to NULL!"       ) ; } 
  if( 0 == ppSvc    ()   ) { return Error("CreateReps::IParticlePropertySvc* points to NULL!") ; } 
  ///
  StatusCode status = ConversionSvc::createReps( pSelector ) ;
  ///
  if( status.isFailure() ) { return Error("CreateReps::Error brom ConversionService::createReps(IDataSelector*)",status); }
  ///
  { MsgStream log(msgSvc() , name() ); log << MSG::VERBOSE << "CreateReps::end" << endreq; }
  ///
  return status; 
  ///
};

///
///
///

inline StatusCode GiGaCnvSvc::Error( const std::string& Message , const StatusCode& status )
{
  MsgStream log( msgSvc() , name() ) ; log << MSG::ERROR << Message << endreq; 
  return status; 
};

///
///
/// 

#endif  //   __GIGA_CONVERSION_GIGACNVSVC_H__ 
















