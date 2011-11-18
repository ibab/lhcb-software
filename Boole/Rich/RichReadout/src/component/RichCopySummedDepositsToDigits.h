
//===============================================================================
/** @file RichCopySummedDepositsToDigits.h
 *
 *  Header file for RICH digitisation algorithm : Rich::MC::Digi::CopySummedDepositsToDigits
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   18/11/2011
 */
//===============================================================================

#ifndef RICHREADOUT_RichCopySummedDepositsToDigits_H
#define RICHREADOUT_RichCopySummedDepositsToDigits_H 1

// base class
#include "RichKernel/RichAlgBase.h"

// Kernel
#include "Kernel/RichSmartID.h"
#include "Kernel/RichDetectorType.h"

// event model
#include "Event/MCRichHit.h"
#include "Event/MCRichDeposit.h"
#include "Event/MCRichSummedDeposit.h"
#include "Event/MCRichDigit.h"

namespace Rich
{
  namespace MC
  {
    namespace Digi
    {

      /** @class CopySummedDepositsToDigits RichCopySummedDepositsToDigits.h
       *
       *  Performs a simple simulation of the RICH HPD pixel response
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   18/11/2011
       */

      class CopySummedDepositsToDigits : public Rich::AlgBase
      {

      public:

        /// Constructor
        CopySummedDepositsToDigits( const std::string& name,
                                ISvcLocator* pSvcLocator );

        /// Destructor
        virtual ~CopySummedDepositsToDigits();

        /// execute
        virtual StatusCode execute();

      private: // data

        std::string m_mcRichSummedDepositsLocation;
        std::string m_mcRichDigitsLocation;

      };

    }
  }
}

#endif // RICHREADOUT_RichCopySummedDepositsToDigits_H
