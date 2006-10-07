// $Id: TrackMasterFitter.cpp,v 1.23 2006-10-07 14:40:24 erodrigu Exp $
// Include files 
// -------------
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// from TrackEvent
#include "Event/TrackFunctor.h"
#include "Event/StateParameters.h"

// local
#include "TrackMasterFitter.h"

#include "TrackInterfaces/ITrackManipulator.h"

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
  , m_refInfoTool(0)
{
  declareInterface<ITrackFitter>( this );

  // Define the default fixed z-positions
  m_zPositions.push_back( StateParameters::ZBegRich1 );
  m_zPositions.push_back( StateParameters::ZEndRich1 );
  m_zPositions.push_back( StateParameters::ZBegRich2 );
  m_zPositions.push_back( StateParameters::ZEndRich2 );
  
  declareProperty( "Extrapolator"        , m_extrapolatorName =
                   "TrackMasterExtrapolator" );
  declareProperty( "NodeFitter"          , m_trackNodeFitterName =
                   "TrackKalmanFilter" );
  declareProperty( "FitUpstream"         , m_upstream         =   true    );
  declareProperty( "NumberFitIterations" , m_numFitIter       =     1     );
  declareProperty( "Chi2Outliers"        , m_chi2Outliers     =     9.0   );
  declareProperty( "MaxNumberOutliers"   , m_numOutlierIter   =     2     );
  declareProperty( "StatesAtMeasZPos"    , m_statesAtMeasZPos =   false   );
  declareProperty( "StateAtBeamLine"     , m_stateAtBeamLine  =   true    );
  declareProperty( "ZPositions"          , m_zPositions                   );
  declareProperty( "ZBegRich1"           ,
                   m_zBegRich1 = StateParameters::ZBegRich1               );
  declareProperty( "ZEndRich1"           ,
                   m_zEndRich1 = StateParameters::ZEndRich1               );
  declareProperty( "ZBegRich2"           ,
                   m_zBegRich2 = StateParameters::ZBegRich2               );
  declareProperty( "ZEndRich2"           ,
                   m_zEndRich2 = StateParameters::ZEndRich2               );
  declareProperty( "IncreaseErrors" , m_increaseErrors =   true           );
  declareProperty( "ErrorX2"        , m_errorX2 = 4.0*Gaudi::Units::mm2   );
  declareProperty( "ErrorY2"        , m_errorY2 = 400.0*Gaudi::Units::mm2 );
  declareProperty( "ErrorTx2"       , m_errorTx2 = 6.e-5                  );
  declareProperty( "ErrorTy2"       , m_errorTy2 = 1.e-4                  );
  declareProperty( "ErrorP"         , m_errorP   = 0.15                   );
  declareProperty( "SetRefInfo"     , m_setRefInfo = true                 );
  declareProperty( "RefInfoTool",
                   m_refInfoToolName = "LongTrackReferenceCreator"        );
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

  m_extrapolator    = tool<ITrackExtrapolator>( m_extrapolatorName,
                                                "Extrapolator",this );
  m_trackNodeFitter = tool<ITrackFitter>( m_trackNodeFitterName,
                                          "NodeFitter", this ) ;
  m_measProvider    = tool<IMeasurementProvider>( "MeasurementProvider",
                                                  "MeasProvider", this );
  
  if ( m_setRefInfo )
    m_refInfoTool = tool<ITrackManipulator>( m_refInfoToolName, 
                                             "RefInfoTool", this );
  
  m_debugLevel   = msgLevel( MSG::DEBUG );  

  info() << " " << endmsg
         << "============ TrackMasterFitter Settings ===========" << endmsg
         << ((m_upstream) ? " Upstream fit" : " Downstream fit") << endmsg 
         << " Number of fit iterations: " << m_numFitIter << endmsg
         << " Max " << m_numOutlierIter << " outliers removed with outliers"
         << " at chi2 > " << m_chi2Outliers << endmsg
         << " State z positions at: " << endmsg
         << ((m_stateAtBeamLine) ? " beam line," : "")
         << ((m_statesAtMeasZPos) ? " every measurement" : " first measurement" );
  if ( m_statesAtMeasZPos ) {
    m_zPositions.clear();
  }
  else {
    std::vector<double>::const_iterator zPos;
    for ( zPos = m_zPositions.begin(); zPos != m_zPositions.end(); ++zPos ) {
      info()  << ", " << *zPos;
    }
  }
  info() << endmsg
         << "==================================================" << endmsg;
  
  return StatusCode::SUCCESS;
}

