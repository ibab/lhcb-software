
//-----------------------------------------------------------------------------
/** @file RichHPDToLevel1Tool.h
 *
 *  Header file for tool : RichHPDToLevel1Tool
 *
 *  CVS Log :-
 *  $Id: RichHPDToLevel1Tool.h,v 1.1 2005-01-07 12:35:59 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 *
 *  @todo Implement a proper L1 numbering scheme
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHHPDTOLEVEL1TOOL_H
#define RICHDAQ_RICHHPDTOLEVEL1TOOL_H 1

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
#include "RichKernel/IRichHPDIDTool.h"

/** @class RichHPDToLevel1Tool RichHPDToLevel1Tool.h
 *
 *  Tool to associate HPDs to Level1 boards
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 *
 *  @todo Finish implementation with true L1 numbering scheme
 */

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

private: // data

  /// Pointer to Rich HPD ID tool
  IRichHPDIDTool * m_hpdID;

  /// Typdef for mapping from RichSmartID to Level1 ID
  typedef std::map< const RichSmartID, RichDAQ::Level1ID > SmartIDToL1;
  SmartIDToL1 m_smartid2L1; ///< HPD RichSmartID to L1 ID map

  /// Typdef for mapping from HPD Hardware ID to Level1 ID
  typedef std::map< const RichDAQ::HPDHardwareID, RichDAQ::Level1ID > HardIDToL1;
  HardIDToL1 m_hardid2L1; ///< HPD Hardware ID to L1 ID map

  /// Typdef for mapping from Level1 ID to list of HPD RichSmartIDs
  typedef std::map< const RichDAQ::Level1ID, RichSmartID::Collection > L1ToSmartIDs;
  L1ToSmartIDs m_l12smartids; ///< L1 ID to RichSmartIDs map

  /// Typdef for mapping from Level1 ID to list of HPD RichSmartIDs
  typedef std::map< const RichDAQ::Level1ID, RichDAQ::HPDHardwareIDs > L1ToHardIDs;
  L1ToHardIDs m_l12hardids; ///< L1 ID to HPD hardware IDs map

};

#endif // RICHDAQ_RICHHPDTOLEVEL1TOOL_H
