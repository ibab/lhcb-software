// $Id: DecayFinder.cpp,v 1.1 2002-05-20 23:09:43 gcorti Exp $
// Include files 
#include <list>
#include <functional>
#include <algorithm>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "CLHEP/Units/SystemOfUnits.h"
#include "Event/Particle.h"

// local
#include "DecayFinder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DecayFinder
//
// 23/04/2002 : Olivier Dormond
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const ToolFactory<DecayFinder>          s_factory ;
const        IToolFactory& DecayFinderFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DecayFinder::DecayFinder( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : AlgTool ( type, name , parent ),
    m_ppSvc(0), m_source("B0 -> pi+ pi-"), m_decay(0)
{
  if( serviceLocator() ) {
    StatusCode sc = StatusCode::FAILURE;
    sc = serviceLocator()->service("ParticlePropertySvc",m_ppSvc);
  }
  if( !m_ppSvc ) {
    throw GaudiException( "ParticlePropertySvc not found",
                          "MCDecayFinderException",
                          StatusCode::FAILURE );
  }

  declareInterface<IDecayFinder>(this);

  declareProperty( "Decay", m_source );
  declareProperty( "ResonnanceThreshold", m_resThreshold = 1e-15*second );
}

//=============================================================================
// Standard destructor
//=============================================================================

DecayFinder::~DecayFinder( )
{
  if( m_decay )
    delete m_decay;
}

//=============================================================================

StatusCode DecayFinder::initialize()
{
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Initializing" << endreq;
  if( compile(m_source) )
  {
    log << MSG::DEBUG << "The compilation of the decay successfull" << endreq;
    return StatusCode::SUCCESS;
  }
  log << MSG::DEBUG << "Could not compile the decay description" << endreq;
  return StatusCode::FAILURE;
}

#include "reclexer.icpp"
#include "recparser.icpp"

bool DecayFinder::compile( std::string &source )
{
  MsgStream log(msgSvc(), name());
  yy_buffer_state *bs = yy_scan_string( source.c_str() );
  try
  {
    if( yparse() )
      throw("Syntax Error");
  }
  catch( DescriptorError e )
  {
    log << MSG::ERROR << "Invalid decay description '"
        << m_source << "'" << endreq;
    log << MSG::ERROR << e.cause() << endreq;
    yy_delete_buffer(bs);
    return false;
  }
  yy_delete_buffer(bs);
  return true;
}

bool DecayFinder::hasDecay( const std::vector<Particle*> &event )
{
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "About to test the event" << endreq;
  const Particle *drop_me = NULL;
  return m_decay->test( event, drop_me );
}

bool DecayFinder::findDecay( const std::vector<Particle*> &event,
                             const Particle *&previous_result )
{
  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "About to test the event" << endreq;
  return m_decay->test( event, previous_result );
}

DecayFinder::Descriptor::Descriptor( IParticlePropertySvc *ppSvc,
                                     double rThre)
  : mother(0), daughters(0), skipResonnance(false),
    elipsis(false), m_resThreshold(rThre), m_ppSvc(ppSvc), alternate(0)
{}

DecayFinder::Descriptor::Descriptor( Descriptor &copy )
  : mother(0), daughters(0), skipResonnance(false),
    elipsis(false), m_resThreshold(0), m_ppSvc(0), alternate(0)
{
  if( copy.mother )
    mother = new ParticleMatcher(*copy.mother);
  std::vector<Descriptor *>::iterator d;
  for( d=copy.daughters.begin(); d!=copy.daughters.end(); d++ )
    daughters.push_back(new Descriptor(**d));
  skipResonnance = copy.skipResonnance;
  elipsis  = copy.elipsis;
  m_resThreshold = copy.m_resThreshold;
  m_ppSvc = copy.m_ppSvc;
}

DecayFinder::Descriptor::Descriptor( ParticleMatcher *m,
                                     IParticlePropertySvc *ppSvc,
                                     double rThre)
  : mother(m), daughters(0), skipResonnance(false),
    elipsis(false), m_resThreshold(rThre), m_ppSvc(ppSvc), alternate(0)
{}

DecayFinder::Descriptor::~Descriptor()
{
  if( mother )
    delete mother;
  std::vector<Descriptor *>::iterator di;
  for( di = daughters.begin(); di != daughters.end(); di++ )
    delete *di;
  if( alternate )
    delete alternate;
}

bool DecayFinder::Descriptor::test( const std::vector<Particle*> &event,
                                    const Particle *&previous_result )
{
  std::vector<Particle *>::const_iterator start;
  if( previous_result &&
      ((start=std::find(event.begin(),event.end(),previous_result))
       == event.end()) )
  {
    previous_result = NULL;
    return false; // Bad previous_result
  }
  if( previous_result )
    start++;

  if( mother == NULL ) // No mother == pp collision
  {
    std::list<const Particle*> prims;
    std::vector<Particle*>::const_iterator i;
    for( i=(previous_result ? start : event.begin()); i != event.end(); i++ )
    {
      // Particle have no origin so let's say it comes from the pp collision.
      prims.push_back(*i);
    }
    if( skipResonnance )
      filterResonnances( prims );
    if( testDaughters(prims) )
    {
      previous_result = (const Particle *)1;
      return true;
    }
    return false;
  }

  std::vector<Particle*>::const_iterator part_i;
  part_i = (previous_result ? start : event.begin());
  while( (part_i != event.end()) && (test(*part_i) == false) )
    part_i++;

  if( part_i != event.end() )
  {
    previous_result = *part_i;
    return true;
  }
  return false;
}

bool DecayFinder::Descriptor::test( const Particle *part )
{
  bool result = false;
  if( mother && mother->test(part) )
  {
    if( daughters.empty() )
      return true; // Nothing to test for the daughters.

    std::list<const Particle *> parts;
    const Vertex *vtx = part->endVertex();
    if( vtx )
    {
      SmartRefVector<Particle>::const_iterator idau;
      for ( idau = vtx->products().begin();
            idau != vtx->products().end(); idau++ )
      {
        parts.push_back(*idau);
      }
    }
    if( skipResonnance )
      filterResonnances( parts );

    result = testDaughters(parts);
  }
  if( result )
    return true;
  if( alternate )
    return alternate->test(part);
  return false;
}

bool DecayFinder::Descriptor::testDaughters( std::list<const Particle*> &parts )
{
  std::vector<Descriptor *>::iterator di;
  for( di = daughters.begin();
       (di != daughters.end()) && !parts.empty(); di++ )
  {
    std::list<const Particle *>::iterator p = parts.begin();
    while( p != parts.end() && ((*di)->test(*p) == false) )
      p++;
    if( p == parts.end() )
      return false;   // None of the parts has matched the test
    parts.erase(p);
  }
  if( di != daughters.end() ) // not enough particles
    return false;
  else if( parts.empty() )    // just the right number of particles
    return true;
  else if( elipsis )          // too many particles but we don't care
    return true;
  return false;               // too many particles
}

void DecayFinder::Descriptor::addDaughter( Descriptor *daughter )
{
  if( daughter->mother == NULL )
    return; // empty daughter of a {}

  if( daughter->mother->getQmark() )
  {
    daughters.push_back( daughter );
    return;
  }
  else if( daughter->mother->getExact() )
  {
    daughters.insert( daughters.begin(), daughter );
    return;
  }
  std::vector<Descriptor *>::iterator d;
  d=daughters.begin();
  while( d!=daughters.end() && (*d)->mother->getExact() )
    d++;
  daughters.insert( d, daughter );
}

void DecayFinder::Descriptor::addNonResonnantDaughters(
                                           std::list<const Particle*> &parts,
                                           const Particle *part )
{
  const Vertex *vtx = part->endVertex();
  if( vtx )
  {
    SmartRefVector<Particle>::const_iterator d;
    for ( d = vtx->products().begin();
          d != vtx->products().end(); d++ )
    {
      ParticleProperty *pp =
        m_ppSvc->findByStdHepID( (*d)->particleID().pid() );
      if( pp->lifetime() >= m_resThreshold )
        parts.push_front(*d);
      else
        addNonResonnantDaughters( parts, *d );
    }
  }
}

void DecayFinder::Descriptor::filterResonnances( std::list<const Particle*>
                                                 &parts )
{
  std::list<const Particle*>::iterator pi;
  std::list<const Particle*>::iterator npi;
  for( pi=parts.begin(); pi!=parts.end(); pi = npi )
  {
    ParticleProperty *pp = 
      m_ppSvc->findByStdHepID( (*pi)->particleID().pid() );
    if( pp->lifetime() < m_resThreshold )
    {
      const Particle *part = *pi;
      npi = pi;
      npi++;
      parts.erase(pi);
      addNonResonnantDaughters( parts, part ); // Daughters are prepended.
    }
    else
      npi = ++pi;
  }
}

void DecayFinder::Descriptor::conjugate( void )
{
  mother->conjugateID();
  std::vector<Descriptor *>::iterator d;
  for( d = daughters.begin(); d != daughters.end(); d++ )
    (*d)->conjugate();
}

DecayFinder::ParticleMatcher::ParticleMatcher( IParticlePropertySvc *ppSvc )
  : type(notest), qmark(false), conjugate(false), oscilate(false),
    inverse(false), stable(false), m_ppSvc(ppSvc)
{}

DecayFinder::ParticleMatcher::ParticleMatcher( ParticleMatcher &copy )
  : type(notest), qmark(false), conjugate(false), oscilate(false),
    inverse(false), stable(false), m_ppSvc(0)
{
  type = copy.type;
  qmark = copy.qmark;
  conjugate = copy.conjugate;
  oscilate = copy.oscilate;
  inverse = copy.inverse;
  stable = copy.stable;
  m_ppSvc = copy.m_ppSvc;
  switch( type )
  {
  case id:
    parms.stdHepID = copy.parms.stdHepID;
    break;
  case quark:
    parms.quarks.q1 = copy.parms.quarks.q1;
    parms.quarks.q2 = copy.parms.quarks.q2;
    parms.quarks.q3 = copy.parms.quarks.q3;
    break;
  case quantum:
    parms.relation.q = copy.parms.relation.q;
    parms.relation.r = copy.parms.relation.r;
    parms.relation.d = copy.parms.relation.d;
    break;
  case notest:
  default:
    break;
  }
}

DecayFinder::ParticleMatcher::ParticleMatcher( char *name,
                                               IParticlePropertySvc *ppSvc )
  : type(id), qmark(false), conjugate(false), oscilate(false),
    inverse(false), stable(false), m_ppSvc(ppSvc)
{
  ParticleProperty *pp = m_ppSvc->find(name);
  if( pp )
    parms.stdHepID = pp->jetsetID();
  else
    throw DescriptorError(std::string("Unknown particle '")+name+"'");
}

DecayFinder::ParticleMatcher::ParticleMatcher( Quarks q1, Quarks q2, Quarks q3,
                                               IParticlePropertySvc *ppSvc )
  : type(quark), qmark(false), conjugate(false), oscilate(false),
    inverse(false), stable(false), m_ppSvc(ppSvc)
{
  parms.quarks.q1 = q1;
  parms.quarks.q2 = q2;
  parms.quarks.q3 = q3;
}

DecayFinder::ParticleMatcher::ParticleMatcher( Quantums q,Relations r,double d,
                                               IParticlePropertySvc *ppSvc )
  : type(quantum), qmark(false), conjugate(false), oscilate(false),
    inverse(false), stable(false), m_ppSvc(ppSvc)
{
  parms.relation.q = q;
  parms.relation.r = r;
  parms.relation.d = d;
}

bool DecayFinder::ParticleMatcher::test( const Particle *part )
{
  switch( type )
  {
  case notest:
    return true;
  case id:
    {
      bool result = false;
      ParticleProperty *pp = 
        m_ppSvc->findByStdHepID( part->particleID().pid() );
      if( !pp )
        return false;
      result = (parms.stdHepID == pp->jetsetID());
      if( conjugate )
      {
        int cc_id = conjugatedID( parms.stdHepID );
        result = result || (cc_id == pp->jetsetID());
      }
      if( oscilate )
        result = false; // Not available on reconstructed particles.
      if( inverse )
        result = !result;
      if( stable )
      {
        int n = 0;
        const Vertex *vtx = part->endVertex();
        if( vtx )
          n = vtx->products().size();
        result = result && (n == 0);
      }
      return result;
    }
  case quark:
    // ******* NOT IMPLEMENTED YES *******
    return false;
  case quantum:
    // ******* NOT IMPLEMENTED YES *******
    return false;
  }
  // Should never come here
  return false;
}

void DecayFinder::ParticleMatcher::conjugateID( void )
{
  if( type == id )
    parms.stdHepID = conjugatedID( parms.stdHepID );
  else if( type != notest )
    throw DescriptorError("Charge conjugate only allowed"
                          " on explicit particle or '?'");
}

int DecayFinder::ParticleMatcher::conjugatedID( int id )
{
  int cc_id = -id;
  switch( cc_id )
  { // Particles which are their own anti-particle
  case -12:     // nu_e
  case -22:     // gamma
  case -23:     // Z0
  case -25:     // H_10
  case -35:     // H_20
  case -36:     // H_30
  case -111:    // pi0
  case -113:    // rho(770)
  case -115:    // a_2(1320)0
  case -221:    // eta
  case -223:    // omega(782)
  case -331:    // eta'
  case -333:    // phi(1020)
  case -441:    // eta_c(1S)
  case -443:    // J/psi(1S)
  case -445:    // chi_c2(1P)
  case -10441:  // chi_c0(1P)
  case -20113:  // a_1(1260)0
  case -20443:  // chi_c1(1P)
  case -100443: // psi(2S)
  case -9000111:// a_0(980)0
    cc_id *= -1;
    break;
  default:
    break;
  }
  return cc_id;
}
