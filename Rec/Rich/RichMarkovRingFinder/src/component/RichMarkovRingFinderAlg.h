
//-----------------------------------------------------------------------------
/** @file RichMarkovRingFinderAlg.h
 *
 *  Header file for algorithm : RichMarkovRingFinderAlg
 *
 *  CVS Log :-
 *  $Id: RichMarkovRingFinderAlg.h,v 1.21 2006-08-03 17:16:42 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-08-09
 */
//-----------------------------------------------------------------------------

#ifndef RICHMARKOVRINGFINDER_RichMarkovRingFinderAlg_H
#define RICHMARKOVRINGFINDER_RichMarkovRingFinderAlg_H 1

// STD
#include <sstream>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

// RichRecBase
#include "RichRecBase/RichRecAlgBase.h"

// interfaces
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichKernel/IRichSmartIDTool.h"

// boost
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"

// Ring Finder includes
#include "GenericRingFinder/GenericInput.h"
// do not want to do this really but need to for the moment
#include "rich-new-simple/CrudeSampler.h"
#include "rich-new-simple/RichParams.h"

//-----------------------------------------------------------------------
/** @class RichMarkovRingFinderAlg RichMarkovRingFinderAlg.h
 *
 *  Trackless ring finder using a Markov Chaion Monte Carlo method
 *
 *  Uses the "Ring Finding Library" developed by C.G.Lester
 *  (lester@hep.phy.cam.ac.uk)
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-08-09
 */
//-----------------------------------------------------------------------

class RichMarkovRingFinderAlg : public RichRecAlgBase
{

public:

  /// Standard constructor
  RichMarkovRingFinderAlg( const std::string& name,
                           ISvcLocator* pSvcLocator,
                           const Rich::DetectorType rich,
                           const Rich::Side panel,
                           const Rich::RadiatorType rad );

  virtual ~RichMarkovRingFinderAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  /// Returns the RICH Detector
  inline const Rich::DetectorType rich() const { return m_rich; }

  /// Returns the RICH panel
  inline const Rich::Side panel() const { return m_panel; }

  /// Returns the RICH radiator
  inline const Rich::RadiatorType rad() const { return m_rad; }

private: // methods

  /// General RICH reco initialisations
  StatusCode richInit();

  /// Run the ring finder
  StatusCode runRingFinder();

  /// Get rings at given location
  LHCb::RichRecRings * getRings( const std::string & location ) const;

  /// Adds data points to the the ring finder input
  StatusCode addDataPoints( GenRingF::GenericInput & input ) const;

  /// Save rings to TES
  StatusCode saveRings( const GenRingF::GenericInput & input,
                        const GenRingF::GenericResults & output ) const;

  /// Fill the ring points in the final reconstructed rings
  void buildRingPoints( LHCb::RichRecRing * ring,
                        const unsigned int nPoints = 100 ) const;

  /// Attempt to match a given Ring its best to RichRecSegment
  void matchSegment( LHCb::RichRecRing * ring ) const;

  /// Add references to given ring to associated pixels
  StatusCode addRingToPixels( LHCb::RichRecRing * ring ) const;

  /// Create data text files for standalone ring finder
  StatusCode dumpToTextfile() const;

  /// Computes the average rings hit prob
  double avRingHitProb( LHCb::RichRecRing * ring ) const;

private: // data

  // tool pointers
  const IRichCherenkovAngle * m_ckAngle;  ///< Pointer to Cherenkov angle tool
  const IRichSmartIDTool * m_smartIDTool; ///< Pointer to RichSmartID tool

  const Rich::DetectorType m_rich;  ///< Which RICH Detector
  const Rich::Side         m_panel; ///< Which RICH Panel
  const Rich::RadiatorType m_rad;   ///< Which RICH radiator

  /// Pointer to the sampler (ring finder)
  CrudeSampler * m_sampler;

  /// Location of all output rings in TES
  std::string m_ringLocation;

  /// Location of best output rings in TES
  std::string m_bestRingLocation;

  /// Job option to turn on dumping of data to text files, for standalone ring finder application
  bool m_dumpText;

  /// Min probability to associate a pixel to a ring
  double m_minAssProb;

  /// Minimum number of hits on best rings
  unsigned int m_minNumHitsBest;

  /// Minimum average hit prob for best rings
  double m_minAvProbBest;

private: // constants ( perhaps should be options ?)

  /// data scale factor, to turn local coordinates into angles
  const double m_scaleFactor ;

};

///////////////////////////////////////////////////////////////////////////
/// @class  Rich2LeftPanelMarkovRingFinderAlg
/// @brief  Standalone ring finder for RICH2 left panel using Markov Chains
///
/// @author Chris Jones   Christopher.Rob.Jones@cern.ch
/// @date   2003-12-02
///////////////////////////////////////////////////////////////////////////
class Rich2LeftPanelMarkovRingFinderAlg : public RichMarkovRingFinderAlg
{
public:
  /// Default Constructor
  Rich2LeftPanelMarkovRingFinderAlg( const std::string& name,
                                     ISvcLocator* pSvcLocator )
    : RichMarkovRingFinderAlg(name,pSvcLocator,Rich::Rich2,Rich::left,Rich::Rich2Gas) { }
};

///////////////////////////////////////////////////////////////////////////
/// @class  Rich2LeftPanelMarkovRingFinderAlg
/// @brief  Standalone ring finder for RICH2 right panel using Markov Chains
///
/// @author Chris Jones   Christopher.Rob.Jones@cern.ch
/// @date   2003-12-02
///////////////////////////////////////////////////////////////////////////
class Rich2RightPanelMarkovRingFinderAlg : public RichMarkovRingFinderAlg
{
public:
  /// Default Constructor
  Rich2RightPanelMarkovRingFinderAlg( const std::string& name,
                                      ISvcLocator* pSvcLocator )
    : RichMarkovRingFinderAlg(name,pSvcLocator,Rich::Rich2,Rich::right,Rich::Rich2Gas) { }
};

#endif // RICHMARKOVRINGFINDER_RichMarkovRingFinderAlg_H
