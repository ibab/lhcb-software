
//-----------------------------------------------------------------------------
/** @file RichDetNumberingTool.cpp
 *
 *  Implementation file for class : RichDetNumberingTool
 *
 *  CVS Log :-
 *  $Id: RichDetNumberingTool.cpp,v 1.1 2005-11-15 12:57:47 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 2004-12-18
 */
//-----------------------------------------------------------------------------

// local
#include "RichDetNumberingTool.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichDetNumberingTool>          s_factory ;
const        IToolFactory& RichDetNumberingToolFactory = s_factory ;

// Standard constructor
RichDetNumberingTool::RichDetNumberingTool( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : RichToolBase ( type, name , parent ),
    m_condBDLocs ( Rich::NRiches )
{
  // Interface
  declareInterface<IRichDetNumberingTool>(this);
  // initialise
  m_condBDLocs[Rich::Rich1] = "/dd/Conditions/ReadoutConf/Rich1/DetectorNumbers";
  m_condBDLocs[Rich::Rich2] = "/dd/Conditions/ReadoutConf/Rich2/DetectorNumbers";
}

// Destructor
RichDetNumberingTool::~RichDetNumberingTool() {};

StatusCode RichDetNumberingTool::initialize()
{
  // Initialise base class
  StatusCode sc = RichToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Update Manager
  IUpdateManagerSvc * ums = svc<IUpdateManagerSvc>("UpdateManagerSvc",true);

  // Register RICH1
  sc = ums->registerCondition( this,
                               m_condBDLocs[Rich::Rich1],
                               &RichDetNumberingTool::buildHPDMappings );
  if (sc.isFailure())
    return Error( "Failed registering dependency on '"+m_condBDLocs[Rich::Rich1]+"'", sc );

  // Register RICH2
  sc = ums->registerCondition( this,
                               m_condBDLocs[Rich::Rich2],
                               &RichDetNumberingTool::buildHPDMappings );
  if (sc.isFailure())
    return Error( "Failed registering dependency on '"+m_condBDLocs[Rich::Rich2]+"'", sc );

  // force first updates
  sc = ums->update(this);
  if (sc.isFailure()) return Error ( "Failed first UMS update", sc );

  return sc;
}

StatusCode RichDetNumberingTool::buildHPDMappings()
{
  info() << "Update triggered for HPD numbering maps" << endreq;

  // clear maps and containers
  m_soft2hard.clear();
  m_hard2soft.clear();
  m_smartIDs.clear();
  m_hardIDs.clear();
  m_l1IDs.clear();
  m_smartid2L1.clear();
  m_hardid2L1.clear();
  m_smartid2L0.clear();
  m_hardid2L0.clear();
  m_l1ToRich.clear();
  m_l12smartids.clear();
  m_l12hardids.clear();

  // NB : Currently updating both RICH1 and RICH2 if either changes ...
  //      Could considering doing this separately, probably not a big issue though

  // RICH1
  StatusCode sc = fillMaps( Rich::Rich1 );
  if ( sc.isFailure() ) return sc;

  // RICH2
  sc            = fillMaps( Rich::Rich2 );
  if ( sc.isFailure() ) return sc;

  return sc;
}

StatusCode RichDetNumberingTool::fillMaps( const Rich::DetectorType rich )
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Building mappings for " << rich << endreq;

  // load conditions
  Condition * numbers = getDet<Condition>(m_condBDLocs[rich]);

  // number of HPDs
  const unsigned int nHPDs         = numbers->param<int>( "NumberOfHPDs" );
  // vector of HPD RichSmartIDs
  const std::vector<int> & softIDs = numbers->paramAsIntVect("HPDSmartIDs");
  // vector of HPD hardware IDs
  const std::vector<int> & hardIDs = numbers->paramAsIntVect("HPDHardwareIDs");
  // vector of HPD Level0 IDs
  const std::vector<int> & l0IDs   = numbers->paramAsIntVect("HPDLevel0IDs");
  // vector of HPD Level1 IDs
  const std::vector<int> & l1IDs   = numbers->paramAsIntVect("HPDLevel1IDs");
  // check consistency
  if ( nHPDs != softIDs.size() ||
       nHPDs != hardIDs.size() ||
       nHPDs != l1IDs.size()   ||
       nHPDs != l0IDs.size()    )
  {
    std::ostringstream mess;
    mess << "Mismatch in " << rich << " HPD numbering schemes : # HPDs = " << nHPDs
         << " # SmartIDs = " << softIDs.size() << " # HardIDs = " << hardIDs.size()
         << " # L0IDs = " << l0IDs.size() << " # L1IDs = " << l1IDs.size();
    return Error( mess.str() );
  }

  // current number of L1 boards
  const int saveL1size = m_l1IDs.size();

  // build cached mappings
  std::vector<int>::const_iterator iSoft = softIDs.begin();
  std::vector<int>::const_iterator iHard = hardIDs.begin();
  std::vector<int>::const_iterator iL1   = l1IDs.begin();
  std::vector<int>::const_iterator iL0   = l0IDs.begin();
  for ( ; iSoft != softIDs.end() &&
          iHard != hardIDs.end() &&
          iL0   != l0IDs.end()   &&
          iL1   != l1IDs.end()   ;
        ++iSoft, ++iHard, ++iL0, ++iL1 )
  {
    // get data
    const RichSmartID            hpdID  ( *iSoft );
    const RichDAQ::HPDHardwareID hardID ( *iHard );
    const RichDAQ::Level1ID      L1ID   ( *iL1   );
    const RichDAQ::Level0ID      L0ID   ( *iL0   );
    // Sanity checks that this HPD is not already in the maps
    if ( m_soft2hard.find(hpdID) != m_soft2hard.end() )
    {
      std::ostringstream mess;
      mess << "Multiple entries for HPD RichSmartID " << hpdID;
      return Error( mess.str() );
    }
    if ( m_hard2soft.find(hardID) != m_hard2soft.end() )
    {
      return Error( "Multiple entries for HPD hardware ID " +
                    (std::string)hardID );
    }
    if ( m_l0hard2soft.find(L0ID) != m_l0hard2soft.end() )
    {
      return Error( "Multiple entries for HPD L0 ID " +
                    (std::string)L0ID );
    }
    // set up mappings etc.
    m_soft2hard[hpdID]  = hardID;
    m_hard2soft[hardID] = hpdID;
    m_l0hard2soft[L0ID] = hpdID;
    m_smartIDs.push_back(hpdID);
    m_hardIDs.push_back(hardID);
    m_smartid2L0[hpdID] = L0ID;
    m_hardid2L0[hardID] = L0ID;
    m_smartid2L1[hpdID] = L1ID;
    m_hardid2L1[hardID] = L1ID;
    m_l12smartids[L1ID].push_back( hpdID );
    m_l12hardids[L1ID].push_back( hardID );
    if ( std::find( m_l1IDs.begin(), m_l1IDs.end(), L1ID ) == m_l1IDs.end() )
    {
      m_l1ToRich[L1ID] = rich;
      m_l1IDs.push_back( L1ID );
    }
    // debug printout
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "HPD RichSmartID " << (int)hpdID << " " << hpdID << " HPDhardID " << hardID
                << " L0 " << L0ID << " L1 " << L1ID << endreq;
    }
  }

  info() << "Built mappings for " << m_l1IDs.size()-saveL1size
         << " L1 and " << nHPDs << " HPDs in " << rich << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichDetNumberingTool::finalize()
{
  // base class finalize
  return RichToolBase::finalize();
}

