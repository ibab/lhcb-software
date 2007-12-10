// $Id: TrackMasterFitter.cpp,v 1.42 2007-12-10 16:07:58 wouter Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// from TrackInterfaces
#include "TrackInterfaces/ITrackManipulator.h"
#include "TrackInterfaces/IMaterialLocator.h"
#include "TrackInterfaces/ITrackExtrapolator.h"            
#include "TrackInterfaces/ITrackFitter.h"
#include "TrackInterfaces/IMeasurementProvider.h"

// from TrackEvent
#include "Event/TrackFunctor.h"
#include "Event/StateParameters.h"
#include "Event/TrackTraj.h"
#include "Event/Track.h"
#include "Event/FitNode.h"
#include "Event/State.h"

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
  , m_trackNodeFitter(0)
  , m_measProvider(0)
  , m_refStateTool(0)
  , m_materialLocator(0)
{
  declareInterface<ITrackFitter>( this );

  declareProperty( "Extrapolator"        , m_extrapolatorName =
                   "TrackMasterExtrapolator" );
  declareProperty( "FitUpstream"         , m_upstream         =   true    );
  declareProperty( "NumberFitIterations" , m_numFitIter       =     3     );
  declareProperty( "Chi2Outliers"        , m_chi2Outliers     =     9.0   );
  declareProperty( "MaxNumberOutliers"   , m_numOutlierIter   =     2     );
  declareProperty( "StateAtBeamLine"     , m_stateAtBeamLine  =   true    );
  declareProperty( "ZPositions"          , 
                   m_zPositions = boost::assign::list_of(StateParameters::ZBegRich1)
                                                        (StateParameters::ZEndRich1)
                                                        (StateParameters::ZBegRich2)
                                                        (StateParameters::ZEndRich2));
  declareProperty( "ErrorX2"        , m_errorX2 = 4.0*Gaudi::Units::mm2   );
  declareProperty( "ErrorY2"        , m_errorY2 = 400.0*Gaudi::Units::mm2 );
  declareProperty( "ErrorTx2"       , m_errorTx2 = 6.e-5                  );
  declareProperty( "ErrorTy2"       , m_errorTy2 = 1.e-4                  );
  declareProperty( "ErrorP"         , m_errorP = boost::assign::list_of(0.15)(5.0e-7));
  declareProperty( "MakeNodes"      , m_makeNodes = true                  );
  declareProperty( "NodesAtAllZPositions",m_makeNodesAtAllReferencePositions=true) ; 
  declareProperty( "MaterialLocator", m_materialLocatorName="DetailedMaterialLocator") ;
  declareProperty( "UpdateTransport", m_updateTransport=false) ;
  declareProperty( "ApplyMaterialCorrections", m_applyMaterialCorrections=true) ;
  declareProperty( "ParticleID", m_particleID=211) ; 
  declareProperty( "TransverseMomentumForScattering", m_scatteringPt = 400.*Gaudi::Units::MeV);
}

//=========================================================================
// Destructor
//=========================================================================
TrackMasterFitter::~TrackMasterFitter() {
}

//=========================================================================
// Initialize
//=========================================================================
StatusCode TrackMasterFitter::initialize() 
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;
  
  m_extrapolator      = tool<ITrackExtrapolator>( m_extrapolatorName, "Extrapolator",this );
  m_trackNodeFitter   = tool<ITrackFitter>( "TrackKalmanFilter", "NodeFitter", this ) ;
  m_measProvider      = tool<IMeasurementProvider>( "MeasurementProvider","MeasProvider", this );
  m_refStateTool      = tool<ITrackManipulator>( "LongTrackReferenceCreator", "RefStateTool", this );
  m_materialLocator   = tool<IMaterialLocator>(m_materialLocatorName, "MaterialLocator", this) ;
  
  m_debugLevel   = msgLevel( MSG::DEBUG );  

  info() << " " << endmsg
         << "============ TrackMasterFitter Settings ===========" << endmsg
         << ((m_upstream) ? " Upstream fit" : " Downstream fit") << endmsg 
         << " Number of fit iterations: " << m_numFitIter << endmsg
         << " Max " << m_numOutlierIter << " outliers removed with outliers"
         << " at chi2 > " << m_chi2Outliers << endmsg
         << " State z positions at: " << endmsg
         << ((m_stateAtBeamLine) ? " beam line," : "") << " first measurement" ;
  std::vector<double>::const_iterator zPos;
  for ( zPos = m_zPositions.begin(); zPos != m_zPositions.end(); ++zPos ) {
    info()  << ", " << *zPos;
  }
  info() << endmsg
         << "==================================================" << endmsg;
  
  return StatusCode::SUCCESS;
}

