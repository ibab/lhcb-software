
//-----------------------------------------------------------------------------
/** @file RichRecPixelQC.h
 *
 *  Header file for algorithm class : RichRecPixelQC
 *
 *  CVS Log :-
 *  $Id: RichRecPixelQC.h,v 1.4 2006-08-31 16:57:53 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECMONITOR_RichRecPixelQC_H
#define RICHRECMONITOR_RichRecPixelQC_H 1

// STD
#include <sstream>

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// RichKernel
#include "RichKernel/RichPoissonEffFunctor.h"
#include "RichKernel/RichStatDivFunctor.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// Event
#include "Event/MCRichDigitSummary.h"

// RichDet
#include "RichDet/DeRichSystem.h"

// Interfaces
#include "RichKernel/IRichMCTruthTool.h"
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"

//-----------------------------------------------------------------------------
/** @class RichRecPixelQC RichRecPixelQC.h
 *
 *  Monitor class for the pixel reconstruction
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

class RichRecPixelQC : public RichRecHistoAlgBase
{

public:

  /// Standard constructor
  RichRecPixelQC( const std::string& name,
                  ISvcLocator* pSvcLocator );

  virtual ~RichRecPixelQC( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // helper classes

  /// Simple class containing MC flags for a given pixel (reconstructed hit)
  class MCFlags
  {
  public:
    /// Default constructor
    MCFlags() :
      isBkg(false),isHPDQCK(false),isGasCK(false),isN2CK(false),
      isChargedTk(false),isChargeShare(false),isAeroFiltCK(false),isSignal(false),
      isAerogelCK(false),isRich1GasCK(false),isRich2GasCK(false) { }
    // data members
    bool isBkg;         ///< Hit is background, of any sort
    bool isHPDQCK;      ///< Hit is from CK radiation from the HPD quartz windows
    bool isGasCK;       ///< Hit is from CK radiation from the gas volume quartz windows
    bool isN2CK;        ///< Hit is from CK radiation from N2
    bool isChargedTk;   ///< Hit is from a charged track hitting the HPD
    bool isChargeShare; ///< Hit is from charge sharing in the HPD silicon detector
    bool isAeroFiltCK;  ///< Hit is from CK radiator in the aerogel Filter
    bool isSignal;      ///< Hit is from (unscatted) CK radiation from one of the main radiators
    bool isAerogelCK;   ///< Hit is from (unscatted) CK radiation from the aerogel radiator
    bool isRich1GasCK;  ///< Hit is from (unscatted) CK radiation from the Rich1 gas radiator
    bool isRich2GasCK;  ///< Hit is from (unscatted) CK radiation from the Rich2 gas radiator
  };

  /// Stores tallys opf various types
  class Tallys
  {
  public:
    /// Constructor
    Tallys()
      : pixels            ( Rich::NRiches,        0 ),
        bkgs              ( Rich::NRiches,        0 ),
        npdqcks           ( Rich::NRiches,        0 ),
        ngasck            ( Rich::NRiches,        0 ),
        n2ck              ( Rich::NRiches,        0 ),
        ntrack            ( Rich::NRiches,        0 ),
        nchargeshare      ( Rich::NRiches,        0 ),
        naerofilter       ( Rich::NRiches,        0 ),
        signal            ( Rich::NRiches,        0 ),
        radHits           ( Rich::NRadiatorTypes, 0 ) {}
    // data members
    std::vector<unsigned int> pixels;   ///< Total number of pixels hit
    std::vector<unsigned int> bkgs;     ///< Total number of background hits
    std::vector<unsigned int> npdqcks;  ///< Total number of HPD quartz window CK hits
    std::vector<unsigned int> ngasck;   ///< Total number of gas volume quartz window CK hits
    std::vector<unsigned int> n2ck;     ///< Total number of N2 CK hits
    std::vector<unsigned int> ntrack;   ///< Total number of charged track on HPD hits
    std::vector<unsigned int> nchargeshare; ///< Total number of silicon charge share hits
    std::vector<unsigned int> naerofilter; ///< Total number of aerogel filter C hits
    std::vector<unsigned int> signal;   ///< Total number of signal hits
    std::vector<unsigned int> radHits;  ///< Total number of signal hits for each radiator
  };

private: // methods

  /// Get various MC history flags for given RichSmartID
  MCFlags getHistories( const RichSmartID id ) const;

  /// Print summary for given RICH
  void printRICH( const Rich::DetectorType rich ) const;

private: // data

  /// Pointer to RichRecMCTruthTool interface
  const IRichRecMCTruthTool* m_richRecMCTruth;

  /// Pointer to RichMCTruthTool
  const IRichMCTruthTool * m_truth;

  /// Raw Buffer Decoding tool
  const IRichRawBufferToSmartIDsTool * m_decoder;

  /// Pointer to RICH system detector element
  const DeRichSystem * m_richSys;

  // tallies

  Tallys m_recoTally; ///< tally for reconstructed pixels
  Tallys m_rawTally;  ///< tally for all raw pixels

  unsigned int m_nEvts; ///< Total number of events processed

};

#endif // RICHRECMONITOR_RichRecPixelQC_H
