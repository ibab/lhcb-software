 
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
#include "RichDet/DeRich.h"

// boost
#include "boost/format.hpp"
#include "boost/lexical_cast.hpp"

//=============================================================================

const CLID CLID_DERichSystem = 12005;  // User defined

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DeRichSystem::DeRichSystem( const std::string & name )
  : DeRichBase     ( name            ),
    m_photDetConf  ( Rich::HPDConfig ), // assume HPD by default
    m_firstL1CopyN ( 0               ),
    m_version      ( 0               )  // default version 0
{
  m_deRich[Rich::Rich1] = NULL;
  m_deRich[Rich::Rich2] = NULL;
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

  // get the version number
  if ( exists("systemVersion") )
    m_version = param<int>("systemVersion");
  if ( msgLevel(MSG::DEBUG) )
    debug() << "System version:" << systemVersion() << endmsg;

  // get rich detectors
  std::vector<std::string> deRichLocs = getDeRichLocations();

  // get condition names for detector numbers
  std::vector<std::string> detCondNames;
  if ( exists("DetectorNumbersConditions") )
  {
    detCondNames = paramVect<std::string>("DetectorNumbersConditions");
  }
  else
  {
    detCondNames.push_back("Rich1DetectorNumbers");
    detCondNames.push_back("Rich2DetectorNumbers");
  }

  // check if the numbers match.
  if ( 0 != detCondNames.size()%deRichLocs.size() )
  {
    error() << "Number of rich detector does not match detector number conditions" << endmsg;
    return StatusCode::FAILURE;
  }

  // for version 1 there are separate conditions for inactive PDs
  std::vector<std::string> inactiveCondNames;
  if ( systemVersion() == 1 )
  {
    inactiveCondNames.push_back("Rich1InactivePDs");
    inactiveCondNames.push_back("Rich2InactivePDs");
  }

  // loop over detectors and conditions to set things up
  for ( unsigned int i = 0; i < deRichLocs.size(); ++i)
  {
    //SmartDataPtr<DeRich> deR( dataSvc(), deRichLocs[i] );

    // if ( !deR )
    // {
    //   error() << "Cannot get Rich detector at: " << deRichLocs[i] << endmsg;
    //   return StatusCode::FAILURE;
    // }
    // m_deRich[deR->rich()] = deR;
    // m_detNumConds[deR->rich()] = condNames[i];

    m_detNumConds[(Rich::DetectorType)i] = detCondNames[i];

    updMgrSvc()->registerCondition( this,
                                    condition(detCondNames[i]).path(),
                                    &DeRichSystem::buildPDMappings );
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Registered:" << condition(detCondNames[i]).path() << endmsg;

    if ( systemVersion() == 1)
    {
      m_inactivePDConds[(Rich::DetectorType)i] = inactiveCondNames[i];
      updMgrSvc()->registerCondition( this,
                                      condition(inactiveCondNames[i]).path(),
                                      &DeRichSystem::buildPDMappings );
      if ( msgLevel(MSG::DEBUG) )
        debug() << "Registered:" << condition(inactiveCondNames[i]).path() << endmsg;
    }
  }

  // Run first update
  const StatusCode sc = updMgrSvc()->update(this);
  if ( sc.isFailure() ) error() << "Failed to update mappings" << endmsg;

  if ( msgLevel(MSG::DEBUG) )
    debug() << "DeRichSystem initialized " << endmsg;

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
  std::map<Rich::DetectorType, DetectorElement*>::const_iterator richElement;
  richElement = m_deRich.find(rich);

  if ( richElement != m_deRich.end() )
  {
    if ( !m_deRich[rich] )
    {
      SmartDataPtr<DetectorElement> deR( dataSvc(), DeRichLocations::location(rich) );
      m_deRich[rich] = deR;
    }

    return richElement->second;

  }
  else
  {
    throw GaudiException( "Did not find Rich Detector","DeRichSystem::deRich",
                          StatusCode::FAILURE );
  }
  return NULL;
}

