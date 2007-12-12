// $Id: TupleToolDecay.cpp,v 1.1.1.1 2007-12-12 17:46:43 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolDecay.h"
#include "Kernel/IParticleTupleTool.h"

#include "Kernel/IDecayFinder.h"
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Kernel/DVAlgorithm.h"

#include <boost/lexical_cast.hpp>

// #include <boost/algorithm/string/replace.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : TupleTool
//
// 2007-11-02 : Jeremie Borel
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolDecay );


using namespace Gaudi;
using namespace LHCb;


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolDecay::TupleToolDecay( const std::string& type,
				const std::string& name,
				const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_hasMatched( false )
  , m_myName(name)
  , m_dkFinder(0)
{
  declareInterface<TupleToolDecay>(this);

  declareProperty( "ToolList", m_stufferList );
  declareProperty( "InheritTools", m_inheritTools = true );
}

//=============================================================================
// Destructor
//=============================================================================
TupleToolDecay::~TupleToolDecay() {}

//=============================================================================

StatusCode TupleToolDecay::initialize( const std::string& dcy ){

  std::string pname; // sets the name to "ToolName" and not to "AlgoParent.ToolName"
  const Algorithm* alg = dynamic_cast<const Algorithm*>( parent() );
  if( alg ) pname = alg->name();

  if( pname.empty() )
    Warning("Did someone change the name structure ???, your printout will not be nice..." );
  else {
    m_myName = m_myName.substr( pname.size()+1 );
  }

  m_dkFinder = tool<IDecayFinder>("DecayFinder", this );

  bool test = bool( m_dkFinder );
  test &= m_dkFinder->setDecay( dcy );

  debug() << "Initialized " << name()
	  << " with decay " << decay() << endreq;

  std::sort( m_stufferList.begin(), m_stufferList.end() );
  std::unique( m_stufferList.begin(), m_stufferList.end() );

  return StatusCode::StatusCode( test );
}

std::string TupleToolDecay::getInfo() const {
  return name() + " :" + decay();
}
void TupleToolDecay::printInfo() const {
  info() << getInfo() << endreq;
}
std::string TupleToolDecay::decay() const {
  return m_dkFinder->decay();
}

void TupleToolDecay::decayMembers ( const LHCb::Particle *head
				    , LHCb::Particle::ConstVector &members){
  m_dkFinder->decayMembers( head, members );
}
