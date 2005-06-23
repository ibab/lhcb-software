
/** @file MCPartToMCRichTrackAlg.h
 *
 *  Header file for algorithm : MCPartToMCRichTrackAlg
 *
 *  CVS Log :-
 *  $Id: MCPartToMCRichTrackAlg.h,v 1.4 2005-06-23 15:08:50 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-02-11
 */

#ifndef RICHMCTOOLS_MCPARTTOMCRICHTRACKALG_H 
#define RICHMCTOOLS_MCPARTTOMCRICHTRACKALG_H 1

// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"

// Linker
#include "Linker/LinkerWithKey.h"

// Event model
#include "Event/MCRichTrack.h"
#include "Event/MCParticle.h"

// base class
#include "RichKernel/RichAlgBase.h"

/** @class MCPartToMCRichTrackAlg MCPartToMCRichTrackAlg.h
 *  
 *  Builds the association tables between MCParticles and MCRichTracks
 * 
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-02-11
 */

class MCPartToMCRichTrackAlg : public RichAlgBase {

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
  typedef LinkerWithKey<MCRichTrack,MCParticle> MCPartToRichTracks;

private: // methods

  /// Add a given event to the linker object
  StatusCode addEvent( MCPartToRichTracks & links, const std::string & evtLoc );

private: // data

  typedef std::vector<std::string> EventList;
  EventList m_evtLocs;

};
#endif // RICHMCTOOLS_MCPARTTOMCRICHTRACKALG_H
