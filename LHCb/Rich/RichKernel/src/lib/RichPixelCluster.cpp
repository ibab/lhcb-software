
//--------------------------------------------------------------------------------
/** @file RichPixelCluster.cpp
 *
 *  Implementation file for pixel clustering class Rich::DAQ::PixelCluster
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

//=========================================================================================
//   Methods for the builder
//=========================================================================================

void
HPDPixelClustersBuilder::
initialise( HPDPixelClusters * clus,
            const HPDPixelCluster::SmartIDVector & smartIDs )
{
  m_hpdClus = clus;
  m_lastID  = 0;

  // use the smartIDs to set the active pixels
  if ( !smartIDs.empty() )
  {

    // What mode are we in ?
    // note, assuming here that there is NOT a mixture of LHCb and ALICE mode hits
    // as this does not make sense (and likely not even technically possible from L1)
    setAliceMode ( smartIDs.front().pixelSubRowDataIsValid() );

    // Initialise the 'is set' array as required. If in LHCb mode, only reset the 
    // first 1/8 of the array, as this is all that is used.
    memset ( m_data, 0, 
             aliceMode() ? sizeof(m_data) : 
             (sizeof(m_data)/Rich::DAQ::NumAlicePixelsPerLHCbPixel) );
    
    // assume all hits are from the same HPD (only sensible case)
    m_hpdID = smartIDs.front().pdID();

    // set the hit pixels as "on"
    for ( const auto& S : smartIDs )
    {
      // double check all pixels are in same mode
      if ( aliceMode() != S.pixelSubRowDataIsValid() )
      {
        throw Rich::Exception( "Invalid mix of LHCb and ALICE mode RichSmartIDs" );
      }
      setOn( rowNumber(S), colNumber(S) );
    }

  }
  else  // empty hit list... just reset.
  {
    setAliceMode(false);
    m_hpdID = LHCb::RichSmartID();
  }

}

void HPDPixelClustersBuilder::setCluster( const int row, 
                                          const int col, 
                                          HPDPixelClusters::Cluster * clus )
{
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
  setCluster( tmpID, row, col, clus );
}

HPDPixelClusters::Cluster *
HPDPixelClustersBuilder::mergeClusters( HPDPixelClusters::Cluster * clus1, 
                                        HPDPixelClusters::Cluster * clus2 )
{
  // add pixels to clus1
  for ( const auto& i : clus2->pixels().smartIDs() )
  {
    setCluster( i, rowNumber(i), colNumber(i), clus1 );
  }
  // delete clus2 and remove from vector
  removeCluster( clus2 );
  // return clus1 as merged cluster
  return clus1;
}

MsgStream& HPDPixelClustersBuilder::fillStream ( MsgStream & os ) const
{

  // column numbers
  os << " c     |";
  for ( int col = 0; col < nPixelCols(); ++col )
  {
    os << boost::format("%3i") % col ;
  }
  os << endmsg;

  // print out each row
  for ( int row = 0; row < nPixelRows(); ++row )
  {
    os << boost::format( " r %3i | " ) % row ;
    for ( int col = 0; col < nPixelCols(); ++col )
    {
      const auto * clus = getCluster(row,col);
      if ( clus ) { os << boost::format("%2i ") % clus->id(); }
      else        { os << " . "; }
    }
    os << endmsg;
  }

  return os;
}

void 
HPDPixelClustersBuilder::
splitClusters( const HPDPixelClusters::Cluster::PtnVector & clusters )
{
  // loop over the clusters to break up
  for ( auto * C : clusters )
  {
    // loop over the smartIDs for this cluster
    for ( const auto & S : C->pixels().smartIDs() )
    {
      // for each ID, make a single channel new cluster
      setCluster( S, rowNumber(S), colNumber(S), createNewCluster() );
    }
    // remove and delete original cluster
    removeCluster( C );
  }
}

//=========================================================================================
//   Methods for the cluster class
//=========================================================================================

void HPDPixelClusters::suppressIDs( HPDPixelCluster::SmartIDVector & smartIDs,
                                    const unsigned int maxSize ) const
{
  // // make a local copy of the orginal IDs
  const HPDPixelCluster::SmartIDVector cache_ids(smartIDs);
  // // clear the list
  smartIDs.clear();
  // // refill the list with those not suppressed
  for ( const auto& S : cache_ids )
  {
    const auto * c = getCluster(S);
    if ( c && c->size() <= maxSize )
    {
      smartIDs.push_back(S);
    }
  }
}

const HPDPixelClusters::Cluster * 
HPDPixelClusters::getCluster( const LHCb::RichSmartID & id ) const
{
  // should look to implement this better sometime...

  // cluster pointer to return
  const Cluster * clus = NULL;

  // loop over clusters
  for ( const auto * c : clusters() )
  {
    // loop over hits in this cluster
    for ( const auto& i : c->pixels().smartIDs() )
    {
      if ( i == id )
      {
        // found the cluster...
        clus = c;
        break;
      }
    }
    if ( clus ) { break; }
  }

  // return the found cluster
  return clus;
}

MsgStream& HPDPixelClusters::fillStream( MsgStream& os ) const
{
  for ( const auto * clus : clusters() )
  {
    os << "ID=" << clus->id() << " " << clus->pixels();
  }
  return os;
}
