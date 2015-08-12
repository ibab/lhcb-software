// Include files 

#include "Event/Track.h"
#include "Event/MCParticle.h"
#include "Linker/LinkedTo.h"

// local
#include "PatCheckerNTuple.h"


//-----------------------------------------------------------------------------
// Implementation file for class : PatCheckerNTuple
//
// 2006-05-11 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PatCheckerNTuple )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PatCheckerNTuple::PatCheckerNTuple( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{
  declareProperty( "NtupleName"           , m_tupleName = "Tracks"    );
  declareProperty( "InputTracks"          , m_container = LHCb::TrackLocation::Forward );
}
//=============================================================================
// Destructor
//=============================================================================
PatCheckerNTuple::~PatCheckerNTuple() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PatCheckerNTuple::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode PatCheckerNTuple::execute() {

  debug() << "==> Execute" << endmsg;

  LHCb::Tracks* tracks = get<LHCb::Tracks>( m_container );
  LinkedTo<LHCb::MCParticle, LHCb::Track> link( evtSvc(), msgSvc(), m_container );
  
  for ( LHCb::Tracks::const_iterator itT = tracks->begin(); tracks->end() != itT ; ++itT ) {
    LHCb::Track* tr = *itT;
    Gaudi::XYZPoint position = tr->position();
    Gaudi::XYZVector slopes  = tr->slopes();
    double momentum          = tr->p()*tr->charge();

    LHCb::MCParticle* part = link.first( tr );
    while( 0 != part ) {
      Gaudi::XYZPoint mcPosition = part->originVertex()->position();
      if ( mcPosition.z() > 700. ) {  // ignore tracks created after the Velo.
        part = link.next();
        continue;
      }
      
      Gaudi::XYZVector mcSlopes  = part->momentum().Vect();
      mcSlopes /= mcSlopes.z();
      double mcMomentum = part->momentum().P();
      int charge = part->particleID().threeCharge()/3;
      mcMomentum *= charge;

      position += ( mcPosition.z() - position.z() ) * slopes;
      
      // Fill the ntuple  
      Tuple tPoint = nTuple( m_tupleName );
      tPoint->column( "x"   , position.x() );
      tPoint->column( "y"   , position.y() );
      tPoint->column( "z"   , position.z() );
      tPoint->column( "tx"  , slopes.x() );
      tPoint->column( "ty"  , slopes.y() );
      tPoint->column( "p"   , momentum );
      tPoint->column( "dx"  , position.x() - mcPosition.x() );
      tPoint->column( "dy"  , position.y() - mcPosition.y() );
      tPoint->column( "dtx" , slopes.x() - mcSlopes.x() );
      tPoint->column( "dty" , slopes.y() - mcSlopes.y() );
      tPoint->column( "dp"  , momentum-mcMomentum );

      tPoint->write();

      if ( msgLevel( MSG::DEBUG) ) {
        debug() << format( "Track%4d MC%5d x%8.3f y%8.3f z%8.3f tx%8.3f ty%8.3f",
                           tr->key(), part->key(), position.x(), position.y(), position.z(),
                           slopes.x(), slopes.y() );
        debug() << format( " dx%8.3f dy%8.3f dp/p%8.4f%%",  position.x() - mcPosition.x(),
                           position.y() - mcPosition.y(), 100.*(1.-mcMomentum/momentum) ) << endmsg;
      }
      part = link.next();
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PatCheckerNTuple::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

//=============================================================================
