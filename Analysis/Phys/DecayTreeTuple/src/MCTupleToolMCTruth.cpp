// $Id: MCTupleToolMCTruth.cpp,v 1.2 2009-01-22 09:27:33 pkoppenb Exp $
// Include files 
#include "gsl/gsl_sys.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/Vector3DTypes.h"

// local
#include "MCTupleToolMCTruth.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/MCParticle.h"

using namespace LHCb;
//-----------------------------------------------------------------------------
// Implementation file for class : MCTupleToolMCTruth
//
// 2009-01-19 : Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCTupleToolMCTruth );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCTupleToolMCTruth::MCTupleToolMCTruth( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IMCParticleTupleTool>(this);

  // Store kinetic information from the associated candidate
  declareProperty( "StoreKineticInfo",  m_storeKinetic = true );

  // Store the end and origin true vertex information
  declareProperty( "StoreVertexInfo",  m_storeVertexes = true );
  
  // Store the propertime information for associated composite particle
  declareProperty( "StorePropertimeInfo", m_storePT = true );

}
//=============================================================================
// Destructor
//=============================================================================
MCTupleToolMCTruth::~MCTupleToolMCTruth() {} 

//=============================================================================
// initialize
//=============================================================================

StatusCode MCTupleToolMCTruth::initialize(){
  if( ! GaudiTool::initialize() ) return StatusCode::FAILURE;
  return StatusCode::SUCCESS ;
}
//=============================================================================
// Fill
//=============================================================================
StatusCode MCTupleToolMCTruth::fill( const LHCb::MCParticle* 
                                     , const LHCb::MCParticle* mcp
                                     , const std::string& head
                                     , Tuples::Tuple& tuple ){
  
  bool test = true;
  
  int mcPid = 0;
  double mcTau = -1;
  
  Gaudi::XYZVector endVertex, originVertex;
  Gaudi::LorentzVector trueP;

  // pointer is ready, prepare the values:
  if( mcp ) {
    mcPid = mcp->particleID().pid();
    trueP = mcp->momentum();

    const SmartRefVector< LHCb::MCVertex > & endVertices = mcp->endVertices();
    endVertex = endVertices.front()->position(); // the first item, the other are discarded.
    originVertex = mcp->originVertex()->position();

    // lifetime
    if( m_storePT ){
      Gaudi::XYZVector dist = endVertex - originVertex;
      // copied from DecayChainNTuple // 
      mcTau = trueP.M() * dist.Dot( trueP.Vect() ) / trueP.Vect().mag2();
      mcTau /= Gaudi::Units::picosecond * Gaudi::Units::c_light;
    }
  }

  // fill the tuple:
  test &= tuple->column( head+"_TRUEID", mcPid );  

  if( m_storeKinetic )
    test &= tuple->column( head + "_TRUEP_", trueP );

  if( m_storeVertexes ){
    test &= tuple->column( head + "_TRUEORIGINVERTEX_", originVertex );
    test &= tuple->column( head + "_TRUEENDVERTEX_", endVertex );
  }

  if( m_storePT )
    test &= tuple->column( head + "_TRUETAU", mcTau );

  
  return StatusCode(test);
}
