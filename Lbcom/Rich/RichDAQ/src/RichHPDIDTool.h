
//-----------------------------------------------------------------------------
/** @file RichHPDIDTool.h
 *
 *  Header file for tool : RichHPDIDTool
 *
 *  CVS Log :-
 *  $Id: RichHPDIDTool.h,v 1.1 2005-01-07 12:35:59 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RICHHPDTOLEVEL1TOOL_H
#define RICHDAQ_RICHHPDTOLEVEL1TOOL_H 1

// Boost
#include "boost/lexical_cast.hpp"

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// Base class
#include "RichKernel/RichToolBase.h"

// Interfaces
#include "RichKernel/IRichHPDIDTool.h"
#include "RichKernel/IRichSmartIDTool.h"

// Rich
#include "Kernel/RichSmartID.h"

/** @class RichHPDIDTool RichHPDIDTool.h
 *
 *  Tool to convert between software (RichSmartID) and hardware
 *  RICH HPD identifiers
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 *
 *  @todo Finish implementation with true hardware numbering scheme from database
 *  @todo Find a better way to get all valid HPD RichSmartIDs
 */

class RichHPDIDTool : public RichToolBase,
                      virtual public IRichHPDIDTool {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichHPDIDTool( const std::string& type,
                 const std::string& name,
                 const IInterface* parent );

  /// Destructor
  virtual ~RichHPDIDTool( );

  // Initialization of the tool after creation
  StatusCode initialize();

  // Finalization of the tool before deletion
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from interface

  // Convert a RichSmartID into the corresponding hardware number
  const RichDAQ::HPDHardwareID hardwareID( const RichSmartID smartID ) const;

  // Convert a RICH HPD hardware number into the corresponding RichSmartID
  const RichSmartID richSmartID( const RichDAQ::HPDHardwareID hID ) const;

private: // data

  /// Type for mapping from RichSmartID to RichDAQ::HPDHardwareID
  typedef std::map< const RichSmartID, RichDAQ::HPDHardwareID > SoftToHard;
  SoftToHard m_soft2hard; ///< Software ID to hardware ID map

  /// Type for mapping from RichDAQ::HPDHardwareID to RichSmartID
  typedef std::map< const RichDAQ::HPDHardwareID, RichSmartID > HardToSoft;
  HardToSoft m_hard2soft; ///< Hardware ID to software ID map

};

#endif // RICHDAQ_RICHHPDTOLEVEL1TOOL_H
