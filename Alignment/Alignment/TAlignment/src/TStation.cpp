//-----------------------------------------------------------------------------
/** @file TStation.cpp
 *
 *  Implementation file for RICH reconstruction tool : TStation
 *
 *  CVS Log :-
 *  $Id: TStation.cpp,v 1.1.1.1 2006-09-29 10:09:56 jblouw Exp $
 *
 *  @author M.Needham Matt.Needham@cern.ch
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/12/2005
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/AlgFactory.h"

#include "GaudiKernel/SystemOfUnits.h"

#include "TStation.h"

using namespace LHCb;

DECLARE_ALGORITHM_FACTORY( TStation );

//-----------------------------------------------------------------------------

TStation::TStation( const std::string& name,
                              ISvcLocator* pSvcLocator ):
  GaudiTupleAlg ( name , pSvcLocator )
{



  // define track containers
  declareProperty("InputContainer", m_inputcontainer = TrackLocation::Default );
  declareProperty("OTTrackContainer", m_OTTrackLocation = "Alignment/OTTracks" );
  declareProperty("ITTrackContainer", m_ITTrackLocation = "Alignment/ITTracks" );
  declareProperty("CrossedTrackContainer", m_XTrackLocation = "Alignment/XTracks");

  
}

TStation::~TStation() { 
        info() << "TStation destructor" << endreq;
}

StatusCode TStation::initialize()
{
  info() << "Initialising TStation alignment..." << endreq;
  const StatusCode sc = GaudiTupleAlg::initialize();
  if ( sc.isFailure() ) return sc;

  // Acquire the track selection tool
  m_trackselection = tool<IATrackSelectorTool>( "ATrackSelector");

  //  ITTrackContainer = getOrCreate<Tracks,Tracks>(m_ITTrackLocation);
  //  OTTrackContainer = getOrCreate<Tracks,Tracks>(m_OTTrackLocation);
  //  XTrackContainer = getOrCreate<Tracks,Tracks>(m_XTrackLocation);

  return sc;
}

StatusCode TStation::execute() {

  debug() << "Into Tstation execute (1)" << endreq;
  
  ITTracks = new Tracks();
  OTTracks = new Tracks();
  XTracks = new Tracks();
  Tracks* inCont = get<Tracks>(m_inputcontainer);
  if ( ITTracks->size() > 0 ) ITTracks->clear();
  if ( OTTracks->size() > 0 ) OTTracks->clear();
  if ( XTracks->size() > 0 ) XTracks->clear();
  
  for ( Tracks::iterator iterT = inCont->begin();
	iterT != inCont->end(); iterT++) {
    //    info() << "Into Tstation execute (3)" << endreq;
    Track* aTrack = *iterT;
    //    info() << "Into Tstation execute (4)" << endreq;
    int OThits = 0, IThits = 0;
    //    info() << "Into Tstation execute (5)" << endreq;
    if ( m_trackselection->accept( *aTrack ) ) {
      //      info() << "Into Tstation execute (6)" << endreq;
      int hot = 0;
      hot = m_trackselection->traversesIT( *aTrack, OThits, IThits );
      
      //      info() << "Into TStation execute(): number of hits on track  = " << hot << endreq;
      if ( hot > 0 ) {
	//	Track *ITTrack = aTrack->clone();
	//fill ITtrack container
	//        info() << "Filling ITTracks..." << endreq;
        ITTracks->insert( aTrack->clone() );
	
      } else if ( hot < 0 ) {
	//	Track *OTTrack = aTrack->clone();
	// fill OTtrack container
	//        info() << "Filling OTTracks..." << endreq;
        OTTracks->insert( aTrack->clone() );
      } else {
	//	Track *XTrack = aTrack->clone();
	//        info() << "Filling XTTracks..." << endreq;
        XTracks->insert( aTrack->clone() );
      }
    }
  }
  debug() << "Into TStation execute(): storing tracks..." <<endreq;
  debug() << "Size of ITTrack container: " << (*ITTracks).size() << endreq;
  put ( ITTracks, m_ITTrackLocation  );
  debug() << "Size of OTTrack container: " << (*OTTracks).size() << endreq;
  put ( OTTracks, m_OTTrackLocation  );
  debug() << "Size of XTrack container: " << (*XTracks).size() << endreq;
  put ( XTracks, m_XTrackLocation  );
  //  delete OTTracks;
  //  delete ITTracks;
  //  delete XTracks;
  return StatusCode::SUCCESS;
}

StatusCode TStation::finalize(){
  debug() << "==> Finalize" << endmsg;
  return GaudiTupleAlg::finalize();
}
