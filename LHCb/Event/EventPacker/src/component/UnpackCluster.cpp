// $Id: $
// Include files
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
                                ISvcLocator* pSvcLocator )
    : GaudiAlgorithm ( name, pSvcLocator )
{
  declareProperty( "InputName" , m_inputName = LHCb::PackedClusterLocation::Default );
  declareProperty( "Extension",  m_extension = "" );
  declareProperty( "AlwaysCreateOutput", m_alwaysOutput = false );
  //setProperty( "OutputLevel", 1 );
}

//=============================================================================
// explicit comparison for cluster sorting (hidden in anonymous namespace)
//=============================================================================
namespace 
{
  template < class TYPE >
  inline bool compareKeys( const TYPE * a, const TYPE * b ) 
  {
    return ( a->key() < b->key() );
  }
}

//=============================================================================
// Destructor
//=============================================================================
UnpackCluster::~UnpackCluster() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackCluster::execute()
{
  if ( m_running ) return StatusCode::SUCCESS;
  m_running = true;
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // If input does not exist, and we aren't making the output regardless, just return
  if ( !m_alwaysOutput &&
       !exist<LHCb::PackedClusters>(m_inputName) ) return StatusCode::SUCCESS;

  // If any clusters already exist, return
  if ( exist<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default+m_extension)  ||
       exist<LHCb::STClusters>  (LHCb::STClusterLocation::TTClusters+m_extension) ||
       exist<LHCb::STClusters>  (LHCb::STClusterLocation::UTClusters+m_extension) ||
       exist<LHCb::STClusters>  (LHCb::STClusterLocation::ITClusters+m_extension)  )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Clusters already exist -> Quitting" << endmsg;
    return StatusCode::SUCCESS;
  }

  // Reset cluster pointers, to force new ones to be creatd when needed next time
  m_vClus  = nullptr;
  m_ttClus = nullptr;
  m_utClus = nullptr;
  m_itClus = nullptr;

  // Force creation of non-upgrade locatons. 
  // Temporary, need to check if all works fine with these removed.
  vClus();
  ttClus();
  itClus();

  // Get the packed data
  const LHCb::PackedClusters* dst =
    getOrCreate<LHCb::PackedClusters,LHCb::PackedClusters>( m_inputName );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Size of packed clusters = " << dst->clusters().size() << " clusters" << endmsg;
  }

  // Fill the clusters
  for ( const auto& clu : dst->clusters() )
  {
    
    const int det = clu.id >> 29;
    int id        = clu.id & 0x1FFFFFFF;
    LHCb::VeloCluster::ADCVector adcs;
    for ( unsigned int kk = clu.begin; clu.end != kk; ++kk )
    {
      adcs.push_back( std::pair<int,unsigned int>( dst->strips()[kk],
                                                   dst->adcs()[kk]  ) );
    }
    if ( 1 == det )
    {
      const LHCb::VeloChannelID vId( id & 0xFFFFFF );
      id = id >> 24;
      const double frac = 0.125 * ( id & 7 );
      int size = 1;
      if ( 0 != frac ) size = 2;
      if ( ( id & 8 ) != 0 ) size = 3;
      bool high = ( id & 0x10 ) != 0;
      const LHCb::VeloLiteCluster vl( vId, frac, size, high );
      LHCb::VeloCluster* vCl = new LHCb::VeloCluster( vl, adcs );
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << " Unpacked " << vCl->channelID() << endmsg;
      try 
      {
        vClus()->insert( vCl, vCl->channelID() );
      }
      catch ( const GaudiException & excpt )
      {
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << excpt.message() << endmsg;
        delete vCl;
      }
    }
    else if ( 2 == det || 3 == det )
    {
      const LHCb::STChannelID sId( id & 0xFFFFFF );
      id = id >> 24;
      const double frac = 0.25 * ( id & 3 );
      int size = 1;
      if ( 0 != frac ) size = 2;
      if ( ( id & 4 ) != 0 ) size = 3;
      bool high = ( id & 8 ) != 0;
      const LHCb::STLiteCluster sl( sId, frac, size, high );
      if ( 0 == clu.spill )
      {
        LHCb::STCluster* sCl = new LHCb::STCluster( sl, adcs,
                                                    double(clu.sum),
                                                    clu.sourceID,
                                                    clu.tell1Channel,
                                                    LHCb::STCluster::Central );
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << " Unpacked " << sCl->channelID() << endmsg;
        try 
        {
          if ( 2 == det )
          {
            ttClus()->insert( sCl, sCl->channelID() );
          }
          else
          {
            itClus()->insert( sCl, sCl->channelID() );
          }
        }
        catch ( const GaudiException & excpt )
        {
          if ( msgLevel(MSG::VERBOSE) )
            verbose() << excpt.message() << endmsg;
          delete sCl;
        }
      }
      else
      {
        Warning( "ST cluster from non central spill !" ).ignore();
      }
    }
    else
    {
      Warning( "Unknown detector for a cluster !").ignore();
    }

  } // end loop over clusters

  // Sort any filled containers.
  if ( m_vClus  ) { std::sort( m_vClus->begin(),  m_vClus->end(),  compareKeys<LHCb::VeloCluster> ); }
  if ( m_ttClus ) { std::sort( m_ttClus->begin(), m_ttClus->end(), compareKeys<LHCb::STCluster>   ); }
  if ( m_utClus ) { std::sort( m_utClus->begin(), m_utClus->end(), compareKeys<LHCb::STCluster>   ); }
  if ( m_itClus ) { std::sort( m_itClus->begin(), m_itClus->end(), compareKeys<LHCb::STCluster>   ); }

  //== If we stored in a different location, compare...
  if ( UNLIKELY(!m_extension.empty()) )
  {

    LHCb::VeloClusters* vRef = get<LHCb::VeloClusters>(LHCb::VeloClusterLocation::Default);
    for ( LHCb::VeloClusters::iterator itV = vClus()->begin(); vClus()->end() != itV; ++itV )
    {
      LHCb::VeloCluster* vCl = *itV;
      LHCb::VeloCluster* vOld = vRef->object( vCl->key() );
      if ( ( vOld->interStripFraction() != vCl->interStripFraction() ) ||
           ( vOld->pseudoSize()         != vCl->pseudoSize() )         ||
           ( vOld->highThreshold()      != vCl->highThreshold() )      ||
           ( vOld->stripValues()        != vCl->stripValues() )         )
      {
        info() << "Old Velo Cluster " << format( "frac%5.2f size%3d thr%2d ", vOld->interStripFraction(),
                                                 vOld->pseudoSize(),  vOld->highThreshold() ) << endmsg;
        info() << " new             " << format( "frac%5.2f size%3d thr%2d ", vCl->interStripFraction(),
                                                 vCl->pseudoSize(),  vCl->highThreshold() ) << endmsg;
      }
    }

    LHCb::STClusters* ttRef = get<LHCb::STClusters>(LHCb::STClusterLocation::TTClusters);
    for ( LHCb::STClusters::iterator itT = ttClus()->begin(); ttClus()->end() != itT; ++itT )
    {
      LHCb::STCluster* sCl = *itT;
      LHCb::STCluster* sOld = ttRef->object( sCl->key() );
      if ( ( sOld->interStripFraction() != sCl->interStripFraction() ) ||
           ( sOld->pseudoSize()         != sCl->pseudoSize() )         ||
           ( sOld->highThreshold()      != sCl->highThreshold() )      ||
           ( sOld->stripValues()        != sCl->stripValues() )         )
      {
        info() << "Old ST Cluster " << format( "frac%5.2f size%3d thr%2d ", sOld->interStripFraction(),
                                               sOld->pseudoSize(),  sOld->highThreshold() ) << endmsg;
        info() << " new           " << format( "frac%5.2f size%3d thr%2d ", sCl->interStripFraction(),
                                               sCl->pseudoSize(),  sCl->highThreshold() ) << endmsg;
      }
    }

    LHCb::STClusters* utRef = get<LHCb::STClusters>(LHCb::STClusterLocation::UTClusters);
    for ( LHCb::STClusters::iterator itU = utClus()->begin(); utClus()->end() != itU; ++itU )
    {
      LHCb::STCluster* sCl = *itU;
      LHCb::STCluster* sOld = utRef->object( sCl->key() );
      if ( ( sOld->interStripFraction() != sCl->interStripFraction() ) ||
           ( sOld->pseudoSize()         != sCl->pseudoSize() )         ||
           ( sOld->highThreshold()      != sCl->highThreshold() )      ||
           ( sOld->stripValues()        != sCl->stripValues() )         )
      {
        info() << "Old ST Cluster " << format( "frac%5.2f size%3d thr%2d ", sOld->interStripFraction(),
                                               sOld->pseudoSize(),  sOld->highThreshold() ) << endmsg;
        info() << " new           " << format( "frac%5.2f size%3d thr%2d ", sCl->interStripFraction(),
                                               sCl->pseudoSize(),  sCl->highThreshold() ) << endmsg;
      }
    }

    LHCb::STClusters* itRef = get<LHCb::STClusters>(LHCb::STClusterLocation::ITClusters);
    for ( LHCb::STClusters::iterator itI = itClus()->begin(); itClus()->end() != itI; ++itI )
    {
      LHCb::STCluster* sCl = *itI;
      LHCb::STCluster* sOld = itRef->object( sCl->key() );
      if ( ( sOld->interStripFraction() != sCl->interStripFraction() ) ||
           ( sOld->pseudoSize()         != sCl->pseudoSize() )         ||
           ( sOld->highThreshold()      != sCl->highThreshold() )      ||
           ( sOld->stripValues()        != sCl->stripValues() )         )
      {
        info() << "Old ST Cluster " << format( "frac%5.2f size%3d thr%2d ", sOld->interStripFraction(),
                                               sOld->pseudoSize(),  sOld->highThreshold() ) << endmsg;
        info() << " new           " << format( "frac%5.2f size%3d thr%2d ", sCl->interStripFraction(),
                                               sCl->pseudoSize(),  sCl->highThreshold() ) << endmsg;
      }
    }

    info() << "Decoded " << vClus()->size() << " Velo, " << ttClus()->size()
           << " TT and " << utClus()->size() << " UT, " << itClus()->size() << " IT clusters;" << endmsg;
  }

  m_running = false;
  return StatusCode::SUCCESS;
}

//=============================================================================
