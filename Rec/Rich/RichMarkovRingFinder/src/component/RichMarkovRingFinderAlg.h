// $Id: RichMarkovRingFinderAlg.h,v 1.8 2004-08-20 09:59:24 abuckley Exp $
#ifndef RICHMARKOVRINGFINDER_RICHMARKOVRINGFINDERALG_H
#define RICHMARKOVRINGFINDER_RICHMARKOVRINGFINDERALG_H 1

// Include files
#include <string>
#include <iostream>
#include <limits>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

// rich base
#include "RichRecBase/RichRecAlgBase.h"
#include "RichRecBase/RichRecProcCode.h"

// Event model
#include "Event/ProcStatus.h"
#include "Event/RichRecStatus.h"
#include "Event/RichRecRing.h"

// Kernel
#include "Kernel/RichSide.h"

// interfaces
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichKernel/IRichSmartIDTool.h"

// Ring finder stuff
#include "finder/Rich1Configuration.h"
#include "finder/Rich2Configuration.h"
#include "finder/RichMarkovRingFinder.h"
#include "stringToNumber/stringToNumber.h"


///////////////////////////////////////////////////////////////////////////
/// @class  RichMarkovRingFinderAlg
/// @brief  Templated base class for concrete Markov Ring Finder Algorithms
/// @author Chris Lester  lester@hep.phy.cam.ac.uk
/// @author Chris Jones   Christoper.Rob.Jones@cern.ch
/// @author Andy Buckley  Andrew.Buckley@cern.ch
/// @date   2003-12-02
///////////////////////////////////////////////////////////////////////////

template <class MyFinder>
class RichMarkovRingFinderAlg : public RichRecAlgBase {

public:

  virtual ~RichMarkovRingFinderAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  /// Standard constructor
  RichMarkovRingFinderAlg( const std::string& name,
                           ISvcLocator* pSvcLocator,
                           const Rich::DetectorType rich,
                           const Rich::Side panel );

private: // methods

  /// Check in the given segment is in the currently configured detector region
  const bool inCorrectArea( const RichRecSegment * segment ) const;

  /// Check in the given pixel is in the currently configured detector region
  const bool inCorrectArea( const RichRecPixel * pixel ) const;

  /// Process the event
  const StatusCode processEvent();

  /// Adds data points to Markov Chain initialisation object
  template <class AnInitialisationObject>
  const StatusCode addDataPoints( AnInitialisationObject & eio ) const;

  /// Add initial circle suggestions to Markov Chain initialisation object
  template <class AnInitialisationObject>
  const StatusCode addCircleSuggestions( AnInitialisationObject & eio ) const;

  /// Fill the ring points in the final reconstructed rings
  void buildRingPoints( RichRecRing * ring,
                        const double scale,
                        const unsigned int nPoints = 100 ) const;

protected:

  /// Returns the RICH Detector
  inline const Rich::DetectorType rich() const { return m_rich; }

  /// Returns the RICH panel
  inline const Rich::Side panel() const { return m_panel; }

private: // data

  // tool pointers
  IRichCherenkovAngle * m_ckAngle;  ///< Pointer to Cherenkov angle tool
  IRichSmartIDTool * m_smartIDTool; ///< Pointer to RichSmartID tool

  bool m_useRichSeed; ///< Inialise ring finder using suggested circles from tracks

  const Rich::DetectorType m_rich;  ///< which Rich Detector
  const Rich::Side         m_panel; ///< which Rich Panel

  // The ring finder
  MyFinder * m_finder; ///< Pointer to internal Ring Finder

  /// Location of output rings in TES
  std::string m_ringLocation;

  /// Probability of associating a hit to a Markov circle above 
  /// which counts as a positive association
  double m_CutoffHitOnCircleProbability;

  /// Fraction of ring radius which an extrapolated track segment must lie within
  /// to be considered associated to the ring
  double m_CutoffSegPositionInRing;

};


