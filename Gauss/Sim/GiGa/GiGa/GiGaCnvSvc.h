#ifndef      __GIGA_CONVERSION_GIGACNVSVC_H__ 
#define      __GIGA_CONVERSION_GIGACNVSVC_H__  1 

///
/// from STL 
#include <string> 
#include <vector> 
#include <map> 


///
/// from Gaudi - base class 
#include "GaudiKernel/IIncidentListener.h" 
#include "GaudiKernel/ConversionSvc.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/Stat.h"

class IGiGaSvc; 
class IGiGaSetUpSvc;
class IDataProviderSvc;
class IParticlePropertySvc;
class IMagneticFieldSvc;
class IObjManager;
class IChronoStatSvc;
class IIncidentSvc;

///
#include "GiGa/IGiGaCnvSvc.h" 

/**  
     @class GiGaCnvSvc GiGaCnvSvc.h GiGa/GiGaCnvSvc.h
     
     Base conversion service  for converting of Gaudi 
     structures into Geant4 primary event record  
     
     @author Vanya Belyaev 
     @date    07/08/2000 
*/

class GiGaCnvSvc: virtual public  IGiGaCnvSvc       ,
                  virtual public  IIncidentListener ,       
                  public          ConversionSvc
{
  ///
public:
  ///
  class Leaf
  {
    ///
  public:
    ///
    Leaf( const std::string&  Path  = ""     ,
	  const CLID&         Clid  = CLID() ,
	  const std::string&  Addr1 = ""     , 
	  const std::string&  Addr2 = ""     ) 
      : m_path   ( Path  )
      , m_clid   (   0   ) 
      , m_addr1  ( Addr1 )
      , m_addr2  ( Addr2 )
    {};
    ///
    virtual ~Leaf(){};
    ///
    inline const std::string& path () const { return m_path  ; } 
    inline const CLID&        clid () const { return m_clid  ; } 
    inline const std::string& addr1() const { return m_addr1 ; } 
    inline const std::string& addr2() const { return m_addr2 ; } 
    ///
    inline Leaf& setPath ( const std::string& Path ) { m_path  = Path; return *this ; } 
    inline Leaf& setClid ( const CLID&        Clid ) { m_clid  = Clid; return *this ; } 
    inline Leaf& setAddr1( const std::string& Addr ) { m_addr1 = Addr; return *this ; } 
    inline Leaf& setAddr2( const std::string& Addr ) { m_addr2 = Addr; return *this ; } 
    ///
  private:
    ///
    std::string     m_path  ;
    CLID            m_clid  ;
    std::string     m_addr1 ;
    std::string     m_addr2 ;
    ///
  };
  ///
  typedef std::vector<GiGaCnvSvc::Leaf> Leaves;  
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
  virtual StatusCode              queryInterface( const IID& , void**  );
  ///
  virtual StatusCode              declareObject ( const std::string& Path        ,
                                                  const CLID       & Clid        ,
                                                  const std::string& Addr1 = ""  ,
                                                  const std::string& Addr2 = ""  );
  ///
  virtual void                    handle         ( const Incident& );
  ///
protected:
  ///
  /// "main" data provider         (mandatory) 
  virtual IDataProviderSvc*       dpSvc     () { return m_dpSvc       ; }  
  ///  event data provider         (optional) 
  virtual IDataProviderSvc*       evtSvc    () { return m_evtSvc      ; }  
  /// detector data provider       (optional)
  virtual IDataProviderSvc*       detSvc    () { return m_detSvc      ; }  
  /// GiGa service                 (mandatory)
  virtual IGiGaSvc*               gigaSvc   () { return m_gigaSvc     ; }  
  /// GiGaSetUp service            (mandatory)
  virtual IGiGaSetUpSvc*          setupSvc  () { return m_setupSvc    ; } 
  /// Magnetic field service       (optional)
  virtual IMagneticFieldSvc*      mfSvc     () { return m_mfSvc       ; } 
  /// particle properties service  (optional)
  virtual IParticlePropertySvc*   ppSvc     () { return m_ppSvc       ; } 
  /// chrono  service              (optional)
  virtual IChronoStatSvc*         chronoSvc () { return m_chronoSvc   ; } 
  /// object manager               (optiona)
  virtual IObjManager*            objMgr    () { return m_objMgr      ; } 
  /// incident service                          
  virtual IIncidentSvc*           incSvc    () { return m_incSvc      ; } 
  ///
  StatusCode    setNameOfDataProviderSvc( const std::string& Name ) { m_dpName = Name ; } 
  ///
  inline StatusCode Error     ( const std::string & msg                       ,  
				const StatusCode  & sc  = StatusCode::FAILURE );
  inline StatusCode Warning   ( const std::string & msg                       ,  
				const StatusCode  & sc  = StatusCode::FAILURE );
  inline StatusCode Print     ( const std::string & msg                       ,  
				const MSG::Level  & lvl = MSG::INFO           ,
				const StatusCode  & sc  = StatusCode::FAILURE );
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
private: 
  ///
  GiGaCnvSvc()                               ; // no default constructor  
  GiGaCnvSvc           ( const GiGaCnvSvc& ) ; // no copy
  GiGaCnvSvc& operator=( const GiGaCnvSvc& ) ; // no assignment 
  ///
  virtual StatusCode              declareObject ( const Leaf& Leaf );
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
  std::string                          m_inName     ; 
  IIncidentSvc*                        m_incSvc     ;
  ///
};        
///////////////////////////////////////////////////////////////////////////////////
inline StatusCode GiGaCnvSvc::Error( const std::string& Message , const StatusCode & Status )
{  
  Stat stat( chronoSvc() ,  name() + ":Error" );
  return  Print( Message , MSG::ERROR  , Status  ) ; 
};  
///////////////////////////////////////////////////////////////////////////////////
inline StatusCode GiGaCnvSvc::Warning( const std::string& Message , const StatusCode & Status )
{
  Stat stat( chronoSvc() ,  name() + ":Warning" );
  return  Print( Message , MSG::WARNING , Status ) ; 
};  
///////////////////////////////////////////////////////////////////////////////////
inline StatusCode GiGaCnvSvc::Print( const std::string& Message , 
				     const MSG::Level & level   , 
				     const StatusCode & Status )
{ MsgStream log( msgSvc() , name() ); log << level << Message << endreq ; return  Status; };  
///////////////////////////////////////////////////////////////////////////////////
inline StatusCode GiGaCnvSvc::Exception( const std::string    & Message , 
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
inline StatusCode GiGaCnvSvc::Exception( const std::string    & Message , 
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
inline StatusCode GiGaCnvSvc::Exception( const std::string    & Message , 
					 const MSG::Level     & level   , 
					 const StatusCode     & Status )
{
  Stat stat( chronoSvc() , "*UNKNOWN* exception" );
  MsgStream log( msgSvc() , name() + ":UNKNOWN exception" ); 
  log << level << Message << ": UNKNOWN exception"  << endreq ; 
  return  Status;
};  
///////////////////////////////////////////////////////////////////////////////////
 
#endif  //   __GIGA_CONVERSION_GIGACNVSVC_H__ 
















