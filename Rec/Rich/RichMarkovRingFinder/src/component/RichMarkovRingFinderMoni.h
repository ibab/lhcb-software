// $Id: RichMarkovRingFinderMoni.h,v 1.12 2004-10-28 18:29:22 abuckley Exp $
#ifndef COMPONENT_RICHMARKOVRINGFINDERMONI_H 
#define COMPONENT_RICHMARKOVRINGFINDERMONI_H 1

// Event model
#include "Event/ProcStatus.h"
#include "Event/RichRecStatus.h"
#include "Event/MCRichSegment.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/RichRecRing.h"
#include "Event/RichRecPixel.h"

// interfaces
#include "RichKernel/IRichMCTrackInfoTool.h"
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichKernel/IRichMCTruthTool.h"
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichMCTrackInfoTool.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "MCTools/IMCEffReconstructible.h"

// base class
#include "RichRecBase/RichRecAlgBase.h"

// local
#include "stringToNumber/stringToNumber.h"

// Boost
#include "boost/lexical_cast.hpp"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram3D.h"


///////////////////////////////////////////////////////////////////////////////
/// @class RichMarkovRingFinderMoni
/// @brief Monitor class for Markov chain ring finder
///
/// @author Chris Lester  lester@hep.phy.cam.ac.uk
/// @author Chris Jones   Christopher.Rob.Jones@cern.ch
/// @author Andy Buckley  buckley@hep.phy.cam.ac.uk
/// @date   2004-09-24
///
/// The monitor runs after the ring finder algorithm and examines the rings
/// made by it. The current monitor code histograms the properties of MC tracks
/// which are unambigously associated to a MCMC ring according to whether or not
/// the track was reconstructed.
///
///////////////////////////////////////////////////////////////////////////////
class RichMarkovRingFinderMoni : public RichRecAlgBase {

public:

