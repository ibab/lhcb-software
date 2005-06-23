
//===============================================================================
/** @file RichDetailedFrontEndResponse.h
 *
 *  Header file for RICH digitisation algorithm : RichDetailedFrontEndResponse
 *
 *  CVS Log :-
 *  $Id: RichDetailedFrontEndResponse.h,v 1.2 2005-06-23 15:10:12 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Alex Howard   a.s.howard@ic.ac.uk
 *  @date   2003-11-06
 */
//===============================================================================

#ifndef RICHREADOUT_RICHDETAILEDFRONTENDRESPONSE_H
#define RICHREADOUT_RICHDETAILEDFRONTENDRESPONSE_H 1

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

// CLHEP
#include "CLHEP/Random/Random.h"

/** @class RichDetailedFrontEndResponse RichDetailedFrontEndResponse.h
 *  
 *  Performs a detailed simulation of the RICH HPD pixel response
 * 
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @author Alex Howard   a.s.howard@ic.ac.uk
 *  @date   2003-11-06
 */

class RichDetailedFrontEndResponse : public RichAlgBase {

public:

  /// Constructor
  RichDetailedFrontEndResponse( const std::string& name, 
                                ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~RichDetailedFrontEndResponse();

  virtual StatusCode initialize();
  virtual StatusCode finalize();
  virtual StatusCode execute();

  typedef std::pair< MCRichSummedDeposit*, RichTimeSample > TimeData;
  typedef std::vector< TimeData > samplecache_t;

private: // methods

  StatusCode Analog();  ///< Run the analogue simulation
  StatusCode Digital(); ///< Run the digital simulation

private: // data

  const RichBase * actual_base;
  RichRegistry theRegistry;

  samplecache_t tscache;

  MCRichSummedDeposits* m_summedDeposits;

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

#endif // RICHREADOUT_RICHDETAILEDFRONTENDRESPONSE_H
