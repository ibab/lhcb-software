// $Id: RichRecAlgBase.h,v 1.10 2003-11-02 21:46:02 jonrob Exp $
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

  // Get Rich Reconstruction data
  RichRecTracks * richTracks();     ///< Returns RichRecTracks pointer
  RichRecPixels * richPixels();     ///< Returns RichRecPixels pointer
  RichRecSegments * richSegments(); ///< Returns RichRecSegments pointer
  RichRecPhotons * richPhotons();   ///< Returns RichRecPhotons pointer
  RichRecStatus * richStatus();     ///< Returns RichRecStatus pointer
 
private:   // Private data

  /// Pointer to RichRecTracks
  RichRecTracks ** m_richTracks;

  /// Pointer to RichRecPixels
  RichRecPixels ** m_richPixels;

  /// Pointer to RichRecSegments
  RichRecSegments ** m_richSegments;

  /// Pointer to RichRecPhotons
  RichRecPhotons ** m_richPhotons;

  /// Pointer to RichRecStatus
  RichRecStatus ** m_richStatus;

  IRichPixelCreator * m_pixTool;
  IRichTrackCreator * m_tkTool;
  IRichStatusCreator * m_statTool;
  IRichSegmentCreator * m_segTool;
  IRichPhotonCreator * m_photTool;

};

inline RichRecTracks * RichRecAlgBase::richTracks()
{
  return *m_richTracks;
}

inline RichRecPixels * RichRecAlgBase::richPixels()
{
  return *m_richPixels;
}

inline RichRecSegments * RichRecAlgBase::richSegments()
{
  return *m_richSegments;
}

inline RichRecPhotons * RichRecAlgBase::richPhotons()
{
  return *m_richPhotons;
}

inline RichRecStatus * RichRecAlgBase::richStatus()
{
  return *m_richStatus;
}

#endif // RICHRECALGS_RICHRECALGBASE_H
