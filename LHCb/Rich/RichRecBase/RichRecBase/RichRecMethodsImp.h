
//-----------------------------------------------------------------------------
/** @file RichRecMethodsImp.h
 *
 * Header file containing implementations of common methods for
 * RichRecAlg and RichRecTool like base classes
 *
 * CVS Log :-
 * $Id: RichRecMethodsImp.h,v 1.1 2005-06-23 15:13:05 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2005-06-21
 */
//-----------------------------------------------------------------------------

protected:

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

/** Access the geometrical efficiency tool nickname
 *
 *  @return geometrical efficiency tool nickname
 */
inline const std::string & geomEffNickName() const
{
  return m_geomEffName;
}

/** Access the expected track signal tool nickname
 *
 *  @return the expected track signal tool nickname
 */
inline const std::string & expTrackSignalNickName() const
{
  return m_expectedTkSigName;
}

/** Access the expected photon signal tool nickname
 *
 *  @return the expected photon signal tool nickname
 */
inline const std::string & expPhotonSignalNickName() const
{
  return m_expectedPhotSigName;
}

/** Access the Cherenkov angle calculator tool nickname
 *
 *  @return the Cherenkov angle calculator tool nickname
 */
inline const std::string & ckAngleNickName() const
{
  return m_ckAngleNickName;
}

/** Access the Cherenkov angle resolution tool nickname
 *
 *  @return the Cherenkov angle resolution tool nickname
 */
inline const std::string & ckAngleResolutionNickName() const
{
  return m_ckAngleResNickName;
}

/** Access the geometry tool nickname
 *
 *  @return the geometry tool nickname
 */
inline const std::string & geometryNickName() const
{
  return m_geometryNickName;
}

/** Access the processing stage
 *
 *  @return the processing phase
 */
inline const std::string & processingStage() const
{
  return m_procStage;
}

public:

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

/** Read/write access to the RichRecStatus for the current event
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

/** Access to the RichRecTrack creator tool
 *
 *  @return Pointer to the RichRecTrack creator interface
 */
inline const IRichTrackCreator * trackCreator() const
{
  if ( !m_tkTool ) acquireTool(trackCreatorNickName(), m_tkTool);
  return m_tkTool;
}

/** Access to the RichRecSegment creator tool
 *
 *  @return Pointer to the RichRecSegment creator interface
 */
inline const IRichSegmentCreator * segmentCreator() const
{
  if ( !m_segTool ) acquireTool(segmentCreatorNickName(), m_segTool);
  return m_segTool;
}

/** Access to the RichRecPixel creator tool
 *
 *  @return Pointer to the RichRecPixel creator interface
 */
inline const IRichPixelCreator * pixelCreator() const
{
  if ( !m_pixTool ) acquireTool(pixelCreatorNickName(), m_pixTool);
  return m_pixTool;
}

/** Access to the RichRecPhoton creator tool
 *
 *  @return Pointer to the RichRecPhoton creator interface
 */
inline const IRichPhotonCreator * photonCreator() const
{
  if ( !m_photTool ) acquireTool(photonCreatorNickName(), m_photTool);
  return m_photTool;
}

/** Access to the RichRecStatus creator tool
 *
 *  @return Pointer to the RichRecStatus creator interface
 */
inline const IRichStatusCreator * statusCreator() const
{
  if ( !m_statTool ) acquireTool(statusCreatorNickName(), m_statTool);
  return m_statTool;
}

/** Access to the Rich Cherenkov Angle calculator tool
 *
 *  @return Pointer to the Rich Cherenkov Angle calculator interface
 */
inline const IRichCherenkovAngle * cherenkovAngleTool() const
{
  if ( !m_ckAngleTool ) acquireTool( ckAngleNickName(), m_ckAngleTool );
  return m_ckAngleTool;
}

/** Access to the expected track signal tool
 *
 *  @return Pointer to the Rich expected track signal tool
 */
inline const IRichExpectedTrackSignal * expTrackSignalTool() const
{
  if ( !m_expTkSigTool ) acquireTool( expTrackSignalNickName(), m_expTkSigTool );
  return m_expTkSigTool;
}

/** Access to the expected photon signal tool
 *
 *  @return Pointer to the Rich expected photon signal tool
 */
inline const IRichPhotonSignal * expPhotonSignalTool() const
{
  if ( !m_exPhotSigTool ) acquireTool( expPhotonSignalNickName(), m_exPhotSigTool );
  return m_exPhotSigTool;
}

/** Access to the Rich Cherenkov Angle resolution tool
 *
 *  @return Pointer to the Rich Cherenkov Angle resolution interface
 */
inline const IRichCherenkovResolution * cherenkovAngleResolutionTool() const
{
  if ( !m_ckAngleResTool ) acquireTool( ckAngleResolutionNickName(), m_ckAngleResTool );
  return m_ckAngleResTool;
}

/** Access to the geometrical efficiency tool
 *
 *  @return Pointer to the geometrical efficiency tool
 */
inline const IRichGeomEff * geomEffTool() const
{
  if ( !m_geomEffTool ) acquireTool( geomEffNickName(), m_geomEffTool );
  return m_geomEffTool; 
}

/** Access to the geometry tool
 *
 *  @return Pointer to the geometry tool
 */
inline const IRichRecGeomTool * geometryTool() const
{
  if ( !m_geometryTool ) acquireTool( geometryNickName(), m_geometryTool );
  return m_geometryTool; 
}
