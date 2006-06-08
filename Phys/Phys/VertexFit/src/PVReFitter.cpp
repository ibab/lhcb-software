// $Id: PVReFitter.cpp,v 1.4 2006-06-08 17:10:20 xieyu Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

#include "Event/Vertex.h"
#include "Event/PrimVertex.h"

// local
#include "PVReFitter.h"

using namespace LHCb ;
using namespace Gaudi::Units;
//-----------------------------------------------------------------------------
// Implementation file for class : PVReFitter
//
// 2006-06-08 : Yuehong Xie
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<PVReFitter>          s_factory ;
const        IToolFactory& PVReFitterFactory = s_factory ; 

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PVReFitter::PVReFitter( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IPVReFitter>(this);

  declareProperty( "maxIter", m_maxIter = 10);
  declareProperty( "MaxDeltaChi2", m_maxDeltaChi2 = 0.001);

}

//=============================================================================
// Destructor
//=============================================================================
PVReFitter::~PVReFitter() {};

//=============================================================================
// Initialize
//=============================================================================
StatusCode PVReFitter::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;
 
  return sc;
};

//=============================================================================
// refit PV
//=============================================================================
StatusCode PVReFitter::reFit(LHCb::Vertex* PV) {

  debug() <<"Now reFit PV"<< endreq;

  StatusCode sc = StatusCode::SUCCESS;

  if(!(PV->isPrimary())) {
    debug() <<"PVReFitter is used to reFit a  non-PV"<< endreq;
    return StatusCode::FAILURE;    
  }

  PrimVertex* primvtx=dynamic_cast<PrimVertex*>(PV);
  if(!primvtx) return StatusCode::FAILURE;
 
  std::vector<LHCb::Track*> tracks;

  SmartRefVector<LHCb::Track> pvtracks = primvtx->tracks();
  for( SmartRefVector<LHCb::Track>::iterator trIt = pvtracks.begin();
       pvtracks.end() != trIt; trIt++) {
    LHCb::Track* track = *trIt;
    tracks.push_back(track);
  }
                                                                                                                                  
  sc=fitPV( PV , tracks);
  if(!sc.isSuccess()) {
    debug() << "fitPV fails" <<endreq;
    return StatusCode::FAILURE;
  }

  return sc;
}
 
//=============================================================================
// remove track used for a LHCb::Particle and refit PV
//============================================================================= 
StatusCode PVReFitter::remove(LHCb::Particle* part,  LHCb::Vertex* PV) {

  debug() <<"Now remove and reFit "<< endreq;

  if(!(PV->isPrimary())) {
    debug() <<"PVReFitter is used to remove a trackfrom a  non-PV"<< endreq;
    return StatusCode::FAILURE;
  }
                                                                                                                                  
  PrimVertex* primvtx=dynamic_cast<PrimVertex*>(PV);
  if(!primvtx) return StatusCode::FAILURE;

  std::vector<const LHCb::Track*> dautracks;
  getFinalTracks(part, dautracks);

  std::vector<LHCb::Track*> tracks;
  SmartRefVector<LHCb::Track> pvtracks = primvtx->tracks();
  for( SmartRefVector<LHCb::Track>::iterator trIt = pvtracks.begin();
       pvtracks.end() != trIt; trIt++) {
    LHCb::Track* track = *trIt;
    bool drop=false;
    for( std::vector<const LHCb::Track*>::iterator idtrack = dautracks.begin();
         dautracks.end() != idtrack; idtrack++ ) {
      if(track==*idtrack) { drop=true; break; }
    }
    if(!drop) tracks.push_back(track);
  }

  StatusCode sc=fitPV( PV , tracks);
  if(!sc.isSuccess()) {
    debug() << "fitPV fails" <<endreq;
    return StatusCode::FAILURE;
  }

  return sc;

}


