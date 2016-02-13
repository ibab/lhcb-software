// Include files
// -------------
// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// from LHCb
#include "LHCbMath/LHCbMath.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackManipulator.h"
#include "TrackInterfaces/IMaterialLocator.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "TrackInterfaces/ITrackProjector.h"


// from TrackEvent
#include "Event/TrackFunctor.h"
#include "Event/StateParameters.h"
#include "Event/Track.h"
#include "Event/KalmanFitResult.h"
#include "Event/FitNode.h"
#include "Event/OTMeasurement.h"
#include "Event/State.h"
#include "TrackKernel/TrackTraj.h"

// local
#include "TrackMasterFitter.h"

#include <cmath>

//stl
#include <algorithm>

#include "boost/container/static_vector.hpp"

using namespace Gaudi;
using namespace LHCb;

namespace {
//! check that this was a prefit iteration
bool isPrefit( const LHCb::TrackFitResult& fr )
{
  return std::any_of( fr.nodes().begin(), fr.nodes().end(),
                      [](const LHCb::Node *node) {
    if ( !node->hasMeasurement()  ||
         node->measurement().type()!= LHCb::Measurement::OT ) {
      return false;
    }
    const auto* otmeas = dynamic_cast<const LHCb::OTMeasurement*>(&(node->measurement())) ;
    return otmeas && otmeas->driftTimeStrategy() == LHCb::OTMeasurement::PreFit;
  });
}

//=========================================================================

//! Add info from fitter as extrainfo to track
void fillExtraInfo(Track& track )
{
  // TODO: migrate clients to use KalmanFitResult directly. Then
  // remove the extrainfo field.

  // Clean up the track info
  track.eraseInfo( Track::FitVeloChi2 ) ;
  track.eraseInfo( Track::FitVeloNDoF ) ;
  track.eraseInfo( Track::FitTChi2 ) ;
  track.eraseInfo( Track::FitTNDoF ) ;
  track.eraseInfo( Track::FitMatchChi2 ) ;
  track.eraseInfo( Track::FitFracUsedOTTimes ) ;

  auto kalfit =
    static_cast<const LHCb::KalmanFitResult*>(track.fitResult()) ;

  if( track.hasT() ) {
    track.addInfo( Track::FitTChi2 , kalfit->chi2Downstream().chi2() ) ;
    track.addInfo( Track::FitTNDoF , kalfit->chi2Downstream().nDoF() ) ;
    unsigned int nOTMeas = kalfit->nMeasurements( LHCb::Measurement::OT ) ;
    if( nOTMeas > 0 )
      track.addInfo( Track::FitFracUsedOTTimes, kalfit->nActiveOTTimes() / double( nOTMeas ) ) ;
  }

  if( track.hasVelo() ) {
    track.addInfo( Track::FitVeloChi2, kalfit->chi2Velo().chi2() ) ;
    track.addInfo( Track::FitVeloNDoF, kalfit->chi2Velo().nDoF() ) ;
    if( track.hasT() )
      track.addInfo( Track::FitMatchChi2, kalfit->chi2Match().chi2() ) ;
  }
}

//=========================================================================
// Determine the z-position of the closest approach to the beam line
//  by linear extrapolation.
//=========================================================================
double closestToBeamLine( const State& state )
{
  const TrackVector& vec = state.stateVector();
  auto z = state.z();
  // check on division by zero (track parallel to beam line!)
  if ( vec[2] != 0 || vec[3] != 0 ) {
    z -= ( vec[0]*vec[2] + vec[1]*vec[3] ) / ( vec[2]*vec[2] + vec[3]*vec[3] );
  }
  // don't go outside the sensible volume
  return std::min(std::max(z,-100*Gaudi::Units::cm), StateParameters::ZBegRich2 ) ;
}


constexpr struct FirstLess_t {
template<class T>
bool operator()(const T& lhs, const T& rhs) const {
  return lhs.first < rhs.first ;
}
} FirstLess { };


constexpr struct SecondGreater_t {
template <typename T>
bool operator()(const T& lhs, const T& rhs) const {
  return lhs.second > rhs.second ;
}
} DecreasingChi2 { };

auto hasMeasurement = [](const LHCb::Node* node) { return node->hasMeasurement(); };

}
//-----------------------------------------------------------------------------
// Implementation file for class : TrackMasterFitter
//
//  Original Author: Rutger van der Eijk
//  Created: 07-04-1999
//  Adapted: 21-03-2002  Olivier Callot
//  Adapted: 15-04-2005  Jose A. Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TrackMasterFitter )

