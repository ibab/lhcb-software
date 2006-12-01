
//--------------------------------------------------------------------------------------
/** @file RichPixelCreatorFromSignalRawBuffer.h
 *
 *  Header file for RICH reconstruction tool : RichPixelCreatorFromSignalRawBuffer
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromSignalRawBuffer.h,v 1.3 2006-12-01 16:18:24 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//--------------------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RichPixelCreatorFromSignalRawBuffer_H
#define RICHRECMCTOOLS_RichPixelCreatorFromSignalRawBuffer_H 1

// base class
#include "RichRecBase/RichPixelCreatorBase.h"

// RichKernel
#include "RichKernel/RichMap.h"

// interfaces
#include "RichRecBase/IRichRecMCTruthTool.h"
#include "RichKernel/IRichMCTruthTool.h"

// Event
#include "Event/RichDigit.h"
#include "Event/MCRichDigit.h"
#include "Event/MCRichOpticalPhoton.h"

//--------------------------------------------------------------------------------------
/** @class RichPixelCreatorFromSignalRawBuffer RichPixelCreatorFromSignalRawBuffer.h
 *
 *  Tool for the creation and book-keeping of RichRecPixel objects.
 *  Uses RichDigits from the digitisation but then refers to the
 *  MCRichOpticalPhoton objects select on the true Cherenkov hits.
 *  Optionally, can also select only those pixels that are associated to
 *  a RichRecxTrack, so that trackless hits can be filtered out.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/09/2003
 */
//--------------------------------------------------------------------------------------

class RichPixelCreatorFromSignalRawBuffer : public RichPixelCreatorBase
{

public: // methods for Gaudi framework

  /// Standard constructor
  RichPixelCreatorFromSignalRawBuffer( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent );

  /// Destructor
  virtual ~RichPixelCreatorFromSignalRawBuffer(){}

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

protected: // methods

  /// Build a new RichRecPixel
  virtual LHCb::RichRecPixel * buildPixel ( const LHCb::RichSmartID id ) const;

  /// Initialise for a new event. Re-implmented from base class version.
  virtual void InitNewEvent();

private: // methods

  /// List of tracked MCParticles
  typedef Rich::Map < const LHCb::MCParticle*, bool > TrackedMCPList;
  /// Get the map for tracked MCParticles for this event
  TrackedMCPList & trackedMCPs() const;

private: // data

  /// General MC truth tool
  const IRichMCTruthTool * m_mcTool;

  /// Reconstruction MC truth tool
  const IRichRecMCTruthTool * m_mcRecTool;

  /** Flag to turn on/off the filtering of pixels that do not
      associated to any reconstructed RichRecTrack */
  bool m_trackFilter;

  /// Reject background hits
  bool m_rejBackHits;

  /// List of tracked MCParticles
  mutable TrackedMCPList m_trackedMCPs;

  // flag to indicated tracked MCParticle list has been made for current event
  mutable bool m_trackMCPsDone;

};

#endif // RICHRECMCTOOLS_RichPixelCreatorFromSignalRawBuffer_H
