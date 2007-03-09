// $Id: AlignOTDetector.cpp,v 1.1 2007-03-09 17:08:56 janos Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// from Kernel
#include "Kernel/OTChannelID.h"

// from OTDet
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTStation.h"
#include "OTDet/DeOTLayer.h"
#include "OTDet/DeOTModule.h"

// local
#include "AlignOTDetector.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlignOTDetector
//
// 2007-02-07 : Jan Amoraal
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( AlignOTDetector );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AlignOTDetector::AlignOTDetector( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IAlignDetectorSelector>(this);
  declareProperty("Align", m_align="Stations");
}

//=============================================================================
// Destructor
//=============================================================================
AlignOTDetector::~AlignOTDetector() {} 


StatusCode AlignOTDetector::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialize!", sc );

  DeOTDetector* tracker = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
  if (!tracker) {
    return Error("Failed to retrieve OT detector element", StatusCode::FAILURE);
  }
  if (m_align == "Stations") {
    const DeOTDetector::Stations& stations = tracker->stations();
    DeOTDetector::Stations::const_iterator iter = stations.begin();
    unsigned int index = 0;
    for ( ; iter < stations.end(); ++iter) {
      m_elements.push_back((*iter));
      m_mapUniqueIDToIndex.insert((*iter)->elementID().station(), index);
      ++index;
    } 
  } else if (m_align == "Layers") {
    const DeOTDetector::Layers& layers = tracker->layers();
    DeOTDetector::Layers::const_iterator iter = layers.begin();
    unsigned int index = 0;
    for ( ; iter < layers.end(); ++iter) {
      m_elements.push_back((*iter));
      m_mapUniqueIDToIndex.insert((*iter)->elementID().uniqueLayer(), index);
      ++index;
    }
  } else if (m_align == "Modules") {
    const DeOTDetector::Modules& modules = tracker->modules();
    DeOTDetector::Modules::const_iterator iter = modules.begin();
    unsigned int index = 0;
    for ( ; iter < modules.end(); ++iter) {
      m_elements.push_back((*iter));
      m_mapUniqueIDToIndex.insert((*iter)->elementID().uniqueModule(), index);
      ++index;
    }
  } else return Error("Unknown elements " + m_align + ". You can only align Stations, Layers or Modules", StatusCode::FAILURE);
 
  return StatusCode::SUCCESS;
}

// return index for a given channel id
AlignOTDetector::ValidIndex AlignOTDetector::index(const LHCb::LHCbID& anLHCbID) const
{
  std::pair<bool, unsigned int> valid;
  std::pair<bool, unsigned int> invalid = std::make_pair(false, 0u);
  
  if (anLHCbID.isOT()) {
    if (m_align == "Stations") {
      MapID::iterator iter = m_mapUniqueIDToIndex.find(anLHCbID.otID().station());
      if (iter != m_mapUniqueIDToIndex.end()) {
        valid = std::make_pair(true, iter->second);
      } else {
        valid = invalid;
      }
    } else if (m_align == "Layers") {
      MapID::iterator iter = m_mapUniqueIDToIndex.find(anLHCbID.otID().uniqueLayer());
      if (iter != m_mapUniqueIDToIndex.end()) {
        valid = std::make_pair(true, iter->second);
      } else {
        valid = invalid;
      }
    } else if (m_align == "Modules") {
      MapID::iterator iter = m_mapUniqueIDToIndex.find(anLHCbID.otID().uniqueModule());
      if (iter != m_mapUniqueIDToIndex.end()) {
        valid =  std::make_pair(true, iter->second);
      } else {
        valid = invalid;
      }
    }
  } else {
    debug() << "LHCb is not an OTChannelID or could not find elements " + m_align << endmsg;
    valid = invalid;
  }
  
  return valid;
}

//=============================================================================
