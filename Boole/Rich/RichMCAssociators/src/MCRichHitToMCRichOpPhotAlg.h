
/** @file MCRichHitToMCRichOpPhotAlg.h
 *
 *  Header file for algorithm : MCRichHitToMCRichOpPhotAlg
 *
 *  CVS Log :-
 *  $Id: MCRichHitToMCRichOpPhotAlg.h,v 1.3 2006-03-13 13:15:03 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-02-11
 */

#ifndef RICHMCASSOCIATORS_MCRICHHITTOMCRICHOPPHOTALG_H
#define RICHMCASSOCIATORS_MCRICHHITTOMCRICHOPPHOTALG_H  1

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
  StatusCode addEvent( const std::string & evtLoc );

  /// Return a pointer to the linker for this event
  MCRichHitsToPhotons * linker();

  /// Reset linker object for new event
  void resetLinker();

private: // data

  /// typedef of event locations to process
  typedef std::vector<std::string> EventList;

  /// event locations to process
  EventList m_evtLocs;

  /// Pointer to linker object
  MCRichHitsToPhotons * m_linker;

};

inline void MCRichHitToMCRichOpPhotAlg::resetLinker()
{
  if ( m_linker ) { delete m_linker; m_linker = NULL; }
}

#endif // RICHMCASSOCIATORS_MCRICHHITTOMCRICHOPPHOTALG_H
