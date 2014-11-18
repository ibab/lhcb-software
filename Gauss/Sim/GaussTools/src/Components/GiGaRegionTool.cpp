// $Id: GiGaRegionTool.cpp,v 1.3 2007-01-12 15:36:52 ranjard Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// G4 
#include "G4Region.hh"
#include "G4LogicalVolume.hh"
#include "G4ProductionCuts.hh"

/// GiGaCnv 
#include "GiGaCnv/GiGaVolumeUtils.h"

// local
#include "GiGaRegionTool.h"


// ============================================================================
/** @file
 *
 *  Implementation file for class : GiGaRegionTool
 * 
 *  @date 2003-05-30 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */
// ============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaRegionTool )

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
GiGaRegionTool::GiGaRegionTool
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : GiGaToolBase( type , name , parent ) 
  // 
  , m_region    ( "UNKNOWN" ) 
  , m_volumes   ()
  // 
  , m_gamma     ( 10 * mm   ) 
  , m_electron  ( 10 * mm   ) 
  , m_positron  ( 10 * mm   ) 
  //
  , m_overwrite ( true      ) 
{
  declareProperty ( "Region"         , m_region     ) ;
  declareProperty ( "Volumes"        , m_volumes    ) ;
  //
  declareProperty ( "CutForGamma"    , m_gamma      ) ;
  declareProperty ( "CutForElectron" , m_electron   ) ;
  declareProperty ( "CutForPositron" , m_positron   ) ;
  //
  declareProperty ( "Overwrite"      , m_overwrite  ) ;
}


// ============================================================================
/** the only one method
 *  @see GiGaToolBase
 *  @see IGiGaTool
 *  @param reg region name  
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaRegionTool::process ( const std::string& /* region */ ) const 
{
  // check the existence of the region
  G4Region* region = GiGaVolumeUtils::findRegion( m_region ) ;
  if ( 0 != region ) 
    { 
      Warning ( " The Region '" + m_region + "'  already exist, skip " ) ;
      return StatusCode::SUCCESS ;
    }
  
  // create new region 
  region = new G4Region( m_region ) ;
  // add volumes to the region 
  for( Volumes::const_iterator ivolume = m_volumes.begin() ;
       m_volumes.end() != ivolume ; ++ivolume ) 
    {
      G4LogicalVolume* volume = GiGaVolumeUtils::findLVolume( *ivolume ) ;
      if( 0 == volume              ) 
        { return Error( " G4LogicalVolume '" + 
                        (*ivolume) + "' is invalid " ) ; }
      if      ( 0 != volume->GetRegion() && !m_overwrite )
        {
          Warning ( " G4LogicalVolume '" + (*ivolume) + 
                    "' already belongs to region '"   + 
                    volume->GetRegion()->GetName()    + "' , skip "    );
          continue ;
        } 
      else if ( 0 != volume->GetRegion() &&  m_overwrite )
        {
          Warning ( " G4LogicalVolume '" + (*ivolume) + 
                    "' already belongs to region '"   + 
                    volume->GetRegion()->GetName()    +"', overwrite " );
        }
      // set region 
      volume -> SetRegion            ( region ) ;
      region -> AddRootLogicalVolume ( volume ) ;
    }
  
  
  // create production cuts 
  G4ProductionCuts* cuts = new G4ProductionCuts() ;
  cuts -> 
    SetProductionCut ( m_gamma    , G4ProductionCuts::GetIndex ( "gamma" ) ) ;
  cuts -> 
    SetProductionCut ( m_electron , G4ProductionCuts::GetIndex ( "e-"    ) ) ;
  cuts -> 
    SetProductionCut ( m_positron , G4ProductionCuts::GetIndex ( "e+"    ) ) ;
  
  // set production cuts for region

  region -> SetProductionCuts ( cuts ) ;
  
  return StatusCode::SUCCESS ;
}


// ============================================================================
// The END 
// ============================================================================
