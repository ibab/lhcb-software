// $Id: PreLoadParticles.cpp,v 1.5 2006-06-18 14:45:41 jonrob Exp $
// Include files

// from EventSys
#include "Event/Particle.h"
#include "Event/Vertex.h"

// local
#include "PreLoadParticles.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PreLoadParticles
//
// 2002-08-22 : Gloria Corti
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<PreLoadParticles>          s_factory ;
const        IAlgFactory& PreLoadParticlesFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PreLoadParticles::PreLoadParticles( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator ) 
{
  setProperty( "AvoidSelResult", "true");
  setProperty( "DecayDescriptor", "\"null\"");
}

//=============================================================================
// Destructor
//=============================================================================
PreLoadParticles::~PreLoadParticles() {};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PreLoadParticles::execute() 
{
  // Save desktop
  const StatusCode scDesktop = desktop()->saveDesktop();
  if ( scDesktop.isFailure() ) 
  {
    return Error( "Not able to save desktop" );
  }

  if ( msgLevel(MSG::DEBUG) )
  {
    // Log number of vertices and particles
    debug() << "Number of particles in desktop = " 
            << desktop()->particles().size() << endmsg;
    debug() << "Number of primary vertices in desktop = " 
            << desktop()->primaryVertices().size() << endmsg;
    debug() << "Number of secondary vertices in desktop = " 
            <<desktop()->secondaryVertices().size() << endmsg;
  }

  setFilterPassed(true);

  return scDesktop;
}
