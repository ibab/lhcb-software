// $Id: TsaSeedAddHits.cpp,v 1.1 2006-12-06 14:35:01 mneedham Exp $

// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"


#include "TsaSeedAddHits.h"
#include "TsaKernel/SeedFunctor.h"
#include "TsaKernel/SeedTrack.h"
#include "SeedLineFit.h"
#include "SeedParabolaFit.h"

#include "TsaKernel/TsaConstants.h"
#include "TsaKernel/Types.h"
#include "TsaKernel/ClusFun.h"
#include "TsaKernel/IITDataSvc.h"


#include "Kernel/STDataFunctor.h"
#include "Kernel/STChannelID.h"

#include <algorithm>
#include <utility>

#include "STDet/DeSTDetector.h"
#include "STDet/DeITStation.h"
#include "STDet/DeITBox.h"

// gsl
#include "gsl/gsl_cdf.h"

DECLARE_TOOL_FACTORY( TsaSeedAddHits );

TsaSeedAddHits::TsaSeedAddHits(const std::string& type,
                               const std::string& name,
                                const IInterface* parent):
GaudiTool(type, name, parent),
 m_parabolaFit(0),
 m_fitLine(0)
{ 

  m_parabolaFit = new SeedParabolaFit(TsaConstants::z0);
  m_fitLine = new SeedLineFit(TsaConstants::z0, TsaConstants::sth);

  declareProperty("itDataSvcType", m_dataSvcType = "ITDataSvc");
  declareProperty("itDataSvcName", m_dataSvcName = "ITDataSvc");
  declareProperty("tol", m_tol =  1.0*Gaudi::Units::mm);
  declareProperty("dCut", m_dCut = 0.18*Gaudi::Units::mm);

  declareInterface<ITsaSeedAddHits>(this);


};

TsaSeedAddHits::~TsaSeedAddHits(){
  // destructer
  delete m_parabolaFit;
  delete m_fitLine;
}


StatusCode TsaSeedAddHits::initialize(){

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize",sc);
  }

  // data service tool !
  m_dataSvc = tool<IITDataSvc>(m_dataSvcName,m_dataSvcType);

  m_tracker = getDet<DeSTDetector>(DeSTDetLocation::IT);
  const DeSTDetector::Stations& stations = m_tracker->stations();
  m_nStations = stations.size();
  for (DeSTDetector::Stations::const_iterator iter = stations.begin(); iter != stations.end() ; ++iter ){
    // get the z of the boxes
    const DeITStation* iStation = dynamic_cast<const DeITStation*>(*iter);
    
    // get the children ---> boxes 
    const DeITStation::Children& boxes = iStation->boxes();
    DeITStation::Children::const_iterator iterBox = boxes.begin();
    for (; iterBox != boxes.end(); ++iterBox){
      Gaudi::XYZPoint aPoint = (*iterBox)->globalCentre();
      m_zBox.push_back(aPoint.z());
      // get the first layer in the box
    }
  } // iter
 
  m_nBox = m_zBox.size()/m_nStations;
 
  unsigned int pos = 0;
  m_xMin.resize(m_zBox.size());
  m_xMax.resize(m_zBox.size());
  m_yMin.resize(m_zBox.size());
  m_yMax.resize(m_zBox.size());
  for (unsigned int iStation = 1; iStation <= m_nStations ;++iStation) {
    for (unsigned int iBox = 1; iBox <= m_nBox; ++iBox ){
      DeSTSector* firstSector = m_tracker->findSector(LHCb::STChannelID(LHCb::STChannelID::typeIT, iStation,1,iBox,1,0));
      DeSTSector* lastSector = m_tracker->findSector(LHCb::STChannelID(LHCb::STChannelID::typeIT, iStation,1,iBox,7,0));
   
      // get trajectories
      std::auto_ptr<LHCb::Trajectory> firstTraj = firstSector->trajectoryFirstStrip();
      std::auto_ptr<LHCb::Trajectory> lastTraj = lastSector->trajectoryLastStrip();

      // fill vectors of min, max x
      Gaudi::XYZPoint p1 = firstTraj->beginPoint();
      m_xMin[pos] = p1.x() - m_tol;
      m_yMin[pos] = p1.y() - m_tol;

      Gaudi::XYZPoint p2 = lastTraj->endPoint();
      m_xMax[pos] = p2.x() + m_tol;
      m_yMax[pos] = p2.y() + m_tol;
      ++pos;

    } // box
  } //station
  
  return StatusCode::SUCCESS;

}

