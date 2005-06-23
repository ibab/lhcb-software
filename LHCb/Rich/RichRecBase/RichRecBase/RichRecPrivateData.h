
//-----------------------------------------------------------------------------
/** @file RichRecPrivateData.h
 *
 * Header file containing private data members for RICH reconstruction
 * Algorithm and Tool base classes
 *
 * CVS Log :-
 * $Id: RichRecPrivateData.h,v 1.1 2005-06-23 15:13:05 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2005-06-21
 */
//-----------------------------------------------------------------------------

// pointers to interfaces

// creator tools
mutable const IRichPixelCreator * m_pixTool;   ///< RichRecPixel Creator tool
mutable const IRichTrackCreator * m_tkTool;    ///< RichRecTrack Creator tool
mutable const IRichSegmentCreator * m_segTool; ///< RichRecSegment Creator tool
mutable const IRichPhotonCreator * m_photTool; ///< RichRecPhoton Creator tool
mutable const IRichStatusCreator * m_statTool; ///< RichRecStatus Creator tool

// Common reconstruction tools
mutable const IRichCherenkovAngle * m_ckAngleTool;         ///< Rich Cherenkov angle calculator tool
mutable const IRichExpectedTrackSignal * m_expTkSigTool;   ///< Expected track signal tool
mutable const IRichPhotonSignal * m_exPhotSigTool;         ///< Expected photon signal tool
mutable const IRichCherenkovResolution * m_ckAngleResTool; ///< Cherenkov angle resolution tool
mutable const IRichGeomEff * m_geomEffTool;                ///< Geometrical efficiency tool
mutable const IRichRecGeomTool * m_geometryTool;           ///< General geometry tool 

// Tool nicknames

// Creator tools
std::string m_trackCrName;      ///< Track creator nickname
std::string m_pixelCrName;      ///< Pixel creator nickname
std::string m_segmentCrName;    ///< Segment creator nickname
std::string m_photonCrName;     ///< Photon creator nickname
std::string m_statusCrName;     ///< Status creator nickname

// Commonly used reconstruction tools
std::string m_geomEffName;         ///< Geometrical efficiency tool nickname
std::string m_expectedTkSigName;   ///< Expected track photon signal tool nickname
std::string m_ckAngleNickName;     ///< Cherenkov angle calculator tool nickname
std::string m_expectedPhotSigName; ///< Expected photon signal tool nickname
std::string m_ckAngleResNickName;  ///< Cherenkov angle resolution tool nickname
std::string m_geometryNickName;    ///< Geometry tool nick name

// job options

/// The processing stage, HLT or Offline. To be be replaced by standard "context" if and when available
std::string m_procStage;
