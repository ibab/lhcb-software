
//=============================================================================
/** @file DeRichSystem.cpp
 *
 * Implementation file for class : DeRichSystem
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 * @date   2006-01-27
 */
//=============================================================================

// Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// RichKernel
#include "RichKernel/RichHPDIdentifier.h"

// DetDesc
#include "DetDesc/Condition.h"

// local
#include "RichDet/DeRichSystem.h"

// boost
#include "boost/format.hpp"
#include "boost/lexical_cast.hpp"

//=============================================================================

const CLID CLID_DERichSystem = 12005;  // User defined

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeRichSystem::DeRichSystem( const std::string & name )
  : DeRichBase     ( name          ),
    m_deRich       ( Rich::NRiches ),
    m_condDBLocs   ( Rich::NRiches ),
    m_firstL1CopyN ( 0             )
{
  m_deRich[Rich::Rich1] = NULL;
  m_deRich[Rich::Rich2] = NULL;
  m_condDBLocs[Rich::Rich1] = "Rich1DetectorNumbers";
  m_condDBLocs[Rich::Rich2] = "Rich2DetectorNumbers";
}

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
StatusCode DeRichSystem::initialize()
{
  setMyName("DeRichSystem");

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Initialize " << name() << endmsg;

  // register for condition updates
  updMgrSvc()->registerCondition( this, 
                                  condition(m_condDBLocs[Rich::Rich1]).path(),
                                  &DeRichSystem::buildHPDMappings );
  updMgrSvc()->registerCondition( this, 
                                  condition(m_condDBLocs[Rich::Rich2]).path(),
                                  &DeRichSystem::buildHPDMappings );

  // Run first update
  const StatusCode sc = updMgrSvc()->update(this);
  if ( sc.isFailure() ) error() << "Failed to update mappings" << endmsg;

  return sc;
}

//=========================================================================
// Check and fill a map
//=========================================================================
template < class SOURCE, class TARGET, class MAP >
bool DeRichSystem::safeMapFill( const SOURCE& source, const TARGET& target, MAP& map )
{
  std::pair<typename MAP::iterator,bool> p = 
    map.insert(typename MAP::value_type(source,target));
  if ( !p.second )
  {
    error() << "Error filling map '" << System::typeinfoName(typeid(map))
            << "' source " << System::typeinfoName(typeid(source)) << "=" << source
            << " already has an entry for target " << System::typeinfoName(typeid(target))
            << " OLD=" << map[source] << " NEW=" << target
            << endmsg;
  }
  return p.second;
}

//=========================================================================
// Access on demand the Detector Elements for Rich1 and Rich2
//=========================================================================
DetectorElement * DeRichSystem::deRich( const Rich::DetectorType rich ) const
{
  if ( !m_deRich[rich] )
  {
    SmartDataPtr<DetectorElement> deR( dataSvc(), DeRichLocations::location(rich) );
    m_deRich[rich] = deR;
  }
  return m_deRich[rich];
}

//=========================================================================
//  buildHPDMappings
//=========================================================================
StatusCode DeRichSystem::buildHPDMappings()
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Update triggered for HPD numbering maps" << endmsg;

  // clear maps and containers
  m_soft2hard.clear();
  m_hard2soft.clear();
  m_activeHPDSmartIDs.clear();
  m_activeHPDHardIDs.clear();
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
  m_l0ToL1.clear();
  m_smartid2copyNumber.clear();
  m_copyNumber2smartid.clear();
  m_inactiveHPDSmartIDs.clear();
  m_inactiveHPDHardIDs.clear();
  m_allHPDSmartIDs.clear();
  m_allHPDHardIDs.clear();
  m_L1HardIDAndInputToHPDHardID.clear();
  m_firstL1CopyN = 0;

  // Fill the maps for each RICH
  const StatusCode sc = ( fillMaps ( Rich::Rich1 ) && 
                          fillMaps ( Rich::Rich2 ) );

  // return
  return sc;
}

