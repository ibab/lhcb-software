// $Id: ClusterLHCbIDCnv.cpp,v 1.1.1.1 2005-04-01 16:18:50 erodrigu Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "TrackTools/ClusterLHCbIDCnv.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ClusterLHCbIDCnv
//
// 2005-03-31 : Eduardo Rodrigues
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<ClusterLHCbIDCnv>          s_factory ;
const        IToolFactory& ClusterLHCbIDCnvFactory = s_factory ; 

//=============================================================================
// Return the OTTime corresponding to an LHCbID
//=============================================================================
const OTTime& ClusterLHCbIDCnv::otTime( const LHCbID& id )
{
  OTTime* dummy = NULL;
  if ( ! id.isOT() ) {
    error() << "LHCbID not of OT type!" << endreq;
    return *dummy;
  }
  
  OTChannelID otChID = id.otID();
  OTTimes* otTimes = get<OTTimes>( OTTimeLocation::Default );
  OTTimes::const_iterator itOTTime;
  for ( itOTTime=otTimes->begin() ; otTimes->end()!= itOTTime ; ++itOTTime ) {
    if ( otChID == (*itOTTime)->channel() ) {
      return *(*itOTTime);
    }
  }

  if ( NULL == (*itOTTime) )
    error() << "LHCbID " << id.lhcbID() << " is not valid!" << endreq;

  return *dummy;
}

//=============================================================================
// Return the ITCluster corresponding to an LHCbID
//=============================================================================
const ITCluster& ClusterLHCbIDCnv::itCluster( const LHCbID& id )
{
  ITCluster* dummy = NULL;
  if ( ! id.isST() ) {
    error() << "LHCbID not of ST type!" << endreq;
    return *dummy;    
  }

  ITChannelID itChID = id.stID();
  ITClusters* itClus = get<ITClusters>( ITClusterLocation::Default );
  ITClusters::const_iterator itITClu;
  for ( itITClu=itClus->begin() ; itClus->end()!= itITClu ; ++itITClu ) {
    if ( itChID == (*itITClu)->channelID() ) {
      return *(*itITClu);
    }
  }

  if ( NULL == (*itITClu) )
    error() << "LHCbID " << id.lhcbID() << " is not valid!" << endreq;

  return *dummy;
}

//=============================================================================
// Return the VeloCluster corresponding to an LHCbID
//=============================================================================
const VeloCluster& ClusterLHCbIDCnv::veloCluster( const LHCbID& id )
{
  VeloCluster* dummy = NULL;
  if ( ! id.isVelo() ) {
    error() << "LHCbID not of Velo type!" << endreq;
    return *dummy;
  }

  VeloChannelID veloChID = id.veloID();
  VeloClusters* veloClus = get<VeloClusters>( VeloClusterLocation::Default );
  VeloClusters::const_iterator itVeloClu;
  for ( itVeloClu=veloClus->begin() ; veloClus->end()!= itVeloClu ; ++itVeloClu ) {
    if ( veloChID.channelID() == (*itVeloClu)->channelID(0).channelID() ) {
      return *(*itVeloClu);
    }
  }  

  if ( NULL == (*itVeloClu) )
    error() << "LHCbID " << id.lhcbID() << " is not valid!" << endreq;

  return *dummy;
}

//=============================================================================
// Return the LHCbID corresponding to a OTTime
//=============================================================================
LHCbID ClusterLHCbIDCnv::lhcbid( const OTTime& otTime )
{
  return LHCbID( otTime.channel() );
}

//=============================================================================
// Return the LHCbID corresponding to a ITCluster
//=============================================================================
LHCbID ClusterLHCbIDCnv::lhcbid( const ITCluster& itCluster )
{
  return LHCbID( itCluster.channelID() );
}

//=============================================================================
// Return the LHCbID corresponding to a VeloCluster
//=============================================================================
LHCbID ClusterLHCbIDCnv::lhcbid( const VeloCluster& veloCluster )
{
  return LHCbID( veloCluster.channelID(0) );
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ClusterLHCbIDCnv::ClusterLHCbIDCnv( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ClusterLHCbIDCnv>( this );

}
//=============================================================================
// Destructor
//=============================================================================
ClusterLHCbIDCnv::~ClusterLHCbIDCnv() {}; 

//=============================================================================
