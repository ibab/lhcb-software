// $Id: TrackMasterFitter.cpp,v 1.74 2009-10-13 14:34:21 wouter Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// from LHCb
#include "LHCbMath/LHCbMath.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackManipulator.h"
#include "TrackInterfaces/IMaterialLocator.h"
#include "TrackInterfaces/ITrackExtrapolator.h"            
#include "TrackInterfaces/ITrackKalmanFilter.h"
#include "TrackInterfaces/IMeasurementProvider.h"

// from TrackEvent
#include "Event/TrackFunctor.h"
#include "Event/StateParameters.h"
#include "Event/Track.h"
#include "Event/TrackFitResult.h"
#include "Event/FitNode.h"
#include "Event/State.h"
#include "TrackKernel/TrackTraj.h"

// local
#include "TrackMasterFitter.h"

// gsl
#include "gsl/gsl_math.h"

//stl
#include <algorithm>

// boost
#include <boost/assign/list_of.hpp> // for 'vector_list_of()'

using namespace Gaudi;
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TrackMasterFitter
//
//  Original Author: Rutger van der Eijk
//  Created: 07-04-1999
//  Adapted: 21-03-2002  Olivier Callot
//  Adapted: 15-04-2005  Jose A. Hernando, Eduardo Rodrigues
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TrackMasterFitter );

//=========================================================================
// Standard Constructor, initializes variables
//=========================================================================
TrackMasterFitter::TrackMasterFitter( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent)
  : GaudiTool( type, name, parent)
  , m_extrapolator(0)
  , m_veloExtrapolator(0)
  , m_trackNodeFitter(0)
  , m_measProvider(0)
  , m_materialLocator(0)
{
  declareInterface<ITrackFitter>( this );

  declareProperty( "Extrapolator"        , m_extrapolatorName =
                   "TrackMasterExtrapolator" );
  declareProperty( "VeloExtrapolator"    , m_veloExtrapolatorName =
                   "TrackLinearExtrapolator" );
  declareProperty( "FitUpstream"         , m_upstream         = true        );
  declareProperty( "NumberFitIterations" , m_numFitIter       = 3           );
  declareProperty( "Chi2Outliers"        , m_chi2Outliers     = 9.0         );
  declareProperty( "MaxNumberOutliers"   , m_numOutlierIter   = 2           );
  declareProperty( "StateAtBeamLine"     , m_stateAtBeamLine  = true        );
  declareProperty( "AddDefaultReferenceNodes" , m_addDefaultRefNodes = true ) ;
  
  declareProperty( "UseSeedStateErrors", m_useSeedStateErrors = false );

  declareProperty( "ErrorX"         , m_errorX  = 20.0*Gaudi::Units::mm );
  declareProperty( "ErrorY"         , m_errorY  = 20.0*Gaudi::Units::mm );
  declareProperty( "ErrorTx"        , m_errorTx = 0.1                    );
  declareProperty( "ErrorTy"        , m_errorTy = 0.1                    );
  declareProperty( "ErrorQoP"       , m_errorQoP = boost::assign::list_of(0.0)(0.01) );
  declareProperty( "MakeNodes"      , m_makeNodes = false                   );
  declareProperty( "MakeMeasurements", m_makeMeasurements = false           );
  declareProperty( "MaterialLocator", m_materialLocatorName = "DetailedMaterialLocator");
  declareProperty( "UpdateTransport", m_updateTransport = false             );
  declareProperty( "MinMomentumForTransport", m_minMomentumForELossCorr = 10.*Gaudi::Units::MeV );
  declareProperty( "ApplyMaterialCorrections", m_applyMaterialCorrections = true );
  declareProperty( "ApplyEnergyLossCorr", m_applyEnergyLossCorrections = true ) ;
  declareProperty( "TransverseMomentumForScattering", m_scatteringPt = 400.*Gaudi::Units::MeV );
  declareProperty( "MaxMomentumForScattering", m_maxMomentumForScattering = 500.*Gaudi::Units::GeV );
  declareProperty( "MinNumVeloRHitsForOutlierRemoval",   m_minNumVeloRHits   = 2 ) ;
  declareProperty( "MinNumVeloPhiHitsForOutlierRemoval", m_minNumVeloPhiHits = 2 ) ;
  declareProperty( "MinNumTTHitsForOutlierRemoval",      m_minNumTTHits      = 2 ) ;
  declareProperty( "MinNumTHitsForOutlierRemoval",       m_minNumTHits       = 4 ) ;
  declareProperty( "MinNumMuonHitsForOutlierRemoval",    m_minNumMuonHits    = 4 ) ;
  declareProperty( "MaxDeltaChiSqConverged",             m_maxDeltaChi2Converged = -1 ) ;
}

