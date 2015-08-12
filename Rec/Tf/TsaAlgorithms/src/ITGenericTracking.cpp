#ifdef _WIN32
#pragma warning ( disable : 4244 ) // conversion double to float in root
#endif


#include <algorithm>

// DigiEvent
#include "Event/STLiteCluster.h"

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
#include "Kernel/STDataFunctor.h"


#include "Event/Track.h"
#include "Event/State.h"
#include "Event/StateParameters.h"

#include <boost/foreach.hpp>
#include <boost/assign/list_of.hpp>

using namespace LHCb;

// Boost
#include <boost/assign/std/vector.hpp>
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
#include <functional>
using std::bind;
using namespace std::placeholders;
#else
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
using namespace boost::lambda;
#endif

#include "TrackInterfaces/IHitExpectation.h"

#include <gsl/gsl_fit.h>
#include "gsl/gsl_cdf.h"


DECLARE_ALGORITHM_FACTORY( ITGenericTracking )

//--------------------------------------------------------------------
//
//  ITGenericTracking : Monitoring class for the STClusters
//
//--------------------------------------------------------------------

ITGenericTracking::ITGenericTracking( const std::string& name,
                                    ISvcLocator* pSvcLocator ) :
  ST::HistoAlgBase(name, pSvcLocator)
{
  // constructer
  declareProperty("InputData", m_clusterLocation = STLiteClusterLocation::ITClusters);
  declareProperty("maxHits",m_maxHits = 1000  );
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
  {
    std::vector<unsigned int> tmp = boost::assign::list_of(1)(2)(3)(4);
    declareProperty("allowedBoxes", m_allowedBoxes = tmp);
  }
  {
    std::vector<unsigned int> tmp = boost::assign::list_of(1);
    declareProperty("firstStation", m_firstStation = tmp);
  }
  {
    std::vector<unsigned int> tmp = boost::assign::list_of(3);
    declareProperty("lastStation", m_lastStation = tmp);
  }
  declareProperty("minHits", m_minHits = 11);
  declareProperty("minXHitsToConfirm", m_minXHitsToConfirm = 0u);
  declareProperty("requireFirstAndLast", m_requireFirstAndLast = true);
  declareProperty("requireSameBox", m_requireSameBox = true );
  declareProperty("maxFaults", m_maxFaults = 100);
  declareProperty("confirm2", m_confirm2 = false);
  declareProperty("selectBestY",m_selectBestY = true);
  declareProperty("fracHigh", m_fracHigh = 0.6);
  declareProperty("assumedMomentum", m_assumedMomentum = 20000.);
  setDetType("IT");
  setForcedInit();
}

ITGenericTracking::~ITGenericTracking() { }

StatusCode ITGenericTracking::initialize()
{
  if( "" == histoTopDir() ) setHistoTopDir("IT/");

  StatusCode sc = ST::HistoAlgBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // hit expectation
  m_hitExpectation = tool<IHitExpectation>("ITHitExpectation");

  const DeSTBaseElement* station2 = tracker()->findTopLevelElement("IT2");
  m_T2Z = station2->globalCentre().z();

  return StatusCode::SUCCESS;
}