//=========================================================================
//  buildPDMappings
//=========================================================================
StatusCode DeRichSystem::buildPDMappings()
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Update triggered for PD numbering maps" << endmsg;

  // clear maps and containers
  m_soft2hard.clear();
  m_hard2soft.clear();
  m_l0hard2soft.clear();
  m_activePDSmartIDs.clear();
  m_inactivePDSmartIDs.clear();
  m_allPDSmartIDs.clear();
  m_activePDHardIDs.clear();
  m_inactivePDHardIDs.clear();
  m_allPDHardIDs.clear();
  m_smartid2L0.clear();
  m_hardid2L0.clear();
  m_l0ToL1.clear();
  m_smartid2L1.clear();
  m_hardid2L1.clear();
  m_smartid2L1In.clear();
  m_hardid2L1In.clear();
  m_l1ToRich.clear();
  m_l12smartids.clear();
  m_l12hardids.clear();
  m_L1HardIDAndInputToPDHardID.clear();
  m_l1IDs.clear();
  m_smartid2copyNumber.clear();
  m_copyNumber2smartid.clear();
  m_l1H2CopyN.clear();
  m_l1LogToHard.clear();
  m_l1HardToLog.clear();
  m_firstL1CopyN = 0;

  // Fill the maps for each RICH
  std::map<Rich::DetectorType, DetectorElement*>::iterator richElement;
  for ( richElement = m_deRich.begin(); richElement != m_deRich.end(); ++richElement)
  {
    const StatusCode sc = fillMaps ( richElement->first );
    if ( !sc )
      return sc;
  }

  //const StatusCode sc = ( fillMaps ( Rich::Rich1 ) &&
  //                         fillMaps ( Rich::Rich2 ) );
  // return

  return StatusCode::SUCCESS;
}