///////////////////////////////////////////////////////////////////////////
/// @class  Rich1LMarkovRingFinderAlg
/// @brief  Standalone ring finder for RICH1 lower panel using Markov Chain
///
/// @author Chris Lester  lester@hep.phy.cam.ac.uk
/// @author Chris Jones   Christopher.Rob.Jones@cern.ch
/// @date   2003-12-02
///////////////////////////////////////////////////////////////////////////
typedef Lester::RichMarkovRingFinder<Lester::Rich1Configuration, Lester::globalRich1LConfiguration> Rich1LMarkovRingFinder;
class Rich1LMarkovRingFinderAlg : public RichMarkovRingFinderAlg<Rich1LMarkovRingFinder> {

public:

  Rich1LMarkovRingFinderAlg( const std::string& name,
                             ISvcLocator* pSvcLocator )
    : RichMarkovRingFinderAlg<Rich1LMarkovRingFinder>(name,pSvcLocator,
                                                      Rich::Rich1,Rich::bottom) { }

};


///////////////////////////////////////////////////////////////////////////
/// @class  Rich1UMarkovRingFinderAlg
/// @brief  Standalone ring finder for RICH1 upper panel using Markov Chain
///
/// @author Chris Lester  lester@hep.phy.cam.ac.uk
/// @author Chris Jones   Christopher.Rob.Jones@cern.ch
/// @date   2003-12-02
///////////////////////////////////////////////////////////////////////////
typedef Lester::RichMarkovRingFinder<Lester::Rich1Configuration, Lester::globalRich1UConfiguration> Rich1UMarkovRingFinder;
class Rich1UMarkovRingFinderAlg : public RichMarkovRingFinderAlg<Rich1UMarkovRingFinder> {

public:

  Rich1UMarkovRingFinderAlg( const std::string& name,
                             ISvcLocator* pSvcLocator )
    : RichMarkovRingFinderAlg<Rich1UMarkovRingFinder>(name, pSvcLocator,
                                                      Rich::Rich1,Rich::top) { }

};


///////////////////////////////////////////////////////////////////////////
/// @class  Rich2LMarkovRingFinderAlg
/// @brief  Standalone ring finder for RICH2 left panel using Markov Chain
///
/// @author Chris Lester  lester@hep.phy.cam.ac.uk
/// @author Chris Jones   Christopher.Rob.Jones@cern.ch
/// @date   2003-12-02
///////////////////////////////////////////////////////////////////////////

typedef Lester::RichMarkovRingFinder<Lester::Rich2Configuration, Lester::globalRich2LConfiguration> Rich2LMarkovRingFinder;
class Rich2LMarkovRingFinderAlg : public RichMarkovRingFinderAlg<Rich2LMarkovRingFinder> {

public:

  Rich2LMarkovRingFinderAlg( const std::string& name, 
                             ISvcLocator* pSvcLocator )
    : RichMarkovRingFinderAlg<Rich2LMarkovRingFinder>(name, pSvcLocator,
                                                      Rich::Rich2,Rich::left) { }

};


///////////////////////////////////////////////////////////////////////////
/// @class  Rich2RMarkovRingFinderAlg
/// @brief  Standalone ring finder for RICH2 right panel using Markov Chain
///
/// @author Chris Lester  lester@hep.phy.cam.ac.uk
/// @author Chris Jones   Christopher.Rob.Jones@cern.ch
/// @date   2003-12-02
///////////////////////////////////////////////////////////////////////////

typedef Lester::RichMarkovRingFinder<Lester::Rich2Configuration, Lester::globalRich2RConfiguration> Rich2RMarkovRingFinder;
class Rich2RMarkovRingFinderAlg : public RichMarkovRingFinderAlg<Rich2RMarkovRingFinder> {

public:

  Rich2RMarkovRingFinderAlg( const std::string& name, 
                             ISvcLocator* pSvcLocator )
    : RichMarkovRingFinderAlg<Rich2RMarkovRingFinder>(name, pSvcLocator,
                                                      Rich::Rich2,Rich::right) { }

};

#endif // RICHMARKOVRINGFINDER_RICHMARKOVRINGFINDERALG_H
