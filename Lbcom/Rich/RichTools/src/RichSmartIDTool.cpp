
//-----------------------------------------------------------------------------
/** @file RichSmartIDTool.cpp
 *
 * Implementation file for class : RichSmartIDTool
 *
 * CVS Log :-
 * $Id: RichSmartIDTool.cpp,v 1.32 2007-07-20 09:27:40 jonrob Exp $
 *
 * @author Antonis Papanestis
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2003-10-28
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ToolFactory.h"

// local
#include "RichSmartIDTool.h"

DECLARE_NAMESPACE_TOOL_FACTORY( Rich, SmartIDTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Rich::SmartIDTool::SmartIDTool( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : RichToolBase( type, name, parent )
{
  declareInterface<ISmartIDTool>(this);
}

//=============================================================================
// Destructor
Rich::SmartIDTool::~SmartIDTool() {}
//=============================================================================

//=============================================================================
StatusCode Rich::SmartIDTool::initialize()
{

  // Initialise base class
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  m_richS = getDet<DeRichSystem>(DeRichLocation::RichSystem);

  // HPD panel names
  const std::string pdPanelName[2][2] = { { DeRichHPDPanelLocation::Rich1Panel0,
                                            DeRichHPDPanelLocation::Rich1Panel1 },
                                          { DeRichHPDPanelLocation::Rich2Panel0,
                                            DeRichHPDPanelLocation::Rich2Panel1 } };

  //loop over riches and photo detector panels
  for ( unsigned int rich = 0; rich < m_photoDetPanels.size(); ++rich )
  {
    for ( unsigned int panel = 0; panel < m_photoDetPanels[rich].size(); ++panel )
    {
      m_photoDetPanels[rich][panel] = getDet<DeRichHPDPanel>( pdPanelName[rich][panel] );
      debug() << "Stored photodetector panel "
              << m_photoDetPanels[rich][panel]->name()
              << " offset=" << m_photoDetPanels[rich][panel]->localOffset()
              << endreq;
    }
  }

  // Initialise channel list
  m_readoutChannels.clear();

  return sc;
}
//=============================================================================

//=============================================================================
// Returns the position of a RichSmartID cluster in global coordinates
// on the HPD entrance window
//=============================================================================
StatusCode
Rich::SmartIDTool::globalPosition ( const Rich::HPDPixelCluster& cluster,
                                    Gaudi::XYZPoint& detectPoint ) const
{
  // Default return status is OK
  StatusCode sc = StatusCode::SUCCESS;
  // reset global coordinate to zero
  detectPoint = Gaudi::XYZPoint(0,0,0);
  // does cluster have any hits !!
  if ( !cluster.smartIDs().empty() )
  {
    // get position for each point in cluster
    for ( LHCb::RichSmartID::Vector::const_iterator iS = cluster.smartIDs().begin();
          iS != cluster.smartIDs().end(); ++iS )
    {
      Gaudi::XYZPoint tmpP;
      sc = sc && globalPosition(*iS,tmpP);
      detectPoint += (Gaudi::XYZVector)tmpP;
    }
    // normalise
    detectPoint /= (double)cluster.size();
  }
  return sc;
}

//=============================================================================
// Returns the position of a RichSmartID in global coordinates
// on the HPD entrance window
//=============================================================================
StatusCode
Rich::SmartIDTool::globalPosition ( const LHCb::RichSmartID smartID,
                                    Gaudi::XYZPoint& detectPoint ) const
{
  return m_photoDetPanels[smartID.rich()][smartID.panel()]->detectionPoint(smartID, detectPoint);
}

//=============================================================================
// Returns the position of a RichSmartID cluster in global coordinates
// on the anode pixel chip
//=============================================================================
StatusCode
Rich::SmartIDTool::anodeGlobalPosition ( const Rich::HPDPixelCluster& cluster,
                                         Gaudi::XYZPoint& detectPoint ) const
{
  // Default return status is OK
  StatusCode sc = StatusCode::SUCCESS;
  // reset global coordinate to zero
  detectPoint = Gaudi::XYZPoint(0,0,0);
  // does cluster have any hits !!
  if ( !cluster.smartIDs().empty() )
  {
    // get position for each point in cluster
    for ( LHCb::RichSmartID::Vector::const_iterator iS = cluster.smartIDs().begin();
          iS != cluster.smartIDs().end(); ++iS )
    {
      Gaudi::XYZPoint tmpP;
      sc = sc && anodeGlobalPosition(*iS,tmpP);
      detectPoint += (Gaudi::XYZVector)tmpP;
    }
    // normalise
    detectPoint /= (double)cluster.size();
  }
  return sc;
}

//=============================================================================
// Returns the position of a RichSmartID in global coordinates
// on the anode pixel chip
//=============================================================================
StatusCode
Rich::SmartIDTool::anodeGlobalPosition ( const LHCb::RichSmartID smartID,
                                         Gaudi::XYZPoint& detectPoint ) const
{
  // Should maybe update detPointOnAnode method to return its own StatusCode ?
  detectPoint = m_photoDetPanels[smartID.rich()][smartID.panel()]->detPointOnAnode(smartID);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Returns the global position of a local coordinate, in the given RICH panel
//=============================================================================
Gaudi::XYZPoint
Rich::SmartIDTool::globalPosition ( const Gaudi::XYZPoint& localPoint,
                                    const Rich::DetectorType rich,
                                    const Rich::Side side ) const
{
  return m_photoDetPanels[rich][side]->globalPosition(localPoint,side) ;
}

//=============================================================================
// Returns the HPD position (center of the silicon wafer)
//=============================================================================
StatusCode
Rich::SmartIDTool::hpdPosition ( const LHCb::RichSmartID hpdid,
                                 Gaudi::XYZPoint& hpdPoint ) const
{
  // Create temporary RichSmartIDs for two corners of the HPD wafer
  LHCb::RichSmartID id1(hpdid), id0(hpdid);
  id0.setPixelRow(10);
  id0.setPixelCol(10);
  id1.setPixelRow(21);
  id1.setPixelCol(21);

  // get position of each of these pixels
  Gaudi::XYZPoint a,b;
  const StatusCode sc = globalPosition(id0,a) && globalPosition(id1,b);

  // return average position (i.e. HPD centre)
  hpdPoint = Gaudi::XYZPoint( 0.5*(a.x()+b.x()) ,
                              0.5*(a.y()+b.y()),
                              0.5*(a.z()+b.z()) );

  return sc;
}

//=============================================================================
// Returns the SmartID for a given global position
//=============================================================================
StatusCode
Rich::SmartIDTool::smartID ( const Gaudi::XYZPoint& globalPoint,
                             LHCb::RichSmartID& smartid ) const
{

  // check to see if the smartID is set, and if HPD is active
  if ( smartid.hpdColIsSet() && smartid.hpdNumInColIsSet() )
    if ( !m_richS->hpdIsActive( smartid ) )
      return StatusCode::FAILURE;

  try
  {
    if (globalPoint.z() < 8000.0)
    {
      // Rich1
      if (globalPoint.y() > 0.0)
      {
        // top side
        smartid.setRich(Rich::Rich1);
        smartid.setPanel(Rich::top);
        return ( m_photoDetPanels[Rich::Rich1][Rich::top]->smartID(globalPoint, smartid) );
      }
      else
      {
        // bottom side
        smartid.setRich(Rich::Rich1);
        smartid.setPanel(Rich::bottom);
        return ( m_photoDetPanels[Rich::Rich1][Rich::bottom]->smartID(globalPoint, smartid) );
      }
    }
    else
    {
      // Rich2
      if (globalPoint.x() > 0.0)
      {
        // left side
        smartid.setRich(Rich::Rich2);
        smartid.setPanel(Rich::left);
        return ( m_photoDetPanels[Rich::Rich2][Rich::left]->smartID(globalPoint, smartid) );
      }
      else
      {
        // right side
        smartid.setRich(Rich::Rich2);
        smartid.setPanel(Rich::right);
        return ( m_photoDetPanels[Rich::Rich2][Rich::right]->smartID(globalPoint, smartid) );
      }
    }
  }

  // Catch any GaudiExceptions thrown
  catch ( const GaudiException & excpt )
  {
    // Print exception as an error
    Error ( "Caught GaudiException " + excpt.tag() + " message '" + excpt.message() + "'" );

    // reset smartid to an invalid one
    smartid = LHCb::RichSmartID(0);

    // return failure status
    return StatusCode::FAILURE;
  }

}

//=============================================================================
// Returns the SmartID for a given global position
// z coord is not valid
//=============================================================================
Gaudi::XYZPoint
Rich::SmartIDTool::globalToPDPanel ( const Gaudi::XYZPoint& globalPoint ) const
{
  if (globalPoint.z() < 8000.0)
  {
    // Rich1
    if (globalPoint.y() > 0.0)
    {
      // top side
      const Gaudi::XYZPoint tempPoint( m_photoDetPanels[Rich::Rich1][Rich::top]->geometry()->toLocal(globalPoint) );
      return Gaudi::XYZPoint( tempPoint.x(),
                              tempPoint.y() + m_photoDetPanels[Rich::Rich1][Rich::top]->localOffset(),
                              0.0 );
    }
    else
    {
      // bottom side
      const Gaudi::XYZPoint tempPoint( m_photoDetPanels[Rich::Rich1][Rich::bottom]->geometry()->toLocal( globalPoint ) );
      return Gaudi::XYZPoint( tempPoint.x(),
                              tempPoint.y() - m_photoDetPanels[Rich::Rich1][Rich::bottom]->localOffset(),
                              0.0 );
    }
  }
  else   // Rich2
  {
    if (globalPoint.x() > 0.0)
    {
      // left side
      const Gaudi::XYZPoint tempPoint( m_photoDetPanels[Rich::Rich2][Rich::left]->geometry()->toLocal(globalPoint) );
      return Gaudi::XYZPoint( tempPoint.x() + m_photoDetPanels[Rich::Rich2][Rich::left]->localOffset(),
                              tempPoint.y(),
                              0.0 );
    }
    else
    {
      // right side
      const Gaudi::XYZPoint tempPoint( m_photoDetPanels[Rich::Rich2][Rich::right]->geometry()->toLocal(globalPoint) );
      return Gaudi::XYZPoint( tempPoint.x() - m_photoDetPanels[Rich::Rich2][Rich::right]->localOffset(),
                              tempPoint.y(),
                              0.0 );
    }
  }

}

//=============================================================================
// Returns a list with all valid smartIDs
//=============================================================================
const LHCb::RichSmartID::Vector& Rich::SmartIDTool::readoutChannelList( ) const
{

  // Only do if list is empty
  if ( m_readoutChannels.empty() )
  {

    // Reserve size ( RICH1 + RICH2 );
    m_readoutChannels.reserve( 400000 );

    StatusCode sc = StatusCode::SUCCESS;

    // Fill for RICH1
    sc = m_photoDetPanels[Rich::Rich1][Rich::top]->readoutChannelList(m_readoutChannels);
    sc = sc && m_photoDetPanels[Rich::Rich1][Rich::bottom]->readoutChannelList(m_readoutChannels);
    const unsigned int nRich1 = m_readoutChannels.size();

    // Fill for RICH2
    sc = sc && m_photoDetPanels[Rich::Rich2][Rich::left]->readoutChannelList(m_readoutChannels);
    sc = sc && m_photoDetPanels[Rich::Rich2][Rich::right]->readoutChannelList(m_readoutChannels);
    const unsigned int nRich2 = m_readoutChannels.size() - nRich1;

    if ( sc.isFailure() ) Exception( "Problem reading readout channel lists from DeRichHPDPanels" );

    // Sort the list
    SmartIDSorter::sortByRegion(m_readoutChannels);

    info() << "Created active HPD channel list : # channels RICH(1/2) = "
           << nRich1 << " / " << nRich2 << endreq;

    if ( msgLevel(MSG::VERBOSE) )
    {
      for ( LHCb::RichSmartID::Vector::const_iterator iID = m_readoutChannels.begin();
            iID != m_readoutChannels.end(); ++iID )
      {
        Gaudi::XYZPoint gPos;
        sc = globalPosition(*iID,gPos);
        if ( sc.isFailure() ) Exception( "Problem converting RichSmartID to global coordinate" );
        verbose() << " RichSmartID " << *iID << " " << (*iID).dataBitsOnly().key() << endreq
                  << "     -> global Position : " << gPos << endreq
                  << "     -> local Position  : " << globalToPDPanel(gPos) << endreq;
      }
    }

  } // end fill smartID vector

  return m_readoutChannels;
}

