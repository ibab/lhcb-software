// $Id: CopyParticle2PVMap.cpp,v 1.1 2010-08-26 17:06:44 jpalac Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include <GaudiKernel/ObjectVector.h>
// LHCb
#include "Kernel/ITriggerTisTos.h"
#include "Event/RecVertex.h"
#include "Event/Particle.h"
// MicroDST
#include "MicroDST/ICloneVertexBase.h"

// DaVinci
#include "DaVinciUtils/Guards.h"
#include "Kernel/DaVinciStringUtils.h"
#include "Kernel/Particle2VertexBase.h"
// local
#include "CopyParticle2PVMap.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CopyParticle2PVMap
//
// 2010-08-18 : Juan Palacios
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CopyParticle2PVMap::CopyParticle2PVMap( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : MicroDSTAlgorithm ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
CopyParticle2PVMap::~CopyParticle2PVMap() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode CopyParticle2PVMap::initialize() {
  StatusCode sc = MicroDSTAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_toCloner = tool<ICloneVertexBase>("VertexBaseFromRecVertexCloner", this);

  return m_toCloner ? StatusCode::SUCCESS : StatusCode::FAILURE;

}

//=============================================================================
// Main execution
//=============================================================================
StatusCode CopyParticle2PVMap::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;


  typedef std::vector<std::string>::const_iterator stringIter;
  stringIter iLoc = this->inputTESLocations().begin();
  stringIter locEnd = this->inputTESLocations().end();


  for ( ; iLoc != locEnd; ++iLoc) {

    const std::string inputLocation = niceLocationName(*iLoc);

    executeLocation(inputLocation);

  }

  return StatusCode::SUCCESS;

}
//=============================================================================
void CopyParticle2PVMap::executeLocation(const std::string& inputLocation)
{

  typedef DaVinci::Map::Particle2VertexBase Map;

  const std::string outputLocation =
    this->outputTESLocation( inputLocation );

  if ( msgLevel(MSG::VERBOSE) ) {
    verbose() << "Going to clone relations from "
              << inputLocation
              << " into " << outputLocation << endmsg;
  }

  if (exist<Map>(outputLocation) )
  {
    this->Warning("Object "+ outputLocation + " already exists. Not cloning.",
                  StatusCode::SUCCESS, 0).ignore() ;
    return;
  }

  if (exist<Map>(inputLocation) )
  {
    if ( msgLevel(MSG::VERBOSE) ) {
      verbose() << "Retrieving relations table from "
                << inputLocation << endmsg;
    }
    const Map* p2pv = get<Map>(inputLocation);
    if ( p2pv && !p2pv->empty() ) {

      if ( msgLevel(MSG::VERBOSE) ) {
        verbose() << "found P->PV map with "<< p2pv->size()
                  << " entries!" << endmsg;
      }
      Map* p2pvClone = new Map();
      DaVinci::Utils::DataObjectGuard guard(p2pvClone);
      Map::const_iterator iP2PV = p2pv->begin();
      Map::const_iterator iP2PVEnd = p2pv->end();

      for (; iP2PV != iP2PVEnd; ++ iP2PV) {
        const LHCb::Particle* part = (*iP2PV).first;
        const LHCb::VertexBase* vtx = (*iP2PV).second;
        if (part && vtx) {
          const LHCb::VertexBase* cloneVtx = (*m_toCloner)(vtx);
          const LHCb::Particle* clonePart = getStoredClone<LHCb::Particle>(part);
          if (cloneVtx && clonePart) {
            p2pvClone->insert(clonePart, cloneVtx);
          }
        }
      }

      if ( msgLevel(MSG::VERBOSE) ) {
        verbose() << "Going to store relations table from "
                  << inputLocation
                  << " into " << outputLocation << endmsg;
        verbose() << "Number of relations in cloned table: "
                  << p2pvClone->size() << endmsg;
      }
      if (!p2pvClone->empty()) {
        put( p2pvClone, outputLocation );
      }
    } else {
      if ( msgLevel(MSG::VERBOSE) ) {
        this->Warning("Found no table at "+inputLocation,
                      StatusCode::FAILURE).ignore();
      }
    }

  } else {
    if ( msgLevel(MSG::VERBOSE) ) {
      this->Warning("Found no table at "+inputLocation,
                    StatusCode::FAILURE).ignore();
    }
  }

}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode CopyParticle2PVMap::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyParticle2PVMap )