StatusCode ITGenericTracking::execute()
{

  setFilterPassed(false);

  LHCb::Tracks* tracks = new LHCb::Tracks(); tracks->reserve(5000);
  std::vector<Track*> tmpTracks; tmpTracks.reserve(50000);
  put (tracks, LHCb::TrackLocation::Tsa);

  // retrieve clusters
  const STLiteCluster::STLiteClusters* clusterCont = get<STLiteCluster::STLiteClusters>(m_clusterLocation);
  if (clusterCont->size() > m_maxHits) return StatusCode::SUCCESS;

  // make line hits
  std::vector<Tf::STHit*> hits; hits.reserve(clusterCont->size());
  for (STLiteCluster::STLiteClusters::const_iterator iter = clusterCont->begin();
    iter != clusterCont->end(); ++iter){
    const DeSTSector* aSector = findSector(iter->channelID());
    Tf::STHit* newHit = new Tf::STHit(*aSector,*iter);
    hits.push_back(newHit);
  }

  // get the x hits
  std::vector<Tf::STHit*> xHits;  xHits.reserve(hits.size());
  LoKi::select(hits.begin(), hits.end(), std::back_inserter(xHits), bind(&Tf::STHit::isX,_1));


  std::vector<Tf::STHit*> x2Hits; x2Hits.reserve(xHits.size());  // get the x hits in station 2
  std::vector<Tf::STHit*> x13Hits; x13Hits.reserve(xHits.size());   // get the x hits in station 13
  BOOST_FOREACH(Tf::STHit* aHit , xHits) {
    if (aHit->cluster().station() == ITNames::IT2) x2Hits.push_back(aHit);
    if (allowedFirstStation(aHit) || allowedLastStation(aHit)) x13Hits.push_back(aHit);
  }

  // sort
  std::stable_sort(x2Hits.begin(), x2Hits.end(),
      bind(std::less<double>(), bind(&Tf::STHit::xMid,_1), bind(&Tf::STHit::xMid,_2))); // sort by x


  // get the stereo hits
  std::vector<Tf::STHit*> stereoHits;  stereoHits.reserve(hits.size());
  LoKi::select(hits.begin(), hits.end(), std::back_inserter(stereoHits),
               bind(std::logical_not<bool>(), bind(&Tf::STHit::isX,_1)));
  if (xHits.size() < 3u || stereoHits.size() < 3u ) return StatusCode::SUCCESS;


  // lets do some tracking
  for (std::vector<Tf::STHit*>::iterator iterX1 = x13Hits.begin(); iterX1 != x13Hits.end(); ++iterX1){

    if (m_requireFirstAndLast && (*iterX1)->cluster().channelID().layer() != 1) continue;

    if ( !allowedFirstStation(*iterX1) || allowedBox(*iterX1) == false) continue;

    std::vector<Tf::STHit*>::iterator iterX2 = iterX1; ++iterX2;
    for (; iterX2 != x13Hits.end(); ++iterX2){
      if (!allowedLastStation(*iterX2) || allowedBox(*iterX2) == false) continue;
      if (!sameBox(*iterX1,*iterX2)) continue;
      if (m_requireFirstAndLast && (*iterX2)->cluster().channelID().layer() != 4) continue;

      // make a line and constrain the parameters
      //      if ((*iterX1)->cluster().station() == (*iterX2)->cluster().station()) continue;
      if (fabs((*iterX1)->zMid()- (*iterX2)->zMid()) < 1.0 ) continue;
      Tf::Tsa::Line xLine = Tf::Tsa::Line((*iterX1)->xMid(), (*iterX2)->xMid(), (*iterX1)->zMid(), (*iterX2)->zMid());

      const double xRef = xLine.value(m_zRef);
      if (fabs(xLine.m()) > m_maxTx) continue;
      if (xRef > m_maxRefX || xRef < m_minRefX || fabs(xLine.m()) > m_maxTx) continue;

	  // collected consistant hits, first station 3 then the rest
      std::vector<Tf::STHit*> selectedX; selectedX.reserve(16);
      if (!collectXHits13(xLine, *iterX1, *iterX2, x13Hits,  selectedX)) continue;
      if ((*iterX1)->cluster().station() == 1 && (*iterX2)->cluster().station() == 3){
	bool collect2 = collectXHits2(xLine, *iterX1, x2Hits, selectedX);
        if (m_confirm2 == true){
          if (collect2 == false) continue;
	}
      }
      // how many hits did we find ?
      if (fullDetail()) plot(selectedX.size(), "# selected x", -0.5, 10.5, 11);
      const unsigned int nUniqueX = countSectors(selectedX);
      if (nUniqueX < m_minXHits) continue;

      std::stable_sort(selectedX.begin(), selectedX.end(), STDataFunctor::Less_by_Channel<Tf::STHit*>());

      std::vector<std::vector<Tf::STHit*> > xcan; xcan.reserve(8);
      splitCandidates(selectedX,xcan);

      std::vector<std::vector<Tf::STHit*> >::const_iterator iterX = xcan.begin();
      for (; iterX != xcan.end(); ++iterX){

        LineFitResults results = fitX(*iterX);
        xLine = Tf::Tsa::Line(results.m,results.c);
        if (fullDetail()) plot(results.chisq/iterX->size(),"x chisq", 0., 25., 50 );

        // now collect the stereo
        std::vector<yInfo> selectedY;  selectedY.reserve(64);
        collectYHits(xLine,stereoHits, *iterX1 ,  selectedY);
        if (fullDetail() ) plot(selectedY.size(), "# selected y", -0.5, 50.5, 51);
        if (selectedY.size() < m_minYHits) continue;

        // now select the best in y
        CandidateHits hits; CandidateLines lines;
        selectY(selectedY,hits ,lines);

        for (unsigned int iCan = 0 ; iCan < hits.size(); ++iCan){

	  std::vector<yInfo> selectedY2 = hits[iCan];
	  Tf::Tsa::Line yLine = lines[iCan];

          if (fullDetail()){
            plot(xRef, "xRef 3", -20000, 20000. , 100);
            plot(yLine.value(m_zRef), "yRef", -50000, 50000. , 100);
	  }

          // confirm x hits are consistant with y
          std::vector<Tf::STHit*> selectedX2; selectedX2.reserve(selectedX.size());
          selectX(*iterX, yLine, selectedX2 );
          if (countSectors(selectedX2) < m_minXHits) continue;

        // make a track !!
          std::vector<LHCb::LHCbID> ids; ids.reserve(12);
          collectIDs(selectedX2, ids);
          collectIDs(selectedY2, ids);

          std::stable_sort(ids.begin(), ids.end(),
              bind(std::less<unsigned int>(), bind(&LHCb::LHCbID::lhcbID,_1), bind(&LHCb::LHCbID::lhcbID,_2)));

          if (fullDetail()) plot(ids.size(), "nhits", -0.5, 20.5, 21);
          const unsigned int nUnique = countSectors(selectedX2) + countSectors(selectedY2);
          if (nUnique < m_minHits) continue;

          // count the number above the high threshold
          const unsigned int nHigh = countHigh(selectedX2, selectedY2);
          if (fullDetail()) plot(nHigh/double(ids.size()), "frac high", 0., 1., 100);
          if (nHigh/double(ids.size()) < m_fracHigh) continue;

	  // make a track !
          const double zMidT = StateParameters::ZMidT;
          Gaudi::TrackVector stateVec = Gaudi::TrackVector(xLine.value(zMidT), yLine.value(zMidT),
                                                           xLine.m(), yLine.m(),
							   1/m_assumedMomentum);
          Gaudi::TrackSymMatrix stateCov = Gaudi::TrackSymMatrix();
          LHCb::State aState = LHCb::State(stateVec,stateCov,zMidT,LHCb::State::AtT);

          LHCb::Track* aTrack = new LHCb::Track(LHCb::Track::TsaTrack, LHCb::Track::Ttrack,
                                               LHCb::Track::PatRecIDs, ids, aState);

          const unsigned int itExpected = m_hitExpectation->nExpected(*aTrack);
          const int faults = itExpected - ids.size();

          if (fullDetail() ) plot(faults , "nFaults", -10.5, 10.5, 21);
          if (faults < m_maxFaults ) {
            //aTrack->addInfo(LHCb::Track::nExpectedIT, itExpected);
            tmpTracks.push_back(aTrack);
            setFilterPassed(true);
          }
          else {
            delete aTrack;
          }
       } // loop over candidates  in y
      } // loop over candidates in x
     } // iterX2
  } // iterX1


  // delete them again
  for (std::vector<Tf::STHit*>::const_iterator iterHit = hits.begin(); iterHit != hits.end(); ++iterHit ){
     delete *iterHit;
  }

  // make some sorting and cleaning...
  //  std::stable_sort(tracks->begin(),tracks->end(),bind(&Track::nLHCbIDs,_1) > bind(&Track::nLHCbIDs, _2));

  for (std::vector<LHCb::Track*>::const_iterator iterT = tmpTracks.begin();
       iterT != tmpTracks.end(); ++iterT){
    if ((*iterT)->checkFlag(Track::Clone ) == true) continue;
    const std::vector<LHCb::LHCbID>& ids1 = (*iterT)->lhcbIDs();
    std::vector<LHCb::Track*>::const_iterator iterT2 = iterT; ++iterT2;
    for (;  iterT2 != tmpTracks.end(); ++iterT2){
       if ((*iterT2)->checkFlag(Track::Clone ) == true) continue;
       const std::vector<LHCb::LHCbID>& ids2 = (*iterT2)->lhcbIDs();
       if ( ids1.size() == ids2.size() && std::equal(ids1.begin(), ids1.end(), ids2.begin()) == true){
	 (*iterT2)->setFlag( LHCb::Track::Clone, true );
       }
    } // iterT2
  } // iterT

  for (Tracks::const_iterator iterT2 = tmpTracks.begin();
       iterT2 != tmpTracks.end(); ++iterT2){
    if ((*iterT2)->checkFlag(Track::Clone ) == false){
      Track* newTrack = (*iterT2)->clone();
      tracks->insert(newTrack);
    }
  }

  // clear tmp list
  std::vector<Track*>::iterator iterVec = tmpTracks.begin();
  for (; iterVec != tmpTracks.end(); ++iterVec){
     delete *iterVec;
  } // iterVec

  if (tracks->size() > 0 && fullDetail()) plot(log10((double)tracks->size()), "nTracks", 0., 10., 200 );

  return StatusCode::SUCCESS;
}

