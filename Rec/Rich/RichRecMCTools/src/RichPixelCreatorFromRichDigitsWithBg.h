
//-----------------------------------------------------------------------------
/** @file RichPixelCreatorFromRichDigitsWithBg.h
 *
 *  Header file for RICH reconstruction tool : RichPixelCreatorFromRichDigitsWithBg
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromRichDigitsWithBg.h,v 1.7 2005-04-15 16:32:30 jonrob Exp $
 *
 *  @author Andy Buckley   buckley@hep.phy.cam.ac.uk
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   05/10/2004
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RICHPIXELCREATORFROMRICHDIGITSWITHBG_H
#define RICHRECMCTOOLS_RICHPIXELCREATORFROMRICHDIGITSWITHBG_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichPixelCreator.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichMCTruthTool.h"

// RichKernel
#include "RichKernel/RichMap.h"
#include "RichKernel/RichHashMap.h"

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

class RichPixelCreatorFromRichDigitsWithBg : public RichRecToolBase,
                                             virtual public IRichPixelCreator,
                                             virtual public IIncidentListener {

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

  // Implement the handle method for the Incident service.
  // This is used to inform the tool of software incidents.
  void handle( const Incident& incident );

public: // methods (and doxygen comments) inherited from public interface

  // Returns a RichRecPixel object pointer for given object.
  // If if it not possible NULL is return.
  RichRecPixel * newPixel( const ContainedObject * obj ) const;

  // Form all possible RichRecPixels from input RichDigits.
  // The most efficient way to make all RichRecPixel objects in the event.
  StatusCode newPixels() const;

  // Returns a pointer to the RichRecPixels
  RichRecPixels * richPixels() const;

private: // methods

  /// Initialise for a new event
  void InitNewEvent();

  /// Returns a RichRecPixel object for given smart ID
  RichRecPixel * newPixel( const RichSmartID id ) const;

  /// Add the current event's digits to a collection of bg track digits
  StatusCode fillBgTrackStack() const;

private: // data

  /// Pointer to RichRecPixels
  mutable RichRecPixels * m_pixels;

  /// Pointer to RichSmartID tool
  IRichSmartIDTool * m_smartIDTool;

  /// Pointer to RichMCTruth tool
  IRichMCTruthTool * m_mcTool;

  /// String containing input RichDigits location in TES
  std::string m_recoDigitsLocation;

  /// Location of RichRecPixels in TES
  std::string m_richRecPixelLocation;

  /// Flag to signify all pixels have been formed
  mutable bool m_allDone;

  /// Map of existing pixels
  mutable RichMap< RichSmartID::KeyType, RichRecPixel* > m_pixelExists;

  /// map to indicate pixels that have been considered
  mutable RichMap< RichSmartID::KeyType, bool > m_pixelDone;

  /// Stack of RichSmartIDS for a single MCParticle for use as track background
  typedef RichMap<const MCParticle*, std::vector<RichSmartID> > BgTrackStack;
  mutable RichMap<Rich::DetectorType, BgTrackStack> m_digitsForTrackBg;

  /// Number of background tracks to add to each event
  std::vector<size_t> m_numBgTracksToAdd;

  /// Flags for which RICH detectors to create pixels for
  std::vector<bool> m_usedDets;

};

inline void RichPixelCreatorFromRichDigitsWithBg::InitNewEvent()
{
  m_allDone = false;
  m_pixelExists.clear();
  m_pixelDone.clear();
  m_pixels = 0;
}

#endif // RICHRECMCTOOLS_RICHPIXELCREATORFROMRICHDIGITSWITHBG_H
