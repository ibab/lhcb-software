// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Event/VeloCluster.h"
#include "Event/STCluster.h"
// local
#include "UnpackCluster.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UnpackCluster
//
// 2012-03-06 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( UnpackCluster )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UnpackCluster::UnpackCluster( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::PackedClusterLocation::Default );
  declareProperty( "Extension",  m_extension  = "" );
}
//=============================================================================
// Destructor
//=============================================================================
UnpackCluster::~UnpackCluster() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode UnpackCluster::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackCluster::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  const LHCb::PackedClusters* dst = get<LHCb::PackedClusters>( m_inputName );
  if ( msgLevel(MSG::DEBUG) ) {
    debug() << "Size of packed clusters = " << dst->clusters().size() << " clusters" << endmsg;
  }
 
  //== Create the cluster containers. 
  LHCb::VeloClusters* vClus = getOrCreate<LHCb::VeloClusters,
    LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default + m_extension );
  
  LHCb::STClusters* ttClus = getOrCreate<LHCb::STClusters,
    LHCb::STClusters>(LHCb::STClusterLocation::TTClusters + m_extension );
  
  LHCb::STClusters* itClus = getOrCreate<LHCb::STClusters,
    LHCb::STClusters>(LHCb::STClusterLocation::ITClusters + m_extension );

  vClus->clear();
  ttClus->clear();
  itClus->clear();
  
  for ( std::vector<LHCb::PackedCluster>::const_iterator itC = dst->clusters().begin();
        dst->clusters().end() != itC; ++itC ) {
    int det = (*itC).id >> 29;
    int id = (*itC).id & 0x1FFFFFFF;
    LHCb::VeloCluster::ADCVector adcs;
    for ( unsigned int kk = (*itC).begin; (*itC).end != kk; ++kk ) {
      adcs.push_back( std::pair<int,unsigned int>( dst->strips()[kk], dst->adcs()[kk] ) );
    }
    if ( 1 == det ) {
      LHCb::VeloChannelID vId( id & 0xFFFFFF );
      id = id >> 24;
      double frac = 0.125 * ( id & 7 );
      int size = 1;
      if ( 0 != frac ) size = 2;
      if ( ( id & 8 ) != 0 ) size = 3;
      bool high = ( id & 0x10 ) != 0;
      LHCb::VeloLiteCluster vl( vId, frac, size, high );
      LHCb::VeloCluster* vCl = new LHCb::VeloCluster( vl, adcs );
      vClus->insert( vCl, vCl->channelID() );
       
    } else if ( 2 == det || 3 == det ) {
      LHCb::STChannelID sId( id & 0xFFFFFF );
      id = id >> 24;
      double frac = 0.25 * ( id & 3 );
      int size = 1;
      if ( 0 != frac ) size = 2;
      if ( ( id & 4 ) != 0 ) size = 3;
      bool high = ( id & 8 ) != 0;
      LHCb::STLiteCluster sl( sId, frac, size, high );
      if ( 0 == (*itC).spill ) {
        LHCb::STCluster* sCl = new LHCb::STCluster( sl, adcs, 
                                                    double( (*itC).sum ), (*itC).sourceID, 
                                                    (*itC).tell1Channel, LHCb::STCluster::Central );
        if ( 2 == det ) {
          ttClus->insert( sCl, sCl->channelID() );
         } else {
          itClus->insert( sCl, sCl->channelID() );
         }
      } else {
        info() << "ST cluster from non central spill!" << endmsg;
      }
    } else {
      info() << "** Unknown detector for a cluster" << endmsg;
    }
  }

  //== If we stored in a different location, compare...
  if ( "" != m_extension ) {
    LHCb::VeloClusters* vRef = get<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default);
    for ( LHCb::VeloClusters::iterator itV = vClus->begin(); vClus->end() != itV; ++itV ) {
      LHCb::VeloCluster* vCl = *itV;
      LHCb::VeloCluster* vOld = vRef->object( vCl->key() );
      if ( ( vOld->interStripFraction() != vCl->interStripFraction() ) ||
           ( vOld->pseudoSize()         != vCl->pseudoSize() )         ||
           ( vOld->highThreshold()      != vCl->highThreshold() )      ||
           ( vOld->stripValues()        != vCl->stripValues() )        ) { 
        info() << "Old Velo Cluster " << format( "frac%5.2f size%3d thr%2d ", vOld->interStripFraction(),
                                                 vOld->pseudoSize(),  vOld->highThreshold() ) << endmsg;
        info() << " new             " << format( "frac%5.2f size%3d thr%2d ", vCl->interStripFraction(),
                                                 vCl->pseudoSize(),  vCl->highThreshold() ) << endmsg;
      }
    }
    
    LHCb::STClusters* ttRef = get<LHCb::STClusters>(LHCb::STClusterLocation::TTClusters);
    for ( LHCb::STClusters::iterator itT = ttClus->begin(); ttClus->end() != itT; ++itT ) {
      LHCb::STCluster* sCl = *itT;
      LHCb::STCluster* sOld = ttRef->object( sCl->key() );
      if ( ( sOld->interStripFraction() != sCl->interStripFraction() ) ||
           ( sOld->pseudoSize()         != sCl->pseudoSize() )         ||
           ( sOld->highThreshold()      != sCl->highThreshold() )      ||
           ( sOld->stripValues()        != sCl->stripValues() )          ) { 
        info() << "Old ST Cluster " << format( "frac%5.2f size%3d thr%2d ", sOld->interStripFraction(),
                                               sOld->pseudoSize(),  sOld->highThreshold() ) << endmsg;
        info() << " new           " << format( "frac%5.2f size%3d thr%2d ", sCl->interStripFraction(),
                                               sCl->pseudoSize(),  sCl->highThreshold() ) << endmsg;
      }
    }
    
    LHCb::STClusters* itRef = get<LHCb::STClusters>(LHCb::STClusterLocation::ITClusters);
    for ( LHCb::STClusters::iterator itI = itClus->begin(); itClus->end() != itI; ++itI ) {
      LHCb::STCluster* sCl = *itI;
      LHCb::STCluster* sOld = itRef->object( sCl->key() );
      if ( ( sOld->interStripFraction() != sCl->interStripFraction() ) ||
           ( sOld->pseudoSize()         != sCl->pseudoSize() )         ||
           ( sOld->highThreshold()      != sCl->highThreshold() )      ||
           ( sOld->stripValues()        != sCl->stripValues() )          ) { 
        info() << "Old ST Cluster " << format( "frac%5.2f size%3d thr%2d ", sOld->interStripFraction(),
                                               sOld->pseudoSize(),  sOld->highThreshold() ) << endmsg;
        info() << " new           " << format( "frac%5.2f size%3d thr%2d ", sCl->interStripFraction(),
                                               sCl->pseudoSize(),  sCl->highThreshold() ) << endmsg;
      }
    }

    info() << "Decoded " << vClus->size() << " Velo, " << ttClus->size() 
           << " TT and " << itClus->size() << " IT clusters;" << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode UnpackCluster::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
