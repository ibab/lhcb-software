
#ifndef RICHREADOUT_RICHSIMPLEHPDFRONTENDRESPONSE_H
#define RICHREADOUT_RICHSIMPLEHPDFRONTENDRESPONSE_H 1

// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/RndmGenerators.h"

// Detector geom
#include "RiSicbGeom/SicbGeom.h"
#include "RiSicbGeom/PixelFinder.h"
#include "RichDetTools/IRichDetInterface.h"

// Rich Kernel
#include "RichKernel/RichSmartID.h"
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

// CLHep
#include "CLHEP/Random/Random.h"

class RichSimpleHPDFrontEndResponse : public Algorithm {

public:

  RichSimpleHPDFrontEndResponse( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~RichSimpleHPDFrontEndResponse();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();

private: // methods

  StatusCode Simple();

private: // data

  const RichBase* actual_base;
  RichRegistry theRegistry;

  MCRichSummedDeposits* SummedDeposits;

  std::string m_mcRichSummedDepositsLocation;
  std::string m_mcRichDigitsLocation;
  std::string m_detMode;

  int m_Baseline;
  int m_AdcCut;

  double m_Sigma;
  double m_Calibration;
  double m_Threshold;

  Rndm::Numbers m_gaussRndm;

};

#endif // RICHREADOUT_RICHSIMPLEHPDFRONTENDRESPONSE_H
