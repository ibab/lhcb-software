
//-----------------------------------------------------------------------------
/** @file MCPartToMCRichTrackAlg.h
 *
 *  Header file for algorithm : MCPartToMCRichTrackAlg
 *
 *  CVS Log :-
 *  $Id: MCPartToMCRichTrackAlg.h,v 1.3 2006-02-13 08:40:17 jonrob Exp $
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
#include "Linker/LinkerTool.h"
#include "Linker/LinkedTo.h"

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

  /// typedef for building linker
  typedef LinkerWithKey<LHCb::MCRichTrack,LHCb::MCParticle> MCPartToRichTracks;

private: // methods

  /// Build the linker object for all MCRichTracks
  StatusCode buildLinks();

  /// Test linker object for all MCRichTracks
  StatusCode testLinks();

  /// Add a given event to the linker object
  StatusCode addEvent( MCPartToRichTracks & links, const std::string & evtLoc );

private: // data

  /// typedef for List of event locations to process
  typedef std::vector<std::string> EventList;

  /// List of event locations to process
  EventList m_evtLocs;

  /// Flag to turn on/off testing of linker
  bool m_testLinker;

};

#endif // RICHMCASSOCIATORS_MCPARTTOMCRICHTRACKALG_H