//=========================================================================
// Destructor
//=========================================================================
TrackMasterFitter::~TrackMasterFitter() {
}

//=========================================================================
// Initialize
//=========================================================================
StatusCode TrackMasterFitter::finalize() 
{
  return GaudiTool::finalize() ;
}

//=========================================================================
// Initialize
//=========================================================================
StatusCode TrackMasterFitter::initialize() 
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;

  m_extrapolator      = tool<ITrackExtrapolator>( m_extrapolatorName, "Extrapolator",this );
  m_veloExtrapolator  = tool<ITrackExtrapolator>( m_veloExtrapolatorName, "VeloExtrapolator",this );
  m_trackNodeFitter   = tool<ITrackKalmanFilter>( "TrackKalmanFilter", "NodeFitter", this ) ;
  m_measProvider      = tool<IMeasurementProvider>( "MeasurementProvider","MeasProvider", this );
  m_materialLocator   = tool<IMaterialLocator>(m_materialLocatorName, "MaterialLocator", this) ;
  
  m_debugLevel   = msgLevel( MSG::DEBUG ) || msgLevel( MSG::VERBOSE ) ;  

  info() << " " << endmsg
         << "============ TrackMasterFitter Settings ===========" << endmsg
         << ((m_upstream) ? " Upstream fit" : " Downstream fit") << endmsg 
         << " Number of fit iterations: " << m_numFitIter << endmsg
         << " Max " << m_numOutlierIter << " outliers removed with outliers"
         << " at chi2 > " << m_chi2Outliers << endmsg
         << " State z positions at: " << endmsg
         << ((m_stateAtBeamLine) ? " beam line," : "") << " first/last measurement"
	 << (m_addDefaultRefNodes ? ", default reference positions" : "" ) ;
  info() << endmsg
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
// Fit the track
//=========================================================================
StatusCode TrackMasterFitter::fit( Track& track, LHCb::ParticleID pid )
{
  // any track that doesnt make it to the end is failed
  track.setFitStatus( Track::FitFailed );
  
  // create the TrackFit if it doesn't exist yet
  if( track.fitResult()==0 ) { track.setFitResult( new LHCb::TrackFitResult() ) ; }
  
  // Make the nodes from the measurements
  StatusCode sc;
  LHCb::TrackFitResult::NodeContainer& nodes = track.fitResult()->nodes() ;
  if( nodes.empty() || m_makeNodes ) {
    sc = makeNodes( track,pid );
    if ( sc.isFailure() )
      return failure( "unable to make nodes from the measurements" );
  } else {
    sc = updateRefVectors( track ) ;
    if ( sc.isFailure() )
      return failure( "unable to update the ref vectors" );
  }
  
  // create a covariance matrix to seed the Kalman fit
  State& seed = nodes.front()->state() ;
  TrackSymMatrix seedCov ; // Set off-diagonal elements to zero
  if (m_useSeedStateErrors) {
    State state0 = track.firstState();
    double z1 = nodes.front()->state().z();
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
    seedCov(4,4) = gsl_pow_2( m_errorQoP[0] * seed.qOverP() ) + gsl_pow_2(m_errorQoP[1]);   
  }
  
  if ( m_debugLevel )
    debug() << "SeedState: z = " << seed.z()
	    << " stateVector = " << seed.stateVector()
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
      sc = updateRefVectors( track );
      if ( sc.isFailure() ) return failure( "problem updating ref vectors" );
    }
    
    // reset the covariance of the first state
    seed.covariance() = seedCov ;
    
    double prevchi2 = track.chi2() ;
    sc = m_trackNodeFitter -> fit( track );
    
    if ( sc.isFailure() ) return failure( "unable to fit the track" );
    
    if ( m_debugLevel ) debug() << "chi2 =  " << track.chi2() 
                                << " ref state = (" << nodes.back()->state().stateVector() 
                                << ") at z= " << nodes.back()->state().z() << endmsg;
    double dchi2 = prevchi2 - track.chi2() ;
    converged = iter>1 && std::abs(dchi2) < m_maxDeltaChi2Converged * track.nDoF();
    
  }
  track.fitResult()->setNIter( iter ) ;
  
  // Outlier removal iterations
  iter = 1;
  while ( iter <= m_numOutlierIter && track.nDoF() > 1 &&
          outlierRemoved( track ) ) {
    if ( m_debugLevel ) debug() << "Outlier iteration # " << iter << endmsg;
    
    // update reference trajectories with smoothed states
    sc = updateRefVectors( track );
    if ( sc.isFailure() ) return failure( "problem updating ref vectors" );
    
    // reset the covariance of the first state
    seed.covariance() = seedCov ;
    
    // Call the track fit
    sc = m_trackNodeFitter -> fit( track );
    if ( sc.isFailure() ) {
      // Should only be used if the track belongs to a container and therefore has a key!
      //std::ostringstream mess;
      //mess << "unable to fit the track # " << track.key();
      //return failure( mess.str() );
      return failure( "unable to fit the track" );
    } 
    
    if ( m_debugLevel ) debug() << "chi2 =  " << track.chi2() 
                                << " ref state = (" << nodes.back()->state().stateVector() 
                                << ") at z= " << nodes.back()->state().z() << endmsg;
    ++iter;  
  }
  
  // determine the track states at user defined z positions
  sc = determineStates( track );
  if ( sc.isFailure() ) 
    return failure( "failed in determining states" ) ;

  if ( m_debugLevel && !track.states().empty() )
    debug() << "first state = " << track.firstState() << endmsg;

  // fill extra info
  fillExtraInfo( track ) ;

  // make sure to declare the track successful
  track.setFitStatus( Track::Fitted );

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
  LHCb::Node *firstMeasurementNode(0), *lastMeasurementNode(0) ;
  for( LHCb::TrackFitResult::NodeContainer::iterator inode = nodes.begin() ;
       inode != nodes.end() && !firstMeasurementNode; ++inode ) 
    if( (*inode)->hasMeasurement() ) firstMeasurementNode = *inode ;
  for( LHCb::TrackFitResult::NodeContainer::reverse_iterator inode = nodes.rbegin() ;
       inode != nodes.rend() && !lastMeasurementNode; ++inode ) 
    if( (*inode)->hasMeasurement() ) lastMeasurementNode = *inode ;
  if ( ( m_upstream && !track.checkFlag(Track::Backward ) ) ||
       ( !m_upstream && track.checkFlag(Track::Backward ))) 
    std::swap( lastMeasurementNode, firstMeasurementNode ) ;
  firstMeasurementNode->state().setLocation( State::FirstMeasurement );
  track.addToStates(firstMeasurementNode->state()) ;

  lastMeasurementNode->state().setLocation( State::LastMeasurement );
  track.addToStates(lastMeasurementNode->state()) ;

  // Add the states at the reference positions
  // ------------------------------------------
  for( LHCb::TrackFitResult::NodeContainer::const_iterator inode = nodes.begin() ;
       inode != nodes.end(); ++inode )
    if( (*inode)->type() == LHCb::Node::Reference )
      track.addToStates( (*inode)->state() ) ;

  if ( m_debugLevel ) {
    debug() << "Track " << track.key() << " has " << track.nStates() 
            << " states after fit\n  at z = " ;
    const std::vector<State*>& allstates = track.states();
    for ( unsigned int it2 = 0; it2 < allstates.size(); it2++ ) {
      debug() << allstates[it2]->z() << ", " ;
    }
    debug() << track.fitResult()->nActiveMeasurements()
            << " measurements used for the fit (out of " 
            << track.nLHCbIDs() << ")." << endmsg;
  }
  
  return sc;
}

