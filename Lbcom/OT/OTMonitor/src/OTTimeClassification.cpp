// Inlcudes
// Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/SystemOfUnits.h"

// Event
#include "Event/OTTime.h"
#include "Event/MCHit.h"

// Linker
#include "Linker/LinkedTo.h"

// BOOST
#include "boost/lexical_cast.hpp"
#include <boost/assign/std/vector.hpp>

// GSL
#include "gsl/gsl_math.h"

// local
#include "OTTimeClassification.h"

/** @file OTTimeClassifcation.cpp
*
*  Implementation of OTTimeClassification algorithm.
*  @author M. Needham
*  @date   20-07-2004
*/

using namespace LHCb;
using namespace boost;
using namespace boost::assign;

/// Declaration of algorithm factory
DECLARE_ALGORITHM_FACTORY( OTTimeClassification );

OTTimeClassification::OTTimeClassification(const std::string& name, 
                                           ISvcLocator* pSvcLocator) :
GaudiHistoAlg(name, pSvcLocator) 
{
  // constructer
  m_spillVector += "/PrevPrev/", "/Prev/", "/", "/Next/", "/NextNext/"; 
  declareProperty("spillVector", m_spillVector);
}

OTTimeClassification::~OTTimeClassification(){
  // destructer
}

StatusCode OTTimeClassification::initialize(){
   
  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  /// set path
  if ("" == histoTopDir()) setHistoTopDir("OT/");

  m_hitLocation = MCHitLocation::OT;
  m_timesLocation = OTTimeLocation::Default;
  m_asctLocation = m_timesLocation + "2MCHits";

  // construct container names once
  std::vector<std::string>::const_iterator iSpillName = m_spillVector.begin() ;
  for ( ; iSpillName != m_spillVector.end(); ++iSpillName ) {
    std::string mcHitPath = "/Event"+(*iSpillName)+m_hitLocation;
    m_spillNames.push_back(mcHitPath);    
  }
  
  return StatusCode::SUCCESS;
}

StatusCode OTTimeClassification::execute(){

  // retrieve clusters
  OTTimes* times = get<OTTimes>(m_timesLocation);

  // linker
  LinkedTo<MCHit> myLink(evtSvc(), msgSvc(), m_asctLocation);
  if (myLink.notFound()) Error( "Failed to find OT MCHits linker table", StatusCode::FAILURE );

  // histos per digit
  OTTimes::const_iterator iTime = times->begin() ;
  for ( ; iTime != times->end(); ++iTime) {
    unsigned int key = unsigned((*iTime)->channel()); 
    std::vector<MCHit*> range = myLink.range(key);
    if (range.empty()) {
      //empty range
      ++m_infoMap["noise"];
    } else {
      fillInfo(range);
    }
  } // loop iterDep

  return StatusCode::SUCCESS;
}

StatusCode  OTTimeClassification::finalize(){

 // normalization
 unsigned int total = tCount();
 unsigned int eventSpill = m_infoMap["primary"] + m_infoMap["secondary"] + m_infoMap["unknown"];
 unsigned int spillover = total - eventSpill - m_infoMap["noise"];

 info() << "--- OTTimes Classification " << endmsg;
 info() << "Event Spill " << eventSpill/ double(total) << endmsg;
 info() << "   |---> Primary " << m_infoMap["primary"]/ double(total) << endmsg;
 info() << "   |---> Secondary " << m_infoMap["secondary"]/ double(total) << endmsg;
 info() << "   |---> Unknown " << m_infoMap["unknown"]/ double(total) << endmsg;
 info() << "Noise " << m_infoMap["noise"]/double(total) << endmsg ;
 info() << "Spillover "  << spillover/double(total) << endmsg;
 
 typedef std::map<std::string, unsigned int>::const_iterator InfoIter; 
 InfoIter iter = m_infoMap.begin();
 for ( ; iter != m_infoMap.end(); ++iter) {
   if ((iter->first != "noise")&&(iter->first != "primary") && (iter->first != "unknown") 
       &&(iter->first != "secondary")) {     
     info() << iter->first+" " << iter->second/ double(total) << endmsg;
   }
 }

 info() <<"----------------------------------------" << endmsg;

 return  GaudiHistoAlg::finalize();
}

unsigned int OTTimeClassification::tCount() const{
  unsigned int total = 0u;
  
  InfoIter iter = m_infoMap.begin();
  for ( ; iter != m_infoMap.end(); ++iter) total += iter->second; 
  
  return total;
}

void OTTimeClassification::fillInfo(const std::vector<MCHit*>& hits) const {
  std::vector<MCHit*>::const_iterator iHit = hits.begin();
  for ( ; iHit != hits.end(); ++iHit) {
    // find out which spill came from...
    const MCHit* aHit = (*iHit);
    std::string spill = this->findSpill(aHit);
    
    if (spill == "/") {
      // event spill...
      const MCParticle* particle = aHit->mcParticle();
      if (0 != particle) {
        const MCVertex* origin = particle->originVertex();
        /// Where does the hit come from (in current spill)?
        if (origin != 0) {
          Gaudi::XYZPoint point = origin->position();
          double r = gsl_hypot(point.x(), point.y());
          plot(point.z()/Gaudi::Units::cm, "z", 0.0, 1200.0, 600 );
          plot2D(point.z()/Gaudi::Units::cm, r/Gaudi::Units::cm, "r vs z", 0.0, 1000.0, 0.0, 500.0, 500, 500); 
          if ((origin->type() < MCVertex::HadronicInteraction)&&(origin->type() != 0)) {
          ++m_infoMap["primary"];  
        } else if (origin->type() == 0) {
          ++m_infoMap["unknown"];
        } else {
          ++m_infoMap["secondary"];
        }
        }  // vertex
      } // particle
    } else {
    ++m_infoMap[spill];   
    } 
  }
}

std::string OTTimeClassification::findSpill(const MCHit* aHit) const {
 
  const ObjectContainerBase* parentCont = aHit->parent();
  const IRegistry* reg =  parentCont->registry();
  const std::string& idName = reg->identifier();

  unsigned int index = 0;
  while (index<m_spillNames.size()&&(m_spillNames[index]!=idName)) ++index;
  
  return m_spillVector[index];
}











