
//-----------------------------------------------------------------------------
/** @file RichSmartIDTool.cpp
 *
 * Implementation file for class : RichSmartIDTool
 *
 * CVS Log :-
 * $Id: RichSmartIDTool.cpp,v 1.21 2006-05-05 09:32:42 jonrob Exp $
 *
 * @author Antonis Papanestis
 * @date 2003-10-28
 */
//-----------------------------------------------------------------------------

// local
#include "RichSmartIDTool.h"

// Declaration of the Tool Factory
static const  ToolFactory<RichSmartIDTool>          s_factory ;
const        IToolFactory& RichSmartIDToolFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichSmartIDTool::RichSmartIDTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : RichToolBase( type, name, parent )
{
  declareInterface<IRichSmartIDTool>(this);
}

//=============================================================================
///Destructor
RichSmartIDTool::~RichSmartIDTool() {}
//=============================================================================

//=============================================================================
StatusCode RichSmartIDTool::initialize()
{

  // Initialise base class
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  m_richS = getDet<DeRichSystem>(DeRichLocation::RichSystem);

  // HPD panel names
  const std::string pdPanelName[2][2]   ={{ DeRichHPDPanelLocation::Rich1Panel0,
                                            DeRichHPDPanelLocation::Rich1Panel1 },
                                          { DeRichHPDPanelLocation::Rich2Panel0,
                                            DeRichHPDPanelLocation::Rich2Panel1 } };
  const std::string pdPanelNameOld[2][2]={{ DeRichHPDPanelLocation::Rich1Panel0_old,
                                            DeRichHPDPanelLocation::Rich1Panel1_old },
                                          { DeRichHPDPanelLocation::Rich2Panel0_old,
                                            DeRichHPDPanelLocation::Rich2Panel1_old }};

  //loop over riches and photo detector panels
  for ( unsigned int rich = 0; rich < m_photoDetPanels.size(); ++rich )
  {
    for ( unsigned int panel = 0; panel < m_photoDetPanels[rich].size(); ++panel )
    {
      try {
        m_photoDetPanels[rich][panel] = getDet<DeRichHPDPanel>( pdPanelName[rich][panel] );
      }
      catch ( GaudiException& e ) {
        m_photoDetPanels[rich][panel] = getDet<DeRichHPDPanel>(pdPanelNameOld[rich][panel]);
      }
      debug() << "Stored photodetector panel "
              << m_photoDetPanels[rich][panel]->name() << endreq;
      m_localOffset[rich][panel] = m_photoDetPanels[rich][panel]->localOffset();
    }
  }

  // Initialise channel list
  m_readoutChannels.clear();

  return sc;
}

//=============================================================================
StatusCode RichSmartIDTool::finalize()
{
  return RichToolBase::finalize();
}

//=============================================================================
// Returns the position of a RichSmartID in global coordinates
//=============================================================================
Gaudi::XYZPoint RichSmartIDTool::globalPosition ( const LHCb::RichSmartID smartID ) const
{
  return m_photoDetPanels[smartID.rich()][smartID.panel()]->detectionPoint(smartID);
}

//=============================================================================
// Returns the global position of a local coordinate, in the given RICH panel
//=============================================================================
Gaudi::XYZPoint RichSmartIDTool::globalPosition ( const Gaudi::XYZPoint& localPoint,
                                                  const Rich::DetectorType rich,
                                                  const Rich::Side side ) const
{
  return m_photoDetPanels[rich][side]->globalPosition(localPoint,side) ;
}

//=============================================================================
// Returns the HPD position (center of the silicon wafer)
//=============================================================================
Gaudi::XYZPoint RichSmartIDTool::hpdPosition ( const LHCb::RichSmartID hpdid ) const
{
  // Create temporary RichSmartIDs for two corners of the HPD wafer
  LHCb::RichSmartID id1(hpdid), id0(hpdid);
  id0.setPixelRow(0);
  id0.setPixelCol(0);
  id1.setPixelRow(31);
  id1.setPixelCol(31);

  Gaudi::XYZPoint a( globalPosition(id0) );
  Gaudi::XYZPoint b( globalPosition(id1) );

  // return average position
  return ( Gaudi::XYZPoint( 0.5*(a.x()+b.x()) ,0.5*(a.y()+b.y()),
                            0.5*(a.z()+b.z()) ) );
}