//=========================================================================
//
//=========================================================================
bool TrackMasterFitter::outlierRemoved( Track& track ) const
{
  // return true if outlier chi2 cut < 0
  if ( m_chi2Outliers < 0.0 ) return true;

  // flag (true if outliers are removed)
  bool outlierWasRemoved = false;
  
  // Count the number of hits of each type
  enum HitType {VeloR, VeloPhi, TT, T, Muon, Unknown} ;
  static HitType hittypemap[11] = { Unknown, VeloR, VeloPhi, VeloR, VeloPhi, TT, T, T, Muon, TT, T } ;
  const size_t minNumHits[5] = { m_minNumVeloRHits, m_minNumVeloPhiHits, m_minNumTTHits, m_minNumTHits, m_minNumMuonHits } ;
  size_t numHits[5]          = {0,0,0,0,0} ;
  std::vector<Node*>& nodes = track.fitResult()->nodes();
  for( LHCb::TrackFitResult::NodeContainer::const_iterator inode =  nodes.begin() ;
       inode != nodes.end() ; ++inode) 
    if( (*inode)->type() == LHCb::Node::HitOnTrack ) 
      ++(numHits[hittypemap[int((*inode)->measurement().type())]]) ;
  
  // loop over the nodes and find the one with the highest chi2 >
  // m_chi2Outliers, provided there is enough hits of this type left.
  std::vector<Node*>::iterator iNode;
  std::vector<Node*>::iterator iWorstNode = nodes.end();
  double worstChi2 = m_chi2Outliers;
  for ( iNode = nodes.begin(); iNode != nodes.end(); ++iNode )
    if ( (*iNode)->hasMeasurement() &&
	 (*iNode)->type() == LHCb::Node::HitOnTrack ) {
      const double chi2 = (*iNode)->chi2();
      HitType hittype = hittypemap[int((*iNode)->measurement().type())] ;
      if ( chi2 > worstChi2 &&
	   numHits[hittype] > minNumHits[hittype] ) {
	worstChi2 = chi2;
	iWorstNode = iNode;
      }
    }
  
  // if a node is found: remove its measurement from the track
  if ( iWorstNode != nodes.end() ) {

    if (m_debugLevel)
      debug() << "Measurement " << iWorstNode-nodes.begin() 
              << " of type="
              << (*iWorstNode) -> measurement().type()
              << " LHCbID="
              << (*iWorstNode) -> measurement().lhcbID().channelID()
              << " at z=" << (*iWorstNode)->z() 
              << " with chi2=" << (*iWorstNode) -> chi2() 
              << " removed." << endmsg;

    // Remove measurement from node (node still exists w/o measurement)
    // One also needs to delete the measurement from the track!
    (*iWorstNode)->setType( LHCb::Node::Outlier ) ;
    outlierWasRemoved = true;
  }

  return outlierWasRemoved;
}

