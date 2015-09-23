
/** @file DeRichSystem.h
 *
 *  Header file for detector description class : DeRichSystem
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2006-01-26
 */

#ifndef RICHDET_DERICHSYSTEM_H
#define RICHDET_DERICHSYSTEM_H 1

// base class
#include "RichDet/DeRichBase.h"

// LHCbKernel
#include "Kernel/RichSide.h"
#include "Kernel/RichDetectorType.h"
#include "Kernel/RichSmartID.h"

// RichKernel
#include "RichKernel/RichDAQDefinitions.h"
#include "RichKernel/RichMap.h"

// local
#include "RichDet/RichDetConfigType.h"

// External declarations
extern const CLID CLID_DERichSystem;

/** @class DeRichSystem RichDet/DeRichSystem.h
 *
 * Class for generic info about the Rich system. In particular
 * to provide conversions and mappings between the various
 * RICH numbering schemes and to provide data on which PDs, Level0 and Level1
 * boards are currently active.
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   27/01/2006
 */
class DeRichSystem: public DeRichBase
{

public:

  /// Constructor for this class
  DeRichSystem( const std::string & name = "" );

  /// Default destructor
  virtual ~DeRichSystem();

  /** Retrieves reference to class identifier
   *  @return the class identifier for this class
   */
  const CLID& clID() const { return classID(); }

  /** Retrieves reference to class identifier
   *  @return the class identifier for this class
   */
  static const CLID& classID();

  /** This is where most of the geometry is read and variables initialised
   *  @return Status of initialisation
   *  @retval StatusCode::FAILURE Initialisation failed, program should
   *  terminate
   */
  virtual StatusCode initialize();

public:

  /** Convert an PD RichSmartID into the corresponding PD hardware number
   *  @param smartID The RichSmartID for the PD
   *  @return The corresponding PD hardware ID
   */
  const Rich::DAQ::HPDHardwareID hardwareID( const LHCb::RichSmartID& smartID ) const;

  /** Convert a RICH PD hardware number into the corresponding PD RichSmartID
   *  @param hID The hardware ID for the PD
   *  @return The corresponding PD RichSmartID
   */
  const LHCb::RichSmartID richSmartID( const Rich::DAQ::HPDHardwareID& hID ) const;

  /** Convert a RICH Level0 hardware number into the corresponding PD RichSmartID
   *  @param l0ID The PD level0 for the PD
   *  @return The corresponding PD RichSmartID
   */
  const LHCb::RichSmartID richSmartID( const Rich::DAQ::Level0ID& l0ID ) const;

  /** Convert a RICH Copy Number into the corresponding PD RichSmartID
   *  @param copyNumber The PD Copy Number
   *  @return The corresponding PD RichSmartID
   */
  const LHCb::RichSmartID richSmartID( const Rich::DAQ::HPDCopyNumber& copyNumber ) const;

  /** Convert a RICH smartID to the corresponding PD Copy Number
   *  @param smartID The PD RichSmartID
   *  @return The corresponding PD Copy Number
   */
  const Rich::DAQ::HPDCopyNumber copyNumber( const LHCb::RichSmartID& smartID ) const;

  /// Returns a list of all active PDs identified by their RichSmartID
  const LHCb::RichSmartID::Vector & activePDRichSmartIDs() const;

  /// Returns a list of all inactive PDs identified by their RichSmartID
  const LHCb::RichSmartID::Vector & inactivePDRichSmartIDs() const;

  /// Returns a list of all (active and inactive) PDs identified by their RichSmartID
  const LHCb::RichSmartID::Vector & allPDRichSmartIDs() const;

  /// Returns a list of all active PDs identified by their hardware IDs
  const Rich::DAQ::HPDHardwareIDs & activePDHardwareIDs() const;

  /// Returns a list of all inactive PDs identified by their hardware IDs
  const Rich::DAQ::HPDHardwareIDs & inactivePDHardwareIDs() const;

  /// Returns a list of all (active and inactive) PDs identified by their hardware IDs
  const Rich::DAQ::HPDHardwareIDs & allPDHardwareIDs() const;

  /** Ask whether a given PD is currently active or dead
   *  @param id The RichSmartID for the PD
   *  @return boolean indicating if the given PD is active or not
   *  @attention For speed, this method does NOT check if the given PD id is a valid one
   *             or not. Invalid PD ids will return true
   */
  bool pdIsActive( const LHCb::RichSmartID& id ) const;

