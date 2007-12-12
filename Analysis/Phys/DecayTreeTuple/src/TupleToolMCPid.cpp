// $Id: TupleToolMCPid.cpp,v 1.1.1.1 2007-12-12 17:46:43 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolMCPid.h"

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
DECLARE_TOOL_FACTORY( TupleToolMCPid );

using namespace LHCb;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolMCPid::TupleToolMCPid( const std::string& type,
				    const std::string& name,
				    const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_pLink(0)
  , m_pComp(0)
  , m_pChi2(0)
{
  declareInterface<IParticleTupleTool>(this);
  
  declareProperty( "InputLocations", m_assocInputs );
  declareProperty( "UseChi2Method", m_useChi2Method=false );
}

//=============================================================================

StatusCode TupleToolMCPid::initialize(){
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

StatusCode TupleToolMCPid::fill( const LHCb::Particle* 
				   , const LHCb::Particle* P
				   , const std::string& head
				   , Tuples::Tuple& tuple ){

  Assert( ( !m_useChi2Method && m_pLink && m_pComp ) 
	  ||
	  ( m_useChi2Method || m_pChi2 )
	  , "One of your associator hasn't been initialized!");
  
  int mcPid = 0;
  int nbAss = 0;
  const MCParticle* mcp(0);

  if( P ){
    if ( m_useChi2Method ){
      double w=0;
      nbAss = m_pChi2->associatedMCP( P );
      mcp = m_pChi2->firstMCP( P, w );
      if( ! tuple->column( head+"_MCWHEIGHT", w ) ) return StatusCode::FAILURE;
    } else {
      if( P->isBasicParticle() ) mcp = m_pLink->firstMCP( P );
      else                       mcp = m_pComp->firstMCP( P );
    }
    if( mcp ) mcPid = mcp->particleID().pid();
  }
  
  if( ! tuple->column( head+"_MCPID", mcPid ) ) return StatusCode::FAILURE;

  return StatusCode::SUCCESS;
}

// const GaudiAlgorithm* TupleToolMCPid::getParent() const {
//   // want to find the parent Algoritm, if any:
//   const IInterface* ii = parent();
//   const GaudiAlgorithm* alg(0);
//   const AlgTool* tool(0);

//   while( ii ){
//     tool = dynamic_cast<const AlgTool*>( ii );
//     alg = dynamic_cast<const GaudiAlgorithm*>( ii );
//     if( alg ){
//       // I'm happy, that's what I want.
//       break;
//     } 
//     if( tool ){ // its a tool, but maybe the parent is an Algorithm ?
//       ii = tool->parent();
//     }
//   }

//   return alg;
// }