//=========================================================================
// Helper to print a failure comment
//=========================================================================
StatusCode TrackMasterFitter::failure( const std::string& comment ) const {
  if ( m_debugLevel )
    debug() << "TrackMasterFitter failure: " << comment << endreq;
  return StatusCode::FAILURE;
}

//=========================================================================
// Fit the track
//=========================================================================
StatusCode TrackMasterFitter::fit( Track& track )
{
  StatusCode sc;
  if ( track.nStates() == 0 )
    return Error( "Track has no state! Can not fit.", StatusCode::FAILURE );
  
  // Make the nodes from the measurements
  if( track.nodes().empty() || m_makeNodes ) {
    sc = makeNodes( track );
    if ( sc.isFailure() )
      return failure( "unable to make nodes from the measurements" );
  }
  
  // Check that the number of measurements is enough
  State& seed = track.nodes().front()->state() ;
  if ( nNodesWithMeasurement( track ) < seed.nParameters() ) {
    debug() << "Track has " << track.nMeasurements() 
            << " measurements. Fitting a " << seed.nParameters() 
            << "D-state" << endmsg;
    return Warning( "Insufficient measurements to fit the State",
                    StatusCode::FAILURE, 1 );
  }

  // create a covariance matrix to seed the Kalman fit
  TrackSymMatrix seedCov ;
  seedCov = TrackSymMatrix(); // Set off-diagonal elements to zero
  seedCov(0,0) = m_errorX2;
  seedCov(1,1) = m_errorY2;
  seedCov(2,2) = m_errorTx2;
  seedCov(3,3) = m_errorTy2;
  // error is like A^2/p^2 + B^2
  seedCov(4,4) = gsl_pow_2( m_errorP[0] * seed.qOverP() ) + gsl_pow_2(m_errorP[1]);
  
  if ( m_debugLevel )
    debug() << "SeedState: z = " << seed.z()
            << " stateVector = " << seed.stateVector()
            << " covariance  = " << seed.covariance() << endmsg;

  // Iterate the track fit for linearisation
  int iter = 1;
  for ( ; iter <= m_numFitIter; ++iter ) {   
    if ( m_debugLevel ) debug() << "Iteration # " << iter << endmsg;

    // update reference trajectories with smoothed states
    if( iter > 1 ) {
      sc = updateRefVectors( track );
      if ( sc.isFailure() ) return failure( "problem updating ref vectors" );
    }

    // reset the covariance of the first state
    seed.covariance() = seedCov ;
    
    sc = m_trackNodeFitter -> fit( track );
    if ( sc.isFailure() ) return failure( "unable to fit the track" );
      
    if ( m_debugLevel ) debug() << "chi2 =  " << track.chi2() 
				<< " ref state = (" << track.nodes().back()->state().stateVector() 
				<< ") at z= " << track.nodes().back()->state().z() << endmsg;
  }
  
  // Outlier removal iterations
  iter = 1;
  while ( iter <= m_numOutlierIter &&
          nNodesWithMeasurement( track ) > seed.nParameters() &&
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
				<< " ref state = (" << track.nodes().back()->state().stateVector() 
				<< ") at z= " << track.nodes().back()->state().z() << endmsg;
    ++iter;  
  }

  // determine the track states at user defined z positions
  sc = determineStates( track );
  if ( sc.isFailure() )  {
    track.clearNodes() ;
    debug() << "fit failed" << endmsg ;
  } else {
    if ( m_debugLevel && !track.states().empty() )
      debug() << "first state = " << track.firstState() << endmsg;
    fillExtraInfo( track ) ;
  }
  
  return sc;
}

