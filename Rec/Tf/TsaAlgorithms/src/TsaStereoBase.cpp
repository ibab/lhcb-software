// $Id: TsaStereoBase.cpp,v 1.8 2010-04-21 09:35:41 mneedham Exp $

#ifdef _WIN32
#pragma warning ( disable : 4244 ) // conversion double to float in root
#endif

// GaudiKernel
#include "GaudiKernel/SystemOfUnits.h"

#include "TsaStereoBase.h"
#include "TsaKernel/SeedHit.h"
#include "TsaKernel/SeedTrack.h"
#include "TsaKernel/SeedFun.h"
#include "SeedLineFit.h"

#include "Event/State.h"

#include "TsaKernel/TsaConstants.h"

#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"

#include "TfKernel/RecoFuncs.h"

using namespace Tf::Tsa;

StereoBase::StereoBase(const std::string& type,
                       const std::string& name,
                       const IInterface* parent):
  GaudiTool(type, name, parent),
  m_fitLine(NULL)
{
  declareProperty("sector", m_sector = -1);
  declareProperty("outlierCut", m_outlierCut = 3.5);
  declareProperty("nSigmaTy", m_nSigmaTy = 10. );
}


StereoBase::~StereoBase()
{
  // destructor
}

StatusCode StereoBase::finalize() 
{
  delete m_fitLine; m_fitLine = 0;
  return GaudiTool::finalize();
}

StatusCode StereoBase::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
    return Error("Failed to initialize",sc);
  }
  m_syMin = -0.3;
  m_syMax = 0.3;

  m_fitLine = new SeedLineFit(msg(),TsaConstants::z0, TsaConstants::sth, m_outlierCut);

  // sector must be set
  if (m_sector == -1){
    return Error("No sector set", StatusCode::FAILURE);
  }
  return StatusCode::SUCCESS;
}

StatusCode StereoBase::execute(LHCb::State& aState, std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6] ){

  // pick up the errors
  const double errTy = sqrt(aState.errTy2());
  m_syMin = aState.ty() - m_nSigmaTy * errTy;
  m_syMax = aState.ty() + m_nSigmaTy * errTy;
  
  // call the method
  StatusCode sc = execute(seeds, hits);
  
  // where we started..
  m_syMax = 0.3;
  m_syMin = -0.3;
  
  return sc;
}

StatusCode StereoBase::execute(std::vector<SeedTrack*>& , std::vector<SeedHit*>* ){
  return StatusCode::SUCCESS;
}

void StereoBase::cleanup(std::vector<SeedHit*> hits[6])
{
  // clean up - copy hits to store
  SeedHits* hitsCont = getOrCreate<SeedHits,SeedHits>(evtSvc(),m_seedHitLocation);
  for ( int lay = 0; lay < 6; ++lay ) {
    for ( std::vector<SeedHit*>::iterator it = hits[lay].begin(); hits[lay].end() != it; ++it )
    {
      hitsCont->insert(*it);

    }
  } // lay
}

unsigned int StereoBase::collectHits(SeedTrack* seed, std::vector<SeedHit*> hits[6],
                                     std::vector<SeedHit*> yHits[6] ){

  unsigned int nCollected = 0;

  for ( int lay = 0; lay < 6; ++lay ) {

    if (hits[lay].empty() == true) continue;

    bool uLay = false;
    if ( lay == 0 || lay == 2 || lay == 4 ) uLay = true;

    const double zFirst = hits[lay].front()->z();
    const double slope = seed->xSlope(zFirst,TsaConstants::z0);
    Gaudi::XYZVector vec(1., TsaConstants::tilt*slope, -slope);
    Gaudi::XYZPoint point(seed->x(zFirst,TsaConstants::z0), hits[lay].front()->yMid(), zFirst );
    Gaudi::Plane3D plane = Gaudi::Plane3D(vec,point);
    Gaudi::XYZPoint iPoint;
    const double tol = 0.1*hits[lay].front()->length();

    for ( std::vector<SeedHit*>::iterator it = SeedFun::startStereo(hits[lay],point.x(),tol);
          hits[lay].end() != it; ++it )
    {
      SeedHit* hit = (*it);
      //if ( !Tsa::ClusFun::intersection(hit->clus(),plane,iPoint) ) continue;
      if ( !Tf::intersection(hit->tfHit(),plane,iPoint) ) continue;

      if ( uLay ) {
        if ( iPoint.y() > hit->yMax() + m_yTol ) continue;
        if ( iPoint.y() < hit->yMin() - m_yTol ) break;
      } else {
        if ( iPoint.y() > hit->yMax() + m_yTol ) break;
        if ( iPoint.y() < hit->yMin() - m_yTol) continue;
      }
     
      hit->setY( iPoint.y() );
      hit->setZ( iPoint.z() );
      yHits[lay].push_back( hit );
      ++nCollected;
    }
  } //hits

  return nCollected;
}
