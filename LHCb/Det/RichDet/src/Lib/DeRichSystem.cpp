
//=============================================================================
/** @file DeRichSystem.cpp
 *
 * Implementation file for class : DeRichSystem
 *
 * $Id: DeRichSystem.cpp,v 1.10 2007-03-19 15:02:47 jonrob Exp $
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 * @date   2006-01-27
 */
//=============================================================================

// Include files
// Gaudi
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// DetDesc
#include "DetDesc/Condition.h"

// local
#include "RichDet/DeRichSystem.h"

// boost
#include "boost/format.hpp"

//=============================================================================

const CLID CLID_DERichSystem = 12005;  // User defined

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeRichSystem::DeRichSystem() { }

//=============================================================================
// Destructor
//=============================================================================
DeRichSystem::~DeRichSystem() { }


// Retrieve Pointer to class defininition structure
const CLID& DeRichSystem::classID()
{
  return CLID_DERichSystem;
}

//=========================================================================
//  initialize
//=========================================================================
StatusCode DeRichSystem::initialize ( )
{
  MsgStream msg( msgSvc(), "DeRichSystem" );
  msg << MSG::DEBUG << "Initialize " << name() << endmsg;

  m_condBDLocs[Rich::Rich1] = "Rich1DetectorNumbers";
  m_condBDLocs[Rich::Rich2] = "Rich2DetectorNumbers";
  SmartRef<Condition> rich1numbers = condition( m_condBDLocs[Rich::Rich1] );
  SmartRef<Condition> rich2numbers = condition( m_condBDLocs[Rich::Rich2] );

  // register condition for updates
  updMgrSvc()->registerCondition(this,rich1numbers.path(),&DeRichSystem::buildHPDMappings);
  updMgrSvc()->registerCondition(this,rich2numbers.path(),&DeRichSystem::buildHPDMappings);
  const StatusCode up = updMgrSvc()->update(this);
  if ( up.isFailure() )
    msg << MSG::ERROR << "Failed to update mappings" << endmsg;

  return up;
}

