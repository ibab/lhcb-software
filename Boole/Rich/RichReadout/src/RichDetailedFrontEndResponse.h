
#ifndef RICHREADOUT_RICHDETAILEDFRONTENDRESPONSE_H
#define RICHREADOUT_RICHDETAILEDFRONTENDRESPONSE_H 1

// base class
#include "RichUtils/RichAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/RndmGenerators.h"

// Detector geom
#include "RiSicbGeom/SicbGeom.h"
#include "RiSicbGeom/PixelFinder.h"
#include "RichDetTools/IRichDetInterface.h"

// Kernel
#include "Kernel/RichSmartID.h"
#include "RichKernel/RichDetectorType.h"

// local
#include "RichPixel.h"
#include "RichTimeSample.h"
#include "RichFrontEndDigitiser.h"
#include "RichRegistry.h"
#include "RichPixelReadout.h"
#include "RichShape.h"
#include "RichNoisifier.h"
#include "RichBase.h"

// event model
#include "Event/MCRichHit.h"
#include "Event/MCRichDeposit.h"
#include "Event/MCRichSummedDeposit.h"
#include "Event/MCRichDigit.h"

// CLHEP
#include "CLHEP/Random/Random.h"

class RichDetailedFrontEndResponse : public RichAlgBase {

public:

  RichDetailedFrontEndResponse( const std::string& name, 
                                ISvcLocator* pSvcLocator );

  virtual ~RichDetailedFrontEndResponse();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();

  typedef std::map< MCRichSummedDeposit*, RichTimeSample > samplecache_t;

private: // methods

  StatusCode Analog();
  StatusCode Digital();

private: // data

  const RichBase* actual_base;
  RichRegistry theRegistry;

  samplecache_t tscache;

  MCRichSummedDeposits* SummedDeposits;

  std::string m_mcRichSummedDepositsLocation;
  std::string m_mcRichDigitsLocation;
  std::string m_detMode;

  std::string m_histPth;

  int m_Pedestal;
  double m_Calibration;

  double m_Noise;
  double m_Threshold;
  double m_ThresholdSigma;

  double el_per_adc;

  Rndm::Numbers m_gaussThreshold;
  Rndm::Numbers m_gaussNoise;

};

#endif // RICHREADOUT_RICHDETAILEDFRONTENDRESPONSE_H
