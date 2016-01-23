
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
: ToolBase ( type, name, parent ),
  m_splitClusters ( false )
{
  // Define interface
  declareInterface<IPixelClusteringTool>(this);
  // job options
  declareProperty( "MinClusterSize", m_minClusSize = 1      );
  declareProperty( "MaxClusterSize", m_maxClusSize = 999999 );
  declareProperty( "AllowDiagonalsInClusters", m_allowDiags = true );
  //setProperty( "OutputLevel", 1 );
}

StatusCode HPDPixelClusteringTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  m_splitClusters = ( m_minClusSize > 1 || m_maxClusSize < 999999 );

  _ri_debug << "Using nearest neighbour pixel clustering tool" << endmsg
            << " -> Minimum cluster size        = " << m_minClusSize << endmsg
            << " -> Maximum cluster size        = " << m_maxClusSize << endmsg
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

  // Make a new pixel clusters object to return
  HPDPixelClusters * pixelData = new HPDPixelClusters();

  // Initialise the builder
  m_clusterBuilder.initialise( pixelData, smartIDs );

  // loop over pixels
  // requires them to be sorted by row then column
  _ri_verbo << "Clustering with " << smartIDs.size() << " RichSmartIDs" << endmsg;
  for ( const auto& S : smartIDs )
  {
    // Print the input hits
    _ri_verbo << " -> " << S << endmsg;

    // get row and column data
    const int col     = m_clusterBuilder.colNumber(S);
    const int row     = m_clusterBuilder.rowNumber(S);
    const int lastrow = row - 1;
    const int lastcol = col - 1;
    const int nextcol = col + 1;

    // Null cluster pointer
    HPDPixelClusters::Cluster * clus(nullptr);

    // check neighbouring pixels

    // last row and last column
    clus = ( m_allowDiags ? m_clusterBuilder.getCluster(lastrow,lastcol) : nullptr );

    // last row and same column
    HPDPixelClusters::Cluster * newclus1 = m_clusterBuilder.getCluster(lastrow,col);
    if ( newclus1 )
    {
      clus = ( clus && clus != newclus1 ?
               m_clusterBuilder.mergeClusters(clus,newclus1) : newclus1 );
    }

    // last row and next column
    HPDPixelClusters::Cluster * newclus2 =
      ( m_allowDiags ? m_clusterBuilder.getCluster(lastrow,nextcol) : nullptr );
    if ( newclus2 )
    {
      clus = ( clus && clus != newclus2 ?
               m_clusterBuilder.mergeClusters(clus,newclus2) : newclus2 );
    }

    // this row and last column
    HPDPixelClusters::Cluster * newclus3 = m_clusterBuilder.getCluster(row,lastcol);
    if ( newclus3 )
    {
      clus = ( clus && clus != newclus3 ?
               m_clusterBuilder.mergeClusters(clus,newclus3) : newclus3 );
    }

    // Did we find a neighbouring pixel cluster ?
    // If not, this is a new cluster
    if ( !clus ) { clus = m_clusterBuilder.createNewCluster(); }

    // assign final cluster to this pixel
    m_clusterBuilder.setCluster(S,row,col,clus);

  } // pixel loop

  // Check final clusters are OK
  if ( UNLIKELY(m_splitClusters) )
  {
    static HPDPixelClusters::Cluster::PtnVector clustersToSplit;
    for ( auto * C : pixelData->clusters() )
    {
      if ( C->size() < m_minClusSize || 
           C->size() > m_maxClusSize ) { clustersToSplit.push_back(C); }
    }
    if ( !clustersToSplit.empty() ) 
    {
      // split the selected clusters
      m_clusterBuilder.splitClusters(clustersToSplit); 
      // clear for next time
      clustersToSplit.clear();
    }
  }
  
  // print the final cluster
  _ri_verbo << m_clusterBuilder << endmsg;
  //_ri_verbo << *pixelData       << endmsg;

  // finally, return a pointer to this cluster object
  // user takes ownership at this point
  return pixelData;
}
