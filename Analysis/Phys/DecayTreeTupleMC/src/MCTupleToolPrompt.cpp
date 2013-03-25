// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// local
#include "MCTupleToolPrompt.h"

// from GaudiAlg
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

// from Event
#include "Event/Particle.h"
#include "Event/MCParticle.h"

// from Kernel
#include "Kernel/IParticle2MCAssociator.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : MCTupleToolPrompt
//
// 2011-06-13 : Philip John Hunt
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCTupleToolPrompt )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  MCTupleToolPrompt::MCTupleToolPrompt( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
    : TupleToolBase ( type, name , parent )
    , m_p2mcAssoc(NULL)
    , m_p2mcAssocType("DaVinciSmartAssociator")
{
  declareInterface<IParticleTupleTool>(this);

  // maximum lifetime of short-lived particles (ns)
  declareProperty( "MaxLifetime", m_maxLifetime = 1e-7
                   , "Maximum lifetime of short-lived particles (ns)" );
  declareProperty( "StoreLongLivedParticleID", m_storeLongLivedPid=true);
  declareProperty( "IP2MCPAssociatorType", m_p2mcAssocType);
}

StatusCode MCTupleToolPrompt::initialize()
{
  StatusCode sc = TupleToolBase::initialize();
  if (sc.isFailure()) return sc;

  if (isVerbose()) m_storeLongLivedPid = true;

  sc = service( "LHCb::ParticlePropertySvc", m_ppSvc );

  if (sc.isFailure()) return sc;

  m_p2mcAssoc = tool<IParticle2MCAssociator>(m_p2mcAssocType, this);

  return sc;
}

StatusCode MCTupleToolPrompt::fill( const LHCb::Particle*
                                    , const LHCb::Particle* P
                                    , const std::string& head
                                    , Tuples::Tuple& tuple )
{


  const std::string prefix = fullName(head);

  Assert (P && m_p2mcAssoc, "No particle found , or misconfigured DaVinci smart associator" );

  if (P->isBasicParticle()) return StatusCode::SUCCESS; // don't fill for basic particles
  bool test=true;

  int longLivedPid=0;
  int longLivedKey=0;

  int isPrompt=-1;

  int mcParentPid=0;
  int mcParentKey=0;

  double lcl_lifetime;

  const LHCb::MCParticle *mcp = 0;

  if (msgLevel(MSG::VERBOSE)) verbose() << "MCTupleToolPrompt::getting related MCP to " << P << endmsg ;
  mcp = m_p2mcAssoc->relatedMCP(P);
  if (msgLevel(MSG::VERBOSE)) verbose() << "TupleToolMCTruth::got mcp " << mcp << endmsg ;

  const LHCb::MCParticle *mcp_parent = mcp;

  ParticleProperty * lclPprop;

  if (msgLevel(MSG::DEBUG)) debug() << "MCTupleToolPrompt::fill " << head << endmsg;

  if (mcp)
  {
    isPrompt=1;
    while( isPrompt && mcp_parent->mother() )
    {
      mcp_parent = mcp_parent->mother();

      mcParentPid=mcp_parent->particleID().pid();
      mcParentKey=mcp_parent->key();

      lclPprop = m_ppSvc->findByStdHepID(mcp_parent->particleID().pid());
      lcl_lifetime = lclPprop->lifetime();

      if(msgLevel(MSG::DEBUG))
      {
        debug() << "Particle: " << lclPprop->evtGenName()
                << "  lifetime: " << lcl_lifetime << endmsg;
      }

      if ( lcl_lifetime > m_maxLifetime*Gaudi::Units::ns )
      {
        isPrompt=0;
        longLivedPid=mcParentPid;
        longLivedKey=mcParentKey;
      }
    }
  }

  if (msgLevel(MSG::DEBUG)) debug() << "MCTupleToolPrompt::fill - filling "
                                    << head << endmsg;

  test &= tuple->column( prefix+"_MC_ISPROMPT", isPrompt );
  if (m_storeLongLivedPid)
  {
    test &= tuple->column( prefix+"_MC_LONGLIVED_ID", longLivedPid );
    test &= tuple->column( prefix+"_MC_LONGLIVED_KEY", longLivedKey );
  }
  return StatusCode(test);
}

//=============================================================================
// Destructor
//=============================================================================
MCTupleToolPrompt::~MCTupleToolPrompt() {}

//=============================================================================
