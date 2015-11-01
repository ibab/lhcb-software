// $Id: DecayTreeTupleBase.cpp,v 1.15 2010-03-01 11:47:34 rlambert Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartIF.h"
#include "Kernel/IDecayFinder.h"
#include "MCInterfaces/IMCDecayFinder.h"
#include "DecayTreeTupleBase/DecayTreeTupleBase.h"
#include "GaudiKernel/IRegistry.h" // IOpaqueAddress
#include "LoKi/Trees.h"
#include "GaudiKernel/AlgTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DecayTreeTupleBase
//
// 2007-11-01 : Jeremie Borel
//-----------------------------------------------------------------------------

using namespace Gaudi ;
using namespace LHCb ;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DecayTreeTupleBase::DecayTreeTupleBase( const std::string& name,
                                        ISvcLocator* pSvcLocator)
  : DaVinciTupleAlgorithm ( name , pSvcLocator )
  , m_mcdkFinder          ( NULL               )
  , m_dkFinder            ( NULL               )
  , m_mcdecayTree         ( Decays::Trees::Invalid_<const LHCb::MCParticle*>() )
  , m_decayTree           ( Decays::Trees::Invalid_<const LHCb::Particle*>() )
{
  declareProperty( "TupleName", m_tupleName = "" );
  declareProperty( "Branches", m_decayMap, "Branches with other tools" );
  declareProperty( "Decay", m_headDecay, "decay descriptor" );
  declareProperty( "UseLabXSyntax", m_useLabName = false, "Use labX syntax" );
  declareProperty( "UseToolNameForBranchName", m_tupleNameAsToolName = true );
  declareProperty( "RevertToPositiveID", m_revertToPositiveID = true );
  declareProperty( "UseLoKiDecayFinders", m_useLoKiDecayFinders = true );
}