const RichDAQ::HPDHardwareID RichDetNumberingTool::hardwareID( const RichSmartID smartID ) const
{
  // See if this RichSmartID is known
  SoftToHard::const_iterator id = m_soft2hard.find( smartID.pdID() );
  if ( m_soft2hard.end() == id )
  {
    std::ostringstream mess;
    mess << "Unknown HPD RichSmartID " << smartID;
    Exception ( mess.str() );
  }

  // Found, so return hardware ID
  return (*id).second;
}

const RichSmartID RichDetNumberingTool::richSmartID( const RichDAQ::HPDHardwareID hID ) const
{
  // See if this HPD hardware ID is known
  HardToSoft::const_iterator id = m_hard2soft.find( hID );
  if ( m_hard2soft.end() == id )
  {
    Exception ( "Unknown HPD hardware ID " + (std::string)hID );
  }

  // Found, so return RichSmartID
  return (*id).second;
}

const RichSmartID RichDetNumberingTool::richSmartID( const RichDAQ::Level0ID l0ID ) const
{
  // See if this Level0 hardware ID is known
  L0HardToSoft::const_iterator id = m_l0hard2soft.find( l0ID );
  if ( m_l0hard2soft.end() == id )
  {
    Exception ( "Unknown Level0 hardware ID " + (std::string)l0ID );
  }

  // Found, so return RichSmartID
  return (*id).second;
}