void ITGenericTracking::collectIDs( std::vector<Tf::STHit*> hits , std::vector<LHCb::LHCbID>& ids)  const {
   // convert LHCbIDs to hits
   for (std::vector<Tf::STHit*>::const_iterator iterHit = hits.begin(); iterHit != hits.end(); ++iterHit ){
     ids.push_back((*iterHit)->lhcbID());
     (*iterHit)->setUsed(true);
   }
}

void ITGenericTracking::collectIDs( std::vector<yInfo> hits , std::vector<LHCb::LHCbID>& ids)  const {
   // convert LHCbIDs to hits
   for (std::vector<yInfo>::const_iterator iterHit = hits.begin(); iterHit != hits.end(); ++iterHit ){
     ids.push_back(iterHit->first->lhcbID());
     iterHit->first->setUsed(true);
   }
}

void ITGenericTracking::selectX(const std::vector<Tf::STHit*>& hits,
                            const Tf::Tsa::Line& yLine,
                            std::vector<Tf::STHit*>& selected2 ){

   // once we we know y we can check that x hits overlap with that y
   for (std::vector<Tf::STHit*>::const_iterator iterHit = hits.begin(); iterHit != hits.end(); ++iterHit ){
     const double yExpected = yLine.value((*iterHit)->zMid());
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
      nLayers.push_back(iter1->first->channelID().uniqueSector());
      nLayers.push_back(iter2->first->channelID().uniqueSector());
      for (std::vector<yInfo>::const_iterator iter3 = hits.begin(); iter3 != hits.end(); ++iter3){

	// should not be in the same module
        const Gaudi::XYZPoint point3 = iter3->second;
	if (fabs(point1.z() - point3.z()) < 1.0 ) continue; // not at same z
        if (fabs(point2.z() - point3.z()) < 1.0 ) continue; // not at same z
        if (iter3->first->channelID().station() == ITNames::IT2) hasT2 = true;

	const double yDiff = yline.value(point3.z()) - point3.y();
        if (fabs(yDiff) < m_yWindow) {
          nLayers.push_back(iter3->first->channelID().uniqueSector());
          if (fullDetail()) plot(yDiff, 161, "yLine", -20., 20., 200);
	}
      } //iter3

      std::stable_sort(nLayers.begin(), nLayers.end());
      nLayers.erase(std::unique(nLayers.begin(), nLayers.end()), nLayers.end());

      const unsigned int nWindow = nLayers.size();
      if (fullDetail()) plot(nWindow,"nWindow y", -0.5, 50.5,51);

      bool confirmed;
      m_confirm2 == false ? confirmed = true : confirmed = hasT2;
      if (nWindow > m_minYHits && confirmed ){
        ++nCand;
	std::vector<yInfo> selected;
        // loop again and collect hits
        for (std::vector<yInfo>::const_iterator iter4 = hits.begin(); iter4 != hits.end(); ++iter4){
          const Gaudi::XYZPoint point = iter4->second;
          const double yDiff = yline.value(point.z()) - point.y();
          if (fabs(yDiff) < m_yWindow) selected.push_back(*iter4);
        }  // iter4

        if (m_selectBestY == false ){
	  std::vector <std::vector<yInfo> > uniqueCan; uniqueCan.reserve(16);
          std::stable_sort(selected.begin(), selected.end(), Less_by_Channel());
          splitCandidates(selected, uniqueCan);
          BOOST_FOREACH( std::vector<yInfo> hits, uniqueCan ){
            if (newStereoCandidate( hits ,canhits ) == true){
	      LineFitResults results = fitY(hits);
	      Tf::Tsa::Line tempLine(results.m,  results.c);
              canhits.push_back(hits); lines.push_back(tempLine);
	      if (fullDetail()) plot(results.chisq/(hits.size() - 2u),"y chi",0., 20., 100);
	    }
	  }
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
}



void ITGenericTracking::collectYHits(const Tf::Tsa::Line& xLine,
				      const std::vector<Tf::STHit*>& yhits, const Tf::STHit* hit1,
                                 std::vector<ITGenericTracking::yInfo>& selected) const{

  using namespace Tf::Tsa;

  // make a plane...
  Gaudi::XYZVector vec(1., TsaConstants::tilt * xLine.m(), -xLine.m());
  Gaudi::XYZPoint point1 = Gaudi::XYZPoint(hit1->xMid(),hit1->yMid(),hit1->zMid()) ;

  Gaudi::Plane3D plane = Gaudi::Plane3D(vec,point1);
  //Gaudi::Plane3D plane = Gaudi::Plane3D(point1, point2, point3);
  Gaudi::XYZPoint iPoint;

  const unsigned int box = hit1->cluster().detRegion();

  for (std::vector<Tf::STHit*>::const_iterator iter = yhits.begin(); iter != yhits.end(); ++iter){

    const unsigned int testbox = (*iter)->cluster().detRegion();
    if (box != testbox) continue;

    const double xTest = xLine.value((*iter)->zMid());
    if (xTest > ((*iter)->xMin() - m_xWindow2)  && xTest < ((*iter)->xMax() + m_xWindow2 )) {

      double mu = 0;
      const Tf::LineHit::Point point = (*iter)->beginPoint();
      const Tf::LineHit::Vector direction = (*iter)->direction();
      Gaudi::Math::Line<Tf::LineHit::Point, Tf::LineHit::Vector> stripline = Gaudi::Math::Line<Tf::LineHit::Point, Tf::LineHit::Vector>(point,direction);

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

  Tf::STHit* hit = hit1;
  if (hit2->xMid() < hit1->xMid() ) hit = hit2;
  const double tol = std::max(10.0*m_xWindow1 , fabs(line.m())*20.0*Gaudi::Units::cm);
  const double testVal = line.value(hit->zMid()) - tol;

  std::vector<Tf::STHit*>::const_iterator iterX1 = std::lower_bound(xhits.begin(),xhits.end(), testVal , compByX_LB<Tf::STHit*>());
  iterX1 = xhits.begin();

  // collect hits in station 1 and 3
  for (; iterX1 != xhits.end(); ++iterX1){

    // hits in station 1 or 3, not the hits we started with
    if (*iterX1 == hit1  || *iterX1 == hit2 ) continue;

    const unsigned int testStation = (*iterX1)->cluster().station();
    bool okStation = testStation == stations[1] || testStation == stations[0];
    if (!okStation) continue;
    if (sameBox(*iterX1, hit1) == false ) continue;

    // not at the same z
    if (fabs((*iterX1)->zMid() - hit1->zMid()) < 1.0  ||
        fabs((*iterX1)->zMid() - hit2->zMid() ) < 1.0)  continue;

    const double xDiff = (*iterX1)->xMid() - line.value((*iterX1)->zMid());
    if (fullDetail() ){
      plot(xDiff, "xDiff_13_"+ITNames().BoxToString((*iterX1)->channelID()), -10., 10., 400);
      plot(xDiff, "xDiff_13_" + ITNames().UniqueLayerToString((*iterX1)->channelID()), -10.,10., 400);
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

  // where to start
  const double tol = std::max(10.0*m_xWindow2 , fabs(line.m())*20.0*Gaudi::Units::cm);
  double testVal = line.value(m_T2Z) - tol;
  std::vector<Tf::STHit*>::const_iterator iterX1 = std::lower_bound(xhits.begin(),xhits.end(), testVal, compByX_LB<Tf::STHit*>());

  // where to to stop
  testVal = line.value(m_T2Z) + tol;
  //  std::vector<Tf::STHit*>::const_iterator last = std::lower_bound(xhits.begin(),xhits.end(), testVal, compByX_LB<Tf::STHit*>());

  unsigned int collected = 0u;
  for (; iterX1 != xhits.end(); ++iterX1){

    if ((*iterX1)->channelID().station() == 2 && sameBox(hit, *iterX1)){
      const double xDiff = (*iterX1)->xMid() - line.value((*iterX1)->zMid());
      if (fullDetail() == true){
        plot(xDiff,"xDiff_2_"+ITNames().UniqueLayerToString((*iterX1)->channelID()), -10., 10., 400);
        plot(xDiff,"xDiff_2_"+ITNames().UniqueBoxToString((*iterX1)->channelID()), -10., 10., 400);
      }
      if (fabs(xDiff) < m_xWindow2){
        ++collected;
        selected.push_back(*iterX1);
      }

      // break once we are out the other side
      if (xDiff > tol) break;

    }
  }  // iterX1

  return collected > m_minXHitsToConfirm ;
}

unsigned int ITGenericTracking::countHigh( const std::vector<Tf::STHit*>& xhits, const std::vector<yInfo>& yhits) const{

  unsigned int nhits = 0;
  for (std::vector<Tf::STHit*>::const_iterator iterX = xhits.begin(); iterX != xhits.end(); ++iterX){
    if ((*iterX)->cluster().highThreshold() == true) ++nhits;
  }

  for (std::vector<yInfo>::const_iterator iterY = yhits.begin(); iterY != yhits.end(); ++iterY){
    if (iterY->first->highThreshold() == true) ++nhits;
  }

  return nhits;
}

unsigned int ITGenericTracking::countSectors(const std::vector<Tf::STHit*>& xhits) const{
  std::vector<unsigned int> nLayers; nLayers.reserve(12);
  BOOST_FOREACH(Tf::STHit* hit, xhits){
    nLayers.push_back(hit->channelID().uniqueSector());
  }
  std::stable_sort(nLayers.begin(), nLayers.end());
  nLayers.erase(std::unique(nLayers.begin(), nLayers.end()), nLayers.end());
  return nLayers.size();
}

unsigned int ITGenericTracking::countSectors(const std::vector<yInfo>& xhits) const{
  std::vector<unsigned int> nLayers; nLayers.reserve(12);
  BOOST_FOREACH(yInfo hit, xhits){
    nLayers.push_back(hit.first->channelID().uniqueSector());
  }
  std::stable_sort(nLayers.begin(), nLayers.end());
  nLayers.erase(std::unique(nLayers.begin(), nLayers.end()), nLayers.end());
  return nLayers.size();
}


void ITGenericTracking::splitCandidates(const std::vector<Tf::STHit*>& input,
                                        std::vector<std::vector<Tf::STHit*> >& output ) const{

  std::vector<Tf::STHit*>::const_iterator iter = input.begin();
  std::vector<Tf::STHit*> firstCand;
  output.push_back(firstCand);
  unsigned int firstSector = (*iter)->uniqueSector();
  //  ++iter;
  while (iter != input.end()){

    // collect candidate hits in this layer
    std::vector<Tf::STHit*> currentSector;
    while (iter != input.end() && (*iter)->uniqueSector() == firstSector){
      currentSector.push_back(*iter);
      ++iter;
    }

    if (currentSector.empty() == false){

      // make a temporary list of candidates
      std::vector<std::vector<Tf::STHit*> > tempCont;
      BOOST_FOREACH(std::vector<Tf::STHit*> hits, output ){
	BOOST_FOREACH(Tf::STHit* newHit, currentSector){
	  std::vector<Tf::STHit*> newCan;
	  std::copy(hits.begin(), hits.end(), std::back_inserter(newCan));
          newCan.push_back(newHit);
          tempCont.push_back(newCan);
	} // currentSector
      }  // tempCont

      // now copy to the output container
      output.clear();
      std::copy(tempCont.begin(),tempCont.end(),std::back_inserter(output));
	// }
    } // if

    if (iter != input.end()) firstSector = (*iter)->uniqueSector();

  } // iter
}

void ITGenericTracking::splitCandidates(const std::vector<yInfo>& input,
                                        std::vector<std::vector<yInfo> >& output ) const{

  std::vector<yInfo>::const_iterator iter = input.begin();
  std::vector<yInfo> firstCand;
  output.push_back(firstCand);
  unsigned int firstSector = iter->first->uniqueSector();
  //  ++iter;
  while (iter != input.end()){

    // collect candidate hits in this layer
    std::vector<yInfo> currentSector;
    while (iter != input.end() && iter->first->uniqueSector() == firstSector){
      currentSector.push_back(*iter);
      ++iter;
    }

    if (currentSector.empty() == false){

      // make a temporary list of candidates
      std::vector<std::vector<yInfo> > tempCont;
      BOOST_FOREACH(std::vector<yInfo> hits, output ){
	BOOST_FOREACH(yInfo newHit, currentSector){
	  std::vector<yInfo> newCan;
	  std::copy(hits.begin(), hits.end(), std::back_inserter(newCan));
          newCan.push_back(newHit);
          tempCont.push_back(newCan);
	} // currentSector
      }  // tempCont

      // now copy to the output container
      output.clear();
      std::copy(tempCont.begin(),tempCont.end(),std::back_inserter(output));
	// }
    } // if

    if (iter != input.end()) firstSector = iter->first->uniqueSector();

  } // iter
}


ITGenericTracking::LineFitResults ITGenericTracking::fitX(const std::vector<Tf::STHit*> hits) const{

  std::vector<double> x; x.reserve(hits.size());
  std::vector<double> z; x.reserve(hits.size());
  std::vector<double> w; w.reserve(hits.size());
  BOOST_FOREACH(Tf::STHit* aHit, hits){
    x.push_back(aHit->xMid()); z.push_back(aHit->zMid()); w.push_back(44);
  }

  return lineFit(x,z,w);
}


ITGenericTracking::LineFitResults ITGenericTracking::fitY(const std::vector<ITGenericTracking::yInfo>& hits) const{

  std::vector<double> y; y.reserve(hits.size());
  std::vector<double> z; z.reserve(hits.size());
  std::vector<double> w; w.reserve(hits.size());
  BOOST_FOREACH(yInfo aHit, hits){
    y.push_back(aHit.second.y()); z.push_back(aHit.second.z()); w.push_back(0.16);
  }

  return lineFit(y,z,w);
}

ITGenericTracking::LineFitResults ITGenericTracking::lineFit(const std::vector<double>& x,
                                                             const std::vector<double>& z,
                                                             const std::vector<double>& w) const{
  LineFitResults result;
  gsl_fit_wlinear(&z[0],1,&w[0],1,&x[0],1,x.size(),&result.c,&result.m,&result.cov00, &result.cov01, &result.cov11,&result.chisq);
  return result;
}

bool ITGenericTracking::newStereoCandidate(const std::vector<ITGenericTracking::yInfo>& testCand,
                                                const CandidateHits& tracks) const{

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
  // return true if nothing was found
  return tracks.end() ==
    std::find_if(tracks.begin(), tracks.end(),    // loop over all the tracks
      [&](const CandidateHits::value_type &can){  // looking for the candidate
        return testCand.size() == can.size() &&   // that has all the same Ys as testCand
            equal(can.begin(), can.end(), testCand.begin(),
              [] (const yInfo &a, const yInfo &b) {
                 return a.second == b.second;
              });
      }); // when a match is found, 'find_if' returns the iterator != 'end()'
#else
  bool newCan = true;
  BOOST_FOREACH(CandidateHits::value_type can, tracks ) {
    if (testCand.size() == can.size() && equal(can.begin(), can.end(), testCand.begin(),
            ret<const yInfo::second_type&>(bind(&yInfo::second,_1)) == ret<const yInfo::second_type&>(bind(&yInfo::second,_2))) == true) {
      newCan = false;
      break;
    }
  } // for each
  return newCan;
#endif
}