//=========================================================================
//  fillMaps
//=========================================================================
StatusCode DeRichSystem::fillMaps( const Rich::DetectorType rich )
{
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Building Mappings for " << rich << endmsg;

  std::string str_NumberOfPDs              = "NumberOfHPDs";
  std::string str_PDSmartIDs               = "HPDSmartIDs";
  std::string str_PDHardwareIDs            = "HPDHardwareIDs";
  std::string str_PDLevel0IDs              = "HPDLevel0IDs";
  std::string str_PDLevel1HardwareIDs      = "HPDLevel1HardwareIDs";
  std::string str_PDLevel1InputNums        = "HPDLevel1InputNums";
  std::string str_PDCopyNumbers            = "HPDCopyNumbers";
  std::string str_InactivePDListInSmartIDs = "InactiveHPDListInSmartIDs";
  std::string str_InactivePDs              = "InactiveHPDs";
  std::string str_PDLevel1IDs              = "HPDLevel1IDs";
  std::string str_Level1LogicalToHardwareIDMap = "Level1LogicalToHardwareIDMap";
  std::string str_PhotoDetConfig           = "RichPhotoDetectorConfiguration";
  std::string str_PhotoDetConfigValue      = "DetectorConfiguration";
  if ( hasCondition ( str_PhotoDetConfig ) )
  {
    const SmartRef<Condition> deRC = condition( str_PhotoDetConfig );
    m_photDetConf =
      (Rich::RichPhDetConfigType) deRC->param<int>(str_PhotoDetConfigValue) ;
    if ( m_photDetConf == Rich::PMTConfig )
    {
      if ( !exists("DetectorNumbersConditions") )
      {
        m_detNumConds[Rich::Rich1] = "Rich1PMTDetectorNumbers";
        m_detNumConds[Rich::Rich2] = "Rich2PMTDetectorNumbers";
      }
      str_NumberOfPDs           = "NumberOfPMTs";
      str_PDSmartIDs            = "PMTSmartIDs";
      str_PDHardwareIDs         = "PMTHardwareIDs";
      str_PDLevel0IDs           = "PMTLevel0IDs";
      str_PDLevel1HardwareIDs   = "PMTLevel1HardwareIDs";
      str_PDLevel1InputNums     = "PMTLevel1InputNums";
      str_PDCopyNumbers         = "PMTCopyNumbers";
      str_InactivePDListInSmartIDs = "InactivePMTListInSmartIDs";
      str_InactivePDs           = "InactivePMTs";
      str_PDLevel1IDs           = "PMTLevel1IDs";
      str_Level1LogicalToHardwareIDMap = "Level1LogicalToHardwareIDMap-PMT";
    }
  }

  // load conditions
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Loading Conditions from " <<  m_detNumConds[rich] << endmsg;
  const SmartRef<Condition> numbers = condition(m_detNumConds[rich]);
  if ( msgLevel(MSG::DEBUG) )
    debug() << m_detNumConds[rich] << " since:" << numbers->validSince().format(true)
            << " till:" << numbers->validTill().format(true) << endmsg;

  SmartRef<Condition> inactives;
  if ( systemVersion() == 1 )
  {
    inactives = condition(m_inactivePDConds[rich]);
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Inactive list since:" << inactives->validSince().format(true)
              << " till:" << inactives->validTill().format(true) << endmsg;
  }

  // local typedefs for vector from Conditions
  typedef std::vector<int> CondData;
  typedef std::vector<std::string> L1Mapping;

  // number of PDs
  const unsigned int nPDs  = numbers->param<int>(str_NumberOfPDs);
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Condition " << str_NumberOfPDs << " = " << nPDs << endmsg;

  // vector of PD RichSmartIDs
  const CondData & softIDs = numbers->paramVect<int>(str_PDSmartIDs);
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Condition " << str_PDSmartIDs << " = " << softIDs << endmsg;

  // vector of PD hardware IDs
  const CondData & hardIDs = numbers->paramVect<int>(str_PDHardwareIDs);
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Condition " << str_PDHardwareIDs << " = " << hardIDs << endmsg;

  // vector of PD Level0 IDs
  const CondData & l0IDs   = numbers->paramVect<int>(str_PDLevel0IDs);
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Condition " << str_PDLevel0IDs << " = " << l0IDs << endmsg;

  // vector of PD Level1 board Hardware IDs
  const CondData & l1IDs   = numbers->paramVect<int>(str_PDLevel1HardwareIDs);
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Condition " << str_PDLevel1HardwareIDs << " = " << l1IDs << endmsg;

  // vector of PD Level1 input numbers
  const CondData & l1Ins   = numbers->paramVect<int>(str_PDLevel1InputNums);
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Condition " << str_PDLevel1InputNums << " = " << l1Ins << endmsg;

  // vector of PD Copy numbers
  const CondData & copyNs  = numbers->paramVect<int>(str_PDCopyNumbers);
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Condition " << str_PDCopyNumbers << " = " << copyNs << endmsg;

  // inactive PDs
  CondData inacts;
  const bool inactivePDListInSmartIDs( numbers->exists(str_InactivePDListInSmartIDs) ||
                                       systemVersion() == 1 );
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Condition " << str_InactivePDListInSmartIDs
              << " exists = " << inactivePDListInSmartIDs << endmsg;
  if ( inactivePDListInSmartIDs )
  {
    // smartIDs
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Inactive PDs are taken from the smartID list" << endmsg;
    const CondData& inactsHuman = ( systemVersion() == 1 ) ?
      inactives->paramVect<int>(str_InactivePDListInSmartIDs) :
      numbers->paramVect<int>(str_InactivePDListInSmartIDs);

    inacts.reserve(inactsHuman.size());
    for ( CondData::const_iterator inpd = inactsHuman.begin();
          inpd != inactsHuman.end(); ++inpd )
    {
      const LHCb::RichSmartID ID( Rich::DAQ::HPDIdentifier(*inpd).smartID() );
      if ( ID.isValid() )
      {
        inacts.push_back( ID );
        if ( std::find( softIDs.begin(), softIDs.end(), (int)ID ) == softIDs.end() )
          warning() << "Invalid smartID in list of inactive PDs: " << *inpd << endmsg;
      }
      else
      {
        error() << "Invalid smartID in the list of inactive PDs " << *inpd << endmsg;
      }
    }
  }

  else
  {
    // hardware IDs
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Inactive PDs are taken from the hardware list" << endmsg;
    inacts = numbers->paramVect<int>(str_InactivePDs);
  }
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Condition InactiveHPDs = " << inacts << endmsg;

  // check consistency
  if ( nPDs != softIDs.size() ||
       nPDs != hardIDs.size() ||
       nPDs != l1IDs.size()   ||
       nPDs != l0IDs.size()   ||
       nPDs != l1Ins.size()   ||
       nPDs != copyNs.size()   )
  {
    error() << "Mismatch in " << rich << " PD numbering schemes : # PDs = "
            << nPDs << " # SmartIDs = " << softIDs.size() << " # HardIDs = "
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
    const LHCb::RichSmartID           pdID   ( *iSoft  );
    const Rich::DAQ::HPDHardwareID    hardID ( *iHard  );
    const Rich::DAQ::Level1HardwareID L1ID   ( *iL1    );
    const Rich::DAQ::Level0ID         L0ID   ( *iL0    );
    const Rich::DAQ::Level1Input      L1IN   ( *iL1In  );
    const Rich::DAQ::HPDCopyNumber    copyN  ( *icopyN );

    // debug printout
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << "PD     " << (int)pdID << " " << pdID
                << " PDhardID " << hardID << " L0 " << L0ID << " L1 HardID " << L1ID
                << " L1 input " << L1IN << endmsg;

    // Sanity checks that this PD is not already in the maps
    if ( m_soft2hard.find(pdID) != m_soft2hard.end() )
    {
      error() << "Multiple entries for PD RichSmartID " << pdID;
      return  StatusCode::FAILURE;
    }
    if ( m_hard2soft.find(hardID) != m_hard2soft.end() )
    {
      error() << "Multiple entries for PD hardware ID "
              << (std::string)hardID << pdID << endmsg;
      return StatusCode::FAILURE;
    }
    if ( m_l0hard2soft.find(L0ID) != m_l0hard2soft.end() )
    {
      error() << "Multiple entries for PD L0 ID "
              << (std::string)L0ID << endmsg;
      return StatusCode::FAILURE;
    }
    if ( m_copyNumber2smartid.find(copyN) != m_copyNumber2smartid.end() )
    {
      error() << "Multiple entries for PD copy number "
              << (std::string)copyN << pdID << endmsg;
      return StatusCode::FAILURE;
    }

    // set up mappings etc.

    CondData::const_iterator & myID = ( inactivePDListInSmartIDs ? iSoft : iHard );
    if ( std::find( inacts.begin(), inacts.end(), *myID ) == inacts.end() )
    {
      m_activePDSmartIDs.push_back ( pdID  );
      m_activePDHardIDs.push_back  ( hardID );
    }
    else
    {
      if ( !pdIsActive(hardID) )
      {
        error() << "PD " << pdID << " hardID " << hardID
                << " listed twice in INACTIVE PD list !" << endmsg;
      }
      else
      {
        m_inactivePDHardIDs.push_back  ( hardID );
        m_inactivePDSmartIDs.push_back ( pdID   );
        if ( msgLevel(MSG::DEBUG) )
          debug() << "PD " << pdID << " hardID " << hardID << " is INACTIVE" << endmsg;
      }
    }

    // Fill maps
    bool OK = true;
    m_allPDHardIDs.push_back(hardID);
    m_allPDSmartIDs.push_back(pdID);

    OK &= safeMapFill(pdID,hardID,m_soft2hard);
    OK &= safeMapFill(hardID,pdID,m_hard2soft);
    OK &= safeMapFill(L0ID,pdID,m_l0hard2soft);
    OK &= safeMapFill(pdID,L0ID,m_smartid2L0);
    OK &= safeMapFill(hardID,L0ID,m_hardid2L0);
    OK &= safeMapFill(pdID,L1ID,m_smartid2L1);
    OK &= safeMapFill(hardID,L1ID,m_hardid2L1);
    OK &= safeMapFill(pdID,L1IN,m_smartid2L1In);
    OK &= safeMapFill(hardID,L1IN,m_hardid2L1In);
    OK &= safeMapFill(L0ID,L1ID,m_l0ToL1);
    OK &= safeMapFill(pdID,copyN,m_smartid2copyNumber);
    OK &= safeMapFill(copyN,pdID,m_copyNumber2smartid);
    m_l12smartids[L1ID].push_back( pdID );
    m_l12hardids[L1ID].push_back( hardID );
    const L1HardIDAndInput idAndInput(L1ID,L1IN);

    OK &= safeMapFill(idAndInput,hardID,m_L1HardIDAndInputToPDHardID);
    if ( std::find( m_l1IDs.rbegin(), m_l1IDs.rend(), L1ID ) == m_l1IDs.rend() )
    {
      m_l1ToRich[L1ID] = rich;
      m_l1IDs.push_back( L1ID );
    }
    if ( !OK ) return StatusCode::FAILURE;

  } // end loop over conditions data

  // Sort PD lists
  std::stable_sort( m_activePDHardIDs.begin(),    m_activePDHardIDs.end()    );
  std::stable_sort( m_activePDSmartIDs.begin(),   m_activePDSmartIDs.end()   );
  std::stable_sort( m_inactivePDHardIDs.begin(),  m_inactivePDHardIDs.end()  );
  std::stable_sort( m_inactivePDSmartIDs.begin(), m_inactivePDSmartIDs.end() );
  std::stable_sort( m_allPDHardIDs.begin(),       m_allPDHardIDs.end()       );
  std::stable_sort( m_allPDSmartIDs.begin(),      m_allPDSmartIDs.end()      );

  // L1 mapping
  const std::string L1LogToHardMapName(str_Level1LogicalToHardwareIDMap);

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
            << " L1 and " << nPDs << " PDs in " << rich << endmsg;

  return StatusCode::SUCCESS;
}

