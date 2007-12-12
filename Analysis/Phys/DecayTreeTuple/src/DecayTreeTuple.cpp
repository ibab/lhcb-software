// $Id: DecayTreeTuple.cpp,v 1.1.1.1 2007-12-12 17:46:43 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartIF.h"

#include "Kernel/IDecayFinder.h"

// #include "GaudiAlg/Tuple.h"
// #include "GaudiAlg/TupleObj.h"

// local
#include "DecayTreeTuple.h"

#include "Kernel/IParticleTupleTool.h"
#include "Kernel/IEventTupleTool.h"
#include "TupleToolDecay.h"

#include "GaudiKernel/IRegistry.h" // IOpaqueAddress

#include <boost/regex.hpp> 


//-----------------------------------------------------------------------------
// Implementation file for class : DecayTreeTuple
//
// 2007-11-01 : Jeremie Borel
//-----------------------------------------------------------------------------
using namespace Gaudi;
using namespace LHCb;
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DecayTreeTuple );

// ===============================================================
// ===============================================================
// ===============================================================

// void DecayTreeTuple::DecayVertex::fill( const LHCb::Particle& p ){
//   const LHCb::Vertex * vtx = p.endVertex ();
//   if( !vtx ) return ;

//   const XYZPoint & pos = vtx->position ();
//   x = pos.x();
//   y = pos.y();
//   z = pos.z();

//   const SymMatrix3x3 & mat = vtx->covMatrix ();
//   ex = sqrt( mat(0,0) );
//   ey = sqrt( mat(1,1) );
//   ez = sqrt( mat(2,2) );

//   chi2 = vtx->chi2();
//   ndof = vtx->nDoF();
// }

//=============================================================================
//=============================================================================

DecayTreeTuple::OnePart::OnePart( DecayTreeTuple* parent
				  , const Particle& me
				  , const std::string& head )
  : m_head( head )
  , m_parent( parent )
  , m_mother(0)
{
  if( !m_parent ){ // should not happen
    std::cout << "Error: parent interface not set!" << std::endl;
    return;
  }
  int pid = me.particleID().pid();
  m_realname = m_parent->ppSvc()->findByStdHepID( pid )->particle();
}

// -----------------------------------------------------
DecayTreeTuple::OnePart::~OnePart(){}

// -----------------------------------------------------
// small interface methode:

std::string DecayTreeTuple::OnePart::headName(){ 
  return m_head; 
};
void DecayTreeTuple::OnePart::headName( const std::string& h ){
  m_head=h; 
};
    
std::string DecayTreeTuple::OnePart::getRealName(){ 
  return m_realname;
};
const std::string& DecayTreeTuple::OnePart::getRealName() const { 
  return m_realname;
};
std::string DecayTreeTuple::OnePart::info() const {
  return m_realname + " (" + m_head + ")";
}

void DecayTreeTuple::OnePart::setMother( const OnePart* mother ){
  m_mother = mother;
}
const DecayTreeTuple::OnePart* DecayTreeTuple::OnePart::getMother() const { 
  return m_mother; 
};

DecayTreeTuple* DecayTreeTuple::OnePart::parent(){ 
  return m_parent; 
};

void DecayTreeTuple::OnePart::addDaughter( OnePart* d ){
  m_daughters.push_back( d );
}
int DecayTreeTuple::OnePart::depth() const {
  if( m_mother ) return m_mother->depth()+1;
  return 0;
}

// -----------------------------------------------------
void DecayTreeTuple::OnePart::addTool( IParticleTupleTool* tool ){
  m_tools.push_back( tool ); 
};
std::vector< IParticleTupleTool* >& DecayTreeTuple::OnePart::tools( ){
  return m_tools;
};
void DecayTreeTuple::OnePart::clearTools(){ 
  m_tools.clear();
};

// -----------------------------------------------------
bool DecayTreeTuple::OnePart::fill( Tuples::Tuple& tuple
				    , const Particle* mother
				    , const Particle* pp ){
  bool test = true;
  for( std::vector< IParticleTupleTool* >::iterator it = m_tools.begin();
       m_tools.end()!=it; ++it ){
    bool localTest = (*it)->fill( mother, pp, headName(), tuple );
    test &= localTest;
    if( localTest ){}
    else {
      m_parent->Warning("Tool '" + (*it)->type() + "' acting on particle '"
			+ headName() + "' returned a failure status." );
    }
  }
  return test;
}
std::vector<std::string> DecayTreeTuple::OnePart::toolList() const {
  std::vector<std::string> v;
  v.reserve( m_tools.size() );
  for( std::vector<IParticleTupleTool*>::const_iterator it=m_tools.begin();
       m_tools.end()!=it; ++ it ){
    v.push_back( (*it)->type() );
  }
  return v;
}

// -----------------------------------------------------
void DecayTreeTuple::OnePart::printStructure( std::ostream& os, bool verbose ) const {

  int dd = depth();
  std::string i;
  if( dd ) i = std::string( 3*dd, ' ' ); //not the same as std::string(char*,int)
  i.append( info() );
  if( !verbose ) os << i << "\n";
  else {
    std::vector<std::string> l = toolList();
    os << "|" << i << std::setw( 20-i.size() ) <<  " " << ":";
    os << DecayTreeTuple::join( l.begin(), l.end(), ", ", "none" ) << "\n";
  }

  if( m_daughters.empty() ) return;

  for( std::vector<const OnePart*>::const_iterator cit=m_daughters.begin();
       cit != m_daughters.end(); ++cit ){
    (*cit)->printStructure( os, verbose );
  }
}

