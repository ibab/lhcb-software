// $Id: GiGaRegionsTool.cpp,v 1.4 2007-01-12 15:36:53 ranjard Exp $
// Include files
// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/MsgStream.h"

// GiGaCnv
#include "GiGaCnv/GiGaVolumeUtils.h"

// SimSvc 
#include "SimSvc/ISimulationSvc.h"
#include "SimSvc/RegionCuts.h"

// local
#include "GiGaRegionsTool.h"

// G4 
#include "G4Region.hh"
#include "G4LogicalVolume.hh"
#include "G4ProductionCuts.hh"

// ============================================================================
/** @file 
 *  
 *  Implementation file for class : GiGaRegionsTool
 * 
 *  @date 2003-06-04 
 *  @author Vanya BELYAEV Ivan.Belyaev@cern.ch
 */
// ============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaRegionsTool )

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
GiGaRegionsTool::GiGaRegionsTool
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : GiGaToolBase ( type ,name , parent )
  , m_simSvcName ( "SimulationSvc"     ) 
  , m_simSvc     ( 0                   ) 
  , m_overwrite  ( true                ) 
{
  declareProperty( "SimulationService" , m_simSvcName ) ; 
  declareProperty( "Overwrite"         , m_overwrite  ) ; 
}


// ============================================================================
/** standard initialization of the tool 
 *  @see  GiGaToolBase
 *  @see  GiGaBase
 *  @see  AlgTool 
 *  @see IAlgTool
 *  @return status code
 */
// ============================================================================
StatusCode GiGaRegionsTool::initialize () 
{
  StatusCode sc = GiGaToolBase::initialize() ;
  if( sc.isFailure() ) 
    { return Error ( "Base class GiGaToolBase is not initialized " , sc ) ; }
  
  if( 0 != m_simSvc ) { m_simSvc->release() ; m_simSvc = 0 ; }
  sc = service( m_simSvcName , m_simSvc , true ) ;
  if( sc.isFailure() ) 
    { return Error ( "Service '"+m_simSvcName+"' is not located" , sc ) ; }
  
  return StatusCode::SUCCESS ;
}


// ============================================================================
/** standard finalization of the tool 
 *  @see  GiGaToolBase
 *  @see  GiGaBase
 *  @see  AlgTool 
 *  @see IAlgTool
 *  @return status code
 */
// ============================================================================
StatusCode GiGaRegionsTool::finalize   () 
{
  if( 0 != m_simSvc ) { m_simSvc->release() ; m_simSvc = 0 ; }
  return GiGaToolBase::finalize() ;
}


// ============================================================================
/** the only one method
 *  @see GiGaToolBase
 *  @see IGiGaTool
 *  @param region region name  
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaRegionsTool::process ( const std::string& region ) const 
{
  if( 0 == simSvc() ) 
    { return Error(" process('"+region+"'): ISimulationSvc* is invalid!" ) ; }
  
  typedef ISimulationSvc::VectOfRegCuts Regions ;
  typedef std::vector<std::string>      Volumes ;
  
  const Regions* regions = simSvc()->regionsDefs() ;
  if( 0 == regions ) 
    { return Error(" process('"+region+"'): Regions* poitns to NULL " ) ; }

  // loop over all regions 
  debug() << " Number of G4 regions =  " << (int) regions->size() << endmsg;
  for( Regions::const_iterator ireg = regions->begin() ; 
       regions->end() != ireg ; ++ireg )
    {
      // all regions? or only selected region?
      if( !region.empty() && ireg->region() != region ) { continue ; }
      debug() << " Process the region '" << ireg->region() << "'" << endmsg;
      //
      G4Region* reg = new G4Region( ireg->region() ) ;
      // add volumes to the region
      const Volumes& volumes = ireg->volumes() ;
      debug() << " Number of volumes in this region =  " << (int)volumes.size() 
              << endmsg;
      for( Volumes::const_iterator ivolume = volumes.begin() ; 
           volumes.end() != ivolume ; ++ivolume ) {
        
        G4LogicalVolume* volume = GiGaVolumeUtils::findLVolume( *ivolume ) ; 
        if( 0 == volume ) { 
          Error ( " process('" + ( ireg->region() ) + 
                  "'): G4LogicalVolume* '"          + (*ivolume) + 
                  "' points to NULL, skip it  "                  ) ; 
          continue;
        }
          
        if( 0 != volume->GetRegion() && !m_overwrite ) {
          Warning ( " G4LogicalVolume '" + (*ivolume) + 
                    "' already belongs to region '"   + 
                    volume->GetRegion()->GetName()    + "' , skip "    );
          continue;
        } 
        else if ( 0 != volume->GetRegion() &&  m_overwrite ) {
          if( (volume->GetRegion()->GetName()) == "DefaultRegionForTheWorld" ) {
            debug() << "G4Region Change for  "<<" G4LogicalVolume '"
                   << (*ivolume) <<" ' to "<< reg->GetName()  << endmsg;
          } else {      
            Warning ( " G4LogicalVolume '" + (*ivolume) + 
                      "' already belongs to region '"   + 
                      volume->GetRegion()->GetName()    +"', overwrite " );
          }
              
        }
        // set region 
        volume -> SetRegion            ( reg    ) ;
        reg    -> AddRootLogicalVolume ( volume ) ;
      } 
      
      
      // create production cuts 
      G4ProductionCuts* cuts = new G4ProductionCuts() ;

      if( 0 <= ireg -> gammaCut    () ) {
        cuts -> SetProductionCut ( ireg -> gammaCut    () ,  
                                   G4ProductionCuts::GetIndex ( "gamma" ) ) ;
      }
      if( 0 <= ireg -> electronCut  () ) {
        cuts -> SetProductionCut ( ireg -> electronCut () ,  
                                   G4ProductionCuts::GetIndex ( "e-"    ) ) ;
      }
      if( 0 <= ireg -> positronCut  () ) {
        cuts -> SetProductionCut ( ireg -> positronCut () ,  
                                   G4ProductionCuts::GetIndex ( "e+"    ) ) ;
      }
      
      // set production cuts for region      
      reg -> SetProductionCuts ( cuts );
    }
  
  return StatusCode::SUCCESS ;
}



// ============================================================================
// The END 
// ============================================================================


