
/** @file RichPixelCreatorFromRichDigitsWithBg.h
 *
 *  Header file for RICH reconstruction tool : RichPixelCreatorFromRichDigitsWithBg
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromRichDigitsWithBg.h,v 1.2 2004-10-13 10:32:49 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.1  2004/10/13 09:37:27  jonrob
 *  Add new pixel creator tool.
 *  Add ability to make pixels for particular radiators.
 *
 *
 *  @author Andy Buckley   buckley@hep.phy.cam.ac.uk
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   05/10/2004
 */

#ifndef RICHRECTOOLS_RICHPIXELCREATORFROMRICHDIGITS_H
#define RICHRECTOOLS_RICHPIXELCREATORFROMRICHDIGITS_H 1

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

// Event
#include "Event/RichDigit.h"
#include "Event/MCRichDigit.h"

/** @class RichPixelCreatorFromRichDigitsWithBg RichPixelCreatorFromRichDigitsWithBg.h
 *
 *  Tool for the creation and book-keeping of RichRecPixel objects.
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

class RichPixelCreatorFromRichDigitsWithBg : public RichRecToolBase,
                                             virtual public IRichPixelCreator,
                                             virtual public IIncidentListener {

public:

  /// Standard constructor
  RichPixelCreatorFromRichDigitsWithBg( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent );

  /// Destructor
  virtual ~RichPixelCreatorFromRichDigitsWithBg(){}

  /// Initialize method
  StatusCode initialize();

  /// Finalize method
  StatusCode finalize();

  /// Implement the handle method for the Incident service.
  /// This is used to inform the tool of software incidents.
  void handle( const Incident& incident );

  /// Returns a RichRecPixel object pointer for given ContainedObject.
  /// If if it not possible NULL is return.
  RichRecPixel * newPixel( const ContainedObject * obj ) const;

  /// Form all possible RichRecPixels from input RichDigits.
  /// The most efficient way to make all RichRecPixel objects in the event.
  StatusCode newPixels() const;

  /// Returns a pointer to the RichRecPixels
  RichRecPixels * richPixels() const;

  /// Add the current event's digits to a collection of bg track digits
  StatusCode fillBgTrackStack() const;

private: // methods

  /// Initialise for a new event
  void InitNewEvent();

  /// Returns a RichRecPixel object for given smart ID
  RichRecPixel * newPixel( const RichSmartID id ) const;

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

  /// Pointer to pixel map
  mutable std::map< RichSmartID::KeyType, RichRecPixel* > m_pixelExists;
  mutable std::map< RichSmartID::KeyType, bool > m_pixelDone;

  /// Stack of spare digits for use as track background
  typedef std::map<const MCParticle*, std::vector<RichSmartID> > BgTrackStack;
  mutable BgTrackStack m_digitsForTrackBg;

  /// Number of background tracks to add to each event
  size_t m_noBgTracksToAdd;

  /// Flags for which RICH detectors to create pixels for
  std::vector<bool> m_usedDets;

};

inline void RichPixelCreatorFromRichDigitsWithBg::InitNewEvent()
{
  // Initialise navigation data
  m_allDone = false;
  m_pixelExists.clear();
  m_pixelDone.clear();
  m_pixels = 0;
}

#endif // RICHRECTOOLS_RICHPIXELCREATORFROMRICHDIGITS_H
