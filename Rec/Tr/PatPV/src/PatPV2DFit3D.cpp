// $Id: PatPV2DFit3D.cpp,v 1.5 2010-01-20 13:46:49 rlambert Exp $
// Include files

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IUpdateManagerSvc.h"


// from gsl
#include "gsl/gsl_math.h"

// from Event
#include "Event/Track.h"
#include "VeloDet/DeVelo.h"

// local
#include "PatPV2DFit3D.h"

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
    m_pvsfit(0)
{
  declareProperty( "maxNumPv"        , m_maxNumPv      = 10             );
  declareProperty( "maxIter"         , m_maxIter       =  3             );
  declareProperty( "minMult"         , m_minMult       =  6             );
  declareProperty( "multiQualityPV"  , m_multiQualityPV   =  10         );
  declareProperty( "minBackQualityPV", m_minBackQualityPV =   2         );
  declareProperty( "dzQualityPV"     , m_dzQualityPV      =  20.*Gaudi::Units::mm     );
  declareProperty( "SaveSeedsAsPV"   , m_saveSeedsAsPV    =  false      );
  declareProperty( "InputTracksName"    , m_inputTracksName     =  LHCb::TrackLocation::RZVelo);
  declareProperty( "OutputVerticesName" , m_outputVerticesName  =  LHCb::RecVertexLocation::Velo2D );
  
  //construct double arrays
  for (int it=0; it<2; it++)
  {
    m_boxOffsetLeft[it]=Gaudi::XYZVector();
    m_boxOffsetRight[it]=Gaudi::XYZVector();
    m_zLeft[it]=0;
    m_zRight[it]=0;
    
  }
  
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

  // subscribe to the updatemanagersvc with a dependency on the magnetic field svc
  IUpdateManagerSvc* updMgrSvc = svc<IUpdateManagerSvc>("UpdateManagerSvc", true);
  DeVelo* velo = getDet<DeVelo>( DeVeloLocation::Default );
  updMgrSvc->registerCondition( this,velo,&PatPV2DFit3D::updateVelo);
  // initialize with the current conditions
  if (!updMgrSvc->update(this)) {
    err() << "error when requesting update from UpdateManagerSvc" << endmsg;
    return  StatusCode::FAILURE;
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
  
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatPV2DFit3D::execute() {
  debug() << "==> Execute" << endmsg;

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

  int iTrack = 0;
  for (std::vector<LHCb::Track*>::const_iterator itT = m_inputTracks->begin(); m_inputTracks->end() != itT; itT++ ) {
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

    //     correction of VeLo Offset
    double xVeloOffset, yVeloOffset, zVeloOffset;
    double r[2], x[2], y[2], tx, ty, z[2], xFirst, yFirst;
    for (int it=0; it<2; it++){
      if (isRight) {
        xVeloOffset = m_boxOffsetRight[it].x();
        yVeloOffset = m_boxOffsetRight[it].y();
        zVeloOffset = m_boxOffsetRight[it].z();
        z[it]       = m_zRight[it];
      }else{
        xVeloOffset = m_boxOffsetLeft[it].x();
        yVeloOffset = m_boxOffsetLeft[it].y();
        zVeloOffset = m_boxOffsetLeft[it].z();
        z[it]       = m_zLeft[it];
      }
      r[it] =  rFirst + trFirst*(z[it] - zFirst);
      x[it] =  r[it]*cos(phi);
      y[it] =  r[it]*sin(phi);

      x[it] += xVeloOffset;
      y[it] += yVeloOffset;
      z[it] += zVeloOffset;
    }
    tx = (x[0] - x[1])/(z[0] - z[1]);
    ty = (y[0] - y[1])/(z[0] - z[1]);
    trFirst = tx*cos(phi) + ty*sin(phi);
    xFirst = x[0] + tx*(zFirst - z[0]);
    yFirst = y[0] + ty*(zFirst - z[0]);
    phi = atan(yFirst/xFirst);
    if (isRight) phi += M_PI;

    rFirst = sqrt(xFirst*xFirst + yFirst*yFirst);
    double z0      = zFirst - rFirst/trFirst; // where r = 0.

    if( maxZ > fabs(z0) ) {
      m_sTracks[iTrack]->setFlags(pTr2d->flag());
      m_sTracks[iTrack]->setType(LHCb::Track::Velo);
      m_sTracks[iTrack]->firstState().setX(xFirst);
      m_sTracks[iTrack]->firstState().setY(yFirst);
      m_sTracks[iTrack]->firstState().setZ(zFirst);
      m_sTracks[iTrack]->firstState().setTx(trFirst*cos(phi));
      m_sTracks[iTrack]->firstState().setTy(trFirst*sin(phi));

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
  }
  std::vector<LHCb::RecVertex> rvts;
  StatusCode scfit = m_pvsfit->reconstructMultiPVFromTracks(rtracks,rvts);
  if (scfit == StatusCode::SUCCESS) {

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
  }

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

//=============================================================================

StatusCode PatPV2DFit3D::updateVelo() {

  debug() << "updating Velo position" << endmsg;

  //== Get Velo detector element, to get the R sector angle
  DeVelo* velo = getDet<DeVelo>( DeVeloLocation::Default );

  std::vector<double> dum( 8, 0. );
  m_phiOfSector = dum;  // make it 8 words..

  //== First R sensor
  std::vector<DeVeloRType*>::const_iterator sensorRIt = velo->rSensorsBegin();

  for (unsigned int izone = 0; izone<4; izone++) {
    m_phiOfSector[izone] = 0.5*((*sensorRIt)->phiMinZone(izone)
                                + (*sensorRIt)->phiMaxZone(izone) );
    m_phiOfSector[izone+4] = m_phiOfSector[izone] + M_PI;
  };
  Gaudi::XYZPoint localZero(0.,0.,0.);
  for (int iSens = 0; iSens < 2; iSens++){
    int sensNo = iSens*40;
    m_boxOffsetLeft[iSens]  = velo->rSensor(sensNo  )->veloHalfBoxToGlobal(localZero) - localZero;
    m_boxOffsetRight[iSens] = velo->rSensor(sensNo+1)->veloHalfBoxToGlobal(localZero) - localZero;
    m_zLeft[iSens]  = velo->rSensor(sensNo  )->z();
    m_zRight[iSens] = velo->rSensor(sensNo+1)->z();
    debug()<< "offset" << sensNo << " " << m_boxOffsetLeft[iSens] << " " << m_boxOffsetRight[iSens] << endmsg;
  }
  return StatusCode::SUCCESS;
}
