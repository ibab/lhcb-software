
//-----------------------------------------------------------------------------
/** @file RichHPDInfoTool.h
 *
 *  Header file for tool : RichHPDInfoTool
 *
 *  CVS Log :-
 *  $Id: RichHPDInfoTool.h,v 1.1 2005-05-13 14:22:12 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHHPDTOLEVEL1TOOL_H
#define RICHDAQ_RICHHPDTOLEVEL1TOOL_H 1

// STD
#include <sstream>
#include <string>

// from Gaudi
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/IRndmGenSvc.h"

// Boost
#include "boost/lexical_cast.hpp"

// Base class
#include "RichKernel/RichToolBase.h"

// Interfaces
#include "RichKernel/IRichHPDInfoTool.h"
#include "RichKernel/IRichSmartIDTool.h"

// Kernel
#include "Kernel/RichSmartID.h"

//-----------------------------------------------------------------------------
/** @class RichHPDInfoTool RichHPDInfoTool.h
 *
 *  Tool to convert between software (RichSmartID) and hardware
 *  RICH HPD identifiers
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 *
 *  @todo Finish implementation with true HPD hardware numbering scheme from database
 *  @todo Find a better way to get all valid HPD RichSmartIDs
 */
//-----------------------------------------------------------------------------

class RichHPDInfoTool : public RichToolBase,
                        virtual public IRichHPDInfoTool {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichHPDInfoTool( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

  /// Destructor
  virtual ~RichHPDInfoTool( );

  // Initialization of the tool after creation
  StatusCode initialize();

  // Finalization of the tool before deletion
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from interface

  // Convert a RichSmartID into the corresponding hardware number
  const RichDAQ::HPDHardwareID hardwareID( const RichSmartID smartID ) const;

  // Convert a RICH HPD hardware number into the corresponding RichSmartID
  const RichSmartID richSmartID( const RichDAQ::HPDHardwareID hID ) const;

  // Access a list of all active HPDs identified by their RichSmartID
  const RichSmartID::Collection & activeHPDRichSmartIDs() const;

  // Access a list of all active HPDs identified by their hardware IDs
  const RichDAQ::HPDHardwareIDs & activeHPDHardwareIDs() const;

  // Ask whether a given HPD is currently active or dead
  bool hpdIsActive( const RichSmartID id ) const;

  // Ask whether a given HPD is currently active or dead
  bool hpdIsActive( const RichDAQ::HPDHardwareID id ) const;

private: // methods

  /// Build HPD mappings
  StatusCode buildHPDMappings();

  /// Build dead and alive HPD map using a fraction of purely randomly choosen dead HPDs
  StatusCode buildRandomDeadHPDs();

  /// Build the fixed dead HPDs
  StatusCode buildFixedDeadHPDs();

private: // data

  /// Type for mapping from RichSmartID to RichDAQ::HPDHardwareID
  typedef RichHashMap< const RichSmartID, RichDAQ::HPDHardwareID > SoftToHard;
  SoftToHard m_soft2hard; ///< Software ID to hardware ID map

  /// Type for mapping from RichDAQ::HPDHardwareID to RichSmartID
  typedef RichHashMap< const RichDAQ::HPDHardwareID, RichSmartID > HardToSoft;
  HardToSoft m_hard2soft; ///< Hardware ID to software ID map

  /// List of all HPD RichSmartIDs
  RichSmartID::Collection m_smartIDs;

  /// List of all HPD hardware IDs
  RichDAQ::HPDHardwareIDs m_hardIDs;

  /// typedef for dead HPD map
  typedef RichHashMap<const RichSmartID,bool> HPDDeadMap;
  /// Map of dead and alive HPDs
  mutable HPDDeadMap m_hpdIsAlive;

  /// Flat random distribution between 0 and 1
  mutable Rndm::Numbers m_uniDist;

  /// Flag to indicate if HPD should be killed randomnly
  bool m_killRandomHPDs;

  /// Fraction of HPD to kill randomly
  double m_killFrac;

  /// List of fixed HPDs to kill
  std::vector< std::string > m_deadHpds;

  /// Random number initialisation
  unsigned int m_raninit;

};

#endif // RICHDAQ_RICHHPDTOLEVEL1TOOL_H
