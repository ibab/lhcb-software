/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $ 
/// ===========================================================================
// from STL
#include <vector>
// GaudiKernel 
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/PropertyMgr.h"
#include "GaudiKernel/System.h"
#include "GaudiKernel/StreamBuffer.h"
// GiGa
#include "GiGa/GiGaSensDetBase.h" 

/** impementation of class GiGaSensDetBase
 *
 *  @author Vanya Belyaev 
 *  @date   22/07/2001 
 */

/// ===========================================================================
/** standard constructor   
 *  @param name name of this sensitive detector instance 
 *  @param Svc pointer to service locator 
 */
/// ===========================================================================
GiGaSensDetBase::GiGaSensDetBase( const std::string& nick , 
                                  ISvcLocator* svc )
  /// ATTENTION !!! this name ill be overwritten later!!!!
  : G4VSensitiveDetector( nick                 ) 
  ///
  , GiGaBase            ( nick     ,   svc     ) 
  /// initilization flag 
  , m_init              ( false                )
  /// Active Flag 
  , m_active            ( true                 ) 
  /// Det Name 
  , m_detName           () 
{ 
  declareProperty( "Active"                , m_active   );
  declareProperty( "DetectrrName"          , m_detName  );
};

/// ===========================================================================
/// virtual destructor 
/// ===========================================================================
GiGaSensDetBase::~GiGaSensDetBase(){};

/// ===========================================================================
/** ident
 *  @return name of the given sensitive detector instance 
 */
/// ===========================================================================
const std::string& GiGaSensDetBase::name () const 
{ return GiGaBase::name() ; }; 

/// ===========================================================================
/** initialize the sensitive detector  
 *  @return status code 
 */
/// ===========================================================================
StatusCode GiGaSensDetBase::initialize() 
{
  StatusCode sc = GiGaBase::initialize() ; 
  if( sc.isFailure() ) { return Error("Could not initialize Base class!"); } 
  /// Correct the names!
  {
    ///
    std::string tmp( m_detName + "/" + name() ); 
    std::string::size_type pos = tmp.find("//") ; 
    while( std::string::npos != pos ) 
      { tmp.erase( pos , 1 ) ; pos = tmp.find("//") ; }
    /// attention!!! direct usage of G4VSensitiveDetector members!!!! 
    pos = tmp.find_last_of('/') ; 
    if( std::string::npos == pos )
      { 
        G4VSensitiveDetector::SensitiveDetectorName = tmp ;  /// ATTENTION !!!
        G4VSensitiveDetector::thePathName           = "/" ;  /// ATTENTION !!! 
      }
    else 
      {
        G4VSensitiveDetector::SensitiveDetectorName = tmp              ;
        G4VSensitiveDetector::SensitiveDetectorName.remove(0,pos+1)    ;
        G4VSensitiveDetector::thePathName           = tmp              ;
        G4VSensitiveDetector::thePathName.remove(pos+1,tmp.length()-1) ;
        if( '/' != G4VSensitiveDetector::thePathName[(unsigned int)(0)] ) 
          { G4VSensitiveDetector::thePathName.insert(0,'/'); } 
      } 
    ///
    G4VSensitiveDetector::fullPathName =
      G4VSensitiveDetector::thePathName + 
      G4VSensitiveDetector::SensitiveDetectorName; 
    ///
    MsgStream log( msgSvc() , name() ) ;
    log << MSG::DEBUG 
        << System::typeinfoName( typeid( *this ) ) 
        << "::initialize() Names for G4VSensitiveDetector are: "  
        << " Path="          << G4VSensitiveDetector::GetPathName()   
        << " Name="          << G4VSensitiveDetector::GetName() 
        << " FullPathName="  << G4VSensitiveDetector::GetFullPathName() 
        << endreq;  
    ///
  }
  ///
  {
    Activate( m_active );
    MsgStream log( msgSvc() , name() ); 
    log << ( !isActive() ? MSG::FATAL : MSG::DEBUG )
        << System::typeinfoName( typeid (*this) )  
        << "::initialize() Sensitive Detector is " 
        << ( !isActive() ? "*NOT* activated " : "activated" ) 
        <<  endreq; /// ATTENTION !!! 
  }
  ///
  m_init = true;
  ///
  return StatusCode::SUCCESS;
}; 

/// ===========================================================================
/** finalize the sensitive detector  
 *  @return status code 
 */
/// ===========================================================================
StatusCode GiGaSensDetBase::finalize() { return GiGaBase::finalize();  };

/// ===========================================================================
/** query the interface
 *  @param ID   uniqie interface identifier 
 *  @param ppI  the placeholder for returned interface 
 *  @return status code 
 */
/// ===========================================================================
StatusCode GiGaSensDetBase::queryInterface( const InterfaceID& iid ,
                                            void** ppI)
{
  if( 0 == ppI ) { return StatusCode::FAILURE; } 
  *ppI = 0; 
  if      ( IID_IGiGaSensDet == iid ) 
    { *ppI = static_cast<IGiGaSensDet*>        ( this ) ; } 
  else                           
    { return GiGaBase::queryInterface( iid , ppI ) ; } /// RETURN!!!
  addRef();
  return StatusCode::SUCCESS;
};

/// ===========================================================================
/// serialize object for reading
/// ===========================================================================
StreamBuffer& GiGaSensDetBase::serialize( StreamBuffer& S )       
{
  GiGaBase::serialize( S ) ; 
  m_init = false ; 
  int    tmp     ; 
  S >> m_detName >> tmp ; 
  ///
  m_active = tmp ? true : false ;   
  ///
  return S;       
};
  
/// ===========================================================================
/// serialize object for writing 
/// ===========================================================================
StreamBuffer& GiGaSensDetBase::serialize( StreamBuffer& S ) const 
{
  GiGaBase::serialize( S ) ; 
  return S << m_detName << (int) m_active ;
}; 

/// ===========================================================================