//=========================================================================
// Update the measurements before a refit
//=========================================================================
StatusCode TrackMasterFitter::updateRefVectors( Track& track ) const
{ 
  StatusCode sc = StatusCode::SUCCESS ;
  std::vector<Node*>& nodes = track.fitResult()->nodes();
  for (TrackFitResult::NodeContainer::iterator iNode = nodes.begin(); 
       iNode != nodes.end(); ++iNode ) 
    (*iNode)->setRefVector( (*iNode)->state().stateVector() ) ; 
  if(m_updateTransport) sc =updateTransport(track) ;
  return sc ;
}

//=========================================================================
// Determine the z-position of the closest approach to the beam line
//  by linear extrapolation.
//=========================================================================
double TrackMasterFitter::closestToBeamLine( const State& state ) const
{
  const TrackVector& vec = state.stateVector();
  double z = state.z();
  // check on division by zero (track parallel to beam line!)
  if ( vec[2] != 0 || vec[3] != 0 ) {
    z -= ( vec[0]*vec[2] + vec[1]*vec[3] ) / ( vec[2]*vec[2] + vec[3]*vec[3] );
  } 
  // don't go outside the sensible volume
  return std::min(std::max(z,-100*Gaudi::Units::cm), StateParameters::ZBegRich2 ) ;
}