//=========================================================================
// Standard Constructor, initializes variables
//=========================================================================
  TrackMasterFitter::TrackMasterFitter( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent)
    : base_class( type, name, parent)
    , m_extrapolator("TrackMasterExtrapolator", this)
    , m_veloExtrapolator("TrackLinearExtrapolator",this)
    , m_trackNodeFitter("TrackKalmanFilter", this)
    , m_measProvider("MeasurementProvider", this )
    , m_materialLocator("DetailedMaterialLocator",this)
    , m_projectorSelector("TrackProjectorSelector",this)
{
  declareInterface<ITrackFitter>( this );

  declareProperty( "Projector", m_projectorSelector ) ;
  declareProperty( "MeasProvider", m_measProvider ) ;
  declareProperty( "NodeFitter", m_trackNodeFitter ) ;
  declareProperty( "Extrapolator", m_extrapolator ) ;
  declareProperty( "VeloExtrapolator"    , m_veloExtrapolator ) ;
  declareProperty( "FitUpstream"         , m_upstream         = true        );
  declareProperty( "NumberFitIterations" , m_numFitIter       = 10          );
  declareProperty( "Chi2Outliers"        , m_chi2Outliers     = 9.0         );
  declareProperty( "MaxNumberOutliers"   , m_numOutlierIter   = 2           );
  declareProperty( "StateAtBeamLine"     , m_stateAtBeamLine  = true        );
  declareProperty( "AddDefaultReferenceNodes" , m_addDefaultRefNodes = true ) ;

  declareProperty( "UseSeedStateErrors", m_useSeedStateErrors = false );

  declareProperty( "ErrorX"         , m_errorX  = 20.0*Gaudi::Units::mm );
  declareProperty( "ErrorY"         , m_errorY  = 20.0*Gaudi::Units::mm );
  declareProperty( "ErrorTx"        , m_errorTx = 0.1                    );
  declareProperty( "ErrorTy"        , m_errorTy = 0.1                    );
  declareProperty( "ErrorQoP"       , m_errorQoP = { 0.0,0.01 } );
  declareProperty( "MakeNodes"      , m_makeNodes = false                   );
  declareProperty( "MakeMeasurements", m_makeMeasurements = false           );
  declareProperty( "MaterialLocator", m_materialLocator);
  declareProperty( "UpdateTransport", m_updateTransport = true );
  declareProperty( "UpdateMaterial", m_updateMaterial  = false );
  declareProperty( "UpdateReferenceInOutlierIterations", m_updateReferenceInOutlierIters = true ) ;
  declareProperty( "MinMomentumELossCorr", m_minMomentumForELossCorr = 10.*Gaudi::Units::MeV );
  declareProperty( "ApplyMaterialCorrections", m_applyMaterialCorrections = true );
  declareProperty( "ApplyEnergyLossCorr", m_applyEnergyLossCorrections = true ) ;
  declareProperty( "TransverseMomentumForScattering", m_scatteringPt = 400.*Gaudi::Units::MeV );
  declareProperty( "MomentumForScattering", m_scatteringP = -1 );
  declareProperty( "MinMomentumForScattering", m_minMomentumForScattering = 100.*Gaudi::Units::MeV );
  declareProperty( "MaxMomentumForScattering", m_maxMomentumForScattering = 500.*Gaudi::Units::GeV );
  declareProperty( "MinNumVeloRHitsForOutlierRemoval",   m_minNumVeloRHits   = 3 ) ;
  declareProperty( "MinNumVeloPhiHitsForOutlierRemoval", m_minNumVeloPhiHits = 3 ) ;
  declareProperty( "MinNumTTHitsForOutlierRemoval",      m_minNumTTHits      = 3 ) ;
  declareProperty( "MinNumTHitsForOutlierRemoval",       m_minNumTHits       = 6 ) ;
  declareProperty( "MinNumMuonHitsForOutlierRemoval",    m_minNumMuonHits    = 4 ) ;
  declareProperty( "MaxDeltaChiSqConverged",             m_maxDeltaChi2Converged = 0.01 ) ;
  declareProperty( "UseClassicalSmoother",       m_useClassicalSmoother = false ) ;
  declareProperty( "FillExtraInfo", m_fillExtraInfo = true ) ;
}


//=========================================================================
// Initialize
//=========================================================================
StatusCode TrackMasterFitter::finalize()
{
  m_extrapolator.release().ignore();
  m_veloExtrapolator.release().ignore();
  m_projectorSelector.release().ignore();
  m_measProvider.release().ignore() ;
  m_trackNodeFitter.release().ignore() ;
  m_materialLocator.release().ignore() ;
  return base_class::finalize() ;
}

//=========================================================================
// Initialize
//=========================================================================
StatusCode TrackMasterFitter::initialize()
{
  StatusCode sc = base_class::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;

  sc = m_projectorSelector.retrieve() ;
  if ( sc.isFailure() ) return sc;

  sc = m_measProvider.retrieve() ;
  if ( sc.isFailure() ) return sc;

  sc = m_extrapolator.retrieve() ;
  if ( sc.isFailure() ) return sc;

  sc = m_veloExtrapolator.retrieve() ;
  if ( sc.isFailure() ) return sc;

  sc = m_trackNodeFitter.retrieve() ;
  if ( sc.isFailure() ) return sc;

  sc = m_materialLocator.retrieve() ;
  if ( sc.isFailure() ) return sc;


  m_debugLevel   = msgLevel( MSG::DEBUG ) || msgLevel( MSG::VERBOSE ) ;

  if(m_debugLevel) debug() << " " << endmsg
                           << "============ TrackMasterFitter Settings ===========" << endmsg
                           << ((m_upstream) ? " Upstream fit" : " Downstream fit") << endmsg
                           << " Number of fit iterations: " << m_numFitIter << endmsg
                           << " Max " << m_numOutlierIter << " outliers removed with outliers"
                           << " at chi2 > " << m_chi2Outliers << endmsg
                           << " State z positions at: " << endmsg
                           << ((m_stateAtBeamLine) ? " beam line," : "") << " first/last measurement"
                           << (m_addDefaultRefNodes ? ", default reference positions" : "" )  << endmsg
                           << "==================================================" << endmsg;

  return StatusCode::SUCCESS;
}

