
#ifndef RICHREADOUT_RICHDETAILEDFRONTENDRESPONSEPMT_H
#define RICHREADOUT_RICHDETAILEDFRONTENDRESPONSEPMT_H 1
// Include files:
#include <sstream>
// from Kernel
#include "RichKernel/RichAlgBase.h"
#include "Kernel/RichSmartID.h"
#include "Kernel/RichDetectorType.h"
// from Rich Kernel
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/RichMap.h"
#include "RichKernel/RichHashMap.h"
#include "RichKernel/IRichSmartIDTool.h"
// from Gaudi
#include "GaudiKernel/RndmGenerators.h"
// from Event
#include "Event/MCRichHit.h"
#include "Event/MCRichDeposit.h"
#include "Event/MCRichSummedDeposit.h"
#include "Event/MCRichDigit.h"
// from RICHDet
#include "RichDet/DeRichPMT.h"
#include "RichDet/DeRich.h"
#include "RichDet/DeRichSystem.h"
// Local
#include "RichPixel.h"
#include "RichTimeSample.h"
#include "RichFrontEndDigitiser.h"
#include "RichRegistry.h"
#include "RichPixelReadout.h"
#include "RichBase.h"
#include "RichChannelPropertiesPMT.h"


/** @class DetailedFrontEndResponsePMT DetailedFrontEndResponsePMT.h
 *
 *  @author Marcin Kucharczyk, Mariusz Witek
 *  @date   2015-10-08
 */

using namespace LHCb;

namespace Rich
{
  namespace MC
  {
    namespace Digi
    {

      class DetailedFrontEndResponsePMT : public Rich::AlgBase {
        
      public:
        DetailedFrontEndResponsePMT(const std::string& name,
                                    ISvcLocator* pSvcLocator);
        virtual ~DetailedFrontEndResponsePMT();
        virtual StatusCode initialize();
        virtual StatusCode execute();
        virtual StatusCode finalize();
        
      private:
        void getClaroShape();
        StatusCode Analog();
        StatusCode Digital();
        int getPixelSize(LHCb::RichSmartID smartID);
        double getPixelGain(double mean, double sigma);
        std::vector<double> timeSampling(double NE);
        LHCb::RichSmartID setRichPmtSmartID(Rich::DetectorType rich,
                                            Rich::Side panel,
                                            unsigned int pdCol,
                                            unsigned int pdNumInCol,
                                            unsigned int pixelRow,
                                            unsigned int pixelCol);
        StatusCode addChannel(LHCb::RichSmartID smartID,
                              LHCb::MCRichSummedDeposit* summedDeposit,
                              double NE,
                              unsigned int pixelRow,
                              unsigned int pixelCol,
                              double chSharing,
                              double crossTalk);
        bool checkSpillover(std::vector<const LHCb::MCRichHit*> mcRichHits,
                            const std::string& hitLocation,
                            LHCb::RichSmartID RichSmartID,
                            double timeOffset,
                            double timeShift);
        

      private:
        std::string m_mcRichSumDepsLocation;
        std::string m_mcRichDigitsLocation;
        RichChannelPropertiesPMT* m_propertyTool;
        bool m_chargeSharing;
        bool m_crossTalk;
        bool m_spillover;
        bool m_electronicNoise;
        LHCb::MCRichSummedDeposits* m_sDeposits;
        Rndm::Numbers m_gaussGain;
        double m_meanGainSmallPix;
        double m_sigmaGainSmallPix;
        double m_meanGainLargePix;
        double m_sigmaGainLargePix;
        double m_horizontalChSharing;
        double m_verticalChSharing;
        double m_diagonalChSharing;
        double m_meanXTalkHorzSmall;
        double m_meanXTalkVertSmall;
        double m_meanXTalkDiagSmall;
        double m_meanXTalkHorzLarge;
        double m_meanXTalkVertLarge;
        double m_meanXTalkDiagLarge;
        double m_amplification;
        double m_FPGAwindow;
        double m_claroShapePar1;
        double m_claroShapePar2;
        unsigned int m_frameLength;
        std::vector<double> m_claroShape;
        Rndm::Numbers m_gaussTZero;
        double m_meanTZero;
        double m_sigmaTZero;
        double m_maxTZero;
        Rndm::Numbers m_gaussENoise;
        double m_meanENoise;
        double m_sigmaENoise;
        double m_thresholdSmall;
        double m_thresholdLarge;
        class PMTChannel {
        public:
          LHCb::MCRichSummedDeposit* summedDeposit;
          LHCb::RichSmartID richPMTSmartID;
          bool grandPixel;
          double NE;
          std::vector<double> timeShape;
          int isCSCT;
        };
        std::vector<PMTChannel> m_channels;
        std::vector<PMTChannel> m_channelsToCheck;
        std::vector<double> m_timeShift;
      }; 
    }
  }
}


#endif // RICHREADOUT_RICHDETAILEDFRONTENDRESPONSEPMT_H

