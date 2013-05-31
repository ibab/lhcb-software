// $Id: GiGaSensDetBase.cpp,v 1.9 2006-07-27 09:33:29 gcorti Exp $ 

// from STL
#include <vector>

// from GiGa
#include "GiGa/GiGaSensDetBase.h" 
#include "GiGa/GiGaUtil.h" 

/** @file 
 *  impementation of class GiGaSensDetBase
 *
 *  @author Vanya Belyaev 
 *  @date   22/07/2001 
 */

namespace GiGaSensDetBaseLocal
{
#ifdef GIGA_DEBUG
  /** @var   s_Counter
   *  static instance counter 
   */
  static GiGaUtil::InstanceCounter<GiGaSensDetBase> s_Counter ;
#endif   
}

//=============================================================================
// Standard constructor 
//=============================================================================
GiGaSensDetBase::GiGaSensDetBase( const std::string& type, 
                                  const std::string& name, 
                                  const IInterface* parent )

  // ATTENTION !!! this name ill be overwritten later!!!!
  : G4VSensitiveDetector( name) 
  , GiGaBase( type , name , parent ) 
{
  declareInterface<IGiGaSensDet> ( this );
  declareProperty( "Active"       , m_active = true ); ///< Active Flag
  declareProperty( "DetectorPath" , m_detPath = "" );  ///< Path

#ifdef GIGA_DEBUG
  GiGaSensDetBaseLocal::s_Counter.increment();
#endif 

}

//=============================================================================
// virtual destructor 
//=============================================================================
GiGaSensDetBase::~GiGaSensDetBase()
{
#ifdef GIGA_DEBUG
  GiGaSensDetBaseLocal::s_Counter.decrement();
#endif 
};

//=============================================================================
// initialize the sensitive detector (Gaudi)
//=============================================================================
StatusCode GiGaSensDetBase::initialize()  
{
  StatusCode sc = GiGaBase::initialize() ; 
  if( sc.isFailure() ) { 
    return Error("Could not initialize base class GiGaBase"); 
  } 

  // Correct the names!
  {

    std::string detname(name());
    std::string::size_type posdot = detname.find(".");
    detname.erase(0,posdot+1);

    std::string tmp( m_detPath + "/" + detname ); 
    std::string::size_type pos = tmp.find("//") ; 
    while( std::string::npos != pos ) 
      { tmp.erase( pos , 1 ) ; pos = tmp.find("//") ; }

    // attention!!! direct usage of G4VSensitiveDetector members!!!! 
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
          { G4VSensitiveDetector::thePathName.insert(0,"/"); } 
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
  return Print("GiGaSensDetBase initialized successfully" ,
               StatusCode::SUCCESS                        , MSG::VERBOSE ) ;
}; 
// ============================================================================

// ============================================================================
/** finalize the sensitive detector  
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSensDetBase::finalize() 
{ 
  Print("GiGaSensDetBase finalization" ,
        StatusCode::SUCCESS            , MSG::VERBOSE ) ;
  ///
  return GiGaBase::finalize();  
};
// ============================================================================

unsigned long GiGaSensDetBase::release()
{
  unsigned long count = addRef() ;
  if ( count > 2 ) {
    AlgTool::release() ;
    return AlgTool::release() ;
  }
  this -> GaudiTool::~GaudiTool() ;
  return 0 ;
};

// ============================================================================
bool GiGaSensDetBase::processStep( G4Step* step,
                                   G4TouchableHistory* history ) {
  // delegate to ProcessHits
  return ProcessHits( step, history );
  
}

  
// ============================================================================
// The END  
// ============================================================================













