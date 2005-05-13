
//--------------------------------------------------------------------------------------
/** @file RichPixelCreatorFromSignalRichDigits.h
 *
 *  Header file for RICH reconstruction tool : RichPixelCreatorFromSignalRichDigits
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromSignalRichDigits.h,v 1.6 2005-05-13 15:00:05 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//--------------------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RICHPIXELCREATORFROMSIGNALRICHDIGITS_H
#define RICHRECMCTOOLS_RICHPIXELCREATORFROMSIGNALRICHDIGITS_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/ToolFactory.h"

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
/** @class RichPixelCreatorFromSignalRichDigits RichPixelCreatorFromSignalRichDigits.h
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

class RichPixelCreatorFromSignalRichDigits : public RichPixelCreatorBase,
                                             virtual public IIncidentListener 
{

public: // methods for Gaudi framework

  /// Standard constructor
  RichPixelCreatorFromSignalRichDigits( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent );

  /// Destructor
  virtual ~RichPixelCreatorFromSignalRichDigits(){}

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

  /// Implement the handle method for the Incident service.
  /// This is used to inform the tool of software incidents.
  void handle( const Incident& incident );

public: // Public interface methods

  // Returns a RichRecPixel object pointer for given ContainedObject.
  // If if it not possible NULL is return.
  RichRecPixel * newPixel( const ContainedObject * obj ) const;

  // Form all possible RichRecPixels from input RichDigits.
  // The most efficient way to make all RichRecPixel objects in the event.
  StatusCode newPixels() const;

private: // methods

  /// Initialise for a new event. Re-implmented from base class version.
  void InitNewEvent();

  /// List of tracked MCParticles
  typedef RichMap < const MCParticle*, bool > TrackedMCPList;
  /// Get the map for tracked MCParticles for this event
  TrackedMCPList & trackedMCPs() const;

private: // data

  /// General MC truth tool
  IRichMCTruthTool * m_mcTool;

  /// Reconstruction MC truth tool
  IRichRecMCTruthTool * m_mcRecTool;

  /// Pointer to delegated pixel maker
  IRichPixelCreator * m_pixMaker;

  /// Nickname of RichPixel Creator to use
  std::string m_subPixelCreatorName;

  /// String containing input RichDigits location in TES
  std::string m_recoDigitsLocation;

  /** Flag to turn on/off the filtering of pixels that do not 
      associated to any reconstructed RichRecTrack */
  bool m_trackFilter;

  /// List of tracked MCParticles
  mutable TrackedMCPList m_trackedMCPs;

  // flag to indicated tracked MCParticle list has been made for current event
  mutable bool m_trackMCPsDone;

};

inline void RichPixelCreatorFromSignalRichDigits::InitNewEvent()
{
  // Initialise data for new event
  RichPixelCreatorBase::InitNewEvent();
  m_trackedMCPs.clear();
  m_trackMCPsDone = false;
}

#endif // RICHRECMCTOOLS_RICHPIXELCREATORFROMSIGNALRICHDIGITS_H
