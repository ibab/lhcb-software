// $Id: TsaStereoBase.cpp,v 1.1 2006-12-06 14:35:02 mneedham Exp $

// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "TsaStereoBase.h"
#include "TsaKernel/SeedHit.h"
#include "TsaKernel/SeedTrack.h"
#include "TsaKernel/SeedFun.h"
#include "SeedLineFit.h"

#include "Event/State.h"

#include "TsaKernel/ClusFun.h"
#include "TsaKernel/TsaConstants.h"

#include "Kernel/Point3DTypes.h"
#include "Kernel/Plane3DTypes.h"

TsaStereoBase::TsaStereoBase(const std::string& type,
                               const std::string& name,
                               const IInterface* parent):
  GaudiTool(type, name, parent){

  declareProperty("sector", m_sector = 0);
  m_fitLine = new SeedLineFit(TsaConstants::z0, TsaConstants::sth);
}

TsaStereoBase::~TsaStereoBase(){
  // destructer
  delete m_fitLine;
}

StatusCode TsaStereoBase::execute(LHCb::State& , std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6] ){
  return execute(seeds, hits);
}

StatusCode TsaStereoBase::execute(std::vector<SeedTrack*>& , std::vector<SeedHit*>* ){
  return StatusCode::SUCCESS;
}

void TsaStereoBase::cleanup(std::vector<SeedHit*> hits[6]){
 // clean up - copy hits to store
 SeedHits* hitsCont = getOrCreate<SeedHits,SeedHits>(evtSvc(),m_seedHitLocation);
 for ( int lay = 0; lay < 6; ++lay ) {
   for ( std::vector<SeedHit*>::iterator it = hits[lay].begin(); hits[lay].end() != it; ++it ) hitsCont->insert(*it);  
 } // lay
}

unsigned int TsaStereoBase::collectHits(SeedTrack* seed, std::vector<SeedHit*> hits[6],
                                        std::vector<SeedHit*> yHits[6] ){

  unsigned int nCollected = 0;

  for ( int lay = 0; lay < 6; ++lay ) {

    if (hits[lay].empty() == true) continue;

    bool uLay = false;
    if ( lay == 0 || lay == 2 || lay == 4 ) uLay = true;
 
    const double zFirst = hits[lay].front()->z();
    const double slope = seed->xSlope(zFirst,TsaConstants::z0);
    Gaudi::XYZVector vec(1., TsaConstants::tilt*slope, -slope);
    Gaudi::XYZPoint point(seed->x(zFirst,TsaConstants::z0), hits[lay].front()->clus()->yMid(), zFirst );
    Gaudi::Plane3D plane = Gaudi::Plane3D(vec,point);
    Gaudi::XYZPoint iPoint;
    const double tol =  0.1*hits[lay].front()->clus()->length();

    for ( std::vector<SeedHit*>::iterator it = SeedFun::startStereo(hits[lay],point.x(),tol); hits[lay].end() != it; ++it ) {
      SeedHit* hit = (*it);
      if ( !Tsa::ClusFun::intersection(hit->clus(),plane,iPoint) ) continue; 
 
      if ( uLay ) {
        if ( iPoint.y() > hit->clus()->yMax() + m_yTol ) continue;
        if ( iPoint.y() < hit->clus()->yMin() - m_yTol ) break;
      } else {
        if ( iPoint.y() > hit->clus()->yMax() + m_yTol )break;
        if ( iPoint.y() < hit->clus()->yMin() - m_yTol) continue;
      }
      hit->setY( iPoint.y() );
      hit->setZ( iPoint.z() );
      yHits[lay].push_back( hit );
      ++nCollected;
      }
  } //hits

  return nCollected;
}
