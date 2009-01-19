// $Id: TupleToolMCTruth.cpp,v 1.10 2009-01-19 18:07:45 pkoppenb Exp $
// Include files
#include "gsl/gsl_sys.h"
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/PhysicalConstants.h"
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
  , m_linkerTool_Links(0)
  , m_linkerTool_Chi2(0)
  , m_linkerTool_Composite(0)
{
  declareInterface<IParticleTupleTool>(this);
  
  // Associator input location. Empty should be fine for most of the case
  declareProperty( "InputLocations", m_assocInputs = std::vector<std::string>(1,"") );

  // Use the chi2 associator instead of the link and composite associator
  declareProperty( "UseChi2Method", m_useChi2Method=false );

  // Store the number of associations seen
  declareProperty( "StoreAssociationNumbers", m_storeNumberOfAssoc=false );

  // Store the true angular information 
  declareProperty( "FillAngles", m_fillangles = false );
}

//=============================================================================

StatusCode TupleToolMCTruth::initialize(){
  if( ! GaudiTool::initialize() ) return StatusCode::FAILURE;

  m_linkerTool_Links = tool<IDaVinciAssociatorsWrapper>("DaVinciAssociatorsWrapper","Wrapper_Links",this);
  m_linkerTool_Chi2 = tool<IDaVinciAssociatorsWrapper>("DaVinciAssociatorsWrapper","Wrapper_Chi2",this);
  m_linkerTool_Composite = tool<IDaVinciAssociatorsWrapper>("DaVinciAssociatorsWrapper","Wrapper_Composite",this);
  m_mcTool = tool<IMCParticleTupleTool>("MCTupleToolMCTruth",this);
  if (m_fillangles) m_mcAngles = tool<IMCParticleTupleTool>("MCTupleToolP2VV",this);
    
  return StatusCode::SUCCESS;
}

//=============================================================================
// Fill
//=============================================================================
StatusCode TupleToolMCTruth::fill( const LHCb::Particle* 
				 , const LHCb::Particle* P
				 , const std::string& head
				 , Tuples::Tuple& tuple ){

  m_pLink = m_linkerTool_Links->linker(Particle2MCMethod::Links,m_assocInputs); 
  m_pComp = m_linkerTool_Composite->linker(Particle2MCMethod::Composite,m_assocInputs);
  m_pChi2 = m_linkerTool_Chi2->linker(Particle2MCMethod::Chi2,m_assocInputs);

  Assert( ( !m_useChi2Method && m_pLink && m_pComp ) 
	  ||
	  ( m_useChi2Method || m_pChi2 )
	  , "One of your associator hasn't been initialized!");
  
  int assignedPid = 0;
  
  const MCParticle* mcp(0);
  bool test = true;
  int nbAss = 0;

  if( P ){
    assignedPid = P->particleID().pid();
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

  if( m_storeNumberOfAssoc )
    test &= tuple->column( head + "_MC_ASSOCNUMBER", nbAss );
  
  test &= m_mcTool->fill(0,mcp,head,tuple);
  
  // true angles information:
  if(m_fillangles) {
    // only for Bs or Bd (patch, not meant to be elegant)
    if (abs(assignedPid) == 511 || abs(assignedPid) == 531) {
      m_mcAngles->fill(0,mcp,head,tuple);
    }
    
  }
  
  return StatusCode(test);
}