  /** Ask whether a given PD is currently active or dead
   *  @param id The hardware id for the PD
   *  @return boolean indicating if the given PD is active or not
   *  @attention For speed, this method does NOT check if the given PD id is a valid one
   *             or not. Invalid PD ids will return true
   */
  bool pdIsActive( const Rich::DAQ::HPDHardwareID& id ) const;

  /** Obtain the Level0 ID number for a given PD RichSmartID
   *  @param smartID The RichSmartID for the PD
   *  @return The corresponding PD Level0 ID
   */
  const Rich::DAQ::Level0ID level0ID( const LHCb::RichSmartID& smartID ) const;

  /** Obtain the Level0 ID number for a given PD hardware ID
   *  @param hardID The hardware ID for the PD
   *  @return The corresponding PD Level0 ID
   */
  const Rich::DAQ::Level0ID level0ID( const Rich::DAQ::HPDHardwareID& hardID ) const;

  /** Obtain the Level1 hardware ID number for a given PD Level0 ID
   *  @param l0ID The Level0 ID
   *  @return The corresponding Level1 hardware ID
   */
  const Rich::DAQ::Level1HardwareID level1HardwareID( const Rich::DAQ::Level0ID& l0ID ) const;

  /** Obtain the Level1 hardware ID number for a given PD RichSmartID
   *  @param smartID The RichSmartID for the PD
   *  @return The corresponding Level1 hardware ID
   */
  const Rich::DAQ::Level1HardwareID level1HardwareID( const LHCb::RichSmartID& smartID ) const;

  /** Obtain the Level1 hardware ID number for a given PD hardware ID
   *  @param hardID The hardware ID for the PD
   *  @return The corresponding Level1 hardware ID
   */
  const Rich::DAQ::Level1HardwareID level1HardwareID( const Rich::DAQ::HPDHardwareID& hardID ) const;

  /** Obtain the Level1 hardware ID number for a Level1 logical ID
   *  @param rich The RICH detector
   *  @param logID The logical ID for the PD
   *  @return The corresponding Level1 hardware ID
   */
  const Rich::DAQ::Level1HardwareID level1HardwareID( const Rich::DetectorType rich,
                                                      const Rich::DAQ::Level1LogicalID& logID ) const;

  /** Obtain the Level1 logical ID number for a given PD RichSmartID
   *  @param smartID The RichSmartID for the PD
   *  @return The corresponding Level1 logical ID
   */
  const Rich::DAQ::Level1LogicalID level1LogicalID( const LHCb::RichSmartID& smartID ) const;

  /** Obtain the Level1 logical ID number for a Level1 hardware ID
   *  @param hardID The hardware ID for the PD
   *  @return The corresponding PD Level1 logical ID
   */
  const Rich::DAQ::Level1LogicalID level1LogicalID( const Rich::DAQ::Level1HardwareID& hardID ) const;

  /** Obtain the Level1 logical ID number for a Level1 hardware ID
   *  @param hardID The hardware ID for the PD
   *  @return The corresponding PD Level1 logical ID
   */
  const Rich::DAQ::Level1CopyNumber copyNumber( const Rich::DAQ::Level1HardwareID& hardID ) const;

  /** Obtain the Level1 input number for a given PD RichSmartID
   *  @param smartID The RichSmartID for the PD
   *  @return The corrresponding Level1 input number
   */
  const Rich::DAQ::Level1Input level1InputNum( const LHCb::RichSmartID& smartID ) const;

  /** Obtain the Level1 input number for a given PD hardware ID
   *  @param hardID The hardware ID for the PD
   *  @return The corrresponding Level1 input number
   */
  const Rich::DAQ::Level1Input level1InputNum( const Rich::DAQ::HPDHardwareID& hardID ) const;

  /** Access the PD hardware ID for the given L1 hardwareID and input number
   *  @param L1HardID L1 board hardware ID
   *  @param L1Input  L1 input number
   *  @return PD hardware ID
   */
  const Rich::DAQ::HPDHardwareID
  pdHardwareID( const Rich::DAQ::Level1HardwareID& L1HardID,
                const Rich::DAQ::Level1Input& L1Input ) const;

