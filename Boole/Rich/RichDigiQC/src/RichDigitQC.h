
//------------------------------------------------------------------------------------
/** @file RichDigitQC.h
 *
 *  Header file for RICH Digitisation Quality Control algorithm : RichDigitQC
 *
 *  CVS Log :-
 *  $Id: RichDigitQC.h,v 1.5 2005-01-07 12:38:09 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-09-08
 */
//------------------------------------------------------------------------------------

#ifndef RICHDIGIQC_RICHDIGITQC_H
#define RICHDIGIQC_RICHDIGITQC_H 1

// Boost
#include "boost/lexical_cast.hpp"

// base class
#include "RichKernel/RichMoniAlgBase.h"

// from Gaudi
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/SmartDataPtr.h"

// Event model
#include "Event/MCRichDigit.h"
#include "Event/RawEvent.h"

// Kernel
#include "Kernel/RichSmartID.h"
#include "Kernel/RichDetectorType.h"
#include "RichKernel/RichStatDivFunctor.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// RICH Interfaces
#include "RichKernel/IRichHPDToLevel1Tool.h"
#include "RichKernel/IRichHPDIDTool.h"
#include "RichKernel/IRichSmartIDTool.h"

/** @class RichDigitQC RichDigitQC.h RichDigiQC/RichDigitQC.h
 *
 *  Monitor for RichDigits
 *
 *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
 *  @date   2003-09-08
 */

class RichDigitQC : public RichMoniAlgBase {

public:

  /// Standard constructor
  RichDigitQC( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichDigitQC( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // methods

  /// Retrieves the raw event. If not available tries to build one from RawBuffer
  RawEvent * rawEvent() const;

private: // data

  /// Pointer to RICH Level1 tool
  IRichHPDToLevel1Tool * m_level1;

  /// Pointer to Rich HPD ID tool
  IRichHPDIDTool * m_hpdID;

  // Pointer to RichSmartID tool
  IRichSmartIDTool * m_smartIDs;

  /// Pointer to Raw Event
  mutable RawEvent * m_rawEvent;

  // job options
  std::string m_digitTDS;        ///< Location of MCRichDigits in TES
  std::string m_rawEventLoc;     ///< Input location for RawEvent in TES

  /// Number of events processed
  unsigned int m_evtC; 

  /// L1 occupancy counter
  typedef std::map< const RichDAQ::Level1ID, unsigned int > L1Counter;

  /// Counter for hits in each HPD
  typedef std::map< const RichSmartID, unsigned int > HPDCounter;
  HPDCounter m_nHPD[Rich::NRiches]; ///< Tally for HPD occupancy, in each RICH

};

#endif // RICHDIGIQC_RICHDIGITQC_H
