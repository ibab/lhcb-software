
//-----------------------------------------------------------------------------
/** @file IRichHPDIDTool.h
 *
 *  Header file for tool interface : IRichHPDIDTool
 *
 *  CVS Log :-
 *  $Id: IRichHPDIDTool.h,v 1.1 2005-01-07 12:41:57 jonrob Exp $
 *  $Log: not supported by cvs2svn $
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
static const InterfaceID IID_IRichHPDIDTool ( "IRichHPDIDTool", 1, 0 );

/** @class IRichHPDIDTool IRichHPDIDTool.h RichKernel/IRichHPDIDTool.h
 *
 *  Interface to tool to convert between software (RichSmartID) and hardware
 *  RICH HPD identifiers
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 */

class IRichHPDIDTool : virtual public IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichHPDIDTool; }

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

};

#endif // RICHKERNEL_IRICHHPDIDTOOL_H
