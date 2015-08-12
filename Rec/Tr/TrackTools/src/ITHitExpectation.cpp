// from GaudiKernel
#include "GaudiKernel/SystemOfUnits.h"

// Event
#include "Event/Track.h"
#include "Event/State.h"

#include "Kernel/ISTChannelIDSelector.h"

// Tsa
#include "TsaKernel/Line.h"
#include "TsaKernel/Parabola.h"
#include "TsaKernel/IITExpectedHits.h"

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>

#include "LoKi/select.h"

#include "LHCbMath/GeomFun.h"
#include "STDet/DeSTDetector.h"

// local
#include "ITHitExpectation.h"

#include <algorithm>
#include <utility>

#include "LHCbMath/GeomFun.h"

using namespace boost::lambda;
using namespace LHCb;
using namespace Gaudi;

DECLARE_TOOL_FACTORY( ITHitExpectation )


//=============================================================================
// 
//=============================================================================
ITHitExpectation::ITHitExpectation(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent)
  : THitExpectation(type, name, parent)
  , m_itDet(NULL), m_expectedITHits(NULL)
  , m_selector(NULL)
{ 
  // constructer
  declareProperty( "SelectorType", m_selectorType = "STSelectChannelIDByElement" );
  declareProperty( "SelectorName", m_selectorName = "ALL" );
  declareProperty( "allStrips", m_allStrips = false);
}

//=============================================================================
// 
//=============================================================================
ITHitExpectation::~ITHitExpectation(){
  // destructer
}

//=============================================================================
// 
//=============================================================================
StatusCode ITHitExpectation::initialize()
{

  StatusCode sc = THitExpectation::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  m_expectedITHits = tool<Tf::Tsa::IITExpectedHits>("Tf::Tsa::ITExpectedHits");

  // need to know the z of T stations....
  m_itDet = getDet<DeSTDetector>(DeSTDetLocation::location("IT"));

  // (selector
  if (m_selectorName != "ALL"){
    info() << "Selector name " << m_selectorName << endmsg;
    m_selector  = tool< ISTChannelIDSelector >( m_selectorType,m_selectorName );
  }
  return StatusCode::SUCCESS;
}


IHitExpectation::Info ITHitExpectation::expectation(const LHCb::Track& aTrack) const{

  IHitExpectation::Info info;
  info.likelihood = 0.0;
  info.nFound = 0;
  info.nExpected  = 0;

  const std::vector<LHCb::LHCbID>& ids = aTrack.lhcbIDs();
  std::vector<LHCb::LHCbID> itHits; itHits.reserve(ids.size());
  LoKi::select(ids.begin(), ids.end(), std::back_inserter(itHits), bind(&LHCbID::isIT,_1));

  Tf::Tsa::Parabola aParab(0.,0.,0.);
  Tf::Tsa::Line aLine(0.,0.);

  const DeSTDetector::Layers& layers = m_itDet->layers();
  // std::vector<unsigned int> expected;

  for (DeSTDetector::Layers::const_iterator iterL = layers.begin(); 
       iterL != layers.end() ; ++iterL ){  

    STChannelID chan = (*iterL)->elementID();

    aParab = xParabola(aTrack,(*iterL)->globalCentre().z());
    aLine = yLine(aTrack,(*iterL)->globalCentre().z());

    ITPairs output;
    int sect = boxToSector(chan);
    
    // to avoid double counting !
    if (chan.detRegion() == 1 && aParab.value((*iterL)->globalCentre().z()) > 0.) continue;
    if (chan.detRegion() == 2 && aParab.value((*iterL)->globalCentre().z()) < 0.) continue;

    StatusCode sc = m_expectedITHits->collect(aParab,aLine,chan,output,sect);   
    if (sc.isFailure()){
      Warning("Failed to calculate expected hits",StatusCode::SUCCESS,1).ignore();
    }
    int old = -1;
    for (ITPairs::iterator iter = output.begin(); iter != output.end(); ++iter ){ 
      if ( int(iter->second) != old) {
        old = int(iter->second);
        if (select(iter->first)) {
           ++(info.nExpected);
           std::vector<LHCb::LHCbID>::iterator itIter = std::find(itHits.begin(), itHits.end(), iter->first); 
           if (itIter != itHits.end() ) ++(info.nFound);
	}
      }
    }  // pairs      
  } //layer
  

  return info;
}


void ITHitExpectation::collect(const LHCb::Track& aTrack ,std::vector<LHCb::LHCbID>& ids) const{

  Tf::Tsa::Parabola aParab(0.,0.,0.);
  Tf::Tsa::Line aLine(0.,0.);

  const DeSTDetector::Layers& layers = m_itDet->layers();

  for (DeSTDetector::Layers::const_iterator iterL = layers.begin(); 
       iterL != layers.end() ; ++iterL ){  

    STChannelID chan = (*iterL)->elementID();

    aParab = xParabola(aTrack,(*iterL)->globalCentre().z());
    aLine = yLine(aTrack,(*iterL)->globalCentre().z());

    ITPairs output;
    int sect = boxToSector(chan);
    
    // to avoid double counting !
    if (chan.detRegion() == 1 && aParab.value((*iterL)->globalCentre().z()) > 0.) continue;
    if (chan.detRegion() == 2 && aParab.value((*iterL)->globalCentre().z()) < 0.) continue;

    StatusCode sc = m_expectedITHits->collect(aParab,aLine,chan,output,sect);   
    if (sc.isFailure()){
      Warning("Failed to calculate expected hits",StatusCode::SUCCESS,1).ignore();
    }
    int old = -1;
    ids.reserve(output.size());
    for (ITPairs::iterator iter = output.begin(); iter != output.end(); ++iter ){ 
      if ( int(iter->second) != old || m_allStrips) {
        old = int(iter->second);
        if (select(iter->first)) ids.push_back(LHCb::LHCbID(iter->first));
      } // test on old or take all
    }  // pairs      
  } //layer
  
}

unsigned int ITHitExpectation::nExpected(const LHCb::Track& aTrack) const{

  return expectation(aTrack).nExpected;
}  

int ITHitExpectation::boxToSector(const LHCb::STChannelID& chan) const{

 // convert box to sector
 int sect = 0;
 if (chan.detRegion() == 3) {
   sect = 1;
 }
 else if (chan.detRegion() == 4){
   sect = 2;
 }
 return sect;
}
