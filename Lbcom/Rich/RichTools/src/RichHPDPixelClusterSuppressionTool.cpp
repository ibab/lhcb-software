
//-----------------------------------------------------------------------------
/** @file RichHPDPixelClusterSuppressionTool.cpp
 *
 * Implementation file for class : RichHPDPixelClusterSuppressionTool
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   21/03/2006
 */
//-----------------------------------------------------------------------------

// local
#include "RichHPDPixelClusterSuppressionTool.h"

// RICH DAQ
using namespace Rich::DAQ;

DECLARE_TOOL_FACTORY( HPDPixelClusterSuppressionTool )

// Standard constructor
HPDPixelClusterSuppressionTool::
HPDPixelClusterSuppressionTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : HighOccHPDSuppressionTool ( type, name, parent )
{
  // Define interface
  declareInterface<IPixelSuppressionTool>(this);
  // job options
  declareProperty( "MaxPixelClusterSize",    m_maxPixClusterSize  = 10 );
  declareProperty( "MinHPDOccForClustering", m_minHPDocc          = 10 );
}

StatusCode HPDPixelClusterSuppressionTool::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = HighOccHPDSuppressionTool::initialize();
  if ( sc.isFailure() ) return sc;

  // get the clustering tool
  acquireTool( "ClusteringTool", m_clustTool, this );

  // sanity checks
  if ( m_maxPixClusterSize > m_minHPDocc ) m_minHPDocc = m_maxPixClusterSize;

  _ri_debug << "  Max HPD pixel cluster size            = " << m_maxPixClusterSize << std::endl
            << "  Min HPD occupancy for clustering      = " << m_minHPDocc << endmsg;

  return sc;
}

bool
HPDPixelClusterSuppressionTool::
applyPixelSuppression( const LHCb::RichSmartID hpdID,
                       LHCb::RichSmartID::Vector & smartIDs ) const
{
  // check overall HPD suppression and update running average occupancy
  bool suppress =
    HighOccHPDSuppressionTool::applyPixelSuppression( hpdID, smartIDs );
  if ( suppress ) return true;

  // number of pixels before suppression
  const unsigned int startSize = smartIDs.size();

  // if occ below min for clustering, just return false (no suppression)
  if ( startSize < m_minHPDocc ) return false;

  // find the clusters
  const HPDPixelClusters * clusters = m_clustTool->findClusters( smartIDs );
 
  // Print out clustering results
  _ri_verbo << hpdID << endmsg << *clusters << endmsg;
  
  // apply pixel cluster suppression
  clusters->suppressIDs(smartIDs,m_maxPixClusterSize);

  // was anything removed ?
  suppress = ( startSize != smartIDs.size() );

  if ( m_sumPrint && suppress )
  {
    // Print message
    std::ostringstream hpd;
    hpd << hpdID;
    Info( "Partially suppressed "+hpd.str(), StatusCode::SUCCESS, 0 );
  }

  // cleanup
  delete clusters;

  // return status
  return suppress;
}
