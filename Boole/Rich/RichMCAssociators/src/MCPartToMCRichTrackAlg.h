
//-----------------------------------------------------------------------------
/** @file MCPartToMCRichTrackAlg.h
 *
 *  Header file for algorithm : MCPartToMCRichTrackAlg
 *
 *  CVS Log :-
 *  $Id: MCPartToMCRichTrackAlg.h,v 1.2 2006-01-23 13:52:07 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-02-11
 */
//-----------------------------------------------------------------------------

#ifndef RICHMCASSOCIATORS_MCPARTTOMCRICHTRACKALG_H
#define RICHMCASSOCIATORS_MCPARTTOMCRICHTRACKALG_H 1

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// Linker
#include "Linker/LinkerWithKey.h"

// Event model
#include "Event/MCRichTrack.h"
#include "Event/MCParticle.h"

// base class
#include "RichKernel/RichAlgBase.h"

//-----------------------------------------------------------------------------
/** @class MCPartToMCRichTrackAlg MCPartToMCRichTrackAlg.h
 *
 *  Builds the association tables between MCParticles and MCRichTracks
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-02-11
 */
//-----------------------------------------------------------------------------

class MCPartToMCRichTrackAlg : public RichAlgBase 
{

public:

  /// Standard constructor
  MCPartToMCRichTrackAlg( const std::string& name, ISvcLocator* pSvcLocator );

  /// Destructor
  virtual ~MCPartToMCRichTrackAlg( );

  virtual StatusCode initialize();    // Algorithm initialization
  virtual StatusCode execute   ();    // Algorithm execution
  virtual StatusCode finalize  ();    // Algorithm finalization

private: // definitions

  /// Shortcut to linker name
  typedef LinkerWithKey<LHCb::MCRichTrack,LHCb::MCParticle> MCPartToRichTracks;

private: // methods

  /// Add a given event to the linker object
  StatusCode addEvent( MCPartToRichTracks & links, const std::string & evtLoc );

private: // data

  /// typedef for List of event locations to process
  typedef std::vector<std::string> EventList;
  /// List of event locations to process
  EventList m_evtLocs;

};

#endif // RICHMCASSOCIATORS_MCPARTTOMCRICHTRACKALG_H
