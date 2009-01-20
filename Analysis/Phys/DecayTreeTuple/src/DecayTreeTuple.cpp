// $Id: DecayTreeTuple.cpp,v 1.7 2009-01-20 09:19:40 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartIF.h"

#include "Kernel/IDecayFinder.h"
#include "Kernel/Escape.h"

#include "DecayTreeTuple.h"

#include "Kernel/IParticleTupleTool.h"
#include "Kernel/IEventTupleTool.h"
#include "TupleToolDecay.h"
#include "OnePart.h"
#include "Event/Particle.h"

#include "GaudiKernel/IRegistry.h" // IOpaqueAddress

#include "boost/lexical_cast.hpp" 


//-----------------------------------------------------------------------------
// Implementation file for class : DecayTreeTuple
//
// 2007-11-01 : Jeremie Borel
//-----------------------------------------------------------------------------
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DecayTreeTuple );

using namespace Gaudi ;
using namespace LHCb ;
//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DecayTreeTuple::DecayTreeTuple( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
  , m_dkFinder(0)
  , m_nSuccessEvent(0)
  , m_nSuccessCandidate(0)
  , m_nFailedEvent(0)
  , m_nFailedCandidate(0)
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
DecayTreeTuple::~DecayTreeTuple() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode DecayTreeTuple::initialize() {
  StatusCode sc = DVAlgorithm::initialize();
  if ( sc.isFailure() ) return sc;

  if (msgLevel(MSG::DEBUG)) debug() << "==> Initialize" << endmsg;

  if( initializeDecays() ) return StatusCode::SUCCESS;
  return StatusCode::FAILURE;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode DecayTreeTuple::execute() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute" << endmsg;

  const LHCb::Particle::ConstVector& mothers = desktop()->particles();
  if( mothers.empty() ){
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }
  if (msgLevel(MSG::VERBOSE)) verbose() << "I have " << mothers.size()
                                        << " particles to handle" << endreq;

  Tuple tuple = nTuple( m_tupleName,  m_tupleName );

  LHCb::Particle::ConstVector heads;
  StatusCode test = getDecayMatches( mothers, heads );
  if( test ){
    if (msgLevel(MSG::VERBOSE)) verbose() << "There is " << heads.size()
                                          << " top particles matching the decay." << endreq;
  }
  else {
    if (msgLevel(MSG::VERBOSE)) verbose() << "No particle matching the decay." << endreq;
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }
  
  test = fillTuple( tuple, heads );

  if( test ){
    if (msgLevel(MSG::VERBOSE)) verbose() << "NTuple sucessfully filled" << endreq;
  }
  //   StatusCode test ;

  //   if( 1 ){
  //     DataObject* pObject = get<DataObject>("/Event");
  //     if (pObject!=0 ) {

  //       Tuple etc = evtCol( "MyTuple" );
  //       // std::cout << "address : " << pObject->registry()->address() << std::endl;

  //       bool toto=true;
  //       test = etc->column( "Address", pObject->registry()->address() );
  //       toto = test.isSuccess();
  //       std::cout << "BOOKING ETC ( toto : " << toto << ")" << std::endl;
  //       std::cout << "BOOKING ETC ( test : " << test << ")" << std::endl;
  //       test = etc->column( "runNumber", 1 );
  //       etc->column( "eventNumber", 2 );
  //       etc->write();       
  //     } else {
  //       Error("not able to retrieve IOpaqueAddress");
  //       return StatusCode::FAILURE;
  //     }

  //   }

  //  setFilterPassed(false);  // Mandatory. Set to true if event is accepted.
  setFilterPassed(test);  // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode DecayTreeTuple::finalize() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;

  for( std::vector<Decays::OnePart*>::iterator it = m_parts.begin();m_parts.end() != it; ++it ){
    if( *it ){
      delete *it;
      *it = 0; // not really usefull
    }
  }

  unsigned int totEvent = m_nFailedEvent + m_nSuccessEvent;
  unsigned int totCand = m_nFailedCandidate + m_nSuccessCandidate;
  info() << totEvent << " non empty events and " 
         << totCand << " candidates processed." << endmsg;
  info() << "Event with one or more skipped candidate rate:" 
         << m_nFailedEvent << "/" <<  totEvent << " = "
         << (totEvent!=0 ? m_nFailedEvent/totEvent : 0 ) << "." << endmsg;

  info() << "Proportion of skipped candidates: " 
         << m_nFailedCandidate << "/" << totCand << " = " 
         << (totCand!=0 ? m_nFailedCandidate / totCand : 0 ) << "." << endmsg;


  return DVAlgorithm::finalize(); //=== For DC04, return StatusCode::SUCCESS;
}
//=============================================================================
//=============================================================================

