
//-----------------------------------------------------------------------------
/** @file RichMarkovRingFinderAlg.h
 *
 *  Header file for algorithm : RichMarkovRingFinderAlg
 *
 *  CVS Log :-
 *  $Id: RichMarkovRingFinderAlg.h,v 1.19 2006-07-27 20:15:25 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-08-09
 */
//-----------------------------------------------------------------------------

#ifndef RICHMARKOVRINGFINDER_RichMarkovRingFinderAlg_H
#define RICHMARKOVRINGFINDER_RichMarkovRingFinderAlg_H 1

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

// RichRecBase
#include "RichRecBase/RichRecAlgBase.h"

// interfaces
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichKernel/IRichSmartIDTool.h"

// boost
//#include "boost/assign/list_of.hpp"
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
 *  Tracklessm ring finder using a Markov Chaion Monte Carlo
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

  /// Adds data points to the the ring finder input
  StatusCode addDataPoints( GenRingF::GenericInput & input ) const;

  /// Save rings to TES
  StatusCode saveRings( const GenRingF::GenericResults & output ) const;

  /// Fill the ring points in the final reconstructed rings
  void buildRingPoints( LHCb::RichRecRing * ring,
                        const double scale,
                        const unsigned int nPoints = 100 ) const;

  /// Attempt to match a given Ring its best to RichRecSegment
  void matchSegment( LHCb::RichRecRing * ring ) const;

private: // data

  // tool pointers
  const IRichCherenkovAngle * m_ckAngle;  ///< Pointer to Cherenkov angle tool
  const IRichSmartIDTool * m_smartIDTool; ///< Pointer to RichSmartID tool

  const Rich::DetectorType m_rich;  ///< Which RICH Detector
  const Rich::Side         m_panel; ///< Which RICH Panel
  const Rich::RadiatorType m_rad;   ///< Which RICH radiator

  /// Location of output rings in TES
  std::string m_ringLocation;

};

#endif // RICHMARKOVRINGFINDER_RichMarkovRingFinderAlg_H

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
