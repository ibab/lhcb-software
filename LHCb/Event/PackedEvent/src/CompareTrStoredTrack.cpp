// $Id: CompareTrStoredTrack.cpp,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/TrStoredTrack.h"
// local
#include "CompareTrStoredTrack.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CompareTrStoredTrack
//
// 2005-03-17 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CompareTrStoredTrack>          s_factory ;
const        IAlgFactory& CompareTrStoredTrackFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CompareTrStoredTrack::CompareTrStoredTrack( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = TrStoredTrackLocation::Default );
  declareProperty( "TestName"  , m_testName   = TrStoredTrackLocation::Default+"Test" );
}
//=============================================================================
// Destructor
//=============================================================================
CompareTrStoredTrack::~CompareTrStoredTrack() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode CompareTrStoredTrack::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CompareTrStoredTrack::execute() {

  debug() << "==> Execute" << endmsg;

  TrStoredTracks* old  = get<TrStoredTracks>( m_inputName  );
  TrStoredTracks* test = get<TrStoredTracks>( m_testName  );

  if ( old->size() != test->size() ) {
    info() << "Old TrStoredTrack size " << old->size()
           << " differs form Test " << test->size()
           << endreq;
    return StatusCode::FAILURE;
  }  
  TrStoredTracks::const_iterator itOld  = old->begin();
  TrStoredTracks::const_iterator itTest = test->begin();

  while ( old->end() != itOld ) {
    TrStoredTrack* oTra = (*itOld++);
    TrStoredTrack* tTra = (*itTest++);
    if ( oTra->key() != tTra->key() ) {
      info() << "Wrong key : old " <<  oTra->key() << " test " << tTra->key() << endreq;
    }
    bool isOK = true;
    if ( MSG::VERBOSE >= msgLevel() ) isOK = false; //== force printing
    unsigned int kk;
    if ( oTra->charge()        != tTra->charge()    ) isOK = false;
    if ( oTra->errorFlag()     != tTra->errorFlag() ) isOK = false;
    if ( oTra->history()       != tTra->history()   ) isOK = false;
    if ( oTra->states().size() != tTra->states().size() ) isOK = false;
    for ( kk = 0; oTra->states().size() > kk; kk++ ) {
      TrState* dum = oTra->states()[kk];  // convert smartref to pointers
      TrState* dum1 = tTra->states()[kk];
      if ( dum != dum1 ) isOK = false;
    }
    if ( !isOK ) {
      info() << "=== TrStoredTrack key " << oTra->key() << endreq;
      info() << "   old " << format( "charge %6.3f error %4x history %8x", 
                                     oTra->charge(), oTra->errorFlag(), oTra->history() ) 
             << endreq;
      info() << "  test " << format( "charge %6.3f error %4x history %8x", 
                                     tTra->charge(), tTra->errorFlag(), tTra->history() ) 
             << endreq << "   old states ";
      for ( kk = 0; oTra->states().size() > kk; kk++ ) {
        info() << " " << oTra->states()[kk];
      }
      info() << endreq << "  test states ";
      for ( kk = 0; tTra->states().size() > kk; kk++ ) {
        info() << " " << tTra->states()[kk];
      }
      info() << endreq;   
    }
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CompareTrStoredTrack::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
