// $Id: ITGenericTracking.cpp,v 1.4 2008-10-24 13:33:34 mneedham Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// DigiEvent
#include "Event/STCluster.h"

// STDet
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

// local
#include "ITGenericTracking.h"

#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"
#include "LHCbMath/GeomFun.h"

#include "Kernel/ITNames.h"
#include "Kernel/Trajectory.h"
#include "TfKernel/STHit.h"
#include "TfKernel/RecoFuncs.h"

#include "LoKi/select.h"
#include "TsaKernel/Line.h"
#include "TsaKernel/TsaConstants.h"


#include "Event/Track.h"
#include "Event/State.h"
#include "Event/StateParameters.h"

using namespace LHCb;

// Boost
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/assign/std/vector.hpp>

#include "TrackInterfaces/IHitExpectation.h"

using namespace boost::lambda;

DECLARE_ALGORITHM_FACTORY( ITGenericTracking );

//--------------------------------------------------------------------
//
//  ITGenericTracking : Monitoring class for the STClusters
//
//--------------------------------------------------------------------

ITGenericTracking::ITGenericTracking( const std::string& name, 
                                    ISvcLocator* pSvcLocator ) :
  GaudiHistoAlg(name, pSvcLocator),
  m_tracker(0)
{
  // constructer
  declareProperty("InputData", m_clusterLocation = STClusterLocation::ITClusters);
  declareProperty("maxHits",m_maxHits = 4500  );
  declareProperty("xWindow1", m_xWindow1 = 1.0);
  declareProperty("xWindow2", m_xWindow2 = 1.0);
  declareProperty("yWindow", m_yWindow =  10.0);
  declareProperty("yTol", m_yTol = 2.0);
  declareProperty("maxTx", m_maxTx = 0.1);
  declareProperty("maxTy", m_maxTy = 0.1);
  declareProperty("zRef", m_zRef = 350000.);
  declareProperty("maxXRef", m_maxRefX = 6000.);
  declareProperty("minXRef", m_minRefX = 2000.);
  declareProperty("maxYRef", m_maxRefY = 1000000.);
  declareProperty("minXHits", m_minXHits = 5);
  declareProperty("minYHits", m_minYHits = 5); 
  declareProperty("allowedBoxes", m_allowedBoxes = boost::assign::list_of(1)(2)(3)(4) );
  declareProperty("firstStation", m_firstStation = boost::assign::list_of(1));
  declareProperty("lastStation", m_lastStation = boost::assign::list_of(3));
  declareProperty("minHits", m_minHits = 11);
  declareProperty("minXHitsToConfirm", m_minXHitsToConfirm = 0u);
  declareProperty("requireFirstAndLast", m_requireFirstAndLast = true);
  declareProperty("requireSameBox", m_requireSameBox = true );
  declareProperty("maxFaults", m_maxFaults = 100);
  declareProperty("maxClusterSize", m_maxClusterSize = 4);
  declareProperty("minCharge", m_minCharge = 10);
  declareProperty("confirmY2", m_confirmY2 = false);
  declareProperty("selectBestY",m_selectBestY = true);
}

ITGenericTracking::~ITGenericTracking() { }

StatusCode ITGenericTracking::initialize()
{
  if( "" == histoTopDir() ) setHistoTopDir("IT/");

  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // hit expectation
  m_hitExpectation = tool<IHitExpectation>("ITHitExpectation");
     
  // detector element     
  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::location("IT"));

  return StatusCode::SUCCESS;
}