//=========================================================================
// Helper to print a failure comment
//=========================================================================
StatusCode TrackMasterFitter::failure( const std::string& comment ) const
{
  if ( m_debugLevel ) debug() << "TrackMasterFitter failure: " + comment << endmsg ;
  return Warning(comment,StatusCode::FAILURE,1) ;
}
//=========================================================================
// Helper to print a failure comment (with info instead of warning)
//=========================================================================
StatusCode TrackMasterFitter::failureInfo( const std::string& comment ) const
{
  if ( m_debugLevel ) debug() << "TrackMasterFitter failure: " + comment << endmsg ;
  return Info(comment,StatusCode::FAILURE,1) ;
}
//=========================================================================
// Fit the track
//=========================================================================
StatusCode TrackMasterFitter::fit( Track& track, LHCb::ParticleID pid ) const
{
  if(m_debugLevel)
    debug() << " TrackMasterFitter::fit" << endmsg ;

  // any track that doesnt make it to the end is failed
  track.setFitStatus( Track::FitFailed );

  // create the KalmanFitResult if it doesn't exist yet
  LHCb::KalmanFitResult* kalfitresult = dynamic_cast<LHCb::KalmanFitResult*>(track.fitResult()) ;
  if( !kalfitresult ) {
    kalfitresult = track.fitResult() ?
      new LHCb::KalmanFitResult(*track.fitResult()) :
      new LHCb::KalmanFitResult() ;
    if( m_useClassicalSmoother )kalfitresult->setBiDirectionnalSmoother(false);
    track.setFitResult( kalfitresult ) ;
  }

  // Make the nodes from the measurements
  StatusCode sc;
  LHCb::TrackFitResult::NodeContainer& nodes = track.fitResult()->nodes() ;
  if( nodes.empty() || m_makeNodes ) {
    sc = makeNodes( track,pid );
    if ( sc.isFailure() ) {
      kalfitresult->clearNodes() ;
      return failureInfo( "unable to make nodes from the measurements" );
    }
  } else {
    sc = updateRefVectors( track, pid ) ;
    if ( sc.isFailure() )
      return failureInfo( "unable to update the ref vectors" );
  }

  // create a covariance matrix to seed the Kalman fit
  TrackSymMatrix seedCov ; // Set off-diagonal elements to zero
  if (m_useSeedStateErrors) {
    State state0 = track.firstState();
    auto z1 = nodes.front()->z();
    extrapolator(track.type())->propagate(state0,z1);
    seedCov = state0.covariance() ;
    if ( m_debugLevel )
      debug() << " state0 at z " << z1
              << " vector " << state0.stateVector() << "\n"
              << " covariance " << state0.covariance() << endmsg;
  } else {
    seedCov(0,0) = m_errorX*m_errorX ;
    seedCov(1,1) = m_errorY*m_errorY ;
    seedCov(2,2) = m_errorTx*m_errorTx ;
    seedCov(3,3) = m_errorTy*m_errorTy ;
    seedCov(4,4) = std::pow( m_errorQoP[0] * track.firstState().qOverP(), 2 ) + std::pow(m_errorQoP[1],2);
  }
  kalfitresult->setSeedCovariance( seedCov ) ;
  static_cast< FitNode*>( nodes.front() )->setSeedCovariance( seedCov ) ;
  static_cast< FitNode*>( nodes.back() )->setSeedCovariance( seedCov ) ;

  if ( m_debugLevel )
    debug() << "SeedState: z = " << nodes.front()->z()
            << " stateVector = " << nodes.front()->refVector()
            << " covariance  = " << seedCov << endmsg;

  // Iterate the track fit for linearisation. Be careful with chi2
  // convergence here: The first iteration might not be using OT
  // drifttimes in which case the chi2 can actually go up in the 2nd
  // iteration.
  int iter = 1;
  bool converged = false ;
  for ( ; iter <= m_numFitIter && !converged ; ++iter ) {
    if ( m_debugLevel ) debug() << "Iteration # " << iter << endmsg;

    // update reference trajectories with smoothed states
    // TODO: combine this with the projection of the residuals which now resides in TrackKalmanFilter
    if( iter > 1) {
      sc = updateRefVectors( track, pid );
      if ( sc.isFailure() ) return failureInfo( "problem updating ref vectors" );
    }

    bool prevwasprefit = isPrefit( *kalfitresult ) ;
    auto prevchi2 = track.chi2() ;
    sc = m_trackNodeFitter -> fit( track );

    if ( sc.isFailure() )  return failureInfo( std::string("unable to fit the track. ") +
                                           kalfitresult->getError() );

    if ( m_debugLevel ) debug() << "chi2 =  " << track.chi2()
                                << " ref state = (" << nodes.back()->refVector()
                                << ") at z= " << nodes.back()->z() << endmsg;
    auto dchi2 = prevchi2 - track.chi2() ;
    // require at least 3 iterations, because of the OT prefit.
    converged = !prevwasprefit && iter>1 && std::abs(dchi2) < m_maxDeltaChi2Converged * track.nDoF();

  }
  kalfitresult->setNIter( iter-1 ) ;

  // Outlier removal iterations
  iter = kalfitresult->nOutliers() ;
  LHCb::Node* outlier = nullptr;
  while ( iter < m_numOutlierIter && track.nDoF() > 1 &&
          (outlier = outlierRemoved( track )) ) {
    if ( m_debugLevel ) debug() << "Outlier iteration # " << iter << endmsg;

    // update reference trajectories with smoothed states
    if (m_updateReferenceInOutlierIters) {
      sc = updateRefVectors( track, pid );
      if ( sc.isFailure() ) return failureInfo( "problem updating ref vectors" );
    }

    // deactivate the outlier
    static_cast<LHCb::FitNode*>(outlier)->deactivateMeasurement() ;

    // Call the track fit
    sc = m_trackNodeFitter -> fit( track );

    if ( sc.isFailure() ) {
      // Should only be used if the track belongs to a container and therefore has a key!
      //std::ostringstream mess;
      //mess << "unable to fit the track # " << track.key();
      //return failure( mess.str() );
      return failureInfo( "unable to fit the track" );
    }

    if ( m_debugLevel ) debug() << "chi2 =  " << track.chi2()
                                << " ref state = (" << nodes.back()->refVector()
                                << ") at z= " << nodes.back()->z() << endmsg;
    ++iter;
  }

  // TODO: if outliers are removed, we add one more iteration to update the reference

  // determine the track states at user defined z positions
  sc = determineStates( track );
  if ( sc.isFailure() )
    return failureInfo( "failed in determining states" ) ;

  if ( m_debugLevel && !track.states().empty() )
    debug() << "first state = " << track.firstState() << endmsg;

  // fill extra info
  if( m_fillExtraInfo ) fillExtraInfo( track ) ;

  // declare the track successful if there are no errors
  if( kalfitresult->inError() ) {
    sc = Warning(kalfitresult->getError(),StatusCode::FAILURE,0) ;
  } else {
    track.setFitStatus( Track::Fitted );
  }

  return sc;
}