StatusCode TsaSeedAddHits::execute(SeedTracks* seeds, SeedHits* hits ){

//-------------------------------------------------------------------------
//  Select tracks in 3-D
//-------------------------------------------------------------------------
//  double dCut = 0.18;                             //  Cut on distance of hit from seed trajectory
//  double tth = 0.08749;                           //  tan(stereo angle)

  Gaudi::XYZPoint iPoint;

  //  Loop over seed candidates
  for ( SeedTracks::iterator it = seeds->begin(); seeds->end() != it; ++it ) {
    SeedTrack* seed = *it;

    // only for selected candidates
    if (seed->select() == false) continue;

    unsigned int nHitAdded = 0;

    for ( int station = 1; station < 4; ++station ) {   //  Loop over stations
      for ( int box = 1; box < 5; ++box ) {             //  Loop over IT boxes

	// ignore boxes the track already has to save time...
        if ((seed->sector() == 0 && box < 3)  || 
            (seed->sector() == 1 && box  == 3 ) ||
            (seed->sector() == 2 && box == 4)) continue;

        int pos = (station - 1)*m_nBox + box -1; 
 
        //  Check that seed candidate passes through (or at least close) to box 
        const double yb = seed->y(m_zBox[pos],TsaConstants::z0);  
        if ( yb < m_yMin[pos] || yb > m_yMax[pos] ) continue;
        const double xb = seed->x(m_zBox[pos],TsaConstants::z0);
        if ( xb < m_xMin[pos] || xb > m_xMax[pos] ) continue;
        
        int hint = -1;
        for ( int layer = 0; layer < 4; ++layer ) {     //  Loop over layers in box

          //  Loop over clusters in that layer
	  //          Tsa::STRange iRange =  m_dataSvc->partition(station,layer+1,box);
	  Tsa::STRange iRange = range(hint, station, layer+1, box);
          for (Tsa::STClusters::const_iterator itIter = iRange.begin(); itIter != iRange.end(); ++itIter){
            Tsa::STCluster* clus = *itIter;
            if ( clus->isHot() || clus->onTrack()) continue;  //  Ignore clusters from hot regions
            
            //  Find seed candidate's coordinate at the cluster (midpoint)
	    const double z = clus->zMid();
            double xs = seed->x(z,TsaConstants::z0);
            double x = clus->xMid();
          
            if ( layer == 1 ) { 
              xs -= TsaConstants::tth *seed->y(z,TsaConstants::z0);  //  Correct for stereo
              x  -= TsaConstants::tth * clus->yMid();
	    }
            else if ( layer == 2 ){
              xs += TsaConstants::tth * seed->y(z, TsaConstants::z0 );
              x += TsaConstants::tth * clus->yMid();
	    }
            
            //  Cut on distance of cluster from seed candidate
            if ( fabs( x - xs ) < m_dCut ) {
                ++nHitAdded;
                hint = clus->channelID().sector();
                SeedHit* aHit = new SeedHit(clus);
                hits->insert(aHit);
                SeedPnt pnt(aHit);
                if (layer == 1 || layer == 2){
                 const double slope = seed->xSlope(aHit->z(),TsaConstants::z0);
		 Gaudi::XYZVector vec(1., TsaConstants::tilt*slope, -slope);
                 Gaudi::XYZPoint point(seed->x(aHit->z(),TsaConstants::z0),aHit->clus()->yMid(), aHit->z() );
                 Gaudi::Plane3D plane = Gaudi::Plane3D(vec,point);
                 if (Tsa::ClusFun::intersection(aHit->clus(),plane,iPoint) == true) {
                   aHit->setY( iPoint.y() );
                   aHit->setZ( iPoint.z() );
                   (*it)->addToYPnts(pnt);
		 } 
		}
                else {
                  (*it)->addToXPnts(pnt); 
		}  
		// } // not already on track
            } // if in box
          } //iterClusters
        } // layer
      } // box
    } // station

    if ( nHitAdded != 0){ 
     // we found some additional hits ---> refit
     double sx = seed->xSlope(8600.,0.0);
     double csth = sqrt(1.0 + sx*sx);
     m_parabolaFit->refit(seed,csth);
     m_fitLine->fit(seed);
    }

  } // tracks

  return StatusCode::SUCCESS;
}

Tsa::STRange TsaSeedAddHits::range(const int hint, const int station, 
                                   const int layer, const int box) const{

  // get the data for this partition 
  Tsa::STRange iRange =  m_dataSvc->partition(station,layer,box);

  // if its empty or we we have no hint return it
  if (iRange.empty() || hint == -1) return iRange;

  // if we have a hint use it
  Tsa::STRange::iterator clusIter;
  Tsa::STRange::iterator cachedIter = iRange.begin();

  unsigned int firstMod =  GSL_MAX(hint-1,1);
  LHCb::STChannelID firstChan(LHCb::STChannelID::typeIT,station, layer, box, firstMod, 0);  
  clusIter = std::lower_bound(cachedIter,iRange.end(),firstChan,
                              STDataFunctor::compBySector_LB<const Tsa::STCluster*>());

  if (clusIter != iRange.end()){
    cachedIter = clusIter;    
    LHCb::STChannelID lastChan(LHCb::STChannelID::typeIT, firstChan.station(), 
                               firstChan.layer(), firstChan.detRegion(),
                               firstChan.sector() + 2 , 0u);
 
    clusIter = std::lower_bound(cachedIter, iRange.end(),lastChan,
                               STDataFunctor::compBySector_LB<const Tsa::STCluster*>());
    return Tsa::STRange(cachedIter,clusIter);
  }
		
  return Tsa::STRange(iRange.begin());			    		   
}