//=============================================================================
// Destructor
//=============================================================================
DecayTreeTupleBase::~DecayTreeTupleBase() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DecayTreeTupleBase::initialize()
{
  const StatusCode sc = DaVinciTupleAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  if (msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;
  if ( m_useLabName && m_revertToPositiveID )
  {
    m_revertToPositiveID = false ;
    return Warning( "UseLabXSyntax and RevertToPositiveID are exclusive. Switched off reverting." );
  }
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DecayTreeTupleBase::execute()
{
  err() << "==> Do not call DecayTreeTupleBase::Execute" << endmsg;
  return StatusCode::FAILURE;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode DecayTreeTupleBase::finalize()
{
  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;
  //the DecayTools are explicitly initialized, and should be explicitly finalized/released
  for ( std::vector<ITupleToolDecay*>::iterator di = m_decays.begin();
        di != m_decays.end(); ++di )
  {
    (*di)->finalize();
    //(*di)->release();
  }
  return DaVinciTupleAlgorithm::finalize();
}

//=============================================================================
//=============================================================================
bool DecayTreeTupleBase::initializeDecays( const bool isMC )
{
  // main decay initialization

  if ( useLoKiDecayFinders() ) {
    if ( isMC ) {
      Decays::IMCDecay* mcdecaytool = tool<Decays::IMCDecay>( "LoKi::MCDecay", this );
      if ( ! mcdecaytool ) {
        Error("Could not retrieve MC-decay finder").ignore();
        return false;
      }
      m_mcdecayTree = mcdecaytool->tree(m_headDecay);
      if ( ! m_mcdecayTree ) {
        Error( "Unable to decode/parse MC-decay descriptor '" + m_headDecay + "'." ).ignore();
        return false;
      }
      info() << "Will look for " << m_mcdecayTree << endmsg;
    } else {
      Decays::IDecay* decaytool = tool<Decays::IDecay>( "LoKi::Decay", this );
      if ( ! decaytool ) {
        Error("Could not retrieve decay finder").ignore();
        return false;
      }
      m_decayTree = decaytool->tree(m_headDecay);
      if ( !m_decayTree ) {
        Error( "Unable to decode/parse decay descriptor '" + m_headDecay + "'." ).ignore();
        return false;
      }
      info() << "Will look for " << m_decayTree << endmsg;
    }
  } else {
    if (isMC)
    {
      m_mcdkFinder = tool<IMCDecayFinder>( "MCDecayFinder", this );
      if ( !m_mcdkFinder->setDecay( m_headDecay ) )
      {
        Error( "Cannot initialize the main decay '" + m_headDecay+ "' properly." ).ignore();
        return false;
      }
      info() << "Will look for " << m_mcdkFinder->decay() << endmsg;
    }
    else
    {
      m_dkFinder = tool<IDecayFinder>( "DecayFinder", this );
      if( !m_dkFinder->setDecay( m_headDecay ) )
      {
        Error( "Cannot initialize the main decay '" + m_headDecay+ "' properly." ).ignore();
        return false;
      }
      info() << "Will look for " << m_dkFinder->decay() << endmsg;
    }
  }

  // sub-decays initialization (if any)
  m_decays.reserve( m_decayMap.size() );
  std::map<std::string,std::string>::iterator mit;
  for( mit=m_decayMap.begin(); m_decayMap.end()!=mit; ++mit )
  {
    if (msgLevel(MSG::DEBUG)) debug() << "Try now to instanciate " << mit->first << endmsg;
    ITupleToolDecay * m = tool<ITupleToolDecay>( "TupleToolDecay", mit->first, this );
    if ( !Gaudi::Utils::setProperty(m,"UseLoKiDecayFinders",useLoKiDecayFinders()) )
    {
      Error( "Failed to set tool property 'UseLoKiDecayFinders'" ).ignore();
      return false;
    }
    if( !m->initializeDecay( mit->second, isMC ) )
    {
      Error( "Cannot initialize '" + mit->first + "' branch properly, skipping it." ).ignore();
      continue;
    }
    m_decays.push_back( m );
  }

  if( !m_decays.empty() )
  {
    info() << "Explicit sub decays :" << m_decays.size() << endmsg;
    for ( std::vector<ITupleToolDecay*>::const_iterator di = m_decays.begin();
          di != m_decays.end(); ++di )
    {
      (*di)->printInfo();
    }
  }
  else
  {
    if (msgLevel(MSG::DEBUG)) debug() << "No sub decay to create" << endmsg;
  }
  return true;
}
//=============================================================================
//=============================================================================
std::vector<std::string> DecayTreeTupleBase::getEventTools() const
{
  std::vector<std::string> ret;
  ret.reserve( m_eTools.size() );
  for ( std::vector<IEventTupleTool*>::const_iterator it = m_eTools.begin();
        m_eTools.end() != it; ++it ) 
  {
    ret.push_back( (*it)->type() ); 
  }
  return ret;
}
//=============================================================================
//=============================================================================
// get daughters
LHCb::MCParticle::ConstVector 
DecayTreeTupleBase::daughtersVector(const LHCb::MCParticle* d) const
{
  if ( !d ) Exception("NULL MCParticle");
  LHCb::MCParticle::ConstVector dau ;
  for ( SmartRefVector< LHCb::MCVertex >::const_iterator v = d->endVertices().begin() ;
        v != d->endVertices().end() ; ++v)
  {
    if ((*v)->isDecay())
    {
      for ( SmartRefVector< LHCb::MCParticle >::const_iterator p = (*v)->products().begin() ;
            p != (*v)->products().end(); ++p)
      {
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
StatusCode DecayTreeTupleBase::fillEventRelatedVariables( Tuples::Tuple& tuple )
{
  if( !m_eTools.empty() )
  {
    for( std::vector< IEventTupleTool* >::iterator it = m_eTools.begin();
         m_eTools.end() != it; ++it )
    {
      if( ! (*it)->fill( tuple ) ) return StatusCode::FAILURE;
    }
    return StatusCode::SUCCESS;
  }
  return true;
}
//=========================================================================
// Check unicity of names
//=========================================================================
bool DecayTreeTupleBase::checkUnicity() const
{
  // check the name unicity
  std::set<std::string> names;
  for ( std::vector<Decays::OnePart*>::const_iterator iP = m_parts.begin();
        iP != m_parts.end(); ++iP )
  {
    const std::string n = (*iP)->headName();
    if ( names.count(n) > 0 )
    {
      Error("You are using two times the name " + n
            + " for your tuple branches.").ignore();
      return false;
    }
  }
  return true;
}
//=========================================================================
// print infos
//=========================================================================
void DecayTreeTupleBase::printInfos() const
{
  info() << "Tree " << m_tupleName << " initialized :-" << endmsg;

  info() << " Event related tools : " << getEventTools() << endmsg;

  info() << " Particle related stuffers :- " << endmsg;
  for ( std::vector<Decays::OnePart*>::const_iterator iP = m_parts.begin();
        iP != m_parts.end(); ++iP )
  {
    if( ! (*iP)->getMother() )
    {
      (*iP)->printStructure( info(), msgLevel(MSG::INFO) );
    }
  }
}
// ===============================================================
// this is where all the difference between MC particle and Particle occurs
void DecayTreeTupleBase::matchSubDecays( const Particle::ConstVector& row ,
                                         Particle::ConstVector& buffer,
                                         const ITupleToolDecay* mit)
{
  if ( !mit )
    Exception("DecayTreeTupleBase::matchSubDecays ITupleToolDecay undefined");

  if ( mit->useLoKiDecayFinders() ? ( !mit->decayTree() ) : ( !mit->decayFinder() ) )
    Exception("DecayTreeTupleBase::matchSubDecays ITupleToolDecay DecayFinder or DecayTree undefined");

  if ( !row[0] )
    Exception("DecayTreeTupleBase::matchSubDecays DecayTreeTupleBase::Null Particle");

  const Particle* head = row[0];
  if (msgLevel(MSG::VERBOSE))
    verbose() << "Match subdecay for Particle " << head->particleID().pid() << endmsg ;
  if ( mit->useLoKiDecayFinders() ) {
    const Decays::IDecay::iTree& tree = mit->decayTree();
    if ( tree(head) ) {
      if ( tree.marked() ) {
        tree.collect(buffer);
      } else {
        buffer.push_back(head);
      }
    }
  } else {
    mit->decayFinder()->decayMembers( head, buffer );
  }
  if (msgLevel(MSG::VERBOSE))
    verbose() << "Decay finder gets " << buffer.size() << " particles for "
              << mit->decay() << endmsg ;
  findAssociatedOneParts( buffer, row, mit, m_pTools );
}
// ===============================================================
// this is where all the difference between MC particle and Particle occurs
void DecayTreeTupleBase::matchSubDecays( const MCParticle::ConstVector& row,
                                         MCParticle::ConstVector& buffer,
                                         const ITupleToolDecay* mit)
{
  if ( !mit )
    Exception("DecayTreeTupleBase::matchSubDecays ITupleToolDecay undefined");

  if ( mit->useLoKiDecayFinders() ? ( !mit->mcDecayTree() ) : ( !mit->mcDecayFinder() ) )
    Exception("DecayTreeTupleBase::matchSubDecays ITupleToolDecay MCDecayFinder undefined");

  if ( !row[0] )
    Exception("DecayTreeTupleBase::matchSubDecays DecayTreeTupleBase::Null MC Particle");

  const MCParticle* head = row[0];
  if (msgLevel(MSG::VERBOSE))
    verbose() << "Match subdecay for MCParticle " << head->particleID().pid() << endmsg ;
  if ( mit->useLoKiDecayFinders() ) {
    const Decays::IMCDecay::iTree& tree = mit->mcDecayTree();
    if ( tree(head) ) {
      if ( tree.marked() ) {
        tree.collect(buffer);
      } else {
        buffer.push_back(head);
      }
    }
  } else {
    mit->mcDecayFinder()->decayMembers( head, buffer );
  }
  if (msgLevel(MSG::VERBOSE))
    verbose() << "Decay finder gets " << buffer.size() << " mc particles for "
              << mit->decay() << endmsg ;
  // this is the cause of all the difference between MC particle and Particle occurs
  findAssociatedOneParts( buffer, row, mit, m_mcTools );
  if (msgLevel(MSG::VERBOSE))
    verbose() << "Looked for associated Parts " << endmsg ;
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
  if (msgLevel(MSG::DEBUG)) debug() << "FillOnePart " << pp->particleID().pid() << endmsg ;
  for( std::vector< IParticleTupleTool* >::iterator it = op->tools().begin();
       op->tools().end()!=it; ++it )
  {
    if (msgLevel(MSG::DEBUG))
      debug() << "FillOnePart " << pp->particleID().pid() << " in " << (*it)->type() << endmsg ;
    if (msgLevel(MSG::VERBOSE))
      verbose() <<  "FillOnePart " << mother << " " << op->headName()  << endmsg ;
    const bool localTest = (*it)->fill( mother, pp, op->headName(), tuple );
    test &= localTest;
    if ( !localTest )
    {
      fatal () << "Tool '" << (*it)->type() << "' acting on particle '" << op->headName() 
               << "' returned a failure status." << endmsg ;
      return StatusCode::FAILURE ;
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
  if (msgLevel(MSG::DEBUG))
    debug() << "FillOnePart MC " << pp->particleID().pid() << " " << op->headName() << endmsg ;
  for( std::vector< IMCParticleTupleTool* >::iterator it = op->mctools().begin();
       op->mctools().end()!=it; ++it )
  {
    if (msgLevel(MSG::DEBUG))
      debug() << "FillOnePart MC " << pp->particleID().pid() << " in " << (*it)->type() << endmsg ;
    const bool localTest = (*it)->fill( mother, pp, op->headName(), tuple );
    test &= localTest;
    if ( !localTest )
    {
      Warning( "Tool '" + (*it)->type() + "' acting on particle '" +
               op->headName() + "' returned a failure status." ).ignore();
    }
  }
  return test;
}
//=============================================================================
bool DecayTreeTupleBase::getDecayMatches( const Particle::ConstVector& pool
                                          , Particle::ConstVector& heads )
{
  const Particle* head(0);
  while( m_dkFinder->findDecay( pool, head ) )
  {
    if (!head)
    {
      Error("Cannot find head of decay",StatusCode::FAILURE,1).ignore();
      return false ;
    }
    heads.push_back( head );
  }
  return !( heads.empty() );
}

//=============================================================================

bool DecayTreeTupleBase::getDecayMatches( const MCParticle::ConstVector& pool
                                          , MCParticle::ConstVector& heads )
{
  const MCParticle* head(0);
  while( m_mcdkFinder->findDecay( pool, head ) )
  {
    heads.push_back( head );
  }
  return !( heads.empty() );
}

// ============================================================================

// Get branch name for given particle
std::string DecayTreeTupleBase::getBranchName( const std::string& realname ) const
{
  if ( m_useLabName )
  {
    return ( std::string("lab") +
             boost::lexical_cast<std::string>( m_parts.size() ) );
  }

  std::string name = Decays::escape( realname ), buffer = name;

  // check that it is not yet used, if yes, append a number until not used.
  bool flag = false;
  int kk = 0;
  do
  {
    flag = false;
    for ( std::vector<Decays::OnePart*>::const_iterator iP = m_parts.begin();
          iP != m_parts.end(); ++iP )
    {
      if ( buffer == (*iP)->headName() )
      {
        flag = true;
        break;
      }
    }
    if ( !flag ) break;
    buffer = name + boost::lexical_cast<std::string>( kk );
    ++kk;
  } while( kk<100 ); //for security.
  return buffer;
}

// ============================================================================
