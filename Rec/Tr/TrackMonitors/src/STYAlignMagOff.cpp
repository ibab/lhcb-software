// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// local
#include "STYAlignMagOff.h"

// detector
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "STDet/DeSTSensor.h"
#include "STDet/DeITDetector.h"
#include "STDet/DeITBox.h"

// kernel
#include "Kernel/STLexicalCaster.h"

// Event

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IHistogram2D.h"

#include <vector>

using namespace LHCb;
using namespace ST;
using namespace AIDA;
using namespace Gaudi;

//-----------------------------------------------------------------------------
// Implementation file for class : STYAlignMagOff
//
// 2010-05-31 : Frederic Guillaume Dupertuis
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( STYAlignMagOff )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
STYAlignMagOff::STYAlignMagOff( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : TrackMonitorBase ( name , pSvcLocator )
{
  declareProperty( "DetType"         , m_detType =  "IT" );
  declareProperty( "XTol"            , m_xTol = 1.* Gaudi::Units::mm );
  declareProperty( "YTol"            , m_yTol = 10.* Gaudi::Units::mm );
  declareProperty( "Global"          , m_global = false );
  declareProperty( "XY"              , m_xy = false );
  declareProperty( "Expected"        , m_expected = false );
  declareProperty( "PrintSectorInfo" , m_printsectorinfo = false );
}
//=============================================================================
// Destructor
//=============================================================================
STYAlignMagOff::~STYAlignMagOff() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode STYAlignMagOff::initialize() {
  StatusCode sc = TrackMonitorBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  
  m_tracker = getDet< DeSTDetector >( DeSTDetLocation::location( m_detType ) );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode STYAlignMagOff::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  Tracks* tracks = get< LHCb::Tracks >( inputContainer() );
  Tracks::const_iterator It, Begin( tracks -> begin() ),
    End( tracks -> end() );

  const DeSTDetector::Sectors& sectors = m_tracker->sectors();
    
  StatusCode extrapolation;
  State trackState;
  DeSTSector* sector;
  DeSTSensor* sensor;
  int xInverted;
  int yInverted;
  double xGlobalScale;
  double yGlobalScale;
  std::vector<unsigned int> sectorHitVect;
  
  if(m_detType == "IT"){
    xGlobalScale = 700.;
    yGlobalScale = 250.;
  }else{
    xGlobalScale = 800.;
    yGlobalScale = 700.;
  }
  
  for (It = Begin; It != End; It++)
  {
    sectorHitVect.clear();

    const std::vector<LHCb::LHCbID>& ids = (*It) -> lhcbIDs();

    bool inST(false);
        
    DeSTDetector::Stations stations(m_tracker->stations());
    
    DeSTDetector::Stations::const_iterator iStat = stations.begin();
    
    for ( ; iStat != stations.end(); ++iStat){
      extrapolation = extrapolator() -> propagate( (**It), ((*iStat)->toGlobal(Gaudi::XYZPoint(0.,0.,0.))).z(), trackState );

      if(! extrapolation.isSuccess())
        continue;
            
      if(fabs(trackState.position().x()) < xGlobalScale && fabs(trackState.position().y()) < yGlobalScale){
        inST = true;
        break;
      }
    }
    
    if(! inST)
      continue;
    
    DeSTDetector::Sectors::const_iterator iSect = sectors.begin();
    
    for ( ; iSect != sectors.end(); ++iSect)
    {
      sensor = (*iSect) -> middleSensor();
      if ( sensor == 0 )
        continue;
      
      extrapolation = extrapolator() -> propagate( (**It), sensor -> plane(), trackState );
      
      if( extrapolation.isSuccess() ){
        
        if( inSector(trackState, *iSect) ){
          
          if(sensor -> yInverted())
            yInverted = -1;
          else
            yInverted = 1;
          
          if(sensor -> xInverted())
            xInverted = -1;
          else
            xInverted = 1;
    
          plot((double)(yInverted) * (*iSect) -> toLocal(trackState.position()).y(),
               "SectLocYExp/"+(*iSect) -> nickname(),
               "Y Spatial Distribution",
               (int)(-(*iSect) -> stripLength()/2.-m_yTol),
               (int)((*iSect) -> stripLength()/2.+m_yTol),
               (unsigned int)((*iSect) -> stripLength()/2.+m_yTol)*8);
          
          if(m_xy){
            plot2D((double)(xInverted) * (*iSect) -> toLocal(trackState.position()).x(),
                   (double)(yInverted) * (*iSect) -> toLocal(trackState.position()).y(),
                   "SectXYExp/"+(*iSect) -> nickname(),
                   "XY Spatial Distribution",
                   (int)(-sensor -> activeWidth()/2.-m_xTol), 
                   (int)(sensor -> activeWidth()/2.+m_xTol),
                   (int)(-(*iSect) -> stripLength()/2.-m_yTol),
                   (int)((*iSect) -> stripLength()/2.+m_yTol),
                   (unsigned int)(sensor -> activeWidth()/2.+m_xTol),
                   (unsigned int)((*iSect) -> stripLength()/2.+m_yTol));
          }

          for(std::vector<LHCb::LHCbID>::const_iterator ItID = ids.begin();
              ItID != ids.end(); ItID++){
            
            if(m_detType == "IT"){
              if(! (*ItID).isIT()) continue;
            }else{
              if(! (*ItID).isTT()) continue;
            }
            
            sector = m_tracker -> findSector((*ItID).stID());
            
            if(sector != *iSect)
              continue;
            
            if(m_global){
              plot2D(trackState.x(),
                     trackState.y(),
                     "GlobalXYLHCbIDs/"+((DeSTStation*)m_tracker -> findStation((*ItID).stID()))->nickname(),
                     "XY Hits Distribution",
                     -xGlobalScale,xGlobalScale,
                     -yGlobalScale,yGlobalScale,
                     (unsigned int)(xGlobalScale)/4,
                     (unsigned int)(yGlobalScale)/2);
            }
            
            plot((double)(yInverted) * sector -> toLocal(trackState.position()).y(),
                 "SectLocYLHCbIDs/"+sector -> nickname(),
                 "Y Hits Distribution",
                 (int)(-sector -> stripLength()/2.-m_yTol), 
                 (int)(sector -> stripLength()/2.+m_yTol),
                 (unsigned int)(sector -> stripLength()/2.+m_yTol)*8);
            
            if(m_xy){
              plot2D((double)(xInverted) * sector -> toLocal(trackState.position()).x(),
                     (double)(yInverted) * sector -> toLocal(trackState.position()).y(),
                     "SectXYLHCbIDs/"+sector -> nickname(),
                     "XY Hits Distribution",
                     (int)(-sensor -> activeWidth()/2.-m_xTol),
                     (int)(sensor -> activeWidth()/2.+m_xTol),
                     (int)(-sector -> stripLength()/2.-m_yTol),
                     (int)(sector -> stripLength()/2.+m_yTol),
                     (unsigned int)(sensor -> activeWidth()/2.+m_xTol),
                     (unsigned int)(sector -> stripLength()/2.+m_yTol));
            }
            
            break;
          }
        }
      }
    }
  }
  
  return StatusCode::SUCCESS;
}
 
/**
 *
 */
bool STYAlignMagOff::inSector( const State& state, 
                           const DeSTSector* sector) const
{
  DeSTSensor* sensor = sector -> middleSensor();
  
  if((fabs(sector -> toLocal(state.position()).x()) < (sensor -> activeWidth()/2. + m_xTol)) && 
     (fabs(sector -> toLocal(state.position()).y()) < (sector -> stripLength()/2. + m_yTol)))
    return true;
    
  return false;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode STYAlignMagOff::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  if(m_printsectorinfo){
    const DeSTDetector::Sectors& sectors = m_tracker->sectors();
  
    DeSTDetector::Sectors::const_iterator iSect = sectors.begin();
    
    for ( ; iSect != sectors.end(); ++iSect)
    {
      info() << (*iSect) -> nickname() << " " << (*iSect) -> prodID() << endmsg;
    }

    info() << endmsg;

    iSect = sectors.begin();
    
    for ( ; iSect != sectors.end(); ++iSect)
    {
      info() << (*iSect) -> nickname() << " " << (*iSect) -> toGlobal( Gaudi::XYZPoint(0.,0.,0.) ) << endmsg;
    }
    
    info() << endmsg;
    
    DeITDetector* m_ITtracker = dynamic_cast<DeITDetector*>(m_tracker);
    if(m_ITtracker != 0){
      const DeITDetector::Boxes& boxes = m_ITtracker->boxes();
      
      DeITDetector::Boxes::const_iterator iBox = boxes.begin();
      
      for ( ; iBox != boxes.end(); ++iBox)
      {
        info() << (*iBox) -> nickname() << " " << (*iBox) -> toGlobal( Gaudi::XYZPoint(0.,0.,0.) ) << endmsg;
      }
    }

    info() << endmsg;

    iSect = sectors.begin();
    
    for ( ; iSect != sectors.end(); ++iSect)
    {
      info() << (*iSect) -> stripToChan(100).uniqueSector() << " " << (*iSect) -> nickname() << endmsg;
    }
        
  }
      
  return TrackMonitorBase::finalize();  // must be called after all other actions
}

//=============================================================================
