// $Id: PatPV2DFit3D.cpp,v 1.1 2009-11-11 15:11:49 witekma Exp $
// Include files

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"
#include "GaudiKernel/AlgFactory.h"

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
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "maxNumPv"        , m_maxNumPv      = 10             );
  declareProperty( "maxIter"         , m_maxIter       =  3             );
  declareProperty( "minMult"         , m_minMult       =  6             );
  declareProperty( "x0MS"            , m_x0MS          =  0.01          );
  declareProperty( "multiQualityPV"  , m_multiQualityPV   =  10         );
  declareProperty( "minBackQualityPV", m_minBackQualityPV =   2         );
  declareProperty( "dzQualityPV"     , m_dzQualityPV      =  20.*Gaudi::Units::mm     );
  declareProperty( "SaveSeedsAsPV"   , m_saveSeedsAsPV    =  false      );
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
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

//    double X0 = 0.0048; // 440 um of Si
  double X0 = m_x0MS;
  m_scatCons = (13.6*sqrt(X0)*(1.+0.038*log(X0))) / ( 0.400 * Gaudi::Units::GeV );


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
  m_boxOffsetLeft  = velo->rSensor(0)->veloHalfBoxToGlobal(localZero) - localZero;
  m_boxOffsetRight = velo->rSensor(1)->veloHalfBoxToGlobal(localZero) - localZero;
  std::cout << "velo offset" << m_boxOffsetLeft << "  " << m_boxOffsetRight << std::endl;
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

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatPV2DFit3D::execute() {

  debug() << "==> Execute" << endmsg;

  m_inputTracks   = get<LHCb::Tracks>( m_inputTracksName );
  std::vector<const LHCb::Track*> rtracks;

  std::vector<LHCb::Track*> travec_2d;
  std::vector<LHCb::Track*> travec_3d;

  m_outputVertices  = new LHCb::RecVertices();
  put(m_outputVertices, m_outputVerticesName);

  std::vector<myTrack> myTracks;
  myTracks.reserve( m_inputTracks->size() );
  std::vector<myTrack>::iterator itMyTr;
  std::vector<myVertex>::iterator itMyPv;

//   to aviod edge effects
//   double maxZ =  (200. -10.)*Gaudi::Units::mm;


  // select 2d tracks and fill local bookkeeping table
  for ( std::vector<LHCb::Track*>::const_iterator itT = m_inputTracks->begin();
        m_inputTracks->end() != itT; itT++ ) {
    LHCb::Track* pTr2d = (*itT);

    if (pTr2d->checkFlag( LHCb::Track::Invalid ))   continue;
    //    if ( pTr2d->checkFlag( LHCb::Track::Backward ))   continue;
    //    if ( int(pTr2d->specific())>3) continue;

    LHCb::Track* new2d = new LHCb::Track();
    new2d->copy(*pTr2d);

    double zFirst  = pTr2d->firstState().z();
    double rFirst  = pTr2d->firstState().x();
    double trFirst = pTr2d->firstState().tx();
    double d2r0    = pTr2d->firstState().errX2();
    double d2tr    = pTr2d->firstState().errTx2();
    int    sector  = pTr2d->specific();
    double phi     = m_phiOfSector[ sector ];
    Gaudi::TrackSymMatrix cov = pTr2d->firstState().covariance();

//     correction of VeLo Offset
    double phi_h   = phi;
    bool   isRight = sector > 3;
    double xFirst  = rFirst*cos(phi);
    double yFirst  = rFirst*sin(phi);

     double xVeloOffset, yVeloOffset, zVeloOffset;
     if (isRight){
       xVeloOffset = m_boxOffsetRight.x();
       yVeloOffset = m_boxOffsetRight.y();
       zVeloOffset = m_boxOffsetRight.z();
     }else{
       xVeloOffset = m_boxOffsetLeft.x();
       yVeloOffset = m_boxOffsetLeft.y();
       zVeloOffset = m_boxOffsetLeft.z();
     }
     xFirst += xVeloOffset;
     yFirst += yVeloOffset;
     zFirst += zVeloOffset;
     phi     = atan(yFirst/xFirst);
     if (isRight) phi += M_PI;
     rFirst  = sqrt(xFirst*xFirst + yFirst*yFirst);
     trFirst*=cos(phi-phi_h);

    double addScat = m_scatCons * trFirst;
    double z0      = zFirst - rFirst/trFirst; // where r = 0.
    d2r0 += (addScat * (zFirst-z0))*(addScat * (zFirst-z0));

    new2d->firstState().setX(xFirst);
    new2d->firstState().setY(yFirst);
    new2d->firstState().setZ(zFirst);
    new2d->firstState().setTx(trFirst*cos(phi));
    new2d->firstState().setTy(trFirst*sin(phi));
    new2d->setType( LHCb::Track::Velo);

    cov(0,0) = d2r0*cos(phi)*cos(phi);
    cov(1,1) = d2r0*sin(phi)*sin(phi);
    cov(2,2) = d2tr*cos(phi)*cos(phi);
    cov(3,3) = d2tr*sin(phi)*sin(phi);
    new2d->firstState().setCovariance(cov);

    // add MS from RF foil, constant Pt
    rtracks.push_back(new2d);
    travec_2d.push_back(pTr2d);
    travec_3d.push_back(new2d);

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
           pv_tracks.push_back( travec_2d[ itr_found - travec_3d.end() ] );
         }
       }
       vertex->clearTracks();
       for ( std::vector<LHCb::Track*>::iterator itput = pv_tracks.begin(); itput != pv_tracks.end(); itput++) {
         vertex->addToTracks(*itput);
       }   
       m_outputVertices->insert(vertex);
     }

  }

  // delete created quasi-3D tracks
  for ( std::vector<const LHCb::Track*>::iterator idel = rtracks.begin(); idel != rtracks.end(); idel++) {
    delete *idel;
  } 

  setFilterPassed(!rvts.empty());

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatPV2DFit3D::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