//=========================================================================
//  buildHPDMappings
//=========================================================================
StatusCode DeRichSystem::buildHPDMappings()
{
  MsgStream msg( msgSvc(), "DeRichSystem" );
  msg << MSG::INFO << "Update triggered for HPD numbering maps" << endreq;

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
  m_smartid2L1In.clear();
  m_hardid2L1In.clear();
  m_l0hard2soft.clear();
  m_inactiveSmartIDs.clear();
  m_inactiveHardIDs.clear();

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

//=========================================================================
//  fillMaps
//=========================================================================
StatusCode DeRichSystem::fillMaps( const Rich::DetectorType rich )
{
  MsgStream msg( msgSvc(), "DeRichSystem" );

  // load conditions
  SmartRef<Condition> numbers = condition(m_condBDLocs[rich]);

  // local typedef for vector from Conditions
  typedef std::vector<int> CondData;

  // number of HPDs
  const unsigned int nHPDs = numbers->param<int>( "NumberOfHPDs" );
  // vector of HPD RichSmartIDs
  const CondData & softIDs = numbers->paramVect<int>("HPDSmartIDs");
  // vector of HPD hardware IDs
  const CondData & hardIDs = numbers->paramVect<int>("HPDHardwareIDs");
  // vector of HPD Level0 IDs
  const CondData & l0IDs   = numbers->paramVect<int>("HPDLevel0IDs");
  // vector of HPD Level1 board IDs
  const CondData & l1IDs   = numbers->paramVect<int>("HPDLevel1IDs");
  // vector of HPD Level1 input numbers
  const CondData & l1Ins   = numbers->paramVect<int>("HPDLevel1InputNums");
  // inactive HPDs
  const CondData & inacts = numbers->paramVect<int>("InactiveHPDs");

  // check consistency
  if ( nHPDs != softIDs.size() ||
       nHPDs != hardIDs.size() ||
       nHPDs != l1IDs.size()   ||
       nHPDs != l0IDs.size()   ||
       nHPDs != l1Ins.size()   )
  {
    msg << MSG::ERROR << "Mismatch in " << rich << " HPD numbering schemes : # HPDs = "
        << nHPDs << " # SmartIDs = " << softIDs.size() << " # HardIDs = "
        << hardIDs.size() << " # L0IDs = " << l0IDs.size() << " # L1BoardIDs = "
        << l1IDs.size() << " # L1InputIDs = " << l1Ins.size() << endmsg;
    return StatusCode::FAILURE;
  }

  // current number of L1 boards
  const int saveL1size = m_l1IDs.size();

  // build cached mappings
  CondData::const_iterator iSoft = softIDs.begin();
  CondData::const_iterator iHard = hardIDs.begin();
  CondData::const_iterator iL1   = l1IDs.begin();
  CondData::const_iterator iL1In = l1Ins.begin();
  CondData::const_iterator iL0   = l0IDs.begin();
  for ( ; iSoft != softIDs.end() &&
          iHard != hardIDs.end() &&
          iL0   != l0IDs.end()   &&
          iL1   != l1IDs.end()   &&
          iL1In != l1Ins.end()   ;
        ++iSoft, ++iHard, ++iL0, ++iL1, ++iL1In )
  {

    // get data
    const LHCb::RichSmartID        hpdID  ( *iSoft );
    const Rich::DAQ::HPDHardwareID hardID ( *iHard );
    const Rich::DAQ::Level1ID      L1ID   ( *iL1   );
    const Rich::DAQ::Level0ID      L0ID   ( *iL0   );
    const Rich::DAQ::Level1Input   L1IN   ( *iL1In );
    // Sanity checks that this HPD is not already in the maps
    if ( m_soft2hard.find(hpdID) != m_soft2hard.end() )
    {
      msg << MSG::ERROR << "Multiple entries for HPD RichSmartID " << hpdID;
      return  StatusCode::FAILURE;
    }
    if ( m_hard2soft.find(hardID) != m_hard2soft.end() )
    {
      msg << MSG::ERROR << "Multiple entries for HPD hardware ID "
          << (std::string)hardID << hpdID << endmsg;
      return StatusCode::FAILURE;
    }
    if ( m_l0hard2soft.find(L0ID) != m_l0hard2soft.end() )
    {
      msg << MSG::ERROR << "Multiple entries for HPD L0 ID "
          << (std::string)L0ID << endmsg;
      return StatusCode::FAILURE;
    }

    // set up mappings etc.

    if ( std::find( inacts.begin(), inacts.end(), *iHard ) == inacts.end() )
    {
      m_smartIDs.push_back(hpdID);
      m_hardIDs.push_back(hardID);
    }
    m_soft2hard[hpdID]  = hardID;
    m_hard2soft[hardID] = hpdID;
    m_l0hard2soft[L0ID] = hpdID;
    m_smartid2L0[hpdID] = L0ID;
    m_hardid2L0[hardID] = L0ID;
    m_smartid2L1[hpdID] = L1ID;
    m_hardid2L1[hardID] = L1ID;
    m_smartid2L1In[hpdID] = L1IN;
    m_hardid2L1In[hardID] = L1IN;
    m_l12smartids[L1ID].push_back( hpdID );
    m_l12hardids[L1ID].push_back( hardID );
    if ( std::find( m_l1IDs.rbegin(), m_l1IDs.rend(), L1ID ) == m_l1IDs.rend() )
    {
      m_l1ToRich[L1ID] = rich;
      m_l1IDs.push_back( L1ID );
    }
    // debug printout
    msg << MSG::VERBOSE << "HPD RichSmartID " << (int)hpdID << " " << hpdID
        << " HPDhardID " << hardID << " L0 " << L0ID << " L1 board " << L1ID
        << " L1 input " << L1IN << endmsg;

  } // end loop over conditions data

  // build inative lists
  for( CondData::const_iterator iInAct = inacts.begin(); iInAct != inacts.end(); ++iInAct )
  {
    const Rich::DAQ::HPDHardwareID hardID ( *iInAct );
    const LHCb::RichSmartID      hpdID  ( richSmartID(hardID) );
    m_inactiveHardIDs.push_back  ( hardID );
    m_inactiveSmartIDs.push_back ( hpdID  );
    msg << MSG::INFO
        << "HPD " << hpdID << " hardID " << hardID << " is INACTIVE" << endreq;
  }

  msg << MSG::INFO << "Built mappings for " << boost::format("%2i") % (m_l1IDs.size()-saveL1size)
      << " L1 and " << nHPDs << " HPDs in " << rich << endreq;

  return StatusCode::SUCCESS;
}

//=========================================================================
//  hardwareID
//=========================================================================
const Rich::DAQ::HPDHardwareID
DeRichSystem::hardwareID( const LHCb::RichSmartID smartID ) const
{
  // See if this RichSmartID is known
  SoftToHard::const_iterator id = m_soft2hard.find( smartID.hpdID() );
  if ( m_soft2hard.end() == id )
  {
    std::ostringstream mess;
    mess << "Unknown HPD RichSmartID " << smartID.hpdID();
    throw GaudiException( mess.str(),
                          "DeRichSystem::hardwareID",
                          StatusCode::FAILURE );
  }

  // Found, so return hardware ID
  return (*id).second;
}

//=========================================================================
//  richSmartID
//=========================================================================
const LHCb::RichSmartID
DeRichSystem::richSmartID( const Rich::DAQ::HPDHardwareID hID ) const
{
  // See if this HPD hardware ID is known
  HardToSoft::const_iterator id = m_hard2soft.find( hID );
  if ( m_hard2soft.end() == id )
  {
    throw GaudiException( "Unknown HPD hardware ID " + (std::string)hID,
                          "DeRichSystem::richSmartID",
                          StatusCode::FAILURE );
  }

  // Found, so return RichSmartID
  return (*id).second;
}

//=========================================================================
//  richSmartID
//=========================================================================
const LHCb::RichSmartID
DeRichSystem::richSmartID( const Rich::DAQ::Level0ID l0ID ) const
{
  // See if this Level0 hardware ID is known
  L0HardToSoft::const_iterator id = m_l0hard2soft.find( l0ID );
  if ( m_l0hard2soft.end() == id )
  {
    throw GaudiException( "Unknown Level0 hardware ID " + (std::string)l0ID,
                          "DeRichSystem::richSmartID",
                          StatusCode::FAILURE );
  }

  // Found, so return RichSmartID
  return (*id).second;
}

//=========================================================================
//  activeHPDRichSmartIDs
//=========================================================================
const LHCb::RichSmartID::Vector& DeRichSystem::activeHPDRichSmartIDs() const
{
  return m_smartIDs;
}

//=========================================================================
//  activeHPDHardwareIDs
//=========================================================================
const Rich::DAQ::HPDHardwareIDs& DeRichSystem::activeHPDHardwareIDs() const
{
  return m_hardIDs;
}

//=========================================================================
//  hpdIsActive
//=========================================================================
bool DeRichSystem::hpdIsActive( const LHCb::RichSmartID id ) const
{
  // is this id in the inactive list
  const bool isActive =
    ( std::find( m_inactiveSmartIDs.begin(),
                 m_inactiveSmartIDs.end(),
                 id.hpdID() ) == m_inactiveSmartIDs.end() );

  // Double check id is valid
  // could make this check optional if it proves too costly in cpu
  SoftToHard::const_iterator iid = m_soft2hard.find( id.hpdID() );
  if ( m_soft2hard.end() == iid )
  {
    std::ostringstream mess;
    mess << "Unknown HPD RichSmartID " << id.hpdID();
    throw GaudiException( mess.str(),
                          "DeRichSystem::hpdIsActive",
                          StatusCode::FAILURE );
  }

  if (!isActive) std::cout << "DeRichSystem::hpdIsActive : inactive HPD" << std::endl;
  return isActive;
}

//=========================================================================
//  hpdIsActive
//=========================================================================
bool DeRichSystem::hpdIsActive( const Rich::DAQ::HPDHardwareID id ) const
{
  // is this id in the inactive list
  const bool isActive =
    ( std::find( m_inactiveHardIDs.begin(),
                 m_inactiveHardIDs.end(),
                 id ) == m_inactiveHardIDs.end() );

  // Double check id is valid
  // could make this check optional if it proves too costly in cpu
  HardToSoft::const_iterator iid = m_hard2soft.find( id );
  if ( m_hard2soft.end() == iid )
  {
    throw GaudiException( "Unknown HPD hardware ID " + (std::string)id,
                          "DeRichSystem::hpdIsActive",
                          StatusCode::FAILURE );
  }

  if (!isActive) std::cout << "DeRichSystem::hpdIsActive : inactive HPD" << std::endl;
  return isActive;
}

//=========================================================================
//  level0ID
//=========================================================================
const Rich::DAQ::Level0ID
DeRichSystem::level0ID( const LHCb::RichSmartID smartID ) const
{
  // See if this RichSmartID is known
  SmartIDToL0::const_iterator id = m_smartid2L0.find( smartID.hpdID() );
  if ( m_smartid2L0.end() == id )
  {
    std::ostringstream mess;
    mess << "Unknown HPD RichSmartID " << smartID;
    throw GaudiException( mess.str(),
                          "DeRichSystem::level0ID",
                          StatusCode::FAILURE );
  }

  // Found, so return Level 0 ID
  return (*id).second;
}

//=========================================================================
//  level1ID
//=========================================================================
const Rich::DAQ::Level1ID
DeRichSystem::level1ID( const LHCb::RichSmartID smartID ) const
{
  // See if this RichSmartID is known
  SmartIDToL1::const_iterator id = m_smartid2L1.find( smartID.hpdID() );
  if ( m_smartid2L1.end() == id )
  {
    std::ostringstream mess;
    mess << "Unknown HPD RichSmartID " << smartID;
    throw GaudiException( mess.str(),
                          "DeRichSystem::level1ID",
                          StatusCode::FAILURE );
  }

  // Found, so return Level1 board number
  return (*id).second;
}

//=========================================================================
//  level1InputNum
// Obtain the Level1 input number for a given RichSmartID
//=========================================================================
const Rich::DAQ::Level1Input
DeRichSystem::level1InputNum( const LHCb::RichSmartID smartID ) const
{
  // See if this RichSmartID is known
  SmartIDToL1In::const_iterator id = m_smartid2L1In.find( smartID.hpdID() );
  if ( m_smartid2L1In.end() == id )
  {
    std::ostringstream mess;
    mess << "Unknown HPD RichSmartID " << smartID;
    throw GaudiException( mess.str(),
                          "DeRichSystem::level1InputNum",
                          StatusCode::FAILURE );
  }

  // Found, so return Level 1 input number
  return (*id).second;
}

//=========================================================================
//  level0ID
//=========================================================================
const Rich::DAQ::Level0ID
DeRichSystem::level0ID( const Rich::DAQ::HPDHardwareID hardID ) const
{
  // See if this hardware ID is known
  HardIDToL0::const_iterator id = m_hardid2L0.find( hardID );
  if ( m_hardid2L0.end() == id )
  {
    throw GaudiException ( "Unknown HPD hardware ID" + (std::string)hardID,
                           "DeRichSystem::level0ID",
                           StatusCode::FAILURE );
  }

  // Found, so return Level 0 ID
  return (*id).second;
}

//=========================================================================
//  level1ID
//=========================================================================
const Rich::DAQ::Level1ID
DeRichSystem::level1ID( const Rich::DAQ::HPDHardwareID hardID ) const
{
  // See if this hardware ID is known
  HardIDToL1::const_iterator id = m_hardid2L1.find( hardID );
  if ( m_hardid2L1.end() == id )
  {
    throw GaudiException( "Unknown HPD hardware ID" + (std::string)hardID,
                          "DeRichSystem::level1ID",
                          StatusCode::FAILURE );
  }

  // Found, so return Level 1 board number
  return (*id).second;
}

//=========================================================================
//  level1InputNum
/// Obtain the Level1 input number for a given HPD hardware ID
//=========================================================================
const Rich::DAQ::Level1Input
DeRichSystem::level1InputNum( const Rich::DAQ::HPDHardwareID hardID ) const
{
  // See if this hardware ID is known
  HardIDToL1In::const_iterator id = m_hardid2L1In.find( hardID );
  if ( m_hardid2L1In.end() == id )
  {
    throw GaudiException( "Unknown HPD hardware ID" + (std::string)hardID,
                          "DeRichSystem::level1InputNum",
                          StatusCode::FAILURE );
  }

  // Found, so return Level 1 board input xnumber
  return (*id).second;
}

//=========================================================================
//  l1HPDSmartIDs
//=========================================================================
const LHCb::RichSmartID::Vector &
DeRichSystem::l1HPDSmartIDs( const Rich::DAQ::Level1ID l1ID ) const
{
  // See if this L1 ID is known
  Rich::DAQ::L1ToSmartIDs::const_iterator id = m_l12smartids.find( l1ID );
  if ( m_l12smartids.end() == id )
  {
    throw GaudiException( "Unknown RICH Level1 board ID " + (std::string)l1ID,
                          "DeRichSystem::l1HPDSmartIDs", StatusCode::FAILURE );
  }

  // Found, so return list
  return (*id).second;
}

//=========================================================================
//  l1HPDHardIDs
//=========================================================================
const Rich::DAQ::HPDHardwareIDs &
DeRichSystem::l1HPDHardIDs( const Rich::DAQ::Level1ID l1ID ) const
{
  // See if this L1 ID is known
  Rich::DAQ::L1ToHardIDs::const_iterator id = m_l12hardids.find( l1ID );
  if ( m_l12hardids.end() == id )
  {
    throw GaudiException( "Unknown RICH Level1 board ID " + (std::string)l1ID,
                          "DeRichSystem::l1HPDHardIDs", StatusCode::FAILURE );
  }

  // Found, so return list
  return (*id).second;
}

//=========================================================================
// l1HPDSmartIDs
// Access mapping between Level 1 IDs and HPD RichSmartIDs
//=========================================================================
const Rich::DAQ::L1ToSmartIDs& DeRichSystem::l1HPDSmartIDs() const
{
  return m_l12smartids;
}

//=========================================================================
//  l1HPDHardIDs
// Access mapping between Level 1 IDs and HPD RichSmartIDs
//=========================================================================
const Rich::DAQ::L1ToHardIDs& DeRichSystem::l1HPDHardIDs() const
{
  return m_l12hardids;
}

//=========================================================================
// richDetector
//=========================================================================
const Rich::DetectorType
DeRichSystem::richDetector( const Rich::DAQ::Level1ID l1ID ) const
{
  // See if this L1 ID is known
  L1ToRICH::const_iterator rich = m_l1ToRich.find( l1ID );
  if ( m_l1ToRich.end() == rich )
  {
    throw GaudiException( "Unknown RICH Level1 board ID " + (std::string)l1ID,
                          "DeRichSystem::richDetector()", StatusCode::FAILURE );
  }

  // Found, so return RICH
  return (*rich).second;
}

//=========================================================================
//  level1IDs
//=========================================================================
const Rich::DAQ::Level1IDs& DeRichSystem::level1IDs() const
{
  return m_l1IDs;
}

//===========================================================================
