// $Id: TupleToolMCTruth.cpp,v 1.2 2008-01-28 18:13:26 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolMCTruth.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/Particle.h"
#include "Event/MCParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EventInfoTupleTool
//
// 2007-11-07 : Jeremie Borel
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolMCTruth );

using namespace LHCb;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolMCTruth::TupleToolMCTruth( const std::string& type,
				    const std::string& name,
				    const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_pLink(0)
  , m_pComp(0)
  , m_pChi2(0)
{
  declareInterface<IParticleTupleTool>(this);
  
  // Associator input location. Empty should be fine for most of the case
  declareProperty( "InputLocations", m_assocInputs );

  // Use the chi2 associator instead of the link and composite associator
  declareProperty( "UseChi2Method", m_useChi2Method=false );

  // Store the number of associations seen
  declareProperty( "StoreAssociationNumbers", m_storeNumberOfAssoc=false );

  // Store kinetic information from the associated candidate
  declareProperty( "StoreKineticInfo",  m_storeKinetic = false );

  // Store the end and origin true vertex information
  declareProperty( "StoreVertexInfo",  m_storeVertexes = false );
  
  // Store the propertime information for associated composite particle
  declareProperty( "StorePropertimeInfo", m_storePT = true );
}

//=============================================================================

StatusCode TupleToolMCTruth::initialize(){
  if( ! GaudiTool::initialize() ) return StatusCode::FAILURE;

  if( !m_assocInputs.empty() ){
    if( !m_useChi2Method ){
      m_pLink = new Particle2MCLinker( this, Particle2MCMethod::Links, m_assocInputs );
      m_pComp = new Particle2MCLinker( this, Particle2MCMethod::Composite, m_assocInputs );
    } else {
      m_pChi2 = new Particle2MCLinker( this, Particle2MCMethod::Chi2, m_assocInputs );
    }
    return StatusCode::SUCCESS;
  }

  if( ! m_useChi2Method ){
    m_pLink = new Particle2MCLinker( this, Particle2MCMethod::Links, "" );
    m_pComp = new Particle2MCLinker( this, Particle2MCMethod::Composite, "" );
  } else {
    m_pChi2 = new Particle2MCLinker( this, Particle2MCMethod::Chi2, "" );
  }
  
  return StatusCode::SUCCESS;
}

StatusCode TupleToolMCTruth::fill( const LHCb::Particle* 
				 , const LHCb::Particle* P
				 , const std::string& head
				 , Tuples::Tuple& tuple ){

  Assert( ( !m_useChi2Method && m_pLink && m_pComp ) 
	  ||
	  ( m_useChi2Method || m_pChi2 )
	  , "One of your associator hasn't been initialized!");
  
  int mcPid = 0;
  int nbAss = 0;
  double mcTau = -1;

  Gaudi::XYZVector endVertex, originVertex;
  Gaudi::LorentzVector trueP;

  const MCParticle* mcp(0);
  bool test = true;

  if( P ){
    if ( m_useChi2Method ){
      double w=0;
      nbAss = m_pChi2->associatedMCP( P );
      mcp = m_pChi2->firstMCP( P, w );
      
      test &= tuple->column( head+"_MC_CHI2WHEIGHT", w );

    } else {
      if( P->isBasicParticle() ) mcp = m_pLink->firstMCP( P );
      else                       mcp = m_pComp->firstMCP( P );
    }
  }
  
  // pointer is ready, prepare the values:
  if( mcp ){

    mcPid = mcp->particleID().pid();

    trueP = mcp->momentum();

    const SmartRefVector< LHCb::MCVertex > & endVertices = mcp->endVertices();
    endVertex = endVertices.front()->position(); // the first item, the other are discarded.
    originVertex = mcp->originVertex()->position();

    // lifetime
    if( m_storePT ){
      Gaudi::XYZVector dist = endVertex - originVertex;
      // copied from DecayChainNTuple, old version. Hope that's correct...
      mcTau = trueP.M() * dist.Dot( trueP.Vect() ) / trueP.Vect().mag2();
      mcTau /= Gaudi::Units::picosecond * Gaudi::Units::c_light;
    }
  }

  // fill the tuple:
  test &= tuple->column( head+"_TRUEID", mcPid );  

  if( m_storeNumberOfAssoc )
    test &= tuple->column( head + "_MC_ASSOCNUMBER", nbAss );

  if( m_storeKinetic )
    test &= tuple->column( head + "_TRUEP", trueP );

  if( m_storeVertexes ){
    test &= tuple->column( head + "_TRUEORIGINVERTEX_", originVertex );
    test &= tuple->column( head + "_TRUEENDVERTEX_", endVertex );
  }

  if( m_storePT )
    test &= tuple->column( head + "_TRUETAU", mcTau );

  return StatusCode(test);
}

