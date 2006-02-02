
//-----------------------------------------------------------------------------
/** @file RichDetNumberingTool.h
 *
 *  Header file for tool : RichDetNumberingTool
 *
 *  CVS Log :-
 *  $Id: RichDetNumberingTool.h,v 1.5 2006-02-02 17:23:41 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RichDetNumberingTool_H
#define RICHDAQ_RichDetNumberingTool_H 1

// STD
#include <sstream>
#include <string>

// from Gaudi
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// Base class
#include "RichKernel/RichToolBase.h"

// Interfaces
#include "RichKernel/IRichDetNumberingTool.h"

// Kernel
#include "Kernel/RichSmartID.h"
#include "RichKernel/RichHashMap.h"

// Det Desc
#include "DetDesc/Condition.h"

// namespaces
using namespace LHCb; ///< LHCb general namespace

//-----------------------------------------------------------------------------
/** @class RichDetNumberingTool RichDetNumberingTool.h
 *
 *  Tool to provide conversions and mappings between the various
 *  RICH numbering schemes and to provide data on which HPDs, Level0 and Level1
 *  boards are currently active.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   10/11/2005
 */
//-----------------------------------------------------------------------------

class RichDetNumberingTool : public RichToolBase,
                             virtual public IRichDetNumberingTool
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichDetNumberingTool( const std::string& type,
                        const std::string& name,
                        const IInterface* parent );

  /// Destructor
  virtual ~RichDetNumberingTool( );

  // Initialization of the tool after creation
  StatusCode initialize();

  // Finalization of the tool before deletion
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from interface

  // Convert a RichSmartID into the corresponding hardware number
  const RichDAQ::HPDHardwareID hardwareID( const RichSmartID smartID ) const;

  // Convert a RICH HPD hardware number into the corresponding RichSmartID
  const RichSmartID richSmartID( const RichDAQ::HPDHardwareID hID ) const;

  // Convert a RICH Level0 hardware number into the corresponding RichSmartID
  const RichSmartID richSmartID( const RichDAQ::Level0ID hID ) const;

  // Access a list of all active HPDs identified by their RichSmartID
  const RichSmartID::Vector & activeHPDRichSmartIDs() const;

  // Access a list of all active HPDs identified by their hardware IDs
  const RichDAQ::HPDHardwareIDs & activeHPDHardwareIDs() const;

  // Ask whether a given HPD is currently active or dead
  bool hpdIsActive( const RichSmartID id ) const;

  // Ask whether a given HPD is currently active or dead
  bool hpdIsActive( const RichDAQ::HPDHardwareID id ) const;

  // Obtain the Level0 ID number for a given RichSmartID
  const RichDAQ::Level0ID level0ID( const RichSmartID smartID ) const;

  // Obtain the Level0 ID number for a given HPD hardware ID
  const RichDAQ::Level0ID level0ID( const RichDAQ::HPDHardwareID hardID ) const;

  // Obtain the Level1 ID number for a given RichSmartID
  const RichDAQ::Level1ID level1ID( const RichSmartID smartID ) const;

  // Obtain the Level1 ID number for a given HPD hardware ID
  const RichDAQ::Level1ID level1ID( const RichDAQ::HPDHardwareID hardID ) const;

  // Obtain the Level1 input number for a given RichSmartID
  const RichDAQ::Level1Input level1InputNum( const LHCb::RichSmartID smartID ) const;

  /// Obtain the Level1 input number for a given HPD hardware ID
  const RichDAQ::Level1Input level1InputNum( const RichDAQ::HPDHardwareID hardID ) const;

  // Obtain a list of RichSmartID HPD identifiers for a given level 1 ID
  const RichSmartID::Vector & l1HPDSmartIDs( const RichDAQ::Level1ID l1ID ) const;

  // Obtain a list of HPD hardware identifiers for a given level 1 ID
  const RichDAQ::HPDHardwareIDs & l1HPDHardIDs( const RichDAQ::Level1ID l1ID ) const;

  // Access mapping between Level 1 IDs and HPD RichSmartIDs
  const RichDAQ::L1ToSmartIDs & l1HPDSmartIDs() const;

  // Access mapping between Level 1 IDs and HPD RichSmartIDs
  const RichDAQ::L1ToHardIDs & l1HPDHardIDs() const;

  // Return which RICH detector a given Level 1 is used with
  const Rich::DetectorType richDetector( const RichDAQ::Level1ID l1ID ) const;

  // Returns a list of all valid Level1 ids
  const RichDAQ::Level1IDs & level1IDs() const;

