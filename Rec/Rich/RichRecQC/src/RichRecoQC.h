// $Id: RichRecoQC.h,v 1.2 2003-07-03 13:12:16 jonesc Exp $
#ifndef RICHRECQC_RICHRECOQC_H
#define RICHRECQC_RICHRECOQC_H 1

// base class
#include "RichRecBase/RichRecAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
//#include "GaudiKernel/IHistogramSvc.h"

// Relations
#include "Relations/IAssociatorWeighted.h"
#include "Relations/IRelationWeighted.h"

// RichRecBase
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichRecBase/RichTrackType.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// Event
#include "Event/RichRecStatus.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// interfaces
#include "RichRecBase/IRichSegmentProperties.h"

/** @class RichRecoQC RichRecoQC.h
 *
 *  Quality control monitor for Rich Reconstruction
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-02
 */

class RichRecoQC : public RichRecAlgBase {

public:

  /// Standard constructor
  RichRecoQC( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichRecoQC( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: // methods

  /// Book histograms
  StatusCode bookHistograms();

  /// Book histograms that require MC truth
  StatusCode bookMCHistograms();

private: // data

  // Pointers to tool instances
  IRichSegmentProperties * m_segProps;

  /// Pointer to RichRecMCTruthTool interface
  IRichRecMCTruthTool* m_richRecMCTruth;

  // job options
  std::string m_mcHistPth; ///< Output MC truth histogram path
  std::string m_histPth;   ///< Output histogram path
  int m_bins;              ///< number of bins to use in histograms
  double m_minBeta;        ///< minimum beta value for 'saturated' tracks

  // Histograms
  IHistogram1D* m_ckTrueDTheta[Rich::NRadiatorTypes];       ///< Cherenkov angle resolution for beta=1 tracks
  IHistogram1D* m_trueSignalPhots[Rich::NRadiatorTypes];    ///< Number of p.e.s for beta=1 tracks

};
#endif // RICHRECQC_RICHRECOQC_H
