// $Id: GiGaSetSimAttributes.cpp,v 1.2 2003-04-11 17:55:36 witoldp Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2003/04/06 19:07:33  ibelyaev
//  update foe newer GiGa, add new tools
// 
// ============================================================================
// GiGa
#include "GiGa/GiGaMACROs.h"
// SimSvc 
#include "SimSvc/ISimulationSvc.h"
// Geant4 
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "GaussG4UserLimits.h"
// local
#include "GiGaSetSimAttributes.h"
// ============================================================================

// ============================================================================
/** @file 
 * 
 *  Implementation file for class : GiGaSetSimAttributes
 * 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-06
 */
// ============================================================================

// ============================================================================
IMPLEMENT_GiGaFactory ( GiGaSetSimAttributes ) ;
// ============================================================================

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
GiGaSetSimAttributes::GiGaSetSimAttributes
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : GiGaToolBase   ( type , name , parent ) 
  , m_simSvcName   ( "SimulationSvc"   ) 
  , m_simSvc       ( 0                 ) 
{
  declareProperty ( "SimulationService" , m_simSvcName ) ;
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaSetSimAttributes::~GiGaSetSimAttributes(){};
// ============================================================================

// ============================================================================
/** initialize the object 
 *  @see GiGaToolBase
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode   GiGaSetSimAttributes::initialize  () 
{
  StatusCode sc = GiGaToolBase::initialize () ;
  if( sc.isFailure() ) { return Error("GiGaToolBase is not initilized!",sc);}
  
  // get the service 
  if( 0 != m_simSvc ) { m_simSvc -> release () ; m_simSvc = 0 ; }
  sc = service( m_simSvcName , m_simSvc , true );
  if( sc.isFailure() ) 
    { return Error("ISimulationSvc('"+m_simSvcName+"') is not located",sc); }
  if( 0 == m_simSvc || 0 == simSvc() ) 
    { return Error("ISimulationSvc('"+m_simSvcName+"') is NULL " ) ; }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** finalize the object  
 *  @see GiGaToolBase
 *  @see GiGaBase
 *  @see  AlgTool
 *  @see IAlgTool
 *  @return status code 
 */
// ============================================================================
StatusCode   GiGaSetSimAttributes::finalize    () 
{
  if( 0 != m_simSvc ) { m_simSvc -> release () ; m_simSvc = 0 ; }
  return GiGaToolBase::finalize ();
};
// ============================================================================

// ============================================================================
/** find g4 volume by name 
 *  @param name name of the volume 
 */
// ============================================================================
G4LogicalVolume* GiGaSetSimAttributes::g4volume         
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
      if( 0 != vol && vol->GetName() == address ) { return vol ; }
    }
  Error ( "g4volume('" + address + "'): volume is not found!" ) ;
  return 0 ;
};
// ============================================================================

// ============================================================================
/** the only one method, perform the erasure of Field Manager 
 *  @see IGiGaTool
 *  @param vol volume name 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSetSimAttributes::process ( const std::string& vol ) const 
{
  if( 0 == simSvc () )
    { return Error( " process('" + vol + "'): simSvc () is NULL! " ) ; }
  
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
    { return Error( " process('" + vol + "'): G4LogicalVolume* is invalid" ) ; }
  
  if(simSvc()->hasSimAttribute(vol)) 
    {
      SimAttribute attr = simSvc() -> simAttribute( vol );
      
      // instanciate GaussG4UserLimits
      GaussG4UserLimits* ulimit = new GaussG4UserLimits("GaussG4UserLimits");
      
      // set max allowed step
      if( -1 != attr.maxAllowedStep() ) 
        { ulimit -> SetMaxAllowedStep(attr.maxAllowedStep(),0); }
      
      // set max track length
      if( -1 != attr.maxTrackLength() )
        { ulimit -> SetUserMaxTrackLength(attr.maxTrackLength(),0); }
      
      // set max time
      if( -1 != attr.maxTime() )
        { ulimit -> SetUserMaxTime(attr.maxTime(),0); }    
      
      // set minimum kinetic energy
      if( -1 != attr.minEkine() )
        { ulimit -> SetUserMinEkine(attr.minEkine(),0); }
      
      // set minimum range
      if( -1 != attr.minRange() )
        { ulimit -> SetUserMinRange(attr.minRange(),0); }
      
      // attach user limits to the given G4 volume
      g4lv -> SetUserLimits(ulimit) ;
    }
  return StatusCode::SUCCESS ;
};
  


// ============================================================================
// The END 
// ============================================================================