//=============================================================================
// 
//=============================================================================
StatusCode TrackMasterFitter::determineStates( Track& track ) const
{
  StatusCode sc = StatusCode::SUCCESS ;

  // clean the non-fitted states in the track!
  track.clearStates();
  
  // Add state closest to the beam-line
  // ----------------------------------
  if ( m_stateAtBeamLine ) {
    // Get the state closest to z=0. from the nodes
    State closeState = track.closestState( 0. );
    // Get the z-position of the "intersection" with the beam line
    double z = closestToBeamLine( closeState );    
    sc = m_extrapolator -> propagate( closeState , z );
    if ( sc.isFailure() ) {
      debug() << "Extrapolating to z = " << z << endmsg;
      Warning("State closest to beam line not added, extrapolation failed",StatusCode::FAILURE,1).ignore() ;
    } else {
      // add the state at the position closest to the beam line
      closeState.setLocation( State::ClosestToBeam );
      track.addToStates( closeState );
    }
  }

  std::vector<Node*>& nodes = track.nodes();

  // Add the state at the first measurement position
  // -----------------------------------------------
  if ( ( m_upstream && !track.checkFlag(Track::Backward ) ) ||
     ( !m_upstream && track.checkFlag(Track::Backward ))) {
    std::vector<Node*>::reverse_iterator iNode = nodes.rbegin();
    while ( !(*iNode)->hasMeasurement() ) ++iNode;
    State& state = (*iNode) -> state();
    state.setLocation( State::FirstMeasurement );
    track.addToStates( state );
  }
  else {
    std::vector<Node*>::iterator iNode = nodes.begin();
    while ( !(*iNode)->hasMeasurement() ) ++iNode;
    State& state = (*iNode) -> state();
    state.setLocation( State::FirstMeasurement );
    track.addToStates( state );
  }

  // Add the states at the predefined positions
  // ------------------------------------------
  for( std::vector<double>::const_iterator iz = m_zPositions.begin(); 
       iz != m_zPositions.end(); ++iz ) {
    double z = *iz ;
    // find the closest node
    std::vector<Node*>::const_iterator inode = nodes.begin();
    const Node* closestnode = *inode ;
    for (++inode;inode != nodes.end(); ++inode ) 
      if( fabs((*inode)->z() - z) < fabs(closestnode->z() - z) ) closestnode = *inode ;
    // copy the state
    LHCb::State state = closestnode->state() ; 
    // extrapolate if necessary
    StatusCode thissc = StatusCode::SUCCESS ;
    if( fabs(closestnode->z() - z) > TrackParameters::propagationTolerance ) {
      // this is wrong for interpolations. we need to work on that, 
      // but we anyway plan to remove filling states from the fit.
      thissc = m_extrapolator -> propagate(state,z);
      state.setLocation( State::LocationUnknown ) ;
    }
    if ( thissc.isFailure() ) {
      warning() << "Extrapolating to z = " << z << " failed. Will not add state at reference position" << endreq ;
      sc = StatusCode::FAILURE ;
    } else {
      // set the location (must find a better way to do this!)
      if ( fabs(z - StateParameters::ZBegRich1) < TrackParameters::lowTolerance )
        state.setLocation( State::BegRich1 );
      else if ( fabs(z - StateParameters::ZEndRich1 ) < TrackParameters::lowTolerance )
        state.setLocation( State::EndRich1 );
      else if ( fabs(z - StateParameters::ZBegRich2 ) < TrackParameters::lowTolerance )
        state.setLocation( State::BegRich2 );
      else if ( fabs(z - StateParameters::ZEndRich2 ) < TrackParameters::lowTolerance )
        state.setLocation( State::EndRich2 );
      // add to the track
      track.addToStates( state );
    }
  }

  if ( m_debugLevel ) {
    debug() << "Track " << track.key() << " has " << track.nStates() 
            << " states after fit\n  at z = " ;
    const std::vector<State*>& allstates = track.states();
    for ( unsigned int it2 = 0; it2 < allstates.size(); it2++ ) {
      debug() << allstates[it2]->z() << ", " ;
    }
    debug() << nNodesWithMeasurement( track ) 
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

  // loop over the nodes and find the one
  // with the highest chi2 > m_chi2Outliers
  std::vector<Node*>& nodes = track.nodes();
  std::vector<Node*>::iterator iNode;
  std::vector<Node*>::iterator iWorstNode = nodes.end();
  double worstChi2 = m_chi2Outliers;
  for ( iNode = nodes.begin(); iNode != nodes.end(); ++iNode ) {
    if ( (*iNode)->hasMeasurement() ) {
      const double chi2 = (*iNode)->chi2();
      if ( chi2 > worstChi2 ) {
        worstChi2 = chi2;
        iWorstNode = iNode;
      }
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
    Measurement& meas = (*iWorstNode) -> measurement();
    (*iWorstNode) -> removeMeasurement();
    track.removeFromMeasurements( &meas );

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
  for (Track::NodeContainer::iterator iNode = track.nodes().begin(); 
       iNode != track.nodes().end(); ++iNode ) 
    (*iNode)->setRefVector( (*iNode)->state().stateVector() ) ; 
  if(m_updateTransport) sc =updateTransport(track) ;
  return sc ;
}

//=========================================================================
// Determine the z-position of the closest approach to the beam line
//  by linear extrapolation.
//=========================================================================
double TrackMasterFitter::closestToBeamLine( State& state ) const
{
  TrackVector& vec = state.stateVector();
  double z = state.z();
  // check on division by zero (track parallel to beam line!)
  if ( vec[2] != 0 || vec[3] != 0 ) {
    z -= ( vec[0]*vec[2] + vec[1]*vec[3] ) / ( vec[2]*vec[2] + vec[3]*vec[3] );
  } 
  // don't go outside the sensible volume
  return std::min(std::max(z,-100*Gaudi::Units::cm), StateParameters::ZBegRich2 ) ;
}

//=========================================================================
// Retrieve the number of nodes with a measurement
//=========================================================================
unsigned int TrackMasterFitter::nNodesWithMeasurement( const Track& track ) 
  const 
{ 
  unsigned int nMeas = 0;
  const std::vector<Node*>& nodes = track.nodes();
  std::vector<Node*>::const_iterator iNode;
  for ( iNode = nodes.begin(); iNode != nodes.end(); ++iNode ) {    
    if ( (*iNode)->hasMeasurement() ) ++nMeas;
  }
  return nMeas;
}

//=============================================================================
// Create the nodes from the measurements
//=============================================================================
StatusCode TrackMasterFitter::makeNodes( Track& track ) const 
{
  // Clear the nodes
  track.clearNodes() ;

  // make sure the track has sufficient reference states
  StatusCode sc = m_refStateTool -> execute( track );
  if (sc.isFailure())
    return Warning("Problems setting reference info", StatusCode::FAILURE, 1);
  
  // Check if it is needed to populate the track with measurements
  if ( track.checkPatRecStatus( Track::PatRecIDs ) ) {
    StatusCode sc = m_measProvider -> load( track );
    if ( sc.isFailure() )
      return Error( "Unable to load measurements!", StatusCode::FAILURE );
    track.setPatRecStatus( Track::PatRecMeas );
    debug() << "# LHCbIDs, Measurements = " << track.nLHCbIDs()
            << ", " << track.nMeasurements() << endreq;
  }

  // Create the nodes for the measurements.
  const std::vector<Measurement*>& measures = track.measurements();
  Track::NodeContainer& nodes = track.nodes();
  nodes.reserve( measures.size() + m_zPositions.size() );
  double zmin(99999*Gaudi::Units::m),zmax(-99999*Gaudi::Units::m) ;
  for ( std::vector<Measurement*>::const_reverse_iterator it =
          measures.rbegin(); it != measures.rend(); ++it ) {
    FitNode* node = new FitNode( **it );
    nodes.push_back( node );
    zmin = std::min( zmin, node->z()) ;
    zmax = std::max( zmax, node->z()) ;
  }

  // Loop over the predefined z positions and add them to the nodes only if they are 
  // between the first and last measurement.
  for (std::vector<double>::const_iterator izpos = m_zPositions.begin() ; 
       izpos != m_zPositions.end(); ++izpos ) {
    double z = *izpos;
    if( m_makeNodesAtAllReferencePositions || (zmin < z && z < zmax ) ) {
      FitNode* node = new FitNode( z );
      State& state = node->state();
      // set the location (must find a better way to do this!)
      if ( fabs(z - StateParameters::ZBegRich1) < TrackParameters::lowTolerance )
        state.setLocation( State::BegRich1 );
      else if ( fabs(z - StateParameters::ZEndRich1 ) < TrackParameters::lowTolerance )
        state.setLocation( State::EndRich1 );
      else if ( fabs(z - StateParameters::ZBegRich2 ) < TrackParameters::lowTolerance )
        state.setLocation( State::BegRich2 );
      else if ( fabs(z - StateParameters::ZEndRich2 ) < TrackParameters::lowTolerance )
        state.setLocation( State::EndRich2 );
      nodes.push_back( node ) ;
    }
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
  for( Track::NodeContainer::const_iterator it = nodes.begin() ;
       it != nodes.end(); ++it ) {
    LHCb::StateVector ref = tracktraj.stateVector((*it)->z())  ;
    (*it)->setRefVector( ref ) ;
    (*it)->state().setState(ref) ;
  }
  
  // add all the noise, if required
  if(m_applyMaterialCorrections && sc.isSuccess()) {
    sc = updateMaterialCorrections( track ) ;
    if(!sc.isSuccess())
      error() << "Problem setting material corrections." << endreq ;
  }
  
  // create the transport of the reference (after the noise, because it uses energy loss)
  if( sc.isSuccess()) {
    sc = updateTransport( track ) ;
    if(!sc.isSuccess()) 
      error() << "Problem setting transports." << endreq ;
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
  // upstream   == from T to Velo
  // downstream == from Velo to T
  double chisqT[2]    = {0,0} ;
  double chisqTT[2]   = {0,0} ;
  double chisqVelo[2] = {0,0} ;
  int    nhitsT(0), nhitsTT(0), nhitsVelo(0) ;
  for( std::vector<Node*>::const_iterator iNode = iNode = track.nodes().begin(); 
       iNode != track.nodes().end(); ++iNode ) 
    if( (*iNode)->hasMeasurement() ) {
      const FitNode* node = dynamic_cast<FitNode*>(*iNode) ;
      if(!node) {
	error() << "fillExtraInfo: node is not a FitNode" << endmsg ;
      } else {
	switch( node->measurement().type() ) {
	case Measurement::VeloR:
	case Measurement::VeloPhi:
	  chisqVelo[0] += node->deltaChi2Upstream() ;
	  chisqVelo[1] += node->deltaChi2Downstream() ;
	  ++nhitsVelo ;
	  break;
	case Measurement::TT:
	  chisqTT[0] += node->deltaChi2Upstream() ;
	  chisqTT[1] += node->deltaChi2Downstream() ;
	  ++nhitsTT ;
	  break;
	case Measurement::OT:
	case Measurement::IT:
	  chisqT[0] += node->deltaChi2Upstream() ;
	  chisqT[1] += node->deltaChi2Downstream() ;
	  ++nhitsT ;
	  break;
	default:
	  break;
	}
      }
    }

  const int nPar = track.nStates()>0 ? track.firstState().nParameters() : 5 ;
  
  if( track.hasT() ) {
    track.addInfo( Track::FitTChi2 , m_upstream ? chisqT[0] : chisqT[1] ) ;
    track.addInfo( Track::FitTNDoF , nhitsT - nPar ) ;
    // if( track.hasTT() ) {
    //   track.addInfo( TTTChi2Key , m_upstream ? chisqT[0] + chisqTT[0] : chisqT[1] + chisqTT[1] ) ;
    //   track.addInfo( TTTNDoFKey , nhitsT + nhitsTT - nPar ) ;
    // }
  }

  if( track.hasVelo() ) {
    track.addInfo( Track::FitVeloChi2, m_upstream ? chisqVelo[1] : chisqVelo[0] ) ;
    track.addInfo( Track::FitVeloNDoF, nhitsVelo - nPar ) ;
    // if( track.hasTT() ) {
    //   track.addInfo( VeloTTChi2Key , m_upstream ? chisqVelo[1] + chisqTT[1] : chisqVelo[0] + chisqTT[0] ) ;
    //   track.addInfo( VeloTTNDoFKey , nhitsVelo + nhitsTT - nPar ) ;
    // }
  
    if( track.hasT() ) {
      // Calculate the chisquare of the breakpoint between TT and T
      double thischisqT       = m_upstream ? chisqT[0] : chisqT[1] ;
      double thischisqVeloTT  = m_upstream ? chisqVelo[1] + chisqTT[1] : chisqVelo[0] + chisqTT[0] ;
      // they should be equal, but this is safer
      double chisqTot   = std::max( chisqT[0]+chisqTT[0]+chisqVelo[0], chisqT[1]+chisqTT[1]+chisqVelo[1] ) ;
      double chisqMatch = chisqTot - thischisqT - thischisqVeloTT ;
      track.addInfo( Track::FitMatchChi2, chisqMatch ) ;
    }

    double thischisqvelo = m_upstream ? chisqVelo[1] : chisqVelo[0] ;
    assert( thischisqvelo == track.info( Track::FitVeloChi2, 0 ) );
  }
}


StatusCode TrackMasterFitter::updateMaterialCorrections(LHCb::Track& track) const
{
  // the noise in each node is the noise in the propagation between
  // the previous node and this node.

  // first collect all volumes on the track. The advantages of collecting them all at once 
  // is that it is much faster. (Less call to TransportSvc.)
  LHCb::Track::NodeContainer& nodes = track.nodes() ;

  if( nodes.size()>1 ) {
    // only apply energyloss correction for tracks that traverse magnet
    bool   applyenergyloss    = track.hasT() && (track.hasVelo()||track.hasTT()) ;
    
    // if only velo, or magnet off, use a fixed momentum based on pt.
    double scatteringMomentum = track.firstState().p() ;
    if( !track.hasT() && !track.hasTT()) {
      double tx     = track.firstState().tx() ;
      double ty     = track.firstState().ty() ;
      double slope2 = tx*tx+ty*ty ;
      double tanth  = std::min(sqrt( slope2/(1+slope2)),1e-4) ;
      scatteringMomentum = m_scatteringPt/tanth ;
    }
    
#define USETRACKTRAJ
#ifdef USETRACKTRAJ
    LHCb::TrackTraj tracktraj( nodes ) ;
    IMaterialLocator::Intersections intersections ;
    m_materialLocator->intersect( tracktraj, intersections ) ;
    
    // now we need to redistribute the result between the nodes. the first node cannot have any noise.
    LHCb::Track::NodeContainer::iterator inode = nodes.begin() ;
    double zorigin((*inode)->z()) ;
    for(++inode; inode!=nodes.end(); ++inode) {
      FitNode* node = dynamic_cast<FitNode*>(*inode) ;
      double ztarget = node->z() ;
      Gaudi::TrackSymMatrix noise ;
      Gaudi::TrackVector delta;
      m_materialLocator->computeMaterialCorrection(noise,delta,intersections,zorigin,ztarget,
                                                   scatteringMomentum,LHCb::ParticleID(m_particleID)) ;
      node->setNoiseMatrix( noise ) ;
      double deltaE = 1 / ( 1/scatteringMomentum + delta(4) ) - scatteringMomentum ;
      node->setDeltaEnergy( applyenergyloss ? deltaE : 0 ) ;
      zorigin = ztarget ;
    }
#else
    IMaterialLocator::Intersections intersections ;

    // now we need to redistribute the result between the nodes. the first node cannot have any noise.
    LHCb::Track::NodeContainer::iterator inode = nodes.begin() ;
    LHCb::Track::NodeContainer::iterator prevnode = inode ;
    for(++inode; inode!=nodes.end(); ++inode) {
      m_materialLocator->intersect((*prevnode)->refVector(),
				   (*inode)->refVector(),intersections) ;
      
      FitNode* node = dynamic_cast<FitNode*>(*inode) ;
      double ztarget = node->z() ;
      double zorigin = (*prevnode)->z() ;
      Gaudi::TrackSymMatrix noise ;
      Gaudi::TrackVector delta;
      m_materialLocator->computeMaterialCorrection(noise,delta,intersections,zorigin,ztarget,
                                                   scatteringMomentum,LHCb::ParticleID(m_particleID)) ;
      node->setNoiseMatrix( noise ) ;
      double deltaE = 1 / ( 1/scatteringMomentum + delta(4) ) - scatteringMomentum ;
      node->setDeltaEnergy( applyenergyloss ? deltaE : 0 ) ;
      prevnode = inode ;
    } 

#endif
  }
  return StatusCode::SUCCESS ;
}

StatusCode TrackMasterFitter::updateTransport(LHCb::Track& track) const
{
  // sets the propagation between the previous node and this. node that the reference 
  // of the previous node is used.
  StatusCode sc = StatusCode::SUCCESS ;
  
  LHCb::Track::NodeContainer& nodes = track.nodes() ;
  if( nodes.size()>1 ) {
    LHCb::Track::NodeContainer::iterator inode = nodes.begin() ;
    const LHCb::StateVector* refvector = &((*inode)->refVector()) ;
    TrackMatrix F = TrackMatrix( ROOT::Math::SMatrixIdentity() );
    for(++inode; inode!=nodes.end(); ++inode) {
      
      FitNode* node = dynamic_cast<FitNode*>(*inode) ;
      double z = node->z() ;
      LHCb::StateVector statevector = *refvector ;
      StatusCode thissc = m_extrapolator -> propagate(statevector,z,&F) ;
      if ( thissc.isFailure() ) {
        error() << "unable to propagate reference vector from z=" << refvector->z() 
                << " to " << z ;
        sc = thissc ;
      }
      
      // correct for energy loss
      double charge = statevector.qOverP() > 0 ? 1 :  -1 ;
      double qopnew = 1/(1/statevector.qOverP() + charge * node->deltaEnergy()) ;
      statevector.setQOverP( qopnew ) ;

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