//=============================================================================
// Create the nodes from the measurements
//=============================================================================
StatusCode TrackMasterFitter::makeNodes( Track& track, LHCb::ParticleID pid ) const 
{
  // Clear the nodes
  LHCb::TrackFitResult& fitresult = *(track.fitResult()) ;
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
    StatusCode sc = m_measProvider -> load( track );
    if ( sc.isFailure() )
      return Error( "Unable to load measurements!", StatusCode::FAILURE );
    track.setPatRecStatus( Track::PatRecMeas );
    if(m_debugLevel)
      debug() << "# LHCbIDs, Measurements = " << track.nLHCbIDs()
	      << ", " << fitresult.nMeasurements() << endmsg;
  }

  // Create the nodes for the measurements.
  const std::vector<Measurement*>& measures = fitresult.measurements();
  TrackFitResult::NodeContainer& nodes = fitresult.nodes();
  nodes.reserve( measures.size() + 6 );
  for ( std::vector<Measurement*>::const_reverse_iterator it =
          measures.rbegin(); it != measures.rend(); ++it ) {
    FitNode* node = new FitNode( **it );
    nodes.push_back( node );
  }

  // Add reference nodes depending on track type
  if( m_addDefaultRefNodes ) {
    if(track.hasVelo() && ( track.hasT() ||track.hasTT()) )
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
  if ( m_stateAtBeamLine ) {
    const LHCb::State& refstate = *(track.checkFlag(Track::Backward) ? track.states().back() : track.states().front()) ;
    FitNode* node = new FitNode( closestToBeamLine(refstate) );
    node->state().setLocation( State::ClosestToBeam );
    nodes.push_back( node ) ;
  }

  // Sort the nodes in z
  if ( m_upstream ) {
    std::stable_sort( nodes.begin(), nodes.end(), TrackFunctor::decreasingByZ<Node>());
  } else {
    std::stable_sort( nodes.begin(), nodes.end(), TrackFunctor::increasingByZ<Node>());
  }

  // Set the reference using a TrackTraj
  LHCb::TrackTraj::StateContainer states ;
  states.insert( states.end(), track.states().begin(), track.states().end() ) ;
  LHCb::TrackTraj tracktraj(states) ;
  for( TrackFitResult::NodeContainer::const_iterator it = nodes.begin() ;
       it != nodes.end(); ++it ) {
    LHCb::StateVector ref = tracktraj.stateVector((*it)->z())  ;
    (*it)->setRefVector( ref ) ;
    (*it)->state().setState(ref) ;
  }
  
  // add all the noise, if required
  if(m_applyMaterialCorrections && sc.isSuccess()) {
    sc = updateMaterialCorrections( track, pid ) ;
  }
  
  // create the transport of the reference (after the noise, because it uses energy loss)
  if( sc.isSuccess()) {
    sc = updateTransport( track ) ;
  }

  return sc ;
}

