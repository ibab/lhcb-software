// $Id: AlignOTDetector.cpp,v 1.3 2007-08-16 13:48:33 graven Exp $
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

// from Boost
#include "boost/assign/std/vector.hpp"
#include "boost/assign/list_of.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lambda/lambda.hpp"

// local
#include "AlignOTDetector.h"

//-----------------------------------------------------------------------------
// Implementation file for class : AlignOTDetector
//
// 2007-02-07 : Jan Amoraal
//-----------------------------------------------------------------------------

using namespace LHCb;
using namespace boost;
using namespace boost::assign;
using namespace boost::lambda;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( AlignOTDetector );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
AlignOTDetector::AlignOTDetector( const std::string& type,
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
AlignOTDetector::~AlignOTDetector() {} 

StatusCode AlignOTDetector::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialize!", sc );

  DeOTDetector* tracker = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
  if (!tracker) return Error("Failed to retrieve OT detector element", StatusCode::FAILURE);
  
  if (m_align == "Stations") {
    const DeOTDetector::Stations& stations = tracker->stations();
    m_indexMap = createIndices(stations.begin()                               , 
                                              stations.end()                                 , 
                                              m_fixed                                        ,
                                              bind<OTChannelID>(&DeOTStation::elementID, _1) , 
                                              bind<unsigned>(&OTChannelID::station, _1)      , 
                                              m_elements                                     );
  } else if (m_align == "Layers") {
    const DeOTDetector::Layers& layers = tracker->layers();
    m_indexMap = createIndices(layers.begin()                                , 
                                              layers.end()                                  , 
                                              m_fixed                                       ,
                                              bind<OTChannelID>(&DeOTLayer::elementID, _1)  , 
                                              bind<unsigned>(&OTChannelID::uniqueLayer, _1) , 
                                              m_elements                                    );
  } else if (m_align == "Modules") {
    const DeOTDetector::Modules& modules = tracker->modules();
    m_indexMap = createIndices(modules.begin()                                , 
                                              modules.end()                                  , 
                                              m_fixed                                       ,
                                              bind<OTChannelID>(&DeOTModule::elementID, _1)  , 
                                              bind<unsigned>(&OTChannelID::uniqueModule, _1) , 
                                              m_elements                                    );
  } else return Error("Unknown elements " + m_align + ". You can only align Stations, Layers or Modules", StatusCode::FAILURE);
 
  return StatusCode::SUCCESS;
}

// return index for a given channel id
BoolIndex AlignOTDetector::index(const LHCbID& anLHCbID) const
{
  
  BoolIndex boolIndex;
  if (m_align == "Stations") {
    boolIndex = getAssociatedIndex(anLHCbID                                , 
                                                m_indexMap                              , 
                                                bind<bool>(&LHCbID::isOT, _1)           , 
                                                bind<OTChannelID>(&LHCbID::otID,_1)     , 
                                                bind<unsigned>(&OTChannelID::station,_1)); 
  } else if (m_align == "Layers") {
    boolIndex = getAssociatedIndex(anLHCbID                                    , 
                                                m_indexMap                                  , 
                                                bind<bool>(&LHCbID::isOT, _1)               , 
                                                bind<OTChannelID>(&LHCbID::otID,_1)         , 
                                                bind<unsigned>(&OTChannelID::uniqueLayer,_1));
  } else if (m_align == "Modules") {
    boolIndex = getAssociatedIndex(anLHCbID                                     , 
                                                m_indexMap                                   ,
                                                bind<bool>(&LHCbID::isOT, _1)                , 
                                                bind<OTChannelID>(&LHCbID::otID,_1)          , 
                                                bind<unsigned>(&OTChannelID::uniqueModule,_1));
  }
  
  return boolIndex;
}

//=============================================================================
