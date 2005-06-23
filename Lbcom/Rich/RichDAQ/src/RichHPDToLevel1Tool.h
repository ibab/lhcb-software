
//-----------------------------------------------------------------------------
/** @file RichHPDToLevel1Tool.h
 *
 *  Header file for tool : RichHPDToLevel1Tool
 *
 *  CVS Log :-
 *  $Id: RichHPDToLevel1Tool.h,v 1.6 2005-06-23 14:50:28 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 *
 *  @todo Interface to database (when available) to get the L1 numbering scheme
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHHPDTOLEVEL1TOOL_H
#define RICHDAQ_RICHHPDTOLEVEL1TOOL_H 1

// STD
#include <sstream>

// Boost
#include "boost/lexical_cast.hpp"

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolFactory.h"

// Base class
#include "RichKernel/RichToolBase.h"

// Kernel
#include "Kernel/RichSmartID.h"

// Interfaces
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichHPDToLevel1Tool.h"
#include "RichKernel/IRichHPDInfoTool.h"

//-----------------------------------------------------------------------------
/** @class RichHPDToLevel1Tool RichHPDToLevel1Tool.h
 *
 *  Tool to associate HPDs to Level1 boards
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 *
 *  @todo Finish implementation with true L1 numbering scheme from database
 */
//-----------------------------------------------------------------------------

class RichHPDToLevel1Tool : public RichToolBase,
                            virtual public IRichHPDToLevel1Tool {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichHPDToLevel1Tool( const std::string& type,
                       const std::string& name,
                       const IInterface* parent );

  /// Destructor
  virtual ~RichHPDToLevel1Tool( );

  // Initialization of the tool after creation
  StatusCode initialize();

  // Finalization of the tool before deletion
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from interface

  // Obtain the Level1 ID number for a given RichSmartID
  const RichDAQ::Level1ID levelL1ID( const RichSmartID smartID ) const;

  // Obtain the Level1 ID number for a given HPD hardware ID
  const RichDAQ::Level1ID levelL1ID( const RichDAQ::HPDHardwareID hardID ) const;

  // Obtain a list of RichSmartID HPD identifiers for a given level 1 ID
  const RichSmartID::Collection & l1HPDSmartIDs( const RichDAQ::Level1ID l1ID ) const;

  // Obtain a list of HPD hardware identifiers for a given level 1 ID
  const RichDAQ::HPDHardwareIDs & l1HPDHardIDs( const RichDAQ::Level1ID l1ID ) const;

  // Access mapping between Level 1 IDs and HPD RichSmartIDs
  const RichDAQ::L1ToSmartIDs & l1HPDSmartIDs() const;

  // Access mapping between Level 1 IDs and HPD RichSmartIDs
  const RichDAQ::L1ToHardIDs & l1HPDHardIDs() const;

  // Return which RICH detector a given Level 1 is used with
  const Rich::DetectorType richDetector( const RichDAQ::Level1ID l1ID ) const;

private: //methods

  /// Build a temporary mapping between HPD and L1 numbers
  void buildTempMapping();

private: // data

  // HPD information tool
  const IRichHPDInfoTool * m_hpdTool;

  /// L1 data from job options
  typedef std::vector< int > JOData;
  JOData m_joData;

  /// Typedef for mapping from RichSmartID to Level1 ID
  typedef RichHashMap< const RichSmartID, RichDAQ::Level1ID > SmartIDToL1;
  SmartIDToL1 m_smartid2L1; ///< HPD RichSmartID to L1 ID map

  /// Typedef for mapping from HPD Hardware ID to Level1 ID
  typedef RichHashMap< const RichDAQ::HPDHardwareID, RichDAQ::Level1ID > HardIDToL1;
  HardIDToL1 m_hardid2L1; ///< HPD Hardware ID to L1 ID map

  /// Typedef for mapping between L1 boards and RICH detector
  typedef RichHashMap< const RichDAQ::Level1ID, Rich::DetectorType > L1ToRICH;
  L1ToRICH m_l1ToRich; ///< L1 to RICH map

  RichDAQ::L1ToSmartIDs m_l12smartids; ///< L1 ID to RichSmartIDs map
  RichDAQ::L1ToHardIDs  m_l12hardids;  ///< L1 ID to HPD hardware IDs map

};

#endif // RICHDAQ_RICHHPDTOLEVEL1TOOL_H