//=========================================================================

void TrackMasterFitter::fillExtraInfo(Track& track ) const 
{
  // This routine calculates the chisquare contributions from
  // different segments of the track. It uses the 'delta-chisquare'
  // contributions from the bi-directional kalman fit. Summing these
  // leads to a real chisquare only if the contributions are
  // uncorrelated. For a Velo-TT-T track you can then calculate:
  //
  // - the chisuare of the T segment and the T-TT segment by using the
  // 'upstream' contributions 
  //
  // - the chisquare of the Velo segment and the Velo-TT segment by
  // using the 'downstream' contributions
  //
  // Note that you cannot calculate the contribution of the TT segment
  // seperately (unless there are no T or no Velo hits). Also, if
  // there are Muon hits, you cannot calculate the T station part, so
  // for now this only works for tracks without muon hits.

  // Clean up the track info
  track.eraseInfo( Track::FitVeloChi2 ) ;
  track.eraseInfo( Track::FitVeloNDoF ) ;
  track.eraseInfo( Track::FitTChi2 ) ;
  track.eraseInfo( Track::FitTNDoF ) ;
  track.eraseInfo( Track::FitMatchChi2 ) ;
  
  // Compute the chisquare integrals for forward and backward
  double chisqT[2]    = {0,0} ;
  double chisqTT[2]   = {0,0} ;
  double chisqVelo[2] = {0,0} ;
  int    nhitsT(0), nhitsTT(0), nhitsVelo(0) ;
  const LHCb::TrackFitResult::NodeContainer& nodes = track.fitResult()->nodes()  ;
  for( std::vector<Node*>::const_iterator iNode = nodes.begin(); 
       iNode != nodes.end(); ++iNode ) 
    if( (*iNode)->hasMeasurement() &&
	(*iNode)->type() != LHCb::Node::Outlier ) {
      const FitNode* node = dynamic_cast<FitNode*>(*iNode) ;
      if(!node) {
	error() << "fillExtraInfo: node is not a FitNode" << endmsg ;
      } else {
	switch( node->measurement().type() ) {
	case Measurement::VeloR:
	case Measurement::VeloPhi:
	case Measurement::VeloLiteR:
	case Measurement::VeloLitePhi:
	  chisqVelo[0] += node->deltaChi2Forward() ;
	  chisqVelo[1] += node->deltaChi2Backward() ;
	  ++nhitsVelo ;
	  break;
	case Measurement::TT:
	  chisqTT[0] += node->deltaChi2Forward() ;
	  chisqTT[1] += node->deltaChi2Backward() ;
	  ++nhitsTT ;
	  break;
	case Measurement::OT:
	case Measurement::IT:
	  chisqT[0] += node->deltaChi2Forward() ;
	  chisqT[1] += node->deltaChi2Backward() ;
	  ++nhitsT ;
	  break;
	default:
	  break;
	}
      }
    }

  const int nPar = track.nStates()>0 ? track.firstState().nParameters() : 5 ;
  bool upstream = track.nodes().front()->z() > track.nodes().back()->z() ;
  
  if( track.hasT() ) {
    track.addInfo( Track::FitTChi2 , upstream ? chisqT[0] : chisqT[1]) ;
    track.addInfo( Track::FitTNDoF , nhitsT - nPar ) ;
  }
  
  if( track.hasVelo() ) {
    track.addInfo( Track::FitVeloChi2, upstream ? chisqVelo[1] : chisqVelo[0] ) ;
    track.addInfo( Track::FitVeloNDoF, nhitsVelo - nPar ) ;
    if( track.hasT() ) {
      // Calculate the chisquare of the breakpoint between TT and T
      double thischisqT       = upstream ? chisqT[0] : chisqT[1] ;
      double thischisqVeloTT  = upstream ? chisqVelo[1] + chisqTT[1] : chisqVelo[0] + chisqTT[0];
      // they should be equal, but this is safer
      double chisqTot   = std::max( chisqT[0]+chisqTT[0]+chisqVelo[0], chisqT[1]+chisqTT[1]+chisqVelo[1] ) ;
      double chisqMatch = chisqTot - thischisqT - thischisqVeloTT ;
      track.addInfo( Track::FitMatchChi2, chisqMatch ) ;
    }
  }
}