//=========================================================================
//  fillMaps
//=========================================================================
StatusCode DeRichSystem::fillMaps( const Rich::DetectorType rich )
{

  // load conditions
  SmartRef<Condition> numbers = condition(m_condDBLocs[rich]);

  // local typedefs for vector from Conditions
  typedef std::vector<int> CondData;
  typedef std::vector<std::string> L1Mapping;

  // number of HPDs
  const unsigned int nHPDs = numbers->param<int>("NumberOfHPDs");
  // vector of HPD RichSmartIDs
  const CondData & softIDs = numbers->paramVect<int>("HPDSmartIDs");
  // vector of HPD hardware IDs
  const CondData & hardIDs = numbers->paramVect<int>("HPDHardwareIDs");
  // vector of HPD Level0 IDs
  const CondData & l0IDs   = numbers->paramVect<int>("HPDLevel0IDs");
  // vector of HPD Level1 board Hardware IDs
  const CondData & l1IDs   = numbers->paramVect<int>("HPDLevel1HardwareIDs");
  // vector of HPD Level1 input numbers
  const CondData & l1Ins   = numbers->paramVect<int>("HPDLevel1InputNums");
  // vector of HPD Copy numbers
  const CondData & copyNs  = numbers->paramVect<int>("HPDCopyNumbers");
  // inactive HPDs
  CondData inacts;
  bool inactiveHPDListInSmartIDs( false );
  if ( numbers->exists("InactiveHPDListInSmartIDs") )
  {
    // smartIDs
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Inactive HPDs are taken from the smartID list" << endmsg;
    const CondData& inactsHuman = numbers->paramVect<int>("InactiveHPDListInSmartIDs");
    inactiveHPDListInSmartIDs = true;
    inacts.reserve(inactsHuman.size());
    for ( CondData::const_iterator inHpd = inactsHuman.begin(); 
          inHpd != inactsHuman.end(); ++inHpd )
    {
      const LHCb::RichSmartID ID( Rich::DAQ::HPDIdentifier(*inHpd).smartID() );
      if ( ID.isValid() ) { inacts.push_back( ID ); }
      else
      {
        error() << "Invalid smartID in the list of inactive HPDs " << *inHpd << endmsg;
      }
    }
  }
  else
  {
    // hardware IDs
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Inactive HPDs are taken from the hardware list" << endmsg;
    inacts = numbers->paramVect<int>("InactiveHPDs");
  }

  // check consistency
  if ( nHPDs != softIDs.size() ||
       nHPDs != hardIDs.size() ||
       nHPDs != l1IDs.size()   ||
       nHPDs != l0IDs.size()   ||
       nHPDs != l1Ins.size()   ||
       nHPDs != copyNs.size()   )
  {
    error() << "Mismatch in " << rich << " HPD numbering schemes : # HPDs = "
            << nHPDs << " # SmartIDs = " << softIDs.size() << " # HardIDs = "
            << hardIDs.size() << " # L0IDs = " << l0IDs.size() << " # L1BoardIDs = "
            << l1IDs.size() << " # L1InputIDs = " << l1Ins.size() << " # CopyNumbers = "
            << copyNs.size() << endmsg;
    return StatusCode::FAILURE;
  }

  // current number of L1 boards
  const unsigned int saveL1size = m_l1IDs.size();

  // build cached mappings
  CondData::const_iterator iSoft  = softIDs.begin();
  CondData::const_iterator iHard  = hardIDs.begin();
  CondData::const_iterator iL1    = l1IDs.begin();
  CondData::const_iterator iL1In  = l1Ins.begin();
  CondData::const_iterator iL0    = l0IDs.begin();
  CondData::const_iterator icopyN = copyNs.begin();
  for ( ; iSoft  != softIDs.end() &&
          iHard  != hardIDs.end() &&
          iL0    != l0IDs.end()   &&
          iL1    != l1IDs.end()   &&
          iL1In  != l1Ins.end()   &&
          icopyN != copyNs.end()  ;
        ++iSoft, ++iHard, ++iL0, ++iL1, ++iL1In, ++icopyN )
  {

    // get data
    const LHCb::RichSmartID           hpdID  ( *iSoft  ); 
    const Rich::DAQ::HPDHardwareID    hardID ( *iHard  );
    const Rich::DAQ::Level1HardwareID L1ID   ( *iL1    );
    const Rich::DAQ::Level0ID         L0ID   ( *iL0    );
    const Rich::DAQ::Level1Input      L1IN   ( *iL1In  );
    const Rich::DAQ::HPDCopyNumber    copyN  ( *icopyN );

    // debug printout
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "PD     " << (int)hpdID << " " << hpdID
                << " HPDhardID " << hardID << " L0 " << L0ID << " L1 HardID " << L1ID
                << " L1 input " << L1IN << endmsg;

    // Sanity checks that this HPD is not already in the maps
    if ( m_soft2hard.find(hpdID) != m_soft2hard.end() )
    {
      error() << "Multiple entries for HPD RichSmartID " << hpdID;
      return  StatusCode::FAILURE;
    }
    if ( m_hard2soft.find(hardID) != m_hard2soft.end() )
    {
      error() << "Multiple entries for HPD hardware ID "
              << (std::string)hardID << hpdID << endmsg;
      return StatusCode::FAILURE;
    }
    if ( m_l0hard2soft.find(L0ID) != m_l0hard2soft.end() )
    {
      error() << "Multiple entries for HPD L0 ID "
              << (std::string)L0ID << endmsg;
      return StatusCode::FAILURE;
    }
    if ( m_copyNumber2smartid.find(copyN) != m_copyNumber2smartid.end() )
    {
      error() << "Multiple entries for HPD copy number "
              << (std::string)copyN << hpdID << endmsg;
      return StatusCode::FAILURE;
    }

    // set up mappings etc.

    CondData::const_iterator & myID = ( inactiveHPDListInSmartIDs ? iSoft : iHard );
    if ( std::find( inacts.begin(), inacts.end(), *myID ) == inacts.end() )
    {
      m_activeHPDSmartIDs.push_back ( hpdID  );
      m_activeHPDHardIDs.push_back  ( hardID );
    }
    else
    {
      if ( !hpdIsActive(hardID) )
      {
        error() << "PD " << hpdID << " hardID " << hardID
                << " listed twice in INACTIVE HPD list !" << endmsg;
      }
      else
      {
        m_inactiveHPDHardIDs.push_back  ( hardID );
        m_inactiveHPDSmartIDs.push_back ( hpdID  );
        if ( msgLevel(MSG::DEBUG) )
          debug() << "PD " << hpdID << " hardID " << hardID << " is INACTIVE" << endmsg;
      }
    }

    // Fill maps
    bool OK = true;
    m_allHPDHardIDs.push_back(hardID);
    m_allHPDSmartIDs.push_back(hpdID);
    OK &= safeMapFill(hpdID,hardID,m_soft2hard);
    OK &= safeMapFill(hardID,hpdID,m_hard2soft);
    OK &= safeMapFill(L0ID,hpdID,m_l0hard2soft);
    OK &= safeMapFill(hpdID,L0ID,m_smartid2L0);
    OK &= safeMapFill(hardID,L0ID,m_hardid2L0);
    OK &= safeMapFill(hpdID,L1ID,m_smartid2L1);
    OK &= safeMapFill(hardID,L1ID,m_hardid2L1);
    OK &= safeMapFill(hpdID,L1IN,m_smartid2L1In);
    OK &= safeMapFill(hardID,L1IN,m_hardid2L1In);
    OK &= safeMapFill(L0ID,L1ID,m_l0ToL1);
    OK &= safeMapFill(hpdID,copyN,m_smartid2copyNumber);
    OK &= safeMapFill(copyN,hpdID,m_copyNumber2smartid);
    m_l12smartids[L1ID].push_back( hpdID );
    m_l12hardids[L1ID].push_back( hardID );
    const L1HardIDAndInput idAndInput(L1ID,L1IN);
    OK &= safeMapFill(idAndInput,hardID,m_L1HardIDAndInputToHPDHardID);
    if ( std::find( m_l1IDs.rbegin(), m_l1IDs.rend(), L1ID ) == m_l1IDs.rend() )
    {
      m_l1ToRich[L1ID] = rich;
      m_l1IDs.push_back( L1ID );
    }
    if ( !OK ) return StatusCode::FAILURE;

  } // end loop over conditions data

  // Sort HPD lists
  std::stable_sort( m_activeHPDHardIDs.begin(),    m_activeHPDHardIDs.end()    );
  std::stable_sort( m_activeHPDSmartIDs.begin(),   m_activeHPDSmartIDs.end()   );
  std::stable_sort( m_inactiveHPDHardIDs.begin(),  m_inactiveHPDHardIDs.end()  );
  std::stable_sort( m_inactiveHPDSmartIDs.begin(), m_inactiveHPDSmartIDs.end() );
  std::stable_sort( m_allHPDHardIDs.begin(),       m_allHPDHardIDs.end()       );
  std::stable_sort( m_allHPDSmartIDs.begin(),      m_allHPDSmartIDs.end()      );

  // L1 mapping
  const std::string L1LogToHardMapName("Level1LogicalToHardwareIDMap");
  if ( numbers->exists(L1LogToHardMapName) )
  {
    const L1Mapping & l1Mapping = numbers->paramVect<std::string>(L1LogToHardMapName);
    for ( L1Mapping::const_iterator iM = l1Mapping.begin();
          iM != l1Mapping.end(); ++iM )
    {
      std::string data = *iM;
      // Strip extra " characters if present at start and end of string
      // To work around a small typo in the DB - Can be removed at some later date
      if ( data[0]             == '"' ) data = data.substr(1,data.size());
      if ( data[data.size()-1] == '"' ) data = data.substr(0,data.size()-1);
      // Format of string is 'LogicalID/HardwareID'
      const std::string::size_type slash = data.find_first_of( "/" );
      if ( slash == 0 )
      { 
        error() << "Badly formed " << L1LogToHardMapName << " for " << rich << endmsg;
        return StatusCode::FAILURE;
      }
      const Rich::DAQ::Level1LogicalID  logID  ( boost::lexical_cast<int>(data.substr(0,slash)) );
      const Rich::DAQ::Level1HardwareID hardID ( boost::lexical_cast<int>(data.substr(slash+1)) );
      const Rich::DetectorType richTmp = this->richDetector(hardID);
      if ( msgLevel(MSG::DEBUG) )
        debug() << richTmp << " L1 ID mapping : Logical=" << logID
                << " Hardware=" << hardID << endmsg;
      (m_l1LogToHard[rich])[logID]  = hardID;
      m_l1HardToLog[hardID]         = logID;
    }
  }
  else 
  {
    error() << "Could not load Condition " << L1LogToHardMapName << endmsg;
    return StatusCode::FAILURE;
  }

  // L1 Logical ID to Copy Numbers
  // Create on the fly here. Should add to CondDB really ...
  for ( Rich::DAQ::Level1HardwareIDs::const_iterator iL1HID = m_l1IDs.begin();
        iL1HID != m_l1IDs.end(); ++iL1HID )
  {
    if ( m_l1H2CopyN.find(*iL1HID) == m_l1H2CopyN.end() )
    {
      m_l1H2CopyN[*iL1HID] = Rich::DAQ::Level1CopyNumber(m_firstL1CopyN);
      ++m_firstL1CopyN;
      if ( msgLevel(MSG::DEBUG) )
        debug() << "L1 Copy Number " << m_l1H2CopyN[*iL1HID] 
                << " -> HardwareID=" << *iL1HID << " LogicalID=" << level1LogicalID(*iL1HID) 
                << endmsg;
    }
  }

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Built mappings for " << boost::format("%2i") % (m_l1IDs.size()-saveL1size)
            << " L1 and " << nHPDs << " PDs in " << rich << endmsg;

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
    mess << "Unknown HPD RichSmartID " 
         << (int)smartID.hpdID() << " " << smartID.hpdID();
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
    mess << "Unknown HPD RichSmartID " 
         << (int)smartID.hpdID() << " " << smartID.hpdID();
    throw GaudiException( mess.str(),
                          "DeRichSystem::level0ID",
                          StatusCode::FAILURE );
  }

  // Found, so return Level 0 ID
  return (*id).second;
}

