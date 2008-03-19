// $Id: TupleToolMCHierarchy.cpp,v 1.1 2008-03-19 20:26:28 sposs Exp $
// Include files
#include "gsl/gsl_sys.h"
// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/PhysicalConstants.h"
// local
#include "TupleToolMCHierarchy.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Event/Particle.h"
#include "Event/MCParticle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EventInfoTupleTool
//
// 2008-02-28 : Stephane Poss
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolMCHierarchy );

using namespace LHCb;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolMCHierarchy::TupleToolMCHierarchy( const std::string& type,
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

}

//=============================================================================

StatusCode TupleToolMCHierarchy::initialize(){
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
    m_pLink = new Particle2MCLinker( this,
                                     Particle2MCMethod::Links,
                                     std::vector<std::string>(1,"") );
    m_pComp = new Particle2MCLinker( this,
                                     Particle2MCMethod::Composite,
                                     std::vector<std::string>(1,"") );
  } else {
    m_pChi2 = new Particle2MCLinker( this,
                                     Particle2MCMethod::Chi2,
                                     std::vector<std::string>(1,""));
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
StatusCode TupleToolMCHierarchy::fill( const LHCb::Particle*
                                       , const LHCb::Particle* P
                                       , const std::string& head
                                       , Tuples::Tuple& tuple ){

  Assert( ( !m_useChi2Method && m_pLink && m_pComp )
          ||
          ( m_useChi2Method || m_pChi2 )
          , "One of your associator hasn't been initialized!");


  int mc_mother_id = 0;
  int mc_mother_key = 0;
  int mc_gd_mother_id = 0;
  int mc_gd_mother_key = 0;
  int mc_gd_gd_mother_id = 0;
  int mc_gd_gd_mother_key = 0;


  const MCParticle* mcp(0);
  bool test = true;

  if( P ){
    if ( m_useChi2Method ){
      double w=0;
      mcp = m_pChi2->firstMCP( P, w );

    } else {
      if( P->isBasicParticle() ) mcp = m_pLink->firstMCP( P );
      else                       mcp = m_pComp->firstMCP( P );
    }
  }

  // pointer is ready, prepare the values:
  if( mcp ){
    const MCParticle* mcpmom(0);
    mcpmom = mcp->mother();
    if(mcpmom){
      mc_mother_id = mcpmom->particleID().pid();
      mc_mother_key = mcpmom->key();
      const MCParticle* mcpmom_mom(0);
      mcpmom_mom = mcpmom->mother();
      if(mcpmom_mom){
        mc_gd_mother_id = mcpmom_mom->particleID().pid();
        mc_gd_mother_key = mcpmom_mom->key();
        const MCParticle* mcpmom_mom_mom(0);
        mcpmom_mom_mom = mcpmom_mom->mother();
        if(mcpmom_mom_mom){
          mc_gd_gd_mother_id = mcpmom_mom_mom->particleID().pid();
          mc_gd_gd_mother_key = mcpmom_mom_mom->key();
        }
      }
    }
  }

  // fill the tuple:
  test &= tuple->column( head+"_MC_MOTHER_ID", mc_mother_id );
  test &= tuple->column( head+"_MC_MOTHER_KEY", mc_mother_key );
  test &= tuple->column( head+"_MC_GD_MOTHER_ID", mc_gd_mother_id );
  test &= tuple->column( head+"_MC_GD_MOTHER_KEY", mc_gd_mother_key );
  test &= tuple->column( head+"_MC_GD_GD_MOTHER_ID", mc_gd_gd_mother_id );
  test &= tuple->column( head+"_MC_GD_GD_MOTHER_KEY", mc_gd_gd_mother_key );

  return StatusCode(test);
}

