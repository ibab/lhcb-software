
// Include files

// from boost
#include <boost/assign/list_of.hpp>
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/SystemOfUnits.h"

#include "TfKernel/HitExtension.h"
#include "TfKernel/RecoFuncs.h"
#include "TfKernel/RegionID.h"

#include "OTDet/DeOTDetector.h"

// local
#include "PatForwardTool.h"
#include "PatFwdPlaneCounter.h"
#include "PatFwdRegionCounter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PatForwardTool
//
// 2005-10-04 : Olivier Callot
//-----------------------------------------------------------------------------


DECLARE_TOOL_FACTORY( PatForwardTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatForwardTool::PatForwardTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
    : GaudiTool ( type, name , parent ),
      m_nnSwitch(false)
{
  declareInterface<IPatForwardTool>(this);
  declareInterface<ITracksFromTrack>(this);

  declareProperty( "SecondLoop"            , m_secondLoop            = true );
  declareProperty( "ZAfterVelo"            , m_zAfterVelo            = 1640. * Gaudi::Units::mm );
  declareProperty( "YCompatibleTol"        , m_yCompatibleTol        =   10. * Gaudi::Units::mm );
  declareProperty( "YCompatibleTolFinal"   , m_yCompatibleTolFinal   =    1. * Gaudi::Units::mm );
  declareProperty( "MinXPlanes"            , m_minXPlanes            =   5   );
  declareProperty( "MinPlanes"             , m_minPlanes             =   9   );
  declareProperty( "MinOTDrift"            , m_minOTDrift            = -0.3 * Gaudi::Units::mm  );
  declareProperty( "MaxOTDrift"            , m_maxOTDrift            =  2.5 * Gaudi::Units::mm  );
  declareProperty( "MaxSpreadX"            , m_maxSpreadX            =  0.6  );
  declareProperty( "MaxSpreadSlopeX"       , m_maxSpreadSlopeX       = .011  );
  declareProperty( "MaxSpreadY"            , m_maxSpreadY            =  1.5  );
  declareProperty( "MaxSpreadSlopeY"       , m_maxSpreadSlopeY       = 70.   );
  declareProperty( "MinPt"                 , m_minPt                 = 80. * Gaudi::Units::MeV );
  declareProperty( "MinMomentum"           , m_minMomentum           =  1. * Gaudi::Units::GeV );
  declareProperty( "MaxChi2"               , m_maxChi2               =  40       );
  declareProperty( "MaxChi2Track"          , m_maxChi2Track          =  40       );
  declareProperty( "MinHits"               , m_minHits               =  12       );
  declareProperty( "MinOTHits"             , m_minOTHits             =  14       );
  declareProperty( "CenterOTYSize"         , m_centerOTYSize         =  100 * Gaudi::Units::mm );
  declareProperty( "MaxDeltaY"             , m_maxDeltaY             =  30.      );
  declareProperty( "MaxDeltaYSlope"        , m_maxDeltaYSlope        =  300.      );
  declareProperty( "MaxXCandidateSize"     , m_maxXCandidateSize     =  50       );

  std::vector<double> tmp = boost::assign::list_of(1255 * Gaudi::Units::MeV)(175 * Gaudi::Units::MeV);
  declareProperty( "MagnetKickParams"      , m_magnetKickParams      =  tmp);

  declareProperty( "UseMomentumEstimate", m_useMomentumEstimate = false );
  declareProperty( "MomentumEstimateError"    , m_momentumEstimateError    =  0.5 );
  declareProperty( "MinRange"              , m_minRange              =   300. * Gaudi::Units::mm  );

  declareProperty("StateErrorX2",  m_stateErrorX2  =   4.0);
  declareProperty("StateErrorY2",  m_stateErrorY2  = 400.);
  declareProperty("StateErrorTX2", m_stateErrorTX2 =  6.e-5);
  declareProperty("StateErrorTY2", m_stateErrorTY2 =  1.e-4);
  declareProperty("StateErrorP",   m_stateErrorP   =  0.15);

  declareProperty("AddTTClusterName", m_addTtToolName = "" );
  declareProperty("AddUTClusterName", m_addUtToolName = "" );
  declareProperty("UseUT",            m_useUT = false );

  declareProperty( "WithoutBField"         , m_withoutBField         = false);

}
//=============================================================================
// Destructor
//=============================================================================
PatForwardTool::~PatForwardTool() {}

//=========================================================================
//  Initialization, get the tools
//=========================================================================
StatusCode PatForwardTool::initialize ( ) {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialize" << endmsg;


  m_tHitManager  = tool<Tf::TStationHitManager<PatForwardHit> >("PatTStationHitManager");
  m_fwdTool      = tool<PatFwdTool>( "PatFwdTool", this);

  if ( "" != m_addTtToolName && (! m_useUT) ) {
    m_addTTClusterTool = tool<IAddTTClusterTool>( m_addTtToolName, this );
  } else {
    m_addTTClusterTool = NULL;
  }

  if ( "" != m_addUtToolName && m_useUT ) {
    m_addUTClusterTool = tool<IAddUTClusterTool>( m_addUtToolName, this );
  } else {
    m_addUTClusterTool = NULL;
  }

  return StatusCode::SUCCESS;
}

//=========================================================================
//  Main execution: Extend a track.
//=========================================================================
void PatForwardTool::forwardTrack( const LHCb::Track* tr, LHCb::Tracks* output ) {

  std::vector<LHCb::Track*> outvec;
  tracksFromTrack(*tr,outvec).ignore();

  if (outvec.size()==0 && m_secondLoop){
      
      int minXPlanes = m_minXPlanes;
      int minPlanes = m_minPlanes;
      
      m_minXPlanes = minXPlanes-1;
      m_minPlanes = minPlanes-1;
      
      tracksFromTrack(*tr,outvec).ignore();

      m_minXPlanes = minXPlanes;
      m_minPlanes = minPlanes;
      
  }

  for (unsigned int i=0; i<outvec.size(); i++)
    output->insert(outvec[i]);

}

StatusCode PatForwardTool::tracksFromTrack( const LHCb::Track& seed,
                                            std::vector<LHCb::Track*>& output ){


  LHCb::Track* tr = (LHCb::Track*) &seed;

  bool isDebug = msgLevel( MSG::DEBUG );

  if ( tr->checkFlag( LHCb::Track::Invalid  ) ) return StatusCode::SUCCESS;
  if ( tr->checkFlag( LHCb::Track::Backward ) ) return StatusCode::SUCCESS;

  PatFwdTrackCandidate track( tr );

  if ( isDebug ) {
    debug() << format( "**** Velo track %3d  x%8.2f  y%8.2f  tx%9.5f  ty%9.5f q/p = %8.6f",
                       tr->key(), track.xStraight( m_zAfterVelo ),
                       track.yStraight( m_zAfterVelo ),
                       track.slX(), track.slY(), 1000. * track.qOverP() ) << endmsg;
  }



  //== Build the initial list of X candidates


  buildXCandidatesList( track );

  int minPlanes = m_minPlanes;  //== Initial value, can be updated later...

  std::vector<PatFwdTrackCandidate> xCandidates;
  std::vector<PatFwdTrackCandidate>::iterator itL;

  int minOTX = int( 1.5 * m_minXPlanes );

  for( itL = m_candidates.begin(); m_candidates.end() != itL; ++itL ) {

    //== Fit the candidate, and store them

    while ( m_fwdTool->fitXCandidate( *itL, m_maxChi2, m_minXPlanes ) ) {

      // Copy the whole stuff, keep only used ones and store it.
      PatFwdTrackCandidate temp = *itL;
      temp.cleanCoords();
      m_fwdTool->chi2PerDoF( temp );  // Compute and store chi2/dof
      if ( isDebug ) {
        debug() << "Chi2/nDof = " << temp.chi2PerDoF() << " nDoF " << temp.nDoF()
                << " dist at center " << m_fwdTool->distAtMagnetCenter( temp )
                << endmsg;
        debugFwdHits( temp );
      }


      //== Check the chi2 with magnet center constraint.

      if ( m_maxChi2Track > temp.chi2PerDoF()  &&
           (!m_withoutBField || fabs(temp.slX()-temp.xSlope(0))<0.005)){
        //== Count how many weighted hits
        PatFwdRegionCounter regions( temp.coordBegin(), temp.coordEnd() );
        int nbHit = regions.nbOT() + 2*regions.nbIT();
        bool inCenter = m_centerOTYSize > fabs( temp.y( 0. ) );

        if ( minOTX <= nbHit || inCenter ) {
          
          const double momentum=1.0/fabs(m_fwdTool->qOverP( temp ));
          const double pt = track.sinTrack()*momentum;

          //== reject if below threshold
          if (m_withoutBField || (momentum>m_minMomentum && pt>m_minPt) ) xCandidates.push_back( temp );
          if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
            debug() << "+++ Store candidate " << xCandidates.size()-1 << endmsg;
        } else {
          if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
            debug() << " --- not enough hits " << nbHit << endmsg;
        }
      }

      //== tag these coordinates in the original, so that we don't find the same track again
      for ( PatFwdHits::iterator itH = (*itL).coordBegin(); (*itL).coordEnd() != itH; ++itH ) {
        if ( (*itH)->isSelected() ) {
          (*itH)->setIgnored( true );
        }
      }
    }
  }

  if ( isDebug ) {
    debug() << "************ List of X candidates , N = " << xCandidates.size() << endmsg;
    for ( itL = xCandidates.begin(); xCandidates.end() != itL; ++itL ) {
      debug() << "Candidate " << itL - xCandidates.begin()
              << " Chi2/nDof = " << (*itL).chi2PerDoF() << endmsg;
      debugFwdHits( *itL );
    }
    if ( xCandidates.size() > 0 ) debug() << "---- Now get the stereo hits on these ---" << endmsg;
  }

  //== Now try to get space track from these X track.


  std::vector<PatFwdTrackCandidate> goodCandidates;
  int maxPlanes = 0;


  for ( itL = xCandidates.begin(); xCandidates.end() != itL; ++itL ) {
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "--- Candidate " << itL - xCandidates.begin()
              << "  X cord size " << (*itL).coordEnd() - (*itL).coordBegin()
              << endmsg;

    PatFwdHits::iterator itH;
    for ( itH = (*itL).coordBegin(); (*itL).coordEnd() != itH ; ++itH ) {
      (*itH)->setIgnored( false);  // restore normal flag.
    }
    PatFwdTrackCandidate& temp = *itL;
    temp.setSelectedCoords( );

    double qOverP = 1000. * m_fwdTool->qOverP( temp );  // in 1/GeV

    if (m_withoutBField) {
      if (m_minMomentum !=0)
        qOverP = 1/m_minMomentum;
      else
        qOverP = 1;
    }
    
    double tol = m_maxSpreadY + m_maxSpreadSlopeY * qOverP *  qOverP;

    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "Adding stereo coordinates, tol = " << tol << endmsg;

    if ( !fillStereoList( temp, tol ) ) continue; // Get the stereo coordinates

    if ( isDebug )  debugFwdHits( temp );
    temp.setSelectedCoords( );

    //== Fit and reject if not good enough

    if ( !m_fwdTool->fitStereoCandidate( temp, m_maxChi2, minPlanes ) ) continue;

    temp.cleanCoords();
    m_fwdTool->chi2PerDoF( temp );  //== compute and store Chi2PerDoF
    if ( isDebug ) {
      debug() << "  ... fit OK  chi2 " << temp.chi2PerDoF() << " nDoF " << temp.nDoF()
              << " dist at center " << m_fwdTool->distAtMagnetCenter( temp )
              << endmsg;
      debugFwdHits( temp );
    }

    //== Remove stereo coordinates incompatible in Y

    double yTol = m_yCompatibleTolFinal;
    if ( !m_fwdTool->removeYIncompatible( temp, yTol, minPlanes ) ) continue;
    temp.cleanCoords();
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "  ... Y is compatible" << endmsg;

    double quality = 0.;

    // Enough stereo planes
    PatFwdPlaneCounter fullCount( temp.coordBegin(), temp.coordEnd() );
    int nbY = fullCount.nbStereo();
    if ( 4 > nbY ) {
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << "Not enough Y planes : " << nbY << endmsg;
      continue;
    }

    if ( m_maxDeltaY + m_maxDeltaYSlope * qOverP *qOverP < fabs(  m_fwdTool->changeInY( temp ) ))  {
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << "  --- Too big change in Y : " <<  m_fwdTool->changeInY( temp ) << endmsg;
      continue;
    }

    quality  = 5. * fabs(  m_fwdTool->changeInY( temp ) ) / ( m_maxDeltaY + qOverP * qOverP * m_maxDeltaYSlope );
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
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
          debug() << "  --- Not enough hits : " << nbHit << endmsg;
        continue;
      }
      if ( temp.nbIT() == 0 && temp.nbOT() < m_minOTHits ) {
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
          debug() << " Too few OT for OT only track : " << temp.nbOT() << endmsg;
        continue;
      }
    }

    int nbPlanes = fullCount.nbDifferent();
    if ( maxPlanes < nbPlanes ) maxPlanes = nbPlanes;

    //== reject if below threshold
    const double momentum=1.0/fabs(m_fwdTool->qOverP( temp ));
    const double pt = track.sinTrack()*momentum;
    if ( !m_withoutBField && ((momentum<m_minMomentum) || (pt<m_minPt)) ) continue;
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

    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "Require enough planes : " << minPlanes << endmsg;
    std::vector<PatFwdTrackCandidate> tempCandidates( goodCandidates );
    goodCandidates.clear();
    for ( itL = tempCandidates.begin(); tempCandidates.end() != itL; ++itL ) {
      PatFwdPlaneCounter tmp( (*itL).coordBegin(), (*itL).coordEnd() );
      if ( tmp.nbDifferent() >= minPlanes ) {
        goodCandidates.push_back( *itL );
        if ( (*itL).quality() < bestQuality ) bestQuality = (*itL).quality();
      } else {
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
          debug() << "Ignore candidate " << itL-tempCandidates.begin()
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
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
          debug() << "Ignore candidate " << itL-tempCandidates.begin()
                  << " : quality too low = " << (*itL).quality() << endmsg;
      }
    }
    // remove if sensibly less OT
    if ( 24 < maxOT ) maxOT = 24;
    maxOT -= 3;
    tempCandidates = goodCandidates;
    goodCandidates.clear();
    for ( itL = tempCandidates.begin(); tempCandidates.end() != itL; ++itL ) {
      if ( 2*(*itL).nbIT() + (*itL).nbOT() > maxOT ) {
        goodCandidates.push_back( *itL );
      } else {
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
          debug() << "Ignore candidate " << itL-tempCandidates.begin()
                  << " : not enough OT = " << (*itL).nbOT() << " mini " << maxOT << endmsg;
      }
    }
  }

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "Storing " << goodCandidates.size() << " good tracks " << endmsg;
  //=== Store tracks...
  for ( itL = goodCandidates.begin(); goodCandidates.end() != itL; ++itL ) {
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


    for (unsigned int i=0; i<m_fwdTool->zOutputs().size(); i++)
      {
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
    fwTra->setChi2PerDoF( (*itL).chi2PerDoF() );
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
      if (sc.isFailure())
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
          debug()<<" Failure in adding TT clusters to track"<<endmsg;
    }
    if ( NULL != m_addUTClusterTool ) {
      StatusCode sc = m_addUTClusterTool->addUTClusters( *fwTra );
      if (sc.isFailure())
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
          debug()<<" Failure in adding UT clusters to track"<<endmsg;
    }
  }
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "Finished track" << endmsg;
  return StatusCode::SUCCESS;
}
//=========================================================================
//  Fill the vector of hit pointer, sorted by projection.
//=========================================================================

