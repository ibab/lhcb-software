
//-----------------------------------------------------------------------------
/** @file RichHPDPixelClusterSuppressionTool.cpp
 *
 * Implementation file for class : RichHPDPixelClusterSuppressionTool
 *
 * CVS Log :-
 * $Id: RichHPDPixelClusterSuppressionTool.cpp,v 1.10 2006-08-28 10:56:09 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   21/03/2006
 */
//-----------------------------------------------------------------------------

// local
#include "RichHPDPixelClusterSuppressionTool.h"

// Declaration of the Tool Factory
static const  ToolFactory<RichHPDPixelClusterSuppressionTool>          s_factory ;
const        IToolFactory& RichHPDPixelClusterSuppressionToolFactory = s_factory ;

// Standard constructor
RichHPDPixelClusterSuppressionTool::
RichHPDPixelClusterSuppressionTool( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : RichHighOccHPDSuppressionTool ( type, name, parent )
{
  // Define interface
  declareInterface<IRichPixelSuppressionTool>(this);
  // job options
  declareProperty( "MaxPixelClusterSize",    m_maxPixClusterSize  = 10 );
  declareProperty( "MinHPDOccForClustering", m_minHPDocc          = 10 );
  declareProperty( "MaxAverageHPDOccForClustering", m_maxAvHPDOcc = 10 );
}

StatusCode RichHPDPixelClusterSuppressionTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichHighOccHPDSuppressionTool::initialize();
  if ( sc.isFailure() ) return sc;

  // sanity checks
  if ( m_maxPixClusterSize > m_minHPDocc ) m_minHPDocc = m_maxPixClusterSize;

  info() << "  Max HPD pixel cluster size            = " << m_maxPixClusterSize << endreq
         << "  Min HPD occupancy for clustering      = " << m_minHPDocc << endreq
         << "  Max HPD av. occupancy for clustering  = " << m_maxAvHPDOcc << endreq;

  return sc;
}

StatusCode RichHPDPixelClusterSuppressionTool::finalize()
{
  // Execute base class method
  return RichHighOccHPDSuppressionTool::finalize();
}

bool
RichHPDPixelClusterSuppressionTool::
applyPixelSuppression( const LHCb::RichSmartID hpdID,
                       LHCb::RichSmartID::Vector & smartIDs ) const
{
  // check overall HPD suppression and update running average occupancy
  bool suppress =
    RichHighOccHPDSuppressionTool::applyPixelSuppression( hpdID, smartIDs );
  if ( suppress ) return true;

  // Get HPD occupancy data
  HPDData & data = hpdData(hpdID);

  // Check if HPD average occupancy is too high to do clustering
  if ( m_maxAvHPDOcc < data.avOcc() ) return true;

  // number of pixels before suppression
  const unsigned int startSize = smartIDs.size();

  // get occupancy data for this HPD
  if ( startSize < m_minHPDocc ) return false;

  // Make a local pixel data object
  PixelData pixelData( smartIDs );

  // cluster numbers
  int clusterID(0);

  // loop over pixels
  // requires them to be sorted by row then column
  // this should be automatic via decoding but its fast so safer to be sure
  const RichSmartIDSorter sorter;
  sorter.sortByRegion(smartIDs);
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
    PixelData::Cluster * clus(NULL);

    // check neighbouring pixels
    if ( pixelData.isOn(lastrow,lastcol) )
    {
      clus = pixelData.getCluster(lastrow,lastcol);
    }
    if ( pixelData.isOn(lastrow,col) )
    {
      PixelData::Cluster * newclus = pixelData.getCluster(lastrow,col);
      if ( clus && newclus && clus != newclus )
      { clus = pixelData.mergeClusters(clus,newclus); }
      else { clus = newclus; }
    }
    if ( pixelData.isOn(lastrow,nextcol) )
    {
      PixelData::Cluster * newclus = pixelData.getCluster(lastrow,nextcol);
      if ( clus && newclus && clus != newclus )
      { clus = pixelData.mergeClusters(clus,newclus); }
      else { clus = newclus; }
    }
    if ( pixelData.isOn(row,lastcol) )
    {
      PixelData::Cluster * newclus = pixelData.getCluster(row,lastcol);
      if ( clus && newclus && clus != newclus )
      { clus = pixelData.mergeClusters(clus,newclus); }
      else { clus = newclus; }
    }

    // Did we find a neighbouring pixel cluster
    // If not, this is a new cluster
    if (!clus) { clus = pixelData.createNewCluster(++clusterID); }

    // assign final cluster to this pixel
    pixelData.setCluster(row,col,clus);

  } // pixel loop

  // Print out clustering results
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << hpdID << endreq << pixelData << endreq;
  }

  // apply pixel cluster suppression
  pixelData.suppressIDs(smartIDs,m_maxPixClusterSize);

  // was anything removed ?
  suppress = ( startSize != smartIDs.size() );

  if ( m_sumPrint && suppress )
  {
    // Print message
    std::ostringstream hpd;
    hpd << hpdID;
    Warning( "Partially suppressed HPD "+hpd.str(), StatusCode::SUCCESS, 10 );
  }

  // return status
  return suppress;
}

// ======================= Methods for PixelData ================================

MsgStream& RichHPDPixelClusterSuppressionTool::
PixelData::fillStream ( MsgStream & os ) const
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

void RichHPDPixelClusterSuppressionTool::
PixelData::suppressIDs( LHCb::RichSmartID::Vector & smartIDs,
                        const unsigned int maxSize )
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
