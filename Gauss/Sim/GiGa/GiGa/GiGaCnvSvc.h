#ifndef      __GIGA_CONVERSION_GIGACNVSVC_H__ 
#define      __GIGA_CONVERSION_GIGACNVSVC_H__  1 

///
/// from STL 
#include <string> 


///
/// from Gaudi - base class 
#include "GaudiKernel/ConversionSvc.h"


class IGiGaSvc; 
class IGiGaSetUpSvc;
class IDataProviderSvc;
class IParticlePropertySvc;
class IMagneticFieldSvc;
class IObjManager;
class IChronoStatSvc;

///
#include "GiGa/IGiGaCnvSvc.h" 


/**  
     @class GiGaCnvSvc GiGaCnvSvc.h GiGa/GiGaCnvSvc.h
     
     Base conversion service  for converting of Gaudi 
     structures into Geant4 primary event record  
     
     @author Vanya Belyaev 
     @date    07/08/2000 
*/

class GiGaCnvSvc: virtual public  IGiGaCnvSvc ,       
                  public        ConversionSvc
{
  ///  
 public:

  /// constructor 
  GiGaCnvSvc( const std::string&   ServiceName       , 
	      ISvcLocator*         ServiceLocator    ,
	      const unsigned int   StorageType       );
  /// virtual destructor
  virtual ~GiGaCnvSvc();
  ///
  
 public: 
  
  ///  
  virtual StatusCode              initialize (); 
  ///
  virtual StatusCode              finalize   ();
  ///
  virtual StatusCode              queryInterface( const IID& , void** );
  ///

 protected:
  ///
  /// "main" data provider         (mandatory) 
  virtual        IDataProviderSvc*       dpSvc     () { return m_dpSvc       ; }  
  ///  event data provider         (optional) 
  virtual        IDataProviderSvc*       evtSvc    () { return m_evtSvc      ; }  
  /// detector data provider       (optional)
  virtual        IDataProviderSvc*       detSvc    () { return m_detSvc      ; }  
  /// GiGa service                 (mandatory)
  virtual        IGiGaSvc*               gigaSvc   () { return m_gigaSvc     ; }  
  /// GiGaSetUp service            (mandatory)
  virtual        IGiGaSetUpSvc*          setupSvc  () { return m_setupSvc    ; } 
  /// Magnetic field service       (optional)
  virtual        IMagneticFieldSvc*      mfSvc     () { return m_mfSvc       ; } 
  /// particle properties service  (optional)
  virtual        IParticlePropertySvc*   ppSvc     () { return m_ppSvc       ; } 
  /// chrono  service              (optional)
  virtual        IChronoStatSvc*         chronoSvc () { return m_chronoSvc   ; } 
  /// object manager               (optiona)
  virtual        IObjManager*            objMgr    () { return m_objMgr      ; } 
  ///
  StatusCode    setNameOfDataProviderSvc( const std::string& Name ) { m_dpName = Name ; } 
  ///
  inline StatusCode Error   ( const std::string & msg ,  
			      const StatusCode  & sc  = StatusCode::FAILURE );
  inline StatusCode Warning ( const std::string & msg ,  
			      const StatusCode  & sc  = StatusCode::FAILURE );
  inline StatusCode Print   ( const std::string & msg ,  
                              const MSG::Level  & lvl = MSG::INFO           ,
			      const StatusCode  & sc  = StatusCode::FAILURE );
  ///
 private: 
  ///
  GiGaCnvSvc()                               ; // no default constructor  
  GiGaCnvSvc           ( const GiGaCnvSvc& ) ; // no copy
  GiGaCnvSvc& operator=( const GiGaCnvSvc& ) ; // no assignment 
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
  std::string                          m_ppName     ; 
  IParticlePropertySvc*                m_ppSvc      ;
  /// 
  std::string                          m_mfName     ; 
  IMagneticFieldSvc*                   m_mfSvc      ;
  ///
  std::string                          m_chronoName ; 
  IChronoStatSvc*                      m_chronoSvc  ;
  ///
  std::string                          m_omName     ; 
  IObjManager*                         m_objMgr     ;
  ///
};        
///////////////////////////////////////////////////////////////////////////////////
inline StatusCode GiGaCnvSvc::Error( const std::string& Message , const StatusCode & Status )
{ return  Print( Message , MSG::ERROR  , Status  ) ; };  
///////////////////////////////////////////////////////////////////////////////////
inline StatusCode GiGaCnvSvc::Warning( const std::string& Message , const StatusCode & Status )
{ return  Print( Message , MSG::WARNING , Status ) ; };  
///////////////////////////////////////////////////////////////////////////////////
inline StatusCode GiGaCnvSvc::Print( const std::string& Message , 
				     const MSG::Level & level   , 
				     const StatusCode & Status )
{
  MsgStream log( msgSvc() , name() ); log << level << Message << endreq ; 
  return  Status;
};  
///////////////////////////////////////////////////////////////////////////////////

#endif  //   __GIGA_CONVERSION_GIGACNVSVC_H__ 
















