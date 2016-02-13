// $Id: HltTrackFit.cpp,v 1.2 2010-03-27 20:06:23 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

//from Event
#include "Event/Track.h"

// local
#include "HltTrackFit.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltTrackFit
//
// 2008-10-31 : Johannes Albrecht
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltTrackFit )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltTrackFit::HltTrackFit( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : base_class ( type, name , parent )
{
  declareInterface<ITracksFromTrack>(this);
  declareProperty("FitterName", m_fitName = "TrackMasterFitter/Fit");
}

//=============================================================================
StatusCode HltTrackFit::initialize() 
{
  StatusCode sc = GaudiTool::initialize();
  if (sc.isSuccess()) m_fit = tool<ITrackFitter>(m_fitName, this);
  return sc;
}

//=============================================================================
StatusCode HltTrackFit::tracksFromTrack( const LHCb::Track& seed,
		                                 std::vector<LHCb::Track*>& tracks ) const
{
  std::unique_ptr<LHCb::Track> tr( seed.clone() );
  StatusCode sc = m_fit->fit( *tr );
  if( sc.isFailure() ) {
     warning() << "Fit failed." << endmsg;
  } else {
     tracks.push_back( tr.release() );
  }
  return sc;
}
