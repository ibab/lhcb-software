// $Id: DecayTreeTupleBase.cpp,v 1.5 2009-01-21 08:50:26 pkoppenb Exp $
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

StatusCode DecayTreeTupleBase::fillParticles( Tuples::Tuple& tuple,
                                          const Particle::ConstVector& row ){
  if ( sizeCheckOrInit( row ) ){
    bool test = true;
    const int size = m_parts.size();
    for( int k=0; size>k; ++k ){ // row[0] is the deday head.
      test &= fillOnePart( m_parts[k], tuple, row[0], row[k] );
    }
    return StatusCode(test);
  }
  return StatusCode::FAILURE;
}
//=============================================================================
//=============================================================================
bool DecayTreeTupleBase::sizeCheckOrInit( const Particle::ConstVector& row ){
  const unsigned int size = row.size();
  if( m_parts.size() == size ) return true;

  if( !m_parts.empty() ){
    Error( "The number of matched particles with the DecayFinder ("
           + m_dkFinder->decay()
           + ") has changed. Skipping the candidate.");
    return false;
  }

  info() << "Entering the initialization process" << endreq;

  // initializing the particles object.
  m_parts.reserve( size );
  for( unsigned int i=0; i<size; ++i ){
    Decays::OnePart *p = new Decays::OnePart( ppSvc()->find ( row[i]->particleID() )->particle(), getBranchName(row[i]) );
    // inherit the default properties:
    m_parts.push_back( p );
  }

  if (msgLevel(MSG::DEBUG)) debug() << "There is " << m_parts.size()
                                    << " particle to initialize." << endreq;

  // set the base properties...
  initializeStufferTools(m_pTools);
  // set the branch names and inherit the particle specific tools
  matchSubDecays( row );

  // re-creating mother->daughter relationship,
  // allows better printout later on
  for( int i=0; i<(int)row.size(); ++i ){
    Decays::OnePart* Mother = m_parts[i];
    const Particle* mother = row[i];

    Particle::ConstVector dau = mother->daughtersVector();
    Particle::ConstVector::const_iterator dauit,f;
    for( dauit=dau.begin(); dau.end()!=dauit; ++dauit ){
      // am I in the search decay ?
      f = std::find( row.begin(), row.end(), *dauit );
      if( f==row.end() ) continue;
      int off = getOffset( *f, row );
      Mother->addDaughter( m_parts[off] );
      m_parts[off]->setMother( Mother );
    }
  }

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
         << tmp.str() << endreq;

  return true;
}
// ===============================================================
// ===============================================================

