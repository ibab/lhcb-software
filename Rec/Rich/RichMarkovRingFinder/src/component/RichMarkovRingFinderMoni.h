// $Id: RichMarkovRingFinderMoni.h,v 1.10 2004-10-22 19:15:28 abuckley Exp $
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
#include "RichRecBase/IRichCherenkovAngle.h"
#include "MCTools/IMCEffReconstructible.h"

// base class
#include "RichRecBase/RichRecAlgBase.h"

// local
#include "stringToNumber/stringToNumber.h"

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

  // Job options
  std::string m_mcHistPth; ///< Output MC truth histogram path
  std::string m_histPth;   ///< Output histogram path

  /// Fraction of MCParticles associated to Markov RichRecRing pixels which must 
  /// be exceeded for a track to be considered matched to a Markov ring
  double m_RingMatchFractionCutoff;

  /// Variables for tracking how well MC matching and rec-matching agree
  std::map<Rich::DetectorType, std::map<bool, unsigned int> > m_numMcVsRecMatchAgreements;

  // Histograms
  std::map<Rich::DetectorType, IHistogram1D*> m_NumRingsPerEvent;
  std::map<Rich::DetectorType, IHistogram1D*> m_NumPixsPerEvent;
  std::map<Rich::DetectorType, IHistogram1D*> m_MarkovRingRadius;
  std::map<Rich::DetectorType, IHistogram1D*> m_MarkovRingNumPixs;
  std::map<Rich::DetectorType, IHistogram2D*> m_MarkovRingRadiusVsNumPixs;
  std::map<Rich::DetectorType, IHistogram1D*> m_MarkovRingBestMCMatchFraction;
  std::map<Rich::DetectorType, IHistogram1D*> m_MarkovRingBestMCMatchNumber;

  std::map<std::string, IHistogram1D*> m_RingTrackMCType;
  std::map<std::string, IHistogram1D*> m_RingTrackOriginInVeloVertexType;
  std::map<std::string, IHistogram1D*> m_RingTrackOriginInVeloElectronVertexType;
  std::map<std::string, IHistogram1D*> m_CkPull;
  std::map<std::string, IHistogram1D*> m_RingTrackRecCategory;
  std::map<std::string, IHistogram1D*> m_RingTrackOriginInVeloRecCategory;
  
  std::map<std::string, IHistogram1D*> m_RingTrackOriginZ;
  std::map<std::string, IHistogram1D*> m_RingTrackOriginZ1;
  std::map<std::string, IHistogram2D*> m_RingTrackOriginXY1;
  std::map<std::string, IHistogram2D*> m_RingTrackOriginXY2;
  std::map<std::string, IHistogram2D*> m_RingTrackOriginXY3;
  std::map<std::string, IHistogram2D*> m_RingTrackOriginRZ;
  std::map<std::string, IHistogram2D*> m_RingTrackOriginRZ1;
  std::map<std::string, IHistogram2D*> m_RingTrackOriginRZ1Zoom;
  std::map<std::string, IHistogram2D*> m_RingTrackOriginRZ2;
  std::map<std::string, IHistogram2D*> m_RingTrackOriginRZ3;
  std::map<std::string, IHistogram3D*> m_RingTrackOrigin;
  std::map<std::string, IHistogram3D*> m_RingTrackOrigin1Zoom;

  std::map<std::string, IHistogram1D*> m_RingTrackDecayZ;
  std::map<std::string, IHistogram1D*> m_RingTrackEndDecayZ;
  std::map<std::string, IHistogram1D*> m_RingTrackEndNotDecayZ;
  std::map<std::string, IHistogram1D*> m_RingTrackDecaysWithVeloOriginZ;
  std::map<std::string, IHistogram2D*> m_RingTrackDecayRZ;
  std::map<std::string, IHistogram2D*> m_RingTrackDecaysWithVeloOriginRZ;
  std::map<std::string, IHistogram3D*> m_RingTrackDecay;
  std::map<std::string, IHistogram3D*> m_RingTrackDecaysWithVeloOrigin;

};
#endif // COMPONENT_RICHMARKOVRINGFINDERMONI_H