void PatForwardTool::fillXList ( PatFwdTrackCandidate& track, 
                                 double kick, double maxRangeRef, double zMagnet ) {
  double xExtrapRef = track.xStraight( m_fwdTool->zReference() );
  // == propagate kick if momentum estimate given
  if (std::abs(kick) > 1e-5) xExtrapRef += kick ; 
  const double xMin = xExtrapRef - maxRangeRef;
  const double xMax = xExtrapRef + maxRangeRef;

  PatFwdHits::const_iterator itFwdH;

  for (unsigned int sta = 0; sta < m_nSta; sta ++){
    for (unsigned int lay = 0; lay< m_nLay; lay++){
      if (lay == 1 || lay == 2) continue;  // X layers
      for (unsigned int region = 0; region <m_nReg; region ++){
        const Tf::EnvelopeBase* regionB = m_tHitManager->region(sta,lay,region);

        double yCompat = m_yCompatibleTol + 50 * fabs(track.slY());
        double yRegion = track.yStraight( regionB->z() );
        
        double xExtrapStation  = track.xStraight( regionB->z() );
        // == propagate kick if momentum estimate given
        if (std::abs(kick) > 1e-5) xExtrapStation += kick *( regionB->z() - zMagnet ) / ( m_fwdTool->zReference() - zMagnet );
        const double deltaX = maxRangeRef * ( regionB->z() - zMagnet ) / ( m_fwdTool->zReference() - zMagnet );
        double xHitMin = xExtrapStation - deltaX;
        xHitMin        = xHitMin - fabs( yRegion * regionB->sinT() ) - 20.;
        double ty = track.slY();
        double y0 = track.yStraight( 0. );

        if ( yRegion+yCompat < regionB->ymin() ||
             yRegion-yCompat > regionB->ymax() ) {
          continue;
        }

        Tf::TStationHitManager<PatForwardHit>::HitRange range =
          m_tHitManager->hitsWithMinX(xHitMin, sta, lay, region);

        for ( PatFwdHits::const_iterator itH = range.begin();
              range.end() != itH; ++itH ) {

          PatFwdHit* hit = *itH;

	  if (hit->hit()->ignore()) continue;

          updateHitForTrack( hit, y0, ty);

          if ( !hit->hit()->isYCompatible( yRegion, yCompat ) )
            continue;

          hit->setSelected( true );
          hit->setIgnored( false );
          hit->setRlAmb( 0 );
          
          if (hit->hit()->type() == Tf::RegionID::OT)
            if ( m_maxOTDrift < hit->driftDistance() ||
                 m_minOTDrift > hit->driftDistance() ) {
              hit->setSelected( false );
              continue;
            }

          double xRef = m_fwdTool->xAtReferencePlane( track, hit );
          hit->setProjection( xRef );

          if ( xMin > xRef ) continue;
          if ( xMax < xRef ) break;

          m_xHitsAtReference.push_back( hit );

        }
      }
    }
  }


  std::sort( m_xHitsAtReference.begin(),
             m_xHitsAtReference.end(),
             Tf::increasingByProjection<PatForwardHit>() );

}

