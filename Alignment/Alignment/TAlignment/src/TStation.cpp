//-----------------------------------------------------------------------------
/** @file TStation.cpp
 *
 *  Implementation file for RICH reconstruction tool : TStation
 *
 *  CVS Log :-
 *  $Id: TStation.cpp,v 1.8 2010-04-19 09:08:43 jblouw Exp $
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
  declareProperty("OTTrackContainer", m_OTTrackLocation = "Event/Alignment/OTTracks" );
  declareProperty("ITTrackContainer", m_ITTrackLocation = "Event/Alignment/ITTracks" );
  declareProperty("CrossedTrackContainer", m_XTrackLocation = "Event/Alignment/XTracks");
  declareProperty( "MinITHitCut",  m_minITHitCut   = 0.0 );
  declareProperty( "MinOTHitCut",  m_minOTHitCut   = 0.0 );
  declareProperty( "MinTTHitCut",  m_minTTHitCut   = 0.0 );
  declareProperty( "MinVeloHitCut",  m_minVeloHitCut   = 0.0 );

  
}

TStation::~TStation() { 
        info() << "TStation destructor" << endreq;
}

StatusCode TStation::initialize() {
  info() << "Initialising TStation alignment..." << endreq;
  const StatusCode sc = GaudiTupleAlg::initialize();
  if ( sc.isFailure() ) return sc;

  // Acquire the track selection tool
  info() << "create track selection tool" << endreq;
  info() << "Using " << m_OTTrackLocation << " to select OTTracks from " << m_inputcontainer << endreq;

  m_trackselection = tool<IATrackSelectorTool>( "ATrackSelector");
  m_trackselection->initialize();

  m_trackenergy = tool<ITrackCaloMatch>( "TrackCaloMatch" );
  setHistoTopDir("OT/");
  CaloEnergy = book( "CaloEnergy", "Calorimeter energy on track", 0.0, 12000.0 );


  m_tfit = tool<ITrackFitter>("TrackInitFit");

  m_total = 0;
  IIncidentSvc* incSvc = svc<IIncidentSvc>("IncidentSvc");
  if (!incSvc) {
    return Error("==> Failed to retrieve IncidentSvc!", StatusCode::FAILURE);
  }
  incSvc->addListener(this, "ResetUniformity");
  incSvc->addListener(this, "PrintUniformTD");

  
  return sc;
}

StatusCode TStation::execute() {
  //  info() << "Into TStation::execute" << endreq;
  setHistoTopDir("OT/");
  if ( ! exist<LHCb::Tracks>( m_inputcontainer ) ) {
    error() << "Container " << m_inputcontainer << " does not exist!" << endreq;
    return StatusCode::SUCCESS;
  }
  LHCb::Tracks* inCont = get<LHCb::Tracks>(m_inputcontainer);
  ITTracks = new LHCb::Tracks();
  ITTracks->empty();
  OTTracks = new LHCb::Tracks();
  OTTracks->empty();
  XTracks = new LHCb::Tracks();
  XTracks->empty();
  //  info() << "Size of Tracks container = " << OTTracks->size() << endreq;
  if ( ITTracks->size() > 0 ) ITTracks->clear();
  if ( XTracks->size() > 0 ) XTracks->clear();
  if ( OTTracks->size() > 0 ) OTTracks->empty();
  //  info() << "Size of Tracks container = " << inCont->size() << endreq;
  int num_tracks = 0;
  for ( LHCb::Tracks::iterator iterT = inCont->begin();
 	iterT != inCont->end(); iterT++) {
    LHCb::Track* aTrack = *iterT;
    m_tfit->fit( *aTrack );
    if ( m_trackselection->accept( *aTrack ) ) {
      num_tracks++;
      double energy = m_trackenergy->energy( *aTrack );
      if ( energy > 0.0 ) 
	fill(CaloEnergy, energy, 1.0);
      debug() << "Energy = " << energy << " on track type " << aTrack->TypesToString(aTrack->type()) << endreq;
      //       int hot = 0;
      //       hot = m_trackselection->traversesIT( *aTrack, OThits, IThits );
      // //      if ( hot == 10 ) debug() << "Error: found " << OThits << " OThits on track and " << IThits << " IThits." << endreq;
      //       if ( hot > 0 && IThits >= m_minITHitCut ) {
      //         ITTracks->insert( aTrack->clone() );
      //       } else if ( hot < 0 && OThits >= m_minOTHitCut ) {
      OTTracks->insert( aTrack->clone() );
      //       } else if ( OThits+IThits > m_minOTHitCut + m_minITHitCut ) {
      //         XTracks->insert( aTrack->clone() );
      //       }
    } else {
      debug() << "Track not selected!" << " "
	      << *iterT << endreq; 
    }
  }
  debug() << "Number of usable tracks = " << num_tracks << endreq;
  m_total += num_tracks;
  //  info() << "Size of Tracks container = " << OTTracks->size() << endreq;
  if ( OTTracks->size() > 0 ) 
    put ( OTTracks, m_OTTrackLocation  );
  
  return StatusCode::SUCCESS;
}

StatusCode TStation::Reset() {
  debug() << "Resetting hit counters in OT..." << endreq;
  m_trackselection->Reset();
  return StatusCode::SUCCESS;
}

StatusCode TStation::PrintTD() {
  m_trackselection->PrintUniformTD();
  return StatusCode::SUCCESS;
}

StatusCode TStation::finalize(){
  debug() << "==> Finalize" << endmsg;
  info() << "Total number of usable tracks: " << m_total << endreq;
  //  delete ITTracks;
  //  delete OTTracks;
  //  delete XTracks;
  return GaudiTupleAlg::finalize();
}

void TStation::handle( const Incident &incident ) {
  info() << "Incident type = " << incident.type() << endreq;
  if ( "ResetUniformity" == incident.type() ) 
    this->Reset();
  if ( "PrintUniformTD" == incident.type() )
    this->PrintTD();
}

StatusCode TStation::queryInterface( const InterfaceID &id, void **ppI ) {
  // check the placeholder
  if ( 0 == ppI ) 
    return StatusCode::FAILURE;
  // check ID of the interface
  if ( IIncidentListener::interfaceID() == id ) {
    *ppI = static_cast<IIncidentListener*>(this);
  } else {
    return Algorithm::queryInterface( id, ppI );
  }
  addRef();
  return StatusCode::SUCCESS;
}
