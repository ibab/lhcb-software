// $Id: PatPV2DFit3D.cpp,v 1.7 2010-04-16 13:25:35 pmorawsk Exp $
// Include files

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/AlgFactory.h"
// #include "GaudiKernel/IUpdateManagerSvc.h"


// from gsl
#include "gsl/gsl_math.h"

// from Event
#include "Event/Track.h"
#include "VeloDet/DeVelo.h"
#include "DetDesc/IDetectorElement.h"

// local
#include "PatPV2DFit3D.h"
// #include "Random.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatPV2DFit3D
//
// 2005-06-22 : Olivier Callot from Mariusz Witek (original method by Ivan Kisel)
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( PatPV2DFit3D );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatPV2DFit3D::PatPV2DFit3D( const std::string& name,
                  ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator ),
    m_maxNumPv(0),
    m_maxIter(0),
    m_dzSeparHisto(0.0),
    m_saveSeedsAsPV(false),
    m_sTracks(0),
    m_phiOfSector(0),
    m_multiQualityPV(0),
    m_minBackQualityPV(0),
    m_dzQualityPV(0),
    m_inputTracks(0),
    m_outputVertices(0),
    m_pvSeedTool(0),
    m_inputTracksName(""),
    m_outputVerticesName(""),
    m_pvsfit(0),
    m_timer(0)
{
  declareProperty( "maxNumPv"        , m_maxNumPv      = 10             );
  declareProperty( "maxIter"         , m_maxIter       =  3             );
  declareProperty( "minMult"         , m_minMult       =  6             );
  declareProperty( "multiQualityPV"  , m_multiQualityPV   =  10         );
  declareProperty( "minBackQualityPV", m_minBackQualityPV =   2         );
  declareProperty( "dzQualityPV"     , m_dzQualityPV      =  20.*Gaudi::Units::mm     );
  declareProperty( "SaveSeedsAsPV"   , m_saveSeedsAsPV    =  false      );
  declareProperty( "MeasureTime"     , m_measureTime      =  false      );
  declareProperty( "InputTracksName"    , m_inputTracksName     =  LHCb::TrackLocation::RZVelo);
  declareProperty( "OutputVerticesName" , m_outputVerticesName  =  LHCb::RecVertexLocation::Velo2D );
}
//=============================================================================
// Destructor
//=============================================================================
PatPV2DFit3D::~PatPV2DFit3D() {};

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatPV2DFit3D::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg

  debug() << "==> Initialize" << endmsg;


  addTracks(100);
  
    //== Get Velo detector element, to get the R sector angle
  DeVelo* velo = getDet<DeVelo>( DeVeloLocation::Default );
  for (IDetectorElement::IDEContainer::const_iterator i = velo->childBegin();i!=velo->childEnd();++i) {
    debug () << " got " << (*i)->name() << endmsg;
    if ((*i)->name().find("VeloLeft") != std::string::npos) {
      debug() << "found left half!" << endmsg;
      m_velo[0] = (*i)->geometry();
    }
    if ((*i)->name().find("VeloRight") != std::string::npos) {
      debug() << "found right half!" << endmsg;
      m_velo[1] = (*i)->geometry();
    }
  }
  if (msgLevel(MSG::DEBUG)){
    Gaudi::XYZPoint localZero(0.,0.,0.);
    debug()<< "VeloLeft  position " << m_velo[0]->toGlobal(localZero)-localZero << endmsg;
    debug()<< "VeloRight position " << m_velo[1]->toGlobal(localZero)-localZero << endmsg;
  }

  // Access PVSeedTool
  m_pvSeedTool = tool<IPVSeeding>( "PVSeedTool", this );
  if( !m_pvSeedTool ) {
    err() << "Unable to retrieve the PVSeedTool" << endmsg;
    return  StatusCode::FAILURE;
  }

  // Access PVOfflineTool
  m_pvsfit = tool<IPVOfflineTool>("PVOfflineTool",this);
  if(!m_pvsfit) {
    err() << "Unable to retrieve the PVOfflineTool" << endmsg;
    return  StatusCode::FAILURE;
  }
  
  setProduceHistos(false);