//=========================================================================
//  Fill the vector of hit pointer, sorted by projection.
//=========================================================================
bool PatForwardTool::fillStereoList ( PatFwdTrackCandidate& track, double tol ) {

  PatFwdHits temp;
  PatFwdHits::const_iterator itH;

  for (unsigned int sta=0; sta<m_nSta; sta++) {
    for (unsigned int lay=1; lay<m_nLay-1; lay++) {
      for (unsigned int region=0; region<m_nReg; region++) {

        const Tf::EnvelopeBase* regionB = m_tHitManager->region(sta,lay,region);
        double dz = regionB->z() - m_fwdTool->zReference();
        double yRegion = track.y( dz );

        double tolY = m_yCompatibleTol;
        if (yRegion+tolY < regionB->ymin() ||
            yRegion-tolY > regionB->ymax() ) continue;

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
        if ( region< m_nOTReg ) minProj += 1.5;

        Tf::TStationHitManager<PatForwardHit>::HitRange range = m_tHitManager->hitsWithMinX(xHitMin, sta, lay, region);

        for ( PatFwdHits::const_iterator itH = range.begin();
              range.end() != itH; ++itH ) {



          PatFwdHit* hit = *itH;
	  if (hit->hit()->ignore())
	    continue;

          if ( !hit->hit()->isYCompatible( (float)yRegion, (float)tolY ) ) continue;

          updateHitForTrack( hit, y0, ty );

          hit->setSelected( true );
          hit->setIgnored( false );
          if (hit->hit()->type() == Tf::RegionID::OT)
            if ( m_maxOTDrift < hit->driftDistance() ||
                 m_minOTDrift > hit->driftDistance()  ) {
              hit->setSelected( false );
              continue;
            }
          double xRef = ( hit->x() - xPred ) * sign;
          hit->setProjection( xRef );

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
  double maxSpread = 3.;

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "List size = " << temp.size() << endmsg;
  if ( minYPlanes > (int)temp.size() ) return false;


  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
    for ( itH = temp.begin(); temp.end() != itH; ++itH ) {
      PatFwdHit* hit = *itH;
      debug() << format( " Selected:  Z %10.2f Xp %10.2f X%10.2f  St%2d lay%2d typ%2d Prev%2d Next%2d",
                         hit->z(),
                         hit->projection(),
                         hit->x(),
                         hit->hit()->station(),
                         hit->hit()->layer(),
                         hit->hit()->region(),
                         hit->hasPrevious(),
                         hit->hasNext() ) << endmsg;
    }
  }

  //== Select a coherent group
  PatFwdHits bestList;
  int nbDifferent = 0;
  double size = 1000.;

  PatFwdHits::iterator itP, itB, itE, itF;
  for ( itP = temp.begin();  temp.end() - minYPlanes >= itP; ++itP ) {
    itE = itP + minYPlanes -1;
    double spread = maxSpread;
    if ((*itP)->hit()->type() == Tf::RegionID::OT) spread += 1.5;  // OT drift ambiguities...

    if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) {
      verbose() << format( "  first %8.2f +minXPlanes -> %8.2f (diff: %8.2f) Spread %6.2f ",
                           (*itP)->projection(), (*itE)->projection(),
                           (*itE)->projection() - (*itP)->projection(), spread );
    }

    //== If not enough hits in the maximum spread, skip
    if (  spread < (*itE)->projection() - (*itP)->projection() ) {
      while( spread < (*itE)->projection() - (*itP)->projection() ) itP++;
      --itP; // as there will be a ++ in the loop !
      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) 
        verbose() << "   not enough planes in spread" << endmsg;
      continue;
    }


    //== Add all hits inside the maximum spread. If not enough planes, restart
    while ( itE != temp.end() &&
            spread > (*itE)->projection() - (*itP)->projection() ) itE++;
    PatFwdPlaneCounter planeCount( itP, itE );
    //== Enough different planes
    if ( minYPlanes > planeCount.nbDifferent() ) {
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << "   Not enough y planes : " << planeCount.nbDifferent() << endmsg;
      continue;
    }
    if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) verbose() << endmsg;

    //== Try to make a single zone, by removing the first and adding other as
    //   long as the spread and minXPlanes conditions are met.
    itB = itP;
    itF = itE;
    while ( itP <= itE - minYPlanes && itF < temp.end() ) {
      planeCount.removeHit( *itP );
      ++itP;
      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) 
        verbose() << " try to extend from itP : " << (*itP)->projection()
                  << " itF " << (*itF)->projection() << endmsg;
      while ( itF < temp.end() &&
              spread > (*itF)->projection() - (*itP)->projection() ) {
        planeCount.addHit( *itF++ );
      }
      if ( minYPlanes <= planeCount.nbDifferent() ) itE = itF;
    }

    double x1 = (*itB)->projection();
    double x2 = (*(itE-1))->projection();
    if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) {
      PatFwdPlaneCounter pc( itB, itE );
      verbose() << format( "Found Y group from %9.2f to %9.2f with %2d entries and %2d planes, spread %9.2f",
                           x1, x2, itE-itB, pc.nbDifferent(), spread)
                << endmsg;
    }
    //== We have the first list. The best one ????
    PatFwdPlaneCounter cnt( itB, itE );
    if ( cnt.nbDifferent() >= nbDifferent ) {
      if ( cnt.nbDifferent() > nbDifferent || x2-x1 < size ) {
        nbDifferent =  cnt.nbDifferent();
        size = x2-x1;
        bestList.clear();
        for ( itP = itB; itE != itP; ++itP ) {
          bestList.push_back( *itP );
        }
        //break; /// Keep first one !
      }
    }
    itP = --itE;
  }

  if ( minYPlanes > (int)bestList.size() ) return false;

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "...Selected " << bestList.size() << " hits from "
            << (*bestList.begin())->projection()
            << " to " << (*bestList.rbegin())->projection() << endmsg;

  for ( itP = bestList.begin(); bestList.end() != itP; ++itP ) {
    track.addCoord( *itP );
  }



  //== Sort by Z
  std::sort( track.coordBegin(), track.coordEnd(), 
	     Tf::increasingByZ<PatForwardHit>() );

  return true;
}

