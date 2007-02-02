
//===============================================================================
/** @file RichSimpleFrontEndResponse.h
 *
 *  Header file for RICH digitisation algorithm : Rich::MC::Digi::SimpleFrontEndResponse
 *
 *  CVS Log :-
 *  $Id: RichSimpleFrontEndResponse.h,v 1.6 2007-02-02 10:13:42 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Alex Howard   a.s.howard@ic.ac.uk
 *  @date   2003-11-06
 */
//===============================================================================

#ifndef RICHREADOUT_RICHSIMPLEFRONTENDRESPONSE_H
#define RICHREADOUT_RICHSIMPLEFRONTENDRESPONSE_H 1

// base class
#include "RichKernel/RichAlgBase.h"

// from Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/RndmGenerators.h"

// interfaces
#include "RichKernel/IRichSmartIDTool.h"

// Kernel
#include "Kernel/RichSmartID.h"
#include "Kernel/RichDetectorType.h"

// local
#include "RichPixel.h"
#include "RichTimeSample.h"
#include "RichFrontEndDigitiser.h"
#include "RichRegistry.h"
#include "RichPixelReadout.h"
#include "RichShape.h"
#include "RichBase.h"

// event model
#include "Event/MCRichHit.h"
#include "Event/MCRichDeposit.h"
#include "Event/MCRichSummedDeposit.h"
#include "Event/MCRichDigit.h"

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
  /** @namespace MC
   *
   *  General namespace for RICH MC related software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   05/12/2006
   */
  //-----------------------------------------------------------------------------
  namespace MC
  {

    //-----------------------------------------------------------------------------
    /** @namespace Digi
     *
     *  General namespace for RICH Digitisation simuation related software
     *
     *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
     *  @date   17/01/2007
     */
    //-----------------------------------------------------------------------------
    namespace Digi
    {

      /** @class SimpleFrontEndResponse RichSimpleFrontEndResponse.h
       *
       *  Performs a simple simulation of the RICH HPD pixel response
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @author Alex Howard   a.s.howard@ic.ac.uk
       *  @date   2003-11-06
       */

      class SimpleFrontEndResponse : public RichAlgBase
      {

      public:

        /// Constructor
        SimpleFrontEndResponse( const std::string& name,
                                ISvcLocator* pSvcLocator );

        /// Destructor
        virtual ~SimpleFrontEndResponse();

        virtual StatusCode initialize();
        virtual StatusCode finalize();
        virtual StatusCode execute();

      private: // methods

        /// Run the Simple treatment
        StatusCode Simple();

      private: // data

        const RichBase* actual_base;
        RichRegistry theRegistry;

        LHCb::MCRichSummedDeposits* SummedDeposits;

        std::string m_mcRichSummedDepositsLocation;
        std::string m_mcRichDigitsLocation;

        int m_Baseline;
        int m_AdcCut;

        double m_Sigma;
        double m_Calibration;
        double m_Threshold;

        Rndm::Numbers m_gaussRndm;

      };

    }
  }
}

#endif // RICHREADOUT_RICHSIMPLEFRONTENDRESPONSE_H
