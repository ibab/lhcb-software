// $Id: GiGaRunManagerG4RM.cpp,v 1.4 2004-02-17 18:26:52 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2003/04/06 18:49:48  ibelyaev
//  see $GIGAROOT/doc/release.notes
//
// ============================================================================
// GiGa 
#include "GiGa/GiGaUtil.h"
#include "GiGa/IGiGaGeoSrc.h"
// Local 
#include "GiGaRunManager.h"
/// G4 
#include "G4VUserDetectorConstruction.hh"

// ============================================================================
/** @file 
 *  Implementation of virtual G4RunManager methods of class GiGaRunManager
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 30/07/2001 
 */
// ============================================================================

// ============================================================================
/** overriden method from G4RunManager
 *  ONE NEVER SHOULD USE IT!!!
 */
// ============================================================================
void GiGaRunManager::BeamOn( int         /* n_event   */ ,                      
                             const char* /* macroFile */ ,
                             int         /* n_select  */ )
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
}

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
      Print(" Geometry will be extracted from " + 
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
    { Error(" There are NO known sources of Geometry information!"); }
  //
  if( 0 == root ) 
    { Exception("InitializeGeometry: NO 'geometry sources' abvailable");}
  ///  
  DefineWorldVolume( root ) ; 
  G4RunManager::geometryInitialized = true;
}

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
}

// ============================================================================
// The End 
// ============================================================================

