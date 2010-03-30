
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/Track.h"
#include "Event/MCParticle.h"
#include "Linker/LinkedTo.h"
#include "LHCbMath/Line.h"
#include "LHCbMath/GeomFun.h"
#include "Event/MCHit.h"

#include "Event/VeloPixCluster.h"
#include "Event/VeloPixLiteMeasurement.h"
// local
#include "VeloPixChecker.h"


//-----------------------------------------------------------------------------
// Implementation file for class : VeloPixChecker
//
// 2006-05-11 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( VeloPixChecker );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
VeloPixChecker::VeloPixChecker( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiTupleAlg ( name , pSvcLocator )
{
  declareProperty( "NtupleName"           , m_tupleName = "Tracks"    );
  declareProperty( "InputTracks"          , m_container = "Rec/Track/VeloPix" );
}
//=============================================================================
// Destructor
//=============================================================================
VeloPixChecker::~VeloPixChecker() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode VeloPixChecker::initialize() {
  StatusCode sc = GaudiTupleAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode VeloPixChecker::execute() {

  debug() << "==> Execute" << endmsg;

  LHCb::Tracks* tracks = get<LHCb::Tracks>( m_container );
  LinkedTo<LHCb::MCParticle, LHCb::Track> link( evtSvc(), msgSvc(), m_container );

  
  
  for ( LHCb::Tracks::const_iterator itT = tracks->begin(); tracks->end() != itT ; ++itT ) {
    LHCb::Track* tr = *itT;
    Gaudi::XYZPoint position = tr->position();
    Gaudi::XYZPoint firstposition = tr->position();
    Gaudi::XYZVector slopes  = tr->slopes();
    double momentum          = tr->p()*tr->charge();
    double transmomentum     = tr->pt();

    
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
      double mcTransMomentum = part->momentum().Pt();
      int charge = part->particleID().threeCharge()/3;
      mcMomentum *= charge;

      Gaudi::XYZVector MCToRecPosition(position.x() - mcPosition.x(),position.y() - mcPosition.y(),position.z() - mcPosition.z());
      
      double IPSquare =  MCToRecPosition.Dot(MCToRecPosition)-MCToRecPosition.Dot(slopes)/slopes.Dot(slopes);
      Gaudi::XYZPoint positionForIp(position.x(),position.y(),position.z());
      
      Gaudi::Math::Line<Gaudi::XYZPoint,Gaudi::XYZVector> line(positionForIp,slopes);

      position += ( mcPosition.z() - position.z() ) * slopes;


      double ip = Gaudi::Math::impactParameter(mcPosition,line);

      // Fill the ntuple  
      Tuple tPoint = nTuple( m_tupleName );
      //tPoint->farray("hit_dx",theDiffx.begin(),theDiffx.end(),"Ndx",20);
      //tPoint->farray("hit_dy",theDiffy.begin(),theDiffy.end(),"Ndx",20);
      //tPoint->farray("hit_dz",theDiffz.begin(),theDiffz.end(),"Ndx",20);
      
      tPoint->column( "x"   , positionForIp.x() );
      tPoint->column( "y"   , positionForIp.y() );
      tPoint->column( "z"   , positionForIp.z() );
      tPoint->column( "tx"  , slopes.x() );
      tPoint->column( "ty"  , slopes.y() );
      tPoint->column( "p"   , momentum );
      tPoint->column( "pt"   , transmomentum );
      tPoint->column( "chi2ndof" , tr->chi2PerDoF() );
      tPoint->column( "nhits" , tr->nLHCbIDs () );
      tPoint->column( "mcpt" ,  mcTransMomentum);
      tPoint->column( "ip"   , ip );
      tPoint->column( "ip1"   , sqrt(IPSquare) );
      tPoint->column( "dx"  , position.x() - mcPosition.x() );
      tPoint->column( "dy"  , position.y() - mcPosition.y() );
      tPoint->column( "firstMeas"  ,firstposition.z()- mcPosition.z() );
      tPoint->column( "dtx" , slopes.x() - mcSlopes.x() );
      tPoint->column( "dty" , slopes.y() - mcSlopes.y() );
      tPoint->column( "mctx" , mcSlopes.x() );
      tPoint->column( "mcty" , mcSlopes.y() );
      tPoint->column( "dp"  , momentum-mcMomentum );

      tPoint->write();

      if ( msgLevel( MSG::DEBUG) ) {
        debug() << format( "Track%4d MC%5d x%8.3f y%8.3f z%8.3f tx%8.3f ty%8.3f",
                           tr->key(), part->key(), position.x(), position.y(), position.z(),
                           slopes.x(), slopes.y() );
        debug() << format( " dx%8.3f dy%8.3f dp/p%8.4f%%",  position.x() - mcPosition.x(),
                           position.y() - mcPosition.y(), 100.*(1.-mcMomentum/momentum) ) << endreq;
      }
      part = link.next();
    }
  }
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode VeloPixChecker::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiTupleAlg::finalize();  // must be called after all other actions
}

//=============================================================================
