#ifndef     GIGA_GIGABASE_H
#define     GIGA_GIGABASE_H 1 
/// STL
#include <string>
#include <exception>
/// GaudiKernel 
#include "GaudiKernel/IProperty.h"
#include "GaudiKernel/ISerialize.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/MsgStream.h"
///
class IMessageSvc          ;
class ISvcLocator          ;
class IChronoStatSvc       ;
class IDataProviderSvc     ;
class IParticlePropertySvc ;  
class IMagneticFieldSvc    ;
class IIncidentSvc         ;
class IGiGaSvc             ; 
class IGiGaSetUpSvc        ; 
///
class PropertyMgr          ;
class MsgStream            ;
class StreamBuffer         ;
class GaudiException       ;
///

/** @class GiGaBase GiGaBase.h GiGa/GiGaBase.h
    
    Helper class for implementation of some GiGa classes. 
    It allows an easy configuration of properties and services location  
    Implement almost all virtual "technical functions".

    @author  Vanya Belyaev
    @date    23/01/2001
*/

class  GiGaBase: virtual public IProperty         , 
		 virtual public ISerialize        , 
                 virtual public IIncidentListener 
{
  ///
protected:
  ///
  /// Constructor and (virtual) Destructor 
  GiGaBase( const std::string& , ISvcLocator* );
  virtual ~GiGaBase();
  ///
public:
  ///
  virtual const std::string&     name    () const { return m_name     ; };
  ///
  /// Increment the reference count of Interface instance
  virtual unsigned long          addRef  ()        { return ++m_count                   ; }; 
  /// Release Interface instance
  virtual unsigned long          release ()        { return 0 < m_count ? --m_count : 0 ; };
  /// query interface 
  virtual StatusCode queryInterface(const InterfaceID& , void** );
  /// initialize object 
  virtual StatusCode initialize() ;
  /// finalize the obkject 
  virtual StatusCode finalize  () ;
  /// serialize object for reading 
  virtual StreamBuffer& serialize( StreamBuffer& S )       ;
  /// serialize object for writing 
  virtual StreamBuffer& serialize( StreamBuffer& S ) const ; 
  /// Set the property by property
  virtual StatusCode                    setProperty   ( const Property& p       )       ;
  /// Get the property by property
  virtual StatusCode                    getProperty   ( Property* p             ) const ;
  /// Get the property by name
  virtual const Property&               getProperty   ( const std::string& name ) const ; 
  /// Get list of properties
  virtual const std::vector<Property*>& getProperties ( )                         const ;
  ///  handle the incident 
  virtual void handle( const Incident& ) ;
  /// 
 protected:
  ///
  ///  Accesors to needed services and Service Locator 
  inline bool                   init      () const { return m_init      ; };
  inline ISvcLocator*           svcLoc    () const { return m_svcLoc    ; };  
  inline IGiGaSvc*              gigaSvc   () const { return m_gigaSvc   ; }; 
  inline IGiGaSetUpSvc*         setupSvc  () const { return m_setupSvc  ; }; 
  inline IMessageSvc*           msgSvc    () const { return m_msgSvc    ; };
  inline IChronoStatSvc*        chronoSvc () const { return m_chronoSvc ; };
  inline IDataProviderSvc*      evtSvc    () const { return m_evtSvc    ; }; 
  inline IDataProviderSvc*      detSvc    () const { return m_detSvc    ; }; 
  inline IIncidentSvc*          incSvc    () const { return m_incSvc    ; }; 
  inline IParticlePropertySvc*  ppSvc     () const { return m_ppSvc     ; }; 
  inline IMagneticFieldSvc*     mfSvc     () const { return m_mfSvc     ; };
  inline PropertyMgr*           propMgr   () const { return m_propMgr   ; };  
  ///
protected: 
  ///
  /// Methods for declaring properties to the property manager
  template <class TYPE>
  StatusCode declareProperty( const std::string& name , TYPE& reference )
  {
    if( 0 != propMgr() ) { propMgr()->declareProperty( name , reference ); } 
    return 0 != propMgr() ? StatusCode::SUCCESS : StatusCode::FAILURE ; 
  };
  StatusCode setProperties  () ; 
  ///
protected:
  ///  Print the error    message and return status code 
  StatusCode Error     ( const std::string& Message , const StatusCode & Status  = StatusCode::FAILURE ) const ;  
  ///  Print the warning  message and return status code 
  StatusCode Warning   ( const std::string& Message , const StatusCode & Status  = StatusCode::FAILURE ) const ;  
  ///  Print the warning  message and return status code 
  StatusCode Print     ( const std::string& Message , const StatusCode & Status  = StatusCode::FAILURE ) const ;  
  ///
  StatusCode Exception ( const std::string    & msg                        ,  
                         const GaudiException & exc                        , 
                         const MSG::Level     & lvl = MSG::FATAL           ,
                         const StatusCode     & sc  = StatusCode::FAILURE );
  ///
  StatusCode Exception ( const std::string    & msg                        ,  
                         const std::exception & exc                        , 
                         const MSG::Level     & lvl = MSG::FATAL           ,
                         const StatusCode     & sc  = StatusCode::FAILURE );
  ///
  StatusCode Exception ( const std::string    & msg                        ,  
                         const MSG::Level     & lvl = MSG::FATAL           ,
                         const StatusCode     & sc  = StatusCode::FAILURE );
  ///
private: 
  ///
  GiGaBase();                              /// no default 
  GiGaBase           ( const GiGaBase& );  /// no copy 
  GiGaBase& operator=( const GiGaBase& );  /// no assignment 
  ///
private:
  ///
  unsigned long         m_count      ; 
  std::string           m_name       ; 
  PropertyMgr*          m_propMgr    ; 
  ISvcLocator*          m_svcLoc     ; 
  bool                  m_init       ;
  /// standard "properties"
  std::string           m_gigaName   ; 
  std::string           m_setupName  ; 
  std::string           m_msgName    ; 
  std::string           m_chronoName ; 
  std::string           m_evtName    ; 
  std::string           m_detName    ; 
  std::string           m_incName    ; 
  std::string           m_ppName     ; 
  std::string           m_mfName     ; 
  ///
  int                   m_output     ; 
  ///
  IGiGaSvc*             m_gigaSvc    ;
  IGiGaSetUpSvc*        m_setupSvc   ;
  IMessageSvc*          m_msgSvc     ; 
  IChronoStatSvc*       m_chronoSvc  ; 
  IDataProviderSvc*     m_evtSvc     ; 
  IDataProviderSvc*     m_detSvc     ; 
  IIncidentSvc*         m_incSvc     ; 
  IParticlePropertySvc* m_ppSvc      ;
  IMagneticFieldSvc*    m_mfSvc      ;
  ///
};
///

#endif //   GIGA_GIGABASE_H












