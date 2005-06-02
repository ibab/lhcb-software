// $Id: UnpackCollision.cpp,v 1.1.1.1 2005-06-02 08:54:21 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

#include "Event/Collision.h"
#include "Kernel/StandardPacker.h"
#include "Event/PackedCollision.h"
// local
#include "UnpackCollision.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UnpackCollision
//
// 2005-03-18 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<UnpackCollision>          s_factory ;
const        IAlgFactory& UnpackCollisionFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UnpackCollision::UnpackCollision( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  declareProperty( "InputName" , m_inputName  = PackedCollisionLocation::Default );
  declareProperty( "OutputName", m_outputName = CollisionLocation::Default );
}
//=============================================================================
// Destructor
//=============================================================================
UnpackCollision::~UnpackCollision() {}; 

//=============================================================================
// Main execution
//=============================================================================
StatusCode UnpackCollision::execute() {

  debug() << "==> Execute" << endmsg;

  PackedCollisions* dst = get<PackedCollisions>( m_inputName );
  debug() << "Size of PackedCollisions = " << dst->end() - dst->begin() << endreq;

  Collisions* newCollisions = new Collisions();
  put( newCollisions, m_outputName );

  StandardPacker pack;
  
  for ( std::vector<PackedCollision>::const_iterator itS = dst->begin();
        dst->end() != itS; ++itS ) {
    const PackedCollision& src = (*itS);

    Collision* coll = new Collision( );
    newCollisions->insert( coll, src.key );
    coll->setIsSignal( src.isSignal );
    coll->setProcessType( src.processType );
    HepPoint3D pt( pack.position( src.x ), pack.position( src.y ), pack.position( src.z) );
    coll->setPrimVtxPosition( pt );
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
