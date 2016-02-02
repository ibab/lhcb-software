
//-----------------------------------------------------------------------------
/** @file IRichRawDataFormatTool.h
 *
 *  Header file for tool interface : Rich::DAQ::IRawDataFormatTool
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

// RichKernel
#include "RichKernel/RichDecodedData.h"

// forward declarations
namespace LHCb
{
  class RawBank;
}

/// Static Interface Identification
static const InterfaceID IID_IRichRawDataFormatTool ( "Rich::DAQ::IRawDataFormatTool", 1, 0 );

namespace Rich
{
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

      /// Type for Input location(s) for RawEvent in TES
      using RawEventLocations = std::vector<std::string>;

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

      /** Decode all RICH RawBanks in the given events into the RICH decoded data structure
       *
       *  @param[in] taeLocations Vector of TAE locations to decode
       *  @param decodedData The RICH data structure to fill
       */
      virtual void decodeToSmartIDs( const RawEventLocations & taeLocations,
                                     Rich::DAQ::L1Map & decodedData ) const = 0;

      /** Decode all RICH RawBanks in the main event into the RICH decoded data structure
       *
       *  @param decodedData The RICH data structure to fill
       */
      inline void decodeToSmartIDs( Rich::DAQ::L1Map & decodedData ) const
      {
        static const RawEventLocations loc(1,"");
        return decodeToSmartIDs( loc, decodedData );
      }

    };

  }
}

#endif // RICHKERNEL_IRICHHPDDATABANKTOOL_H
