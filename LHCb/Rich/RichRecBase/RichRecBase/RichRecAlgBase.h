
//-----------------------------------------------------------------------------
/** @file RichRecAlgBase.h
 *
 *  Header file for reconstruction algorithm base class : RichRecAlgBase
 *
 *  CVS Log :-
 *  $Id: RichRecAlgBase.h,v 1.18 2005-02-02 10:01:20 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECALGS_RICHRECALGBASE_H
#define RICHRECALGS_RICHRECALGBASE_H 1

// base class
#include "RichKernel/RichAlgBase.h"

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
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */

class RichRecAlgBase : public RichAlgBase {

public:

  /// Standard constructor
  RichRecAlgBase( const std::string& name,
                  ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~RichRecAlgBase() = 0;

  /** Initialization of the algorithm after creation
   *
   * @return The status of the initialization
   * @retval StatusCode::SUCCESS Initialization was successful
   * @retval StatusCode::FAILURE Initialization failed
   */
  virtual StatusCode initialize();

  /** The main event processing method. Called once for each event
   *
   * @return The status of the event processing
   * @retval StatusCode::SUCCESS Event processing was successful
   * @retval StatusCode::FAILURE Event processing failed
   */
  virtual StatusCode execute();

  /** Finalization of the algorithm before deletion
   *
   * @return The status of the finalization
   * @retval StatusCode::SUCCESS Finalization was successful
   * @retval StatusCode::FAILURE Finalization failed
   */
  virtual StatusCode finalize();

protected:  // Protected methods

  // Read/write access to data

  /** Read/write access to the RichRecTracks for the current event
   *
   * @return Pointer to the RichRecTracks container.
   * Internal consistency checking guarantees the data to be valid
   */
  inline RichRecTracks * richTracks()
  {
    return trackCreator()->richTracks();
  }

  /** Read/write access to the RichRecPixels for the current event
   *
   * @return Pointer to the RichRecPixels container.
   * Internal consistency checking guarantees the data to be valid
   */
  inline RichRecPixels * richPixels()
  {
    return pixelCreator()->richPixels();
  }

  /** Read/write access to the RichRecSegments for the current event
   *
   * @return Pointer to the RichRecSegments container.
   * Internal consistency checking guarantees the data to be valid
   */
  inline RichRecSegments * richSegments()
  {
    return segmentCreator()->richSegments();
  }

  /** Read/write access to the RichRecPhotons for the current event
   *
   * @return Pointer to the RichRecPhotons container.
   * Internal consistency checking guarantees the data to be valid
   */
  inline RichRecPhotons * richPhotons()
  {
    return photonCreator()->richPhotons();
  }

  /** Read/write access to the RichRecPhotons for the current event
   *
   * @return Pointer to the RichRecStatus data object.
   * Internal consistency checking guarantees the data to be valid
   */
  inline RichRecStatus * richStatus()
  {
    return statusCreator()->richStatus();
  }

  // read only access to data

  /** Read access to the RichRecTracks for the current event
   *
   * @return const Pointer to the RichRecTracks container.
   * Internal consistency checking guarantees the data to be valid
   */
  inline const RichRecTracks * richTracks() const
  {
    return trackCreator()->richTracks();
  }

  /** Read access to the RichRecPixels for the current event
   *
   * @return const Pointer to the RichRecPixels container.
   * Internal consistency checking guarantees the data to be valid
   */
  inline const RichRecPixels * richPixels() const
  {
    return pixelCreator()->richPixels();
  }

  /** Read access to the RichRecSegments for the current event
   *
   * @return const Pointer to the RichRecSegments container.
   * Internal consistency checking guarantees the data to be valid
   */
  inline const RichRecSegments * richSegments() const
  {
    return segmentCreator()->richSegments();
  }

  /** Read access to the RichRecPhotons for the current event
   *
   * @return const Pointer to the RichRecPhotons container.
   * Internal consistency checking guarantees the data to be valid
   */
  inline const RichRecPhotons * richPhotons() const
  {
    return photonCreator()->richPhotons();
  }

  /** Read access to the RichRecStatus for the current event
   *
   * @return const Pointer to the RichRecStatus data object.
   * Internal consistency checking guarantees the data to be valid
   */
  inline const RichRecStatus * richStatus() const
  {
    return statusCreator()->richStatus();
  }

  // Pointers to data object creator tools

  /** Access to the RichRecPixel creator tool
   *
   *  @return Pointer to the RichRecPixel creator interface
   */
  inline const IRichPixelCreator * pixelCreator() const
  {
    return m_pixTool;
  }

  /** Access to the RichRecTrack creator tool
   *
   *  @return Pointer to the RichRecTrack creator interface
   */
  inline const IRichTrackCreator * trackCreator() const
  {
    return m_tkTool;
  }

  /** Access to the RichRecSegment creator tool
   *
   *  @return Pointer to the RichRecSegment creator interface
   */
  inline const IRichSegmentCreator * segmentCreator() const
  {
    return m_segTool;
  }

  /** Access to the RichRecPhoton creator tool
   *
   *  @return Pointer to the RichRecPhoton creator interface
   */
  inline const IRichPhotonCreator * photonCreator() const
  {
    return m_photTool;
  }

  /** Access to the RichRecStatus creator tool
   *
   *  @return Pointer to the RichRecStatus creator interface
   */
  inline const IRichStatusCreator * statusCreator() const
  {
    return m_statTool;
  }

private:   // Private data

  // pointers to interfaces
  mutable IRichPixelCreator *   m_pixTool;  ///< RichRecPixel Creator tool
  mutable IRichTrackCreator *   m_tkTool;   ///< RichRecTrack Creator tool
  mutable IRichSegmentCreator * m_segTool;  ///< RichRecSegment Creator tool
  mutable IRichPhotonCreator *  m_photTool; ///< RichRecPhoton Creator tool
  mutable IRichStatusCreator *  m_statTool; ///< RichRecStatus Creator tool

};

#endif // RICHRECALGS_RICHRECALGBASE_H
