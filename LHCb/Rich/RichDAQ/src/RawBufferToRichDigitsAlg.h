//-----------------------------------------------------------------------------
/** @file RawBufferToRichDigitsAlg.h
 *
 *  Header file for RICH DAQ algorithm : Rich::DAQ::RawBufferToRichDigitsAlg
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RAWBUFFERTORICHDIGITSALG_H
#define RICHDAQ_RAWBUFFERTORICHDIGITSALG_H 1

// base class
#include "RichKernel/RichAlgBase.h"

// Event
#include "Event/RichDigit.h"

// Interfaces
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"

namespace Rich
{
  namespace DAQ
  {

    //-----------------------------------------------------------------------------
    /** @class RawBufferToRichDigitsAlg RawBufferToRichDigitsAlg.h
     *
     *  Algorithm to create RichDigits from RawEvent object
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   2003-11-06
     */
    //-----------------------------------------------------------------------------

    class RawBufferToRichDigitsAlg : public AlgBase
    {

    public:

      /// Standard constructor
      RawBufferToRichDigitsAlg( const std::string& name, ISvcLocator* pSvcLocator );

      virtual ~RawBufferToRichDigitsAlg( ); ///< Destructor

      virtual StatusCode initialize();    // Algorithm initialization
      virtual StatusCode execute   ();    // Algorithm execution

    private: // data

      /// Output location for RichDigits
      std::string m_richDigitsLoc;

      /// Raw Buffer Decoding tool
      const IRawBufferToSmartIDsTool * m_decoder;

      /// Flag to turn off RichDigit creation
      bool m_decodeOnly;

    };

  }
}

#endif // RICHDAQ_RAWBUFFERTORICHDIGITSALG_H
