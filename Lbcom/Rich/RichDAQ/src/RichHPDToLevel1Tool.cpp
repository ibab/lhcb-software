
//-----------------------------------------------------------------------------
/** @file RichHPDToLevel1Tool.cpp
 *
 * Implementation file for class : RichHPDToLevel1Tool
 *
 * CVS Log :-
 * $Id: RichHPDToLevel1Tool.cpp,v 1.5 2005-01-22 13:33:42 jonrob Exp $
 * $Log: not supported by cvs2svn $
 * Revision 1.4  2005/01/18 09:06:55  jonrob
 * Add method to get RICH detector from Level1 ID
 *
 * Revision 1.3  2005/01/14 16:57:12  jonrob
 * Add functionality to use job options for mapping data
 *
 * Revision 1.2  2005/01/13 13:10:14  jonrob
 * Update mapping type
 *
 * Revision 1.1  2005/01/07 12:35:59  jonrob
 * Complete rewrite
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2004-12-18
 */
//-----------------------------------------------------------------------------

// local
#include "RichHPDToLevel1Tool.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichHPDToLevel1Tool>          s_factory ;
const        IToolFactory& RichHPDToLevel1ToolFactory = s_factory ;

// Standard constructor
RichHPDToLevel1Tool::RichHPDToLevel1Tool( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : RichToolBase ( type, name , parent )
{

  declareInterface<IRichHPDToLevel1Tool>(this);

  declareProperty( "L1Mapping", m_joData );

}

// Destructor
RichHPDToLevel1Tool::~RichHPDToLevel1Tool() {};


StatusCode RichHPDToLevel1Tool::initialize()
{
  // Initialise base class
  const StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // If map from job options is empty, build a temporary one
  // Used for old data base versions...
  if ( m_joData.empty() ) { buildTempMapping(); return sc; }

  // acquire tools
  IRichHPDIDTool * hpdIDTool;
  acquireTool( "RichHPDIDTool",   hpdIDTool  );

  // Use data from job options...
  JOData::const_iterator iD = m_joData.begin();
  bool newL1Bank = true;
  RichDAQ::Level1ID L1ID = 0;
  std::vector< unsigned int > nL1s( Rich::NRiches, 0 );
  while ( iD != m_joData.end() )
  {
    if ( newL1Bank ) {
      // Data is L1 ID for the coming block...
      L1ID = *iD;
      newL1Bank = false;
    } else {
      if ( *iD > 0 ) {
        const RichDAQ::HPDHardwareID hardID = *iD;
        // Get RichSmartID
        const RichSmartID smartID = hpdIDTool->richSmartID( hardID );
        // Fill maps
        m_smartid2L1 [ smartID ] = L1ID;
        m_hardid2L1  [ hardID  ] = L1ID;
        if ( m_l12smartids[L1ID].empty() ) ++nL1s[smartID.rich()]; // temporary count
        m_l12smartids[L1ID].push_back( smartID );
        m_l12hardids[L1ID].push_back( hardID );
        verbose() << "RichSmartID " << smartID << " HardID " << format("%3i",hardID)
                  << " -> L1ID " << format("%2i",L1ID) << endreq;
      } else {
        newL1Bank = true;
      }
    }
    ++iD;
  }
  m_joData.clear();

  // release tool
  releaseTool( hpdIDTool );

  info() << "Created L1 ID <-> HPD map : # L1 Boards RICH(1/2) = " << nL1s[Rich::Rich1]
         << " / " << nL1s[Rich::Rich2] << endreq;

  return sc;
}

StatusCode RichHPDToLevel1Tool::finalize()
{
  // base class finalize
  return RichToolBase::finalize();
}

const RichDAQ::Level1ID
RichHPDToLevel1Tool::levelL1ID( const RichSmartID smartID ) const
{
  // See if this RichSmartID is known
  SmartIDToL1::const_iterator id = m_smartid2L1.find( smartID.pdID() );
  if ( m_smartid2L1.end() == id )
  {
    std::ostringstream mess;
    mess << "Unknown HPD RichSmartID " << smartID;
    Exception ( mess.str() );
  }

  // Found, so return hardware ID
  return (*id).second;
}

const RichDAQ::Level1ID
RichHPDToLevel1Tool::levelL1ID( const RichDAQ::HPDHardwareID hardID ) const
{
  // See if this hardware ID is known
  HardIDToL1::const_iterator id = m_hardid2L1.find( hardID );
  if ( m_hardid2L1.end() == id )
  {
    Exception ( "Unknown HPD hardware ID" + boost::lexical_cast<std::string>(hardID) );
  }

  // Found, so return RichSmartID
  return (*id).second;
}

const RichSmartID::Collection &
RichHPDToLevel1Tool::l1HPDSmartIDs( const RichDAQ::Level1ID l1ID ) const
{
  // See if this L1 ID is known
  RichDAQ::L1ToSmartIDs::const_iterator id = m_l12smartids.find( l1ID );
  if ( m_l12smartids.end() == id )
  {
    Exception ( "Unknown RICH Level 1 ID " + boost::lexical_cast<std::string>(l1ID) );
  }

  // Found, so return list
  return (*id).second;
}

const RichDAQ::HPDHardwareIDs &
RichHPDToLevel1Tool::l1HPDHardIDs( const RichDAQ::Level1ID l1ID ) const
{
  // See if this L1 ID is known
  RichDAQ::L1ToHardIDs::const_iterator id = m_l12hardids.find( l1ID );
  if ( m_l12hardids.end() == id )
  {
    Exception ( "Unknown RICH Level 1 ID " + boost::lexical_cast<std::string>(l1ID) );
  }

  // Found, so return list
  return (*id).second;
}

// Access mapping between Level 1 IDs and HPD RichSmartIDs
const RichDAQ::L1ToSmartIDs & RichHPDToLevel1Tool::l1HPDSmartIDs() const
{
  return m_l12smartids;
}

// Access mapping between Level 1 IDs and HPD RichSmartIDs
const RichDAQ::L1ToHardIDs & RichHPDToLevel1Tool::l1HPDHardIDs() const
{
  return m_l12hardids;
}

const Rich::DetectorType
RichHPDToLevel1Tool::richDetector( const RichDAQ::Level1ID l1ID ) const
{
  // See if this L1 ID is known
  L1ToRICH::const_iterator rich = m_l1ToRich.find( l1ID );
  if ( m_l1ToRich.end() == rich )
  {
    Exception ( "Unknown RICH Level 1 ID " + boost::lexical_cast<std::string>(l1ID) );
  }

  // Found, so return RICH
  return (*rich).second;
}

// Used for "old" geometries which don't have job option defined layouts
// To be removed when no longer needed
void RichHPDToLevel1Tool::buildTempMapping()
{
  // acquire tools
  IRichHPDIDTool * hpdIDTool;
  acquireTool( "RichHPDIDTool",   hpdIDTool  );
  IRichSmartIDTool * smartIDs;
  acquireTool( "RichSmartIDTool", smartIDs );

  // Get list of all valid readout channels
  const RichSmartID::Collection & pixels = smartIDs->readoutChannelList();

  // number of HPDs per L1 board
  // Probably fixed, but should still be in XML...
  const unsigned int nHPDsPerL1 = 48;

  // Create temporary mapping between RichSmartID/hardwareID and Level1 number
  // Eventually, will need to come from some sort of data base
  std::vector< unsigned int > nL1s( Rich::NRiches, 0 );
  RichDAQ::Level1ID L1ID = 0;
  unsigned int iHPD = 0;
  Rich::DetectorType rich = Rich::InvalidDetector;
  for ( RichSmartID::Collection::const_iterator iID = pixels.begin(); iID != pixels.end(); ++iID )
  {
    const RichSmartID hpdID = (*iID).pdID();
    if ( m_smartid2L1.find(hpdID) == m_smartid2L1.end() )
    {
      // Get hardware ID for this RichSmartID
      const RichDAQ::HPDHardwareID hardID = hpdIDTool->hardwareID( hpdID );
      // Create L1 ID
      if ( iHPD >= nHPDsPerL1 ||
           ( rich != Rich::InvalidDetector && rich != hpdID.rich() ) ) { ++L1ID; iHPD = 0; }
      rich = hpdID.rich();

      // Fill maps
      m_smartid2L1[hpdID] = L1ID;
      m_hardid2L1[hardID] = L1ID;
      if ( m_l12smartids[L1ID].empty() ) ++nL1s[hpdID.rich()]; // temporary count
      m_l12smartids[L1ID].push_back( hpdID );
      m_l12hardids[L1ID].push_back( hardID );
      m_l1ToRich[L1ID] = rich;
      ++iHPD;

      verbose() << "RichSmartID " << hpdID << " HardID " << hardID
                << " -> L1ID " << L1ID << endreq;
    }
  }

  // release tools
  releaseTool( smartIDs  );
  releaseTool( hpdIDTool );

  info() << "Created L1 ID <-> HPD map : # L1 Boards RICH(1/2) = " << nL1s[Rich::Rich1]
         << " / " << nL1s[Rich::Rich2] << endreq;
}