//   std::cout << "XXXXXXXXXXXXXX2" << std::endl;
  if (m_measureTime){
    m_timer = tool<ISequencerTimerTool>( "SequencerTimerTool", this );
    m_timer->increaseIndent();
    m_timePrep  = m_timer->addTimer("getting tracks");
    m_timeHist  = m_timer->addTimer("producing histos");
    m_timeIns   = m_timer->addTimer("inserting reconstructed vertices");
    m_timeFit   = m_timer->addTimer("fitting vertices");
    m_timeTrMod = m_timer->addTimer("modyfing tracks");
    m_timer->decreaseIndent();
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatPV2DFit3D::execute() {
  debug() << "==> Execute" << endmsg;
  if (m_measureTime) m_timer->start(m_timePrep);
  m_inputTracks   = get<LHCb::Tracks>( m_inputTracksName );


  std::vector<LHCb::Track*> travec_2d;
  std::vector<LHCb::Track*> travec_3d;

  m_outputVertices  = new LHCb::RecVertices();
  put(m_outputVertices, m_outputVerticesName);


//   to aviod edge effects
  double maxZ =  (200. -10.)*Gaudi::Units::mm;

  // select 2d tracks and fill local bookkeeping table
  unsigned int nTracks = 0;
  for ( std::vector<LHCb::Track*>::const_iterator itTr = m_inputTracks->begin(); m_inputTracks->end() != itTr; itTr++ ) {
    nTracks++;
  }
//   int nTracks = m_inputTracks.size();
  if (nTracks > m_sTracks.size()) addTracks(nTracks - m_sTracks.size());
  std::vector<const LHCb::Track*> rtracks;
  if (m_measureTime) m_timer->stop(m_timePrep);

  int iTrack = 0;
  for (std::vector<LHCb::Track*>::const_iterator itT = m_inputTracks->begin(); m_inputTracks->end() != itT; itT++ ) {
    if (m_measureTime) m_timer->start(m_timeTrMod);
    LHCb::Track* pTr2d = (*itT);

    if (pTr2d->checkFlag( LHCb::Track::Invalid ))   continue;

    double zFirst  = pTr2d->firstState().z();
    double rFirst  = pTr2d->firstState().x();
    double trFirst = pTr2d->firstState().tx();
    double d2r0    = pTr2d->firstState().errX2();
    double d2tr    = pTr2d->firstState().errTx2();
    int    sector  = pTr2d->specific();
    double phi     = m_phiOfSector[ sector ];
    Gaudi::TrackSymMatrix cov = pTr2d->firstState().covariance();
    bool   isRight = sector > 3;
    
//================== eld fix =====================================
//     double xVeloOffset, yVeloOffset, zVeloOffset;
//     double r[2], x[2], y[2], tx, ty, z[2], xFirst, yFirst;
//     for (int it=0; it<2; it++){
//       if (isRight) {
//         xVeloOffset = m_boxOffsetRight[it].x();
//         yVeloOffset = m_boxOffsetRight[it].y();
//         zVeloOffset = m_boxOffsetRight[it].z();
//         z[it]       = m_zRight[it];
//       }else{
//         xVeloOffset = m_boxOffsetLeft[it].x();
//         yVeloOffset = m_boxOffsetLeft[it].y();
//         zVeloOffset = m_boxOffsetLeft[it].z();
//         z[it]       = m_zLeft[it];
//       }
//       r[it] =  rFirst + trFirst*(z[it] - zFirst);
//       x[it] =  r[it]*cos(phi);
//       y[it] =  r[it]*sin(phi);      
//       x[it] += xVeloOffset;
//       y[it] += yVeloOffset;
//       z[it] += zVeloOffset;
//     }
//     tx = (x[0] - x[1])/(z[0] - z[1]);
//     ty = (y[0] - y[1])/(z[0] - z[1]);
//     trFirst = tx*cos(phi) + ty*sin(phi);
//     xFirst = x[0] + tx*(zFirst - z[0]);
//     yFirst = y[0] + ty*(zFirst - z[0]);
//     phi = atan(yFirst/xFirst);
//     if (isRight) phi += M_PI;
//     rFirst = sqrt(xFirst*xFirst + yFirst*yFirst);
//     double z0      = zFirst - rFirst/trFirst; // where r = 0.
//================== old fix end =================================

//================== new fix =====================================
//     ROOT::Math::Random r(0);

    Gaudi::XYZPoint  position(rFirst*cos(phi), rFirst*sin(phi), zFirst);
    Gaudi::XYZVector slope(trFirst*cos(phi), trFirst*sin(phi), 1.0);
    if(msgLevel(MSG::DEBUG)){
      debug() << "Local  position " << position << endmsg;
      debug() << "Local  slope    " << slope    << endmsg;
    }
    if (isRight){
      position = m_velo[1]->toGlobal(position);
      slope    = m_velo[1]->toGlobal(slope);
    }
    else{
      position = m_velo[0]->toGlobal(position);
      slope    = m_velo[0]->toGlobal(slope);
    }
    if(msgLevel(MSG::DEBUG)){
      debug() << "Global position " << position << endmsg;
      debug() << "Global slope    " << slope    << endmsg;
    }
    slope.SetX(slope.x()/slope.z());
    slope.SetY(slope.y()/slope.z());
    slope.SetZ(1.0);
//     phi = atan(position.y()/position.x());
//     if (isRight) phi += M_PI;
    rFirst  = sqrt(position.x()*position.x() + position.y()*position.y());
    trFirst = sqrt(slope.x()*slope.x() + slope.y()*slope.y());
    double z0      = position.z() - rFirst/trFirst; // where r = 0.
//================== new fix end =================================
      

    if( maxZ > fabs(z0) ) {
      m_sTracks[iTrack]->setFlags(pTr2d->flag());
      m_sTracks[iTrack]->setType(LHCb::Track::Velo);
//       m_sTracks[iTrack]->firstState().setX(xFirst);
//       m_sTracks[iTrack]->firstState().setY(yFirst);
//       m_sTracks[iTrack]->firstState().setZ(zFirst);
//       m_sTracks[iTrack]->firstState().setTx(trFirst*cos(phi));
//       m_sTracks[iTrack]->firstState().setTy(trFirst*sin(phi));
      m_sTracks[iTrack]->firstState().setX(position.x());
      m_sTracks[iTrack]->firstState().setY(position.y());
      m_sTracks[iTrack]->firstState().setZ(position.z());
      m_sTracks[iTrack]->firstState().setTx(slope.x());
      m_sTracks[iTrack]->firstState().setTy(slope.y());

      cov(0,0) = d2r0*cos(phi)*cos(phi);
      cov(1,1) = d2r0*sin(phi)*sin(phi);
      cov(2,2) = d2tr*cos(phi)*cos(phi);
      cov(3,3) = d2tr*sin(phi)*sin(phi);
      m_sTracks[iTrack]->firstState().setCovariance(cov);

      rtracks.push_back(m_sTracks[iTrack]);
      travec_2d.push_back(pTr2d);
      travec_3d.push_back(m_sTracks[iTrack]);
    }
    iTrack++;
    if (m_measureTime) m_timer->stop(m_timeTrMod);
  }
  if (m_measureTime) m_timer->start(m_timeFit);
  std::vector<LHCb::RecVertex> rvts;
  StatusCode scfit = m_pvsfit->reconstructMultiPVFromTracks(rtracks,rvts);
  if (m_measureTime) m_timer->stop(m_timeFit);
  if (scfit == StatusCode::SUCCESS) {
    if (m_measureTime) m_timer->start(m_timeIns);
    // insert reconstructed vertices
     for(std::vector<LHCb::RecVertex>::iterator iv = rvts.begin(); iv != rvts.end(); iv++) {
       LHCb::RecVertex* vertex = new LHCb::RecVertex();
       *vertex = *iv;
       vertex->setTechnique(LHCb::RecVertex::Primary);
       Gaudi::XYZPoint pos_shift = vertex->position();
       pos_shift.SetZ(pos_shift.z() - 0.019);
       vertex->setPosition(pos_shift);

       // replace temporary quasi 3D tracks by original RZVelo tracks
       std::vector<LHCb::Track*> pv_tracks;

       SmartRefVector<LHCb::Track>::const_iterator iTr;
       for ( iTr = vertex->tracks().begin(); iTr != vertex->tracks().end(); iTr++ ) {
         std::vector<LHCb::Track*>::iterator itr_found;
         itr_found = std::find(travec_3d.begin(), travec_3d.end(), *iTr );
         if ( itr_found != travec_3d.end() ) {
           pv_tracks.push_back( travec_2d[ itr_found - travec_3d.begin() ] );
         }
       }
       vertex->clearTracks();
       for ( std::vector<LHCb::Track*>::iterator itput = pv_tracks.begin(); itput != pv_tracks.end(); itput++) {
         vertex->addToTracks(*itput);
       }
       m_outputVertices->insert(vertex);
     }
     if (m_measureTime) m_timer->stop(m_timeIns);
  }
  if (m_measureTime) m_timer->start(m_timeHist);
  if( produceHistos() ){
     for(std::vector<LHCb::RecVertex>::iterator iv = rvts.begin(); iv != rvts.end(); iv++) {
       double xv = iv->position().x();
       double yv = iv->position().y();
       double zv = iv->position().z();
       double rv = std::sqrt(xv*xv+yv*yv);
       plot1D(zv,"PV2DFit3DVertexZPosition", -200,200,200);
       plot1D(rv,"PV2DFit3DVertexTransversePosition", 0,1,200);       
     }
  }
  if (m_measureTime) m_timer->stop(m_timeHist);

  setFilterPassed(!rvts.empty());

  return StatusCode::SUCCESS;
}

void PatPV2DFit3D::addTracks(int n){
  for (int fill=0; fill<n; fill++){
    LHCb::Track* newTr = new LHCb::Track();
    newTr->setType( LHCb::Track::Velo);
    LHCb::State* newSt = new LHCb::State();
    newSt->setX(.0);
    newSt->setY(.0);
    newSt->setZ(.0);
    newSt->setTx(.0);
    newSt->setTy(.0);
    newTr->addToStates(*newSt);
    m_sTracks.push_back(newTr);
  }
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatPV2DFit3D::finalize() {

  for ( std::vector<LHCb::Track*>::iterator idel = m_sTracks.begin(); idel != m_sTracks.end(); idel++) {
    delete *idel;
  }
  debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}