bool DecayTreeTuple::initializeDecays() {
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
    if( !m->initialize( mit->second ) ){
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

std::vector<std::string> DecayTreeTuple::getEventTools() const {
  std::vector<std::string> ret;
  ret.reserve( m_eTools.size() );
  std::vector<IEventTupleTool*>::const_iterator it = m_eTools.begin();
  for( ; m_eTools.end() != it; ++it ) ret.push_back( (*it)->type() );
  return ret;
}
std::vector<std::string> DecayTreeTuple::getParticleTools() const {
  std::vector<std::string> ret;
  ret.reserve( m_pTools.size() );
  std::vector<IParticleTupleTool*>::const_iterator it = m_pTools.begin();
  for( ; m_pTools.end() != it; ++it ) ret.push_back( (*it)->type() );
  return ret;
}

//=============================================================================
//=============================================================================
void DecayTreeTuple::initializeStufferTools(){
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
    SmartIF<IParticleTupleTool> test2(tt);
    if( test2 ){
      m_pTools.push_back( test2 );

      // inherit by default: give all the tools to the particles:
      std::vector<Decays::OnePart*>::iterator op;
      for( op=m_parts.begin(); op!=m_parts.end(); ++op ){
        (*op)->addTool( test2 );
      }
    }

    if( !test1 && !test2 ){
      Error("Can't get the tool '" + *it + "', check you're syntax" );
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
StatusCode DecayTreeTuple::fillTuple( Tuples::Tuple& tuple
                                      , const Particle::ConstVector& heads ){
  Particle::ConstVector::const_iterator pit = heads.begin();
  Particle::ConstVector row;

  int nCandidates=0;
  StatusCode test = StatusCode::FAILURE;

  for( ; heads.end()!=pit; ++pit ){
    test = StatusCode::FAILURE;
    row.clear();
    m_dkFinder->decayMembers( *pit, row );
    row.insert( row.begin(), *pit ); // must insert the head as it cant be flagged.

    if( fillParticles( tuple, row ) ){
      tuple->column( "nCandidate", nCandidates ).ignore();
      ++nCandidates;
    } else {
      Warning("Failed to fill a candidate. Will skip it.").ignore();
      continue;
    }

    test = fillEventRelatedVariables( tuple );

    if( test ){
      test = tuple->write();
      ++m_nSuccessCandidate;
    } else {
      Warning("Failed to fill some variable, will skip this candidate.");
      ++m_nFailedCandidate;
    }
  }

  if( test ){
    ++m_nSuccessEvent;
  } else {
    ++m_nFailedEvent;
  }

  return test;
}

//=============================================================================
//=============================================================================

void DecayTreeTuple::initializeOnePartsStufferTools( Decays::OnePart* P
                                                     , const TupleToolDecay* m ){

  // there is a specific descriptor for P, i.e. default settings are wrong
  P->clearTools();

  // tool list must become : specific + (inherited-specific)
  std::vector<std::string> remainTools;
  std::vector<std::string> globalTools = getParticleTools();
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
      for( int k=0; k<(int)m_pTools.size(); ++k ){
        if( *it == m_pTools[k]->type() ){
          if (msgLevel(MSG::VERBOSE)) verbose() << "Parts " << P->info() << " inherits "
                                                << m_pTools[k]->type() << endreq;
          P->addTool( m_pTools[k] );
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
    IParticleTupleTool* tt = tool<IParticleTupleTool>( *it, m );
    if( !tt ){
      Error("Can't instanciate the tool '" + *it + "', check your syntax");
      continue;
    }
    P->addTool( tt );
  }
}
//=============================================================================
//=============================================================================

StatusCode DecayTreeTuple::fillParticles( Tuples::Tuple& tuple
                                          , const Particle::ConstVector& row ){
  if( sizeCheckOrInit( row ) ){
    bool test = true;
    const int size = m_parts.size();
    for( int k=0; size>k; ++k ){ // row[0] is the deday head.
      test &= m_parts[k]->fill( tuple, row[0], row[k] );
    }
    return StatusCode(test);
  }
  return StatusCode::FAILURE;
}

//=============================================================================
//=============================================================================

StatusCode DecayTreeTuple::fillEventRelatedVariables( Tuples::Tuple& tuple ){
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

bool DecayTreeTuple::getDecayMatches( const Particle::ConstVector& pool
                                      , Particle::ConstVector& heads ){
  const Particle* head(0);
  while( m_dkFinder->findDecay( pool, head ) ){
    heads.push_back( head );
  }
  return !( heads.empty() );
}

//=============================================================================
//=============================================================================
// StatusCode DecayTreeTuple::writeEventAddress( Tuples::Tuple& toot ){
//   Tuples::Tuple tuple = evtCol( m_tupleName, m_tupleName );
//   if (msgLevel(MSG::DEBUG)) debug() << "Writing IOpaqueAddress to " << m_eventColName << endreq;

//   // pick up the location of the event --
//   // this is what makes the tag collection a collection...
//   DataObject* pObject = get<DataObject>("/Event");
//   if ( pObject ) {
//     if (msgLevel(MSG::DEBUG)) debug() << "IOpaqueAddress is "
// 	    << pObject->registry()->address() << endreq;
//     if( tuple->column("Address", pObject->registry()->address() ) )
//       return StatusCode::SUCCESS;
//   } 
//   tuple->column("bbo", 2 );
  
 
//   Error("Not able to retrieve IOpaqueAddress, this is a fatal error as the two tuples are not syncronized anymore!");
//   //  Assert( 0 );
//   return StatusCode::FAILURE;
// }
//=============================================================================
//=============================================================================

bool DecayTreeTuple::sizeCheckOrInit( const Particle::ConstVector& row ){
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
    Decays::OnePart *p = new Decays::OnePart( this, *(row[i]) , getBranchName(row[i]) );
    // inherit the default properties:
    m_parts.push_back( p );
  }

  if (msgLevel(MSG::DEBUG)) debug() << "There is " << m_parts.size()
                                    << " particle to initialize." << endreq;

  // set the base properties...
  initializeStufferTools();
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

int DecayTreeTuple::getOffset( const Particle* p, const Particle::ConstVector& v, bool secure ){
  Particle::ConstVector::const_iterator it, f;
  f = std::find( v.begin(), v.end(), p );
  if( secure ) Assert( f != v.end() );
  return f-v.begin();
}

// ===============================================================
// ===============================================================

void DecayTreeTuple::matchSubDecays( const Particle::ConstVector& row ){

  std::vector<TupleToolDecay*>::iterator mit;
  Particle::ConstVector buffer;

  const Particle* head = row[0];

  // is there any special name for this Particle ?:
  for( mit=m_decays.begin(); m_decays.end()!=mit; ++mit ){
    if( !*mit ) continue;

    buffer.clear();
    (*mit)->decayMembers( head, buffer );

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
      initializeOnePartsStufferTools( m_parts[off], *mit );
    }
  }
}

// follow the options, 
std::string DecayTreeTuple::getBranchName( const Particle* p ){
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
