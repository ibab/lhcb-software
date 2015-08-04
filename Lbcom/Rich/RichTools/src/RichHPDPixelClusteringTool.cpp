
//-----------------------------------------------------------------------------
/** @file RichHPDPixelClusteringTool.cpp
 *
 * Implementation file for class : RichHPDPixelClusteringTool
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   21/03/2006
 */
//-----------------------------------------------------------------------------

// local
#include "RichHPDPixelClusteringTool.h"

// RICH DAQ
using namespace Rich::DAQ;

DECLARE_TOOL_FACTORY( HPDPixelClusteringTool )

// Standard constructor
HPDPixelClusteringTool::
HPDPixelClusteringTool( const std::string& type,
                        const std::string& name,
                        const IInterface* parent )
  : ToolBase ( type, name, parent )
{
  // Define interface
  declareInterface<IPixelClusteringTool>(this);
  // job options
  declareProperty( "MinClusterSize", m_minClusSize = 1      );
  declareProperty( "MaxClusterSize", m_maxClusSize = 999999 );
  declareProperty( "AllowDiagonalsInClusters", m_allowDiags = true );
}

StatusCode HPDPixelClusteringTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  if( msgLevel(MSG::DEBUG) )
    debug() << "Using nearest neighbour pixel clustering tool" << std::endl
            << " -> Minimum cluster size        = " << m_minClusSize << std::endl
            << " -> Maximum cluster size        = " << m_maxClusSize << std::endl
            << " -> Allow diagonals in clusters = " << m_allowDiags << endmsg;

  return sc;
}

const Rich::HPDPixelClusters *
HPDPixelClusteringTool::findClusters( LHCb::RichSmartID::Vector & smartIDs ) const
{
  // make sure pixels are sorted ok
  // this should be automatic via decoding but can do it here to be sure
  // as because the vector should be in the correct order, it should be v fast
  sortIDs(smartIDs);

  // Make a local pixel data object
  HPDPixelClusters * pixelData = new HPDPixelClusters( smartIDs );

  // loop over pixels
  // requires them to be sorted by row then column
  if ( msgLevel(MSG::VERBOSE) ) 
    verbose() << "Clustering with " << smartIDs.size() << " RichSmartIDs" << endmsg;
  for ( LHCb::RichSmartID::Vector::const_iterator iS = smartIDs.begin();
        iS != smartIDs.end(); ++iS )
  {
    // Print the input hits
    if ( msgLevel(MSG::VERBOSE) ) verbose() << " -> " << *iS << endmsg;

    // get row and column data
    const int col     = pixelData->colNumber(*iS);
    const int row     = pixelData->rowNumber(*iS);
    const int lastrow = row - 1;
    const int lastcol = col - 1;
    const int nextcol = col + 1;

    // Null cluster pointer
    HPDPixelClusters::Cluster * clus(NULL);

    // check neighbouring pixels

    // last row and last column
    clus = ( m_allowDiags ? pixelData->getCluster(lastrow,lastcol) : NULL );

    // last row and same column
    HPDPixelClusters::Cluster * newclus1 = pixelData->getCluster(lastrow,col);
    if ( newclus1 )
    {
      clus = ( clus && clus != newclus1 ?
               pixelData->mergeClusters(clus,newclus1) : newclus1 );
    }

    // last row and next column
    HPDPixelClusters::Cluster * newclus2 =
      ( m_allowDiags ? pixelData->getCluster(lastrow,nextcol) : NULL );
    if ( newclus2 )
    {
      clus = ( clus && clus != newclus2 ?
               pixelData->mergeClusters(clus,newclus2) : newclus2 );
    }

    // this row and last column
    HPDPixelClusters::Cluster * newclus3 = pixelData->getCluster(row,lastcol);
    if ( newclus3 )
    {
      clus = ( clus && clus != newclus3 ?
               pixelData->mergeClusters(clus,newclus3) : newclus3 );
    }

    // Did we find a neighbouring pixel cluster ?
    // If not, this is a new cluster
    if (!clus) { clus = pixelData->createNewCluster(); }

    // assign final cluster to this pixel
    pixelData->setCluster(row,col,clus);

  } // pixel loop

  // Check final clusters are OK
  HPDPixelClusters::Cluster::PtnVector clustersToSplit;
  for ( HPDPixelClusters::Cluster::PtnVector::const_iterator iC = pixelData->clusters().begin();
        iC != pixelData->clusters().end() ; ++iC )
  {
    if ( (*iC)->size() < m_minClusSize || (*iC)->size() > m_maxClusSize )
    {
      clustersToSplit.push_back( *iC );
    }
  }
  if ( !clustersToSplit.empty() ) pixelData->splitClusters(clustersToSplit);
  
  // print the final cluster
  if ( msgLevel(MSG::VERBOSE) ) { verbose() << *pixelData << endmsg; }

  // finally, return a pointer to this cluster object
  // user takes ownership at this point
  return pixelData;
}