//=============================================================================
//
//=============================================================================
StatusCode TrackMasterFitter::determineStates( Track& track ) const
{
  StatusCode sc = StatusCode::SUCCESS;

  // clean the non-fitted states in the track!
  track.clearStates();

  LHCb::TrackFitResult::NodeContainer& nodes = track.fitResult()->nodes();

  // Add the state at the first and last measurement position
  // -----------------------------------------------
  auto inode = std::find_if( nodes.cbegin(), nodes.cend(), hasMeasurement );
  auto* firstMeasurementNode = (inode!=nodes.cend()? *inode : nullptr );
  auto jnode = std::find_if( nodes.crbegin(), nodes.crend(), hasMeasurement );
  auto* lastMeasurementNode = (jnode!=nodes.crend()? *jnode : nullptr );

  bool upstream = nodes.front()->z() > nodes.back()->z() ;
  bool reversed =
    ( upstream && !track.checkFlag(Track::Backward ) ) ||
    ( !upstream && track.checkFlag(Track::Backward )) ;

  // This state is not filtered for a forward only fit.
  if( m_addDefaultRefNodes ) {
    State firststate = firstMeasurementNode->state() ;
    firststate.setLocation( reversed ? State::LastMeasurement : State::FirstMeasurement );
    track.addToStates( firststate ) ;
  }
  // This state is always filtered
  State laststate = lastMeasurementNode->state() ;
  laststate.setLocation( reversed ? State::FirstMeasurement : State::LastMeasurement );
  track.addToStates( laststate ) ;

  // Add the states at the reference positions
  // ------------------------------------------
  for( const auto& node : nodes)
    if( node->type() == LHCb::Node::Reference )
      track.addToStates( node->state() ) ;

  if ( m_debugLevel ) {
    debug() << "Track " << track.key() << " has " << track.nStates()
            << " states after fit\n  at z = " ;
    for ( const auto& s : track.states() ) debug() << s->z() << ", " ;
    debug() << track.fitResult()->nActiveMeasurements()
            << " measurements used for the fit (out of "
            << track.nLHCbIDs() << ")." << endmsg;
  }
  return sc;
}


//=========================================================================
//
//=========================================================================
namespace {
  enum HitType {VeloR, VeloPhi, TT, T, Muon, Unknown} ;
  inline int hittypemap( LHCb::Measurement::Type type ) {
    HitType rc = Unknown ;
    switch( type ) {
    case LHCb::Measurement::Unknown:   rc = Unknown ; break ;
    case LHCb::Measurement::VeloR:   rc = VeloR ; break ;
    case LHCb::Measurement::VeloPhi: rc = VeloPhi ; break ;
    case LHCb::Measurement::VeloLiteR:   rc = VeloR ; break ;
    case LHCb::Measurement::VeloLitePhi: rc = VeloPhi ; break ;
    case LHCb::Measurement::TT: rc = TT; break;
    case LHCb::Measurement::IT: rc = T; break;
    case LHCb::Measurement::OT: rc = T; break;
    case LHCb::Measurement::Muon: rc = Muon; break;
    case LHCb::Measurement::TTLite: rc = TT; break;
    case LHCb::Measurement::ITLite: rc = T; break;
    case LHCb::Measurement::VP: rc = VeloR; break;
    case LHCb::Measurement::Calo: rc = Unknown; break;
    case LHCb::Measurement::Origin : rc = Unknown; break;
    case LHCb::Measurement::FT : rc = T; break;
    case LHCb::Measurement::UT : rc = TT; break;
    case LHCb::Measurement::UTLite : rc = TT; break;
    }
    return rc;
  }
}