//=========================================================================
// Helper to print a failure comment
//=========================================================================
StatusCode TrackMasterFitter::failure( const std::string& comment ) {
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

  // Make the nodes from the measurements
  sc = makeNodes( track );
  if ( sc.isFailure() )
    return failure( "unable to make nodes from the measurements" );

  // Get the seed state
  if ( track.nStates() == 0 )
    return Error( "Track has no state! Can not fit.", StatusCode::FAILURE );
  State seed = seedState( track );

  // Check that the number of measurements is enough
  if ( nNodesWithMeasurement( track ) < seed.nParameters() ) {
    debug() << "Track has " << track.nMeasurements() 
            << " measurements. Fitting a " << seed.nParameters() 
            << "D-state" << endmsg;
    return Warning( "Insufficient measurements to fit the State",
                    StatusCode::FAILURE );
  }

  // Extrapolate the given seedstate to the z position of the first node
  std::vector<Node*>& nodes = track.nodes();
  std::vector<Node*>::iterator firstNode = nodes.begin();
  double z = (*firstNode) -> z();
  sc = m_extrapolator -> propagate( seed, z );
  if ( sc.isFailure() ) 
    return failure( "unable to extrapolate to measurement" );
  seed.setLocation( ((*firstNode)->state()).location() );

  // set covariance matrix to a somewhat larger value for the fit
  TrackSymMatrix& seedCov = seed.covariance(); 
  if ( m_increaseErrors ) {
    seedCov = TrackSymMatrix(); // Set off-diagonal elements to zero
    seedCov(0,0) = m_errorX2;
    seedCov(1,1) = m_errorY2;
    seedCov(2,2) = m_errorTx2;
    seedCov(3,3) = m_errorTy2;
    seedCov(4,4) = pow( m_errorP * seed.qOverP(), 2. );
    debug() << "-> seed state covariance matrix blown up" << endmsg;
  }

  // Set the seed state in the first node
  (*firstNode)->setState( seed );

  if ( m_debugLevel )
    debug() << "SeedState: z = " << seed.z()
            << " stateVector = " << seed.stateVector()
            << " covariance  = " << seed.covariance() << endmsg;

  // Iterate the track fit for linearisation
  int iter = 1;
  for ( ; iter <= m_numFitIter; ++iter ) {   
    if ( m_debugLevel ) debug() << "Iteration # " << iter << endmsg;

    // Re-seeding (i.e. inflate covariance matrix)
    if ( iter > 1 ) reSeed( track, seedCov );    

    sc = m_trackNodeFitter -> fit( track );
    if ( sc.isFailure() ) {
      std::ostringstream mess;
      mess << "unable to fit the track # " << track.key();
      return failure( mess.str() );
    }
      
    // Update the reference trajectories in the measurements
    updateRefVectors( track );
  }
  
  // Outlier removal iterations
  iter = 1;
  while ( iter <= m_numOutlierIter &&
          nNodesWithMeasurement( track ) > seed.nParameters() &&
          outlierRemoved( track ) ) {
    if ( m_debugLevel ) debug() << "Outlier iteration # " << iter << endmsg;

    // Re-seeding (i.e. inflate covariance matrix
    reSeed( track, seedCov );

    // Call the track fit
    sc = m_trackNodeFitter -> fit( track );
    if ( sc.isFailure() ) {
      std::ostringstream mess;
      mess << "unable to fit the track # " << track.key();
      return failure( mess.str() );
    }    
      
    // Update the reference trajectories in the measurements
    updateRefVectors( track );

    ++iter;  
  }

  // determine the track states at user defined z positions
  sc = determineStates( track );
  if ( sc.isFailure() )  clearNodes( nodes );  // clear the node vector

  return sc;
}

