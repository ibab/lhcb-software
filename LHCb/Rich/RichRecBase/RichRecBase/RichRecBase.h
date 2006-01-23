
//-----------------------------------------------------------------------------
/** @file RichRecBase.h
 *
 *  Header file for RICH reconstruction base class : RichRecBase
 *
 *  CVS Log :-
 *  $Id: RichRecBase.h,v 1.2 2006-01-23 14:08:55 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2005-08-26
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHRECBASE_H
#define RICHRECBASE_RICHRECBASE_H 1

// Event
namespace LHCb
{
  class ProcStatus;
  class RichRecStatus;
}
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

// Physical Constants
#include "Kernel/PhysicalConstants.h"

//-----------------------------------------------------------------------------
/** @class RichRecBase RichRecBase.h RichRecBase/RichRecBase.h
 *
 *  Base class containing common RICH reconstruction functionality
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-08-26
 */
//-----------------------------------------------------------------------------

template <class PBASE>
class RichRecBase
{

public:

  /// Standard constructor
  RichRecBase( PBASE * base = 0 );

  /// Destructor
  ~RichRecBase( ) { }

protected:

  /// Intialise common RICH Reco
  StatusCode initialiseRichReco();

  /// Finalise common RICH Reco
  StatusCode finaliseRichReco();

private:

  /// Const access to derived class
  inline const PBASE * base() const { return m_base; }

  /// Non-const access to derived class
  inline       PBASE * base()       { return m_base; }

protected: // tool nicknames

  /** Access the RichRecPixel creator nickname
   *
   *  @return RichRecPixel creator nickname
   */
  inline const std::string & pixelCreatorNickName() const
  {
    return m_pixelCrName;
  }

  /** Access the RichRecSegment creator nickname
   *
   *  @return RichRecSegment creator nickname
   */
  inline const std::string & segmentCreatorNickName() const
  {
    return m_segmentCrName;
  }

  /** Access the RichRecTrack creator nickname
   *
   *  @return RichRecTrack creator nickname
   */
  inline const std::string & trackCreatorNickName() const
  {
    return m_trackCrName;
  }

  /** Access the RichRecPhoton creator nickname
   *
   *  @return RichRecPhoton creator nickname
   */
  inline const std::string & photonCreatorNickName() const
  {
    return m_photonCrName;
  }

  /** Access the RichRecStatus creator nickname
   *
   *  @return RichRecStatus creator nickname
   */
  inline const std::string & statusCreatorNickName() const
  {
    return m_statusCrName;
  }

protected: // Pointers to data object creator tools

  /** Access to the RichRecTrack creator tool
   *
   *  @return Pointer to the RichRecTrack creator interface
   */
  inline const IRichTrackCreator * trackCreator() const
  {
    if ( !m_tkTool ) base()->acquireTool(trackCreatorNickName(), m_tkTool);
    return m_tkTool;
  }

  /** Access to the RichRecSegment creator tool
   *
   *  @return Pointer to the RichRecSegment creator interface
   */
  inline const IRichSegmentCreator * segmentCreator() const
  {
    if ( !m_segTool ) base()->acquireTool(segmentCreatorNickName(), m_segTool);
    return m_segTool;
  }

  /** Access to the RichRecPixel creator tool
   *
   *  @return Pointer to the RichRecPixel creator interface
   */
  inline const IRichPixelCreator * pixelCreator() const
  {
    if ( !m_pixTool ) base()->acquireTool(pixelCreatorNickName(), m_pixTool);
    return m_pixTool;
  }

  /** Access to the RichRecPhoton creator tool
   *
   *  @return Pointer to the RichRecPhoton creator interface
   */
  inline const IRichPhotonCreator * photonCreator() const
  {
    if ( !m_photTool ) base()->acquireTool(photonCreatorNickName(), m_photTool);
    return m_photTool;
  }

  /** Access to the RichRecStatus creator tool
   *
   *  @return Pointer to the RichRecStatus creator interface
   */
  inline const IRichStatusCreator * statusCreator() const
  {
    if ( !m_statTool ) base()->acquireTool(statusCreatorNickName(), m_statTool);
    return m_statTool;
  }

protected: // Read/write access to data

