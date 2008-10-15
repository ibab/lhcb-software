
//-----------------------------------------------------------------------------
/** @file RawBufferToRichDigitsAlg.h
 *
 *  Header file for RICH DAQ algorithm : Rich::DAQ::RawBufferToRichDigitsAlg
 *
 *  CVS Log :-
 *  $Id: RawBufferToRichDigitsAlg.h,v 1.1.1.1 2008-10-15 12:34:57 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-11-06
 */
//-----------------------------------------------------------------------------

#ifndef RICHDAQ_RAWBUFFERTORICHDIGITSALG_H
#define RICHDAQ_RAWBUFFERTORICHDIGITSALG_H 1

// gaudi
#include "GaudiKernel/SmartDataPtr.h"

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
      virtual StatusCode finalize  ();    // Algorithm finalization

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
