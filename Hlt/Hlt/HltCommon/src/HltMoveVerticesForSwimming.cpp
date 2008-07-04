// $Id: HltMoveVerticesForSwimming.cpp,v 1.4 2008-07-04 08:07:41 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// from Event
#include "HltMoveVerticesForSwimming.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : HltMoveVerticesForSwimming 
//
// 2008-02-12 : V. Gligorov
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( HltMoveVerticesForSwimming );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltMoveVerticesForSwimming::HltMoveVerticesForSwimming( const std::string& name, ISvcLocator* pSvcLocator)
  : HltAlgorithm ( name , pSvcLocator )
{
  declareProperty("Blifetime", m_bLifetime = -999999.0);
  declareProperty("SwimmingDistance", m_swimmingDistance = 0.0);
  declareProperty("ParticlesName" , m_particlesName = "");
}

//=============================================================================
// Destructor
//=============================================================================
HltMoveVerticesForSwimming::~HltMoveVerticesForSwimming() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode HltMoveVerticesForSwimming::initialize() {

  info() << "Starts" << endmsg;
  
  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_inputVertices = &(retrieveTSelection<LHCb::RecVertex>(m_inputSelectionName));

  m_outputVertices = &(registerTSelection<LHCb::RecVertex>());

  saveConfiguration();
  info() << "HltAlgorithm initialized" << endmsg;

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltMoveVerticesForSwimming::execute() {

  StatusCode sc = HltAlgorithm::execute(); // must be executed first

  //Check if particles exist
  if (!exist<Particles>(m_particlesName)) return sc;

  Particles* pars = get<Particles>(m_particlesName);
  if (m_debug) {
    if (pars == 0) verbose() << " no particles found! " << endmsg;
    else verbose() << " particles found " << pars->size() << endmsg;
  }  
  if (pars == 0) return sc;
  
  m_outputVertices->clean();

  //Lets see what we just did, for debug
  if (m_debug) {
        debug() << "Printing out the input vertices" << endmsg;
        for (iV = (*m_inputVertices).begin(); iV != (*m_inputVertices).end(); ++iV) {

                debug() << (*iV) << endmsg;

        }

  }

  //Swim the primary vertices
  m_bDirection = (*(pars->begin()))->slopes().Unit();
  m_bVertexPosition = (*(pars->begin()))->endVertex()->position();
  m_bMass = (*(pars->begin()))->measuredMass();
  m_bMom = (*(pars->begin()))->p();
  //sc = move_PVs();

  debug() << "The B direction is" << endmsg;
  debug() << "X slope = " << m_bDirection.X() << endmsg;
  debug() << "Y slope = " << m_bDirection.Y() << endmsg;
  debug() << "Z slope = " << m_bDirection.Z() << endmsg;

  debug() << "The B vertex position is" << endmsg;
  debug() << "X = " << m_bVertexPosition.X() << endmsg;
  debug() << "Y = " << m_bVertexPosition.Y() << endmsg;
  debug() << "Z = " << m_bVertexPosition.Z() << endmsg;

  debug() << "The swimming distance is " << m_swimmingDistance << endmsg;

  if (m_inputVertices->size() > 0) m_bLifetime = get_B_lifetime();
  else m_bLifetime = -999999.0;
  
  int ncan = m_outputVertices->size();
  //candidateFound(ncan);
  
  debug() << " candidates found " << ncan << endmsg;

  return sc;
  
}
//=============================================================================
StatusCode HltMoveVerticesForSwimming::move_PVs(){
//Swim the PVs in the required direction

  Gaudi::XYZPoint newVertPos; //The new position of the vertices we swim

  debug() << "Beginning to move vertices for swimming" << endmsg;

  for (iV = (*m_inputVertices).begin(); iV != (*m_inputVertices).end(); ++iV) {

        debug() << "The primary vertex is at " << (*iV) << endmsg;
        debug() << "With X coordinate " << (*iV)->position().X() << endmsg;
        debug() << "With Y coordinate " << (*iV)->position().Y() << endmsg;
        debug() << "With Z coordinate " << (*iV)->position().Z() << endmsg;

        newVertPos = (*iV)->position() + m_swimmingDistance*m_bDirection;
        //newVertPos = m_bVertexPosition - m_swimmingDistance*m_bDirection;

        debug() << "The new vertex" << endmsg;
        debug() << "With X coordinate " << newVertPos.X() << endmsg;
        debug() << "With Y coordinate " << newVertPos.Y() << endmsg;
        debug() << "With Z coordinate " << newVertPos.Z() << endmsg;

        (*iV)->setPosition(newVertPos);
        m_outputVertices->push_back( const_cast<LHCb::RecVertex*>(*iV));

  }

  return StatusCode::SUCCESS;
}
//=============================================================================
double HltMoveVerticesForSwimming::get_B_lifetime(){
//Get the B lifetime. The "correct" PV is defined as the one
//with the larger absolute cos(theta) the the B. 

  Gaudi::XYZPoint newVertPos;
  double templifetime = -999999.;
  double vertexCosTheta = 0.;
  double bestVertexCosTheta = 0.;
  LHCb::RecVertex* bestVertex = (*(*m_inputVertices).begin()); 
  
  debug() << "Trying to find the best vertex" << endmsg;

  for (iV = (*m_inputVertices).begin(); iV != (*m_inputVertices).end(); ++iV) {

	debug() << "First candidate is " << (*iV) << endmsg;

  	vertexCosTheta = ((m_bVertexPosition - (*iV)->position()).Unit()).Dot(m_bDirection.Unit());

	debug() << "It has a cos theta of " << vertexCosTheta << endmsg;

	if (fabs(vertexCosTheta) > fabs(bestVertexCosTheta)) {

		debug() << "Which is a new best cos theta" << endmsg;

		bestVertexCosTheta = fabs(vertexCosTheta);
		bestVertex = (*iV);
	}
  }

  debug() << "The best vertex is " << bestVertex << endmsg;

  debug() << "It has" << endmsg;
  debug() << "X coordinate " << bestVertex->position().X() << endmsg;
  debug() << "Y coordinate " << bestVertex->position().Y() << endmsg;
  debug() << "Z coordinate " << bestVertex->position().Z() << endmsg;

  newVertPos = bestVertex->position() + m_swimmingDistance*m_bDirection; 
  bestVertex->setPosition(newVertPos);

  debug() << "The moved vertex is at " << bestVertex << endmsg;
  debug() << "With X coordinate " << bestVertex->position().X() << endmsg;
  debug() << "With Y coordinate " << bestVertex->position().Y() << endmsg;
  debug() << "With Z coordinate " << bestVertex->position().Z() << endmsg;

  m_outputVertices->push_back( const_cast<LHCb::RecVertex*>(bestVertex));

  templifetime = 1000.*m_bMass*sqrt(	(m_bVertexPosition - 
		       			 bestVertex->position()
		      			).Mag2()
		     		   )/(300.*m_bMom); //get proper lifetime in ps

  if (m_bVertexPosition.Z() < bestVertex->position().Z()) {
  //Negative lifetime if B behind the moved vertex
  	templifetime *= - 1;
  } 

  debug() << "With lifetime " << templifetime << endmsg;

  return templifetime;

}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode HltMoveVerticesForSwimming::finalize() {

  StatusCode sc = HltAlgorithm::finalize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  debug() << "==> Finalize" << endmsg;

  return  StatusCode::SUCCESS;
}

