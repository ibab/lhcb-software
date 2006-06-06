// $Id: TrackMasterFitter.cpp,v 1.12 2006-06-06 13:35:00 erodrigu Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from LHCbInterfaces
#include "Kernel/PhysicalConstants.h"

// from TrackEvent
#include "Event/TrackFunctor.h"

// local
#include "TrackMasterFitter.h"

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
{
  declareInterface<ITrackFitter>( this );

  // Define the default fixed z-positions
  m_zPositions.push_back(   990.0*Gaudi::Units::mm );
  m_zPositions.push_back(  2165.0*Gaudi::Units::mm );
  m_zPositions.push_back(  9450.0*Gaudi::Units::mm );
  m_zPositions.push_back( 11900.0*Gaudi::Units::mm );

  declareProperty( "Extrapolator"        , m_extrapolatorName =
                                           "TrackMasterExtrapolator" );
  declareProperty( "TrackNodeFitterName" , m_trackNodeFitterName =
                                           "TrackKalmanFilter" );
  declareProperty( "FitUpstream"         , m_upstream         =   true     );
  declareProperty( "NumberFitIterations" , m_numFitIter       =     5      );
  declareProperty( "Chi2Outliers"        , m_chi2Outliers     =     9.0    );
  declareProperty( "StatesAtMeasZPos"    , m_statesAtMeasZPos =   false    );
  declareProperty( "StateAtBeamLine"     , m_stateAtBeamLine  =   true     );
  declareProperty( "ZPositions"          , m_zPositions                    );
  declareProperty( "ZBegRich1"           ,
                   m_zBegRich1 =   990.0*Gaudi::Units::mm );
  declareProperty( "ZEndRich1"           ,
                   m_zEndRich1 =  2165.0*Gaudi::Units::mm );
  declareProperty( "ZBegRich2"           ,
                   m_zBegRich2 =  9450.0*Gaudi::Units::mm );
  declareProperty( "ZEndRich2"           ,
                   m_zEndRich2 = 11900.0*Gaudi::Units::mm );
  declareProperty( "IncreaseErrors" , m_increaseErrors =   true           );
  declareProperty( "ErrorX2"        , m_errorX2 = 4.0*Gaudi::Units::mm2   );
  declareProperty( "ErrorY2"        , m_errorY2 = 400.0*Gaudi::Units::mm2 );
  declareProperty( "ErrorTx2"       , m_errorTx2 = 6.e-5                  );
  declareProperty( "ErrorTy2"       , m_errorTy2 = 1.e-4                  );
  declareProperty( "ErrorP"         , m_errorP   = 0.15                   );
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

  m_extrapolator    = tool<ITrackExtrapolator>( m_extrapolatorName );
  m_trackNodeFitter = tool<ITrackFitter>( m_trackNodeFitterName, 
                                          "NodeFitter", this ) ;
  m_measProvider    = tool<IMeasurementProvider>( "MeasurementProvider",
                                                  "MeasProvider", this );
  m_debugLevel   = msgLevel( MSG::DEBUG );  

  info() << " " << endmsg
         << "============ TrackMasterFitter Settings ===========" << endmsg
         << ((m_upstream) ? " Upstream fit" : " Downstream fit")
         << endmsg << " Max " << m_numFitIter 
         << " iterations with outliers at chi2 > " << m_chi2Outliers << endmsg
         << " State z positions at: " << endmsg
         << ((m_stateAtBeamLine) ? "beam line, " : "")
         << "first measurement, "
         << ((m_statesAtMeasZPos) ? "every measurement, " : "" );
  std::vector<double>::const_iterator zPos;
  for ( zPos = m_zPositions.begin(); zPos != m_zPositions.end(); ++zPos ) {
    info() << *zPos << ", " ;
  }
  info() << endmsg
         << "==================================================" << endmsg;
  
  return StatusCode::SUCCESS;
}

//=========================================================================
// Helper to print a failure comment
//=========================================================================
StatusCode TrackMasterFitter::failure(const std::string& comment) {
  info() << "TrackMasterFitter failure: " << comment << endreq;
  return StatusCode::FAILURE;
}

