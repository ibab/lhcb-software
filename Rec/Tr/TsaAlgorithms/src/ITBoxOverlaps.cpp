// $Id: ITBoxOverlaps.cpp,v 1.2 2007-02-02 16:31:12 cattanem Exp $

#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"
#include "ITBoxOverlaps.h"
#include "GaudiKernel/Point3DTypes.h"
#include "Kernel/Trajectory.h"


ITBoxOverlaps::ITBoxOverlaps(DeSTDetector* tracker, double tolerance){

 // constructer - note this implementation depends strongly on detector numbering...
 m_firstIT = tracker->firstStation();

 // loop station
 for (unsigned int iStation = m_firstIT; iStation <= tracker->lastStation();++iStation) {  
  
    // get layer 1 ---> x L/R and 5 T/B 
    DeSTSector* lrSector = tracker->findSector(LHCb::STChannelID(LHCb::STChannelID::typeIT, iStation,1,2,1,0)); 
    DeSTSector* tbSector = tracker->findSector(LHCb::STChannelID(LHCb::STChannelID::typeIT, iStation,1,4,7,0));

    // get trajectories
    std::auto_ptr<LHCb::Trajectory> lrTraj = lrSector->trajectoryFirstStrip();
    std::auto_ptr<LHCb::Trajectory> tbTraj = tbSector->trajectoryLastStrip();

    Gaudi::XYZPoint firstPoint = lrTraj->position( lrTraj->length());
    Gaudi::XYZPoint secondPoint = tbTraj->position(0.);

    m_vMinCont.push_back(fabs(secondPoint.y()- tolerance));
    m_vMaxCont.push_back(fabs(firstPoint.y() + tolerance));      
  
    m_uMinCont.push_back(fabs(firstPoint.x() - tolerance));
    m_uMaxCont.push_back(fabs(secondPoint.x() + tolerance));

      
 }  // iStation
}
