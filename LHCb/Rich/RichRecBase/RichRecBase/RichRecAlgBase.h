// $Id: RichRecAlgBase.h,v 1.8 2003-08-06 09:55:31 jonrob Exp $
#ifndef RICHRECALGS_RICHRECALGBASE_H
#define RICHRECALGS_RICHRECALGBASE_H 1

// from Gaudi
//#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/GaudiException.h"

// Event
class ProcStatus;
class RichRecStatus;
#include "Event/RichRecTrack.h"
#include "Event/RichRecSegment.h"
#include "Event/RichRecPixel.h"
#include "Event/RichRecPhoton.h"

// Interfaces
#include "RichRecBase/IRichToolRegistry.h"
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

class RichRecAlgBase : public Algorithm {

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

  /// Test printout level
  bool msgLevel( int mLevel );

  /// Return message level setting
  int msgLevel();

  /// Acquires a tool of the correct type from the RichToolRegistry for a given name
  template <typename TOOL> inline
  TOOL* acquireTool( std::string tName, TOOL*& pTool ) {
    if ( !m_toolReg ) throw GaudiException( "RichToolRegistry not initialised", 
                                            name(), StatusCode::FAILURE );
    if ( !toolSvc()->retrieveTool(m_toolReg->toolType(tName),tName,pTool) ) {
      pTool = NULL;
      throw GaudiException( "Unable to retrieve tool '" + tName +
                            "' of type '" + m_toolReg->toolType(tName) + "'" ,
                            name(), StatusCode::FAILURE );
    }
    return pTool;
  }
  
  /// Release a tool
  template <typename TOOL> inline
  void releaseTool( TOOL *& pTool ) {
    if ( pTool ) { toolSvc()->releaseTool( pTool ); pTool = NULL; }
  }

private:   // Private data

  /// Pointer to tool registry
  IRichToolRegistry * m_toolReg;

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

  /// Location of RichRecStatus in TES
  std::string m_richRecStatusLocation;

  /// Runtime name for RichToolRegistry
  std::string m_regName;

  /// Message service printout level
  int m_msgLevel;

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

inline bool RichRecAlgBase::msgLevel( int mLevel )
{
  return ( m_msgLevel && m_msgLevel <= mLevel );
}

inline int RichRecAlgBase::msgLevel()
{
  return m_msgLevel;
}

#endif // RICHRECALGS_RICHRECALGBASE_H
