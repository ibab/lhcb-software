// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// local
#include "OTCoordinates.h"

// detector
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTLayer.h"
#include "OTDet/DeOTModule.h"

// kernel

//
#include <iostream>
#include <fstream>

using namespace LHCb;
using namespace OT;
using namespace Gaudi;
using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : OTCoordinates
//
// 2011-05-15 : Frederic Guillaume Dupertuis
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTCoordinates )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTCoordinates::OTCoordinates( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : TrackMonitorBase ( name , pSvcLocator )
{
  declareProperty( "PrintModuleInfo" , m_printmoduleinfo = true );
  declareProperty( "AlignmentTag"    , m_alignmenttag = "v4_2" );
}
//=============================================================================
// Destructor
//=============================================================================
OTCoordinates::~OTCoordinates() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode OTCoordinates::initialize() {
  StatusCode sc = TrackMonitorBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  
  m_tracker = getDet< DeOTDetector >( DeOTDetectorLocation::Default );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode OTCoordinates::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode OTCoordinates::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  
  if(m_printmoduleinfo){
    const DeOTDetector::Modules& modules = m_tracker->modules();
    const DeOTDetector::Layers& layers = m_tracker->layers();

    std::string OutputNameModules("OTCoordinates-"+m_alignmenttag+"-Modules.txt");
    std::string OutputNameLayers("OTCoordinates-"+m_alignmenttag+"-Layers.txt");
    
    ofstream OutputModules(OutputNameModules.c_str());
    ofstream OutputLayers(OutputNameLayers.c_str());
            
    DeOTDetector::Layers::const_iterator iLayer = layers.begin();

    std::string layernickname;
        
    for ( ; iLayer != layers.end(); ++iLayer)
    {
      layernickname = (*iLayer) -> name();
      layernickname.replace(0,42,"");
      
      info() << layernickname << " " << (*iLayer) -> geometry() -> toGlobal( Gaudi::XYZPoint(0.,0.,0.) ) << endmsg;
      OutputLayers << layernickname << " " << (*iLayer) -> geometry() -> toGlobal( Gaudi::XYZPoint(0.,0.,0.) ) << endl;
    }
    
    info() << endmsg;
        
    DeOTDetector::Modules::const_iterator iModule = modules.begin();

    std::string modulenickname;
        
    for ( ; iModule != modules.end(); ++iModule)
    {
      modulenickname = (*iModule) -> name();
      modulenickname.replace(0,42,"");
      
      info() << modulenickname << " " << (*iModule) -> geometry() -> toGlobal( Gaudi::XYZPoint(0.,0.,0.) ) << endmsg;
      OutputModules << modulenickname << " " << (*iModule) -> geometry() -> toGlobal( Gaudi::XYZPoint(0.,0.,0.) ) << endl;
    }
    
    info() << endmsg;
    OutputModules.close();
    OutputLayers.close();
  }
      
  return TrackMonitorBase::finalize();  // must be called after all other actions
}

//=============================================================================