StatusCode ITGenericTracking::execute()
{
   
  setFilterPassed(false);

  LHCb::Tracks* tracks = new LHCb::Tracks();
  put (tracks, LHCb::TrackLocation::Tsa); 

  // retrieve clusters
  const STClusters* clusterCont = get<STClusters>(m_clusterLocation);

  if (clusterCont->size() > m_maxHits) return StatusCode::SUCCESS;

  // make line hits
  std::vector<Tf::STHit*> hits; hits.reserve(clusterCont->size());
  for (STClusters::const_iterator iter = clusterCont->begin();
    iter != clusterCont->end(); ++iter){
    const DeSTSector* aSector = m_tracker->findSector((*iter)->channelID());
    if (aSector == 0) {
      std::cout << "Error: sector not found" << std::endl;
    }
    if ((*iter)->size() < m_maxClusterSize && (*iter)->totalCharge() > m_minCharge){
      Tf::STHit* newHit = new Tf::STHit(*aSector,(*iter)->liteCluster());
      hits.push_back(newHit);
    }
  }

  // get the x hits 
  std::vector<Tf::STHit*> xHits;  xHits.reserve(hits.size());
  LoKi::select(hits.begin(), hits.end(), std::back_inserter(xHits), bind(&Tf::STHit::isX,_1));  

  // get the stereo hits 
  std::vector<Tf::STHit*> stereoHits;  stereoHits.reserve(hits.size());
  LoKi::select(hits.begin(), hits.end(), std::back_inserter(stereoHits), !bind(&Tf::STHit::isX,_1));  
  if (xHits.size() < 3u || stereoHits.size() < 3u ) return StatusCode::SUCCESS; 


  // lets do some tracking
  for (std::vector<Tf::STHit*>::iterator iterX1 = xHits.begin(); iterX1 != xHits.end(); ++iterX1){

    if (m_requireFirstAndLast && (*iterX1)->cluster().channelID().layer() != 1) continue;

    if ( !allowedFirstStation(*iterX1) || allowedBox(*iterX1) == false) continue;

    std::vector<Tf::STHit*>::iterator iterX2 = iterX1; ++iterX2;
    for (; iterX2 != xHits.end(); ++iterX2){
      if (!allowedLastStation(*iterX2) || allowedBox(*iterX2) == false) continue;
      if (!sameBox(*iterX1,*iterX2)) continue;
      
    if (m_requireFirstAndLast && (*iterX2)->cluster().channelID().layer() != 4) continue;


      // make a line and constrain the parameters 
      Tf::Tsa::Line xLine = Tf::Tsa::Line((*iterX1)->x(), (*iterX2)->x(), (*iterX1)->z(), (*iterX2)->z());
      plot(xLine.m(),"tx", -0.5, 0.5, 100); 
      const double xRef = xLine.value(m_zRef);  
      plot(xRef, "xRef before", -20000, 20000. , 100);
      if (fabs(xLine.m()) > m_maxTx) continue;
      plot(xRef, "xRef", -20000, 20000. , 100);
      if (xRef > m_maxRefX || xRef < m_minRefX || fabs(xLine.m()) > m_maxTx) continue;

	  // collected consistant hits, first station 3 then the rest
      std::vector<Tf::STHit*> selectedX; selectedX.reserve(8);
      if (!collectXHits13(xLine, *iterX1, *iterX2, xHits,  selectedX)) continue;
      if ((*iterX1)->cluster().station() == 1 && (*iterX2)->cluster().station() == 3){
        if (!collectXHits2(xLine, *iterX1, xHits, selectedX)) continue;
      }
      // how many hits did we find ?
      plot(selectedX.size(), "# selected x", -0.5, 10.5, 11);
      if (selectedX.size() < m_minXHits) continue;

      plot(xRef, "xRef2", -20000, 20000. , 100);

      // now collect the stereo
      std::vector<yInfo> selectedY; 
      collectYHits(xLine,stereoHits, *iterX1 ,  selectedY);
      plot(selectedY.size(), "# selected y", -0.5, 50.5, 51);  
      if (selectedY.size() < m_minYHits) continue;

      // now select the best in y
      CandidateHits hits; CandidateLines lines;
      selectY(selectedY,hits ,lines);

      for (unsigned int iCan = 0 ; iCan < hits.size(); ++iCan){
  
	std::vector<Tf::STHit*> selectedY2 = hits[iCan];
	Tf::Tsa::Line yLine = lines[iCan];

	//        if (selectedY2.size() < m_minYHits) continue;

        plot(xRef, "xRef 3", -20000, 20000. , 100);
        plot(yLine.value(m_zRef), "yRef", -50000, 50000. , 100);
  
        // confirm x hits are consistant with y
        std::vector<Tf::STHit*> selectedX2; 
        selectX(selectedX, yLine, selectedX2 );
        plot(selectedX2.size(), "selected x2", -0.5, 10.5, 11);  
        if (selectedX2.size() < m_minXHits) continue;

        // make a track !!
        std::vector<LHCb::LHCbID> ids; ids.reserve(12);
        collectIDs(selectedX2, ids);
        collectIDs(selectedY2, ids);

        plot(ids.size(), "nhits", -0.5, 20.5, 21);
        if (ids.size() < m_minHits) continue;

        // count the number above the high threshold
        unsigned int nHigh = countHigh(selectedX2, selectedY2, clusterCont);
        plot(nHigh/double(ids.size()), "frac high", 0., 1., 100);
        if (nHigh/double(ids.size()) < 0.6) continue;

	// make a track ! 
        const double zMidT = StateParameters::ZMidT;
        Gaudi::TrackVector stateVec = Gaudi::TrackVector(xLine.value(zMidT), yLine.value(zMidT),
                                                         xLine.m(), yLine.m(), 1/20000.);
        Gaudi::TrackSymMatrix stateCov = Gaudi::TrackSymMatrix();
        LHCb::State aState = LHCb::State(stateVec,stateCov,zMidT,LHCb::State::AtT);
  

        LHCb::Track* aTrack = new LHCb::Track(LHCb::Track::TsaTrack, LHCb::Track::Ttrack, 
                                              LHCb::Track::PatRecIDs, ids, aState);

        const unsigned int itExpected = m_hitExpectation->nExpected(*aTrack);
        const int faults = itExpected - ids.size();
   
        plot(faults , "nFaults", -10.5, 10.5, 21);
        if (faults < m_maxFaults ) {
          aTrack->addInfo(LHCb::Track::nExpectedIT, itExpected);
          tracks->insert(aTrack);
          setFilterPassed(true);
        }
        else {
          delete aTrack;
        }
      } // loop over candidates 
    } // iterX2
  } // iterX1


  // delete them again
  for (std::vector<Tf::STHit*>::const_iterator iterHit = hits.begin(); iterHit != hits.end(); ++iterHit ){
     delete *iterHit;
  }

  return StatusCode::SUCCESS;
}

