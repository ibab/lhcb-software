// $Id: RichMarkovRingFinderMoni.h,v 1.6 2004-08-17 13:33:49 jonrob Exp $
#ifndef COMPONENT_RICHMARKOVRINGFINDERMONI_H 
#define COMPONENT_RICHMARKOVRINGFINDERMONI_H 1

// Event model
#include "Event/ProcStatus.h"
#include "Event/RichRecStatus.h"
#include "Event/MCRichSegment.h"
#include "Event/RichRecRing.h"

// interfaces
#include "RichKernel/IRichMCTrackInfoTool.h"
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichKernel/IRichMCTruthTool.h"

// base class
#include "RichRecBase/RichRecAlgBase.h"

// local
#include "stringToNumber/stringToNumber.h"

// Histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IHistogram3D.h"

using std::vector;
using std::string;
using std::map;
using std::pair;
using std::for_each;
using std::mem_fun_ref;

void printMap(pair<string,double> element);

///////////////////////////////////////////////////////////////////////////////
/// @class RichMarkovRingFinderMoni
/// @brief Monitor class for Markov chain ring finder
///
/// @author Chris Lester  lester@hep.phy.cam.ac.uk
/// @author Chris Jones   Christopher.Rob.Jones@cern.ch
/// @author Andy Buckley  buckley@hep.phy.cam.ac.uk
/// @date   2003-12-04
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
  RichMarkovRingFinderMoni( const string& name, ISvcLocator* pSvcLocator );

  virtual ~RichMarkovRingFinderMoni( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private: // methods
  StatusCode bookHistograms();

private: // data

  IRichRecMCTruthTool* m_richRecMCTruth; ///< Pointer to RichRecMCTruthTool interface
  IRichMCTruthTool* m_richMCTruth;
  IRichMCTrackInfoTool* m_mcTrackInfo;

  // job options
  string m_mcHistPth; ///< Output MC truth histogram path
  string m_histPth;   ///< Output histogram path

  // Histograms
  map<string, IHistogram1D*> m_RingTrackOriginZ;
  map<string, IHistogram1D*> m_RingTrackOriginZ1;
  map<string, IHistogram2D*> m_RingTrackOriginXY1;
  map<string, IHistogram2D*> m_RingTrackOriginXY2;
  map<string, IHistogram2D*> m_RingTrackOriginXY3;
  map<string, IHistogram2D*> m_RingTrackOriginRZ;
  map<string, IHistogram2D*> m_RingTrackOriginRZ1;
  map<string, IHistogram2D*> m_RingTrackOriginRZ1Zoom;
  map<string, IHistogram2D*> m_RingTrackOriginRZ2;
  map<string, IHistogram2D*> m_RingTrackOriginRZ3;
  map<string, IHistogram3D*> m_RingTrackOrigin;
  map<string, IHistogram3D*> m_RingTrackOrigin1Zoom;

  map<string, IHistogram1D*> m_RingTrackDecayZ;
  map<string, IHistogram2D*> m_RingTrackDecayRZ;
  map<string, IHistogram3D*> m_RingTrackDecay;

};
#endif // COMPONENT_RICHMARKOVRINGFINDERMONI_H
