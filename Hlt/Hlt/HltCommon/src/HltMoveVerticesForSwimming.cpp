// $Id: HltMoveVerticesForSwimming.cpp,v 1.5 2008-07-30 13:37:32 graven Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "boost/foreach.hpp"

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
  , m_selections(*this)
{
  declareProperty("Blifetime", m_bLifetime = -999999.0);
  declareProperty("SwimmingDistance", m_swimmingDistance = 0.0);
  declareProperty("ParticlesName" , m_particlesName = "");

  m_selections.declareProperties();
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

  m_selections.retrieveSelections();
  m_selections.registerSelection();

  saveConfiguration();
  info() << "HltAlgorithm initialized" << endmsg;

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltMoveVerticesForSwimming::execute() {

  StatusCode sc = HltAlgorithm::execute(); // must be executed first
  if (sc.isFailure()) return sc;

  //Check if particles exist
  if (!exist<Particles>(m_particlesName)) return sc;

  Particles* pars = get<Particles>(m_particlesName);
  if (m_debug) {
    if (pars == 0) verbose() << " no particles found! " << endmsg;
    else verbose() << " particles found " << pars->size() << endmsg;
  }  
  if (pars == 0) return sc;
  
  m_selections.output()->clean(); //TODO: is this really needed?

  //Lets see what we just did, for debug
  if (m_debug) {
        debug() << "Printing out the input vertices" << endmsg;
        BOOST_FOREACH( LHCb::RecVertex* v, *m_selections.input<1>()) {
                debug() << *v << endmsg;
        }
  }

  //Swim the primary vertices
  m_bDirection = (*(pars->begin()))->slopes().Unit();
  m_bVertexPosition = (*(pars->begin()))->endVertex()->position();
  m_bMass = (*(pars->begin()))->measuredMass();
  m_bMom = (*(pars->begin()))->p();
  //FIXME: why is the next line commented out???
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

  m_bLifetime = get_B_lifetime();
  
  int ncan = m_selections.output()->size();
  debug() << " candidates found " << ncan << endmsg;
  return sc;
  
}
//=============================================================================
StatusCode HltMoveVerticesForSwimming::move_PVs(){
//Swim the PVs in the required direction

  Gaudi::XYZPoint newVertPos; //The new position of the vertices we swim

  debug() << "Beginning to move vertices for swimming" << endmsg;

  BOOST_FOREACH( LHCb::RecVertex *vertex, *m_selections.input<1>()) {

        debug() << "The primary vertex is at " << vertex << endmsg;
        debug() << "With X coordinate " << vertex->position().X() << endmsg;
        debug() << "With Y coordinate " << vertex->position().Y() << endmsg;
        debug() << "With Z coordinate " << vertex->position().Z() << endmsg;

        newVertPos = vertex->position() + m_swimmingDistance*m_bDirection;
        //newVertPos = m_bVertexPosition - m_swimmingDistance*m_bDirection;

        debug() << "The new vertex" << endmsg;
        debug() << "With X coordinate " << newVertPos.X() << endmsg;
        debug() << "With Y coordinate " << newVertPos.Y() << endmsg;
        debug() << "With Z coordinate " << newVertPos.Z() << endmsg;

        vertex->setPosition(newVertPos); //@TODO / @FIXME: we should NOT change the input vertex!!!
        m_selections.output()->push_back( const_cast<LHCb::RecVertex*>(vertex));
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
double HltMoveVerticesForSwimming::get_B_lifetime(){
//Get the B lifetime. The "correct" PV is defined as the one
//with the larger absolute cos(theta) the the B. 

  double templifetime = -999999.;
  if (m_selections.input<1>()->empty()) return templifetime;

  Gaudi::XYZPoint newVertPos;
  double vertexCosTheta = 0.;
  double bestVertexCosTheta = 0.;

  LHCb::RecVertex* bestVertex = m_selections.input<1>()->front();
  
  debug() << "Trying to find the best vertex" << endmsg;

  BOOST_FOREACH( LHCb::RecVertex* vertex, *m_selections.input<1>() ) {
	debug() << "First candidate is " << vertex << endmsg;
  	vertexCosTheta = ((m_bVertexPosition - vertex->position()).Unit()).Dot(m_bDirection.Unit());
	debug() << "It has a cos theta of " << vertexCosTheta << endmsg;
	if (fabs(vertexCosTheta) > fabs(bestVertexCosTheta)) {
		debug() << "Which is a new best cos theta" << endmsg;
		bestVertexCosTheta = fabs(vertexCosTheta);
		bestVertex = vertex;
	}
  }

  debug() << "The best vertex is " << bestVertex << endmsg;

  debug() << "It has" << endmsg;
  debug() << "X coordinate " << bestVertex->position().X() << endmsg;
  debug() << "Y coordinate " << bestVertex->position().Y() << endmsg;
  debug() << "Z coordinate " << bestVertex->position().Z() << endmsg;

  newVertPos = bestVertex->position() + m_swimmingDistance*m_bDirection; 
  bestVertex->setPosition(newVertPos); //@FIXME @TODO should NOT modify input vertex

  debug() << "The moved vertex is at " << bestVertex << endmsg;
  debug() << "With X coordinate " << bestVertex->position().X() << endmsg;
  debug() << "With Y coordinate " << bestVertex->position().Y() << endmsg;
  debug() << "With Z coordinate " << bestVertex->position().Z() << endmsg;

  m_selections.output()->push_back( const_cast<LHCb::RecVertex*>(bestVertex));

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
