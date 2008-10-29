
//--------------------------------------------------------------------------------
/** @file RichPixelCluster.cpp
 *
 *  Implementation file for pixel clustering class Rich::DAQ::PixelCluster
 *
 *  CVS Log :-
 *  $Id: RichPixelCluster.cpp,v 1.7 2008-10-29 11:07:15 cattanem Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   02/02/2007
 */
//--------------------------------------------------------------------------------

// local
#include "RichKernel/RichPixelCluster.h"
#include "RichKernel/RichException.h"

// boost
#include "boost/format.hpp"

#include <cstring> // for memset with gcc 4.3

using namespace Rich;

HPDPixelClusters::HPDPixelClusters( const HPDPixelCluster::SmartIDVector & smartIDs )
  : m_lastID    ( 0     ),
    m_aliceMode ( false )
{

  // initialise the c arrays
  // only do the first 1/8 - To cover LHCb mode only
  // if Alice mode is needed then set the rest when needed
  memset ( m_data,     0, sizeof(m_data)     / Rich::DAQ::NumAlicePixelsPerLHCbPixel );
  memset ( m_clusters, 0, sizeof(m_clusters) / Rich::DAQ::NumAlicePixelsPerLHCbPixel );

  // use the smartIDs to set the active pixels
  if ( !smartIDs.empty() )
  {

    // What mode are we in ?
    // note, assuming here that there is NOT a mixture of LHCb and ALICE mode hits
    // as this does not make sense (and likely not even technically possible from L1)
    setAliceMode ( smartIDs.front().pixelSubRowDataIsValid() );
    if ( aliceMode() )
    {
      memset ( m_data,     0, sizeof(m_data)     );
      memset ( m_clusters, 0, sizeof(m_clusters) );
    }

    // assume all hits are from the same HPD (only sensible case)
    m_hpdID = smartIDs.front().hpdID();

    // set the hit pixels as "on"
    for ( HPDPixelCluster::SmartIDVector::const_iterator iS = smartIDs.begin();
          iS != smartIDs.end(); ++iS )
    {
      // double check all pixels are in same mode
      if ( aliceMode() != (*iS).pixelSubRowDataIsValid() )
      {
        throw Rich::Exception( "Invalid mix of LHCb and ALICE mode RichSmartIDs" );
      }
      setOn( rowNumber(*iS), colNumber(*iS) );
    }

    // reserve size for 5 pixels in cluster
    //m_allclus.reserve(5); 

  } // smartids not empty

}

void HPDPixelClusters::setCluster( const int row, const int col, Cluster * clus )
{
  // Set the pointer to the cluster for this (row,col)
  (m_clusters[row])[col] = clus;
  // set the pixel RichSmartID accordingly (ALICE or LHCb mode)
  LHCb::RichSmartID tmpID(m_hpdID);
  tmpID.setPixelCol(col);
  if ( !aliceMode() )
  {
    tmpID.setPixelRow    ( row );
  }
  else
  {
    tmpID.setPixelRow    ( row / Rich::DAQ::NumAlicePixelsPerLHCbPixel );
    tmpID.setPixelSubRow ( row % Rich::DAQ::NumAlicePixelsPerLHCbPixel );
  }
  // save this hit to the list of pixels for this cluster
  clus->addPixel(tmpID);
}

HPDPixelClusters::Cluster *
HPDPixelClusters::mergeClusters( Cluster * clus1, Cluster * clus2 )
{
  // add pixels to clus1
  for ( HPDPixelCluster::SmartIDVector::const_iterator i = clus2->pixels().smartIDs().begin();
        i != clus2->pixels().smartIDs().end(); ++i )
  {
    setCluster( rowNumber(*i), colNumber(*i), clus1 );
  }
  // delete clus2 and remove from vector
  removeCluster( clus2 );
  // return clus1 as merged cluster
  return clus1;
}

MsgStream& HPDPixelClusters::fillStream ( MsgStream & os ) const
{
  // column numbers
  os << " c     |";
  for ( int col = 0; col < nPixelCols(); ++col )
  {
    os << boost::format("%3i") % col ;
  }
  os << endreq;

  // print out each row
  for ( int row = 0; row < nPixelRows(); ++row )
  {
    os << boost::format( " r %3i | " ) % row ;
    for ( int col = 0; col < nPixelCols(); ++col )
    {
      const Cluster * clus = getCluster(row,col);
      if ( clus ) { os << boost::format("%2i ") % clus->id(); }
      else        { os << " . "; }
    }
    os << endreq;
  }

  return os;
}

void HPDPixelClusters::suppressIDs( HPDPixelCluster::SmartIDVector & smartIDs,
                                    const unsigned int maxSize ) const
{
  HPDPixelCluster::SmartIDVector newSmartIDs;
  newSmartIDs.reserve(smartIDs.size());
  for ( HPDPixelCluster::SmartIDVector::const_iterator iS = smartIDs.begin();
        iS != smartIDs.end(); ++iS )
  {
    if ( getCluster(rowNumber(*iS),colNumber(*iS))->size() <= maxSize )
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
    for ( HPDPixelCluster::SmartIDVector::const_iterator iS = (*iC)->pixels().smartIDs().begin();
          iS != (*iC)->pixels().smartIDs().end(); ++iS )
    {
      // for each ID, make a single channel new cluster
      Cluster * newClus = createNewCluster();
      setCluster( rowNumber(*iS), colNumber(*iS), newClus );
    }
    // remove and delete original cluster
    removeCluster( *iC );
  } // loop over clusters
}