//=========================================================================
// Fit the track
//=========================================================================
StatusCode TrackMasterFitter::fit( Track& track )
{
  StatusCode sc;

  // Get the seed state
  if ( track.nStates() == 0 )
    return Error( "Track has no state! Can not fit.", StatusCode::FAILURE );
  State seed = seedState( track );

  // set covariance matrix to a somewhat larger value for the fit
  if ( m_increaseErrors ) {
    TrackSymMatrix& cov = seed.covariance();
    cov(0,0) = m_errorX2;
    cov(1,1) = m_errorY2;
    cov(2,2) = m_errorTx2;
    cov(3,3) = m_errorTy2;
    cov(4,4) = pow( m_errorP * seed.qOverP(), 2. );
    debug() << "-> seed state covariance matrix blown up" << endreq;
  }
  
  debug() << "SeedState: z = " << seed.z()
          << " stateVector = " << seed.stateVector()
          << " covariance  = " << seed.covariance() << endreq;

  // Make the nodes from the measurements
  sc = makeNodes( track );
  if ( sc.isFailure() )
    return failure("Unable to make nodes from the measurements");

  // Check that the number of measurements is enough
  if ( nNodesWithMeasurement( track ) < seed.nParameters() ) {
    error() << "Track has only " << track.nMeasurements()
            << " measurements. Not enough to fit a "
            << seed.nParameters() << "D-state!" << endreq;
    return StatusCode::FAILURE;
  }

  // Extrapolate the given seedstate to the z position of the first node
  std::vector<Node*>& nodes = track.nodes();
  std::vector<Node*>::iterator firstNode = nodes.begin();
  double z = (*firstNode) -> z();
  sc = m_extrapolator -> propagate( seed, z );
  if ( sc.isFailure() ) 
    return failure("unable to extrapolate to measurement");
  seed.setLocation( ((*firstNode)->state()).location() );
  (*firstNode)->setState( seed );

  // Iterations of filtering-smoothing sequence
  bool doNextIteration = true;
  int iter = 0;
  do {
    // fit iteration
    ++iter;
    if ( m_debugLevel ) debug() << "Iteration # " << iter << endreq;
    // Call the track fit
    sc = m_trackNodeFitter->fit( track );
    if ( sc.isFailure() ) {
      std::ostringstream mess;
      mess << "unable to fit the track # " << track.key();
      return failure( mess.str() );
    }

    // Prepare for next iteration
    if ( iter < m_numFitIter ) {
      // Outlier removal
      if ( nNodesWithMeasurement( track ) > seed.nParameters() ) {
        doNextIteration = outlierRemoved( track );
      } else {
        doNextIteration = false;
      }

      if ( doNextIteration ) {
        // Re-seeding for next iteration (i.e. inflate covariance matrix)
        State& firstState = (*firstNode)->state();
        reSeed( seed, firstState );
        seed = firstState;

        // Update the reference trajectories in the measurements
        updateRefVectors( track );
      }
    }
  } while ( iter < m_numFitIter && doNextIteration );

  // determine the track states at user defined z positions
  sc = determineStates( track );
  if ( sc.isFailure() ) {
    warning() << "Failed to determine the states at the requested z-positions"
              << endreq;
    // clear the node vector
    clearNodes( nodes );
    return sc;
  }

  // set the HistoryFit flag to "Kalman fit done"
  track.setHistoryFit( Track::Kalman );

  return sc;
}

//=============================================================================
// 
//=============================================================================
StatusCode TrackMasterFitter::determineStates( Track& track )
{
  // clean the non-fitted states in the track!
  const std::vector<State*> allstates = track.states();
  for ( std::vector<State*>::const_iterator it = allstates.begin();
        it != allstates.end(); ++it) track.removeFromStates( *it );

  std::vector<Node*>& nodes = track.nodes();
  // Add state closest to the beam-line
  // ----------------------------------
  if ( m_stateAtBeamLine ) {
    // Get the state closest to z=0.0 from the nodes
    // TODO: should be a method from Track
    std::vector<Node*>::iterator iNode = 
      std::min_element( nodes.begin(), nodes.end(),
                        TrackFunctor::distanceAlongZ<Node>( 0. ) );
    State closeState = (*iNode)->state();
    
    // Get the z-position of the "intersection" with the beam line
    double z = closestToBeamLine( closeState );    
    StatusCode sc = m_extrapolator -> propagate( closeState , z );
    if ( sc.isFailure() ) {
      warning() << "Extrapolation of state to z = " << z << " failed."
                << " No state closest to beam-line added." << endreq;
    } else {
      // add the state at the position closest to the beam line
      closeState.setLocation( State::ClosestToBeam );
      track.addToStates( closeState );
    }
  }

  // Add state at first measurement
  // ------------------------------
  if ( m_upstream ) {
    std::vector<Node*>::reverse_iterator iNode = nodes.rbegin();
    while ( !(*iNode)->hasMeasurement() ) ++iNode;
    State& state = (*iNode) -> state();
    state.setLocation( State::FirstMeasurement );
  } else {
    std::vector<Node*>::iterator iNode = nodes.begin();
    while ( !(*iNode)->hasMeasurement() ) ++iNode;
    State& state = (*iNode) -> state();
    state.setLocation( State::FirstMeasurement );
  }
  
  // Add the states at the predefined positions
  // or at all nodes if requested
  // ------------------------------------------
  std::vector<Node*>::const_iterator it;
  for ( it = nodes.begin(); it < nodes.end(); ++it ) {
    State& state = (*it)->state();
    if ( m_statesAtMeasZPos ) track.addToStates( state );
    else {
      std::vector<double>::const_iterator zPos;
      for ( zPos = m_zPositions.begin(); zPos != m_zPositions.end(); ++zPos ) {
        if ( fabs(*zPos - state.z()) < TrackParameters::looseTolerance )
          track.addToStates( state );
      }
    }
  }

  if ( m_debugLevel ) {
    debug() << "Track " << track.key() << " has " << track.nStates() 
            << " states after fit" << endmsg << "  at z = " ;
    const std::vector<State*>& allstates = track.states();
    for ( unsigned int it2 = 0; it2 < allstates.size(); it2++ ) {
      debug() << allstates[it2]->z() << ", " ;
    }
    debug() << nNodesWithMeasurement( track ) 
            << " measurements used for the fit (out of " 
            << track.nLHCbIDs() << ")." << endmsg;
  }

  return StatusCode::SUCCESS;

}

