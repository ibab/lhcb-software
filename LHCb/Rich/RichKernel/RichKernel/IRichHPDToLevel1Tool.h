
//-----------------------------------------------------------------------------
/** @file IRichHPDToLevel1Tool.h
 *
 *  Header file for tool interface : IRichHPDToLevel1Tool
 *
 *  CVS Log :-
 *  $Id: IRichHPDToLevel1Tool.h,v 1.3 2005-01-18 08:59:20 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.2  2005/01/13 12:15:18  jonrob
 *  Add methods to access the various maps
 *
 *  Revision 1.1  2005/01/07 12:41:57  jonrob
 *  Add new interfaces
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_IRICHHPDTOLEVEL1TOOL_H
#define RICHKERNEL_IRICHHPDTOLEVEL1TOOL_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Kernel
#include "RichKernel/RichDAQDefinitions.h"

/// Static Interface Identification
static const InterfaceID IID_IRichHPDToLevel1Tool ( "IRichHPDToLevel1Tool", 1, 0 );

/** @class IRichHPDToLevel1Tool IRichHPDToLevel1Tool.h RichKernel/IRichHPDToLevel1Tool.h
 *
 *  Interface to tool to associate HPDs to L1 boards
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 */

class IRichHPDToLevel1Tool : virtual public IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichHPDToLevel1Tool; }

  /** Obtain the Level1 ID number for a given RichSmartID
   *
   *  @param smartID The RichSmartID identifier
   *
   *  @return The Level1 board ID
   */
  virtual const RichDAQ::Level1ID levelL1ID( const RichSmartID smartID ) const = 0;

  /** Obtain the Level1 ID number for a given HPD hardware ID
   *
   *  @param hardID The HPD hardware ID
   *
   *  @return The Level1 board ID
   */
  virtual const RichDAQ::Level1ID levelL1ID( const RichDAQ::HPDHardwareID hardID ) const = 0;

  /** Obtain a list of RichSmartID HPD identifiers for a given level 1 ID
   *
   *  @param l1ID The Level1 board ID
   *
   *  @return RichSmartID HPD identifiers
   */
  virtual const RichSmartID::Collection & l1HPDSmartIDs( const RichDAQ::Level1ID l1ID ) const = 0;

  /** Obtain a list of HPD hardware identifiers for a given level 1 ID
   *
   *  @param l1ID The Level1 board ID
   *
   *  @return HPD hardware identifiers
   */
  virtual const RichDAQ::HPDHardwareIDs & l1HPDHardIDs( const RichDAQ::Level1ID l1ID ) const = 0;

  /** Access mapping between Level 1 IDs and HPD RichSmartIDs
   *
   *  @return Mapping between L1 and RichSmartIDs
   */
  virtual const RichDAQ::L1ToSmartIDs & l1HPDSmartIDs() const = 0;

  /** Access mapping between Level 1 IDs and HPD RichSmartIDs
   *
   *  @return Mapping between L1 and RichSmartIDs
   */
  virtual const RichDAQ::L1ToHardIDs & l1HPDHardIDs() const = 0;

  /** Return which RICH detector a given Level 1 is used with
   *
   *  @return Rich identifier
   */
  virtual const Rich::DetectorType richDetector( const RichDAQ::Level1ID l1ID ) const = 0;

};

#endif // RICHKERNEL_IRICHHPDTOLEVEL1TOOL_H
