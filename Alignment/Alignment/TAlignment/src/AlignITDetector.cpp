// $Id: AlignITDetector.cpp,v 1.1 2007-10-15 16:24:49 lnicolas Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// from Kernel
#include "Kernel/STChannelID.h"

// from STDet
#include "STDet/DeITDetector.h"
#include "STDet/DeITStation.h"
#include "STDet/DeITLayer.h"
#include "STDet/DeITBox.h"
#include "STDet/DeITSector.h"
#include "STDet/DeITLadder.h"

// from Boost
#include "boost/assign/std/vector.hpp"
#include "boost/assign/list_of.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lambda/lambda.hpp"

// local
#include "AlignITDetector.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlignITDetector
//
// 2007-02-07 : Jan Amoraal
//-----------------------------------------------------------------------------

using namespace LHCb;
using namespace boost;
using namespace boost::assign;
using namespace boost::lambda;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( AlignITDetector );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AlignITDetector::AlignITDetector( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_constraints(0)
{
  declareInterface<IAlignDetectorSelector>(this);
  declareProperty("Align", m_align="Stations");
  m_fixed = boost::assign::list_of(false)(false)(false);
  declareProperty("Fixed", m_fixed);
  declareProperty("Constraints", m_constraints);
}

//=============================================================================
// Destructor
//=============================================================================
AlignITDetector::~AlignITDetector() {} 

StatusCode AlignITDetector::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialize!", sc );
  
  DeITDetector* tracker = getDet<DeITDetector>(DeSTDetLocation::IT);
  if (!tracker) return Error("Failed to retrieve OT detector element", StatusCode::FAILURE);
  
  if (m_align == "Stations") {
    const DeITDetector::Stations& stations = tracker->stations();
    m_indexMap = createIndices(stations.begin()                               , 
                               stations.end()                                 , 
                               m_fixed                                        ,
                               bind<STChannelID>(&DeITStation::elementID, _1) , 
                               bind<unsigned>(&STChannelID::station, _1)      , 
                               m_elements                                     );
  } else if (m_align == "Layers") {
    const DeITDetector::Layers& layers = tracker->layers();
    m_indexMap = createIndices(layers.begin()                                , 
                               layers.end()                                  , 
                               m_fixed                                       ,
                               bind<STChannelID>(&DeITLayer::elementID, _1)  , 
                               bind<unsigned>(&STChannelID::uniqueLayer, _1) , 
                               m_elements                                    );
  } else if (m_align == "Boxes") {
    const DeITDetector::Boxes& boxes = tracker->boxes();
    m_indexMap = createIndices(boxes.begin()                                , 
                               boxes.end()                                  , 
                               m_fixed                                       ,
                               bind<STChannelID>(&DeITSector::elementID, _1)  , 
                               bind<unsigned>(&STChannelID::uniqueDetRegion, _1) , 
                               m_elements                                    );
  } else if (m_align == "Sectors") {
    const DeITDetector::Sectors& sectors = tracker->sectors();
    m_indexMap = createIndices(sectors.begin()                                , 
                               sectors.end()                                  , 
                               m_fixed                                       ,
                               bind<STChannelID>(&DeITSector::elementID, _1)  , 
                               bind<unsigned>(&STChannelID::uniqueSector, _1) , 
                               m_elements                                    );
  } else return Error("Unknown elements " + m_align 
                      + ". You can only align Stations, Layers, Boxes or Sectors", StatusCode::FAILURE);
  
  return StatusCode::SUCCESS;
}

// return index for a given channel id
BoolIndex AlignITDetector::index(const LHCbID& anLHCbID) const
{
  
  BoolIndex boolIndex;
  if (m_align == "Stations") {
    boolIndex = getAssociatedIndex(anLHCbID                                , 
                                   m_indexMap                              , 
                                   bind<bool>(&LHCbID::isIT, _1)           , 
                                   bind<STChannelID>(&LHCbID::stID,_1)     , 
                                   bind<unsigned>(&STChannelID::station,_1)); 
  } else if (m_align == "Layers") {
    boolIndex = getAssociatedIndex(anLHCbID                                    , 
                                   m_indexMap                                  , 
                                   bind<bool>(&LHCbID::isIT, _1)               , 
                                   bind<STChannelID>(&LHCbID::stID,_1)         , 
                                   bind<unsigned>(&STChannelID::uniqueLayer,_1));
  } else if (m_align == "Boxes") {
    boolIndex = getAssociatedIndex(anLHCbID                                     , 
                                   m_indexMap                                   ,
                                   bind<bool>(&LHCbID::isIT, _1)                , 
                                   bind<STChannelID>(&LHCbID::stID,_1)          , 
                                   bind<unsigned>(&STChannelID::uniqueDetRegion,_1));
  } else if (m_align == "Sectors") {
    boolIndex = getAssociatedIndex(anLHCbID                                     , 
                                   m_indexMap                                   ,
                                   bind<bool>(&LHCbID::isIT, _1)                , 
                                   bind<STChannelID>(&LHCbID::stID,_1)          , 
                                   bind<unsigned>(&STChannelID::uniqueSector,_1));
  }
  
  return boolIndex;
}

//=============================================================================