  /// Standard constructor
  RichMarkovRingFinderMoni( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichMarkovRingFinderMoni( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  enum RecType { rec = 0, notrec = 1 };

private: // inner class (a possibly redundant functor)

  /// Functor for counting the total number of elements represented
  /// by a "census" map of the form map<T, size_t>
  template <class T>
  class CountMap {
  private: 
    size_t _sum;
  public:
    CountMap() : _sum(0) {}
    void operator() (std::pair<T, size_t> elem) { _sum += elem.second; }
    operator size_t() { return _sum; }
  };

  enum MCOriginLocation {InVelo, InRich1, InTT, InMagnet, InT123, InRich2, Other};

private: // methods
  StatusCode bookHistograms();

private: // data

  IRichRecMCTruthTool* m_richRecMCTruth; ///< Pointer to RichRecMCTruthTool interface
  IRichMCTruthTool* m_richMCTruth; ///< Pointer to RichMCTruthTool interface
  IRichMCTrackInfoTool* m_mcTrackInfo; ///< Pointer to RichMCTrackInfoTool interface
  IRichCherenkovAngle* m_ckAngle; ///< Pointer to RichCherenkovAngle tool interface
  IRichRayTracing* m_raytrace; ///< Pointer to RichRaytracing tool interface
  IMCEffReconstructible* m_mcReconstructible; ///< Pointer to MCEffReconstructible tool interface
  IRichMCTrackInfoTool* m_mcRichTrackInfo; ///< Pointer to MCEffReconstructible tool interface

  // Job options
  std::string m_mcHistPth; ///< Output MC truth histogram path
  std::string m_histPth;   ///< Output histogram path

  /// Fraction of MCParticles associated to Markov RichRecRing pixels which must 
  /// be exceeded for a track to be considered matched to a Markov ring
  double m_RingMatchFractionCutoff;

  /// Variables for tracking how well MC matching and rec-matching agree
  std::map<Rich::DetectorType, std::map<bool, unsigned int> > m_numMcVsRecMatchAgreements;

  /// Basis and normal vectors for the Rich panels
  typedef map<Rich::DetectorType, map<Rich::Side, pair<HepVector3D, HepVector3D> > > RichPanelBases;
  typedef map<Rich::DetectorType, map<Rich::Side, HepVector3D > > RichPanelNormals;
  typedef pair<HepVector3D, HepVector3D> RichPanelBasis;
  RichPanelBases pdBases;
  RichPanelNormals pdNormals;

  // Histograms
  // ==========
  typedef std::map<Rich::DetectorType, IHistogram1D*> Histos1DByDetector;
  typedef std::map<Rich::DetectorType, IHistogram2D*> Histos2DByDetector;
  typedef std::map<Rich::DetectorType, IHistogram3D*> Histos3DByDetector;
  typedef std::map<Rich::DetectorType, std::map<Rich::Side, IHistogram1D*> > Histos1DByDetectorAndSide;
  typedef std::map<Rich::DetectorType, std::map<Rich::Side, IHistogram2D*> > Histos2DByDetectorAndSide;
  typedef std::map<Rich::DetectorType, std::map<Rich::Side, IHistogram3D*> > Histos3DByDetectorAndSide;
  typedef std::map<Rich::DetectorType, std::map<RecType, IHistogram1D*> > Histos1DByDetectorAndRecType;
  typedef std::map<Rich::DetectorType, std::map<RecType, IHistogram2D*> > Histos2DByDetectorAndRecType;
  typedef std::map<Rich::DetectorType, std::map<RecType, IHistogram3D*> > Histos3DByDetectorAndRecType;

  /// Number of Markov rings in the event
  Histos1DByDetector m_NumRingsPerEvent;
  /// Number of pixels in the event
  Histos1DByDetector m_NumPixsPerEvent;
  /// Radius distributin of Markov rings
  Histos1DByDetector m_MarkovRingRadius;
  /// Number of pixels on Markov rings
  Histos1DByDetector m_MarkovRingNumPixs;
  /// Correlation of Markov radius and number of pixels
  Histos2DByDetector m_MarkovRingRadiusVsNumPixs;
  /// Fraction of Markov ring pixels belonging to the dominant MCParticle
  Histos1DByDetector m_MarkovRingBestMCMatchFraction;
  /// Raw number of Markov ring pixels belonging to the dominant MCParticle
  Histos1DByDetector m_MarkovRingBestMCMatchNumber;

  // *** Fraction of agreements between rec segment-matching and MC pixel-matching

  /// MCPhoton incidence angle to pd plane normal
  Histos1DByDetector m_MarkovRingMCPhotonNormalAngle;
  /// MCPhoton shift from pd plane normal in the PD basis
  Histos2DByDetectorAndSide m_MarkovRingMCPhotonShiftFromNormal;
  /// Reverse direction of the incident MCPhoton
  Histos3DByDetectorAndSide m_MarkovRingMCPhotonReverseDirection;
  /// Reverse direction of the incident MCPhoton (x)
  Histos1DByDetectorAndSide m_MarkovRingMCPhotonReverseDirectionX;
  /// Reverse direction of the incident MCPhoton (y)
  Histos1DByDetectorAndSide m_MarkovRingMCPhotonReverseDirectionY;
  /// Reverse direction of the incident MCPhoton (z)
  Histos1DByDetectorAndSide m_MarkovRingMCPhotonReverseDirectionZ;

  /// Pull distributions and similar
  Histos1DByDetectorAndRecType m_RingTrackMCType;
  Histos1DByDetectorAndRecType m_RingTrackOriginInVeloVertexType;
  Histos1DByDetectorAndRecType m_RingTrackOriginInVeloElectronVertexType;
  Histos1DByDetectorAndRecType m_CkPull;
  Histos1DByDetectorAndRecType m_RecSegPdPointError;
  Histos1DByDetectorAndRecType m_MarkovSegPdPointError;
  Histos1DByDetectorAndRecType m_RingTrackRecCategory;
  Histos1DByDetectorAndRecType m_RingTrackOriginInVeloRecCategory;


  // Backtrace monitoring
  // for exit points
  Histos1DByDetectorAndRecType m_normalBacktracePointErrorMag;
  Histos1DByDetectorAndRecType m_meanBacktracePointErrorMag;
  Histos1DByDetectorAndRecType m_normalBacktracePointErrorX;
  Histos1DByDetectorAndRecType m_meanBacktracePointErrorX;
  Histos1DByDetectorAndRecType m_normalBacktracePointErrorY;
  Histos1DByDetectorAndRecType m_meanBacktracePointErrorY;
  Histos2DByDetectorAndRecType m_normalBacktracePointError;
  Histos2DByDetectorAndRecType m_meanBacktracePointError;
  // and for slopes
  Histos1DByDetectorAndRecType m_normalBacktraceSlopeErrorMag;
  Histos1DByDetectorAndRecType m_meanBacktraceSlopeErrorMag;
  Histos1DByDetectorAndRecType m_normalBacktraceSlopeErrorX;
  Histos1DByDetectorAndRecType m_meanBacktraceSlopeErrorX;
  Histos1DByDetectorAndRecType m_normalBacktraceSlopeErrorY;
  Histos1DByDetectorAndRecType m_meanBacktraceSlopeErrorY;
  Histos2DByDetectorAndRecType m_normalBacktraceSlopeError;
  Histos2DByDetectorAndRecType m_meanBacktraceSlopeError;
  // and for angles
  Histos1DByDetectorAndRecType m_normalBacktraceAngleError;
  Histos1DByDetectorAndRecType m_meanBacktraceAngleError;


  /// Ring track origin points
  Histos1DByDetectorAndRecType m_RingTrackOriginZ;
  Histos1DByDetectorAndRecType m_RingTrackOriginZ1;
  Histos2DByDetectorAndRecType m_RingTrackOriginXY1;
  Histos2DByDetectorAndRecType m_RingTrackOriginXY2;
  Histos2DByDetectorAndRecType m_RingTrackOriginXY3;
  Histos2DByDetectorAndRecType m_RingTrackOriginRZ;
  Histos2DByDetectorAndRecType m_RingTrackOriginRZ1;
  Histos2DByDetectorAndRecType m_RingTrackOriginRZ1Zoom;
  Histos2DByDetectorAndRecType m_RingTrackOriginRZ2;
  Histos2DByDetectorAndRecType m_RingTrackOriginRZ3;
  Histos3DByDetectorAndRecType m_RingTrackOrigin;
  Histos3DByDetectorAndRecType m_RingTrackOrigin1Zoom;

  // Ring track decay points
  Histos1DByDetectorAndRecType m_RingTrackDecayZ;
  Histos1DByDetectorAndRecType m_RingTrackEndDecayZ;
  Histos1DByDetectorAndRecType m_RingTrackEndNotDecayZ;
  Histos1DByDetectorAndRecType m_RingTrackDecaysWithVeloOriginZ;
  Histos2DByDetectorAndRecType m_RingTrackDecayRZ;
  Histos2DByDetectorAndRecType m_RingTrackDecaysWithVeloOriginRZ;
  Histos3DByDetectorAndRecType m_RingTrackDecay;
  Histos3DByDetectorAndRecType m_RingTrackDecaysWithVeloOrigin;

};
#endif // COMPONENT_RICHMARKOVRINGFINDERMONI_H
