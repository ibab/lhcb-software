// Include files 
#include "Event/Track.h"
#include "Event/StandardPacker.h"

// local
#include "PackTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackTrack
//
// 2008-11-12 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PackTrack )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  PackTrack::PackTrack( const std::string& name,
                        ISvcLocator* pSvcLocator)
    : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = LHCb::TrackLocation::Default );
  declareProperty( "OutputName", m_outputName = LHCb::PackedTrackLocation::Default );
  declareProperty( "AlwaysCreateOutput",         m_alwaysOutput = false     );
  declareProperty( "DeleteInput",                m_deleteInput  = false     );
  declareProperty( "EnableCheck",                m_enableCheck  = false     );
}

//=============================================================================
// Destructor
//=============================================================================
PackTrack::~PackTrack() {}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackTrack::execute()
{
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  // If input does not exist, and we aren't making the output regardless, just return
  if ( !m_alwaysOutput && 
       !exist<LHCb::Tracks>(m_inputName) ) return StatusCode::SUCCESS;

  // Input
  LHCb::Tracks* tracks = getOrCreate<LHCb::Tracks,LHCb::Tracks>( m_inputName );

  // Output
  LHCb::PackedTracks* out = new LHCb::PackedTracks();
  put( out, m_outputName );
  out->setVersion( 4 );

  // Track Packer
  const LHCb::TrackPacker packer(*this);

  // Pack the tracks
  packer.pack( *tracks, *out );

  // Packing checks
  if ( UNLIKELY(m_enableCheck) )
  {
    // make new unpacked output data object
    LHCb::Tracks * unpacked = new LHCb::Tracks();
    put( unpacked, m_inputName+"_PackingCheck" );
    
    // unpack
    packer.unpack( *out, *unpacked );
    
    // run checks
    packer.check( *tracks, *unpacked ).ignore();
    
    // clean up after checks
    StatusCode sc = evtSvc()->unregisterObject( unpacked );
    if( sc.isSuccess() ) 
      delete unpacked;
    else
      return Error("Failed to delete test data after unpacking check", sc );
  }

  // If requested, remove the input data from the TES and delete
  if ( UNLIKELY(m_deleteInput) )
  {
    const StatusCode sc = evtSvc()->unregisterObject( tracks );
    if ( sc.isSuccess() )
    {
      delete tracks;
      tracks = NULL;
    }
    else
    {
      return Error("Failed to delete input data as requested", sc );
    }
  }
  else
  {
    // Clear the registry address of the unpacked container, to prevent reloading
    tracks->registry()->setAddress( 0 );
  }

  // Summary of the size of the PackTracks container
  counter("# PackedTracks") += out->tracks().size();

  return StatusCode::SUCCESS;
}

//=============================================================================
