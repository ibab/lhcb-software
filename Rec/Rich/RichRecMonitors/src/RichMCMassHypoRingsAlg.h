
/** @file RichMCMassHypoRingsAlg.h
 *
 *  Header file for algorithm class : RichMCMassHypoRingsAlg
 *
 *  CVS Log :-
 *  $Id: RichMCMassHypoRingsAlg.h,v 1.2 2005-06-23 15:14:55 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */

#ifndef RICHRECMONITOR_RICHMCMASSHYPORINGSALG_H
#define RICHRECMONITOR_RICHMCMASSHYPORINGSALG_H 1

// Base class
#include "RichRecBase/RichRecAlgBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Event
#include "Event/MCRichSegment.h"
#include "Event/RichRecRing.h"

// RichKernel
#include "RichKernel/RichHashMap.h"
#include "RichKernel/StringHashFuncs.h"

// Interfaces
#include "RichKernel/IRichMCTruthTool.h"
#include "RichKernel/IRichMCTrackInfoTool.h"
#include "RichRecBase/IRichMassHypothesisRingCreator.h"
#include "RichRecBase/IRichRayTraceCherenkovCone.h"

/** @class RichMCMassHypoRingsAlg RichMCMassHypoRingsAlg.h
 *
 *  Builds RichRecRing objects representing the true Cherenkov rings, as
 *  determined from the Monte Carlo MCRichSegment objects.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */

class RichMCMassHypoRingsAlg : public RichRecAlgBase {

public:

  /// Standard constructor
  RichMCMassHypoRingsAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichMCMassHypoRingsAlg();   ///< Destructor

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // methods

  /// Build the MC rings for a given event location
  StatusCode buildRings( const std::string & evtLoc ) const;

  /// Returns the Ring creator for a given location
  const IRichMassHypothesisRingCreator * ringCreator( const std::string & loc ) const;

  /// Returns the cherenkov angle for a given MCRichSegment
  double ckTheta( const MCRichSegment * segment ) const;

private: // Private data members

  /// typedef to a list of event locations to process
  typedef std::vector<std::string> EventList;

  /// List of event locations to process
  EventList m_evtLocs;

  /** typedef to a map translating an event location into a particular
   *  Mass Hypothesis ring creator */
  typedef RichHashMap<std::string,const IRichMassHypothesisRingCreator*> RingCreators;

  /// The Mass hypothesis ring creators
  mutable RingCreators m_ringCrs;

  const IRichMCTruthTool * m_truth;              ///< MC truth tool
  const IRichMCTrackInfoTool * m_mcTkInfo;       ///< MC Track information
  const IRichRayTraceCherenkovCone * m_rayTrace; ///< Ray tracing

  /// Max Cherenkov theta angle
  std::vector<double> m_maxCKtheta;

  /// Min Cherenkov theta angle
  std::vector<double> m_minCKtheta;

  /// Flags for which radiators to create rings for
  std::vector<bool> m_usedRads;

};

inline const IRichMassHypothesisRingCreator *
RichMCMassHypoRingsAlg::ringCreator( const std::string & loc ) const
{
  const IRichMassHypothesisRingCreator *& tool = m_ringCrs[loc];
  if (!tool) 
  {
    const int slash = loc.find_first_of( "/" );
    const std::string toolName =
      "MCCherenkovRings" + ( slash > 0 ? loc.substr(0,slash) : loc );
    acquireTool( toolName, tool );
  }
  return tool;
}

#endif // RICHRECMONITOR_RICHMCMASSHYPORINGSALG_H