//=============================================================================
// fit PV from a vector of tracks
//=============================================================================
StatusCode PVReFitter::fitPV(LHCb::Vertex* PV, std::vector<LHCb::Track*> & tracks) {
  StatusCode sc;

  debug() <<"Now entering fitPV!"<<endreq;
                                                                                                                                  
  if(tracks.size()<2) {
    debug() << "number of track left for the PV "<< tracks.size() <<endreq;
    return StatusCode::FAILURE;
  }
  
  LHCb::Track* tr1 =  tracks.back(); 
  tracks.pop_back();
  LHCb::Track* tr2 =  tracks.back();
  tracks.pop_back();

  if(!tr1 || !tr2 ) {
    debug() << "Null track pointer found ! " << endreq;
    return StatusCode::FAILURE;
  }

  sc = seedPV(PV, tr1, tr2);
  if(sc.isFailure()) {
    debug() << "Fail to seedPV" << endreq;
    return StatusCode::FAILURE;
  }

  for( std::vector<LHCb::Track*>::iterator itrack = tracks.begin(); tracks.end() != itrack; itrack++ ) 
  {
    LHCb::Track* tr = *itrack;
    sc = addTr(PV, tr);
    if(sc.isFailure()) {
      debug() << "Fail to addTr" << endreq;
      return StatusCode::FAILURE;
    }
  }

  return sc;
}


//=============================================================================
// get final tracks of a particle
//=============================================================================
void PVReFitter::getFinalTracks(LHCb::Particle* part, std::vector<const LHCb::Track*> & tracks)
{
  const LHCb::ProtoParticle*   proto  = part->proto() ;

  if( proto ) {
    const LHCb::Track* thetrack = proto->track();
    if(thetrack) tracks.push_back(thetrack);
  } else {
    const SmartRefVector< LHCb::Particle > & Prods = part->daughters();
    SmartRefVector< LHCb::Particle >::const_iterator iProd;
    for (iProd=Prods.begin(); iProd !=Prods.end(); ++iProd){
      const LHCb::Particle* daughter = *iProd;
      getFinalTracks(const_cast<LHCb::Particle*>(daughter), tracks);
    }
  }                                                                                                                                  
}

//=============================================================================
// seeding a PV with two tracks
//=============================================================================

StatusCode PVReFitter::seedPV(LHCb::Vertex* PV,
                    LHCb::Track* tr1,
                    LHCb::Track* tr2)
{
  StatusCode sc;

  PrimVertex* primvtx=dynamic_cast<PrimVertex*>(PV);
  if(!primvtx) return StatusCode::FAILURE;

  primvtx->clearTracks();

  double chi2Fit;

  Gaudi::XYZPoint position;
  Gaudi::SymMatrix3x3 cov;

  // fitting ...

  PV->setPosition(position);
  PV->setCovMatrix(cov);
  
  double chi2new = chi2Fit;
  PV->setChi2(chi2new);

  int nDoFnew = 1;
  PV->setNDoF(nDoFnew);

  primvtx->addToTracks(tr1);
  primvtx->addToTracks(tr2);

  return sc;
}

//=============================================================================
// add a track to a PV
//=============================================================================

StatusCode  PVReFitter::addTr(LHCb::Vertex* PV,
                              LHCb::Track* tr)
{
  StatusCode sc;

  PrimVertex* primvtx=dynamic_cast<PrimVertex*>(PV);
  if(!primvtx) return StatusCode::FAILURE;

  double chi2Fit;

  Gaudi::XYZPoint position;
  Gaudi::SymMatrix3x3 cov;

  //fitting ...
                                                                                                                                                                        
  PV->setPosition(position);
  PV->setCovMatrix(cov);

  double chi2new = chi2Fit + PV->chi2();
  PV->setChi2(chi2new);

  int nDoFnew = 2 + PV->nDoF() ;
  PV->setNDoF(nDoFnew);

  primvtx->addToTracks(tr);

  return sc;
}

