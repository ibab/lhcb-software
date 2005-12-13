
//-----------------------------------------------------------------------------
/** @file IRichDetNumberingTool.h
 *
 *  Header file for tool interface : IRichDetNumberingTool
 *
 *  CVS Log :-
 *  $Id: IRichDetNumberingTool.h,v 1.2 2005-12-13 17:27:25 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_IRichDetNumberingTool_H
#define RICHKERNEL_IRichDetNumberingTool_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Rich DAQ
#include "RichKernel/RichDAQDefinitions.h"

/// Static Interface Identification
static const InterfaceID IID_IRichDetNumberingTool ( "IRichDetNumberingTool", 1, 0 );

//-----------------------------------------------------------------------------
/** @class IRichDetNumberingTool IRichDetNumberingTool.h RichKernel/IRichDetNumberingTool.h
 *
 *  Interface to tool to provide conversions and mappings between the various
 *  RICH numbering schemes and to provide data on which HPDs, Level0 and Level1
 *  boards are currently active.
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   10/11/2005
 */
//-----------------------------------------------------------------------------

class IRichDetNumberingTool : virtual public IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichDetNumberingTool; }

  /** Convert a RichSmartID into the corresponding hardware number
   *
   *  @param smartID RichSmartID identifier
   *
   *  @return Corresponding HPD hardware number
   */
  virtual const RichDAQ::HPDHardwareID hardwareID( const LHCb::RichSmartID smartID ) const = 0;

  /** Convert a RICH HPD hardware number into the corresponding RichSmartID
   *
   *  @param hID The HPD hardware ID number
   *
   *  @return smartID The RichSmartID software identifier
   */
  virtual const LHCb::RichSmartID richSmartID( const RichDAQ::HPDHardwareID hID ) const = 0;

  /** Convert a RICH Level0 hardware number into the corresponding RichSmartID
   *
   *  @param hID The Level0 hardware ID number
   *
   *  @return smartID The RichSmartID software identifier
   */
  virtual const LHCb::RichSmartID richSmartID( const RichDAQ::Level0ID hID ) const = 0;

  /** Access a list of all active HPDs identified by their RichSmartID
   *
   *  @return read access to a vector of all HPD RichSmartIDs
   */
  virtual const LHCb::RichSmartID::Vector & activeHPDRichSmartIDs() const = 0;

  /** Access a list of all active HPDs identified by their hardware IDs
   *
   *  @return read access to a vector of all HPD hardware IDs
   */
  virtual const RichDAQ::HPDHardwareIDs & activeHPDHardwareIDs() const = 0;

  /** Ask whether a given HPD is currently active or dead
   *
   *  @param id RichSmartID for the HPD, or the pixel in an HPD
   *
   *  @return       Status of the HPD
   *  @retval true  HPD is active
   *  @retval false HPD is inactive
   */
  virtual bool hpdIsActive( const LHCb::RichSmartID id ) const = 0;

  /** Ask whether a given HPD is currently active or dead
   *
   *  @param id HPD hardware identifier
   *
   *  @return       Status of the HPD
   *  @retval true  HPD is active
   *  @retval false HPD is inactive
   */
  virtual bool hpdIsActive( const RichDAQ::HPDHardwareID id ) const = 0;

  /** Obtain the Level0 ID number for a given RichSmartID
   *
   *  @param smartID The RichSmartID identifier
   *
   *  @return The Level0 board ID
   */
  virtual const RichDAQ::Level0ID level0ID( const LHCb::RichSmartID smartID ) const = 0;

  /** Obtain the Level0 ID number for a given HPD hardware ID
   *
   *  @param hardID The HPD hardware ID
   *
   *  @return The Level0 board ID
   */
  virtual const RichDAQ::Level0ID level0ID( const RichDAQ::HPDHardwareID hardID ) const = 0;

  /** Obtain the Level1 ID number for a given RichSmartID
   *
   *  @param smartID The RichSmartID identifier
   *
   *  @return The Level1 board ID
   */
  virtual const RichDAQ::Level1ID level1ID( const LHCb::RichSmartID smartID ) const = 0;

  /** Obtain the Level1 ID number for a given HPD hardware ID
   *
   *  @param hardID The HPD hardware ID
   *
   *  @return The Level1 board ID
   */
  virtual const RichDAQ::Level1ID level1ID( const RichDAQ::HPDHardwareID hardID ) const = 0;

  /** Obtain a list of RichSmartID HPD identifiers for a given level 1 ID
   *
   *  @param l1ID The Level1 board ID
   *
   *  @return RichSmartID HPD identifiers
   */
  virtual const LHCb::RichSmartID::Vector & l1HPDSmartIDs( const RichDAQ::Level1ID l1ID ) const = 0;

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

  /** Returns a list of all valid Level1 ids
   *
   *  @return List of all valid Level1 IDs
   */
  virtual const RichDAQ::Level1IDs & level1IDs() const = 0;

};

#endif // RICHKERNEL_IRichDetNumberingTool_H
