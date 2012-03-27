// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Event/Track.h"

// local
#include "PackCluster.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackCluster
//
// 2012-03-05 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PackCluster )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  PackCluster::PackCluster( const std::string& name,
                            ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::TrackLocation::Default );
  declareProperty( "OutputName", m_outputName = LHCb::PackedClusterLocation::Default );
  declareProperty( "AlwaysCreateOutput",         m_alwaysOutput = false     );
}

//=============================================================================
// Destructor
//=============================================================================
PackCluster::~PackCluster() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackCluster::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // If input does not exist, and we aren't making the output regardless, just return
  if ( !m_alwaysOutput && !exist<LHCb::Tracks>(m_inputName) ) return StatusCode::SUCCESS;
  const LHCb::Tracks* tracks = getOrCreate<LHCb::Tracks,LHCb::Tracks>( m_inputName );
  LHCb::PackedClusters* out = new LHCb::PackedClusters();
  put( out, m_outputName );

  std::vector<LHCb::LHCbID> allIds;
  allIds.reserve( tracks->size() * 40 );
  for ( LHCb::Tracks::const_iterator itT = tracks->begin(); tracks->end() != itT; ++itT ) 
  {
    const LHCb::Track* track = *itT;
    for ( std::vector<LHCb::LHCbID>::const_iterator itI = track->lhcbIDs().begin();
          track->lhcbIDs().end() != itI; ++itI )
    {
      if ( (*itI).isOT() ) continue;
      allIds.push_back( *itI );
    }
  }

  // Sort and remove duplicate LHCbIDs
  std::sort( allIds.begin(), allIds.end() );
  std::vector<LHCb::LHCbID>::iterator itEnd = std::unique( allIds.begin(), allIds.end() );
  allIds.resize( itEnd - allIds.begin() );

  // Load the clusters
  LHCb::VeloClusters* vClus = get<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default);
  LHCb::STClusters*  ttClus = get<LHCb::STClusters>(LHCb::STClusterLocation::TTClusters);
  LHCb::STClusters*  itClus = get<LHCb::STClusters>(LHCb::STClusterLocation::ITClusters);

  for ( std::vector<LHCb::LHCbID>::const_iterator itI = allIds.begin();
        allIds.end() != itI; ++itI ) 
  {
    if ( (*itI).isVelo() )
    {
      const LHCb::VeloCluster* cl = vClus->object( (*itI).veloID() );
      if ( cl ) 
      {
        out->addVeloCluster( cl );
      }
      else 
      {
        std::ostringstream mess;
        mess << "Unknown Velo cluster : " << *itI;
        Warning( mess.str() ).ignore();
      }
    } 
    else if ( (*itI).isTT() )
    {
      const LHCb::STCluster* cl = ttClus->object( (*itI).stID() );
      if ( cl ) 
      {
        out->addTTCluster( cl );
      }
      else 
      {
        std::ostringstream mess;
        mess << "Unknown TT cluster : " << *itI;
        Warning( mess.str() ).ignore();
      }
    } 
    else if ( (*itI).isIT() ) 
    {
      const LHCb::STCluster* cl = itClus->object( (*itI).stID() );
      if ( cl ) 
      {
        out->addITCluster( cl );
      }
      else
      {
        std::ostringstream mess;
        mess << "Unknown IT cluster : " << *itI;
        Warning( mess.str() ).ignore();
      }
    } 
    else 
    {
      std::ostringstream mess;
      mess << "Unknown LHCb ID on track : " << *itI;
      Warning( mess.str() ).ignore();
    }
  }

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Stored " << out->clusters().size() << " clusters" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