//=========================================================================
//  Debug one vector of of Hits
//=========================================================================
void PatForwardTool::debugFwdHits ( PatFwdTrackCandidate& track ) {
  debugFwdHits( track, debug() );
}

void PatForwardTool::debugFwdHits ( PatFwdTrackCandidate& track, MsgStream& msg ) {
  PatFwdHits::iterator itP;
  for ( itP = track.coordBegin(); track.coordEnd() != itP; ++itP ) {
    msg << format( " Z %10.2f Xp %10.2f X%10.2f  St%2d lay%2d typ%2d Prev%2d Next%2d Drift %7.3f",
                   (*itP)->z(),
                   (*itP)->projection(),
                   (*itP)->x(),
                   (*itP)->hit()->station(),
                   (*itP)->hit()->layer(),
                   (*itP)->hit()->region(),
                   (*itP)->hasPrevious(),
                   (*itP)->hasNext(),
                   (*itP)->driftDistance() );
    if ( track.fitted() ) msg << format( " chi2%7.2f", m_fwdTool->chi2Hit( track, (*itP)) );
    /* for ( std::vector<int>::const_iterator itM = (*itP)->MCKeys().begin();
       (*itP)->MCKeys().end() != itM; ++itM ) {
       msg << " " << *itM;
       if ( (*itM) == m_MCKey ) msg << " <=*** ";
       }
    */
    msg << endmsg;
  }
}




