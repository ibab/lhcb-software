// $Id: GiGaSensDetBase.cpp,v 1.7 2003-07-07 16:48:09 ranjard Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2002/12/07 14:27:51  ibelyaev
//  see $GIGAROOT/cmt/requirements file
//
// Revision 1.5  2002/08/26 12:27:58  witoldp
// small changed to GiGaSensDetBase
//
// Revision 1.4  2002/05/07 12:21:34  ibelyaev
//  see $GIGAROOT/doc/release.notes  7 May 2002
//
// ============================================================================
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
};

// ============================================================================
/** standard constructor 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaSensDetBase::GiGaSensDetBase 
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent )
  // ATTENTION !!! this name ill be overwritten later!!!!
  : G4VSensitiveDetector( name                 ) 
  , GiGaBase            ( type , name , parent ) 
  // Active Flag 
  , m_active            ( true                 ) 
  // Path 
  , m_detPath           ( ""                   ) 
{
  declareInterface<IGiGaSensDet> ( this );
  declareProperty( "Active"       , m_active   );
  declareProperty( "DetectorPath" , m_detPath   );
#ifdef GIGA_DEBUG
  GiGaSensDetBaseLocal::s_Counter.increment () ;
#endif 
};
// ============================================================================

// ============================================================================
// virtual destructor 
// ============================================================================
GiGaSensDetBase::~GiGaSensDetBase()
{
#ifdef GIGA_DEBUG
  GiGaSensDetBaseLocal::s_Counter.decrement () ;
#endif 
};
// ============================================================================
/** initialize the sensitive detector  
 *  @see GiGaBase 
 *  @see  AlgTool 
 *  @see IAlgTool 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSensDetBase::initialize() 
{
  StatusCode sc = GiGaBase::initialize() ; 
  if( sc.isFailure() ) 
    { return Error("Could not initialize base class GiGaBase"); } 
  // Correct the names!
  {
    ///

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

// ============================================================================
// The END  
// ============================================================================