LHCb::Node* TrackMasterFitter::outlierRemoved( Track& track ) const
{
  // return true if outlier chi2 cut < 0
  LHCb::Node* outlier = nullptr;
  if ( m_chi2Outliers < 0.0 ) return outlier ;

  // Count the number of hits of each type

  const size_t minNumHits[5] = { m_minNumVeloRHits, m_minNumVeloPhiHits, m_minNumTTHits, m_minNumTHits, m_minNumMuonHits } ;
  size_t numHits[5]          = {0,0,0,0,0} ;

  auto fr = dynamic_cast<const LHCb::KalmanFitResult*>(track.fitResult()) ;
  auto nodes = fr->fitNodes() ;
  for( const auto& node : nodes ) {
    if( node->type() == LHCb::Node::HitOnTrack ){
      ++numHits[hittypemap(node->measurement().type())] ;
    }
  }

  // loop over the nodes and find the one with the highest chi2 >
  // m_chi2Outliers, provided there is enough hits of this type left.

  // Computing the chi2 will trigger the smoothing. Especially in the
  // trigger, where we don't update the reference, this makes a big
  // difference. One way to save some time is to first test a number
  // of which we know that it is either equal to or bigger than the
  // chi2 contribution of the hit, namely the chi2 sum of the match
  // and the hit at this node. We then sort the hits in this number,
  // and only compute chi2 if it can be bigger than the current worst
  // one.
  const double totalchi2  = std::max( nodes.front()->totalChi2(LHCb::FitNode::Backward).chi2(),
                                      nodes.back()->totalChi2(LHCb::FitNode::Forward).chi2() ) ;
  using NodeWithChi2 = std::pair<const FitNode*, double>;
  std::vector< NodeWithChi2 > nodesWithChi2UL ;
  nodesWithChi2UL.reserve( nodes.size() ) ;
  int numtried(0),numcalled(0) ;
  for ( const auto&  node : nodes ) {
    if ( node->hasMeasurement() &&
         node->type() == LHCb::Node::HitOnTrack ) {
      int hittype = hittypemap(node->measurement().type()) ;
      if( numHits[hittype] > minNumHits[hittype] ) {
        ++numtried ;
        auto chi2MatchAndHit = totalchi2 ;
        for(int dir=0; dir<2; ++dir) {
          const auto* tmpnode =  node->prevNode(dir) ;
          if(tmpnode) chi2MatchAndHit -= tmpnode->totalChi2(dir).chi2() ;
        }
        if( chi2MatchAndHit > m_chi2Outliers ) {
          nodesWithChi2UL.emplace_back(  node, chi2MatchAndHit ) ;
        }
      }
    }
  }

  // now sort them
  auto worstChi2 = m_chi2Outliers;
  std::sort( nodesWithChi2UL.begin(), nodesWithChi2UL.end(), DecreasingChi2) ;

  // -- if the first is smaller than worstChi2, and it is sorted in decreasing order
  // -- the 'if' will never be true and we can return here (M. De Cian)
  if( !nodesWithChi2UL.empty() && nodesWithChi2UL.front().second < worstChi2 ) return outlier;
  for(const auto& node : nodesWithChi2UL ) {
    if( node.second > worstChi2 ) {
      const auto chi2 = node.first->chi2();
      ++numcalled ;
      if ( chi2 > worstChi2 ) {
        worstChi2 = chi2;
        outlier = const_cast<LHCb::FitNode*>(node.first) ;
      }
    }
  }
  if(outlier && m_debugLevel) {
    debug() << "Measurement of type="
            << outlier -> measurement().type()
            << " LHCbID="
            << outlier -> measurement().lhcbID().channelID()
            << " at z=" << outlier->z()
            << " with chi2=" << outlier->chi2()
            << " removed." << endmsg;
  }

  return outlier ;
}

//=========================================================================
// Update the measurements before a refit
//=========================================================================
StatusCode TrackMasterFitter::updateRefVectors( Track& track, LHCb::ParticleID pid ) const
{
  if( m_debugLevel )
    debug() << "TrackMasterFitter::updateRefVectors" << endmsg ;

  StatusCode sc = StatusCode::SUCCESS ;
  for (auto & node : track.fitResult()->nodes() ) {
    node->setRefVector( node->state().stateVector() ) ;
  }
  if( m_debugLevel )
    debug() << "Ref vector for first state: " << track.fitResult()->nodes().front()->refVector() << endmsg ;

  // update the projections. need to be done every time ref is
  // updated. we can move this code here at some point.
  sc = projectReference( track ) ;
  if( sc.isFailure() ) return failureInfo( "problem projecting reference" );

  // update the material using the new ref vectors
  if(m_applyMaterialCorrections && m_updateMaterial) {
    sc = updateMaterialCorrections(track,pid) ;
    if ( sc.isFailure() ) return failureInfo( "problem updating material" );
  }

  // update the transport using the new ref vectors
  if(m_updateTransport) {
    sc = updateTransport(track) ;
    if ( sc.isFailure() ) return failureInfo( "problem updating transport" );
  }
  return sc ;
}

//=========================================================================
// Update the measurements before a refit
//=========================================================================
StatusCode TrackMasterFitter::projectReference( LHCb::Track& track ) const
{
  StatusCode sc = StatusCode::SUCCESS ;
  for( LHCb::Node* node: track.fitResult()->nodes() ) {
    if( !node->refIsSet() ) {
      sc = Warning( "Node without reference", StatusCode::FAILURE, 0 );
      if(msgLevel(MSG::DEBUG)) debug() << "Node without reference" << endmsg ;
      break ;
    }
    if( !node->hasMeasurement() ) continue ;
    // if the reference is not set, issue an error
    ITrackProjector *proj = m_projectorSelector->projector(node->measurement());
    if ( !proj ) {
      sc = Warning( "Could not get projector for measurement", StatusCode::FAILURE, 0 );
      if(msgLevel(MSG::DEBUG)) debug() << "could not get projector for measurement" << endmsg ;
      break ;
    }
    LHCb::FitNode& fitnode = dynamic_cast<LHCb::FitNode&>(*node) ;
    sc = proj -> projectReference(fitnode) ;
    if ( sc.isFailure() ) {
      Warning( "unable to project statevector", sc, 0 ).ignore();
      if(msgLevel(MSG::DEBUG)) debug() << "unable to project this statevector: " << node->refVector()
                                       << endmsg ;
      break ;
    }
  }
  return sc;
}


