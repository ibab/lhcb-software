// $Id: PackTrStoredTrack.cpp,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/TrStoredTrack.h"
#include "Kernel/StandardPacker.h"
#include "Event/PackedTrack.h"
// local
#include "PackTrStoredTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackTrStoredTrack
//
// 2005-03-16 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<PackTrStoredTrack>          s_factory ;
const        IAlgFactory& PackTrStoredTrackFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PackTrStoredTrack::PackTrStoredTrack( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = TrStoredTrackLocation::Default );
  declareProperty( "OutputName", m_outputName = PackedTrackLocation::Default );
  declareProperty( "StoreMeasurements", m_storeMeasurements = false );
}
//=============================================================================
// Destructor
//=============================================================================
PackTrStoredTrack::~PackTrStoredTrack() {}; 

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackTrStoredTrack::execute() {

  debug() << "==> Execute" << endmsg;

  TrStoredTracks* tracks = get<TrStoredTracks>( m_inputName );
  debug() << m_inputName << " contains " << tracks->size()
          << " tracks to convert." << endreq;
  
  StandardPacker pack;
  
  PackedTracks* out = new PackedTracks();
  put( out, m_outputName );

  for ( TrStoredTracks::const_iterator itT = tracks->begin();
        tracks->end() != itT; ++itT ) {
    TrStoredTrack* tra = (*itT);

    PackedTrack newTra;

    newTra.key     = tra->key();
    newTra.charge  = int( tra->charge() );
    newTra.error   = tra->errorFlag();
    newTra.history = tra->history();
    

    //== Store state references
    for ( SmartRefVector<TrState>::const_iterator itS = tra->states().begin();
          tra->states().end() != itS; ++itS ) {
      int ref = pack.reference( out, (*itS)->parent(), (*itS)->key() );
      newTra.states.push_back( ref );
      if ( MSG::VERBOSE >= msgLevel() ) {
        verbose() << "Reference to state       " 
                  << format( "%8x",  ref ) << endreq;
      }
    }

    //== Store measurement references
    if ( m_storeMeasurements ) {
      SmartRefVector<TrStoredMeasurement>::const_iterator itM;
      for ( itM = tra->measurements().begin();
            tra->measurements().end() != itM; ++itM ) {
        int ref = pack.reference( out, (*itM)->parent(), (*itM)->key() );
        newTra.measurements.push_back( ref );
        if ( MSG::VERBOSE >= msgLevel() ) {
          verbose() << "Reference to measurement " 
                    << format( "%8x",  ref ) << endreq;
        }
      }
    }
    
    out->addEntry( newTra );
  }

  // Clear the registry address of the unpacked container, to prevent reloading
  tracks->registry()->setAddress( 0 );

  return StatusCode::SUCCESS;
};

//=============================================================================