//=============================================================================
// Returns the SmartID for a given global position
//=============================================================================
StatusCode RichSmartIDTool::smartID ( const Gaudi::XYZPoint& globalPoint,
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
Gaudi::XYZPoint RichSmartIDTool::globalToPDPanel ( const Gaudi::XYZPoint& globalPoint ) const
{
  if (globalPoint.z() < 8000.0)
  {
    // Rich1
    if (globalPoint.y() > 0.0) {
      // top side
      Gaudi::XYZPoint tempPoint( m_photoDetPanels[Rich::Rich1][Rich::top]->
                                 geometry()->toLocal( globalPoint ) );

      tempPoint.SetCoordinates( tempPoint.x(),
                                tempPoint.y() + m_localOffset[Rich::Rich1][Rich::top],
                                0.0 );
      return tempPoint;
    }
    else {
      // bottom side
      Gaudi::XYZPoint tempPoint( m_photoDetPanels[Rich::Rich1][Rich::bottom]->
                                 geometry()->toLocal( globalPoint ) );

      tempPoint.SetCoordinates( tempPoint.x(),
                                tempPoint.y() - m_localOffset[Rich::Rich1][Rich::bottom],
                                0.0 );

      return tempPoint;
    }
  }
  else   // Rich2
  {
    if (globalPoint.x() > 0.0) {
      // left side
      Gaudi::XYZPoint tempPoint( m_photoDetPanels[Rich::Rich2][Rich::left]->
                                 geometry()->toLocal( globalPoint ) );

      tempPoint.SetCoordinates( tempPoint.x() + m_localOffset[Rich::Rich2][Rich::left],
                                tempPoint.y(),
                                0.0 );
      return tempPoint;

    }
    else {
      // right side
      Gaudi::XYZPoint tempPoint( m_photoDetPanels[Rich::Rich2][Rich::right]->
                                 geometry()->toLocal( globalPoint ) );

      tempPoint.SetCoordinates( tempPoint.x() - m_localOffset[Rich::Rich2][Rich::right],
                                tempPoint.y(),
                                0.0 );
      return tempPoint;
    }
  }

}

//=============================================================================
// Returns a list with all valid smartIDs
//=============================================================================
const LHCb::RichSmartID::Vector& RichSmartIDTool::readoutChannelList( ) const
{

  // Only do if list is empty
  if ( m_readoutChannels.empty() ) {

    // Reserve size ( RICH1 + RICH2 );
    m_readoutChannels.reserve( 400000 );

    // Fill for RICH1
    m_photoDetPanels[Rich::Rich1][Rich::top]->readoutChannelList(m_readoutChannels);
    m_photoDetPanels[Rich::Rich1][Rich::bottom]->readoutChannelList(m_readoutChannels);
    const unsigned int nRich1 = m_readoutChannels.size();

    // Fill for RICH2
    m_photoDetPanels[Rich::Rich2][Rich::left]->readoutChannelList(m_readoutChannels);
    m_photoDetPanels[Rich::Rich2][Rich::right]->readoutChannelList(m_readoutChannels);
    const unsigned int nRich2 = m_readoutChannels.size() - nRich1;

    // Sort the list
    const RichSmartIDSorter sorter;
    sorter.sortByRegion(m_readoutChannels);

    info() << "Created active HPD channel list : # channels RICH(1/2) = "
           << nRich1 << " / " << nRich2 << endreq;

    if ( msgLevel(MSG::VERBOSE) )
    {
      for ( LHCb::RichSmartID::Vector::const_iterator iID = m_readoutChannels.begin();
            iID != m_readoutChannels.end(); ++iID )
      {
        const Gaudi::XYZPoint gPos = globalPosition(*iID);
        verbose() << " RichSmartID " << *iID << " " << (*iID).dataBitsOnly().key() << endreq
                  << "     -> global Position : " << gPos << endreq
                  << "     -> local Position  : " << globalToPDPanel(gPos) << endreq;
      }
    }

  } // end fill smartID vector

  return m_readoutChannels;
}