  /** Obtain a list of RichSmartID PD identifiers for a given level1 hardwareID
   *  @param l1ID The level1 ID number
   *  @return Vector of all PD RichSmartIDs for that Level1 board
   */
  const LHCb::RichSmartID::Vector & l1PDSmartIDs( const Rich::DAQ::Level1HardwareID& l1ID ) const;

  /** Obtain a list of PD hardware identifiers for a given level1 hardware ID
   *  @param l1ID The level1 ID number
   *  @return Vector of all PD hardware IDs for that Level1 board
   */
  const Rich::DAQ::HPDHardwareIDs & l1PDHardIDs( const Rich::DAQ::Level1HardwareID& l1ID ) const;

  /** Reconst Rich::DAQ::PDHardwareIDturn which RICH detector a given Level1 hardware ID is for
   *  @param l1ID The Level 1 hardware ID
   *  @return The RICH detector
   */
  Rich::DetectorType richDetector( const Rich::DAQ::Level1HardwareID& l1ID ) const;

  /// Direct access to the mapping between Level1 IDs and PD RichSmartIDs
  const Rich::DAQ::L1ToSmartIDs & l1PDSmartIDs() const;

  /// Direct access to the mapping between Level1 IDs and PD RichSmartIDs
  const Rich::DAQ::L1ToHardIDs & l1PDHardIDs() const;

  /// Returns a list of all valid Level1 board hardware IDs
  const Rich::DAQ::Level1HardwareIDs & level1HardwareIDs() const;

  /// Returns the number of PDs in the given RICH detector
  unsigned int nPDs( const Rich::DetectorType rich ) const;

  /// Returns the total number of PDs
  unsigned int nPDs() const;

public:

  /**
   * Retrieves the location of the PD/PMT in the detector store, so it can be
   * loaded using the getDet<DeRichPD> method.
   * @return The location of the PD in the detector store
   */
  std::string getDePDLocation( const LHCb::RichSmartID& smartID ) const;

public:

  /// The photon detector type
  inline Rich::RichPhDetConfigType RichPhotoDetConfig() const
  {
    return m_photDetConf;
  }

  /// The version of RichSystem
  inline int systemVersion() const
  {
    return m_version;
  }

private: // methods

  /// Update methods for PD mappings
  StatusCode buildPDMappings();

  /// Fill the maps for the given RICH detector
  StatusCode fillMaps( const Rich::DetectorType rich );

  /// Access on demand the Detector Elements for Rich1 and Rich2
  DetectorElement * deRich( const Rich::DetectorType rich ) const;

  /// Save information to a map, checking first it is not already set
  template < class SOURCE, class TARGET, class MAP >
  bool safeMapFill( const SOURCE& source, const TARGET& target, MAP& map );

  /// Get the locations of the rich detectors
  std::vector<std::string> getDeRichLocations();

private: // data

  // RICH PhotoDetector Configuration
  Rich::RichPhDetConfigType m_photDetConf;

  /// Type for mapping from RichSmartID to Rich::DAQ::HPDHardwareID
  typedef GaudiUtils::HashMap< const LHCb::RichSmartID, Rich::DAQ::HPDHardwareID > SoftToHard;
  SoftToHard m_soft2hard; ///< Software ID to hardware ID map

  /// Type for mapping from Rich::DAQ::HPDHardwareID to RichSmartID
  typedef GaudiUtils::HashMap< const Rich::DAQ::HPDHardwareID, LHCb::RichSmartID > HardToSoft;
  HardToSoft m_hard2soft; ///< HPD Hardware ID to software ID map

  /// Type for mapping from Rich::DAQ::HPDHardwareID to RichSmartID
  typedef GaudiUtils::HashMap< const Rich::DAQ::Level0ID, LHCb::RichSmartID > L0HardToSoft;
  L0HardToSoft m_l0hard2soft; ///< Level0 Hardware ID to software ID map

  /// List of all active PD RichSmartIDs
  LHCb::RichSmartID::Vector m_activePDSmartIDs;

  /// List of all inactive PD RichSmartIDs
  LHCb::RichSmartID::Vector m_inactivePDSmartIDs;

  /// List of all PD RichSmartIDs
  LHCb::RichSmartID::Vector m_allPDSmartIDs;

  /// List of all active PD hardware IDs
  Rich::DAQ::HPDHardwareIDs m_activePDHardIDs;

