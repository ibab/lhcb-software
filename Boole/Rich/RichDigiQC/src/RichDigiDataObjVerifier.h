// $Id: RichDigiDataObjVerifier.h,v 1.10 2006-03-12 12:09:26 jonrob Exp $
#ifndef RICHDIGIQC_RICHDIGIDATOBJVERIFIER_H
#define RICHDIGIQC_RICHDIGIDATOBJVERIFIER_H 1

// base class
#include "RichKernel/RichAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event model
#include "Event/MCRichDigit.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichHit.h"

// Kernel
#include "Kernel/RichSmartID.h"
#include "Kernel/RichDetectorType.h"

// Constants
#include "Kernel/PhysicalConstants.h"

/** @class RichDigiDataObjVerifier RichDigiDataObjVerifier.h RichDigiQC/RichDigiDataObjVerifier.h
 *
 *  Low level printout of Rich Digitisation objects
 *
 *  @author Chris Jones   (Christopher.Rob.Jones@cern.ch)
 *  @date   2003-09-08
 */

class RichDigiDataObjVerifier : public RichAlgBase 
{

public:

  /// Standard constructor
  RichDigiDataObjVerifier( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichDigiDataObjVerifier( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: // methods

  /// Check the MCRichHits at the given location
  void checkHitsAt( const std::string & location ) const;

  /// Check the MCRichOpticalPhotons at the given location
  void checkPhotsAt( const std::string & location ) const;

private: // data

  // job options
  bool m_bdMcDigits;      ///< Flag to turn on/off the checking and printing of MCRichDigits
  bool m_bdMCHits;        ///< Flag to turn on/off the checking and printing of MCRichHits
  bool m_bdMCPhots;       ///< Flag to turn on/off the checking and printing of MCRichOpticalPhotons

};

#endif // RICHDIGIQC_RICHDIGIDATOBJVERIFIER_H
