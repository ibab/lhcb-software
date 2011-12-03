// $Id: TupleToolMCTruth.cpp,v 1.17 2010-01-26 15:39:26 rlambert Exp $
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

// kernel
#include "Kernel/IParticle2MCAssociator.h"
//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolMCTruth
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
  : TupleToolBase ( type, name , parent )
  , m_p2mcAssoc(0)
  , m_p2mcAssocType("DaVinciSmartAssociator")
  , m_toolList(1,"MCTupleToolKinematic")
  , m_mcTools(0)

{
  declareInterface<IParticleTupleTool>(this);

  // The names of MCTupleTools to use on the associated mcp
  declareProperty( "ToolList", m_toolList  );
  declareProperty( "IP2MCPAssociatorType", m_p2mcAssocType );

}

//=============================================================================

StatusCode TupleToolMCTruth::initialize()
{
  const StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  m_p2mcAssoc = tool<IParticle2MCAssociator>(m_p2mcAssocType,
                                             this);

  //initialise the tools
  std::sort( m_toolList.begin(), m_toolList.end() );
  m_toolList.erase( std::unique(m_toolList.begin(),m_toolList.end()), m_toolList.end() );
  for( std::vector<std::string>::const_iterator it = m_toolList.begin(); m_toolList.end()!=it ; ++it )
  {
    if (msgLevel(MSG::VERBOSE)) verbose() << "Adding the tool " << *it << endmsg ;
    IMCParticleTupleTool* aTool=tool<IMCParticleTupleTool>(*it,this);
    if(aTool) m_mcTools.push_back(aTool);
    else Warning("There was a problem retrieving " + *it +" , this tool will be ignored");
  }

  if (msgLevel(MSG::VERBOSE)) {
    verbose() << "Completed TupleTool intialisation, "
              << m_mcTools.size()
              << " tools added " << endmsg ;
  }

  return sc;
}

StatusCode TupleToolMCTruth::fill( const LHCb::Particle*
                                   , const LHCb::Particle* P
                                   , const std::string& head
                                   , Tuples::Tuple& tuple )
{

  const std::string prefix=fullName(head);

  Assert( m_p2mcAssoc
          , "The DaVinci smart associator hasn't been initialized!");


  int mcPid=0;
  bool test = true;
  const LHCb::MCParticle* mcp(0);

  if ( P )
  {
    //assignedPid = P->particleID().pid();
    if (msgLevel(MSG::VERBOSE)) verbose() << "TupleToolMCTruth::getting related MCP to " << P << endmsg ;
    mcp = m_p2mcAssoc->relatedMCP(P);
    if (msgLevel(MSG::VERBOSE)) verbose() << "TupleToolMCTruth::got mcp " << mcp << endmsg ;
  }

  // pointer is ready, prepare the values:
  if( mcp )
  {
    mcPid=mcp->particleID().pid();
  }

  // fill the tuple:
  test &= tuple->column( prefix+"_TRUEID", mcPid );

  // fill all requested MCTools
  for(std::vector< IMCParticleTupleTool* >::const_iterator it=m_mcTools.begin(); it!=m_mcTools.end(); it++)
    test &=(*it)->fill(NULL,mcp,prefix,tuple);

  return StatusCode(test);
}