//=========================================================================
//  hardwareID
//=========================================================================
const Rich::DAQ::HPDHardwareID
DeRichSystem::hardwareID( const LHCb::RichSmartID& smartID ) const
{
  // See if this RichSmartID is known
  SoftToHard::const_iterator id = m_soft2hard.find( smartID.pdID() );
  if ( m_soft2hard.end() == id )
  {
    std::ostringstream mess;
    mess << "Unknown PD RichSmartID "
         << (int)smartID.pdID() << " " << smartID.pdID();
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
DeRichSystem::richSmartID( const Rich::DAQ::HPDHardwareID& hID ) const
{
  // See if this PD hardware ID is known
  HardToSoft::const_iterator id = m_hard2soft.find( hID );
  if ( m_hard2soft.end() == id )
  {
    throw GaudiException( "Unknown PD hardware ID " + (std::string)hID,
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
DeRichSystem::richSmartID( const Rich::DAQ::Level0ID& l0ID ) const
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
// level0ID
//=========================================================================
const Rich::DAQ::Level0ID
DeRichSystem::level0ID( const LHCb::RichSmartID& smartID ) const
{
  // See if this RichSmartID is known
  SmartIDToL0::const_iterator id = m_smartid2L0.find( smartID.pdID() );
  if ( m_smartid2L0.end() == id )
  {
    std::ostringstream mess;
    mess << "Unknown PD RichSmartID "
         << (int)smartID.pdID() << " " << smartID.pdID();
    throw GaudiException( mess.str(),
                          "DeRichSystem::level0ID",
                          StatusCode::FAILURE );
  }

  // Found, so return Level 0 ID
  return (*id).second;
}

//=========================================================================
// level1HardwareID
//=========================================================================
const Rich::DAQ::Level1HardwareID
DeRichSystem::level1HardwareID( const LHCb::RichSmartID& smartID ) const
{
  // See if this RichSmartID is known
  SmartIDToL1::const_iterator id = m_smartid2L1.find( smartID.pdID() );
  if ( m_smartid2L1.end() == id )
  {
    std::ostringstream mess;
    mess << "Unknown PD RichSmartID "
         << (int)smartID.pdID() << " " << smartID.pdID();
    throw GaudiException( mess.str(),
                          "DeRichSystem::level1HardwareID",
                          StatusCode::FAILURE );
  }
  // Found, so return Level1 board number
  return (*id).second;
}

//=========================================================================
// level1InputNum
// Obtain the Level1 input number for a given RichSmartID
//=========================================================================
const Rich::DAQ::Level1Input
DeRichSystem::level1InputNum( const LHCb::RichSmartID& smartID ) const
{
  // See if this RichSmartID is known
  SmartIDToL1In::const_iterator id = m_smartid2L1In.find( smartID.pdID() );
  if ( m_smartid2L1In.end() == id )
  {
    std::ostringstream mess;
    mess << "Unknown PD RichSmartID "
         << (int)smartID.pdID() << " " << smartID.pdID();
    throw GaudiException( mess.str(),
                          "DeRichSystem::level1InputNum",
                          StatusCode::FAILURE );
  }

  // Found, so return Level 1 input number
  return (*id).second;
}

//=========================================================================
// level0ID
//=========================================================================
const Rich::DAQ::Level0ID
DeRichSystem::level0ID( const Rich::DAQ::HPDHardwareID& hardID ) const
{
  // See if this hardware ID is known
  HardIDToL0::const_iterator id = m_hardid2L0.find( hardID );
  if ( m_hardid2L0.end() == id )
  {
    throw GaudiException ( "Unknown PD hardware ID" + (std::string)hardID,
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
DeRichSystem::level1HardwareID( const Rich::DAQ::HPDHardwareID& hardID ) const
{
  // See if this hardware ID is known
  HardIDToL1::const_iterator id = m_hardid2L1.find( hardID );
  if ( m_hardid2L1.end() == id )
  {
    throw GaudiException( "Unknown PD hardware ID" + (std::string)hardID,
                          "DeRichSystem::level1HardwareID",
                          StatusCode::FAILURE );
  }

  // Found, so return Level 1 board number
  return (*id).second;
}

//=========================================================================
//  level1InputNum
/// Obtain the Level1 input number for a given PD hardware ID
//=========================================================================
const Rich::DAQ::Level1Input
DeRichSystem::level1InputNum( const Rich::DAQ::HPDHardwareID& hardID ) const
{
  // See if this hardware ID is known
  HardIDToL1In::const_iterator id = m_hardid2L1In.find( hardID );
  if ( m_hardid2L1In.end() == id )
  {
    throw GaudiException( "Unknown PD hardware ID" + (std::string)hardID,
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
DeRichSystem::richSmartID( const Rich::DAQ::HPDCopyNumber& copyNumber ) const
{
  // See if this Level0 hardware ID is known
  CopyNToSmartID::const_iterator id = m_copyNumber2smartid.find( copyNumber );
  if ( m_copyNumber2smartid.end() == id )
  {
    throw GaudiException( "Unknown PD Copy Number " + (std::string)copyNumber,
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
DeRichSystem::copyNumber( const LHCb::RichSmartID& smartID ) const
{
  // See if this RichSmartID is known
  SmartIDToCopyN::const_iterator id = m_smartid2copyNumber.find( smartID.pdID() );
  if ( m_smartid2copyNumber.end() == id )
  {
    std::ostringstream mess;
    mess << "Unknown PD RichSmartID "
         << (int)smartID.pdID() << " " << smartID.pdID();
    throw GaudiException( mess.str(),
                          "DeRichSystem::copyNumber",
                          StatusCode::FAILURE );
  }

  // Found, so return copy number
  return (*id).second;
}

//=========================================================================
//  l1PDSmartIDs
//=========================================================================
const LHCb::RichSmartID::Vector &
DeRichSystem::l1PDSmartIDs( const Rich::DAQ::Level1HardwareID& l1ID ) const
{
  // See if this L1 ID is known
  Rich::DAQ::L1ToSmartIDs::const_iterator id = m_l12smartids.find( l1ID );
  if ( m_l12smartids.end() == id )
  {
    throw GaudiException( "Unknown RICH Level1 board ID " + (std::string)l1ID,
                          "DeRichSystem::l1PDSmartIDs", StatusCode::FAILURE );
  }

  // Found, so return list
  return (*id).second;
}

//=========================================================================
//  l1PDHardIDs
//=========================================================================
const Rich::DAQ::HPDHardwareIDs &
DeRichSystem::l1PDHardIDs( const Rich::DAQ::Level1HardwareID& l1ID ) const
{
  // See if this L1 ID is known
  Rich::DAQ::L1ToHardIDs::const_iterator id = m_l12hardids.find( l1ID );
  if ( m_l12hardids.end() == id )
  {
    throw GaudiException( "Unknown RICH Level1 board ID " + (std::string)l1ID,
                          "DeRichSystem::l1PDHardIDs", StatusCode::FAILURE );
  }

  // Found, so return list
  return (*id).second;
}

//=========================================================================
// richDetector
//=========================================================================
Rich::DetectorType
DeRichSystem::richDetector( const Rich::DAQ::Level1HardwareID& l1ID ) const
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
                                const Rich::DAQ::Level1LogicalID& logID ) const
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
DeRichSystem::level1HardwareID( const Rich::DAQ::Level0ID& l0ID ) const
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
DeRichSystem::level1LogicalID( const Rich::DAQ::Level1HardwareID& hardID ) const
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
// Access the PD hardware ID for the given L1 hardwareID and input number
//=========================================================================
const Rich::DAQ::HPDHardwareID
DeRichSystem::pdHardwareID( const Rich::DAQ::Level1HardwareID& L1HardID,
                            const Rich::DAQ::Level1Input& L1Input ) const
{
  const L1HardIDAndInput key(L1HardID,L1Input);
  L1HardIDAndInputToPDHardID::const_iterator iID = m_L1HardIDAndInputToPDHardID.find(key);
  if ( m_L1HardIDAndInputToPDHardID.end() == iID )
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
DeRichSystem::copyNumber( const Rich::DAQ::Level1HardwareID& hardID ) const
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
// getDePDLocation
//=========================================================================
std::string 
DeRichSystem::getDePDLocation( const LHCb::RichSmartID& smartID ) const
{
  std::string loc;

  if ( UNLIKELY( smartID.idType() == LHCb::RichSmartID::MaPMTID ) )
  {
    if ( deRich(smartID.rich())->exists("PMTPanelDetElemLocations") )
    {
      const std::vector<std::string>& panelLoc =
        deRich(smartID.rich())->paramVect<std::string>("PMTPanelDetElemLocations");
      loc = panelLoc[smartID.panel()];
    }
    else
    {
      if ( smartID.rich() == Rich::Rich1 )
      {
        if ( smartID.panel() == Rich::top )
        {
          loc = DeRichLocations::Rich1Panel0;
        }
        else
        {
          loc = DeRichLocations::Rich1Panel1;
        }
      }
      else
      {
        if ( smartID.panel() == Rich::left )
        {
          loc = DeRichLocations::Rich2Panel0;
        }
        else
        {
          loc = DeRichLocations::Rich2Panel1;
        }
      }
    }

    const unsigned int aM = smartID.pdCol();
    const unsigned int aP = smartID.pdNumInCol();
    const std::string ast_aM = boost::lexical_cast<std::string>(aM);
    const std::string ast_aP = boost::lexical_cast<std::string>(aP);
    return ( loc+"/MAPMT_MODULE:"+ast_aM+"/MAPMT:"+ast_aP );

  }
  else // HPDs
  {

    if ( deRich(smartID.rich())->exists("HPDPanelDetElemLocations") )
    {
      const std::vector<std::string>& panelLoc =
        deRich(smartID.rich())->paramVect<std::string>("HPDPanelDetElemLocations");
      loc = panelLoc[smartID.panel()];
    }
    else
    {
      if ( smartID.rich() == Rich::Rich1 )
      {
        if ( smartID.panel() == Rich::top )
        {
          loc = DeRichLocations::Rich1Panel0;
        }
        else
        {
          loc = DeRichLocations::Rich1Panel1;
        }
      }
      else
      {
        if( smartID.panel() == Rich::left )
        {
          loc = DeRichLocations::Rich2Panel0;
        }
        else
        {
          loc = DeRichLocations::Rich2Panel1;
        }
      }
    }

    const Rich::DAQ::HPDCopyNumber cNumber = copyNumber( smartID );
    return loc + "/HPD:" + std::string(cNumber);
  }

}

//===========================================================================

const Rich::DAQ::Level1LogicalID
DeRichSystem::level1LogicalID( const LHCb::RichSmartID& smartID ) const
{
  return level1LogicalID( level1HardwareID(smartID) );
}

//===========================================================================

unsigned int DeRichSystem::nPDs( const Rich::DetectorType rich ) const
{
  unsigned int nPDs = 0;
  for ( LHCb::RichSmartID::Vector::const_iterator iPD = allPDRichSmartIDs().begin();
        iPD != allPDRichSmartIDs().end(); ++iPD )
  {
    if ( (*iPD).rich() == rich ) { ++nPDs; }
  }
  return nPDs;
}

//=========================================================================
//  getDeRichLocations
//=========================================================================
std::vector<std::string> DeRichSystem::getDeRichLocations ( )
{
  // find the Rich detectos
  SmartDataPtr<DetectorElement> afterMag(dataSvc(),"/dd/Structure/LHCb/AfterMagnetRegion");
  if ( !afterMag )
  {
    throw GaudiException( "Could not load AfterMagnetRegion ",
                          "DeRichBase::deRichSys()", StatusCode::FAILURE );
  }

  std::vector<std::string> deRichLocs;
  if ( afterMag->exists("RichDetectorLocations") )
  {
    deRichLocs = afterMag->paramVect<std::string>("RichDetectorLocations");
  }
  else
  {
    deRichLocs.push_back( DeRichLocations::Rich1 );
    deRichLocs.push_back( DeRichLocations::Rich2 );
  }

  return deRichLocs;

}

//===========================================================================
