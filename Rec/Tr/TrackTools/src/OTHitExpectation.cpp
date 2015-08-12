// from GaudiKernel
#include "GaudiKernel/SystemOfUnits.h"

// Event
#include "Event/Track.h"
#include "Event/State.h"

// Tsa
#include "TsaKernel/Line.h"
#include "TsaKernel/Parabola.h"
#include "TsaKernel/IOTExpectedHits.h"

#include "LHCbMath/GeomFun.h"
#include "OTDet/DeOTDetector.h"

#include "LoKi/select.h"

// local
#include "OTHitExpectation.h"

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/assign/std/vector.hpp>
#include <boost/assign/list_of.hpp>

#include <algorithm>
#include <utility>

using namespace boost::lambda;
using namespace LHCb;
using namespace Gaudi;

//namespace {
//
//  bool sameChan(const LHCbID& chan1, const LHCbID& chan2 ){
//      return (chan1.otID().geometry() == chan2.otID().geometry());
//  }
//}

DECLARE_TOOL_FACTORY( OTHitExpectation )

//=============================================================================
// 
//=============================================================================
OTHitExpectation::OTHitExpectation(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent)
  : THitExpectation(type, name, parent)
  , m_otDet(NULL), m_expectedOTHits(NULL)
{ 
  // constructer
  std::vector<double> tmp = boost::assign::list_of(0.261)(5.1)(11.87);
  declareProperty("likPar", m_likPar = tmp);

}

//=============================================================================
// 
//=============================================================================
OTHitExpectation::~OTHitExpectation(){
  // destructer
}

//=============================================================================
// 
//=============================================================================
StatusCode OTHitExpectation::initialize()
{

  StatusCode sc = THitExpectation::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize", sc);
  }

  m_expectedOTHits = tool<Tf::Tsa::IOTExpectedHits>("Tf::Tsa::OTExpectedHits");

  m_otDet = getDet<DeOTDetector>(DeOTDetectorLocation::Default);

  return StatusCode::SUCCESS;
}


IHitExpectation::Info OTHitExpectation::expectation(const LHCb::Track& aTrack) const{

  IHitExpectation::Info info;
  info.nExpected = 0;
  info.nFound = 0;
  info.likelihood = 0.0;

  const std::vector<LHCb::LHCbID>& ids = aTrack.lhcbIDs();
  std::vector<LHCb::LHCbID> otHits; otHits.reserve(ids.size());
  LoKi::select(ids.begin(), ids.end(), std::back_inserter(otHits), bind(&LHCbID::isOT,_1));
  
  Tf::Tsa::Parabola aParab(0.,0.,0.);
  Tf::Tsa::Line aLine(0.,0.);

  const DeOTDetector::Layers& layers = m_otDet->layers();
  for (DeOTDetector::Layers::const_iterator iterL = layers.begin(); 
       iterL != layers.end() ; ++iterL ){  

    DeOTQuarter* quart = (*iterL)->quarters().front();
    DeOTModule* module = quart->modules().front();
    aParab = xParabola(aTrack,module->centerOfModule().z());
    aLine = yLine(aTrack,module->centerOfModule().z());
  
    LHCb::OTChannelID testChan = channelHint((*iterL)->elementID(),ids);
    OTPairs output;


    if (testChan.module() == 0){
	// try both sectors...
	for (unsigned int iSector = 3; iSector <=4; ++iSector){
	    StatusCode sc = m_expectedOTHits->collect(aParab,aLine,testChan,output, iSector); 
	    if (sc.isFailure()){
		Warning("Failed to calculate expected hits",StatusCode::SUCCESS,1).ignore();
	    }  
	}  // sectors
    }
    else {
	StatusCode sc = m_expectedOTHits->collect(aParab,aLine,testChan,output, 0);    
	if (sc.isFailure()){
            Warning("Failed to calculate expected hits",StatusCode::SUCCESS,1).ignore();
        }
    }
    
    for (OTPairs::iterator iter = output.begin() ;iter != output.end(); ++iter ){

      std::vector<LHCbID>::const_iterator lIter = otHits.begin();
      while (lIter != otHits.end() && lIter->otID().geometry() != iter->first.geometry()) ++lIter; 
 
      ++(info.nExpected);
      if (lIter != otHits.end() ) {
        ++(info.nFound);
      }
      else {
        info.likelihood += log( m_likPar[0] + exp( m_likPar[1] * fabs(iter->second) - m_likPar[2] ) );
      } // if                                           
    }  // pairs
   
  } // layers

  return info;
}

void OTHitExpectation::collect(const LHCb::Track& aTrack ,std::vector<LHCb::LHCbID>& ids) const{
  
  const std::vector<LHCb::LHCbID>& idsOnTrack = aTrack.lhcbIDs();
  std::vector<LHCb::LHCbID> otHits; otHits.reserve(idsOnTrack.size());
  LoKi::select(idsOnTrack.begin(), idsOnTrack.end(), std::back_inserter(otHits), bind(&LHCbID::isOT,_1));
  
  Tf::Tsa::Parabola aParab(0.,0.,0.);
  Tf::Tsa::Line aLine(0.,0.);

  const DeOTDetector::Layers& layers = m_otDet->layers();
  for (DeOTDetector::Layers::const_iterator iterL = layers.begin(); 
       iterL != layers.end() ; ++iterL ){  

    DeOTQuarter* quart = (*iterL)->quarters().front();
    DeOTModule* module = quart->modules().front();
    aParab = xParabola(aTrack,module->centerOfModule().z());
    aLine = yLine(aTrack,module->centerOfModule().z());
  
    LHCb::OTChannelID testChan = channelHint((*iterL)->elementID(),ids);
    OTPairs output;


    if (testChan.module() == 0){	
	// try both sectors...
	for (unsigned int iSector = 3; iSector <=4; ++iSector){
	    StatusCode sc = m_expectedOTHits->collect(aParab,aLine,testChan,output, iSector); 
	    if (sc.isFailure()){
		Warning("Failed to calculate expected hits",StatusCode::SUCCESS,1).ignore();
	    }  
	}  // sectors
    }
    else {
        StatusCode sc = m_expectedOTHits->collect(aParab,aLine,testChan,output, 0);
        if (sc.isFailure()){
            Warning("Failed to calculate expected hits",StatusCode::SUCCESS,1).ignore();
        }
    }

    ids.reserve(output.size());
    for (OTPairs::iterator iter = output.begin() ;iter != output.end(); ++iter ){
      ids.push_back(LHCbID(iter->first)); 
    }  // pairs
   
  } // layers

}

unsigned int OTHitExpectation::nExpected(const LHCb::Track& aTrack) const{

  IHitExpectation::Info info = expectation(aTrack);
  return info.nExpected;
}

LHCb::OTChannelID OTHitExpectation::channelHint(const OTChannelID& hintChan,
                                               const std::vector<LHCbID>& ids) const{
   
 std::vector<LHCbID>::const_iterator iter = ids.begin();

 while (iter != ids.end() && 
   iter->otID().uniqueLayer() != hintChan.uniqueLayer() ){
   ++iter;
 } //iter

 return (iter == ids.end() ? hintChan : OTChannelID(iter->otID().geometry()) );
}
