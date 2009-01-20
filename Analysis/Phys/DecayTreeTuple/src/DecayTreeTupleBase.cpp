// $Id: DecayTreeTupleBase.cpp,v 1.4 2009-01-20 18:31:30 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartIF.h"
#include "Kernel/IDecayFinder.h"
#include "MCInterfaces/IMCDecayFinder.h"

#include "DecayTreeTupleBase.h"

#include "Kernel/IParticleTupleTool.h"
#include "Kernel/IMCParticleTupleTool.h"
#include "Kernel/IEventTupleTool.h"

#include "boost/lexical_cast.hpp" 
#include "Kernel/Escape.h"

#include "GaudiKernel/IRegistry.h" // IOpaqueAddress

//-----------------------------------------------------------------------------
// Implementation file for class : DecayTreeTupleBase
//
// 2007-11-01 : Jeremie Borel
//-----------------------------------------------------------------------------
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DecayTreeTupleBase );

using namespace Gaudi ;
using namespace LHCb ;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DecayTreeTupleBase::DecayTreeTupleBase( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
  , m_dkFinder(0)
{
  declareProperty( "Branches", m_decayMap );
  declareProperty( "Decay", m_headDecay );
  declareProperty( "TupleName", m_tupleName="DecayTree" );
  declareProperty( "UseLabXSyntax", m_useLabName = false );

  declareProperty( "UseToolNameForBranchName", m_tupleNameAsToolName = true );

  // fill some default value
  m_toolList.push_back( "TupleToolKinematic" );
  m_toolList.push_back( "TupleToolPid" );
  m_toolList.push_back( "TupleToolGeometry" );
  m_toolList.push_back( "TupleToolEventInfo" );
  
  declareProperty( "ToolList", m_toolList );

}
//=============================================================================
// Destructor
//=============================================================================
DecayTreeTupleBase::~DecayTreeTupleBase() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DecayTreeTupleBase::initialize() {
  StatusCode sc = DVAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  if (msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;

  if( initializeDecays() ) return StatusCode::SUCCESS;
  return StatusCode::FAILURE;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode DecayTreeTupleBase::execute() {

  err() << "==> Do not call DecayTreeTupleBase::Execute" << endmsg;
  return StatusCode::FAILURE;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode DecayTreeTupleBase::finalize() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;
  return DVAlgorithm::finalize(); 
}
//=============================================================================
//=============================================================================

bool DecayTreeTupleBase::initializeDecays() {
  // main decay initialization

  m_dkFinder = tool<IDecayFinder>( "DecayFinder", this );
  if( !m_dkFinder->setDecay( m_headDecay ) ){
    Error( "Cannot initialize the main decay '" + m_headDecay
           + "' properly." );
    return false;
  }
  info() << "Will look for " << m_dkFinder->decay() << endreq;

  // sub-decays initialization (if any)
  m_decays.reserve( m_decayMap.size() );
  std::map<std::string,std::string>::iterator mit;
  for( mit=m_decayMap.begin(); m_decayMap.end()!=mit; ++mit ){
    if (msgLevel(MSG::DEBUG)) debug() << "Try now to instanciate " << mit->first << endreq;
    TupleToolDecay *m = tool<TupleToolDecay>( "TupleToolDecay", mit->first, this );
    if( !m->initialize( mit->second, false ) ){
      Error( "Cannot initialize '" + mit->first + "' branch properly, skipping it." );
      continue;
    }
    m_decays.push_back( m );
  }

  if( !m_decays.empty() ) {
    info() << "Explicit sub decays :" << m_decays.size() << endreq;
    for( int i=0; i<(int)m_decays.size(); ++i ){
      m_decays[i]->printInfo();
    }
  } else {
    if (msgLevel(MSG::DEBUG)) debug() << "No sub decay to create" << endreq;
  }
  return true;
}
//=============================================================================
//=============================================================================

std::vector<std::string> DecayTreeTupleBase::getEventTools() const {
  std::vector<std::string> ret;
  ret.reserve( m_eTools.size() );
  std::vector<IEventTupleTool*>::const_iterator it = m_eTools.begin();
  for( ; m_eTools.end() != it; ++it ) ret.push_back( (*it)->type() );
  return ret;
}
//=============================================================================
//=============================================================================

//=============================================================================
//=============================================================================
//=============================================================================
//=============================================================================

StatusCode DecayTreeTupleBase::fillEventRelatedVariables( Tuples::Tuple& tuple ){
  if( !m_eTools.empty() ){
    // const IPhysDesktop* desk = desktop();
    for( std::vector< IEventTupleTool* >::iterator it = m_eTools.begin();
         m_eTools.end() != it; ++it ){
      if( ! (*it)->fill( tuple ) ) return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }
  return true;

}

//=============================================================================
//=============================================================================

bool DecayTreeTupleBase::getDecayMatches( const Particle::ConstVector& pool
                                      , Particle::ConstVector& heads ){
  const Particle* head(0);
  while( m_dkFinder->findDecay( pool, head ) ){
    heads.push_back( head );
  }
  return !( heads.empty() );
}

//=============================================================================
//=============================================================================


int DecayTreeTupleBase::getOffset( const Particle* p, const Particle::ConstVector& v, bool secure ){
  Particle::ConstVector::const_iterator it, f;
  f = std::find( v.begin(), v.end(), p );
  if( secure ) Assert( f != v.end() );
  return f-v.begin();
}

// ===============================================================
// ===============================================================
std::string DecayTreeTupleBase::getBranchName( const Particle* p ){
  if( m_useLabName )
    return std::string("lab") 
      + boost::lexical_cast<std::string>( m_parts.size() );
  
  std::string realname = ppSvc()->find( p->particleID() )->particle();
  
  std::string name = Decays::escape( realname ), buffer = name;

  // check that it is not yet used, if yes, append a number until not
  // used.
  bool flag = false;
  int kk = 0;
  do{
    flag = false;
    for( int k=0; k<(int)m_parts.size(); ++k ){
      if( buffer == m_parts[k]->headName() ){
        flag = true;
        break;
      }
    }
    if( !flag ) break;
    buffer = name + boost::lexical_cast<std::string>( kk );
    ++kk;
  } while( kk<100 ); //for security.
  return buffer;
}
//=============================================================================
std::vector<std::string> DecayTreeTupleBase::getParticleTools(const std::vector< IParticleTupleTool* > pTools ) const {
  std::vector<std::string> ret;
  ret.reserve( pTools.size() );
  std::vector<IParticleTupleTool*>::const_iterator it = pTools.begin();
  for( ; pTools.end() != it; ++it ) ret.push_back( (*it)->type() );
  return ret;
}
//=============================================================================
