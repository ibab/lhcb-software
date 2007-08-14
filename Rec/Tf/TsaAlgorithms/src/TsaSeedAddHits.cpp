// $Id: TsaSeedAddHits.cpp,v 1.2 2007-08-14 16:56:15 jonrob Exp $

// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "TsaSeedAddHits.h"

// gsl
#include "gsl/gsl_cdf.h"

using namespace Tf::Tsa;

DECLARE_TOOL_FACTORY( SeedAddHits );

SeedAddHits::SeedAddHits(const std::string& type,
                         const std::string& name,
                         const IInterface* parent):
  GaudiTool(type, name, parent),
  m_parabolaFit(0),
  m_fitLine(0)
{

  declareProperty("itDataSvcType", m_dataSvcType = "Tf::ITStationHitManager<Tf::Tsa::SeedingHit>");
  declareProperty("itDataSvcName", m_dataSvcName = "TsaITDataManager");
  declareProperty("tol", m_tol =  1.0*Gaudi::Units::mm);
  declareProperty("dCut", m_dCut = 0.18*Gaudi::Units::mm);

  declareInterface<ITsaSeedAddHits>(this);

}

SeedAddHits::~SeedAddHits(){
  // destructer
  delete m_parabolaFit;
  delete m_fitLine;
}


StatusCode SeedAddHits::initialize(){

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize",sc);
  }

  m_hitMan = tool<ITHitMan>(m_dataSvcType,m_dataSvcName);
  m_fitLine = new SeedLineFit(msg(), TsaConstants::z0, TsaConstants::sth);
  m_parabolaFit = new SeedParabolaFit(TsaConstants::z0);

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

StatusCode SeedAddHits::execute(SeedTracks* seeds, SeedHits* hits ){

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
          //Tsa::STRange iRange =  m_dataSvc->partition(station,layer+1,box);
          //Tsa::STRange iRange = range(hint, station, layer+1, box);
          const TfTsHitNumMap::TfRegions & tfRegions = m_hitNumMap.tfITRegions(box);
          for ( TfTsHitNumMap::TfRegions::const_iterator iTfR = tfRegions.begin();
                iTfR != tfRegions.end(); ++iTfR )
          {
            Range iRange = m_hitMan->hits(station-1,layer,*iTfR);
            for (Hits::const_iterator itIter = iRange.begin(); itIter != iRange.end(); ++itIter)
            {
              //Tsa::STCluster* clus = *itIter;
              // CRJ : Hit cleaning not yet available
              //if ( clus->isHot() || clus->onTrack()) continue;  //  Ignore clusters from hot regions

              //  Find seed candidate's coordinate at the cluster (midpoint)
              //const double z = clus->zMid();
              const double z = (*itIter)->hit()->zMid();
              double xs = seed->x(z,TsaConstants::z0);
              //double x = clus->xMid();
              double x = (*itIter)->hit()->xMid();

              if ( layer == 1 ) {
                xs -= TsaConstants::tth *seed->y(z,TsaConstants::z0);  //  Correct for stereo
                //x  -= TsaConstants::tth * clus->yMid();
                x  -= TsaConstants::tth * (*itIter)->hit()->yMid();
              }
              else if ( layer == 2 ){
                xs += TsaConstants::tth * seed->y(z, TsaConstants::z0 );
                //x += TsaConstants::tth * clus->yMid();
                x += TsaConstants::tth * (*itIter)->hit()->yMid();
              }

              //  Cut on distance of cluster from seed candidate
              if ( fabs( x - xs ) < m_dCut ) {
                ++nHitAdded;
                //hint = clus->channelID().sector();
                hint = (*itIter)->stChannelID().sector();
                SeedHit* aHit = new SeedHit(*itIter);
                hits->insert(aHit);
                verbose() << "SeedHit inserted " << *aHit << endreq;
                SeedPnt pnt(aHit);
                if (layer == 1 || layer == 2){
                  const double slope = seed->xSlope(aHit->z(),TsaConstants::z0);
                  Gaudi::XYZVector vec(1., TsaConstants::tilt*slope, -slope);
                  //Gaudi::XYZPoint point(seed->x(aHit->z(),TsaConstants::z0),aHit->clus()->yMid(), aHit->z() );
                  Gaudi::XYZPoint point(seed->x(aHit->z(),TsaConstants::z0),aHit->yMid(), aHit->z() );
                  Gaudi::Plane3D plane = Gaudi::Plane3D(vec,point);
                  //if (Tsa::ClusFun::intersection(aHit->clus(),plane,iPoint) == true) {
                  if (Tf::intersection(*itIter,plane,iPoint) )
                  {
                    verbose() << "  Intersection Point " << iPoint << endreq;
                    aHit->setY( iPoint.y() );
                    aHit->setZ( iPoint.z() );
                    (*it)->addToYPnts(pnt);
                  }
                }
                else {
                  (*it)->addToXPnts(pnt);
                }
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