//=========================================================================
//  Build the list of vector of X hit candidates.
//=========================================================================
void PatForwardTool::buildXCandidatesList ( PatFwdTrackCandidate& track ) {

  m_candidates.clear();
  m_xHitsAtReference.clear();

  double xExtrap  = track.xStraight( m_fwdTool->zReference() );
  //== calculate if minPt or minMomentum sets the window size
  double minMom = m_minPt / track.sinTrack();
  if ( m_minMomentum > minMom ) minMom = m_minMomentum;
  //== calculate center of magnet from Velo track
  const double zMagnet =  m_fwdTool->zMagnet( track );
  const double dSlope =  m_magnetKickParams[0] / ( minMom - m_magnetKickParams[1] ) ;
  double maxRange = dSlope*( m_fwdTool->zReference() - zMagnet);

  double kick = 0.0;
  if (m_useMomentumEstimate && 0 != track.qOverP() && !m_withoutBField) {
    const double q = track.qOverP() > 0 ? 1. : -1.;
    const double magscalefactor = m_fwdTool->magscalefactor() ;
    kick = q*magscalefactor*(-1)*m_magnetKickParams[0] / ( fabs(1./track.qOverP()) -  m_magnetKickParams[1] ) ;
    kick *= ( m_fwdTool->zReference() - zMagnet);
    maxRange = m_minRange + m_momentumEstimateError*fabs(kick);
  
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "   xExtrap = " << xExtrap
              << " q/p " << track.qOverP()
              << " predict " << xExtrap + kick << endmsg;

  }


  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "Search X coordinates, xMin " << xExtrap - maxRange
            << " xMax " << xExtrap + maxRange << endmsg;

  fillXList( track, kick, maxRange, zMagnet );
  
  if ( m_minXPlanes > m_xHitsAtReference.end() -  m_xHitsAtReference.begin() ) return;

  PatFwdHits::iterator itP, itB, itE, itF;
  int minXPlanes = m_minXPlanes;

  double lastEnd = -1.e10;

  for ( itP = m_xHitsAtReference.begin();  m_xHitsAtReference.end() - minXPlanes > itP; ++itP ) {
    itE = itP + minXPlanes -1;
    double spreadSl = ( (*itP)->projection() - xExtrap ) * m_maxSpreadSlopeX;
    double spread = m_maxSpreadX + fabs( spreadSl );


     if ((*itP)->hit()->type() == Tf::RegionID::OT)  spread += 1.5;  // OT drift ambiguities...

    //== If not enough hits in the maximum spread, skip
    if (  spread < (*itE)->projection() - (*itP)->projection() ) {
      while( spread < (*itE)->projection() - (*itP)->projection() ) itP++;
      --itP; // as there will be a ++ in the loop !
      continue;
    }

    if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) {
      verbose() << format( "  first %8.2f +minXPlanes -> %8.2f (diff: %8.2f) Spread %6.2f ",
                           (*itP)->projection(), (*itE)->projection(),
                           (*itE)->projection() - (*itP)->projection(), spread )<<endmsg;
    }

    //== Add all hits inside the maximum spread. If not enough planes, restart
    while ( itE != m_xHitsAtReference.end() &&
            spread > (*itE)->projection() - (*itP)->projection() ) itE++;
    PatFwdPlaneCounter planeCount( itP, itE );
    //== Enough different planes
    if ( minXPlanes > planeCount.nbDifferent() ) {
      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) 
        verbose() << "   Not enough x planes : " << planeCount.nbDifferent() << endmsg;
      continue;
    }
    if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) verbose() << endmsg;

    //== Try to make a single zone, by removing the first and adding other as
    //   long as the spread and minXPlanes conditions are met.
    itB = itP;
    itF = itE;
    while ( itP <= itE - minXPlanes && itF < m_xHitsAtReference.end() ) {
      planeCount.removeHit( *itP );
      ++itP;
      while ( itF < m_xHitsAtReference.end() &&
              spread > (*itF)->projection() - (*itP)->projection() ) {
        planeCount.addHit( *itF++ );
      }
      if ( minXPlanes <= planeCount.nbDifferent() ) itE = itF;
    }

    double x1 = (*itB)->projection();
    double x2 = (*(itE-1))->projection();
    if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) {
      PatFwdPlaneCounter pc( itB, itE );
      verbose() << format( "Found X group from %9.2f to %9.2f with %2d entries and %2d planes, spread %9.2f",
                           x1, x2, itE-itB, pc.nbDifferent(), spread)
                << endmsg;
    }
    //== Protect against too dirty area.
    if ( itE - itB < m_maxXCandidateSize ) { 
      PatFwdHits temp( itB, itE );       // Create a copy of the list
      //== Try to merge the lists, if the first new point is close to the last one...
      if ( spread > x1 - lastEnd ) {
        (*m_candidates.rbegin()).addCoords( temp );
      } else {
        PatFwdTrackCandidate aCandidate( track.track(), temp );
        m_candidates.push_back( aCandidate );
      }
      lastEnd = x2;
    }
    itP = --itE;
  }

}

//=============================================================================