void ITGenericTracking::collectIDs( std::vector<Tf::STHit*> hits , std::vector<LHCb::LHCbID>& ids)  const {
   // convert LHCbIDs to hits
   for (std::vector<Tf::STHit*>::const_iterator iterHit = hits.begin(); iterHit != hits.end(); ++iterHit ){
     ids.push_back((*iterHit)->lhcbID());
     (*iterHit)->setUsed(true);
   }
}

void ITGenericTracking::selectX(const std::vector<Tf::STHit*>& hits, 
                            const Tf::Tsa::Line& yLine, 
                            std::vector<Tf::STHit*>& selected2 ){
			
   // once we we know y we can check that x hits overlap with that y											
   for (std::vector<Tf::STHit*>::const_iterator iterHit = hits.begin(); iterHit != hits.end(); ++iterHit ){
     const double yExpected = yLine.value((*iterHit)->z());
     if ((*iterHit)->isYCompatible(yExpected, m_yTol)) selected2.push_back(*iterHit); 
   }
}

void ITGenericTracking::selectY(const std::vector<yInfo>& hits, CandidateHits& canhits, 
                                 CandidateLines& lines ) const{

  // select the y hits. Count hits in a window around each pairs of hits
  // take pair that has highest number of hits in the window as being the correct line
  unsigned int nCand = 0u;
  for (std::vector<yInfo>::const_iterator iter1 = hits.begin(); iter1 != hits.end(); ++iter1){
    if (!allowedFirstStation(iter1->first)) continue;
    std::vector<yInfo>::const_iterator iter2 = iter1; ++iter2;
    for (; iter2 != hits.end(); ++iter2){
      if (!allowedLastStation(iter2->first)) continue; 
      const Gaudi::XYZPoint point1 = iter1->second;
      const Gaudi::XYZPoint point2= iter2->second;
      if (fabs(point1.z() - point2.z()) < 1.0 ) continue; // not at same z
      Tf::Tsa::Line yline = Tf::Tsa::Line(point1.y(), point2.y(), point1.z(), point2.z()); 
      if (fabs(yline.m()) > m_maxTy || fabs(yline.value(m_zRef)) > m_maxRefY ) continue;

      std::vector<unsigned int> nLayers; nLayers.reserve(12); 
      bool hasT2 = false;
      nLayers.push_back(iter1->first->cluster().channelID().uniqueSector());
      nLayers.push_back(iter2->first->cluster().channelID().uniqueSector());
      for (std::vector<yInfo>::const_iterator iter3 = hits.begin(); iter3 != hits.end(); ++iter3){

	// should not be in the same module 
        const Gaudi::XYZPoint point3 = iter3->second;
	if (fabs(point1.z() - point3.z()) < 1.0 ) continue; // not at same z
        if (fabs(point2.z() - point3.z()) < 1.0 ) continue; // not at same z
        if (iter3->first->cluster().station() == ITNames::IT2) hasT2 = true;

	const double yDiff = yline.value(point3.z()) - point3.y();
        if (fabs(yDiff) < m_yWindow) {
          nLayers.push_back(iter3->first->cluster().channelID().uniqueSector());
          plot(yDiff, 161, "yLine", -20., 20., 200);
	}		    			    
      } //iter3

      std::sort(nLayers.begin(), nLayers.end());
      nLayers.erase(std::unique(nLayers.begin(), nLayers.end()), nLayers.end());
      unsigned int nWindow = nLayers.size();
      plot(nWindow,"nWindow y", -0.5, 50.5,51);
   
      bool confirmed;
      m_confirmY2 == false ? confirmed = true : confirmed = hasT2; 
      if (nWindow > m_minYHits && confirmed ){
        ++nCand;
	std::vector<Tf::STHit*> selected;
        // loop again and collect hits
        for (std::vector<yInfo>::const_iterator iter4 = hits.begin(); iter4 != hits.end(); ++iter4){
          const Gaudi::XYZPoint point = iter4->second;
          double yDiff = yline.value(point.z()) - point.y();
          plot(yDiff,"yDiff", -100., 100., 100);
          if (fabs(yDiff) < m_yWindow) selected.push_back(iter4->first); 
        }  // iter4        

        if (m_selectBestY == false ){
          canhits.push_back(selected); lines.push_back(yline);
	} 
        else {
          if (canhits.empty() == true) {
            canhits.push_back(selected); lines.push_back(yline);
	  }
          else if (selected.size() > canhits.front().size()){
            canhits.pop_back();
            canhits.push_back(selected); lines.push_back(yline);
	  }
	} // select best 
      } // if good candidate
    } // iter2
  } // iter1 
  
  plot(nCand,"nCand y", -0.5, 50.5, 51);

} 


