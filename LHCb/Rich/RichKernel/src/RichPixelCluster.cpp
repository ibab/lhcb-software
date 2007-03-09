
//--------------------------------------------------------------------------------
/** @file RichPixelCluster.cpp
 *
 *  Implementation file for pixel clustering class Rich::DAQ::PixelCluster
 *
 *  CVS Log :-
 *  $Id: RichPixelCluster.cpp,v 1.2 2007-03-09 17:58:13 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   02/02/2007
 */
//--------------------------------------------------------------------------------

// local
#include "RichKernel/RichPixelCluster.h"

using namespace Rich;

HPDPixelClusters::HPDPixelClusters( const LHCb::RichSmartID::Vector & smartIDs )
  : m_lastID ( 0 )
{
  // initialise the c arrays
  memset ( m_data,     0, sizeof(m_data)     );
  memset ( m_clusters, 0, sizeof(m_clusters) );
  // use the smartIDs
  if ( !smartIDs.empty() )
  {
    // set the hit pixels as "on"
    for ( LHCb::RichSmartID::Vector::const_iterator iS = smartIDs.begin();
          iS != smartIDs.end(); ++iS )
    {
      setOn( (*iS).pixelRow(), (*iS).pixelCol() );
    }
    m_hpdID = smartIDs.front().hpdID();
  }
}

HPDPixelClusters::Cluster *
HPDPixelClusters::mergeClusters( Cluster * clus1, Cluster * clus2 )
{
  // add pixels to clus1
  for ( LHCb::RichSmartID::Vector::const_iterator i = clus2->pixels().smartIDs().begin();
        i != clus2->pixels().smartIDs().end(); ++i )
  {
    setCluster( (*i).pixelRow(), (*i).pixelCol(), clus1 );
  }
  // delete clus2 and remove from vector
  removeCluster( clus2 );
  // return clus1 as merged cluster
  return clus1;
}

HPDPixelClusters::~HPDPixelClusters()
{
  for ( Cluster::PtnVector::iterator i = m_allclus.begin();
        i != m_allclus.end(); ++i ) { delete *i; }
}

MsgStream& HPDPixelClusters::fillStream ( MsgStream & os ) const
{
  // column numbers
  os << " c    |";
  for ( int col = 0; col < nPixelRowsOrCols; ++col )
  {
    os << format("%3i",col);
  }
  os << endreq;

  // print out each row
  for ( int row = 0; row < nPixelRowsOrCols; ++row )
  {
    os << format( " r %2i | ", row );
    for ( int col = 0; col < nPixelRowsOrCols; ++col )
    {
      const Cluster * clus = getCluster(row,col);
      if ( clus ) { os << format("%2i ",clus->id()); }
      else        { os << " . "; }
    }
    os << endreq;
  }

  return os;
}

void HPDPixelClusters::suppressIDs( LHCb::RichSmartID::Vector & smartIDs,
                                    const unsigned int maxSize ) const
{
  LHCb::RichSmartID::Vector newSmartIDs;
  newSmartIDs.reserve(smartIDs.size());
  for ( LHCb::RichSmartID::Vector::const_iterator iS = smartIDs.begin();
        iS != smartIDs.end(); ++iS )
  {
    if ( getCluster((*iS).pixelRow(),(*iS).pixelCol())->size() <= maxSize )
    {
      newSmartIDs.push_back(*iS);
    }
  }
  smartIDs = newSmartIDs;
}

void HPDPixelClusters::splitClusters( const Cluster::PtnVector & clusters )
{
  // loop over the clusters to break up
  for ( Cluster::PtnVector::const_iterator iC = clusters.begin();
        iC != clusters.end(); ++iC )
  {
    // loop over the smartIDs for this cluster
    for ( LHCb::RichSmartID::Vector::const_iterator iS = (*iC)->pixels().smartIDs().begin();
          iS != (*iC)->pixels().smartIDs().end(); ++iS )
    {
      // for each ID, make a single channel new cluster
      Cluster * newClus = createNewCluster();
      setCluster( (*iS).pixelRow(), (*iS).pixelCol(), newClus );
    }
    // remove and delete original cluster
    removeCluster( *iC );
  } // loop over clusters
}
