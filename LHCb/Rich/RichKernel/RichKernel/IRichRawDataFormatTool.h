
//-----------------------------------------------------------------------------
/** @file IRichRawDataFormatTool.h
 *
 *  Header file for tool interface : Rich::DAQ::IRawDataFormatTool
 *
 *  CVS Log :-
 *  $Id: IRichRawDataFormatTool.h,v 1.9 2007-02-01 17:24:54 jonrob Exp $
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

// numberings
#include "RichDet/RichDAQDefinitions.h"

// forward declarations
namespace LHCb
{
  class RawBank;
}

/// Static Interface Identification
static const InterfaceID IID_IRichRawDataFormatTool ( "Rich::DAQ::IRawDataFormatTool", 1, 0 );

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @namespace DAQ
   *
   *  namespace for RICH DAQ software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
  namespace DAQ
  {

    /** @class IRawDataFormatTool IRichRawDataFormatTool.h RichKernel/IRichRawDataFormatTool.h
     *
     *  Interface to tool to encode and decode the Raw Buffer for the RICH
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   2004-12-18
     */

    class IRawDataFormatTool : virtual public IAlgTool
    {

    public:

      /** static interface identification
       *  @return unique interface identifier
       */
      static const InterfaceID& interfaceID() { return IID_IRichRawDataFormatTool; }

      /** Fills the RICH RawEvent from a vector of RichSmartIDs
       *
       *  @param smartIDs  Vector of RichSmartIDs
       *  @param version The RICH DAQ data bank version
       */
      virtual void fillRawEvent( const LHCb::RichSmartID::Vector & smartIDs,
                                 const Rich::DAQ::BankVersion version = Rich::DAQ::LHCb2 ) const = 0;

      /** Decode a single RawBank into RichSmartID identifiers
       *
       *  @param bank     Pointer to RawBank to decode
       *  @param smartIDs Vector of RichSmartIDs to fill
       */
      virtual void decodeToSmartIDs( const LHCb::RawBank & bank,
                                     Rich::DAQ::PDMap & smartIDs ) const = 0;

      /** Decode all RICH RawBanks into RichSmartID identifiers
       *
       *  @param smartIDs Vector of RichSmartIDs to fill
       */
      virtual void decodeToSmartIDs( Rich::DAQ::PDMap & smartIDs ) const = 0;

    };

  }
}

#endif // RICHKERNEL_IRICHHPDDATABANKTOOL_H