//=============================================================================
// Create the nodes from the measurements
//=============================================================================
StatusCode TrackMasterFitter::makeNodes( Track& track, LHCb::ParticleID pid ) const
{
  // Clear the nodes
  LHCb::KalmanFitResult& fitresult = static_cast<LHCb::KalmanFitResult&>(*(track.fitResult())) ;
  fitresult.clearNodes() ;

  // Clear the measurements if asked for
  if(m_makeMeasurements) fitresult.clearMeasurements() ;

  // make sure the track has sufficient reference states
  if( m_debugLevel ) debug() << "Track before making nodes: " << track << endmsg ;
  StatusCode sc = initializeRefStates( track, pid );
  if (sc.isFailure())
    return Warning("Problems setting reference info", StatusCode::FAILURE, 1);

  // Check if it is needed to populate the track with measurements
  if ( track.checkPatRecStatus( Track::PatRecIDs ) ||
       fitresult.measurements().empty() ) {
    StatusCode sc1 = m_measProvider -> load( track );
    if ( sc1.isFailure() )
      return Error( "Unable to load measurements!", StatusCode::FAILURE );
    track.setPatRecStatus( Track::PatRecMeas );
    if(m_debugLevel)
      debug() << "# LHCbIDs, Measurements = " << track.nLHCbIDs()
              << ", " << fitresult.nMeasurements() << endmsg;
  }

  // check that there are sufficient measurements. in fact, one is
  // enough for the fit not to fail
  if( fitresult.measurements().empty() )
    return Warning("No measurements on track", StatusCode::FAILURE, 0);

  // Create the nodes for the measurements.
  const auto& measures = fitresult.measurements();
  TrackFitResult::NodeContainer& nodes = fitresult.nodes();
  nodes.reserve( measures.size() + 6 );
  std::transform( measures.rbegin(), measures.rend(),
                  std::back_inserter(nodes),
                  [](Measurement* m) { return new FitNode(*m); } );

  // Add reference nodes depending on track type
  if( m_addDefaultRefNodes ) {
    if(track.hasVelo() && !track.checkFlag(Track::Backward ))
      nodes.push_back( new FitNode( StateParameters::ZEndVelo, State::EndVelo )) ;
    if(track.hasTT() ) {
      nodes.push_back( new FitNode( StateParameters::ZBegRich1, State::BegRich1 )) ;
      nodes.push_back( new FitNode( StateParameters::ZEndRich1, State::EndRich1 )) ;
    }
    if(track.hasT() ) {
      nodes.push_back( new FitNode( StateParameters::ZBegT, State::AtT )) ;
      nodes.push_back( new FitNode( StateParameters::ZBegRich2, State::BegRich2 )) ;
      nodes.push_back( new FitNode( StateParameters::ZEndRich2, State::EndRich2 )) ;
    }
  }

  // At a node for the position at the beamline
  if ( m_stateAtBeamLine && (track.hasTT() || track.hasVelo()) ) {
    const LHCb::State& refstate = *(track.checkFlag(Track::Backward) ? track.states().back() : track.states().front()) ;
    nodes.push_back(new FitNode( closestToBeamLine(refstate), State::ClosestToBeam  ) ); 
  }

  // Sort the nodes in z
  bool backward = track.checkFlag(LHCb::Track::Backward) ;
  bool upstream = (m_upstream&&!backward) || (!m_upstream&&backward) ;
  if ( upstream ) {
    std::sort( nodes.begin(), nodes.end(), TrackFunctor::decreasingByZ<Node>());
  } else {
    std::sort( nodes.begin(), nodes.end(), TrackFunctor::increasingByZ<Node>());
  }

  // Set the reference using a TrackTraj
  LHCb::TrackTraj::StateContainer states ;
  states.insert( states.end(), track.states().begin(), track.states().end() ) ;
  LHCb::TrackTraj tracktraj(states) ;
  std::for_each( nodes.begin(), nodes.end(),
                 [&](LHCb::Node *node) { node->setRefVector( tracktraj.stateVector(node->z()) ) ;
  });

  // set links between nodes
  fitresult.establishNodeLinks() ;

  // update the projections. need to be done every time ref is updated
  sc = projectReference( track ) ;

  // add all the noise, if required
  if(m_applyMaterialCorrections && sc.isSuccess()) {
    sc = updateMaterialCorrections( track, pid ) ;
  }

  // create the transport of the reference (after the noise, because it uses energy loss)
  return sc.isSuccess() ? updateTransport( track ) : sc;
}

//=========================================================================

