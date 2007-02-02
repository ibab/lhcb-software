
//===============================================================================
/** @file RichDetailedFrontEndResponse.h
 *
 *  Header file for RICH digitisation algorithm : Rich::MC::Digi::DetailedFrontEndResponse
 *
 *  CVS Log :-
 *  $Id: RichDetailedFrontEndResponse.h,v 1.7 2007-02-02 10:13:42 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Alex Howard   a.s.howard@ic.ac.uk
 *  @date   2003-11-06
 */
//===============================================================================

#ifndef RICHREADOUT_RICHDETAILEDFRONTENDRESPONSE_H
#define RICHREADOUT_RICHDETAILEDFRONTENDRESPONSE_H 1

// STD
#include <sstream>

// base class
#include "RichKernel/RichAlgBase.h"

// from Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/RndmGenerators.h"

// Interfaces
#include "RichKernel/IRichSmartIDTool.h"

// Kernel
#include "Kernel/RichSmartID.h"
#include "Kernel/RichDetectorType.h"

// RichKernel
#include "RichKernel/RichMap.h"
#include "RichKernel/RichHashMap.h"

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

      /** @class DetailedFrontEndResponse RichDetailedFrontEndResponse.h
       *
       *  Performs a detailed simulation of the RICH HPD pixel response
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @author Alex Howard   a.s.howard@ic.ac.uk
       *  @date   2003-11-06
       */

      class DetailedFrontEndResponse : public RichAlgBase
      {

      public:

        /// Constructor
        DetailedFrontEndResponse( const std::string& name,
                                  ISvcLocator* pSvcLocator );

        /// Destructor
        virtual ~DetailedFrontEndResponse();

        virtual StatusCode initialize();
        virtual StatusCode execute();
        virtual StatusCode finalize();

        typedef std::pair< LHCb::MCRichSummedDeposit*, RichTimeSample > TimeData;
        typedef std::vector< TimeData > samplecache_t;

      private: // methods

        StatusCode Analog();  ///< Run the analogue simulation
        StatusCode Digital(); ///< Run the digital simulation

      private: // data

        const RichBase * actual_base;
        RichRegistry theRegistry;

        samplecache_t tscache;

        LHCb::MCRichSummedDeposits* m_summedDeposits;

        std::string m_mcRichSummedDepositsLocation;
        std::string m_mcRichDigitsLocation;

        int m_Pedestal;
        double m_Calibration;

        double m_Noise;
        double m_Threshold;
        double m_ThresholdSigma;

        double el_per_adc;

        Rndm::Numbers m_gaussThreshold;
        Rndm::Numbers m_gaussNoise;

      };

    }
  }
}

#endif // RICHREADOUT_RICHDETAILEDFRONTENDRESPONSE_H
