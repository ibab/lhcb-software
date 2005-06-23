
//-----------------------------------------------------------------------------
/** @file RichPixelCreatorFromRichDigitsWithBg.h
 *
 *  Header file for RICH reconstruction tool : RichPixelCreatorFromRichDigitsWithBg
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromRichDigitsWithBg.h,v 1.10 2005-06-23 15:14:12 jonrob Exp $
 *
 *  @author Andy Buckley   buckley@hep.phy.cam.ac.uk
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   05/10/2004
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RICHPIXELCREATORFROMRICHDIGITSWITHBG_H
#define RICHRECMCTOOLS_RICHPIXELCREATORFROMRICHDIGITSWITHBG_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichPixelCreatorBase.h"

// interfaces
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichMCTruthTool.h"

// RichKernel
#include "RichKernel/RichMap.h"

// Event
#include "Event/RichDigit.h"
#include "Event/MCRichDigit.h"

//------------------------------------------------------------------------------------
/** @class RichPixelCreatorFromRichDigitsWithBg RichPixelCreatorFromRichDigitsWithBg.h
 *
 *  Tool for the creation and book-keeping of RichRecPixel objects.
 *
 *  Uses RichDigits from the digitisation as the parent objects.
 *  Additionally maintains a cache of RichDigits to be used as trackless ring
 *  background for testing reconstruction robustness to untracked tracks.
 *  Cache is filled as required from real events and for these events no pixels are
 *  created so effectively these events are skipped.
 *
 *  @author Andy Buckley   buckley@hep.phy.cam.ac.uk
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   05/10/2004
 */
//------------------------------------------------------------------------------------

class RichPixelCreatorFromRichDigitsWithBg : public RichPixelCreatorBase
{

public: // methods for Gaudi

  /// Standard constructor
  RichPixelCreatorFromRichDigitsWithBg( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent );

  /// Destructor
  virtual ~RichPixelCreatorFromRichDigitsWithBg(){}

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Returns a RichRecPixel object pointer for given object.
  // If if it not possible NULL is return.
  RichRecPixel * newPixel( const ContainedObject * obj ) const;

  // Form all possible RichRecPixels from input RichDigits.
  // The most efficient way to make all RichRecPixel objects in the event.
  StatusCode newPixels() const;

private: // methods

  /// Returns a RichRecPixel object for given smart ID
  RichRecPixel * newPixel( const RichSmartID id ) const;

  /// Add the current event's digits to a collection of bg track digits
  StatusCode fillBgTrackStack() const;

private: // data

  /// Pointer to RichSmartID tool
  const IRichSmartIDTool * m_smartIDTool;

  /// Pointer to RichMCTruth tool
  const IRichMCTruthTool * m_mcTool;

  /// String containing input RichDigits location in TES
  std::string m_recoDigitsLocation;

  /// Typedef for mapping between MCParticles and RIchSmartIDs
  typedef RichMap<const MCParticle*, RichSmartID::Collection > BgTrackStack;

  /// Stack of RichSmartIDs for a single MCParticle for use as track background
  mutable RichMap<Rich::DetectorType, BgTrackStack> m_digitsForTrackBg;

  /// Number of background tracks to add to each event
  std::vector<size_t> m_numBgTracksToAdd;

};

#endif // RICHRECMCTOOLS_RICHPIXELCREATORFROMRICHDIGITSWITHBG_H