  /// List of all inactive PD hardware IDs
  Rich::DAQ::HPDHardwareIDs m_inactivePDHardIDs;

  /// List of all inactive PD hardware IDs
  Rich::DAQ::HPDHardwareIDs m_allPDHardIDs;

  /// Typedef for mapping from RichSmartID to Level0 ID
  typedef GaudiUtils::HashMap< const LHCb::RichSmartID, Rich::DAQ::Level0ID > SmartIDToL0;
  SmartIDToL0 m_smartid2L0; ///< PD RichSmartID to L0 ID map

  /// Typedef for mapping from PD Hardware ID to Level0 ID
  typedef GaudiUtils::HashMap< const Rich::DAQ::HPDHardwareID, Rich::DAQ::Level0ID > HardIDToL0;
  HardIDToL0 m_hardid2L0; ///< PD Hardware ID to L0 ID map

  /// Typedef for mapping from Level0 to Level1 ID
  typedef GaudiUtils::HashMap< const Rich::DAQ::Level0ID, Rich::DAQ::Level1HardwareID > L0ToL1;
  L0ToL1 m_l0ToL1; ///< PD Level0 to L1 ID map

  /// Typedef for mapping from RichSmartID to Level1 ID
  typedef GaudiUtils::HashMap< const LHCb::RichSmartID, Rich::DAQ::Level1HardwareID > SmartIDToL1;
  SmartIDToL1 m_smartid2L1; ///< PD RichSmartID to L1 ID map

  /// Typedef for mapping from PD Hardware ID to Level1 ID
  typedef GaudiUtils::HashMap< const Rich::DAQ::HPDHardwareID, Rich::DAQ::Level1HardwareID > HardIDToL1;
  HardIDToL1 m_hardid2L1; ///< PD Hardware ID to L1 ID map

  /// Typedef for mapping from RichSmartID to Level1 input number
  typedef GaudiUtils::HashMap< const LHCb::RichSmartID, Rich::DAQ::Level1Input > SmartIDToL1In;
  SmartIDToL1In m_smartid2L1In; ///< PD RichSmartID to L1 input number map

  /// Typedef for mapping from PD Hardware ID to Level1 input number
  typedef GaudiUtils::HashMap< const Rich::DAQ::HPDHardwareID, Rich::DAQ::Level1Input > HardIDToL1In;
  HardIDToL1In m_hardid2L1In; ///< PD Hardware ID to L1 input number map

  /// Typedef for mapping between L1 boards and RICH detector
  typedef GaudiUtils::HashMap< const Rich::DAQ::Level1HardwareID, Rich::DetectorType > L1ToRICH;
  L1ToRICH m_l1ToRich; ///< L1 to RICH map

  Rich::DAQ::L1ToSmartIDs m_l12smartids; ///< L1 ID to RichSmartIDs map
  Rich::DAQ::L1ToHardIDs  m_l12hardids;  ///< L1 ID to PD hardware IDs map

  /// L1 hardware ID + Input number pair
  typedef std::pair< const Rich::DAQ::Level1HardwareID, const Rich::DAQ::Level1Input > L1HardIDAndInput;
  /// Typedef for mapping L1 HardwareID + L1 input number to PD Hardware ID
  typedef Rich::Map< L1HardIDAndInput, Rich::DAQ::HPDHardwareID > L1HardIDAndInputToPDHardID;
  L1HardIDAndInputToPDHardID m_L1HardIDAndInputToPDHardID;

  /// List of all valid Level1 IDs
  Rich::DAQ::Level1HardwareIDs m_l1IDs;

  /// smartID to copy number map
  typedef GaudiUtils::HashMap< const LHCb::RichSmartID, Rich::DAQ::HPDCopyNumber > SmartIDToCopyN;
  SmartIDToCopyN m_smartid2copyNumber;

  /// copy number to smartID map
  typedef GaudiUtils::HashMap< const Rich::DAQ::HPDCopyNumber, LHCb::RichSmartID > CopyNToSmartID;
  CopyNToSmartID m_copyNumber2smartid;

  /// L1 Logical ID to L1 Copy Number
  typedef GaudiUtils::HashMap< const Rich::DAQ::Level1HardwareID, Rich::DAQ::Level1CopyNumber > L1HIDToCopyN;
  L1HIDToCopyN m_l1H2CopyN;

