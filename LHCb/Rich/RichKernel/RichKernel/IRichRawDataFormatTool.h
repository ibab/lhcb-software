
//-----------------------------------------------------------------------------
/** @file IRichRawDataFormatTool.h
 *
 *  Header file for tool interface : IRichRawDataFormatTool
 *
 *  CVS Log :-
 *  $Id: IRichRawDataFormatTool.h,v 1.3 2005-12-13 17:27:25 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_IRICHHPDDATABANKTOOL_H
#define RICHKERNEL_IRICHHPDDATABANKTOOL_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Kernel
#include "Kernel/RichSmartID.h"
#include "RichKernel/RichDAQDefinitions.h"

// DAQ
class RichHPDDataBank;

/// Static Interface Identification
static const InterfaceID IID_IRichRawDataFormatTool ( "IRichRawDataFormatTool", 1, 0 );

/** @class IRichRawDataFormatTool IRichRawDataFormatTool.h RichKernel/IRichRawDataFormatTool.h
 *
 *  Interface to tool to encode and decode the Raw Buffer for the RICH
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2004-12-18
 */

class IRichRawDataFormatTool : virtual public IAlgTool 
{

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichRawDataFormatTool; }

  /** Creates a bank data of a given version from the given RichSmartID vector
   *
   *  NOTE : Ownership of the data object passes to the caller.
   *         It is their responsibility to delete when no longer needed.
   *
   *  @param smartIDs Vector of RichSmartIDs to use to create the data bank
   *  @param version   The RICH DAQ data bank version
   */
  virtual const RichHPDDataBank * createDataBank( const LHCb::RichSmartID::Vector & smartIDs,
                                                  const RichDAQ::BankVersion version = RichDAQ::LHCb0 ) const = 0;

  /** Creates a bank data from the given raw block of data
   *
   *  NOTE : Ownership of the data object passes to the caller.
   *         It is their responsibility to delete when no longer needed.
   *
   *  @param dataStart Pointer to the start of the raw data
   *  @param dataSize  The length of the data block (excluding header HPD word)
   *  @param version   The RICH DAQ data bank version
   */
  virtual const RichHPDDataBank * createDataBank( const RichDAQ::LongType * dataStart,
                                                  const unsigned int dataSize,
                                                  const RichDAQ::BankVersion version = RichDAQ::LHCb0 ) const = 0;

  /** Creates a bank data from RichSmartIDs, sorted by Level 1 ID
   *
   *  NOTE : Ownership of the data object passes to the caller.
   *         It is their responsibility to delete when no longer needed.
   *
   *  @param L1Data  Vector of level 1 sorted RichSmartIDs
   *  @param version The RICH DAQ data bank version
   */
  virtual void createDataBank( const RichDAQ::L1Map & L1Data,
                               const RichDAQ::BankVersion version = RichDAQ::LHCb0 ) const = 0;

  /** Decode a single RawBank into RichSmartID identifiers
   *
   *  @param bank     Pointer to RawBank to decode
   *  @param smartIDs Vector of RichSmartIDs to fill
   */
  virtual void decodeToSmartIDs( const LHCb::RawBank & bank,
                                 LHCb::RichSmartID::Vector & smartIDs ) const = 0;

  /** Decode all RICH RawBanks into RichSmartID identifiers
   *
   *  @param smartIDs Vector of RichSmartIDs to fill
   */
  virtual void decodeToSmartIDs( LHCb::RichSmartID::Vector & smartIDs ) const = 0;

};

#endif // RICHKERNEL_IRICHHPDDATABANKTOOL_H
