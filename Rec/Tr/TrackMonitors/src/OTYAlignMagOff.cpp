// $Id: $
// Include files 

#include <string>

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// local
#include "OTYAlignMagOff.h"

// detector
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"
#include "OTDet/DeOTStation.h"

// kernel

// Event

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IHistogram2D.h"

#include <iomanip>

using namespace LHCb;
using namespace OT;
using namespace AIDA;
using namespace Gaudi;

//-----------------------------------------------------------------------------
// Implementation file for class : OTYAlignMagOff
//
// 2010-05-31 : Frederic Guillaume Dupertuis
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTYAlignMagOff )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTYAlignMagOff::OTYAlignMagOff( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : TrackMonitorBase ( name , pSvcLocator )
{
  declareProperty( "XTol"            , m_xTol = 1.* Gaudi::Units::mm );
  declareProperty( "YTol"            , m_yTol = 10.* Gaudi::Units::mm );
  declareProperty( "Global"          , m_global = false );
  declareProperty( "Expected"        , m_expected = false );
  declareProperty( "PrintModuleInfo" , m_printmoduleinfo = false );
}
//=============================================================================
// Destructor
//=============================================================================
OTYAlignMagOff::~OTYAlignMagOff() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode OTYAlignMagOff::initialize() {
  StatusCode sc = TrackMonitorBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  
  m_tracker = getDet< DeOTDetector >( DeOTDetectorLocation::Default );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode OTYAlignMagOff::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  Tracks* tracks = get< LHCb::Tracks >( inputContainer() );
  Tracks::const_iterator It, Begin( tracks -> begin() ),
    End( tracks -> end() );

  const DeOTDetector::Modules& modules = m_tracker->modules();
    
  StatusCode extrapolation;
  State trackState;
  DeOTModule* module;
  DeOTLayer* layer;
  //DeOTStation* station;
  double xGlobalScale(6000.);
  double yGlobalScale(1000.);
  bool isA;
  bool isB;
  bool isA_filled;
  bool isB_filled;
  std::string monolayer;
  std::vector<unsigned int> moduleHitVectA;
  std::vector<unsigned int> moduleHitVectB;
  
  for (It = Begin; It != End; It++)
  {
    moduleHitVectA.clear();
    moduleHitVectB.clear();
    
    const std::vector<LHCb::LHCbID>& ids = (*It) -> lhcbIDs();

    bool inOT(false);
        
    DeOTDetector::Stations stations(m_tracker->stations());
    
    DeOTDetector::Stations::const_iterator iStat = stations.begin();
    
    for ( ; iStat != stations.end(); ++iStat){
      extrapolation = extrapolator() -> propagate( (**It), ((*iStat)->geometry()->toGlobal(Gaudi::XYZPoint(0.,0.,0.))).z(), 
                                                   trackState );
      
      if( extrapolation.isSuccess() ){
        if(fabs(trackState.position().x()) < xGlobalScale && fabs(trackState.position().y()) < yGlobalScale){
          inOT = true;
          break;
        }
      }
    }
    
    if(! inOT)
      continue;
    
    DeOTDetector::Modules::const_iterator iModule = modules.begin();
    
    for ( ; iModule != modules.end(); ++iModule)
    {
      extrapolation = extrapolator() -> propagate( (**It), (*iModule) -> plane(), trackState );
      
      if( extrapolation.isSuccess() ){
        if( inModule(trackState, *iModule) ){
          std::string nickname((*iModule) -> name());
          nickname.replace(0,42,"");
          
          layer = m_tracker -> findLayer((*iModule)-> elementID());

          plot((*iModule) -> toLocal(trackState.position()).y(),
               "ModuleLocYExp/"+nickname,
               "Y Spatial Distribution",
               (int)(-(*iModule) -> wireLength( (*iModule) -> elementID () )/2.-m_yTol),
               (int)((*iModule) -> wireLength( (*iModule) -> elementID () )/2.+m_yTol),
               (unsigned int)((*iModule) -> wireLength( (*iModule) -> elementID () )/2.+m_yTol)*4);
          
          if(!((*iModule) -> layerID() == 2 || (*iModule) -> layerID() == 1)){
            plot(layer -> geometry() -> toLocal(trackState.position()).y(),
                 "LayerLocYExp/"+nickname,
                 "Y Spatial Distribution",
                 -1000.,1000.,
                 4000);
          }

          isA_filled = false;
          isB_filled = false;
                    
          for(std::vector<LHCb::LHCbID>::const_iterator ItID = ids.begin();
              ItID != ids.end(); ItID++){
            
            if(! (*ItID).isOT()) continue;
            
            module = m_tracker -> findModule((*ItID).otID());
            
            if(module != *iModule)
              continue;
            
            layer = m_tracker -> findLayer((*ItID).otID());
            //station = m_tracker -> findStation((*ItID).otID());
                                    
            isA  = module -> monoLayerA( (*ItID).otID().straw() );
            isB  = module -> monoLayerB( (*ItID).otID().straw() );
            
            if(isA && isA_filled)
              continue;

            if(isB && isB_filled)
              continue;

            if(isA){
              monolayer = "A";
              isA_filled = true;
            }else{
              monolayer = "B";
              isB_filled = true;
            }
                        
            std::string nickname(module -> name());
            nickname.replace(0,42,"");
            std::string stationnickname(nickname);
            stationnickname.replace(2,stationnickname.size(),"");
      
            plot(module -> toLocal(trackState.position()).y(),
                 "ModuleLocYLHCbIDs/"+nickname+monolayer,
                 "Y Hits Distribution",
                 (int)(-module -> wireLength((*ItID).otID())/2.-m_yTol), 
                 (int)(module -> wireLength((*ItID).otID())/2.+m_yTol),
                 (unsigned int)(module -> wireLength((*ItID).otID())/2.+m_yTol)*4);

            if(!((*iModule) -> layerID() == 2 || (*iModule) -> layerID() == 1)){
              plot(layer -> geometry() -> toLocal(trackState.position()).y(),
                   "LayerLocYLHCbIDs/"+nickname+monolayer,
                   "Y Hits Distribution",
                   -1000.,1000.,
                   4000);
              
            }
          }
        }
      }
    }
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  
//=============================================================================
bool OTYAlignMagOff::inModule( const State& state, 
                         const DeOTModule* module) const
{
  if((fabs(module -> toLocal(state.position()).y()) < (module -> wireLength(module -> elementID())/2. + m_yTol)) && 
     (fabs(module -> toLocal(state.position()).x()) < ((0.25*module->nChannels()+0.25)*module->xPitch() + m_xTol)))
    return true;
    
  return false;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode OTYAlignMagOff::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  if(m_printmoduleinfo){
    const DeOTDetector::Modules& modules = m_tracker->modules();
    DeOTDetector::Modules::const_iterator iModule = modules.begin();
    DeOTLayer* layer;
    
    for ( ; iModule != modules.end(); ++iModule)
    {
      std::string nickname((*iModule) -> name());
      nickname.replace(0,42,"");
      layer = m_tracker -> findLayer((*iModule)-> elementID());
      info() << nickname << " " << layer -> geometry() -> toLocal((*iModule) -> toGlobal( Gaudi::XYZPoint(0.,0.,0.) )).y() << endmsg;
    }
    
    info() << endmsg;
  }
    
  return TrackMonitorBase::finalize();  // must be called after all other actions
}

//=============================================================================
