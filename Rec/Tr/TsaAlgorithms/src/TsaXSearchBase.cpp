// $Id: TsaXSearchBase.cpp,v 1.1 2006-12-06 14:35:03 mneedham Exp $

// GaudiKernel
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "TsaXSearchBase.h"

#include "TsaKernel/SeedHit.h"
#include "TsaKernel/SeedTrack.h"

#include "Event/State.h"


TsaXSearchBase::TsaXSearchBase(const std::string& type,
                               const std::string& name,
                               const IInterface* parent):
  GaudiTool(type, name, parent){

  // constructer
 declareProperty("xSearch_xsParam", m_xsParam = 1.0/3125.0);
 declareProperty("arrow", m_arrow = 0.0022);
 declareProperty("seedHitLocation",  m_seedHitLocation = SeedHitLocation::Default);
 declareProperty("sector", m_sector = 0); 

 declareInterface<ITsaSeedStep>(this);
};

TsaXSearchBase::~TsaXSearchBase(){
  // destructer
}

StatusCode TsaXSearchBase::initialize(){

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize",sc);
  }
 
  m_sxMax = m_sxCut;
  m_sxMin = -m_sxCut;

  if (m_collectPolicy == "Curved"){
    m_collectFun = &TsaXSearchBase::collectXHitsCurved;    
  }
  else {
    m_collectFun = &TsaXSearchBase::collectXHitsLinear;    
  }

  return StatusCode::SUCCESS;
}

StatusCode TsaXSearchBase::execute(LHCb::State& aState, std::vector<SeedTrack*>& seeds, std::vector<SeedHit*> hits[6] ){

  // pick up the errors
  const double errTx = sqrt(aState.errTx2()); 
  m_sxMin = aState.tx() - 5.0*errTx;
  m_sxMax = aState.tx() - 5.0*errTx;

  // call the method
  StatusCode sc = execute(seeds, hits);
 
  // where we started..
  m_sxMax = m_sxCut;
  m_sxMin = -m_sxCut;

  return sc;
}

StatusCode TsaXSearchBase::execute(std::vector<SeedTrack*>& , std::vector<SeedHit*>* ){
  return StatusCode::SUCCESS;
}

void TsaXSearchBase::cleanup(std::vector<SeedHit*> hits[6]){
 // clean up - copy hits to store
 SeedHits* hitsCont = getOrCreate<SeedHits,SeedHits>(evtSvc(),m_seedHitLocation);
 for ( int lay = 0; lay < 6; ++lay ) {
   for ( std::vector<SeedHit*>::iterator it = hits[lay].begin(); hits[lay].end() != it; ++it ) hitsCont->insert(*it);  
 } // lay
}

void TsaXSearchBase::tagUsedHits(SeedHit* hit1, SeedHit* hit2,  std::vector<SeedHit*> select[3]) const{

 // tag the used hits 
 hit1->setUse1( true );
 hit2->setUse1( true );
 select[0].push_back( hit1 );
 select[2].push_back( hit2 );
 for ( std::vector<SeedHit*>::iterator it = select[0].begin(); select[0].end() != it; ++it ) {
   SeedHit* hit = (*it);
   if ( hit->use1() == 0 && hit->use2() == 0 ) continue;
   std::vector<SeedHit*>& skip2 = hit->skip();
   for ( std::vector<SeedHit*>::iterator it2 = select[2].begin(); select[2].end() != it2; ++it2 ) {
     SeedHit* hitb = (*it2);
     if ( hitb->use1() == 0 && hitb->use2() == 0 ) continue;
     skip2.push_back( hitb );
   }
 }
}


void TsaXSearchBase::collectXHitsLinear(std::vector<SeedHit*>& hits, 
                             const double x,
                             const double z,
                             const double sx,
                             const double win, std::vector<SeedHit*>& selected) const{
 
  std::vector<SeedHit*>::iterator it3 = SeedFun::startX(hits, x, z, sx,win);  
  for ( ; hits.end() != it3; ++it3 ) {
     SeedHit* hit = (*it3);
     double dx = hit->x() - x - sx*( hit->z() - z );
     if ( dx < -win ) continue;
     if ( dx >  win ) break;
     //hit->setUse(false);
     selected.push_back( hit );
  }
}

void TsaXSearchBase::collectXHitsCurved(std::vector<SeedHit*>& hits, 
                                        const double x,
                                        const double z,
                                        const double sx,
                                        const double win, std::vector<SeedHit*>& selected) const{

  double intersept = x - z*sx;
  std::vector<SeedHit*>::iterator it3 = SeedFun::startX(hits, x, z, sx,win); 
  for ( ; hits.end() != it3; ++it3 ) {
     SeedHit* hit = (*it3);
     double dx = hit->x() - x - sx*( hit->z() - z ) - m_arrow*intersept;
     if ( dx < -win ) continue;
     if ( dx >  win ) break;
     //     hit->setUse1( false );
     //hit->setUse2( false );
     selected.push_back( hit );
  }
}
