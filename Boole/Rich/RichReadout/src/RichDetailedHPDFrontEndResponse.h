
#ifndef RICHREADOUT_RICHDETAILEDHPDFRONTENDRESPONSE_H
#define RICHREADOUT_RICHDETAILEDHPDFRONTENDRESPONSE_H 1

// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IToolSvc.h"

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

class RichDetailedHPDFrontEndResponse : public Algorithm {

public:

  RichDetailedHPDFrontEndResponse( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~RichDetailedHPDFrontEndResponse();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();

  typedef std::map<MCRichSummedDeposit*, RichTimeSample > samplecache_t;

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

  int m_Baseline;
  double m_Calibration;

};

#endif // RICHREADOUT_RICHHPDFRONTENDRESPONSESICB_H
