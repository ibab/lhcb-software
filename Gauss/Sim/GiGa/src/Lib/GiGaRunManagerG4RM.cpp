// $Id: GiGaRunManagerG4RM.cpp,v 1.4 2002-02-12 17:10:29 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2002/01/22 18:20:53  ibelyaev
//  Vanya: update for newer versions of Gaudi and Geant4
//
// Revision 1.2  2001/08/12 15:42:49  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.1  2001/08/01 09:42:24  ibelyaev
// redesign and reimplementation of GiGaRunManager class
// 
// ============================================================================
// GiGa 
#include "GiGa/GiGaUtil.h"
#include "GiGa/IGiGaGeoSrc.h"
#include "GiGa/GiGaRunManager.h"
/// G4 
#include "G4VUserDetectorConstruction.hh"

// ============================================================================
/** Implementation of virtual G4RunManager methods of class GiGaRunManager
 *
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 * @date 30/07/2001 
 */
// ============================================================================

// ============================================================================
/** overriden method from G4RunManager
 *  ONE NEVER SHOULD USE IT!!!
 */
// ============================================================================
void GiGaRunManager::BeamOn( int         n_event       ,                      
                             const char* macroFile ,                       
                             int         n_select      )
{
  Error("The usage of BeamOn() method is dangerouse!");
  StatusCode sc( StatusCode::SUCCESS ) ;
  try
    {
      if(  !evt_Is_Processed() ) { sc  = processTheEvent() ; } 
      if( sc.isFailure() ){ Exception(" forbidden BeamOn() method!");}
    }
  catch( const GaudiException& Excp )
    { Exception( "BeamOn()", Excp ); }
  catch( const std::exception& Excp ) 
    { Exception( "BeamOn()", Excp ); }
  catch( ... ) 
    { Exception( "BeamOn()"       ); }
};

// ============================================================================
// Initialize the geometry
// ============================================================================
void GiGaRunManager::InitializeGeometry()
{
  /// get root of geometry tree 
  G4VPhysicalVolume* root = 0; 
  if      ( 0 != m_rootGeo                  ) 
    { 
      Print(" Already converted geometry will be used!"); 
      root = m_rootGeo ;   
    } 
  else if ( 0 != geoSrc()                  )
    {
      Print(" Geometry will be exxtracted from " + 
            GiGaUtil::ObjTypeName( geoSrc() ) ); 
      root = geoSrc()->world (); 
    }
  else if ( 0 != G4RunManager::userDetector )
    {
      Print(" Geometry will be constructed using " + 
            GiGaUtil::ObjTypeName( G4RunManager::userDetector ) );
      root = G4RunManager::userDetector->Construct() ;
    }
  else   
    { Error(" There are NO ANY sources of Geometry information!"); }
  //
  if( 0 == root ) 
    { Exception("InitializeGeometry: no any geometry sources abvailable");}
  ///  
  DefineWorldVolume( root ) ; 
  G4RunManager::geometryInitialized = true;
};

// ============================================================================
// Initialize 
// ============================================================================
void GiGaRunManager::Initialize()
{
  StatusCode sc (StatusCode::SUCCESS ) ; 
  try
    {
      if(  !krn_Is_Initialized() ) { sc = initializeKernel() ; }
      if( sc.isFailure() ) 
        { Exception("Initialize(): could not initializeKernel()");}
    }
  catch( const GaudiException& Excp )
    { Exception( "GiGaRunManager::Initialize()", Excp ); }
  catch( const std::exception& Excp ) 
    { Exception( "GiGaRunManager::Initialize()", Excp ); }
  catch( ... ) 
    { Exception( "GiGaRunManager::Initialize()"       ); }
  ///
};

// ============================================================================
// The End 
// ============================================================================

