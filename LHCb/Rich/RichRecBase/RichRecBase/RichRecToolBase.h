// $Id: RichRecToolBase.h,v 1.10 2004-02-02 14:23:04 jonesc Exp $
#ifndef RICHRECALGS_RICHRECTOOLBASE_H
#define RICHRECALGS_RICHRECTOOLBASE_H 1

// Base class
#include "RichUtils/RichToolBase.h"

// Interfaces
#include "RichRecBase/IRichSegmentCreator.h"
#include "RichRecBase/IRichTrackCreator.h"
#include "RichRecBase/IRichPhotonCreator.h"
#include "RichRecBase/IRichPixelCreator.h"
#include "RichRecBase/IRichStatusCreator.h"

/** @class RichRecToolBase RichRecToolBase.h RichRecBase/RichRecToolBase.h
 *
 *  Abstract base class for RICH reconstruction tools providing
 *  some basic functionality.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-26
 */

class RichRecToolBase : public RichToolBase {

public:

  /// Standard constructor
  RichRecToolBase( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

  /// Destructor
  virtual ~RichRecToolBase() {};

  /// Initialize method
  virtual StatusCode initialize();

  /// Finalize method
  virtual StatusCode finalize();

protected:   // Protected methods

  // Pointers to data object creator tools

  /// Returns pointer to RichRecTrack creator tool
  inline const IRichTrackCreator * trackCreator() const
  {
    if ( !m_tkTool ) acquireTool("RichTrackCreator", m_tkTool);
    return m_tkTool;
  }

  /// Returns pointer to RichRecSegment creator tool
  inline const IRichSegmentCreator * segmentCreator() const
  {
    if ( !m_segTool ) acquireTool("RichSegmentCreator", m_segTool);
    return m_segTool;
  }

  /// Returns pointer to RichRecPixel creator tool
  inline const IRichPixelCreator * pixelCreator() const
  {
    if ( !m_pixTool ) acquireTool("RichPixelCreator", m_pixTool);
    return m_pixTool;
  }

  /// Returns pointer to RichRecPhoton creator tool
  inline const IRichPhotonCreator * photonCreator() const
  {
    if ( !m_photTool ) acquireTool("RichPhotonCreator", m_photTool);
    return m_photTool;
  }

  /// Returns pointer to RichRecStatus creator tool
  inline const IRichStatusCreator * statusCreator() const
  {
    if ( !m_statTool ) acquireTool("RichStatusCreator", m_statTool);
    return m_statTool;
  }

private:   // Private data

  mutable IRichPixelCreator * m_pixTool;
  mutable IRichTrackCreator * m_tkTool;
  mutable IRichStatusCreator * m_statTool;
  mutable IRichSegmentCreator * m_segTool;
  mutable IRichPhotonCreator * m_photTool;

};

#endif // RICHRECALGS_RICHRECTOOLBASE_H
