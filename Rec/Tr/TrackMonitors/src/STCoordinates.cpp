// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// local
#include "STCoordinates.h"

// detector
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTLayer.h"
#include "STDet/DeSTSector.h"
#include "STDet/DeSTSensor.h"
#include "STDet/DeITDetector.h"
#include "STDet/DeITBox.h"

// kernel
#include "Kernel/STLexicalCaster.h"

//
#include <iostream>
#include <fstream>

using namespace LHCb;
using namespace ST;
using namespace Gaudi;
using namespace std;

//-----------------------------------------------------------------------------
// Implementation file for class : STCoordinates
//
// 2010-05-31 : Frederic Guillaume Dupertuis
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( STCoordinates )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
STCoordinates::STCoordinates( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : TrackMonitorBase ( name , pSvcLocator )
{
  declareProperty( "DetType"         , m_detType =  "IT" );
  declareProperty( "PrintSectorInfo" , m_printsectorinfo = true );
  declareProperty( "AlignmentTag"    , m_alignmenttag = "v4_2" );
}
//=============================================================================
// Destructor
//=============================================================================
STCoordinates::~STCoordinates() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode STCoordinates::initialize() {
  StatusCode sc = TrackMonitorBase::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  
  m_tracker = getDet< DeSTDetector >( DeSTDetLocation::location( m_detType ) );

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode STCoordinates::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode STCoordinates::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  
  if(m_printsectorinfo){
    const DeSTDetector::Sectors& sectors = m_tracker->sectors();
    const DeSTDetector::Layers& layers = m_tracker->layers();

    std::string OutputNameSectors(m_detType+"Coordinates-"+m_alignmenttag+"-Sectors.txt");
    std::string OutputNameLayers(m_detType+"Coordinates-"+m_alignmenttag+"-Layers.txt");
    
    ofstream OutputSectors(OutputNameSectors.c_str());
    ofstream OutputLayers(OutputNameLayers.c_str());
            
    info() << m_detType << endmsg;
    info() << endmsg;
    
    if(m_detType == "IT"){
      DeITDetector* m_ITtracker = dynamic_cast<DeITDetector*>(m_tracker);
      std::string OutputNameBoxes(m_detType+"Coordinates-"+m_alignmenttag+"-Boxes.txt");
      ofstream OutputBoxes(OutputNameBoxes.c_str());
      if(m_ITtracker != 0){
        const DeITDetector::Boxes& boxes = m_ITtracker->boxes();
        DeITDetector::Boxes::const_iterator iBox = boxes.begin();
        for ( ; iBox != boxes.end(); ++iBox)
        {
          info() << (*iBox) -> nickname() << " " << (*iBox) -> geometry() -> toGlobal( Gaudi::XYZPoint(0.,0.,0.) ) << endmsg;
          OutputBoxes << (*iBox) -> nickname() << " " << (*iBox) -> geometry() -> toGlobal( Gaudi::XYZPoint(0.,0.,0.) ) << endl;
        }
      }
      info() << endmsg;
      OutputBoxes.close();
    }
    
    DeSTDetector::Layers::const_iterator iLayer = layers.begin();
    
    for ( ; iLayer != layers.end(); ++iLayer)
    {
      info() << (*iLayer) -> nickname() << " " << (*iLayer) -> geometry() -> toGlobal( Gaudi::XYZPoint(0.,0.,0.) ) << endmsg;
      OutputLayers << (*iLayer) -> nickname() << " " << (*iLayer) -> geometry() -> toGlobal( Gaudi::XYZPoint(0.,0.,0.) ) << endl;
    }
    
    info() << endmsg;
        
    DeSTDetector::Sectors::const_iterator iSect = sectors.begin();
    
    for ( ; iSect != sectors.end(); ++iSect)
    {
      info() << (*iSect) -> nickname() << " " << (*iSect) -> geometry() -> toGlobal( Gaudi::XYZPoint(0.,0.,0.) ) << endmsg;
      OutputSectors << (*iSect) -> nickname() << " " << (*iSect) -> geometry() -> toGlobal( Gaudi::XYZPoint(0.,0.,0.) ) << endl;
    }
    
    info() << endmsg;
    OutputSectors.close();
    OutputLayers.close();
  }
      
  return TrackMonitorBase::finalize();  // must be called after all other actions
}

//=============================================================================