void ITGenericTracking::collectYHits(const Tf::Tsa::Line& xLine, 
				      const std::vector<Tf::STHit*>& yhits, const Tf::STHit* hit1,
                                 std::vector<ITGenericTracking::yInfo>& selected) const{
  
  using namespace Tf::Tsa;
  
  // make a plane... 
  Gaudi::XYZVector vec(1., TsaConstants::tilt * xLine.m(), -xLine.m());  
  Gaudi::XYZPoint point1 = Gaudi::XYZPoint(hit1->xMid(),hit1->yMid(),hit1->zMid()) ;
  // Gaudi::XYZPoint point2 = Gaudi::XYZPoint(hit2->xMid(),hit2->yMid(),hit2->zMid()) ;
  //Gaudi::XYZPoint point3 = Gaudi::XYZPoint(hit1->xMax(),hit1->yMax(),hit1->z(hit1->yMax())) ;


  Gaudi::Plane3D plane = Gaudi::Plane3D(vec,point1);
  //Gaudi::Plane3D plane = Gaudi::Plane3D(point1, point2, point3);
  Gaudi::XYZPoint iPoint;

  unsigned int box = hit1->cluster().detRegion(); 
 
  for (std::vector<Tf::STHit*>::const_iterator iter = yhits.begin(); iter != yhits.end(); ++iter){
    const double xTest = xLine.value((*iter)->z());
    if (xTest > ((*iter)->xMin() - m_xWindow2)  && xTest < ((*iter)->xMax() + m_xWindow2 )) {
      
      double mu = 0;
      Tf::LineHit::Point point = (*iter)->beginPoint();
      Tf::LineHit::Vector direction = (*iter)->direction();
      Gaudi::Math::Line<Tf::LineHit::Point, Tf::LineHit::Vector> stripline = Gaudi::Math::Line<Tf::LineHit::Point, Tf::LineHit::Vector>(point,direction);

      unsigned int testbox = (*iter)->cluster().detRegion();  
      if (box != testbox) continue;

      if (!Gaudi::Math::intersection(stripline,plane,iPoint, mu) ) continue;
      if ((*iter)->isYCompatible(iPoint.y(), m_yTol) ){
        selected.push_back(std::make_pair(*iter, iPoint));
      }
    }
  } // iter
}

