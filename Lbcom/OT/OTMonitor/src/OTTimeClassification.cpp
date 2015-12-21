
// local
#include "OTTimeClassification.h"

// Gaudi
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/SystemOfUnits.h"

// Event
#include "Event/OTTime.h"
#include "Event/MCHit.h"

// OT geometry
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"

// Linker
#include "Linker/LinkedTo.h"


using namespace LHCb;

// BOOST
#include "boost/foreach.hpp"
#include <boost/assign.hpp>
#include "boost/lexical_cast.hpp"

using namespace boost;
using namespace boost::assign;

/** @file OTTimeClassifcation.cpp
 *
 *  Implementation of OTTimeClassification algorithm.
 *  @author M. Needham
 *  @date   20-07-2004
 */

/// Declaration of algorithm factory
DECLARE_ALGORITHM_FACTORY( OTTimeClassification )

OTTimeClassification::OTTimeClassification(const std::string& name, ISvcLocator* pSvcLocator)
  : GaudiHistoAlg(name, pSvcLocator)
  , m_decoder("OTRawBankDecoder")
  , m_tracker(0)

{
  // constructor
  m_spillVector += "/PrevPrev/", "/Prev/", "/", "/Next/", "/NextNext/";

  declareProperty("spillVector", m_spillVector);
}

OTTimeClassification::~OTTimeClassification()
{
  // destructor
}

StatusCode OTTimeClassification::initialize()
{
  StatusCode sc = GaudiHistoAlg::initialize();
  if(sc.isFailure()) return Error("Failed to initialize", sc);

  m_decoder.retrieve().ignore();

  // set path
  if("" == histoTopDir()) setHistoTopDir("OT/");

  m_tracker = getDet<DeOTDetector>(DeOTDetectorLocation::Default);

  // construct container names once
  BOOST_FOREACH(const std::string& spillName, m_spillVector)
  {
    m_spillNames += "/Event" + spillName + MCHitLocation::OT;
  }

  return StatusCode::SUCCESS;
}

StatusCode OTTimeClassification::execute()
{
  // linker
  LinkedTo<MCHit> link(evtSvc(), msgSvc(), OTTimeLocation::Default + "2MCHits");
  if(link.notFound()) Error( "Failed to find OT MCHits linker table", StatusCode::FAILURE );

  BOOST_FOREACH(const DeOTModule* module, m_tracker->modules())
  {
    OTChannelID moduleId = module->elementID();
    OTLiteTimeRange liteTimes = m_decoder->decodeModule(moduleId);

    BOOST_FOREACH(const OTLiteTime& liteTime, liteTimes)
    {
      unsigned int key = (unsigned int)liteTime.channel();
      LinkedTo<MCHit>::LRange range = link.range(key);

      if(range.empty()) m_infoMap["noise"]++;
      else fillInfo(range);
    } // foreach(liteTime)
  } // foreach(module)

  return StatusCode::SUCCESS;
}

StatusCode  OTTimeClassification::finalize()
{
  unsigned int total = 0;
  for(InfoIter it = m_infoMap.begin(); it != m_infoMap.end(); it++)
  {
    total += it->second;
  }

  unsigned int eventSpill = m_infoMap["primary"] + m_infoMap["secondary"] + m_infoMap["unknown"];
  unsigned int spillover = total - eventSpill - m_infoMap["noise"];

  info() << "--- OTTimeClassification ---------------" << endmsg;
  if( total > 0 ) {
    info() << "Event Spill " << eventSpill / double(total) << endmsg;
    info() << "   |---> Primary " << m_infoMap["primary"] / double(total) << endmsg;
    info() << "   |---> Secondary " << m_infoMap["secondary"] / double(total) << endmsg;
    info() << "   |---> Unknown " << m_infoMap["unknown"] / double(total) << endmsg;
    info() << "Noise " << m_infoMap["noise"] / double(total) << endmsg ;
    info() << "Spillover " << spillover / double(total) << endmsg;

    for(InfoIter it = m_infoMap.begin(); it != m_infoMap.end(); it++)
    {
      if(it->first != "noise" && it->first != "primary" && it->first != "unknown" && it->first != "secondary")
      {
        info() << it->first << " " << it->second / double(total) << endmsg;
      }
    }
  }
  else {
    info() << "No OTTimes found, classification not posible" << endmsg;
  }
  
  info() << "----------------------------------------" << endmsg;

  return  GaudiHistoAlg::finalize();
}

void OTTimeClassification::fillInfo(const std::vector<MCHit*>& hits)
{
  BOOST_FOREACH(MCHit* hit, hits)
  {
    std::string spill = findSpill(hit);

    if(spill == "/")
    {
      const MCParticle* particle = hit->mcParticle();
      if(particle != 0)
      {
        const MCVertex* origin = particle->originVertex();
        if(origin != 0)
        {
          Gaudi::XYZPoint point = origin->position();
          double r = std::hypot(point.x(), point.y());
          plot(point.z() / Gaudi::Units::cm, "z", 0.0, 1200.0, 600);
          plot2D(point.z() / Gaudi::Units::cm, r / Gaudi::Units::cm, "r vs z", 0.0, 1000.0, 0.0, 500.0, 500, 500);
          if(origin->type() == 0)
          {
            m_infoMap["unknown"]++;
          }
          else if(origin->type() < MCVertex::HadronicInteraction)
          {
            m_infoMap["primary"]++;
          }
          else
          {
            m_infoMap["secondary"]++;
          }
        } // if(origin != 0)
      } // if(particle != 0)
    }
    else // spill != "/"
    {
      m_infoMap[spill]++;
    }
  } // foreach(hit)
}

std::string OTTimeClassification::findSpill(const MCHit* hit) const
{
  const ObjectContainerBase* container = hit->parent();
  const IRegistry* registry = container->registry();
  const std::string& id = registry->identifier();

  unsigned int index = 0;
  while(index < m_spillNames.size() && m_spillNames[index] != id) ++index;

  return m_spillVector[index];
}