private: // methods

  /// Update methods for HPD mappings
  StatusCode buildHPDMappings();

  /// Fill the maps for the given RICH detector
  StatusCode fillMaps( const Rich::DetectorType rich );

private: // data

  /// Type for mapping from RichSmartID to RichDAQ::HPDHardwareID
  typedef Rich::HashMap< const RichSmartID, RichDAQ::HPDHardwareID > SoftToHard;
  SoftToHard m_soft2hard; ///< Software ID to hardware ID map

  /// Type for mapping from RichDAQ::HPDHardwareID to RichSmartID
  typedef Rich::HashMap< const RichDAQ::HPDHardwareID, RichSmartID > HardToSoft;
  HardToSoft m_hard2soft; ///< HPD Hardware ID to software ID map

  /// Type for mapping from RichDAQ::HPDHardwareID to RichSmartID
  typedef Rich::HashMap< const RichDAQ::Level0ID, RichSmartID > L0HardToSoft;
  L0HardToSoft m_l0hard2soft; ///< Level0 Hardware ID to software ID map

  /// List of all active HPD RichSmartIDs
  RichSmartID::Vector m_smartIDs;

  /// List of all active HPD hardware IDs
  RichDAQ::HPDHardwareIDs m_hardIDs;

  /// Typedef for mapping from RichSmartID to Level0 ID
  typedef Rich::HashMap< const RichSmartID, RichDAQ::Level0ID > SmartIDToL0;
  SmartIDToL0 m_smartid2L0; ///< HPD RichSmartID to L0 ID map

  /// Typedef for mapping from HPD Hardware ID to Level0 ID
  typedef Rich::HashMap< const RichDAQ::HPDHardwareID, RichDAQ::Level0ID > HardIDToL0;
  HardIDToL0 m_hardid2L0; ///< HPD Hardware ID to L0 ID map

  /// Typedef for mapping from RichSmartID to Level1 ID
  typedef Rich::HashMap< const RichSmartID, RichDAQ::Level1ID > SmartIDToL1;
  SmartIDToL1 m_smartid2L1; ///< HPD RichSmartID to L1 ID map

  /// Typedef for mapping from HPD Hardware ID to Level1 ID
  typedef Rich::HashMap< const RichDAQ::HPDHardwareID, RichDAQ::Level1ID > HardIDToL1;
  HardIDToL1 m_hardid2L1; ///< HPD Hardware ID to L1 ID map

  /// Typedef for mapping from RichSmartID to Level1 input number
  typedef Rich::HashMap< const RichSmartID, RichDAQ::Level1Input > SmartIDToL1In;
  SmartIDToL1In m_smartid2L1In; ///< HPD RichSmartID to L1 input number map

  /// Typedef for mapping from HPD Hardware ID to Level1 input number
  typedef Rich::HashMap< const RichDAQ::HPDHardwareID, RichDAQ::Level1Input > HardIDToL1In;
  HardIDToL1In m_hardid2L1In; ///< HPD Hardware ID to L1 input number map

  /// Typedef for mapping between L1 boards and RICH detector
  typedef Rich::HashMap< const RichDAQ::Level1ID, Rich::DetectorType > L1ToRICH;
  L1ToRICH m_l1ToRich; ///< L1 to RICH map

  RichDAQ::L1ToSmartIDs m_l12smartids; ///< L1 ID to RichSmartIDs map
  RichDAQ::L1ToHardIDs  m_l12hardids;  ///< L1 ID to HPD hardware IDs map

  /// List of all valid Level1 IDs
  RichDAQ::Level1IDs m_l1IDs;

  /// Location of RICH Numbering schemes in Conditions DB
  std::vector<std::string> m_condBDLocs;

};

#endif // RICHDAQ_RichDetNumberingTool_H
