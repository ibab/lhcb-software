// $Id: $

#include "Event/PackedCluster.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackedCluster
//
// 2012-03-26 : Chris Jones
//-----------------------------------------------------------------------------

void LHCb::PackedClusters::addVeloCluster( const LHCb::VeloCluster* vCl )
{
  m_clusters.emplace_back( PackedCluster() );
  auto& t = m_clusters.back();
  t.id = 0x20000000 + vCl->liteCluster().channelID().channelID();
  const int size = (8*vCl->interStripFraction()) + 0.5;
  t.id += size * 0x1000000;
  if ( vCl->pseudoSize() > 2 ) t.id += 0x8000000;
  if ( vCl->highThreshold() )  t.id += 0x10000000;
  t.begin = m_strips.size();
  for ( unsigned int kk = 0; vCl->size() > kk; ++kk )
  {
    m_strips.push_back( vCl->strip(kk)    );
    m_adcs.push_back(   vCl->adcValue(kk) );
  }
  t.end = m_strips.size();
}

void LHCb::PackedClusters::addSTCluster( const LHCb::STCluster* sCl,
                                         const unsigned int key )
{
  m_clusters.emplace_back( PackedCluster() );
  auto& t = m_clusters.back();
  t.id = key + sCl->liteCluster().channelID().channelID();
  const int size = (4*sCl->interStripFraction()) + 0.5;
  t.id += size * 0x1000000;
  if ( sCl->pseudoSize() > 2 ) t.id += 0x4000000;
  if ( sCl->highThreshold() )  t.id += 0x8000000;
  t.begin = m_strips.size();
  for ( unsigned int kk = 0; sCl->size() > kk; ++kk )
  {
    m_strips.push_back( sCl->strip(kk)    );
    m_adcs.push_back(   sCl->adcValue(kk) );
  }
  t.end          = m_strips.size();
  t.sum          = sCl->neighbourSum();
  t.sourceID     = sCl->sourceID();
  t.tell1Channel = sCl->tell1Channel();
  t.spill        = int( sCl->spill() );
}
