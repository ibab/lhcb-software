
/** @file MCRichHitToMCRichOpPhotAlg.h
 *
 *  Header file for algorithm : MCRichHitToMCRichOpPhotAlg
 *
 *  CVS Log :-
 *  $Id: MCRichHitToMCRichOpPhotAlg.h,v 1.2 2006-01-23 13:52:07 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-02-11
 */

#ifndef RICHMCTOOLS_MCRICHHITTOMCRICHOPPHOTALG_H
#define RICHMCTOOLS_MCRICHHITTOMCRICHOPPHOTALG_H  1

// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

// Linker
#include "Linker/LinkerWithKey.h"

// Event model
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCParticle.h"

// base class
#include "RichKernel/RichAlgBase.h"

/** @class MCRichHitToMCRichOpPhotAlg MCRichHitToMCRichOpPhotAlg.h
 *
 *  Builds the association tables between MCRichHits and MCRichOpticalPhotons
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-02-11
 */

class MCRichHitToMCRichOpPhotAlg : public RichAlgBase 
{

public:

  /// Standard constructor
  MCRichHitToMCRichOpPhotAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCRichHitToMCRichOpPhotAlg( ); ///< Destructor

  // Algorithm initialization
  virtual StatusCode initialize();

  // Algorithm execution
  virtual StatusCode execute   ();

  // Algorithm finalization
  virtual StatusCode finalize  ();

private: // definitions

  /// Shortcut to linker name
  typedef LinkerWithKey<LHCb::MCRichOpticalPhoton,LHCb::MCRichHit> MCRichHitsToPhotons;

private: // methods

  /// Add a given event to the linker object
  StatusCode addEvent( MCRichHitsToPhotons & links, const std::string & evtLoc );

private: // data

  /// typedef of event locations to process
  typedef std::vector<std::string> EventList;
  /// event locations to process
  EventList m_evtLocs;

};

#endif // RICHMCTOOLS_MCRICHHITTOMCRICHOPPHOTALG_H