  /// Rich1 & Rich2 detector elements
  mutable std::map<Rich::DetectorType, DetectorElement*> m_deRich;

  /// Location of RICH Numbering schemes in Conditions DB
  std::map<Rich::DetectorType, std::string> m_detNumConds;

  /// Location of inactive PDs in Conditions DB
  std::map<Rich::DetectorType, std::string> m_inactivePDConds;

  /// Logical to hardware L1 ID map
  typedef GaudiUtils::HashMap< const Rich::DAQ::Level1LogicalID, Rich::DAQ::Level1HardwareID > L1LogToHard;
  GaudiUtils::HashMap<const Rich::DetectorType,L1LogToHard> m_l1LogToHard;

  /// Logical to hardware L1 ID map
  typedef GaudiUtils::HashMap< const Rich::DAQ::Level1HardwareID, Rich::DAQ::Level1LogicalID > L1HardToLog;
  L1HardToLog m_l1HardToLog;

  /// First L1 Copy Number
  unsigned int m_firstL1CopyN;

  /// version number
  int m_version;

};

//=========================================================================
// activePDRichSmartIDs
//=========================================================================
inline const LHCb::RichSmartID::Vector& DeRichSystem::activePDRichSmartIDs() const
{
  return m_activePDSmartIDs;
}

//=========================================================================
// inactivePDRichSmartIDs
//=========================================================================
inline const LHCb::RichSmartID::Vector& DeRichSystem::inactivePDRichSmartIDs() const
{
  return m_inactivePDSmartIDs;
}

//=========================================================================
// allPDRichSmartIDs
//=========================================================================
inline const LHCb::RichSmartID::Vector& DeRichSystem::allPDRichSmartIDs() const
{
  return m_allPDSmartIDs;
}

//=========================================================================
// activePDHardwareIDs
//=========================================================================
inline const Rich::DAQ::HPDHardwareIDs& DeRichSystem::activePDHardwareIDs() const
{
  return m_activePDHardIDs;
}

//=========================================================================
// activePDHardwareIDs
//=========================================================================
inline const Rich::DAQ::HPDHardwareIDs& DeRichSystem::inactivePDHardwareIDs() const
{
  return m_inactivePDHardIDs;
}

//=========================================================================
// allPDHardwareIDs
//=========================================================================
inline const Rich::DAQ::HPDHardwareIDs& DeRichSystem::allPDHardwareIDs() const
{
  return m_allPDHardIDs;
}

//=========================================================================
// pdIsActive
//=========================================================================
inline bool DeRichSystem::pdIsActive( const LHCb::RichSmartID& id ) const
{
  // is this id in the inactive list
  return ( m_inactivePDSmartIDs.empty() ||
           std::find( m_inactivePDSmartIDs.begin(),
                      m_inactivePDSmartIDs.end(),
                      id.pdID() ) == m_inactivePDSmartIDs.end() );
}

//=========================================================================
// pdIsActive
//=========================================================================
inline bool DeRichSystem::pdIsActive( const Rich::DAQ::HPDHardwareID& id ) const
{
  // is this id in the inactive list
  return ( m_inactivePDHardIDs.empty() ||
           std::find( m_inactivePDHardIDs.begin(),
                      m_inactivePDHardIDs.end(),
                      id ) == m_inactivePDHardIDs.end() );
}

//=========================================================================
// l1PDSmartIDs
// Access mapping between Level 1 IDs and PD RichSmartIDs
//=========================================================================
inline const Rich::DAQ::L1ToSmartIDs& DeRichSystem::l1PDSmartIDs() const
{
  return m_l12smartids;
}

//=========================================================================
// l1PDHardIDs
// Access mapping between Level 1 IDs and PD RichSmartIDs
//=========================================================================
inline const Rich::DAQ::L1ToHardIDs& DeRichSystem::l1PDHardIDs() const
{
  return m_l12hardids;
}

//=========================================================================
// level1 hardware IDs
//=========================================================================
inline const Rich::DAQ::Level1HardwareIDs& DeRichSystem::level1HardwareIDs() const
{
  return m_l1IDs;
}

//=========================================================================
// Number HPDs
//=========================================================================
inline unsigned int DeRichSystem::nPDs() const
{
  return allPDRichSmartIDs().size();
}

#endif    // RICHDET_DERICHSYSTEM_H