//=========================================================================
//
//=========================================================================
bool TrackMasterFitter::outlierRemoved( Track& track )
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
      double chi2 = (*iNode) -> chi2();
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
              << "of type="
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
//
//=========================================================================
StatusCode TrackMasterFitter::reSeed( State& seed, State& newSeed )
{
  // First extrapolate the previous seed to the z position of 
  // the current newSeed
  StatusCode sc = m_extrapolator->propagate( seed, newSeed.z() );
  if ( sc.isFailure() ) { 
    warning() << "extrapolation of state to z=" << newSeed.z()
              << " failed" << endreq;
    return sc;
  }

  // use the large covariance matrix of the previous seed for new seed
  TrackSymMatrix& tC = newSeed.covariance();
  tC = seed.covariance();
  
  return StatusCode::SUCCESS;
}

//=========================================================================
// Update the measurements before a refit
//=========================================================================
void TrackMasterFitter::updateRefVectors( Track& track ) 
{ 
  std::vector<Node*>& nodes = track.nodes();
  std::vector<Node*>::iterator iNode;
  for ( iNode = nodes.begin(); iNode != nodes.end(); ++iNode ) {    
    if ( (*iNode)->hasMeasurement() ) {
      Measurement& meas = (*iNode) -> measurement();
      meas.setRefVector( (*iNode)->state().stateVector() );
    }
  }
}

//=========================================================================
// 
//=========================================================================
void TrackMasterFitter::clearNodes( std::vector<Node*>& nodes ) 
{
  if ( !nodes.empty() ) {
    std::vector<Node*>::iterator iNode = nodes.begin();
    while ( iNode != nodes.end() ) {
      Node* aNode = *iNode;
      nodes.erase( iNode );
      delete aNode;
    }
  }
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
  return z;
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
// Get a seed State from the Track
//=============================================================================
const State& TrackMasterFitter::seedState( Track& track )
{
  return (m_upstream) ? (*track.states().back()) : (track.firstState());
}

//=============================================================================
// Create the nodes from the measurements
//=============================================================================
StatusCode TrackMasterFitter::makeNodes( Track& track ) 
{
  // Clear the nodes
  std::vector<Node*>& nodes = track.nodes();
  clearNodes( nodes );

  // Check if it is needed to populate the track with measurements
  if ( track.status() == Track::PatRecIDs ) {
    m_measProvider -> load();    
    StatusCode sc = m_measProvider -> load( track );
    if ( sc.isFailure() )
      return Error( "Unable to load measurements!", StatusCode::FAILURE );
    debug() << "# LHCbIDs, Measurements = " << track.nLHCbIDs()
            << ", " << track.nMeasurements() << endreq;
  }

  // reserve some space in node vector
  const std::vector<Measurement*>& measures = track.measurements();
  nodes.reserve( measures.size() + m_zPositions.size() );

  // Create the nodes and add them to the private copy
  for ( std::vector<Measurement*>::const_reverse_iterator it =
          measures.rbegin(); it != measures.rend(); ++it ) {
    Measurement& meas = *(*it);    
    FitNode* node = new FitNode( meas );
    nodes.push_back( node );
  }

  // Loop over the predefined z positions and add them to the nodes
  std::vector<double>::const_iterator zPos;
  for ( zPos = m_zPositions.begin(); zPos != m_zPositions.end(); ++zPos ) {
      FitNode* node = new FitNode( *zPos );
      State& locState = node->state();
      if ( fabs(*zPos - m_zBegRich1) < TrackParameters::lowTolerance )
        locState.setLocation( State::BegRich1 );
      else if ( fabs(*zPos - m_zEndRich1 ) < TrackParameters::lowTolerance )
        locState.setLocation( State::EndRich1 );
      else if ( fabs(*zPos - m_zBegRich2 ) < TrackParameters::lowTolerance )
        locState.setLocation( State::BegRich2 );
      else if ( fabs(*zPos - m_zEndRich2 ) < TrackParameters::lowTolerance )
        locState.setLocation( State::EndRich2 );
      nodes.push_back( node );
  }

  // Sort the nodes in z
  if ( m_upstream ) {
    std::sort( nodes.begin(), nodes.end(), TrackFunctor::decreasingByZ<Node>());
  } else {
    std::sort( nodes.begin(), nodes.end(), TrackFunctor::increasingByZ<Node>());
  }

  if ( m_debugLevel ) {
    for ( std::vector<Node*>::const_iterator inode = nodes.begin();
          inode != nodes.end(); ++inode ) {
    } 
  }
  

  return StatusCode::SUCCESS;
}

//=========================================================================
