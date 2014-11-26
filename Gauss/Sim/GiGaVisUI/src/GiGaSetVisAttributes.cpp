// $Id: GiGaSetVisAttributes.cpp,v 1.4 2008-12-08 10:06:18 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// DetDesc
#include "DetDesc/ILVolume.h" 
// VisSvc
#include "VisSvc/IVisualizationSvc.h"
// Geant4 
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
// GiGa
//#include "GiGa/GiGaMACROs.h"
// local 
#include "GiGaSetVisAttributes.h"

/** @file 
 *  Implementation file for class : GiGaSetVisAttributes
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 */

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaSetVisAttributes )

// ============================================================================
/** standard constructor
 *  @see GiGaToolBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaSetVisAttributes::GiGaSetVisAttributes
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : GiGaToolBase   ( type , name , parent ) 
  , m_visSvcName   ( "VisualizationSvc"   ) 
  , m_visSvc       ( 0                    ) 
{
  declareProperty ( "VisualizationService" , m_visSvcName ) ;
}


// ============================================================================
/// destructor 
// ============================================================================
GiGaSetVisAttributes::~GiGaSetVisAttributes(){}


// ============================================================================
/** initialize the object 
 *  @see GiGaToolBase
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode   GiGaSetVisAttributes::initialize  () 
{
  StatusCode sc = GiGaToolBase::initialize () ;
  if( sc.isFailure() ) { return Error("GiGaToolBase is not initilized!",sc);}
  
  if( 0 == detSvc() ) 
    { return Error("IDataProviderSvc*(detSvc()) points ti NULL!"); }
  
  // get the service 
  if( 0 != m_visSvc ) { m_visSvc -> release () ; m_visSvc = 0 ; }
  sc = service( m_visSvcName , m_visSvc , true );
  if( sc.isFailure() ) 
    { return Error("IVisualizationSvc('"+m_visSvcName+"') is not located",sc); }
  if( 0 == m_visSvc || 0 == visSvc() ) 
    { return Error("IVisualizationSvc('"+m_visSvcName+"') is NULL " ) ; }
  
  return StatusCode::SUCCESS ;
}


// ============================================================================
/** finalize the object  
 *  @see GiGaToolBase
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode   GiGaSetVisAttributes::finalize    () 
{
  if( 0 != m_visSvc ) { m_visSvc -> release () ; m_visSvc = 0 ; }
  return GiGaToolBase::finalize ();
}


// ============================================================================
/** get ILVolume by its address
 *  @return ILVolume object 
 */
// ============================================================================
const ILVolume* GiGaSetVisAttributes::volume
( const std::string& address ) const 
{
  if( 0 == detSvc() ) 
    { Error("volume('"+address+"'): detSvc() is NULL"); return 0 ; } 
  SmartDataPtr<ILVolume> logvol( detSvc() , address ) ;
  return logvol ;
}

// ============================================================================
/** find g4 volume by name 
 *  @param name name of the volume 
 */
// ============================================================================
G4LogicalVolume* GiGaSetVisAttributes::g4volume         
( const std::string& address ) const 
{
  const G4LogicalVolumeStore* store = G4LogicalVolumeStore::GetInstance();
  if( 0 == store ) 
  {
    Error ( "g4volume('" + address + "'): G4LogicalVolumeStore* is NULL!" ) ;
    return 0 ;
  }
  for( G4LogicalVolumeStore::const_iterator ivolume = 
         store -> begin() ; store->end() != ivolume ; ++ivolume ) 
  {
    G4LogicalVolume* vol = *ivolume ;
    if( 0 != vol ) {
      std::string G4VolName = vol->GetName();
      if (G4VolName == address ) { return vol ; }
    }
  }
  Error ( "g4volume('" + address + "'): volume is not found!" ) ;
  return 0 ;
}


// ============================================================================
/** the only one method, perform the erasure of Field Manager 
 *  @see IGiGaTool
 *  @param vol volume name 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSetVisAttributes::process ( const std::string& vol ) const 
{
  if( 0 == detSvc () ) 
    { return Error( " process('" + vol + "'): detSvc () is NULL! " ) ; }
  if( 0 == visSvc () )
    { return Error( " process('" + vol + "'): visSvc () is NULL! " ) ; }
  
  // for all volumes 
  if( vol.empty() || "All" == vol || "ALL" == vol || "all" == vol ) 
    {
      const G4LogicalVolumeStore* store = G4LogicalVolumeStore::GetInstance();
      if( 0 == store ) 
        { return Error( " process('" + vol + 
                        "'): G4LogicalVolumeStore* is invalid" ) ; }
      for( G4LogicalVolumeStore::const_iterator ivolume = 
             store -> begin() ; store -> end() != ivolume ; ++ivolume ) 
        {
          G4LogicalVolume* vol = *ivolume ;
          if( 0 != vol ) { process( vol->GetName() ) ;  }
        }
      return StatusCode::SUCCESS ;
    };
  
  G4LogicalVolume* g4lv = g4volume ( vol ) ;
  if ( 0 == g4lv ) 
    { return Error( " process('" + vol + "'): invald G4LogicalVolume " ) ; }

  const ILVolume*    lv = volume   ( vol ) ;

  // Marco Pappagallo - 2008-10-16, fix ERROR message in following if block
  if( "World" == vol) {
    debug() << "World volume exists only in Geant4 world: no xml associated!"
            << endreq;
    return StatusCode::SUCCESS;
  }

  if( 0 == lv ) 
    { return Error( " process('" + vol + "'): invalid ILVolume "       ) ; }
  
  const VisAttribute va =  visSvc()-> visAttribute( lv ) ;
  
  static const VisAttribute s_def = VisAttribute () ;
  if( s_def.visible       () != va.visible       () || 
      s_def.openStatus    () != va.openStatus    () || 
      s_def.displayMode   () != va.displayMode   () || 
      s_def.color().red   () != va.color().red   () ||
      s_def.color().green () != va.color().green () ||
      s_def.color().blue  () != va.color().red   () ||
      s_def.color().alpha () != va.color().alpha ()  )
    {
      // create new attribute 
      G4VisAttributes* g4va = new G4VisAttributes () ;
      
      if      ( VisAttribute::VISIBLE     == va.visible     () ) 
        { g4va -> SetVisibility         ( true  ) ; }
      else if ( VisAttribute::NOT_VISIBLE == va.visible     () ) 
        { g4va -> SetVisibility         ( false ) ; }
      
      if      ( VisAttribute::OPENED      == va.openStatus  () )
        { g4va -> SetDaughtersInvisible ( false ) ; }
      else if ( VisAttribute::CLOSED      == va.openStatus  () )
        { g4va -> SetDaughtersInvisible ( true  ) ; }
      
      if      ( VisAttribute::PLAIN       == va.displayMode () )
        { g4va -> SetDaughtersInvisible ( true  ) ; }
      else if ( VisAttribute::WIRE_FRAME  == va.displayMode () )
        { g4va -> SetForceWireframe     ( true  ) ; }
      
      g4va -> SetColor( va.color () .red   () , 
                        va.color () .green () , 
                        va.color () .blue  () , 
                        va.color () .alpha () ) ;
      
      // set visualization attributes 
      g4lv -> SetVisAttributes( g4va ) ;
    }
  
  return StatusCode::SUCCESS ;
}


// ============================================================================
// The END 
// ============================================================================
