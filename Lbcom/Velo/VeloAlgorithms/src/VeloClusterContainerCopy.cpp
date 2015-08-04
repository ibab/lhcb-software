// $Id: $

// Include files 
// -------------
// Kernel/LHCbKernel
#include "Kernel/IVeloClusterSelector.h"

// Event/DigiEvent
#include "Event/VeloCluster.h"

// Local
#include "VeloClusterContainerCopy.h"


//-----------------------------------------------------------------------------
// Implementation file for class : VeloClusterContainerCopy
//
// 2011-06-28 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloClusterContainerCopy )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloClusterContainerCopy::VeloClusterContainerCopy( const std::string& name,
                                                    ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_clusterSelector( 0 )
{
  declareProperty( "InputLocation" ,
                   m_inputLocation = LHCb::VeloClusterLocation::Default );
  declareProperty( "OutputLocation",
                   m_outputLocation = LHCb::VeloClusterLocation::Default+"Selected" );
  declareProperty( "SelectorType"  ,
                   m_selectorType = "VeloSelectClustersByChannel" );
  declareProperty( "SelectorName"  ,
                   m_selectorName = "Selector" );
}

//=============================================================================
// Destructor
//=============================================================================
VeloClusterContainerCopy::~VeloClusterContainerCopy() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode VeloClusterContainerCopy::initialize()
{
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Initialize" << endmsg;
  
  m_clusterSelector = tool<IVeloClusterSelector>( m_selectorType, m_selectorName, this );
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloClusterContainerCopy::execute() {
  
  if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Execute" << endmsg;
  
  LHCb::VeloClusters* clusterCont = get<LHCb::VeloClusters>( m_inputLocation );
  
  LHCb::VeloClusters* outputCont  = new LHCb::VeloClusters();
  put( outputCont, m_outputLocation );
  
  LHCb::VeloClusters::const_iterator iterC = clusterCont -> begin();
  for ( ; iterC != clusterCont -> end(); ++iterC ) {
    const bool select = (*m_clusterSelector)( *iterC );
    if ( select ) {
      LHCb::VeloCluster* newClus = (*iterC) -> clone(); 
      outputCont -> insert( newClus, (*iterC) -> channelID() );
      ++counter( "Selected" );
    }
    ++counter( "Processed" );
  }
  
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloClusterContainerCopy::finalize() {

  if ( msgLevel( MSG::DEBUG ) ) debug() << "==> Finalize" << endmsg;

  const double selected  = counter( "Selected"  ).flag();
  const double processed = counter( "Processed" ).flag();

  double fracSel = 0.0; 
  if ( ! LHCb::Math::Equal_To<double>() ( processed, 0.0 ) ) { 
    fracSel = selected / processed;
  }
  if ( msgLevel( MSG::INFO ) )
    info() << "Fraction of clusters selected : " << 100 * fracSel << " %"  << endmsg;
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
