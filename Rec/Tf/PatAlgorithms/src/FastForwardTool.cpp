// Include files

#include <algorithm>
#include <array>

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

#include "TfKernel/HitExtension.h"
#include "TfKernel/RecoFuncs.h"
#include "TfKernel/RegionID.h"

#include "OTDet/DeOTDetector.h"

// local
#include "FastForwardTool.h"
#include "PatFwdPlaneCounter.h"
#include "PatFwdRegionCounter.h"
#include "PatFwdFitLine.h"
#include "PatFwdFitParabola.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FastForwardTool
//
// 2005-10-04 : Olivier Callot
//-----------------------------------------------------------------------------


DECLARE_TOOL_FACTORY( FastForwardTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FastForwardTool::FastForwardTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_nnSwitch(false)
{
  declareInterface<IPatForwardTool>(this);
  declareInterface<ITracksFromTrack>(this);

  declareProperty( "ZAfterVelo"            , m_zAfterVelo            = 1640. * Gaudi::Units::mm );
  declareProperty( "YCompatibleTol"        , m_yCompatibleTol        =   10. * Gaudi::Units::mm );
  declareProperty( "YCompatibleTolFinal"   , m_yCompatibleTolFinal   =   10. * Gaudi::Units::mm );
  declareProperty( "MinXPlanes"            , m_minXPlanes            =   4   );
  declareProperty( "MinPlanes"             , m_minPlanes             =   9   );
  declareProperty( "MinOTDrift"            , m_minOTDrift            = -0.8 * Gaudi::Units::mm );
  declareProperty( "MaxOTDrift"            , m_maxOTDrift            =  2.7 * Gaudi::Units::mm );
  declareProperty( "MaxSpreadX"            , m_maxSpreadX            =  3.  * Gaudi::Units::mm );
  declareProperty( "MaxSpreadSlopeX"       , m_maxSpreadSlopeX       = .010  );
  declareProperty( "MaxSpreadY"            , m_maxSpreadY            =  1.0  );
  declareProperty( "MaxSpreadSlopeY"       , m_maxSpreadSlopeY       = 70.   );
  declareProperty( "XMatchTol"             , m_xMatchTol             =  1. * Gaudi::Units::mm  );
  declareProperty( "MinPt"                 , m_minPt                 = 50. * Gaudi::Units::MeV );
  declareProperty( "MinMomentum"           , m_minMomentum           =  1. * Gaudi::Units::GeV );
  declareProperty( "MaxChi2"               , m_maxChi2               =  20       );
  declareProperty( "MaxChi2Track"          , m_maxChi2Track          =  20       );
  declareProperty( "MinHits"               , m_minHits               =  14       );
  declareProperty( "MinOTHits"             , m_minOTHits             =  16       );
  declareProperty( "CenterOTYSize"         , m_centerOTYSize         =  60 * Gaudi::Units::mm );
  declareProperty( "MaxDeltaY"             , m_maxDeltaY             =  40.      );
  declareProperty( "MaxDeltaYSlope"        , m_maxDeltaYSlope        =  400.      );

  declareProperty( "RangePerMeV"           , m_rangePerMeV           =  5250. * Gaudi::Units::GeV );
  declareProperty( "MinRange"              , m_minRange              =   300. * Gaudi::Units::mm  );
  declareProperty( "RangeErrorFraction"    , m_rangeErrorFraction    =  0.60        );

  declareProperty( "WithTTEstimate"        , m_withTTEstimate        = true  );
  declareProperty( "ZTTProj"               , m_zTTProj               =  2500. * Gaudi::Units::mm );
  declareProperty( "ZTTField"              , m_zTTField              =  1750. * Gaudi::Units::mm );
  declareProperty( "MaxTTProj"             , m_maxTTProj             =    10. * Gaudi::Units::mm );
  declareProperty( "TTToTScale"            , m_ttToTScale            =   200. * Gaudi::Units::mm );
  declareProperty( "TTToTWindow"           , m_ttToTWindow           =   900. * Gaudi::Units::mm );

  declareProperty("StateErrorX2",  m_stateErrorX2  =   4.0);
  declareProperty("StateErrorY2",  m_stateErrorY2  = 400.);
  declareProperty("StateErrorTX2", m_stateErrorTX2 =  6.e-5);
  declareProperty("StateErrorTY2", m_stateErrorTY2 =  1.e-4);
  declareProperty("StateErrorP",   m_stateErrorP   =  0.15);

  declareProperty("AddTTClusterName", m_addTtToolName = "PatAddTTCoord" );

  declareProperty( "WithoutBField"         , m_withoutBField         = false);

  declareProperty( "DebugToolName"         , m_debugToolName    = "" );
  declareProperty( "WantedKey"             , m_wantedKey        = -1 );
  declareProperty( "WantedVelo"            , m_wantedVelo       = -1 );
}
//=============================================================================
// Destructor
//=============================================================================
FastForwardTool::~FastForwardTool() {}

//=========================================================================
//  Initialization, get the tools
//=========================================================================
StatusCode FastForwardTool::initialize ( ) {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;


  m_tHitManager  = tool<Tf::TStationHitManager<PatForwardHit> >("PatTStationHitManager");
  m_fwdTool      = tool<FastFwdTool>( "FastFwdTool", this);
  m_ttHitManager = tool<Tf::TTStationHitManager <PatTTHit> >("PatTTStationHitManager");

  if ( "" != m_addTtToolName ) {
    m_addTTClusterTool = tool<IAddTTClusterTool>( m_addTtToolName, this );
  } else {
    m_addTTClusterTool = NULL;
  }

  m_debugTool = 0;
  if ( "" != m_debugToolName ) {
    m_debugTool = tool<IPatDebugTool>( m_debugToolName );
  }

  for (unsigned int sta = 0; sta < m_nSta; sta ++){
    int nb = 0;
    double avZ = 0.;    
    for (unsigned int lay = 0; lay< m_nLay; lay += 3){
      const Tf::EnvelopeBase* regionB = m_tHitManager->region(sta,lay,0);
      nb += 1;
      avZ += regionB->z();
    }
    m_zStation.push_back( avZ/nb );
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Main execution: Extend a track.
//=========================================================================
void FastForwardTool::forwardTrack( const LHCb::Track* tr, LHCb::Tracks* output ) {

  std::vector<LHCb::Track*> outvec;
  tracksFromTrack(*tr,outvec).ignore();

  for ( unsigned int i=0; i<outvec.size(); i++ ) {
    output->insert(outvec[i]);
  }
}

//=========================================================================
//  Main method: From a Velo seed, produce all the forward tracks.
//=========================================================================
StatusCode FastForwardTool::tracksFromTrack( const LHCb::Track& seed,
                                             std::vector<LHCb::Track*>& output ){

  LHCb::Track* tr = (LHCb::Track*) &seed;

  bool isDebug = msgLevel( MSG::DEBUG );
  if ( tr->key() == m_wantedVelo ) isDebug = true;

  if ( tr->checkFlag( LHCb::Track::Invalid  ) ) return StatusCode::SUCCESS;
  if ( tr->checkFlag( LHCb::Track::Backward ) ) return StatusCode::SUCCESS;

  PatFwdTrackCandidate track( tr );

  const LHCb::State& state = seed.closestState( m_fwdTool->zReference() );
  double dzState =  m_fwdTool->zReference() - state.z();
  m_errY = sqrt( state.errY2() + dzState * dzState * state.errTy2() );

  if ( isDebug ) {
    info() << format( "**** Velo track %3d  x%8.2f  y%8.2f  tx%9.5f  ty%9.5f q/p = %8.6f  tolY %8.2f",
                      tr->key(), track.xStraight( m_zAfterVelo ),
                      track.yStraight( m_zAfterVelo ),
                      track.slX(), track.slY(), 1000. * track.qOverP(), m_errY ) << endmsg;
  }

  if ( m_withTTEstimate ) {
    m_ttOffset = getTTOffset( track, isDebug );
  } else {
    m_ttOffset = 10000.;
  }

  //== Build the initial list of X candidates

  buildXCandidatesList( track );

  int minPlanes = m_minPlanes;  //== Initial value, can be updated later...

  std::vector<PatFwdTrackCandidate> xCandidates;
  std::vector<PatFwdTrackCandidate>::iterator itL;

  int minOTX = int( 1.5 * m_minXPlanes );

  //== Convert candidates to temporary tracks.

  for( itL = m_candidates.begin(); m_candidates.end() != itL; ++itL ) {
    PatFwdTrackCandidate temp = *itL;
    std::stable_sort( temp.coordBegin(), temp.coordEnd(), LowerByID() );

    if ( isDebug ) {
      info() << "Chi2/nDof = " << temp.chi2PerDoF() << " nDoF " << temp.nDoF()
             << " dist at center " << m_fwdTool->distAtMagnetCenter( temp )
             << endmsg;
      printTrack( temp );
    }

    //=== Is this candidate a clone of an existing one. Else ignore or replace...

    bool abort = false;
    for ( std::vector<PatFwdTrackCandidate>::iterator itCand = xCandidates.begin();
          xCandidates.end() != itCand; ++itCand ) {
      if ( isClone( temp, *itCand ) ) {
        bool keep = false;
        double diffChi2 =  temp.chi2PerDoF() - (*itCand).chi2PerDoF() ;
        if ( temp.coords().size() >  (*itCand).coords().size() + .3 * diffChi2) keep = true;
        if ( temp.coords().size() == (*itCand).coords().size() &&
             temp.chi2PerDoF()    <  (*itCand).chi2PerDoF()    ) keep = true;
        if ( keep ) {
          if ( isDebug ) {
            info() << "--- is a clone of " << itCand - xCandidates.begin() << " longer/with better Chi2. Replace" << endmsg;
          }
          *itCand = temp;
        } else {
          if ( isDebug ) {
            info() << "--- is a clone of " << itCand - xCandidates.begin() << " shorter/with worst Chi2. Ignore." << endmsg;
          }
        }
        abort = true;
        break;
      }
    }
    if ( abort ) continue;

    //== Check the chi2 with magnet center constraint.

    if ( m_maxChi2Track > temp.chi2PerDoF()  &&
         (!m_withoutBField || fabs(temp.slX()-temp.xSlope(0))<0.005)){
      //== Count how many weighted hits
      PatFwdRegionCounter regions( temp.coordBegin(), temp.coordEnd() );
      int nbHit = regions.nbOT() + 2*regions.nbIT();
      bool inCenter = m_centerOTYSize > fabs( temp.y( 0. ) );

      if ( minOTX <= nbHit || inCenter ) {
        xCandidates.push_back( temp );
        if ( isDebug ) info() << "+++ Store candidate " << xCandidates.size()-1 << endmsg;
      } else {
        if ( isDebug ) info() << " --- not enough hits " << nbHit << endmsg;
      }
    } else {
      if ( isDebug ) info() << " --- bad Chi2. Reject" << endmsg;
    }
  }

  if ( isDebug ) {
    info() << "************ List of X candidates , N = " << xCandidates.size() << endmsg;
    for ( itL = xCandidates.begin(); xCandidates.end() != itL; ++itL ) {
      info() << "Candidate " << itL - xCandidates.begin()
             << " Chi2/nDof = " << (*itL).chi2PerDoF() << endmsg;
      printTrack( *itL );
    }
    if ( xCandidates.size() > 0 ) debug() << "---- Now get the stereo hits on these ---" << endmsg;
  }

  //== Now try to get space track from these X track.

  std::vector<PatFwdTrackCandidate> goodCandidates;
  int maxPlanes = 0;

  for ( itL = xCandidates.begin(); xCandidates.end() != itL; ++itL ) {
    if ( isDebug ) info() << "--- Candidate " << itL - xCandidates.begin()
                          << "  X cord size " << (*itL).coordEnd() - (*itL).coordBegin()
                          << endmsg;

    PatFwdHits::iterator itH;
    PatFwdTrackCandidate& temp = *itL;
    temp.setSelectedCoords( );

    double qOverP = 1000. * m_fwdTool->qOverP( temp );  // in 1/GeV

    if (m_withoutBField) {
      if (m_minMomentum !=0)
        qOverP = 1/m_minMomentum;
      else
        qOverP = 1;
    }

    double tol = m_maxSpreadY + 0.5 * m_errY + m_maxSpreadSlopeY * qOverP *  qOverP;

    if ( isDebug ) info() << "Adding stereo coordinates, tol = " << tol << endmsg;

    if ( !fillStereoList( temp, tol ) ) continue; // Get the stereo coordinates

    if ( isDebug )  printTrack( temp );
    temp.setSelectedCoords( );

    //== Fit and reject if not good enough

    if ( !m_fwdTool->fitStereoCandidate( temp, m_maxChi2, minPlanes, isDebug ) ) {
      if ( isDebug ) {
        info() << "Rejected in fitStereo, minPlanes " << minPlanes << endmsg;
        printTrack( temp );
      }
      continue;
    }

    temp.cleanCoords();
    m_fwdTool->chi2PerDoF( temp );  //== compute and store Chi2PerDoF

    //== Remove stereo coordinates incompatible in Y

    double yTol = m_yCompatibleTolFinal;
    if ( !m_fwdTool->removeYIncompatible( temp, yTol, minPlanes ) ) {
      if ( isDebug ) {
        info() << "Rejected by removeYIncompatible, minPlanes " << minPlanes << " tol " << yTol << endmsg;
        printTrack( temp );
      }
      continue;
    }
    temp.cleanCoords();
    if ( isDebug ) info() << "  ... Y is compatible" << endmsg;

    double quality = 0.;

    // Enough stereo planes
    PatFwdPlaneCounter fullCount( temp.coordBegin(), temp.coordEnd() );
    int nbY = fullCount.nbStereo();
    if ( 4 > nbY ) {
      if ( isDebug ) info() << "Not enough Y planes : " << nbY << endmsg;
      continue;
    }
    double yChangeTol = m_maxDeltaY + m_maxDeltaYSlope * qOverP * qOverP;

    if ( yChangeTol < fabs(  m_fwdTool->changeInY( temp ) ))  {
      if ( isDebug ) info() << "  --- Too big change in Y : " <<  m_fwdTool->changeInY( temp )
                            << " max " << yChangeTol << endmsg;
      continue;
    }

    quality  = 5. * fabs(  m_fwdTool->changeInY( temp ) ) / yChangeTol;
    quality += temp.chi2PerDoF() / 10.;
    quality += 10 * fabs(qOverP);  // low momentum are worse

    temp.setQuality( quality );

    //== Verify if enough OT measurements, counting IT for 2/plane
    //== Ignore the y central region, OT inefficient there.

    PatFwdRegionCounter regions( temp.coordBegin(), temp.coordEnd() );
    int nbHit = regions.nbOT() + 2*regions.nbIT();
    temp.setNbIT( regions.nbIT() );
    temp.setNbOT( regions.nbOT() );

    bool inCenter = m_centerOTYSize > fabs( temp.y( 0. ) );
    if ( !inCenter ) {
      if ( m_minHits > nbHit ){
        if ( isDebug ) info() << "  --- Not enough hits : " << nbHit << endmsg;
        continue;
      }
      if ( temp.nbIT() == 0 && temp.nbOT() < m_minOTHits ) {
        if ( isDebug ) info() << " Too few OT for OT only track : " << temp.nbOT() << endmsg;
        continue;
      }
    }
    if ( isDebug ) {
      info() << "  ... fit OK  chi2 " << temp.chi2PerDoF() << " nDoF " << temp.nDoF()
             << " dist at center " << m_fwdTool->distAtMagnetCenter( temp )
             << " qOverP " << qOverP
             << " quality " << quality
             << endmsg;
      printTrack( temp );
    }

    int nbPlanes = fullCount.nbDifferent();
    if ( maxPlanes < nbPlanes ) maxPlanes = nbPlanes;

    goodCandidates.push_back( temp );

    //== Update requirement according to already found good solutions...
    PatFwdPlaneCounter planeCounter( temp.coordBegin(), temp.coordEnd() );
    if ( minPlanes+1 < planeCounter.nbDifferent() ) minPlanes =  planeCounter.nbDifferent()-1;
  }
  // added for Tr/NNTools -- sort all candidates with respect to PatQuality
  if( this->nnSwitch()){
    std::stable_sort( goodCandidates.begin(), goodCandidates.end(), sortQuality());
    // loop over all candidates
    std::vector<PatFwdTrackCandidate>::iterator iall;
    int cand_count = 0;
    for( iall = goodCandidates.begin(); goodCandidates.end() != iall; ++iall){
      if(goodCandidates.size() == 1){
        (*iall).setCand1stQuality((*iall).quality());
        (*iall).setCand2ndQuality(0.);
      }
      ++cand_count;
      if(goodCandidates.size() > 1){
        (*iall).setCand1stQuality((*iall).quality());
        std::vector<PatFwdTrackCandidate>::iterator iallb;
        bool cand2nd = false;
        for( iallb = goodCandidates.begin(); goodCandidates.end() != iallb;
             ++iallb){
          if( (*iall).quality() == (*iallb).quality()) continue;
          if( !cand2nd){
            (*iall).setCand2ndQuality((*iallb).quality());
            cand2nd = true;
          }
        }
      }
    }
  }
  // end of NNTools loop
  //================================================================================
  //  Now some filtering of tracks, in case of multiple candidates.
  //================================================================================

  if ( 1 < goodCandidates.size()  ) {

    // remove track with sensibly lower number of planes
    int minPlanes = maxPlanes - 1;
    double bestQuality = 1000.;
    int maxOT = 0;

    if ( isDebug ) info() << "Require enough planes : " << minPlanes << endmsg;
    std::vector<PatFwdTrackCandidate> tempCandidates( goodCandidates );
    goodCandidates.clear();
    for ( itL = tempCandidates.begin(); tempCandidates.end() != itL; ++itL ) {
      PatFwdPlaneCounter tmp( (*itL).coordBegin(), (*itL).coordEnd() );
      if ( tmp.nbDifferent() >= minPlanes ) {
        goodCandidates.push_back( *itL );
        if ( (*itL).quality() < bestQuality ) bestQuality = (*itL).quality();
      } else {
        if ( isDebug ) info() << "Ignore candidate " << itL-tempCandidates.begin()
                              << " : not enough planes = " << tmp.nbDifferent() << endmsg;
      }
    }
    // remove worst quality
    bestQuality += 1.0;
    tempCandidates = goodCandidates;
    goodCandidates.clear();
    for ( itL = tempCandidates.begin(); tempCandidates.end() != itL; ++itL ) {
      if ( (*itL).quality() < bestQuality ) {
        goodCandidates.push_back( *itL );
        if ( 2*(*itL).nbIT() + (*itL).nbOT() > maxOT ) maxOT =  2*(*itL).nbIT()+(*itL).nbOT();
      } else {
        if ( isDebug ) info() << "Ignore candidate " << itL-tempCandidates.begin()
                              << " : quality too low = " << (*itL).quality() << endmsg;
      }
    }
    // remove if sensibly less OT
    if ( 24 < maxOT ) maxOT = 24;
    maxOT -= 4;
    tempCandidates = goodCandidates;
    goodCandidates.clear();
    for ( itL = tempCandidates.begin(); tempCandidates.end() != itL; ++itL ) {
      if ( 2*(*itL).nbIT() + (*itL).nbOT() > maxOT || m_centerOTYSize > fabs( (*itL).y(0.) ) ) {
        goodCandidates.push_back( *itL );
      } else {
        if ( isDebug ) info() << "Ignore candidate " << itL-tempCandidates.begin()
                              << " : not enough OT = " << (*itL).nbOT() << " mini " << maxOT << endmsg;
      }
    }
  }

  if ( isDebug ) info() << "Storing " << goodCandidates.size() << " good tracks " << endmsg;
  //=== Store tracks...
  for ( itL = goodCandidates.begin(); goodCandidates.end() != itL; ++itL ) {
    if ( isDebug ) {
      info() << "    track # " << itL - goodCandidates.begin() << endmsg;
      printTrack( *itL );
    }

    LHCb::Track* fwTra = tr->clone();
    fwTra->clearAncestors();
    fwTra->addToAncestors( tr );  // Set the Velo track as only ancestor of the Forward track
    output.push_back( fwTra );
    fwTra->setType( LHCb::Track::Long );
    fwTra->setHistory( LHCb::Track::PatForward );

    //== Add a new state in the T stations ...

    double qOverP = m_fwdTool->qOverP( *itL );
    // set q/p in all of the existing states
    const std::vector< LHCb::State * > states = fwTra->states();
    std::vector< LHCb::State * >::const_iterator iState;
    for ( iState = states.begin() ; iState != states.end() ; ++iState ){
      (*iState)->setQOverP(qOverP);
      (*iState)->setErrQOverP2(qOverP*qOverP*0.012*0.012);
    }

    Gaudi::TrackSymMatrix cov;
    cov(0,0) = m_stateErrorX2;
    cov(1,1) = m_stateErrorY2;
    cov(2,2) = m_stateErrorTX2;
    cov(3,3) = m_stateErrorTY2;
    double errQOverP = m_stateErrorP*qOverP;
    cov(4,4) = errQOverP * errQOverP;


    for (unsigned int i=0; i<m_fwdTool->zOutputs().size(); i++) {
      LHCb::State temp;
      double dz = m_fwdTool->zOutputs()[i] - m_fwdTool->zReference();
      temp.setLocation( LHCb::State::AtT );
      temp.setState( (*itL).x( dz ), (*itL).y( dz ), m_fwdTool->zOutputs()[i],
                     (*itL).xSlope( dz ), (*itL).ySlope( dz ), qOverP );

      //== overestimated covariance matrix, as input to the Kalman fit


      temp.setCovariance( cov );
      fwTra->addToStates( temp );
    }
    //== New information, on the track fit.
    fwTra->setChi2PerDoF( 0. );//(*itL).chi2PerDoF() );
    fwTra->setNDoF(       (*itL).nDoF() );
    fwTra->addInfo(LHCb::Track::PatQuality, (*itL).quality());
    // added for NNTools
    fwTra->addInfo(LHCb::Track::Cand1stQPat, (*itL).cand1stquality());
    fwTra->addInfo(LHCb::Track::Cand2ndQPat, (*itL).cand2ndquality());

    //== Add reference to the used clusters/, T stations
    for ( PatFwdHits::iterator itH = (*itL).coordBegin(); (*itL).coordEnd() != itH; itH++ ) {
      PatFwdHit* myHit = (*itH);
      fwTra->addToLhcbIDs( myHit->hit()->lhcbID() );
      myHit->hit()->setStatus(Tf::HitBase::UsedByPatForward);
      myHit->setIsUsed(true);
    }
    fwTra -> setPatRecStatus( LHCb::Track::PatRecIDs );


    if ( NULL != m_addTTClusterTool ) {
      StatusCode sc = m_addTTClusterTool->addTTClusters( *fwTra );
      if (sc.isFailure()) {
        if ( isDebug ) info()<<" Failure in adding TT clusters to track"<<endmsg;
      }
    }
  }
  if ( isDebug ) info() << "Finished track" << endmsg;
  return StatusCode::SUCCESS;
}
//=========================================================================
//  Build the list of vector of X hit candidates.
//=========================================================================
void FastForwardTool::buildXCandidatesList ( PatFwdTrackCandidate& track ) {

  bool isDebug = msgLevel( MSG::DEBUG );
  if ( 0 != m_debugTool &&
       0 <= m_wantedKey &&
       track.track()->key() == m_wantedVelo ) isDebug = true;

  m_candidates.clear();

  double xExtrap  = track.xStraight( m_fwdTool->zReference() );
  double minMom = m_minPt / track.sinTrack();
  if ( m_minMomentum > minMom ) minMom = m_minMomentum;
  double maxRange = m_rangePerMeV / minMom;

  if ( 0 != track.qOverP() && !m_withoutBField) {
    if ( isDebug ) info() << "   xExtrap = " << xExtrap
                          << " q/p " << track.qOverP()
                          << " predict " << xExtrap + (m_rangePerMeV * track.qOverP()) << endmsg;
    xExtrap += m_rangePerMeV * track.qOverP();
    maxRange = m_minRange + m_rangeErrorFraction * m_rangePerMeV * fabs( track.qOverP() );
  }

  double minProj  = xExtrap - maxRange;
  double maxProj  = xExtrap + maxRange;

  if ( m_ttOffset < 9000. ) {
    double xOffset = m_ttToTScale * m_ttOffset;
    double tmp =  xExtrap + xOffset - m_ttToTWindow;
    if ( tmp > minProj ) minProj = tmp;
    tmp = xExtrap + xOffset + m_ttToTWindow;
    if ( tmp < maxProj ) maxProj = tmp;
    if ( isDebug ) info() << ".. m_ttOffset " << m_ttOffset << " xExtrap " << xExtrap << " xOffset " << xOffset << endmsg;
  }

  bool inFullOT = fabs( track.slY() ) > 0.050;

  if ( isDebug ) info() << "Search X coordinates, xMin " << minProj
                        << " xMax " << maxProj << endmsg;

  fillXList( track, minProj, maxProj );

  //== Try to handle the 3 independent lists per station.

  PatFwdHits::iterator itH, itH0, itH1, itH2;
  PatFwdHits::iterator itPrev1, itPrev2, last[2];

  if ( isDebug ) {
    info() << "=== All hits from this MCParticle " << endmsg;

    for (unsigned int sta = 0; sta < m_nSta; sta ++){
      for (unsigned int lay = 0; lay< m_nLay; lay++){
        for (unsigned int region = 0; region <m_nReg; region ++) {
          Tf::TStationHitManager<PatForwardHit>::HitRange range = m_tHitManager->hits( sta, lay, region );
          for ( PatFwdHits::const_iterator itH = range.begin(); range.end() != itH; ++itH ) {
            if ( matchKey( *itH ) ) printCoord( *itH );
          }
        }
      }
    }
    info() << "=== Selected X hits for wanted MCParticle " << m_wantedKey << " extrapolation " << xExtrap << " ===" << endmsg;
    for ( int kk = 0 ; 3 > kk ; ++kk ) {
      for ( PatFwdHits::iterator itH = m_xHitsPerStation[kk].begin(); m_xHitsPerStation[kk].end() != itH; ++itH ) {
        if ( matchKey( *itH ) ) printCoord( *itH );
      }
    }
  }

  //== Compute the scale factor for the drift distance
  double zMag = m_fwdTool->zMagnet( track );
  double dzRef = m_fwdTool->zReference() - zMag;
  double scaleT1  = dzRef / (m_zStation[0] - zMag);
  double scaleT2  = dzRef / (m_zStation[1] - zMag);
  double scaleT3  = dzRef / (m_zStation[2] - zMag);

  PatFwdHits lastGood;

  itPrev2 = m_xHitsPerStation[2].begin();
  itPrev1 = m_xHitsPerStation[1].begin();

  for ( itH0 = m_xHitsPerStation[0].begin(); m_xHitsPerStation[0].end() != itH0 ; itH0++ ) {
    bool printDetails = false;
    if ( isDebug && matchKey( *itH0 ) ) printDetails = true;
    if ( printDetails ) {
      info() << "Start         ";
      printCoord( *itH0 );
    }

    last[0] = itH0;
    last[1] = itH0;

    //== cosInv is the correction factor for the drift distance in the projection plane (constant z)
    double zProj2 = dzRef * dzRef;
    double dx = (*itH0)->projection() - track.xStraight( zMag );
    double cosInv = sqrt( (dx*dx + zProj2 ) / zProj2 );

    //== Get a cluster of measurements, taking into account the drift distance. Try right and left solutions form first hit.
    PatFwdHits seeds[2];
    double matchTol = 0.5;
    double maxima   = matchTol;
    int iCase = 1;
    if (  (*itH0)->hit()->type() == Tf::RegionID::OT ) {
      iCase = 2;
      matchTol = m_xMatchTol;
      maxima   = matchTol + m_maxOTDrift;
    }
    double startValue = (*itH0)->projection() - (*itH0)->driftDistance() * cosInv * scaleT1;
    double chi2[2];
    while ( 0 < iCase ) {
      --iCase;
      if ( 0 == iCase ) startValue = (*itH0)->projection() + (*itH0)->driftDistance() * cosInv * scaleT1;
      seeds[iCase].push_back( *itH0 );
      chi2[iCase] = 0.;
      for ( itH = itH0+1; itH < m_xHitsPerStation[0].end(); ++itH ) {
        double drift = (*itH)->driftDistance() * cosInv * scaleT1;
        if ( (*itH)->projection() - drift > startValue + maxima  ) break;
        double current =  (*itH)->projection() - drift;
        if ( fabs( (*itH)->projection() + drift -startValue ) <  fabs( current-startValue ) ) {
          current = (*itH)->projection() + drift;
        }
        if ( fabs( current- startValue ) < matchTol ) {
          seeds[iCase].push_back( *itH );
          chi2[iCase] += (current-startValue)*(current-startValue);
          if ( isDebug && matchKey( *itH ) ) printDetails = true;
          if ( printDetails ) {
            info() << "Added0 iCase " << iCase ;
            printCoord( *itH );
          }
          last[iCase] = itH;
        }
      }
    }

    PatFwdHits hitsIn0;
    int bestCase = 0;
    if ( seeds[0].size() < seeds[1].size() ) bestCase = 1;
    if ( seeds[0].size() == seeds[1].size() && chi2[1] < chi2[0] ) bestCase = 1;

    hitsIn0 = seeds[bestCase];
    startValue = (*itH0)->projection() + (1-2*bestCase) *  (*itH0)->driftDistance() * cosInv * scaleT1;

    if ( printDetails ) info() << "0: size " << seeds[0].size() << " chi2 " << chi2[0]
                               << " 1: size " << seeds[1].size() << " chi2 " << chi2[1]
                               << " best " << bestCase << " startValue " << startValue
                               << endmsg;
    itH0 = last[iCase];  // restart only after this group.

    double projF = 0.;
    double zF    = 0.;
    for ( itH = hitsIn0.begin(); hitsIn0.end() != itH; ++itH ) {
      double drift = (*itH)->driftDistance() * cosInv * scaleT1;
      double dist = (*itH)->projection() + drift - startValue;
      (*itH)->setRlAmb( +1 );
      if ( dist > drift ) {
        dist = dist - 2*drift;
        (*itH)->setRlAmb( -1 );
      }
      projF += dist;
      zF    += (*itH)->z();
    }
    projF /= hitsIn0.size();
    zF    /= hitsIn0.size();
    projF += startValue;

    double spread = ( fabs( projF - xExtrap ) ) * m_maxSpreadSlopeX;
    if ( spread < m_maxSpreadX ) spread = m_maxSpreadX;
    if ( track.slY2() < 225.e-6 && spread < 6. ) spread = 6.;  // low angle -> allow scattering in pipe below 15 mrad

    double minProj  = projF - spread - m_xMatchTol - m_maxOTDrift;

    if ( printDetails ) info() << "For station 2, between " << projF-spread << " and " << projF + spread << endmsg;

    for ( itH2 = itPrev2;  m_xHitsPerStation[2].end() != itH2; ++itH2 ) {
      double drift2 = (*itH2)->driftDistance() * cosInv * scaleT3;
      if ( (*itH2)->projection()+drift2 < minProj ) {
        itPrev2 = itH2;
        continue;
      }
      if ( (*itH2)->projection()+drift2 < projF - spread ) continue;
      if ( (*itH2)->projection()-drift2 > projF + spread ) break;
      if ( printDetails ) {
        info() << "  end2        ";
        printCoord( *itH2 );
      }

      seeds[0].clear();
      seeds[1].clear();
      last[0] = itH2;
      last[1] = itH2;
      iCase = 1;
      double matchTol = 0.5;
      double maxima   = 1.;
      if (  (*itH2)->hit()->type() == Tf::RegionID::OT ) {
        iCase = 2;
        matchTol = m_xMatchTol;
        maxima   = matchTol + m_maxOTDrift;
      }

      double startValue = (*itH2)->projection() - drift2;
      while ( 0 < iCase ) {
        --iCase;
        if ( 0 == iCase ) startValue = (*itH2)->projection() + drift2;
        seeds[iCase].push_back( *itH2 );
        chi2[iCase] = 0.;
        for ( itH = itH2+1; itH < m_xHitsPerStation[2].end(); ++itH ) {
          double drift = (*itH)->driftDistance() * cosInv * scaleT3;
          if ( (*itH)->projection() - drift > startValue + maxima  ) break;
          double current =  (*itH)->projection() - drift;
          if ( fabs( (*itH)->projection() + drift - startValue ) <  fabs( current-startValue ) ) {
            current = (*itH)->projection() + drift;
          }
          if ( fabs( current- startValue ) < matchTol ) {
            seeds[iCase].push_back( *itH );
            chi2[iCase] += (current-startValue)*(current-startValue);
            last[iCase] = itH;
            if ( isDebug && matchKey( *itH ) ) printDetails = true;
            if ( printDetails ) {
              info() << "Added2 iCase " << iCase << format( " dist %7.3f ", current-startValue );
              printCoord( *itH );
            }
          }
        }
      }

      PatFwdHits hitsIn2;
      bestCase = 0;
      if ( seeds[0].size() < seeds[1].size() ) bestCase = 1;
      if ( seeds[0].size() == seeds[1].size() && chi2[1] < chi2[0] ) bestCase = 1;
      startValue = (*itH2)->projection() + (1-2*bestCase) * drift2;
      hitsIn2 = seeds[bestCase];
      itH2    = last[bestCase];

      int nHits = hitsIn0.size() + hitsIn2.size();
      if ( nHits < 3 ) continue;

      //== In full OT acceptance, ask for at least 4 hits in T1 + T3.
      if ( inFullOT && nHits < 4 ) continue;
      if ( fabs(track.slY() ) > 0.010 && fabs( projF ) > 1000. && nHits < 4 ) continue;

      for ( itH = hitsIn0.begin(); hitsIn0.end() != itH ; ++itH ) (*itH)->setSelected( true );

      PatFwdHits temp( hitsIn0 );
      double projL = 0.;
      double zL    = 0.;
      for ( itH = hitsIn2.begin(); hitsIn2.end() != itH; ++itH ) {
        double drift = (*itH)->driftDistance() * cosInv * scaleT3;
        double dist = (*itH)->projection() + drift - startValue;
        (*itH)->setSelected( true );
        (*itH)->setRlAmb( +1 );
        if ( dist > drift ) {
          dist = dist - 2*drift;
          (*itH)->setRlAmb( -1 );
        }
        projL += dist;
        zL    += (*itH)->z();
        temp.push_back( *itH );
      }
      projL /= hitsIn2.size();
      zL    /= hitsIn2.size();
      projL += startValue;

      PatFwdPlaneCounter cntr( temp.begin(), temp.end() );
      if ( m_minXPlanes-2 > cntr.nbDifferent() ) {
        if ( printDetails ) {
          info() << " ---- too few planes  : " << cntr.nbDifferent() << " -- " << endmsg;
          for ( itH = temp.begin(); temp.end() != itH ; ++itH ) {
            printCoord( *itH );
          }
        }
        continue;
      }
      
      double projSlope = ( projL - projF) / ( zL - zF );
      double xCompat1 = 1.5 + 400 * fabs( projSlope );
      if ( printDetails ) info() << "ProjF " << projF << " projL " << projL
                                 << " xcompat1 = " << xCompat1
                                 << " ndif " << cntr.nbDifferent()
                                 << endmsg;
      
      for ( itH1 = itPrev1; m_xHitsPerStation[1].end() != itH1; ++itH1 ) {
        double pred = projF + projSlope * ( (*itH1)->z() - zF );
        if ( fabs( (*itH1)->projection() - pred ) - (*itH1)->driftDistance()*cosInv*scaleT2 > xCompat1  ) {
          if ( (*itH1)->projection() > pred + xCompat1 + m_maxOTDrift ) break;
          if ( (*itH1)->projection() < minProj ) itPrev1 = itH1;
          continue;
        }
        (*itH1)->setRlAmb( 0 );   // Do not preset it.
        (*itH1)->setSelected( true );
        temp.push_back( *itH1 );
        cntr.addHit( *itH1 );
        if ( printDetails ) {
          info() << "Added1  ndif " << cntr.nbDifferent();
          printCoord( *itH1 );
        }
      }
      
      if ( m_minXPlanes > cntr.nbDifferent() ) {
        if ( printDetails ) info() << " ---- too few planes: " << cntr.nbDifferent() << endmsg;
        continue;
      }

      if ( inFullOT && temp.size() < 7 ) {
        if ( printDetails ) info() << "Track fully in OT, lees than 7 hits. Ignore" << endmsg;
        continue;
      }

      PatFwdTrackCandidate aCandidate( track.track(), temp );
      bool store = true;
      if ( temp == lastGood ) {
        store = false;
        if ( isDebug ) info() << "=== Identical to the previous one. skip ===" << endmsg;
      }
      if ( store ) {
        lastGood = temp;
        if ( !fitXCandidate( aCandidate, m_maxChi2, m_minXPlanes, isDebug ) ) {
          if ( isDebug ) {
            info() << "  --- fit failed..." << endmsg;
          }
          store = false;
        } else {
          aCandidate.cleanCoords();
          m_fwdTool->chi2PerDoF( aCandidate );  // Compute and store chi2/dof
        }
      }
      if ( store ) {
        m_candidates.push_back( aCandidate );
        if ( isDebug ) {
          info() << "=== Store candidate : chi2 " << aCandidate.chi2PerDoF() << endmsg;
          printTrack( aCandidate );
        }
      }
    }
  }

  if ( isDebug ) {
    info() << endmsg << "$$$ Final list of X candidates has " << m_candidates.size() << " entries" << endmsg;
    for (   std::vector<PatFwdTrackCandidate>::iterator itC = m_candidates.begin();
            m_candidates.end() != itC; ++itC ) {
      info() << "--- candidate " << itC - m_candidates.begin() << endmsg;
      printTrack( *itC );
    }
    info() << endmsg;
  }
}
//=========================================================================
//  Fill the vector of hit pointer, sorted by projection.
//=========================================================================
void FastForwardTool::fillXList ( PatFwdTrackCandidate& track, double xMin, double xMax ) {

  PatFwdHits::const_iterator itFwdH;
  m_xHitsPerStation.clear();

  for (unsigned int sta = 0; sta < m_nSta; sta ++){
    PatFwdHits inStation;
    inStation.reserve(1000);
    for (unsigned int lay = 0; lay< m_nLay; lay += 3){
      for (unsigned int region = 0; region <m_nReg; region ++){
        const Tf::EnvelopeBase* regionB = m_tHitManager->region(sta,lay,region);

        double yCompat = 10 * m_errY;
        double yRegion = track.yStraight( regionB->z() );
        double xHitMin = xMin * regionB->z() / m_fwdTool->zReference() - fabs( yRegion * regionB->sinT() ) - 20.;
        double ty      = track.slY();
        double y0      = track.yStraight( 0. );
        if ( fabs(yRegion) > 1000. ) yCompat = fabs( yRegion ) - 500.;  // Outside enough -> no Y selection.

        double minY =  yRegion-yCompat;
        double maxY =  yRegion+yCompat;

        if ( track.track()->key() == m_wantedVelo ) {
          info() << format( "Station%2d Layer%2d region%2d yRegion %8.2f yCompat %8.2f min %8.2f max %8.2f xHitMin%9.2f",
                            sta, lay, region, yRegion, yCompat, regionB->ymin(),regionB->ymax(), xHitMin );
          if ( maxY < regionB->ymin() || minY > regionB->ymax() ) info() << " *** OUTSIDE ***";
          info() << endmsg;
        }
        if ( maxY < regionB->ymin() || minY > regionB->ymax() ) continue;

        Tf::TStationHitManager<PatForwardHit>::HitRange range = m_tHitManager->hitsWithMinX(xHitMin, sta, lay, region);

        for ( PatFwdHits::const_iterator itH = range.begin(); range.end() != itH; ++itH ) {

          PatFwdHit* hit = *itH;
          if ( maxY < hit->hit()->yMin() || minY > hit->hit()->yMax() ) continue;
          updateHitForTrack( hit, y0, ty );

          double xRef = m_fwdTool->fastXAtReference( track, hit );
          hit->setProjection( xRef );

          if ( xMin > xRef ) continue;
          if ( xMax < xRef ) break;

          if (hit->hit()->type() == Tf::RegionID::OT) {
            if ( m_maxOTDrift < hit->driftDistance() ||
                 m_minOTDrift > hit->driftDistance() ) {
              continue;
            }
          }
          hit->setSelected( true );
          inStation.push_back( hit );
        }
      }
    }
    std::sort( inStation.begin(), inStation.end(), Tf::increasingByProjection<PatForwardHit>() );
    m_xHitsPerStation.push_back( inStation );
  }
}

//=========================================================================
//  Fit the X projection of a candidate
//=========================================================================
bool FastForwardTool::fitXCandidate ( PatFwdTrackCandidate& track,
                                      double maxChi2, int minPlanes, bool isDebug ) {

  double highestChi2 = 1.e10;
  PatFwdHits::iterator itH;
  PatFwdHit* centre = *(track.coordBegin() + track.coords().size()/2 );
  for ( itH = track.coordBegin(); track.coordEnd() != itH ; ++itH ) {
    (*itH)->setSelected( true );
    //(*itH)->setRlAmb( 0 );
    if ( (*itH)->driftDistance() < centre->driftDistance() ) centre = *itH;
  }
  PatFwdPlaneCounter planeCount( track.coordBegin(), track.coordEnd() );

  m_fwdTool->xAtReferencePlane( track, centre, true );

  while ( maxChi2 < highestChi2 && minPlanes <= planeCount.nbDifferent() ) {
    if (!m_fwdTool->fitXProjection( track, track.coordBegin(), track.coordEnd(), true )) {
      if ( isDebug ) info() << "Abandon: Matrix not positive definite." << endmsg;
      return false;
    }
    if ( isDebug ) {
      info() << "-- Fit X projection, current status before removing worst: " << endmsg;
      printTrack( track );
    }
    highestChi2 = 0;
    PatFwdHit* worst = *track.coordBegin();
    for ( itH = track.coordBegin(); track.coordEnd() != itH ; ++itH ) {
      PatFwdHit* hit = *itH;
      if ( !hit->isSelected() ) continue;
      double chi2 = m_fwdTool->chi2Hit( track, hit );
      if ( highestChi2 < chi2 ) {
        highestChi2 = chi2;
        worst = hit;
      }
    }

    if ( maxChi2 < highestChi2 ) {
      planeCount.removeHit( worst );
      worst->setSelected( false );
      if ( isDebug ) info() << "--- Remove worst and retry, plane count = " << planeCount.nbDifferent() << endmsg;
    }
  }
  return minPlanes <= planeCount.nbDifferent();
}

//=========================================================================
//  Return if the two candidates share 70% of the smallest number of hits.
//  Hits are supposed to be sorted by LHCBID
//=========================================================================
bool FastForwardTool::isClone ( PatFwdTrackCandidate& one, PatFwdTrackCandidate& two ) {
  int nbCommon = 0;
  PatFwdHits::iterator itH1 = one.coordBegin();
  PatFwdHits::iterator itH2 = two.coordBegin();
  while ( itH1 != one.coordEnd() && itH2 != two.coordEnd() ) {
    if ( (*itH1)->hit()->lhcbID() < (*itH2)->hit()->lhcbID() ) {
      ++itH1;
    } else if ( (*itH2)->hit()->lhcbID() < (*itH1)->hit()->lhcbID() ) {
      ++itH2;
    } else {
      ++itH1;
      ++itH2;
      ++nbCommon;
    }
  }
  int minSize = one.coordEnd() - one.coordBegin();
  int twoSize = two.coordEnd() - two.coordBegin();
  if ( twoSize < minSize ) minSize = twoSize;
  if ( nbCommon > 0.7 * nbCommon ) return true;
  return false;
}

//=========================================================================
//  Return the best TT offset for this track.
//=========================================================================
double FastForwardTool::getTTOffset ( PatFwdTrackCandidate& track, bool isDebug ) {

  double offset = 10000.;
  LHCb::State state = track.track()->closestState( m_zTTProj );
  PatTTHits ttHits;
  double yTol   = 5. * sqrt( state.errY2() + m_zTTProj * m_zTTProj * state.errTy2() );
  Tf::TTStationHitManager<PatTTHit>::HitRange hits = m_ttHitManager->hits();

  // -- Loop over all the TT hits, get all TT hits whose projection is compatible with the track
  for ( PatTTHits::const_iterator itTT = hits.begin(); hits.end() != itTT; ++itTT ) {
    PatTTHit* tt = *itTT;
    double z = tt->z();
    double yPred = state.y() + ( z - state.z() ) * state.ty();

    // -- Check if hit is compatible in y with the track
    if( !tt->hit()->isYCompatible( yPred, yTol) ) continue ;
    double tyTr = state.ty();
    updateTTHitForTrack( tt, state.y()-state.z()*state.ty(), tyTr );
 
    // -- Calculate the projection, which takes possible multiple scattering into account
    double xPred    = state.x() + ( z-state.z() ) * state.tx();
    double projDist = ( tt->x() - xPred ) * ( m_zTTProj - m_zTTField ) / ( z - m_zTTField );
    tt->setProjection( projDist );
    if ( fabs( projDist ) < m_maxTTProj ) ttHits.push_back( tt );
  }

  std::sort( ttHits.begin(), ttHits.end(), Tf::increasingByProjection<PatTTHit>() );

  double tolMatch = 0.8;
  double rangeSize = 0.;
  if ( ttHits.size() < 4 ) return 10000.;
  PatTTHits::iterator itF, itL;
  for ( itF = ttHits.begin(); itF != ttHits.end() - 3 ;  ++itF ) {
    itL = itF;
    int nbPlanes = 0;
    std::array<int, 4> firedPlanes;
    firedPlanes.fill(0); 
    while ( itL < ttHits.end() &&
            (*itL)->projection() <  (*itF)->projection() + tolMatch
            && nbPlanes < 4 ) {
      if ( 0 == firedPlanes[ (*itL)->planeCode() ] ) nbPlanes++;
      firedPlanes[ (*itL)->planeCode() ] += 1; // -- Count number of fired planes
      itL++;
    }
    if ( 4 == nbPlanes ) {
      double point = .5 * ( (*(itL-1))->projection() + (*itF)->projection() );
      if ( fabs(point) < fabs( offset ) ) offset = point;
      rangeSize  = (*(itL-1))->projection() - (*itF)->projection();
    }
  }
  if ( isDebug ) {
    if ( offset < 9000. ) {
      info() << "=== Found range at " << offset << " of size " << rangeSize << endmsg;
    } else {
      info() << "=== No good range found" << endmsg;
    }
    for ( PatTTHits::const_iterator itH = ttHits.begin(); ttHits.end() != itH; ++itH ) {
      printTTHit( *itH );
    }
  }
  return offset;
}
//=========================================================================
//  Fill the vector of hit pointer, sorted by projection.
//=========================================================================
bool FastForwardTool::fillStereoList ( PatFwdTrackCandidate& track, double tol ) {

  PatFwdHits temp;
  PatFwdHits::iterator itH;

  PatFwdRegionCounter regions( track.coordBegin(), track.coordEnd() );

  bool isDebug = msgLevel( MSG::DEBUG );

  for (unsigned int sta=0; sta<m_nSta; sta++) {
    for (unsigned int lay=1; lay<m_nLay-1; lay++) {
      for (unsigned int region=0; region<m_nReg; region++) {

        const Tf::EnvelopeBase* regionB = m_tHitManager->region(sta,lay,region);
        double dz = regionB->z() - m_fwdTool->zReference();
        double yRegion = track.y( dz );

        double tolY = m_yCompatibleTol;
        if ( regions.nbInRegion( region ) == 0 &&
             ( yRegion+tolY < regionB->ymin() ||
               yRegion-tolY > regionB->ymax() ) ) continue;

        double xPred   = track.x( dz );

        //== Correct for stereo
        double xHitMin = xPred - fabs( yRegion * regionB->sinT() ) - 40. - tol;

        //== get position and slope at z=0 from track at zReference (0 for y/ySlope functions)
        double ty = track.ySlope( 0. );
        double y0 = track.y( 0. )  -  m_fwdTool->zReference()  * ty;  // Extrapolate from back...

        //== Project in Y, in fact in X but oriented, such that a displacement in Y is a
        //== displacement also in this projectio.
        double sign = -1.;
        if ( regionB->sinT() >0 ) sign = +1;

        double minProj = tol;
        if ( region < m_nOTReg ) minProj += 2.5;

        Tf::TStationHitManager<PatForwardHit>::HitRange range = m_tHitManager->hitsWithMinX(xHitMin, sta, lay, region);

        for ( PatFwdHits::const_iterator itH = range.begin(); range.end() != itH; ++itH ) {
          PatFwdHit* hit = *itH;

          if ( regions.nbInRegion( region ) == 0 &&
               !hit->hit()->isYCompatible( (float)yRegion, (float)tolY ) ) continue;

          updateHitForTrack( hit, y0, ty );

          hit->setSelected( true );
          if (hit->hit()->type() == Tf::RegionID::OT) {
            if ( m_maxOTDrift < hit->driftDistance() ||
                 m_minOTDrift > hit->driftDistance()  ) {
              hit->setSelected( false );
              continue;
            }
          }
          double xRef = ( hit->x() - xPred ) * sign;
          hit->setProjection( xRef );

          if ( track.track()->key() == m_wantedVelo && matchKey( hit ) ) {
            isDebug = true;
            if ( minProj > fabs(xRef) ) {
              info() << "+++";
            } else {
              info() << "out";
            }
            printCoord( hit );
          }

          if ( -minProj > xRef * sign ) continue;
          if (  minProj < xRef * sign ) break;

          temp.push_back( hit );
        }
      }
    }
  }

  //== Sort by projection

  std::sort( temp.begin(), temp.end(), Tf::increasingByProjection<PatForwardHit>() );

  int minYPlanes = 4;

  if ( isDebug ) {
    info() << "*** Stereo List size = " << temp.size() << endmsg;
    for ( itH = temp.begin(); temp.end() != itH; ++itH ) {
      printCoord( *itH );
    }
  }
  if ( minYPlanes > (int)temp.size() ) return false;

  //== Get a list of hits compatible together, using also the drift distance.

  std::array<int, 12> nbInPlane;
  nbInPlane.fill(0);
  double sum = 0.;
  double sw  = 0.;
  for ( itH = temp.begin(); temp.end() > itH; ++itH ) {
    sum += (*itH)->hit()->weight() * (*itH)->projection()/(*itH)->z();
    sw  += (*itH)->hit()->weight();
    nbInPlane[(*itH)->planeCode()] += 1;
  }
  double meanProj = sum / sw;
  bool mustFilter = true;
  while ( mustFilter ) {
    if ( isDebug ) info() << "Central slope at " << meanProj << endmsg;
    mustFilter = false;
    PatFwdHits::iterator worst = temp.begin();
    double maxDist = 0.;
    sum = 0.;
    sw  = 0.;
    //== Get the farest hit, remove, until all inside allowed window.
    for ( itH = temp.begin(); temp.end() > itH; ++itH ) {
      double dist = (*itH)->projection() - meanProj * (*itH)->z();
      if ( (*itH)->hit()->type() == Tf::RegionID::OT ) {
        if ( dist < 0. ) {
          dist += (*itH)->driftDistance();
        } else {
          dist -= (*itH)->driftDistance();
        }
      }
      sum += (*itH)->hit()->weight() * dist / (*itH)->z();
      sw  += (*itH)->hit()->weight();
      if ( isDebug ) {
        info() << format( "Dist %6.2f ", dist );
        printCoord( *itH );
      }
      //== Keep the worst if more than 2 hits in the plane, or if really far
      if ( fabs(dist) > maxDist &&
           ( nbInPlane[ (*itH)->planeCode() ] > 2 || fabs(dist) > 4. ) ) {
        maxDist = fabs(dist);
        worst   = itH;
      }
    }
    meanProj += sum / sw;
    if ( maxDist > 1.5 ) {
      if ( isDebug ) {
        info() << format( "MaxD = %7.2f ", maxDist );
        printCoord( *worst );
      }
      nbInPlane[ (*worst)->planeCode() ] -= 1;
      temp.erase( worst );
      if ( minYPlanes > (int)temp.size() ) return false;
      mustFilter = true;
    }
  }

  if ( minYPlanes > (int)temp.size() ) return false;

  //== Set a good starting point for the fit...
  double dy =  meanProj * m_fwdTool->zReference() / fabs( temp.front()->hit()->sinT() );
  track.updateParameters( 0., 0., 0., 0., dy, 0. );
  m_fwdTool->updateHitsForTrack( track, temp.begin(), temp.end() );

  for ( itH = temp.begin(); temp.end() != itH; ++itH ) {
    track.addCoord( *itH );
  }

  //== Sort by Z
  std::sort( track.coordBegin(), track.coordEnd(),
	  Tf::increasingByZ<PatForwardHit>() );

  return true;
}
//=========================================================================
//  Debug one TT hit, with MC truth if the tool is defined
//=========================================================================
void FastForwardTool::printTTHit( const PatTTHit* hit ) {
  info() << format( " Z %10.2f Xp %10.2f X%10.2f  St%2d lay%2d reg%2d planeCode%2d typ%2d   ",
                    hit->z(),
                    hit->projection(),
                    hit->x(),
                    hit->hit()->station(),
                    hit->hit()->layer(),
                    hit->hit()->region(),
                    hit->planeCode(),
                    hit->hit()->type() );
  if ( 0 != m_debugTool ) {
    LHCb::LHCbID myId =  hit->hit()->lhcbID();
    m_debugTool->printKey( info(), myId );
    if ( m_debugTool->matchKey( myId, m_wantedKey ) ) info() << " ***";
  }
  info() << endmsg;
}
//=========================================================================
//  Debug one hit, with MC truth if the tool is defined
//=========================================================================
void FastForwardTool::printCoord( const PatForwardHit* hit ) {
  info() << format( " Z %10.2f Xp %10.2f X%10.2f  St%2d lay%2d reg%2d typ%2d Pr%2d Nxt%2d RL %2d Drift %7.3f   ",
                    hit->z(),
                    hit->projection(),
                    hit->x(),
                    hit->hit()->station(),
                    hit->hit()->layer(),
                    hit->hit()->region(),
                    hit->hit()->type(),
                    hit->hasPrevious(),
                    hit->hasNext(),
                    hit->rlAmb(),
                    hit->driftDistance() );
  if ( 0 != m_debugTool ) {
    LHCb::LHCbID myId =  hit->hit()->lhcbID();
    m_debugTool->printKey( info(), myId );
    if ( m_debugTool->matchKey( myId, m_wantedKey ) ) info() << " ***";
  }
  info() << endmsg;
}
//=========================================================================
//  Debug one track, i.e. all hits of the track.
//=========================================================================
void FastForwardTool::printTrack ( PatFwdTrackCandidate& track  ) {
  PatFwdHits::iterator itP;
  for ( itP = track.coordBegin(); track.coordEnd() != itP; ++itP ) {
    if ( !(*itP)->isSelected() ) continue;
    if ( track.fitted() ) info() << format( "Chi2%7.2f Dist%7.2f x%8.2f ",
                                            m_fwdTool->chi2Hit( track, *itP),
                                            m_fwdTool->distanceHitToTrack( track, *itP ),
                                            m_fwdTool->xForHit( track, *itP) );
    printCoord( *itP );
  }
}
//=============================================================================
