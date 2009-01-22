// $Id: DecayTreeTupleBase.cpp,v 1.8 2009-01-22 09:27:33 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartIF.h"
#include "Kernel/IDecayFinder.h"
#include "MCInterfaces/IMCDecayFinder.h"

#include "DecayTreeTupleBase.h"

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
  declareProperty( "UseLabXSyntax", m_useLabName = false );

  declareProperty( "UseToolNameForBranchName", m_tupleNameAsToolName = true );

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

  if (msgLevel(MSG::DEBUG)) debug() << "==> Initialize (doing nothing)" << endmsg;
  return StatusCode::SUCCESS;
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
bool DecayTreeTupleBase::initializeDecays( bool isMC) {
  // main decay initialization
  
  if (isMC) {
    m_mcdkFinder = tool<IMCDecayFinder>( "MCDecayFinder", this );
    if( !m_mcdkFinder->setDecay( m_headDecay ) ){
      Error( "Cannot initialize the main decay '" + m_headDecay+ "' properly." );
      return false; 
    }
    info() << "Will look for " << m_mcdkFinder->decay() << endreq;
  } else {    
    m_dkFinder = tool<IDecayFinder>( "DecayFinder", this );
    if( !m_dkFinder->setDecay( m_headDecay ) ){
      Error( "Cannot initialize the main decay '" + m_headDecay+ "' properly." );
      return false;
    }
    info() << "Will look for " << m_dkFinder->decay() << endreq;
  }

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
/// get daughters
LHCb::MCParticle::ConstVector DecayTreeTupleBase::daughtersVector(const LHCb::MCParticle* d) const{
  if (0==d) Exception("NULL MCParticle");
  LHCb::MCParticle::ConstVector dau ;
  for ( SmartRefVector< LHCb::MCVertex >::const_iterator v = d->endVertices().begin() ;
        v != d->endVertices().end() ; ++v){
    if ((*v)->isDecay()){
      for ( SmartRefVector< LHCb::MCParticle >::const_iterator p = (*v)->products().begin() ;
            p != (*v)->products().end() ; ++p){
        dau.push_back(*p);
      }
    }
  }
  if (msgLevel(MSG::VERBOSE)) verbose() << d->particleID().pid() << " has " 
                                        << dau.size() << " daughters" << endmsg ;
  return dau ;
}
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
//=========================================================================
// Check unicity of names  
//=========================================================================
bool DecayTreeTupleBase::checkUnicity() const {
  // check the name unicity 
  std::set<std::string> names;
  for( int k=0; k<(int)m_parts.size(); ++k ){
    std::string n = m_parts[k]->headName();
    if( names.count( n )>0 ){
      Error("You are using two times the name " + n 
            + " for your tuple branches.");
      return false;
    }
  }
  return true ;
}
//=========================================================================
// print infos
//=========================================================================
void DecayTreeTupleBase::printInfos() const {
  // initalization done, printing some infos:

  // generic tool info:
  std::stringstream tmp;
  std::vector<std::string> tools = getEventTools();
  std::string tList = Decays::join( tools.begin(), tools.end() );
  tmp << "Event related tools: " << tList;

  tmp << "\nParticle related stuffers: realname (tuplename) \n";
  for( int i=0; i<(int)m_parts.size(); ++i ){
    if( !m_parts[i]->getMother() ){
      m_parts[i]->printStructure( tmp, msgLevel( MSG::INFO ) );
    }
  }
  info() << "Tree " << m_tupleName << " initialized:\n"
         << tmp.str() << endreq ;
  
}
// ===============================================================
// this is where all the difference between MC particle and Particle occurs
void DecayTreeTupleBase::matchSubDecays( const Particle::ConstVector& row ,
                                         Particle::ConstVector& buffer, 
                                         const TupleToolDecay* mit){

  const Particle* head = row[0];
  mit->decayFinder()->decayMembers( head, buffer );
  findAssociatedOneParts( buffer, row, mit, m_pTools );
}
// ===============================================================
// this is where all the difference between MC particle and Particle occurs
void DecayTreeTupleBase::matchSubDecays( const MCParticle::ConstVector& row,
                                         MCParticle::ConstVector& buffer, 
                                         const TupleToolDecay* mit){
  const MCParticle* head = row[0];
  mit->mcDecayFinder()->decayMembers( head, buffer );
  // this is the cause of all the difference between MC particle and Particle occurs
  findAssociatedOneParts( buffer, row, mit, m_mcTools ); 
}
//=============================================================================
// Moved from OnePart
//=============================================================================
bool DecayTreeTupleBase::fillOnePart( Decays::OnePart* op 
                            , Tuples::Tuple& tuple
                            , const Particle* mother
                            , const Particle* pp )
{
  bool test = true;
  for( std::vector< IParticleTupleTool* >::iterator it = op->tools().begin();
       op->tools().end()!=it; ++it ){
    bool localTest = (*it)->fill( mother, pp, op->headName(), tuple );
    test &= localTest;
    if( localTest ){}
    else {
      Warning("Tool '" + (*it)->type() + "' acting on particle '"+ op->headName() + "' returned a failure status." );
    }
  }
  return test;
}
//=============================================================================
// Same for MCParticle (cannot template trivially because of IParticleTupleTool)
//=============================================================================
bool DecayTreeTupleBase::fillOnePart( Decays::OnePart* op 
                            , Tuples::Tuple& tuple
                            , const MCParticle* mother
                            , const MCParticle* pp )
{
  bool test = true;
  for( std::vector< IMCParticleTupleTool* >::iterator it = op->mctools().begin();
       op->mctools().end()!=it; ++it ){
    bool localTest = (*it)->fill( mother, pp, op->headName(), tuple );
    test &= localTest;
    if( localTest ){}
    else {
      Warning("Tool '" + (*it)->type() + "' acting on particle '"+ op->headName() + "' returned a failure status." );
    }
  }
  return test;
}
//=============================================================================
bool DecayTreeTupleBase::getDecayMatches( const Particle::ConstVector& pool
                                          , Particle::ConstVector& heads )
{
  const Particle* head(0);
  while( m_dkFinder->findDecay( pool, head ) ){
    heads.push_back( head ); 
  }
  return !( heads.empty() );  
}
//=============================================================================
bool DecayTreeTupleBase::getDecayMatches( const MCParticle::ConstVector& pool
                                          , MCParticle::ConstVector& heads )
{
  const MCParticle* head(0);
  while( m_mcdkFinder->findDecay( pool, head ) ){
    heads.push_back( head ); 
  }
  return !( heads.empty() );  
}

