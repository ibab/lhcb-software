// Include files 
// -------------
// Event/DigiEvent
#include "Event/VeloCluster.h"

// Kernel/LHCbKernel
#include "Kernel/VeloChannelID.h"
#include "Kernel/IVeloChannelIDSelector.h"

// local
#include "VeloSelectClustersByChannel.h"

//-----------------------------------------------------------------------------
// Implementation file for class : VeloSelectClustersByChannel
//
// 2011-06-28 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( VeloSelectClustersByChannel )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloSelectClustersByChannel::VeloSelectClustersByChannel( const std::string& type,
                                                          const std::string& name,
                                                          const IInterface* parent )
  : GaudiTool ( type, name , parent ), m_selector(0)
{
  declareProperty( "SelectorType", m_selectorType = "VeloSelectChannelIDOnTrack" );
  declareProperty( "SelectorName", m_selectorName = "Selector" );
  
  declareInterface<IVeloClusterSelector>( this );
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode VeloSelectClustersByChannel::initialize()
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  m_selector = tool<IVeloChannelIDSelector>( m_selectorType, m_selectorName, this );
  
  if ( msgLevel( MSG::INFO ) )
    info() << "Adding Tool: type: " << m_selectorType << " / name: " 
           << m_selectorName << endmsg;
  
  return StatusCode::SUCCESS;
}

//=============================================================================
// 
//=============================================================================
bool VeloSelectClustersByChannel::select( const LHCb::VeloCluster* cluster ) const
{
  return (*this) ( cluster );
}

//=============================================================================
// 
//============================================================================= 
bool VeloSelectClustersByChannel::operator()( const LHCb::VeloCluster* cluster ) const
{
  // Just delegate to the channel selector
  return m_selector -> select( cluster -> channelID() );
}

//=============================================================================
// Destructor
//=============================================================================
VeloSelectClustersByChannel::~VeloSelectClustersByChannel() {} 

//=============================================================================
