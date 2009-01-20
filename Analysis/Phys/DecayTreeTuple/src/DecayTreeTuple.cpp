// $Id: DecayTreeTuple.cpp,v 1.10 2009-01-20 18:31:30 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "Kernel/IDecayFinder.h"

#include "Kernel/IParticleTupleTool.h"
#include "Kernel/IMCParticleTupleTool.h"
#include "Kernel/IEventTupleTool.h"
#include "TupleToolDecay.h"

#include "boost/lexical_cast.hpp" 
#include "Kernel/Escape.h"


// local
#include "DecayTreeTuple.h"

using namespace LHCb;
using namespace Gaudi ;
//-----------------------------------------------------------------------------
// Implementation file for class : DecayTreeTuple
//
// 2009-01-20 : Patrick Koppenburg based on Jérémie Borel's initial vesion
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( DecayTreeTuple );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DecayTreeTuple::DecayTreeTuple( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : DecayTreeTupleBase ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
DecayTreeTuple::~DecayTreeTuple() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode DecayTreeTuple::initialize() {
  StatusCode sc = DecayTreeTupleBase::initialize(); 
  if ( sc.isFailure() ) return sc;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Execute
//=============================================================================
StatusCode DecayTreeTuple::execute(){
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
  
  setFilterPassed(test);
  // Mandatory. Set to true if event is accepted.  
  return StatusCode::SUCCESS;
  
}
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
    } else {
      Warning("Failed to fill some variable, will skip this candidate.");
    }
  }


  return test;
}
//=============================================================================

StatusCode DecayTreeTuple::fillParticles( Tuples::Tuple& tuple
                                          , const Particle::ConstVector& row ){
  if( sizeCheckOrInit( row ) ){
    bool test = true;
    const int size = m_parts.size();
    for( int k=0; size>k; ++k ){ // row[0] is the deday head.
      //      test &= m_parts[k]->fill( tuple, row[0], row[k] );
      test &= fillOnePart( m_parts[k], tuple, row[0], row[k] );
    }
    return StatusCode(test);
  }
  return StatusCode::FAILURE;
}
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

void DecayTreeTuple::matchSubDecays( const Particle::ConstVector& row ){

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
//  Finalize
//=============================================================================
StatusCode DecayTreeTuple::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  return DecayTreeTupleBase::finalize();
}
//=============================================================================
// Moved from OnePart
//=============================================================================
bool DecayTreeTuple::fillOnePart( Decays::OnePart* op 
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
