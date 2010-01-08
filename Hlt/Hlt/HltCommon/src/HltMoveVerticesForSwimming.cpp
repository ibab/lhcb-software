// $Id: HltMoveVerticesForSwimming.cpp,v 1.8 2010-01-08 12:40:24 gligorov Exp $
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
  declareProperty("SwimmingDistance", m_swimmingDistance = 1.0);
  declareProperty("bMomentum_X" , m_bMom_X = 0.098);
  declareProperty("bMomentum_Y" , m_bMom_Y = 0.098);
  declareProperty("bMomentum_Z" , m_bMom_Z = 0.98);
  declareProperty("bVertex_X" , m_bVert_X = 0.1);
  declareProperty("bVertex_Y" , m_bVert_Y = 0.1);
  declareProperty("bVertex_Z" , m_bVert_Z = 1.0);
  declareProperty("bMass",m_bMass = 5279.);

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

  StatusCode sc = HltAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  m_selections.retrieveSelections();
  m_selections.registerSelection();

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode HltMoveVerticesForSwimming::execute() {

  StatusCode sc = StatusCode::SUCCESS;

  m_selections.output()->clean(); //TODO: is this really needed?

  //Lets see what we just did, for debug
  if (msgLevel(MSG::DEBUG)) {
        debug() << "Printing out the input vertices" << endmsg;
        BOOST_FOREACH( LHCb::RecVertex* v, *m_selections.input<1>()) {
                debug() << *v << endmsg;
        }
  }

  //Swim the primary vertices
  m_bVertexPosition.SetX(m_bVert_X);
  m_bVertexPosition.SetY(m_bVert_Y);
  m_bVertexPosition.SetZ(m_bVert_Z);
  m_bDirection.SetX(m_bMom_X);
  m_bDirection.SetY(m_bMom_Y);
  m_bDirection.SetZ(m_bMom_Z); 

  debug() << "The B direction is" << endmsg;
  debug() << "X slope = " << m_bDirection.X() << endmsg;
  debug() << "Y slope = " << m_bDirection.Y() << endmsg;
  debug() << "Z slope = " << m_bDirection.Z() << endmsg;

  debug() << "The B vertex position is" << endmsg;
  debug() << "X = " << m_bVertexPosition.X() << endmsg;
  debug() << "Y = " << m_bVertexPosition.Y() << endmsg;
  debug() << "Z = " << m_bVertexPosition.Z() << endmsg;

  debug() << "The swimming distance is " << m_swimmingDistance << endmsg;

  m_bLifetime = move_PVs();
  
  int ncan = m_selections.output()->size();
  debug() << " candidates found " << ncan << endmsg;
  return sc;
  
}
//=============================================================================
double HltMoveVerticesForSwimming::move_PVs(){
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

  double m_bMom = sqrt(pow(m_bMom_X,2) + pow(m_bMom_Y,2) + pow(m_bMom_Z,2)); 

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