void DecayTreeTupleBase::matchSubDecays( const Particle::ConstVector& row ){

  std::vector<TupleToolDecay*>::iterator mit;
  Particle::ConstVector buffer;

  const Particle* head = row[0];

  // is there any special name for this Particle ?:
  for( mit=m_decays.begin(); m_decays.end()!=mit; ++mit ){
    if( !*mit ) continue;

    buffer.clear();
    (*mit)->decayFinder()->decayMembers( head, buffer );

    const int size=buffer.size();
    for( int k=0; k<size; ++k ){
      // loop on the matched particles and find the associated Decays::OnePart* object
      int off = getOffset( buffer[k], row, false );
      if( off == (int)row.size() ){
        Error("The decay descriptor \n'" + (*mit)->getInfo()
              +"'\n returned a match which is not also matched by your main decay descriptor. Ignoring it.");
        break;
      }
      if( m_tupleNameAsToolName ){
        std::string n = (*mit)->getName();
        // if there is more than one, append numerical values:
        if( size>1 ) n.append( boost::lexical_cast<std::string>(k) );
        m_parts[off]->headName( Decays::escape(n) );
      }
      // assign the correct tools:
      initializeOnePartsStufferTools( m_parts[off], *mit, m_pTools );
    }
  }
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
//=============================================================================
std::vector<std::string> DecayTreeTupleBase::getParticleTools(const std::vector< IParticleTupleTool* > pTools ) const {
  std::vector<std::string> ret;
  ret.reserve( pTools.size() );
  std::vector<IParticleTupleTool*>::const_iterator it = pTools.begin();
  for( ; pTools.end() != it; ++it ) ret.push_back( (*it)->type() );
  return ret;
}
//=============================================================================
//=============================================================================
// The templates need to go here
//=============================================================================
template<class ITUPLETOOL>
void DecayTreeTupleBase::initializeStufferTools(std::vector< ITUPLETOOL* >& pTools){
  std::sort( m_toolList.begin(), m_toolList.end() );
  std::unique( m_toolList.begin(), m_toolList.end() );
  
  // base instantiation:
  std::vector<std::string>::const_iterator it = m_toolList.begin();
  for( ; m_toolList.end()!=it ; ++it ){
    IAlgTool* tt = tool<IAlgTool>( *it, this );
    if( !tt ){
      Error("Can't get the tool '" + *it + "', check you're syntax" );
      continue;
    }
    // splitting in the correct list.   
    SmartIF<IEventTupleTool> test1(tt);
    if( test1 ) m_eTools.push_back( test1 );
    SmartIF<ITUPLETOOL> test2(tt);
    if( test2 ){
      pTools.push_back( test2 );
      
      // inherit by default: give all the tools to the particles:
      std::vector<Decays::OnePart*>::iterator op;
      for( op=m_parts.begin(); op!=m_parts.end(); ++op ){
        (*op)->addTool( test2 );
      }
    }
    
    if( !test1 && !test2 ){
      Error("Can't get the tool '" + *it + "', check your syntax" );
    }
    if( test1 && test2 ){
      Warning("The tool '" + *it +
              "', will be called both by the IParticleTupleTool" +
              " and IEventTupleTool interfaces. That's fine as long as you" +
              " know what you are doing." );
    }
    if( test1 && !test2 )
      if (msgLevel(MSG::VERBOSE)) verbose() << *it << " instantiated as an Event related tool" << endreq;
    if( !test1 && test2 )
      if (msgLevel(MSG::VERBOSE)) verbose() << *it << " instantiated as a Particle related tool" << endreq;
  }
  
  if (msgLevel(MSG::DEBUG)) debug() << "Generic and inherited tool list successfully created" << endreq;
}
//=============================================================================
//=============================================================================
template<class ITUPLETOOL> void
DecayTreeTupleBase::initializeOnePartsStufferTools( Decays::OnePart* P
                                                    , const TupleToolDecay* m
                                                    , std::vector< ITUPLETOOL* >& pTools   ){
  
  // there is a specific descriptor for P, i.e. default settings are wrong
  P->clearTools();
  
  // tool list must become : specific + (inherited-specific)
  std::vector<std::string> remainTools;
  std::vector<std::string> globalTools = getParticleTools(pTools);
  const std::vector<std::string>& locTools = m->getStuffers();
  
  std::insert_iterator<std::vector<std::string> > ii( remainTools
                                                      , remainTools.begin() );
  std::set_difference( globalTools.begin(), globalTools.end()
                       , locTools.begin(), locTools.end()
                       , ii );
  
  if (msgLevel(MSG::VERBOSE)) verbose() << "Remains " << remainTools.size() << " to inherit" << std::endl;
  
  // inherit again the remaining global tools:
  std::vector<std::string>::const_iterator it = remainTools.begin();
  if( m->inheritTools() ){
    for( ; remainTools.end() != it ; ++it ){
      // find the right tool:
      bool flag = false;
      for( unsigned int k=0; k< pTools.size(); ++k ){
        if( *it == pTools[k]->type() ){
          if (msgLevel(MSG::VERBOSE)) verbose() << "Parts " << P->info() << " inherits "
                                                << pTools[k]->type() << endreq;
          P->addTool( pTools[k] );
          flag = true;
          break;
        }
      }
      if( !flag )
        Warning( "Hmm, should not happen, the tool '" +
                 *it + "' will be ignored for some reason..." );
    }
  }
  
  // now instanciate the specific tools from the TupleToolDecay:
  for( it=locTools.begin(); locTools.end()!=it; ++it ){
    ITUPLETOOL* tt = tool<ITUPLETOOL>( *it, m );
    if( !tt ){
      Error("Can't instanciate the tool '" + *it + "', check your syntax");
      continue;
    }
    P->addTool( tt );
  }
}
