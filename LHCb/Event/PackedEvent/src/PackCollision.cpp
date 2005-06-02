// $Id: PackCollision.cpp,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/Collision.h"
#include "Kernel/StandardPacker.h"
#include "Event/PackedCollision.h"
// local
#include "PackCollision.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PackCollision
//
// 2005-03-18 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<PackCollision>          s_factory ;
const        IAlgFactory& PackCollisionFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PackCollision::PackCollision( const std::string& name,
                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName =CollisionLocation::Default );
  declareProperty( "OutputName", m_outputName=PackedCollisionLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
PackCollision::~PackCollision() {}; 

//=============================================================================
// Main execution
//=============================================================================
StatusCode PackCollision::execute() {

  debug() << "==> Execute" << endmsg;

  Collisions* colls = get<Collisions>( m_inputName );
  debug() << m_inputName << " contains " << colls->size()
          << " Collisions to convert." << endreq;
  
  StandardPacker pack;
  
  PackedCollisions* out = new PackedCollisions();
  put( out, m_outputName );

  for ( Collisions::const_iterator itV = colls->begin();
        colls->end() != itV; ++itV ) {
    Collision* coll = (*itV);
    PackedCollision newColl;

    newColl.key  = coll->key();
    newColl.isSignal = coll->isSignal();
    newColl.processType = coll->processType();
    newColl.x    = pack.position( coll->primVtxPosition().x() );
    newColl.y    = pack.position( coll->primVtxPosition().y() );
    newColl.z    = pack.position( coll->primVtxPosition().z() );
    out->addEntry( newColl );
  }

  // Clear the registry address of the unpacked container, to prevent reloading
  colls->registry()->setAddress( 0 );

  return StatusCode::SUCCESS;
};

//=============================================================================