StatusCode TrackMasterFitter::updateMaterialCorrections(LHCb::Track& track, LHCb::ParticleID pid) const
{
  if( m_debugLevel )
    debug() << "TrackMasterFitter::updateMaterialCorrections" << endmsg ;

  // the noise in each node is the noise in the propagation between
  // the previous node and this node.

  // first collect all volumes on the track. The advantages of collecting them all at once
  // is that it is much faster. (Less call to TransportSvc.)
  LHCb::TrackFitResult& fitresult = *(track.fitResult()) ;
  LHCb::TrackFitResult::NodeContainer& nodes = fitresult.nodes() ;

  if( nodes.size()>1 ) {
    // only apply energyloss correction for tracks that traverse magnet
    bool   applyenergyloss    = m_applyEnergyLossCorrections && track.hasT() && (track.hasVelo()||track.hasTT()) ;

    // if only velo, or magnet off, use a fixed momentum based on pt.
    auto scatteringMomentum = track.firstState().p() ;
    if( m_scatteringPt>0 && !track.hasT() && !track.hasTT() ) {
      auto tx     = track.firstState().tx() ;
      auto ty     = track.firstState().ty() ;
      auto slope2 = tx*tx+ty*ty ;
      auto tanth  = std::max(std::sqrt( slope2/(1+slope2)),1e-4) ;
      scatteringMomentum = m_scatteringPt/tanth ;
    }
    // set some limits for the momentum used for scattering
    scatteringMomentum = std::min( scatteringMomentum, m_maxMomentumForScattering ) ;
    scatteringMomentum = std::max( scatteringMomentum, m_minMomentumForScattering ) ;

    // if m_scatteringP is set, use it
    if( m_scatteringP>0 ) scatteringMomentum = m_scatteringP ;
    fitresult.setPScatter( scatteringMomentum ) ;


    if ( m_debugLevel ) debug() << "scattering momentum: " << scatteringMomentum << endreq ;

    // this is farily tricky now: we want to use TracjTraj, but we
    // cannot create it directly from the nodes, because that would
    // trigger the filter!

    // note that this is the same traj as used in setting the first
    // ref. cannot we just save it somewhere?
    LHCb::TrackTraj::StateContainer states ;
    states.insert( states.end(), track.states().begin(), track.states().end() ) ;
    LHCb::TrackTraj tracktraj(states) ;
    auto zmin = nodes.front()->z() ;
    auto zmax = nodes.back()->z() ;
    if(zmin>zmax) std::swap(zmin,zmax) ;
    tracktraj.setRange( zmin,zmax ) ;

    IMaterialLocator::Intersections intersections ;
    // make sure we have the space we need in intersections so we don't need to
    // reallocate (offline, I've seen tracks with more than 670 intersections
    // in 100 events; we stay a bit above that to be on the safe side - and we
    // don't mind the occasional reallocate if it's a rare track that has even
    // more intersections)
    intersections.reserve(1024);
    m_materialLocator->intersect( tracktraj, intersections ) ;

    // now we need to redistribute the result between the nodes. the first node cannot have any noise.
    auto inode = nodes.begin() ;
    auto zorigin = (*inode)->z() ;
    for(++inode; inode!=nodes.end(); ++inode) {
      FitNode* node = dynamic_cast<FitNode*>(*inode) ;
      auto ztarget = node->z() ;
      LHCb::State state( node->refVector() ) ;
      state.covariance() = Gaudi::TrackSymMatrix() ;
      state.setQOverP( 1/scatteringMomentum  ) ;
      m_materialLocator->applyMaterialCorrections(state,intersections,zorigin,pid,true,applyenergyloss) ;
      auto deltaE = 1/state.qOverP() - scatteringMomentum ;

      node->setNoiseMatrix( state.covariance() ) ;
      node->setDeltaEnergy( deltaE  ) ;

      zorigin = ztarget ;
    }
  }
  return StatusCode::SUCCESS ;
}

StatusCode TrackMasterFitter::updateTransport(LHCb::Track& track) const
{
  if( m_debugLevel )
    debug() << "TrackMasterFitter::updateTransport" << endmsg ;

  // FIXME: This is probably the right place to set the number of
  // track parameters in the fitresult: simply check that the
  // propagation does not depend on momentum
  bool hasMomentum = false ;

  // sets the propagation between the previous node and this. node that the reference
  // of the previous node is used.
  StatusCode sc = StatusCode::SUCCESS ;

  LHCb::KalmanFitResult* fitresult = static_cast<LHCb::KalmanFitResult*>(track.fitResult()) ;
  LHCb::KalmanFitResult::FitNodeRange nodes = fitresult->fitNodes() ;
  if( nodes.size()>1 ) {
    const ITrackExtrapolator* extrap = extrapolator(track.type()) ;
    auto inode = nodes.begin() ;
    const LHCb::StateVector* refvector = &((*inode)->refVector()) ;
    TrackMatrix F =  ROOT::Math::SMatrixIdentity() ;
    for(++inode; inode!=nodes.end() && sc.isSuccess() ; ++inode) {

      FitNode* node = *inode ;
      auto z = node->z() ;
      LHCb::StateVector statevector = *refvector ;
      StatusCode thissc = extrap -> propagate(statevector,z,&F) ;
      if ( thissc.isFailure() ) {
        std::stringstream msg ;
        msg << "Unable to propagate reference vector for track type: " << track.type() ;
        Info(msg.str(),StatusCode::SUCCESS).ignore() ;
        if( m_debugLevel )
          debug() << "unable to propagate reference vector from z=" << refvector->z()
                  << " to " << z
                  << "; track type = " << track.type()
                  << ": vec = " << refvector->parameters() << endmsg ;
        sc = thissc ;
      }

      // correct for energy loss
      auto dE = node->deltaEnergy() ;
      if ( std::abs(statevector.qOverP()) > LHCb::Math::lowTolerance ) {
        auto charge = statevector.qOverP() > 0 ? 1. :  -1. ;
        auto momnew = std::max( m_minMomentumForELossCorr,
                                std::abs(1/statevector.qOverP()) + dE ) ;
        if ( std::abs(momnew) > m_minMomentumForELossCorr )
          statevector.setQOverP(charge/momnew) ;
      }

      // calculate the 'transport vector' (need to replace that)
      Gaudi::TrackVector tranportvec = statevector.parameters() - F * refvector->parameters() ;
      node->setTransportMatrix( F );
      node->setTransportVector( tranportvec );

      // update the reference
      refvector = &((*inode)->refVector()) ;

      // test dtx/dqop to see if the momentum affects this track.
      if( std::abs(F(2,4)) != 0 ) hasMomentum = true ;
    }
  }

  if( m_useSeedStateErrors ) {
    // we need to do this until we can properly deal with the seed state
    fitresult->setNTrackParameters( 0 ) ;
  } else {
    fitresult->setNTrackParameters( hasMomentum ? 5 : 4 ) ;
  }

  if( m_debugLevel )
    debug() << "End of TrackMasterFitter::updateTransport" << endmsg ;
  return sc ;
}

