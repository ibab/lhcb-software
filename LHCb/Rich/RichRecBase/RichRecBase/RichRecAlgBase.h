// $Id: RichRecAlgBase.h,v 1.11 2004-02-02 14:23:04 jonesc Exp $
#ifndef RICHRECALGS_RICHRECALGBASE_H
#define RICHRECALGS_RICHRECALGBASE_H 1

// base class
#include "RichUtils/RichAlgBase.h"

// Event
class ProcStatus;
class RichRecStatus;
#include "Event/RichRecTrack.h"
#include "Event/RichRecSegment.h"
#include "Event/RichRecPixel.h"
#include "Event/RichRecPhoton.h"

// Interfaces
#include "RichRecBase/IRichSegmentCreator.h"
#include "RichRecBase/IRichTrackCreator.h"
#include "RichRecBase/IRichPhotonCreator.h"
#include "RichRecBase/IRichPixelCreator.h"
#include "RichRecBase/IRichStatusCreator.h"

/** @class RichRecAlgBase RichRecAlgBase.h RichRecBase/RichRecAlgBase.h
 *
 *  Abstract base class for RICH reconstruction algorithms providing
 *  some basic functionality.
 *
 *  @author Chris Jones
 *  @date   05/04/2002
 */

class RichRecAlgBase : public RichAlgBase {

public:

  /// Standard constructor
  RichRecAlgBase( const std::string& name,
                  ISvcLocator* pSvcLocator );

  virtual ~RichRecAlgBase() = 0;   ///< Destructor

  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode finalize  (); ///< Algorithm finalization

protected:  // Protected methods

  // Read/write access to data

  /// Returns RichRecTracks pointer
  inline RichRecTracks * richTracks()
  {
    return trackCreator()->richTracks();
  }

  /// Returns RichRecPixels pointer
  inline RichRecPixels * richPixels()
  {
    return pixelCreator()->richPixels();
  }

  /// Returns RichRecSegments pointer
  inline RichRecSegments * richSegments()
  {
    return segmentCreator()->richSegments();
  }

  /// Returns RichRecPhotons pointer
  inline RichRecPhotons * richPhotons()
  {
    return photonCreator()->richPhotons();
  }

  /// Returns RichRecStatus pointer
  inline RichRecStatus * richStatus()
  {
    return statusCreator()->richStatus();
  }

  // read only access to data

  /// Returns RichRecTracks pointer
  inline const RichRecTracks * richTracks() const
  {
    return trackCreator()->richTracks();
  }

  /// Returns RichRecPixels pointer
  inline const RichRecPixels * richPixels() const
  {
    return pixelCreator()->richPixels();
  }

  /// Returns RichRecSegments pointer
  inline const RichRecSegments * richSegments() const
  {
    return segmentCreator()->richSegments();
  }

  /// Returns RichRecPhotons pointer
  inline const RichRecPhotons * richPhotons() const
  {
    return photonCreator()->richPhotons();
  }

  /// Returns RichRecStatus pointer
  inline const RichRecStatus * richStatus() const
  {
    return statusCreator()->richStatus();
  }

  // Pointers to data object creator tools

  /// Returns pointer to RichRecTrack creator tool
  inline const IRichTrackCreator * trackCreator() const
  {
    return m_tkTool;
  }

  /// Returns pointer to RichRecSegment creator tool
  inline const IRichSegmentCreator * segmentCreator() const
  {
    return m_segTool;
  }

  /// Returns pointer to RichRecPixel creator tool
  inline const IRichPixelCreator * pixelCreator() const
  {
    return m_pixTool;
  }

  /// Returns pointer to RichRecPhoton creator tool
  inline const IRichPhotonCreator * photonCreator() const
  {
    return m_photTool;
  }

  /// Returns pointer to RichRecStatus creator tool
  inline const IRichStatusCreator * statusCreator() const
  {
    return m_statTool;
  }

private:   // Private data

  // pointers to too interfaces
  IRichPixelCreator * m_pixTool;
  IRichTrackCreator * m_tkTool;
  IRichStatusCreator * m_statTool;
  IRichSegmentCreator * m_segTool;
  IRichPhotonCreator * m_photTool;

};

#endif // RICHRECALGS_RICHRECALGBASE_H
