// $Id: RichRecAlgBase.h,v 1.3 2003-06-30 15:11:56 jonrob Exp $
#ifndef RICHRECALGS_RICHRECALGBASE_H
#define RICHRECALGS_RICHRECALGBASE_H 1

// from GaudiAlg
//#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/IAlgTool.h"

// Interfaces
#include "RichRecBase/IRichToolRegistry.h"

// Event
class ProcStatus;
class RichRecStatus;
#include "Event/RichRecTrack.h"
#include "Event/RichRecSegment.h"
#include "Event/RichRecPixel.h"
#include "Event/RichRecPhoton.h"

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

  // Miscellaneous data
  TrStoredTracks * trTracks();     ///< Returns TrStoredTracks pointer
  ProcStatus * procStatus();       ///< Returns ProcStatus pointer

  /// Test printout level
  bool msgLevel( int mLevel );

  /// Return message level setting
  int msgLevel();

  /// Acquire a tool from the RichToolRegistry
  template <typename TOOL> inline
  TOOL* acquireTool( std::string name, TOOL*& pTool) {
    m_toolList[name] = true;
    return (pTool=dynamic_cast<TOOL*>(m_toolReg->acquireTool(name)));
  }
  
  /// Release a tool
  void releaseTool( std::string name );

private:  // private methods

  RichRecStatus * updateRichStatus();    ///< Updates RichRecStatus pointer
  ProcStatus * updateProcStatus();       ///< Updates ProcStatus pointer
  TrStoredTracks * updateTrTracks();     ///< Updates TrStoredTracks pointer

private:   // Private data

  /// Pointer to tool registry
  IRichToolRegistry * m_toolReg;
  
  ///< pointer to ProcStatus
  ProcStatus * m_procStat;
  
  /// Pointer to RichRecTracks
  RichRecTracks ** m_richTracks;
  
  /// Pointer to RichRecPixels
  RichRecPixels ** m_richPixels;
  
  /// Pointer to RichRecSegments
  RichRecSegments ** m_richSegments;
  
  /// Pointer to RichRecPhotons
  RichRecPhotons ** m_richPhotons;
  
  /// Pointer to RichRecStatus
  RichRecStatus * m_richStatus;
  
  /// Handle object for TrStoredTracks
  TrStoredTracks * m_trTracks;

  /// Location of processing status object in TES
  std::string m_procStatLocation;

  /// Location of TrStoredTracks in TES
  std::string m_trTracksLocation;

  /// Location of RichRecStatus in TES
  std::string m_richRecStatusLocation;

  /// Message service printout level
  int m_msgLevel;

  /// Vector of tool names currently in use
  typedef std::map<std::string,bool> ToolList;
  ToolList m_toolList;

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

inline TrStoredTracks * RichRecAlgBase::trTracks()
{
  return updateTrTracks();
}

inline RichRecStatus * RichRecAlgBase::richStatus()
{
  return updateRichStatus();
}

inline bool RichRecAlgBase::msgLevel( int mLevel )
{
  return ( m_msgLevel && m_msgLevel <= mLevel );
}

inline int RichRecAlgBase::msgLevel()
{
  return m_msgLevel;
}

inline ProcStatus * RichRecAlgBase::procStatus()
{
  return ( updateProcStatus() );
}

inline void RichRecAlgBase::releaseTool( std::string name ) {
  m_toolList[name] = false;
  m_toolReg->releaseTool(name);
}

#endif // RICHRECALGS_RICHRECALGBASE_H