StatusCode TrackMasterFitter::initializeRefStates(LHCb::Track& track,
                                                  LHCb::ParticleID pid ) const
{
  if( m_debugLevel )
    debug() << "TrackMasterFitter::initializeRefStates" << endmsg ;

  // given existing states on the track, this tool adds states at fixed
  // z-positions along the track. if a track state already exists
  // sufficiently close to the desired state, it will not add the
  // state.
  StatusCode sc = StatusCode::SUCCESS ;

  // first fix the momentum of states on the track. need to make sure this works for Velo-TT as well.
  if( track.states().empty() ) {
    sc = Error( "Track has no state! Can not fit.", StatusCode::FAILURE );
  } else {
    // first need to make sure all states already on track have
    // reasonable momentum. still needs to check that this works for
    // velo-TT
    const LHCb::State* stateAtT = track.stateAt(LHCb::State::AtT) ;
    const LHCb::State& refstate = stateAtT ? *stateAtT :
      *( track.checkFlag(Track::Backward) ? track.states().front() : track.states().back()) ;
    for( auto* state : track.states() )
      const_cast<LHCb::State*>(state)->setQOverP( refstate.qOverP() ) ;


    // collect the z-positions where we want the states
    boost::container::static_vector<double,4> zpositions ;
    if( track.hasT() ) {
      zpositions.push_back( StateParameters::ZBegT) ;
      zpositions.push_back( StateParameters::ZEndT ) ;
    }
    if( track.hasTT() || (track.hasT() && track.hasVelo() ) )
      zpositions.push_back(StateParameters::ZEndTT) ;
    if( track.hasVelo() )
      zpositions.push_back(StateParameters::ZEndVelo) ;

    // the following container is going to hold pairs of 'desired'
    // z-positionds and actual states. the reason for the gymnastics
    // is that we always want to propagate from the closest availlable
    // state, but then recursively. this will make the parabolic
    // approximation reasonably accurate.
    typedef std::pair<double, const LHCb::State*> ZPosWithState ;
    std::vector< ZPosWithState > states ; states.reserve(track.states().size());
    // we first add the states we already have
    std::transform( track.states().begin(), track.states().end(),
                    std::back_inserter(states),
                    [](const LHCb::State* s) { return std::make_pair(s->z(),s); } );

    // now add the other z-positions, provided nothing close exists
    const double maxDistance = 50*Gaudi::Units::cm ;
    for( auto z : zpositions ) {
      bool not_found = std::none_of( states.begin(), states.end(),
                                     [&](const ZPosWithState& s) {
          return std::abs(z - s.first) < maxDistance;
      });
      if(not_found) states.emplace_back( z,nullptr ) ;
    }
    std::sort( states.begin(), states.end(), FirstLess ) ;

    // create the states in between
    const ITrackExtrapolator* extrap = extrapolator( track.type() ) ;
    LHCb::Track::StateContainer newstates ;
    for( auto it = states.begin(); it != states.end() && sc.isSuccess() ; ++it) {
      if( it->second ) continue;

      // find the nearest existing state to it
      auto best= states.end() ;
      for( auto jt = states.begin(); jt != states.end() ; ++jt)
        if( it != jt && jt->second
            && ( best==states.end() || std::abs( jt->first - it->first) < std::abs( best->first - it->first) ) )
          best = jt ;

      assert( best != states.end() ) ;

      // move from that state to this iterator, using the extrapolator and filling all states in between.
      int direction = best > it ? -1 : +1 ;
      LHCb::StateVector statevec( best->second->stateVector(), best->second->z() ) ;
      for( auto jt = best+direction ; jt != it+direction && sc.isSuccess() ; jt += direction) {
        StatusCode thissc = extrap->propagate( statevec, jt->first, 0, pid ) ;
        if( !thissc.isSuccess() ) {
          Warning("initializeRefStates() fails in propagating state",StatusCode::FAILURE).ignore() ;
          if( m_debugLevel )
            debug() << "Problem propagating state: " << statevec << " to z= " << jt->first << endmsg ;
          sc = thissc ;
        } else {
          newstates.push_back( new LHCb::State( statevec ) );
          jt->second = newstates.back() ;
        }
      }

    }

    // finally, copy the new states to the track.
    if( sc.isSuccess() ) {
      track.addToStates( newstates ) ;
    } else {
      for( LHCb::State* state: newstates) delete state ;
    }
  }
  return sc ;
}