//=========================================================================
//  level1HardwareID
//=========================================================================
const Rich::DAQ::Level1HardwareID
DeRichSystem::level1HardwareID( const LHCb::RichSmartID smartID ) const
{
  // See if this RichSmartID is known
  SmartIDToL1::const_iterator id = m_smartid2L1.find( smartID.hpdID() );
  if ( m_smartid2L1.end() == id )
  {
    std::ostringstream mess;
    mess << "Unknown HPD RichSmartID " 
         << (int)smartID.hpdID() << " " << smartID.hpdID();
    throw GaudiException( mess.str(),
                          "DeRichSystem::level1HardwareID",
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
    mess << "Unknown HPD RichSmartID " 
         << (int)smartID.hpdID() << " " << smartID.hpdID();
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
//  level1HardwareID
//=========================================================================
const Rich::DAQ::Level1HardwareID
DeRichSystem::level1HardwareID( const Rich::DAQ::HPDHardwareID hardID ) const
{
  // See if this hardware ID is known
  HardIDToL1::const_iterator id = m_hardid2L1.find( hardID );
  if ( m_hardid2L1.end() == id )
  {
    throw GaudiException( "Unknown HPD hardware ID" + (std::string)hardID,
                          "DeRichSystem::level1HardwareID",
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
//  richSmartID from copy number
//=========================================================================
const LHCb::RichSmartID
DeRichSystem::richSmartID( const Rich::DAQ::HPDCopyNumber copyNumber ) const
{
  // See if this Level0 hardware ID is known
  CopyNToSmartID::const_iterator id = m_copyNumber2smartid.find( copyNumber );
  if ( m_copyNumber2smartid.end() == id )
  {
    throw GaudiException( "Unknown HPD Copy Number " + (std::string)copyNumber,
                          "DeRichSystem::richSmartID",
                          StatusCode::FAILURE );
  }

  // Found, so return RichSmartID
  return (*id).second;
}

//=========================================================================
// Obtain the Copy Number number for a given RichSmartID
//=========================================================================
const Rich::DAQ::HPDCopyNumber
DeRichSystem::copyNumber( const LHCb::RichSmartID smartID ) const
{
  // See if this RichSmartID is known
  SmartIDToCopyN::const_iterator id = m_smartid2copyNumber.find( smartID.hpdID() );
  if ( m_smartid2copyNumber.end() == id )
  {
    std::ostringstream mess;
    mess << "Unknown HPD RichSmartID " 
         << (int)smartID.hpdID() << " " << smartID.hpdID();
    throw GaudiException( mess.str(),
                          "DeRichSystem::copyNumber",
                          StatusCode::FAILURE );
  }

  // Found, so return copy number
  return (*id).second;
}

//=========================================================================
//  l1HPDSmartIDs
//=========================================================================
const LHCb::RichSmartID::Vector &
DeRichSystem::l1HPDSmartIDs( const Rich::DAQ::Level1HardwareID l1ID ) const
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
DeRichSystem::l1HPDHardIDs( const Rich::DAQ::Level1HardwareID l1ID ) const
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
// richDetector
//=========================================================================
Rich::DetectorType
DeRichSystem::richDetector( const Rich::DAQ::Level1HardwareID l1ID ) const
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
// Obtain the Level1 hardware ID number for a Level1 logical ID
//=========================================================================
const Rich::DAQ::Level1HardwareID
DeRichSystem::level1HardwareID( const Rich::DetectorType rich,
                                const Rich::DAQ::Level1LogicalID logID ) const
{
  L1LogToHard::const_iterator iID = m_l1LogToHard[rich].find(logID);
  if ( m_l1LogToHard[rich].end() == iID )
  {
    throw GaudiException( "Unknown L1 logical ID " + (std::string)logID,
                          "DeRichSystem::level1HardwareID",
                          StatusCode::FAILURE );
  }
  return (*iID).second;
}

//=========================================================================
// Obtain the Level1 hardware ID number for a Level0 lID
//=========================================================================
const Rich::DAQ::Level1HardwareID 
DeRichSystem::level1HardwareID( const Rich::DAQ::Level0ID l0ID ) const
{
  L0ToL1::const_iterator iID = m_l0ToL1.find(l0ID);
  if ( m_l0ToL1.end() == iID )
  {
    throw GaudiException( "Unknown L0 ID " + (std::string)l0ID,
                          "DeRichSystem::level1HardwareID",
                          StatusCode::FAILURE );
  }
  return (*iID).second;
}

//=========================================================================
// Obtain the Level1 logical ID number for a Level1 hardware ID
//=========================================================================
const Rich::DAQ::Level1LogicalID
DeRichSystem::level1LogicalID( const Rich::DAQ::Level1HardwareID hardID ) const
{
  L1HardToLog::const_iterator iID = m_l1HardToLog.find(hardID);
  if ( m_l1HardToLog.end() == iID )
  {
    throw GaudiException( "Unknown L1 hardware ID " + (std::string)hardID,
                          "DeRichSystem::level1LogicalID",
                          StatusCode::FAILURE );
  }
  return (*iID).second;
}

//=========================================================================
// Access the HPD hardware ID for the given L1 hardwareID and input number
//=========================================================================
const Rich::DAQ::HPDHardwareID
DeRichSystem::hpdHardwareID( const Rich::DAQ::Level1HardwareID L1HardID,
                             const Rich::DAQ::Level1Input L1Input ) const
{
  const L1HardIDAndInput key(L1HardID,L1Input);
  L1HardIDAndInputToHPDHardID::const_iterator iID = m_L1HardIDAndInputToHPDHardID.find(key);
  if ( m_L1HardIDAndInputToHPDHardID.end() == iID )
  {
    throw GaudiException( "Unknown L1 hardware ID " + (std::string)L1HardID +
                          " and L1 input " + (std::string)L1Input + " pair",
                          "DeRichSystem::hpdHardwareID",
                          StatusCode::FAILURE );
  }
  return (*iID).second;
}

//=========================================================================
// L1 Logical ID to Copy Number
//=========================================================================
const Rich::DAQ::Level1CopyNumber
DeRichSystem::copyNumber( const Rich::DAQ::Level1HardwareID hardID ) const
{
  L1HIDToCopyN::const_iterator iCN = m_l1H2CopyN.find(hardID);
  if ( m_l1H2CopyN.end() == iCN )
  {
    throw GaudiException( "Unknown L1 Hardware ID " + (std::string)hardID,
                          "DeRichSystem::copyNumber",
                          StatusCode::FAILURE );
  }
  return (*iCN).second;
}

//=========================================================================
// getDeHPDLocation
//=========================================================================
std::string DeRichSystem::getDeHPDLocation ( const LHCb::RichSmartID smartID ) const
{
  std::string loc;

  if ( deRich(smartID.rich())->exists("HPDPanelDetElemLocations") )
  {
    const std::vector<std::string>& panelLoc = 
      deRich(smartID.rich())->paramVect<std::string>("HPDPanelDetElemLocations");
    loc = panelLoc[smartID.panel()];
  }
  else
  {
    if( smartID.rich() == Rich::Rich1 )
    {
      if( smartID.panel() == Rich::top )
        loc = DeRichLocations::Rich1Panel0;
      else
        loc = DeRichLocations::Rich1Panel1;
    }
    else
    {
      if( smartID.panel() == Rich::left )
        loc = DeRichLocations::Rich2Panel0;
      else
        loc = DeRichLocations::Rich2Panel1;
    }
  }

  const Rich::DAQ::HPDCopyNumber cNumber = copyNumber( smartID );
  return loc + "/HPD:" + std::string(cNumber);
}
//===========================================================================

const Rich::DAQ::Level1LogicalID 
DeRichSystem::level1LogicalID( const LHCb::RichSmartID smartID ) const
{
  return level1LogicalID( level1HardwareID(smartID) );
}
