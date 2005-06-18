// $Id: RichDigiDataObjVerifier.h,v 1.6 2005-06-18 11:27:15 jonrob Exp $
#ifndef RICHDIGIQC_RICHDIGIDATOBJVERIFIER_H
#define RICHDIGIQC_RICHDIGIDATOBJVERIFIER_H 1

// base class
#include "RichKernel/RichAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// Event model
#include "Event/RichDigit.h"
#include "Event/MCRichDigit.h"
//#include "Event/MCRichDeposit.h"
//#include "Event/MCRichSummedDeposit.h"
#include "Event/MCRichHit.h"

// Kernel
#include "Kernel/RichSmartID.h"
#include "Kernel/RichDetectorType.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"


/** @class RichDigiDataObjVerifier RichDigiDataObjVerifier.h RichDigiQC/RichDigiDataObjVerifier.h
 *
 *  Low level printout of Rich Digitisation objects
 *
 *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
 *  @date   2003-09-08
 */

class RichDigiDataObjVerifier : public RichAlgBase {

public:

  /// Standard constructor
  RichDigiDataObjVerifier( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichDigiDataObjVerifier( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: // methods

  void printHitsAt( const std::string & location ) const;

private: // data

  // job options
  bool m_bdDigits;        ///< Flag to turn on/off the printing of RichDigits
  bool m_bdMcDigits;      ///< Flag to turn on/off the printing of MCRichDigits
  bool m_bdMCDeposits;    ///< Flag to turn on/off the printing of MCRichDeposits
  bool m_bdMCSumDeposits; ///< Flag to turn on/off the printing of MCRichSummedDeposits
  bool m_bdMCHits;        ///< Flag to turn on/off the printing of MCRichHits

};
#endif // RICHDIGIQC_RICHDIGIDATOBJVERIFIER_H