bool ITGenericTracking::collectXHits13(const Tf::Tsa::Line& line,	
                                       Tf::STHit* hit1, Tf::STHit* hit2, 
                                       const std::vector<Tf::STHit*>& xhits,  
                                       std::vector<Tf::STHit*>& selected) const{

  bool confirmed = false;
  unsigned int stations[2] = {hit1->cluster().station(), hit2->cluster().station()}; 


  // collect hits in station 1 and 3
  for (std::vector<Tf::STHit*>::const_iterator iterX1 = xhits.begin(); iterX1 != xhits.end(); ++iterX1){
   
    // hits in station 1 or 3, not the hits we started with
    if (*iterX1 == hit1  || *iterX1 == hit2 ) continue;
 
    const unsigned int testStation = (*iterX1)->cluster().station();
    bool okStation = testStation == stations[1] || testStation == stations[0]; 
    if (!okStation) continue;
    if (sameBox(*iterX1, hit1) == false ) continue;
     
    // not at the same z
    if (fabs((*iterX1)->z() - hit1->z()) < 1.0  || 
        fabs((*iterX1)->z() - hit2->z() ) < 1.0)  continue;

    const double xDiff = (*iterX1)->x() - line.value((*iterX1)->z());
    plot(xDiff, "xDiff_13_"+ITNames().BoxToString((*iterX1)->cluster().channelID()), -10., 10., 400);
    if (hit1->cluster().layer() == 1 && hit2->cluster().layer() == 4){
      plot(xDiff, "xDiff_13_" + ITNames().UniqueLayerToString((*iterX1)->cluster().channelID()), -10.,10., 400);
    }
    if (fabs(xDiff) < m_xWindow1) selected.push_back(*iterX1);

  } // hits

  if (selected.size() > m_minXHitsToConfirm){
    confirmed = true;
    selected.push_back(hit1); selected.push_back(hit2);
  }

  return confirmed;
}

bool ITGenericTracking::collectXHits2(const Tf::Tsa::Line& line,
                                      Tf::STHit* hit, 
				      const std::vector<Tf::STHit*>& xhits,  std::vector<Tf::STHit*>& selected) const{

  unsigned int collected = 0u;
  for (std::vector<Tf::STHit*>::const_iterator iterX1 = xhits.begin(); iterX1 != xhits.end(); ++iterX1){

    if ((*iterX1)->cluster().station() == 2 && sameBox(hit, *iterX1)){
      const double xDiff = (*iterX1)->x() - line.value((*iterX1)->z());
      plot(xDiff,"xDiff_2_"+ITNames().UniqueLayerToString((*iterX1)->cluster().channelID()), -10., 10., 400);
      plot(xDiff,"xDiff_2_"+ITNames().UniqueBoxToString((*iterX1)->cluster().channelID()), -10., 10., 400);
      ///plot(xDiff,ITNames().BoxToString((*iterX1)->cluster().channelID())+"xDiff_2_"+ITNames().UniqueSectorToString((*iterX1)->cluster().channelID()), -10., 10., 400);
  
      if (fabs(xDiff) < m_xWindow2){
        ++collected;
        selected.push_back(*iterX1);
      } 
    } 
  }  // iterX1

  return collected > m_minXHitsToConfirm ;
}

unsigned int ITGenericTracking::countHigh( const std::vector<Tf::STHit*>& xhits, const std::vector<Tf::STHit*>& yhits, 
                                           const STClusters* clusterCont ) const{

  unsigned int nhits = 0; double totalCharge = 0;
  for (std::vector<Tf::STHit*>::const_iterator iterX = xhits.begin(); iterX != xhits.end(); ++iterX){
    if ((*iterX)->cluster().highThreshold() == true) ++nhits;
    const double charge = clusterCont->object((*iterX)->cluster().channelID())->totalCharge();
    plot(charge,"chargeHigh", 0., 100., 100);
    plot((*iterX)->cluster().strip(),"strip", -0.5, 400.5, 401);
    plot((*iterX)->cluster().strip()%32,"strip % 32", -0.5, 400.5, 401);
    plot((*iterX)->cluster().strip(),"strip % 128", -0.5, 400.5, 401);
    totalCharge += charge;
  }
  for (std::vector<Tf::STHit*>::const_iterator iterY = yhits.begin(); iterY != yhits.end(); ++iterY){
    if ((*iterY)->cluster().highThreshold() == true) ++nhits;
    const double charge = clusterCont->object((*iterY)->cluster().channelID())->totalCharge();
    plot(charge,"chargeHigh", 0., 100., 100);
    plot((*iterY)->cluster().strip(),"strip", -0.5, 400.5, 401);
    totalCharge += charge;
  }

  const unsigned int totalHits = yhits.size() + xhits.size();
  plot(totalCharge/totalHits,"total charge", 0., 100., 100);

  return nhits;
}


