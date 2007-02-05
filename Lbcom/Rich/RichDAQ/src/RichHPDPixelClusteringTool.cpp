
//-----------------------------------------------------------------------------
/** @file RichHPDPixelClusteringTool.cpp
 *
 * Implementation file for class : RichHPDPixelClusteringTool
 *
 * CVS Log :-
 * $Id: RichHPDPixelClusteringTool.cpp,v 1.4 2007-02-05 15:02:35 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   21/03/2006
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ToolFactory.h"

// local
#include "RichHPDPixelClusteringTool.h"

// RICH DAQ
using namespace Rich::DAQ;

DECLARE_TOOL_FACTORY( HPDPixelClusteringTool );

// Standard constructor
HPDPixelClusteringTool::
HPDPixelClusteringTool( const std::string& type,
                        const std::string& name,
                        const IInterface* parent )
  : ToolBase ( type, name, parent )
{
  // Define interface
  declareInterface<IPixelClusteringTool>(this);
}

StatusCode HPDPixelClusteringTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  info() << "  Using nearest neighbour pixel clustering tool" << endreq;

  return sc;
}

const Rich::HPDPixelClusters::ConstSharedPtn
HPDPixelClusteringTool::findClusters( LHCb::RichSmartID::Vector & smartIDs ) const
{

  // Make a local pixel data object
  HPDPixelClusters * pixelData = new HPDPixelClusters( smartIDs );

  // cluster numbers
  int clusterID(0);

  // loop over pixels
  // requires them to be sorted by row then column
  // this should be automatic via decoding but its fast so safer to be sure
  SmartIDSorter::sortByRegion(smartIDs);
  for ( LHCb::RichSmartID::Vector::const_iterator iS = smartIDs.begin();
        iS != smartIDs.end(); ++iS )
  {
    // get row and column data
    const int col     = (*iS).pixelCol();
    const int row     = (*iS).pixelRow();
    const int lastrow = row - 1;
    const int lastcol = col - 1;
    const int nextcol = col + 1;

    // Null cluster pointer
    HPDPixelClusters::Cluster * clus(NULL);

    // check neighbouring pixels

    // last row and last column
    clus = pixelData->getCluster(lastrow,lastcol);

    // last row and same column
    HPDPixelClusters::Cluster * newclus1 = pixelData->getCluster(lastrow,col);
    if ( newclus1 )
    {
      clus = ( clus && clus != newclus1 ? 
               pixelData->mergeClusters(clus,newclus1) : newclus1 );
    }

    // last row and next column
    HPDPixelClusters::Cluster * newclus2 = pixelData->getCluster(lastrow,nextcol);
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
    if (!clus) { clus = pixelData->createNewCluster(++clusterID); }

    // assign final cluster to this pixel
    pixelData->setCluster(row,col,clus);

  } // pixel loop

  // finally, return a shared auto smart pointer to this cluster object
  return HPDPixelClusters::ConstSharedPtn(pixelData);
}