//=============================================================================
//================= class DecayTreeTuple ======================================
//=============================================================================

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

  debug() << "==> Initialize" << endmsg;

  if( initializeDecays() )
    return StatusCode::SUCCESS;
  return StatusCode::FAILURE;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode DecayTreeTuple::execute() {

  debug() << "==> Execute" << endmsg;

  const LHCb::Particle::ConstVector& mothers = desktop()->particles();
  if( mothers.empty() ){
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }
  verbose() << "I have " << mothers.size()
	    << " particles to handle" << endreq;

  Tuple tuple = nTuple( m_tupleName,  m_tupleName );

  Particle::ConstVector heads;
  StatusCode test = getDecayMatches( mothers, heads );
  if( test ){
    verbose() << "There is " << heads.size()
	      << " top particles matching the decay." << endreq;
  }
  else {
    verbose() << "No particle matching the decay." << endreq;
    setFilterPassed(false);
    return StatusCode::SUCCESS;
  }
  
  test = fillTuple( tuple, heads );

  if( test ){
    verbose() << "NTuple sucessfully filled" << endreq;
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

  debug() << "==> Finalize" << endmsg;

  for( std::vector<OnePart*>::iterator it = m_parts.begin();
       m_parts.end() != it; ++it ){
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

std::string DecayTreeTuple::escape( const std::string& input ) {

  const int size = input.size();
  if( size == 0 ) return "";

  // all chars must be matched, or they fall in the catchall
  const char *expr = 
    "([[:word:]])" // 1 : allowed chars
    "|^(~)"        // 2 : starting ~
    "|([~])"       // 3 : then ok
    "|(\\*)"       // 4 : star
    "|([+])"       // 5 : +
    "|([-])"       // 6 : -
    "|(.)";        // 7 : default catchall.
    
  const char* replace =
    "(?1$1)" 
    "(?2anti)"
    "(?3~)"
    "(?4st)"
    "(?5plus)"
    "(?6minus)"
    "(?7_)";

  boost::regex e( expr );
  std::string ret;
  ret = boost::regex_replace( input, e, replace
			      , boost::match_default | boost::format_all );

  // remove double __ and trailing and leading _
  e.assign( "(^_+)|(_+$)|(_{2,})" );
  ret = boost::regex_replace( ret, e, "(?1)(?2)(?3_)"
			      , boost::match_default | boost::format_all );

  return ret;

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
    debug() << "Try now to instanciate " << mit->first << endreq;
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
    debug() << "No sub decay to create" << endreq;
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
      std::vector<OnePart*>::iterator op;
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
      verbose() << *it << " instantiated as an Event related tool" << endreq;
    if( !test1 && test2 )
      verbose() << *it << " instantiated as a Particle related tool" << endreq;
  }

  debug() << "Generic and inherited tool list successfully created" << endreq;
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
      tuple->column( "nCandidate", nCandidates );
      ++nCandidates;
    } else {
      Warning("Failed to fill a candidate. Will skip it.");
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

void DecayTreeTuple::initializeOnePartsStufferTools( OnePart* P
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

  verbose() << "Remains " << remainTools.size() << " to inherit" << std::endl;

  // inherit again the remaining global tools:
  std::vector<std::string>::const_iterator it = remainTools.begin();
  if( m->inheritTools() ){
    for( ; remainTools.end() != it ; ++it ){
      // find the right tool:
      bool flag = false;
      for( int k=0; k<(int)m_pTools.size(); ++k ){
	if( *it == m_pTools[k]->type() ){
	  verbose() << "Parts " << P->info() << " inherits "
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
//   debug() << "Writing IOpaqueAddress to " << m_eventColName << endreq;

//   // pick up the location of the event --
//   // this is what makes the tag collection a collection...
//   DataObject* pObject = get<DataObject>("/Event");
//   if ( pObject ) {
//     debug() << "IOpaqueAddress is "
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
    OnePart *p = new OnePart( this, *(row[i]) , getBranchName(row[i]) );
    // inherit the default properties:
    m_parts.push_back( p );
  }

  debug() << "There is " << m_parts.size()
	  << " particle to initialize." << endreq;

  // set the base properties...
  initializeStufferTools();
  // set the branch names and inherit the particle specific tools
  matchSubDecays( row );

  // re-creating mother->daughter relationship,
  // allows better printout later on
  for( int i=0; i<(int)row.size(); ++i ){
    OnePart* Mother = m_parts[i];
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
  std::string tList = DecayTreeTuple::join( tools.begin(), tools.end() );
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
      // loop on the matched particles and find the associated OnePart* object
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
	m_parts[off]->headName( escape(n) );
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

  int pid = p->particleID().pid();
  std::string realname = ppSvc()->findByStdHepID( pid )->particle();

  std::string name = escape( realname ), buffer = name;

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