StatusCode TrackMasterFitter::updateMaterialCorrections(LHCb::Track& track, LHCb::ParticleID pid) const
{
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
    double scatteringMomentum = track.firstState().p() ;
    if( !track.hasT() && !track.hasTT()) {
      double tx     = track.firstState().tx() ;
      double ty     = track.firstState().ty() ;
      double slope2 = tx*tx+ty*ty ;
      double tanth  = std::max(sqrt( slope2/(1+slope2)),1e-4) ;
      scatteringMomentum = m_scatteringPt/tanth ;
    }

    scatteringMomentum = std::min( scatteringMomentum, m_maxMomentumForScattering );
    fitresult.setPScatter( scatteringMomentum ) ;

    LHCb::TrackTraj tracktraj( nodes ) ;
    IMaterialLocator::Intersections intersections ;
    m_materialLocator->intersect( tracktraj, intersections ) ;
    
    // now we need to redistribute the result between the nodes. the first node cannot have any noise.
    LHCb::TrackFitResult::NodeContainer::iterator inode = nodes.begin() ;
    double zorigin((*inode)->z()) ;
    for(++inode; inode!=nodes.end(); ++inode) {
      FitNode* node = dynamic_cast<FitNode*>(*inode) ;
      double ztarget = node->z() ;
      Gaudi::TrackSymMatrix noise ;
      Gaudi::TrackVector delta;
      m_materialLocator->computeMaterialCorrection(noise,delta,intersections,zorigin,ztarget,
                                                   scatteringMomentum, pid) ;
      node->setNoiseMatrix( noise ) ;
      node->setDeltaEnergy( 0 ) ;
      if( applyenergyloss ) {
	double deltaE = 1 / ( 1/scatteringMomentum + delta(4) ) - scatteringMomentum ;
	node->setDeltaEnergy( deltaE ) ;
      }
      zorigin = ztarget ;
    }
    
  }
  return StatusCode::SUCCESS ;
}

StatusCode TrackMasterFitter::updateTransport(LHCb::Track& track) const
{
  // sets the propagation between the previous node and this. node that the reference 
  // of the previous node is used.
  StatusCode sc = StatusCode::SUCCESS ;
  
  LHCb::TrackFitResult::NodeContainer& nodes = track.fitResult()->nodes() ;
  if( nodes.size()>1 ) {
    ITrackExtrapolator* extrap = extrapolator(track.type()) ;
    LHCb::TrackFitResult::NodeContainer::iterator inode = nodes.begin() ;
    const LHCb::StateVector* refvector = &((*inode)->refVector()) ;
    TrackMatrix F = TrackMatrix( ROOT::Math::SMatrixIdentity() );
    for(++inode; inode!=nodes.end(); ++inode) {
      
      FitNode* node = dynamic_cast<FitNode*>(*inode) ;
      double z = node->z() ;
      LHCb::StateVector statevector = *refvector ;
      StatusCode thissc = extrap -> propagate(statevector,z,&F) ;
      if ( thissc.isFailure() ) {
        error() << "unable to propagate reference vector from z=" << refvector->z() 
		<< " to " << z 
		<< "; track type = " << track.type()
		<< ": vec = " << refvector->parameters() << endmsg ;
        sc = thissc ;
      }
      
      // correct for energy loss
      double dE = node->deltaEnergy() ;
      if ( std::abs(statevector.qOverP()) > LHCb::Math::lowTolerance ) {
	double charge = statevector.qOverP() > 0 ? 1 :  -1 ;
	double momnew = std::max( m_minMomentumForELossCorr,
				  fabs(1/statevector.qOverP()) + dE ) ;  
	if ( fabs(momnew) > m_minMomentumForELossCorr )
	  statevector.setQOverP(charge/momnew) ;
      }

      // calculate the 'transport vector' (need to replace that)
      Gaudi::TrackVector tranportvec = statevector.parameters() - F * refvector->parameters() ;
      node->setTransportMatrix( F );
      node->setTransportVector( tranportvec );
      node->setTransportIsSet( true );
      
      // update the reference
      refvector = &((*inode)->refVector()) ;
    }
  }
  return sc ;
}

