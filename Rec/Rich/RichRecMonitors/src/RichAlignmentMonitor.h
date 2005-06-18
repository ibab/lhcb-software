
/** @file RichAlignmentMonitor.h
 *
 *  Header file for algorithm class : RichAlignmentMonitor
 *
 *  CVS Log :-
 *  $Id: RichAlignmentMonitor.h,v 1.1.1.1 2005-06-18 11:44:45 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.7  2005/04/20 15:34:43  papanest
 *  Improvements in RichAlignmentMonitor
 *
 *  Revision 1.6  2004/09/24 11:29:56  papanest
 *  Bug fix
 *
 *  Revision 1.5  2004/07/27 09:48:04  jonrob
 *  Add doxygen file documentation and CVS information
 *
 *
 *  @author Antonis Papanestis   a.papanestis@rl.ac.uk
 *  @date   2004-02-19
 */

#ifndef RICHRECMONITOR_RICHALIGNMENTMONITOR_H
#define RICHRECMONITOR_RICHALIGNMENTMONITOR_H 1

// Include files
// base class
#include "RichRecBase/RichRecMoniAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Relations
//#include "Relations/IAssociatorWeighted.h"
//#include "Relations/IRelationWeighted.h"

// Event
#include "Event/RichRecStatus.h"
#include "Event/MCRichOpticalPhoton.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile1D.h"

// Interfaces
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichKernel/IRichRefractiveIndex.h"
#include "RichRecBase/IRichParticleProperties.h"
#include "RichRecBase/IRichCherenkovAngle.h"

// Kernel
#include "RichKernel/BoostArray.h"

// boost
#include "boost/lexical_cast.hpp"

/** @class RichAlignmentMonitor RichAlignmentMonitor.h
 *
 *  Provides monitoring histograms for the alignment of the RICH mirrors.
 *
 *  @author Antonis Papanestis   a.papanestis@rl.ac.uk
 *  @date   2004-02-19
 */

class RichAlignmentMonitor : public RichRecMoniAlgBase {

public:

  // type defintion
  typedef RichGeomPhotonCode::ShortType MirrorNumber;

  /// Standard constructor
  RichAlignmentMonitor( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichAlignmentMonitor( ); ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // methods

  /// four configurations: 2 no MCTruth, photon track association and + true
  /// Cherenkov angle 
  enum MCINFO {NO_MC = 0, NO_MC2_RICH2, TRACK, TR_A_ANGLE};

  /// Book hisograms
  StatusCode bookHistos();

  /// Book deltaTheta Phi hisogram
  StatusCode bookPhiHisto(MCINFO level, int histoID);

private: // data

  // properties
  std::string m_histPth;
  /// different histo path according to configuration.
  std::string m_histPthMCLevel[4];
  int m_maxUsedTracks;

  // set to know to stop all MC Truth
  bool m_useMCTruth;
  // use all tracks (not only saturated)
  bool m_useAllTracks;
  // associate photons with their MC track (reject background) 
  bool m_assocTrackPhoton;
  // use a second higher saturation energy for Rich2
  bool m_highSatEnergyRich2;

  // Flag to turn on/off the reconstruction of all photons if none are present
  bool m_buildPhotons;

  // energy for saturated ring
  boost::array<double,2> m_saturationEnergy;
  double m_saturationEnergyR1;
  double m_saturationEnergyR2;
  double m_2ndSaturationEnergyR2;

  // to avoid bias towards small angles use only a photons in the expected

  double m_deltaThetaRange;
  double m_deltaThetaHistoRange;

  

  // Pointer to RichRecMCTruthTool interface
  IRichRecMCTruthTool* m_richRecMCTruth;
  IRichRefractiveIndex* m_richRefIndexTool;
  IRichParticleProperties* m_richPartProp; ///< Rich Particle properties
  IRichCherenkovAngle* m_ckAngle;  ///< Pointer to RichCherenkovAngle tool

  // Histograms
  IHistogram1D* m_ambigMirrorsHist[2];
  IHistogram1D* m_sphMirrorNumberHist[2];
  IHistogram1D* m_flatMirrorNumberHist[2];
  IHistogram1D* m_trackBeta[2];
  IHistogram1D* m_ChAngleError[2];
  IHistogram2D* m_ChAngleErrorvPhiUnamb[2];
  IHistogram2D* m_sphMirReflPoint[2];
  IHistogram2D* m_flatMirReflPoint[2];

  IHistogram2D* m_alignmentHist[3000];
  int m_histCounter;
  int m_histIndex[4][2][6000];

  double m_saturatedAngle[2];

};
#endif // RICHRECMONITOR_RICHALIGNMENTMONITOR_H
