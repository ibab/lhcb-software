
#ifndef RICHREADOUT_RICHSIMPLEFRONTENDRESPONSE_H
#define RICHREADOUT_RICHSIMPLEFRONTENDRESPONSE_H 1

// base class
#include "RichUtils/RichAlgBase.h"

// from Gaudi
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/RndmGenerators.h"

// Detector geom
#include "RichDetTools/IRichSmartIDTool.h"

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

// CLHep
#include "CLHEP/Random/Random.h"

/** @class RichSimpleFrontEndResponse RichSimpleFrontEndResponse.h
 *  
 *  Performs a simple simulation of the RICH HPD pixel response
 * 
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Alex Howard   a.s.howard@ic.ac.uk
 *  @date   2003-11-06
 */

class RichSimpleFrontEndResponse : public RichAlgBase {

public:

  RichSimpleFrontEndResponse( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~RichSimpleFrontEndResponse();

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

  int m_Baseline;
  int m_AdcCut;

  double m_Sigma;
  double m_Calibration;
  double m_Threshold;

  Rndm::Numbers m_gaussRndm;

};

#endif // RICHREADOUT_RICHSIMPLEFRONTENDRESPONSE_H