//=============================================================================
// 
//=============================================================================
StatusCode TrackMasterFitter::determineStates( Track& track )
{
  // clean the non-fitted states in the track!
  track.clearStates();
  
  // Add state closest to the beam-line
  // ----------------------------------
  if ( m_stateAtBeamLine ) {
    // Get the state closest to z=0. from the nodes
    State closeState = track.closestState( 0. );
    // Get the z-position of the "intersection" with the beam line
    double z = closestToBeamLine( closeState );    
    StatusCode sc = m_extrapolator -> propagate( closeState , z );
    if ( sc.isFailure() ) {
      debug() << "Extrapolating to z = " << z << endmsg;
      return Warning( "State closest to beam line not added, extrapolation failed",
                      StatusCode::FAILURE );
    } else {
      // add the state at the position closest to the beam line
      closeState.setLocation( State::ClosestToBeam );
      track.addToStates( closeState );
    }
  }

  std::vector<Node*>& nodes = track.nodes();

  // Add the state at the first measurement position
  // -----------------------------------------------
  if ( m_upstream ) {
    std::vector<Node*>::reverse_iterator iNode = nodes.rbegin();
    while ( !(*iNode)->hasMeasurement() ) ++iNode;
    State& state = (*iNode) -> state();
    state.setLocation( State::FirstMeasurement );
    if ( !m_statesAtMeasZPos ) track.addToStates( state );
  }
  else {
    std::vector<Node*>::iterator iNode = nodes.begin();
    while ( !(*iNode)->hasMeasurement() ) ++iNode;
    State& state = (*iNode) -> state();
    state.setLocation( State::FirstMeasurement );
    if ( !m_statesAtMeasZPos ) track.addToStates( state );
  }

  // Add the states at the predefined positions
  // or at all nodes if requested
  // ------------------------------------------
  std::vector<Node*>::const_iterator it;
  for ( it = nodes.begin(); it < nodes.end(); ++it ) {
    State& state = (*it)->state();
    if ( m_statesAtMeasZPos && (*it)->hasMeasurement() ) {
      track.addToStates( state );
    }
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
            << " states after fit\n  at z = " ;
    const std::vector<State*>& allstates = track.states();
    for ( unsigned int it2 = 0; it2 < allstates.size(); it2++ ) {
      debug() << allstates[it2]->z() << ", " ;
    }
    debug() << nNodesWithMeasurement( track ) 
            << " measurements used for the fit (out of " 
            << track.nLHCbIDs() << ")." << endmsg;
  }

  unsigned int numStates = 0;
  if ( m_statesAtMeasZPos )
    numStates += nNodesWithMeasurement( track );
  else
    numStates += m_zPositions.size() + 1; // state at 1st meas. also saved
  if ( m_stateAtBeamLine ) ++numStates;

  if ( numStates != track.nStates() )
    return Warning( "Failed to determine all the requested states!",
                    StatusCode::FAILURE );
  else
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

//=========================================================================
// Fill the covariance matrix of the first node with the original ones
//=========================================================================
void TrackMasterFitter::reSeed( Track& track, const TrackSymMatrix& seedCov )
{
  std::vector<Node*>& nodes = track.nodes();
  std::vector<Node*>::iterator firstNode = nodes.begin();
  TrackSymMatrix& nodeCov = ((*firstNode)->state()).covariance();
  nodeCov = seedCov;
  return;
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
  if ( track.checkPatRecStatus( Track::PatRecIDs ) ) {
    m_measProvider -> load();    
    StatusCode sc = m_measProvider -> load( track );
    if ( sc.isFailure() )
      return Error( "Unable to load measurements!", StatusCode::FAILURE );
    track.setPatRecStatus( Track::PatRecMeas );
    debug() << "# LHCbIDs, Measurements = " << track.nLHCbIDs()
            << ", " << track.nMeasurements() << endreq;
  }

  if ( m_setRefInfo ) m_refInfoTool -> execute( track );

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

  return StatusCode::SUCCESS;
}

//=========================================================================
