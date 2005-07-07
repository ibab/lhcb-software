// $Id: UnpackTrStoredTrack.cpp,v 1.2 2005-07-07 07:36:55 ocallot Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/TrStoredTrack.h"
#include "Kernel/StandardPacker.h"
#include "Event/PackedTrack.h"
// local
#include "UnpackTrStoredTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UnpackTrStoredTrack
//
// 2005-03-17 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<UnpackTrStoredTrack>          s_factory ;
const        IAlgFactory& UnpackTrStoredTrackFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UnpackTrStoredTrack::UnpackTrStoredTrack( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = PackedTrackLocation::Default );
  declareProperty( "OutputName", m_outputName = TrStoredTrackLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
UnpackTrStoredTrack::~UnpackTrStoredTrack() {}; 

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackTrStoredTrack::execute() {

  debug() << "==> Execute" << endmsg;

  PackedTracks* dst = get<PackedTracks>( m_inputName );
  debug() << "Size of PackedTracks = " << dst->end() - dst->begin() << endreq;

  TrStoredTracks* newTracks = new TrStoredTracks();
  put( newTracks, m_outputName );

  StandardPacker pack;
  
  for ( std::vector<PackedTrack>::const_iterator itS = dst->begin();
        dst->end() != itS; ++itS ) {
    const PackedTrack& src = (*itS);

    TrStoredTrack* track = new TrStoredTrack( );
    newTracks->insert( track, src.key );

    track->setCharge( double(src.charge) );
    track->setErrorFlag ( int( src.error ) );
    track->setHistory( src.history );
    int key;
    int hintID;
    std::vector<int>::const_iterator itI;
    for ( itI = src.states.begin() ; src.states.end() != itI ; ++itI ) {
      pack.hintAndKey( *itI, dst, newTracks, hintID, key );
      SmartRef<TrState> ref( newTracks, hintID, key );
      track->addToStates( ref );
    }
    //== Measurements
    for ( itI = src.measurements.begin() ; src.measurements.end() != itI ; ++itI ) {
      pack.hintAndKey( *itI, dst, newTracks, hintID, key );
      SmartRef<TrStoredMeasurement> ref( newTracks, hintID, key );
      track->addToMeasurements( ref );
    }
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
