// $Id: AlignOTDetector.cpp,v 1.2 2007-07-20 17:24:33 janos Exp $
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
  : GaudiTool ( type, name , parent )
{
  declareInterface<IAlignDetectorSelector>(this);
  declareProperty("Align", m_align="Stations");
  m_elementFixed = boost::assign::list_of(false)(false)(false);
  declareProperty("Fixed", m_elementFixed);
}

//=============================================================================
// Destructor
//=============================================================================
AlignOTDetector::~AlignOTDetector() {} 

template <typename ID, typename ITER, typename FUNC>
void AlignOTDetector::createIndices(ITER i, ITER end, FUNC functor) {
  for (unsigned(index) = 0u, fixed = 0u; i != end; ++i) {
    /// Note: ++not++ does not apply here. I really do want to post-increment. 
    if (m_elementFixed[fixed++]) continue;
    m_elements.push_back((*i));
    const ID aChannel = (*i)->elementID();
    /// Note: ++not++ does not apply here. I really do want to post-increment.
    /// functor(x) == x->functor()
    m_mapUniqueIDToIndex.insert(functor(aChannel), index++);
  }
}

template <typename ID, typename MAP, typename FUNCCHECKDET, typename FUNCLHCB, typename FUNCSUBDET>
AlignOTDetector::BoolIndex AlignOTDetector::getAssociatedIndex(const LHCbID& anLHCbID, MAP map, FUNCCHECKDET funcCheckDet, 
                                                               FUNCLHCB funcLHCb, FUNCSUBDET funcSubDet) const {
  /// functor(x) == x->functor()
  if (!funcCheckDet(anLHCbID)) return std::make_pair(false, 0u);
  const ID aChannel           = funcLHCb(anLHCbID);
  typename MAP::iterator iter = map.find(funcSubDet(aChannel));
  return iter != map.end() ? std::make_pair(true, iter->second) : std::make_pair(false, 0u); 
}

StatusCode AlignOTDetector::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialize!", sc );

  DeOTDetector* tracker = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
  if (!tracker) return Error("Failed to retrieve OT detector element", StatusCode::FAILURE);
  
  if (m_align == "Stations") {
    const DeOTDetector::Stations& stations = tracker->stations();
    createIndices<OTChannelID>(stations.begin(), stations.end(), bind<unsigned>(&OTChannelID::station, _1));
  } else if (m_align == "Layers") {
    const DeOTDetector::Layers& layers = tracker->layers();
    createIndices<OTChannelID>(layers.begin(), layers.end(), bind<unsigned>(&OTChannelID::uniqueLayer, _1));
  } else if (m_align == "Modules") {
    const DeOTDetector::Modules& modules = tracker->modules();
    createIndices<OTChannelID>(modules.begin(), modules.end(), bind<unsigned>(&OTChannelID::uniqueModule, _1));
  } else return Error("Unknown elements " + m_align + ". You can only align Stations, Layers or Modules", StatusCode::FAILURE);
 
  return StatusCode::SUCCESS;
}

// return index for a given channel id
AlignOTDetector::BoolIndex AlignOTDetector::index(const LHCbID& anLHCbID) const
{
  
  BoolIndex boolIndex;
  if (m_align == "Stations") {
    boolIndex = getAssociatedIndex<OTChannelID>(anLHCbID, m_mapUniqueIDToIndex, 
                                                bind<bool>(&LHCbID::isOT, _1), 
                                                bind<OTChannelID>(&LHCbID::otID,_1), 
                                                bind<unsigned>(&OTChannelID::station,_1)); 
  } else if (m_align == "Layers") {
    boolIndex = getAssociatedIndex<OTChannelID>(anLHCbID, m_mapUniqueIDToIndex, 
                                                bind<bool>(&LHCbID::isOT, _1), 
                                                bind<OTChannelID>(&LHCbID::otID,_1), 
                                                bind<unsigned>(&OTChannelID::uniqueLayer,_1));
  } else if (m_align == "Modules") {
    boolIndex = getAssociatedIndex<OTChannelID>(anLHCbID, m_mapUniqueIDToIndex, 
                                                bind<bool>(&LHCbID::isOT, _1), 
                                                bind<OTChannelID>(&LHCbID::otID,_1), 
                                                bind<unsigned>(&OTChannelID::uniqueModule,_1));
  }
  
  return boolIndex;
}

//=============================================================================
