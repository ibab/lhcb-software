
//-----------------------------------------------------------------------------
/** @file IRichHPDInfoTool.h
 *
 *  Header file for tool interface : IRichHPDInfoTool
 *
 *  CVS Log :-
 *  $Id: IRichHPDInfoTool.h,v 1.1 2005-05-13 14:30:09 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_IRICHHPDIDTOOL_H
#define RICHKERNEL_IRICHHPDIDTOOL_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Rich DAQ
#include "RichKernel/RichDAQDefinitions.h"

/// Static Interface Identification
static const InterfaceID IID_IRichHPDInfoTool ( "IRichHPDInfoTool", 1, 0 );

//-----------------------------------------------------------------------------
/** @class IRichHPDInfoTool IRichHPDInfoTool.h RichKernel/IRichHPDInfoTool.h
 *
 *  Interface to tool to convert between software (RichSmartID) and hardware
 *  RICH HPD identifiers
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 */
//-----------------------------------------------------------------------------

class IRichHPDInfoTool : virtual public IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichHPDInfoTool; }

  /** Convert a RichSmartID into the corresponding hardware number
   *
   *  @param smartID RichSmartID identifier
   *
   *  @return Corresponding HPD hardware number
   */
  virtual const RichDAQ::HPDHardwareID hardwareID( const RichSmartID smartID ) const = 0;

  /** Convert a RICH HPD hardware number into the corresponding RichSmartID
   *
   *  @param hID The hardware ID number
   *
   *  @return smartID The RichSmartID software identifier
   */
  virtual const RichSmartID richSmartID( const RichDAQ::HPDHardwareID hID ) const = 0;

  /** Access a list of all active HPDs identified by their RichSmartID
   *
   *  @return read access to a vector of all HPD RichSmartIDs
   */
  virtual const RichSmartID::Collection & activeHPDRichSmartIDs() const = 0;

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
  virtual bool hpdIsActive( const RichSmartID id ) const = 0;

  /** Ask whether a given HPD is currently active or dead
   *
   *  @param id HPD hardware identifier
   *
   *  @return       Status of the HPD
   *  @retval true  HPD is active
   *  @retval false HPD is inactive
   */
  virtual bool hpdIsActive( const RichDAQ::HPDHardwareID id ) const = 0;

};

#endif // RICHKERNEL_IRICHHPDIDTOOL_H