template<class T>
inline bool LessThanFirst(const T& lhs, const T& rhs)
{
  return lhs.first < rhs.first ;
}

StatusCode TrackMasterFitter::initializeRefStates(LHCb::Track& track,
						  LHCb::ParticleID pid ) const
{
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
    for( LHCb::Track::StateContainer::const_iterator it = track.states().begin() ;
         it != track.states().end() ; ++it)
      const_cast<LHCb::State*>(*it)->setQOverP( refstate.qOverP() ) ;
    
    
    // collect the z-positions where we want the states
    std::vector<double> zpositions ;
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
    typedef std::vector< ZPosWithState > ZPosWithStateContainer ;
    std::vector< ZPosWithState > states ;
    // we first add the states we already have
    for( std::vector<LHCb::State*>::const_iterator it = track.states().begin() ;
         it != track.states().end() ; ++it) 
      states.push_back( ZPosWithState((*it)->z(),(*it)) ) ;

    // now add the other z-positions, provided nothing close exists
    const double maxDistance = 50*Gaudi::Units::cm ;
    for( std::vector<double>::iterator iz = zpositions.begin() ;
         iz != zpositions.end(); ++iz) {
      bool found = false ;
      for( ZPosWithStateContainer::const_iterator it = states.begin() ;
           it != states.end()&&!found ; ++it)
        found = fabs( *iz - it->first ) < maxDistance ;
      if(!found) states.push_back( ZPosWithState(*iz,0) ) ;
    }
    std::sort( states.begin(), states.end(), LessThanFirst<ZPosWithState> ) ;
    
    // create the states in between
    ITrackExtrapolator* extrap = extrapolator( track.type() ) ;
    LHCb::Track::StateContainer newstates ;
    for( ZPosWithStateContainer::iterator it = states.begin();
         it != states.end() ; ++it) 
      if( it->second == 0 ) {
        // find the nearest existing state to it
        ZPosWithStateContainer::iterator best= states.end() ;
        for( ZPosWithStateContainer::iterator jt = states.begin();
             jt != states.end() ; ++jt) 
          if( it != jt && jt->second
              && ( best==states.end() || fabs( jt->first - it->first) < fabs( best->first - it->first) ) )
            best = jt ;
        
        assert( best != states.end() ) ;
        
        // move from that state to this iterator, using the extrapolator and filling all states in between.
        int direction = best > it ? -1 : +1 ;
        LHCb::StateVector statevec( best->second->stateVector(), best->second->z() ) ;
        for( ZPosWithStateContainer::iterator jt = best+direction ;
             jt != it+direction ; jt += direction) {
          StatusCode thissc = extrap->propagate( statevec, jt->first, 0, pid ) ;
          LHCb::State* newstate = new LHCb::State( statevec ) ;
          jt->second = newstate ;
          newstates.push_back( newstate ) ;
          if( !thissc.isSuccess() ) {
            error() << "Problem propagating state in LongTrackReferenceCreator::createReferenceStates" << endmsg ;
            sc = thissc ;
          }
        }
      }
    
    // finally, copy the new states to the track. 
    track.addToStates( newstates ) ;
  }
  return sc ;
}