const RichSmartID::Collection & RichDetNumberingTool::activeHPDRichSmartIDs() const
{
  return m_smartIDs;
}

const RichDAQ::HPDHardwareIDs & RichDetNumberingTool::activeHPDHardwareIDs() const
{
  return m_hardIDs;
}

bool RichDetNumberingTool::hpdIsActive( const RichSmartID id ) const
{
  // See if this RichSmartID is known
  SoftToHard::const_iterator iHPD = m_soft2hard.find( id.pdID() );
  return ( m_soft2hard.end() != iHPD );
}

bool RichDetNumberingTool::hpdIsActive( const RichDAQ::HPDHardwareID id ) const
{
  // See if this hardware ID is known
  HardToSoft::const_iterator iHPD = m_hard2soft.find( id );
  return ( m_hard2soft.end() != iHPD );
}

const RichDAQ::Level0ID
RichDetNumberingTool::level0ID( const RichSmartID smartID ) const
{
  // See if this RichSmartID is known
  SmartIDToL0::const_iterator id = m_smartid2L0.find( smartID.pdID() );
  if ( m_smartid2L0.end() == id )
  {
    std::ostringstream mess;
    mess << "Unknown HPD RichSmartID " << smartID;
    Exception ( mess.str() );
  }

  // Found, so return hardware ID
  return (*id).second;
}

const RichDAQ::Level1ID
RichDetNumberingTool::level1ID( const RichSmartID smartID ) const
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

const RichDAQ::Level0ID
RichDetNumberingTool::level0ID( const RichDAQ::HPDHardwareID hardID ) const
{
  // See if this hardware ID is known
  HardIDToL0::const_iterator id = m_hardid2L0.find( hardID );
  if ( m_hardid2L0.end() == id )
  {
    Exception ( "Unknown HPD hardware ID" + (std::string)hardID );
  }

  // Found, so return RichSmartID
  return (*id).second;
}

const RichDAQ::Level1ID
RichDetNumberingTool::level1ID( const RichDAQ::HPDHardwareID hardID ) const
{
  // See if this hardware ID is known
  HardIDToL1::const_iterator id = m_hardid2L1.find( hardID );
  if ( m_hardid2L1.end() == id )
  {
    Exception ( "Unknown HPD hardware ID" + (std::string)hardID );
  }

  // Found, so return RichSmartID
  return (*id).second;
}

const RichSmartID::Collection &
RichDetNumberingTool::l1HPDSmartIDs( const RichDAQ::Level1ID l1ID ) const
{
  // See if this L1 ID is known
  RichDAQ::L1ToSmartIDs::const_iterator id = m_l12smartids.find( l1ID );
  if ( m_l12smartids.end() == id )
  {
    Exception ( "Unknown RICH Level 1 ID " + (std::string)l1ID );
  }

  // Found, so return list
  return (*id).second;
}

const RichDAQ::HPDHardwareIDs &
RichDetNumberingTool::l1HPDHardIDs( const RichDAQ::Level1ID l1ID ) const
{
  // See if this L1 ID is known
  RichDAQ::L1ToHardIDs::const_iterator id = m_l12hardids.find( l1ID );
  if ( m_l12hardids.end() == id )
  {
    Exception ( "Unknown RICH Level 1 ID " + (std::string)l1ID );
  }

  // Found, so return list
  return (*id).second;
}

// Access mapping between Level 1 IDs and HPD RichSmartIDs
const RichDAQ::L1ToSmartIDs & RichDetNumberingTool::l1HPDSmartIDs() const
{
  return m_l12smartids;
}

// Access mapping between Level 1 IDs and HPD RichSmartIDs
const RichDAQ::L1ToHardIDs & RichDetNumberingTool::l1HPDHardIDs() const
{
  return m_l12hardids;
}

const Rich::DetectorType
RichDetNumberingTool::richDetector( const RichDAQ::Level1ID l1ID ) const
{
  // See if this L1 ID is known
  L1ToRICH::const_iterator rich = m_l1ToRich.find( l1ID );
  if ( m_l1ToRich.end() == rich )
  {
    Exception ( "Unknown RICH Level 1 ID " + (std::string)l1ID );
  }

  // Found, so return RICH
  return (*rich).second;
}

const RichDAQ::Level1IDs & RichDetNumberingTool::level1IDs() const
{
  return m_l1IDs;
}