  /** Read/write access to the RichRecTracks for the current event
   *
   * Internal consistency checking guarantees the data to be valid
   *
   * @return Pointer to the RichRecTracks container.
   */
  inline LHCb::RichRecTracks * richTracks()
  {
    return trackCreator()->richTracks();
  }

  /** Read/write access to the RichRecPixels for the current event
   *
   * Internal consistency checking guarantees the data to be valid
   *
   * @return Pointer to the RichRecPixels container.
   */
  inline LHCb::RichRecPixels * richPixels()
  {
    return pixelCreator()->richPixels();
  }

  /** Read/write access to the RichRecSegments for the current event
   *
   * Internal consistency checking guarantees the data to be valid
   *
   * @return Pointer to the RichRecSegments container.
   */
  inline LHCb::RichRecSegments * richSegments()
  {
    return segmentCreator()->richSegments();
  }

  /** Read/write access to the RichRecPhotons for the current event
   *
   * Internal consistency checking guarantees the data to be valid
   *
   * @return Pointer to the RichRecPhotons container.
   */
  inline LHCb::RichRecPhotons * richPhotons()
  {
    return photonCreator()->richPhotons();
  }

  /** Read/write access to the RichRecStatus for the current event
   *
   * Internal consistency checking guarantees the data to be valid
   *
   * @return Pointer to the RichRecStatus data object.
   */
  inline LHCb::RichRecStatus * richStatus()
  {
    return statusCreator()->richStatus();
  }

protected: // read only access to data

  /** Read access to the RichRecTracks for the current event
   *
   * Internal consistency checking guarantees the data to be valid
   *
   * @return const Pointer to the RichRecTracks container.
   */
  inline const LHCb::RichRecTracks * richTracks() const
  {
    return trackCreator()->richTracks();
  }

  /** Read access to the RichRecPixels for the current event
   *
   * Internal consistency checking guarantees the data to be valid
   *
   * @return const Pointer to the RichRecPixels container.
   */
  inline const LHCb::RichRecPixels * richPixels() const
  {
    return pixelCreator()->richPixels();
  }

  /** Read access to the RichRecSegments for the current event
   *
   * Internal consistency checking guarantees the data to be valid
   *
   * @return const Pointer to the RichRecSegments container.
   */
  inline const LHCb::RichRecSegments * richSegments() const
  {
    return segmentCreator()->richSegments();
  }

  /** Read access to the RichRecPhotons for the current event
   *
   * Internal consistency checking guarantees the data to be valid
   *
   * @return const Pointer to the RichRecPhotons container.
   */
  inline const LHCb::RichRecPhotons * richPhotons() const
  {
    return photonCreator()->richPhotons();
  }

  /** Read access to the RichRecStatus for the current event
   *
   * Internal consistency checking guarantees the data to be valid
   *
   * @return const Pointer to the RichRecStatus data object.
   */
  inline const LHCb::RichRecStatus * richStatus() const
  {
    return statusCreator()->richStatus();
  }

private:

  PBASE * m_base; ///< Pointer to derived class

private:

  mutable const IRichPixelCreator * m_pixTool;   ///< RichRecPixel Creator tool
  mutable const IRichTrackCreator * m_tkTool;    ///< RichRecTrack Creator tool
  mutable const IRichSegmentCreator * m_segTool; ///< RichRecSegment Creator tool
  mutable const IRichPhotonCreator * m_photTool; ///< RichRecPhoton Creator tool
  mutable const IRichStatusCreator * m_statTool; ///< RichRecStatus Creator tool

  std::string m_trackCrName;      ///< Track creator nickname
  std::string m_pixelCrName;      ///< Pixel creator nickname
  std::string m_segmentCrName;    ///< Segment creator nickname
  std::string m_photonCrName;     ///< Photon creator nickname
  std::string m_statusCrName;     ///< Status creator nickname

};

#endif // RICHRECBASE_RICHRECBASE_H
