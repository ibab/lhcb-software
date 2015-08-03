// Include files 
#include <list>
#include <functional>
#include <algorithm>

// from Gaudi
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/SystemOfUnits.h"

// from PartProp
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

// local
#include "MCDecayFinder.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCDecayFinder
//
// 20/04/2002 : Olivier Dormond
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( MCDecayFinder )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCDecayFinder::MCDecayFinder( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_ppSvc(0), m_source("B0 -> pi+ pi-"), m_decay(0), m_members(0), m_brackets("{}()[]")
{

  declareInterface<IMCDecayFinder>(this);

  declareProperty( "Decay", m_source );
  declareProperty( "ResonanceThreshold", 
                   m_resThreshold = 1e-15*Gaudi::Units::second );
}

//=============================================================================
// Standard destructor
//=============================================================================

MCDecayFinder::~MCDecayFinder( )
{
  if( m_decay )
    delete m_decay;
  if( m_members )
    delete m_members;
}

//=============================================================================

StatusCode MCDecayFinder::initialize(){
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;
  
  if (msgLevel(MSG::DEBUG)) debug() << "==> Initializing" << endmsg;

  m_ppSvc = svc<LHCb::IParticlePropertySvc>( "LHCb::ParticlePropertySvc", true );

  if( m_source.length() == 0 ){
    return Warning( "No decay specified!", StatusCode::SUCCESS );
  }
  if( compile(m_source) ){
    if (msgLevel(MSG::DEBUG)) debug() << "The compilation of the decay was successful"
            << endmsg;
    return StatusCode::SUCCESS;
  }
  return Error( "Could not compile the decay description" );
}

StatusCode MCDecayFinder::setDecay( std::string decay ){

  Descriptor *old_decay = m_decay;
  std::vector<ParticleMatcher *> *old_members = m_members;
  // Be sure we don't get a old piece if the feature is not used.
  m_decay = NULL;
  m_members = NULL;

  if (msgLevel(MSG::DEBUG)) debug() << "Setting decay to " << decay << endmsg;
  if( sanityCheck(decay) && compile(decay) ) {
    if (msgLevel(MSG::DEBUG)) debug() << "The compilation of the decay was successfull"
            << endmsg;
    m_source = decay;
    if( old_decay )
      delete old_decay;
    if( old_members ) {
      std::vector<ParticleMatcher *>::iterator pm_i;
      for( pm_i = old_members->begin(); pm_i != old_members->end(); pm_i++ )
        delete *pm_i;
      old_members->clear();
      delete old_members;
    }
    return StatusCode::SUCCESS;
  }

  // Restore previous decay if compilation failed.
  if( m_decay && (m_decay != old_decay) )
    delete m_decay;
  m_decay = old_decay;
  if( m_members && (m_members != old_members) ) {
    std::vector<ParticleMatcher *>::iterator pm_i;
    for( pm_i = m_members->begin(); pm_i != m_members->end(); pm_i++ )
      delete *pm_i;
    m_members->clear();
    delete m_members;
  }
  m_members = old_members;
  err() << "Could not compile the decay description: " << decay << endmsg;

  return StatusCode::FAILURE;
}

std::string MCDecayFinder::revert( void ){
  std::string result = "";
  if( m_decay == NULL )
    return result;
  bool alt = m_decay->getAlternate() != NULL;
  if( alt )
    result += "{ ";
  Descriptor *a = m_decay;
  while( a ) {
    result += a->describe();
    if( a->getAlternate() ) {
      result += ", ";
    }
    a = a->getAlternate();
  }
  if( alt )
    result += " }";
  return result;
}

#include "mclexer.icpp"
#include "mcparser.icpp"

bool MCDecayFinder::compile( std::string &source )
{
  yy_buffer_state *bs = yy_scan_string( source.c_str() );
  try
  {
    if( yparse() )
      throw("Syntax Error");
  }
  catch( DescriptorError e )
  {
    err() << "Invalid decay description '"
          << source << "'" << endmsg;
    err() << e.cause() << endmsg;
    yy_delete_buffer(bs);
    return false;
  }
  yy_delete_buffer(bs);
  if (msgLevel(MSG::DEBUG)) debug() << "Result of the compilation:\n"
                                    << revert() << endmsg;
  return true;
}

bool MCDecayFinder::hasDecay( const  LHCb::MCParticle::ConstVector &event )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "About to test the event" << endmsg;
  if (!m_decay)   Exception("Trying to find an unspecified decay!");
  const LHCb::MCParticle *drop_me = NULL;
  bool r =  m_decay->test( event.begin(), event.end(), drop_me );
  return r ;
}

bool MCDecayFinder::findDecay( const LHCb::MCParticle::ConstVector& event,
                               const LHCb::MCParticle*& previous_result )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "About to test the event" << endmsg;
  if (!m_decay)   Exception("Trying to find an unspecified decay!");
  bool r = m_decay->test( event.begin(), event.end(), previous_result );
  if (!r && UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "Could not find decay" << endmsg;
  return r ;
}

bool MCDecayFinder::hasDecay( const LHCb::MCParticles &event )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "About to test the event" << endmsg;
  if (!m_decay) Exception("Trying to find an unspecified decay!");
  const LHCb::MCParticle *drop_me = NULL;
    bool r = m_decay->test( event.begin(), event.end(), drop_me );
    if (!r && UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "Could not find decay" << endmsg;
    return r ;
}

bool MCDecayFinder::findDecay( const LHCb::MCParticles &event,
                               const LHCb::MCParticle *&previous_result )
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "About to test the event" << endmsg;
  if (!m_decay) Exception("Trying to find an unspecified decay!");
  bool r = m_decay->test( event.begin(), event.end(), previous_result );
  if (!r && UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "Could not find decay" << endmsg;
  return r;
}

bool MCDecayFinder::hasDecay( void )
{
  LHCb::MCParticles* mcparts = 
    get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default );
  if( !mcparts )
  {
    fatal() << "Unable to find MC particles at '"
            << LHCb::MCParticleLocation::Default << "'" << endmsg;
    return false;
  }
  return hasDecay( *mcparts );
}

bool MCDecayFinder::findDecay( const LHCb::MCParticle *&previous_result )
{
  LHCb::MCParticles* mcparts = 
    get<LHCb::MCParticles>(LHCb::MCParticleLocation::Default );
  if( !mcparts )
  {
    fatal() << "Unable to find MC particles at '"
            << LHCb::MCParticleLocation::Default << "'" << endmsg;
    return false;
  }
  return findDecay( *mcparts, previous_result );
}

void MCDecayFinder::descendants( const LHCb::MCParticle *head,
                                 LHCb::MCParticle::ConstVector& result,
                                 bool leaf )
{
  bool term = true;
  SmartRefVector<LHCb::MCVertex>::const_iterator vi;
  for( vi = head->endVertices().begin(); vi != head->endVertices().end(); vi++ )
  {
    SmartRefVector<LHCb::MCParticle>::const_iterator pi;
    for( pi = (*vi)->products().begin(); pi != (*vi)->products().end(); pi++ )
    {
      term = false;
      descendants( *pi, result, leaf );
    }
  }
  
  if( !leaf || term )
    result.push_back(const_cast<LHCb::MCParticle *>(head));
}

void MCDecayFinder::decayMembers( const LHCb::MCParticle *head,
                                  LHCb::MCParticle::ConstVector&members )
{
  if( m_members ) {
    LHCb::MCParticle::ConstVector flat;
    descendants( head, flat, false );
    LHCb::MCParticle::ConstVector::const_iterator pi;
    for( pi = flat.begin(); pi != flat.end(); pi++ ) {
      std::vector<ParticleMatcher *>::const_iterator mi;
      for( mi = m_members->begin(); mi != m_members->end(); mi++ )
        if( (*mi)->test(*pi) ) {
          members.push_back(*pi);
          break;
        }
    }
  }
  
  m_decay->test(head, &members);
}

void MCDecayFinder::decaySubTrees(
                                  const LHCb::MCParticle *head,
                                  std::vector<std::pair<const LHCb::MCParticle*,
                                  LHCb::MCParticle::ConstVector >
                                  > & subtrees )
{
  m_decay->test(head, NULL, &subtrees);
}

MCDecayFinder::Descriptor::Descriptor( LHCb::IParticlePropertySvc *ppSvc,
                                       double rThre)
  : mother(0), daughters(0), skipResonance(false),
    elipsis(false), m_resThreshold(rThre), m_ppSvc(ppSvc), alternate(0)
{}

MCDecayFinder::Descriptor::Descriptor( Descriptor &copy )
  : mother(0), daughters(0), skipResonance(false),
    elipsis(false), m_resThreshold(0), m_ppSvc(0), alternate(0)
{
  if( copy.mother )
    mother = new ParticleMatcher(*copy.mother);
  std::vector<Descriptor *>::iterator d;
  for( d=copy.daughters.begin(); d!=copy.daughters.end(); d++ ) {
    daughters.push_back(new Descriptor(**d));
  }
  skipResonance = copy.skipResonance;
  elipsis  = copy.elipsis;
  m_resThreshold = copy.m_resThreshold;
  m_ppSvc = copy.m_ppSvc;
}

MCDecayFinder::Descriptor::Descriptor( ParticleMatcher *m,
                                       LHCb::IParticlePropertySvc *ppSvc,
                                       double rThre)
  : mother(m), daughters(0), skipResonance(false),
    elipsis(false), m_resThreshold(rThre), m_ppSvc(ppSvc), alternate(0)
{}

MCDecayFinder::Descriptor::~Descriptor()
{
  if( mother )
    delete mother;
  std::vector<Descriptor *>::iterator di;
  for( di = daughters.begin(); di != daughters.end(); di++ )
    delete *di;
  if( alternate )
    delete alternate;
}

std::string MCDecayFinder::Descriptor::describe( void )
{
  std::string result = "";
  if( !daughters.empty() )
    result += '(';
  if( mother )
    result += mother->describe();
  else
    result += "pp";
  if( !daughters.empty() ) {
    if( skipResonance )
      result += " => ";
    else
      result += " -> ";
    std::vector<Descriptor *>::const_iterator di;
    for( di=daughters.begin(); di!=daughters.end(); di++ ) {
      result += (*di)->describe();
      if( *di != daughters.back() )
        result += ' ';
    }
  }
  if( elipsis )
    result += " ...";
  if( !daughters.empty() )
    result += ')';
  return result;
}

bool MCDecayFinder::Descriptor::test( const LHCb::MCParticle *part,
                                      LHCb::MCParticle::ConstVector *collect,
                                      std::vector<std::pair<const LHCb::MCParticle*,
                                      LHCb::MCParticle::ConstVector >
                                      > *subtrees )
{
  LHCb::MCParticle::ConstVector local_collect(0);
  LHCb::MCParticle::ConstVector *local = NULL;
  if( collect || subtrees )
    local = &local_collect;
  bool result = false;
  if( mother && mother->test(part,local) )
  {
    if( daughters.empty() )
      result = true; // Nothing to test for the daughters.
    else {
      std::list<const LHCb::MCParticle *> parts;
      SmartRefVector<LHCb::MCVertex>::const_iterator vi;
      for ( vi = part->endVertices().begin();
            vi != part->endVertices().end(); vi++)
      {
        if(! *vi) continue;
        SmartRefVector<LHCb::MCParticle>::const_iterator idau;
        for ( idau = (*vi)->products().begin();
              idau != (*vi)->products().end(); idau++ )
        {
          if(! *idau) continue;
          parts.push_back(*idau);
        }
      }
      if( skipResonance )
        filterResonances( parts );
      
      if( subtrees ) {
        std::vector<std::pair<const LHCb::MCParticle*,
          LHCb::MCParticle::ConstVector > > local_subtrees;
        result = testDaughters(parts,local,&local_subtrees);
        if( result )
          subtrees->insert(subtrees->end(),
                           local_subtrees.begin(),
                           local_subtrees.end());
      } else {
        result = testDaughters(parts,local);
      }
    }
  }
  if( result ) {
    if( collect )
      collect->insert( collect->end(),
                       local_collect.begin(), local_collect.end() );
    if( subtrees && !daughters.empty() )
      subtrees->push_back(std::pair<const LHCb::MCParticle*,LHCb::MCParticle::ConstVector >
                          (part,local_collect) );
    return true;
  }
  if( alternate )
    return alternate->test(part,collect,subtrees);
  return false;
}

bool
MCDecayFinder::Descriptor::testDaughters(std::list<const LHCb::MCParticle*> &parts,
                                         LHCb::MCParticle::ConstVector *collect,
                                         std::vector<std::pair<const LHCb::MCParticle*,
                                         LHCb::MCParticle::ConstVector >
                                         > *subtrees)
{
  std::vector<Descriptor *>::iterator di;
  for( di = daughters.begin();
       (di != daughters.end()) && !parts.empty(); di++ )
  {
    if(! *di) continue;
    std::list<const LHCb::MCParticle *>::iterator p = parts.begin();
    while( p != parts.end() && ((*di)->test(*p,collect,subtrees) == false) )
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

void MCDecayFinder::Descriptor::addDaughter( Descriptor *daughter )
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

void MCDecayFinder::Descriptor::addNonResonantDaughters(
                                                         std::list<const LHCb::MCParticle*> &parts,
                                                         const LHCb::MCParticle *part )
{
  SmartRefVector<LHCb::MCVertex>::const_iterator vi;
  for ( vi = part->endVertices().begin();
        vi != part->endVertices().end(); vi++ )
  {
    if(! *vi) break;
    SmartRefVector<LHCb::MCParticle>::const_iterator idau;
    for ( idau = (*vi)->products().begin();
          m_ppSvc && idau != (*vi)->products().end(); idau++ )
    {
      if(! *idau) break;
      
      const LHCb::ParticleProperty *pp = m_ppSvc->find( (*idau)->particleID() );
      //      std::cout << "addNonResonantDaughters " << (*idau)->particleID() << " " << pp << std::endl ;
      
      if(!pp)
      {
        // throw DescriptorError(std::string("Unknown particle '")+"'");
        std::cout << "MCDecayFinder::Descriptor::addNonResonantDaughters WARNING Particle property not obtainable for " 
                  << (*idau)->particleID() << std::endl ;
        break;
      }
      
      if( pp->lifetime() >= m_resThreshold )
        parts.push_front(*idau);
      else
        addNonResonantDaughters( parts, *idau );
    }
  }
}

void MCDecayFinder::Descriptor::filterResonances( std::list<const LHCb::MCParticle*>
                                                   &parts )
{
  std::list<const LHCb::MCParticle*>::iterator pi;
  std::list<const LHCb::MCParticle*>::iterator npi;
  for( pi=parts.begin(); m_ppSvc && pi!=parts.end(); pi = npi )
  {
    const LHCb::ParticleProperty *pp = m_ppSvc->find( (*pi)->particleID() );
    // std::cout << "filterResonances " << (*pi)->particleID() << " " << pp << std::endl ;
    if(!pp)
    {
      //      throw DescriptorError(std::string("Unknown particle '")+"'");
      std::cout << "MCDecayFinder::Descriptor::filterResonances WARNING Particle property not obtainable for " 
                << (*pi)->particleID() << std::endl ;
      break;
    }
    
    if( pp->lifetime() < m_resThreshold )
    {
      const LHCb::MCParticle *part = *pi;
      npi = pi;
      npi++;
      parts.erase(pi);
      addNonResonantDaughters( parts, part ); // Daughters are prepended.
    }
    else
      npi = ++pi;
  }
}

void MCDecayFinder::Descriptor::conjugate( void )
{
  if( mother )
    mother->conjugateID();
  std::vector<Descriptor *>::iterator d;
  for( d = daughters.begin(); d != daughters.end(); d++ )
    (*d)->conjugate();
}

MCDecayFinder::ParticleMatcher::ParticleMatcher( LHCb::IParticlePropertySvc *ppSvc )
  : type(notest), lift(false), empty_f(false), qmark(false), conjugate(false),
    oscillate(false), noscillate(false), inverse(false), stable(false),
    m_ppSvc(ppSvc)
{}

MCDecayFinder::ParticleMatcher::ParticleMatcher( ParticleMatcher &copy )
{
  type = copy.type;
  lift = copy.lift;
  empty_f = copy.empty_f;
  qmark = copy.qmark;
  conjugate = copy.conjugate;
  oscillate = copy.oscillate;
  noscillate = copy.noscillate;
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

MCDecayFinder::ParticleMatcher::ParticleMatcher( std::string *name,
                                                 LHCb::IParticlePropertySvc *ppSvc )
  : type(id), lift(false), empty_f(false), qmark(false), conjugate(false),
    oscillate(false), noscillate(false), inverse(false), stable(false),
    m_ppSvc(ppSvc)
{
  const LHCb::ParticleProperty *pp = m_ppSvc->find(*name);
  if( pp )
    parms.stdHepID = pp->pid().pid();
  else
    throw DescriptorError(std::string("Unknown particle '")+*name+"'");
}

MCDecayFinder::ParticleMatcher::ParticleMatcher(Quarks q1, Quarks q2, Quarks q3,
                                                LHCb::IParticlePropertySvc *ppSvc )
  : type(quark), lift(false), empty_f(false), qmark(false), conjugate(false),
    oscillate(false), noscillate(false), inverse(false), stable(false),
    m_ppSvc(ppSvc)
{
  parms.quarks.q1 = q1;
  parms.quarks.q2 = q2;
  parms.quarks.q3 = q3;
}

MCDecayFinder::ParticleMatcher::ParticleMatcher(Quantums q,Relations r,double d,
                                                LHCb::IParticlePropertySvc *ppSvc )
  : type(quantum), lift(false), empty_f(false), qmark(false), conjugate(false),
    oscillate(false), noscillate(false), inverse(false), stable(false),
    m_ppSvc(ppSvc)
{
  parms.relation.q = q;
  parms.relation.r = r;
  parms.relation.d = d;
}

std::string MCDecayFinder::ParticleMatcher::describe( void )
{
  std::string result = "";
  if( stable )     result += '(';
  if( lift )       result += '^';
  if( qmark )      result += '?';
  if( conjugate )  result += '[';
  if( oscillate )  result += '[';
  if( noscillate ) result += '[';
  if( inverse )    result += '!';
  switch( type ) {
  case notest:
    result += "## MUST NOT COMPILE ##";
    break;
  case id:
    if(!m_ppSvc) break;
    if(!m_ppSvc->find(LHCb::ParticleID(parms.stdHepID))) throw DescriptorError(std::string("Unknown particle"));;
    result += m_ppSvc->find(LHCb::ParticleID(parms.stdHepID))->particle();
    break;
  case quark:
    result += "<X";
    switch( parms.quarks.q1 ) {
    case up:          result +='u';  break;
    case down:        result +='d';  break;
    case charm:       result +='c';  break;
    case strange:     result +='s';  break;
    case top:         result +='t';  break;
    case bottom:      result +='b';  break;
    case antiup:      result +="u~"; break;
    case antidown:    result +="d~"; break;
    case anticharm:   result +="c~"; break;
    case antistrange: result +="s~"; break;
    case antitop:     result +="t~"; break;
    case antibottom:  result +="b~"; break;
    case empty:                      break;
    }
    switch( parms.quarks.q2 ) {
    case up:          result +='u';  break;
    case down:        result +='d';  break;
    case charm:       result +='c';  break;
    case strange:     result +='s';  break;
    case top:         result +='t';  break;
    case bottom:      result +='b';  break;
    case antiup:      result +="u~"; break;
    case antidown:    result +="d~"; break;
    case anticharm:   result +="c~"; break;
    case antistrange: result +="s~"; break;
    case antitop:     result +="t~"; break;
    case antibottom:  result +="b~"; break;
    case empty:                      break;
    }
    switch( parms.quarks.q3 ) {
    case up:          result +='u';  break;
    case down:        result +='d';  break;
    case charm:       result +='c';  break;
    case strange:     result +='s';  break;
    case top:         result +='t';  break;
    case bottom:      result +='b';  break;
    case antiup:      result +="u~"; break;
    case antidown:    result +="d~"; break;
    case anticharm:   result +="c~"; break;
    case antistrange: result +="s~"; break;
    case antitop:     result +="t~"; break;
    case antibottom:  result +="b~"; break;
    case empty:                      break;
    }
    result += '>';
    break;
  case quantum:
    result += "## NOT IMPLEMENTED ##";
    break;
  }
  if( noscillate ) result += "]nos";
  if( oscillate )  result += "]os";
  if( conjugate )  result += "]cc";
  if( stable )     result += ')';
  return result;
}

static inline int DIGIT( int n, int id )
{
  int base = 1;
  for( ; n; n-- ) base *= 10;
  return ((abs(id)%(10*base))/base);
}

static inline int quarkOne( int id )
{
  return DIGIT(3,id);
}

static inline int quarkTwo( int id )
{
  return DIGIT(2,id);
}

static inline int quarkThree( int id )
{
  return DIGIT(1,id);
}

static inline bool isIon( int id )
{
  return( 1==DIGIT(10,id) );
}

static inline bool isHadron( int id )
{
  return abs(id)>=110;
}

static inline bool isMeson( int id )
{
  return( isHadron(id) && !isIon(id) && 0==quarkOne(id) );
}

static const int QuarksCharge[] = { 0, -1, 1, -1, 1, -1, 1 };

static int firstQuark( int id )
{
  int q;

  if( !isHadron(id) || isIon(id) )
    return 0;

  if( isMeson(id) )
  {
    q = quarkTwo(id);
    q *= QuarksCharge[q];
  }
  else
    q = quarkOne(id);

  return q*(id>0 ? 1 : -1);
}

static int secondQuark( int id )
{
  int q;

  if( !isHadron(id) || isIon(id) )
    return 0;

  if( isMeson(id) )
  {
    q = quarkThree(id);
    int r = quarkTwo(id);
    if( QuarksCharge[q] == QuarksCharge[r] )
      q *= -QuarksCharge[q];
    else
      q *= QuarksCharge[q];
  }
  else
    q = quarkTwo(id);

  return q*(id>0 ? 1 : -1);
}

static int thirdQuark( int id )
{
  int q;

  if( !isHadron(id) || isIon(id) )
    return 0;

  if( isMeson(id) )
    q = 0;
  else
    q = quarkThree(id);

  return q*(id>0 ? 1 : -1);
}

bool
MCDecayFinder::ParticleMatcher::test( const LHCb::MCParticle *part,
                                      LHCb::MCParticle::ConstVector *collect )
{
  bool result = false;
  switch( type ) {
  case notest:
    result = true;
    break;
  case id:
    result = (parms.stdHepID == part->particleID().pid());
    if( conjugate ) {
      int cc_id = conjugatedID( parms.stdHepID );
      result = result || (cc_id == part->particleID().pid());
    }
    if( oscillate )  result = result && part->hasOscillated();
    if( noscillate ) result = result && (!part->hasOscillated());
    if( inverse )    result = !result;
    if( stable ) {
      int n = 0;
      SmartRefVector<LHCb::MCVertex>::const_iterator vi;
      for ( vi = part->endVertices().begin();
            vi != part->endVertices().end(); vi++)
        n += (*vi)->products().size();
      result = result && (n == 0);
    }
    break;
  case quark:
    {
      static Quarks Q[] = { empty, down, up, strange, charm, bottom, top };
      static Quarks AQ[] = { empty, antidown, antiup, antistrange,
                             anticharm, antibottom, antitop };
      int q = firstQuark(part->particleID().pid());
      Quarks q1 = (q<0 ? AQ[-q] : Q[q]);
      Quarks cq1 = (q<0 ? Q[-q] : AQ[q]); // cc hypothesis
      q = secondQuark(part->particleID().pid());
      Quarks q2 = (q<0 ? AQ[-q] : Q[q]);
      Quarks cq2 = (q<0 ? Q[-q] : AQ[q]); // cc hypothesis
      q = thirdQuark(part->particleID().pid());
      Quarks q3 = (q<0 ? AQ[-q] : Q[q]);
      Quarks cq3 = (q<0 ? Q[-q] : AQ[q]); // cc hypothesis
      
      // Shortcuts
      Quarks pq1 = parms.quarks.q1;
      Quarks pq2 = parms.quarks.q2;
      Quarks pq3 = parms.quarks.q3;

      // We don't care of the ordering so we check all permutations.
      // q1, q2, q3
      if( (pq1 == q1 || pq1 == empty) && (pq2 == q2 || pq2 == empty) &&
          (pq3 == q3 || pq3 == empty) )
        result = true;
      // q1, q3, q2
      else if( (pq1 == q1 || pq1 == empty) && (pq2 == q3 || pq2 == empty) &&
               (pq3 == q2 || pq3 == empty) )
        result = true;
      // q2, q1, q3
      else if( (pq1 == q2 || pq1 == empty) && (pq2 == q1 || pq2 == empty) &&
               (pq3 == q3 || pq3 == empty) )
        result = true;
      // q2, q3, q1
      else if( (pq1 == q2 || pq1 == empty) && (pq2 == q3 || pq2 == empty) &&
               (pq3 == q1 || pq3 == empty) )
        result = true;
      // q3, q1, q2
      else if( (pq1 == q3 || pq1 == empty) && (pq2 == q1 || pq2 == empty) &&
               (pq3 == q2 || pq3 == empty) )
        result = true;
      // q3, q2, q1
      else if( (pq1 == q3 || pq1 == empty) && (pq2 == q2 || pq2 == empty) &&
               (pq3 == q1 || pq3 == empty) )
        result = true;

      // Should we check for the charge conjugated particle ?
      if( !conjugate )
        break; // No. Ok. Everything done.

      // cq1, cq2, cq3
      if( (pq1 == cq1 || pq1 == empty) && (pq2 == cq2 || pq2 == empty) &&
          (pq3 == cq3 || pq3 == empty) )
        result = true;
      // cq1, cq3, cq2
      else if( (pq1 == cq1 || pq1 == empty) && (pq2 == cq3 || pq2 == empty) &&
               (pq3 == cq2 || pq3 == empty) )
        result = true;
      // cq2, cq1, cq3
      else if( (pq1 == cq2 || pq1 == empty) && (pq2 == cq1 || pq2 == empty) &&
               (pq3 == cq3 || pq3 == empty) )
        result = true;
      // cq2, cq3, cq1
      else if( (pq1 == cq2 || pq1 == empty) && (pq2 == cq3 || pq2 == empty) &&
               (pq3 == cq1 || pq3 == empty) )
        result = true;
      // cq3, cq1, cq2
      else if( (pq1 == cq3 || pq1 == empty) && (pq2 == cq1 || pq2 == empty) &&
               (pq3 == cq2 || pq3 == empty) )
        result = true;
      // cq3, cq2, cq1
      else if( (pq1 == cq3 || pq1 == empty) && (pq2 == cq2 || pq2 == empty) &&
               (pq3 == cq1 || pq3 == empty) )
        result = true;
    }
    break;
  case quantum:
    // ******* NOT IMPLEMENTED YES *******
    return false;
  }
  if( result && lift && collect )
    collect->push_back( const_cast<LHCb::MCParticle*>(part) );
  return result;
}

void MCDecayFinder::ParticleMatcher::conjugateID( void )
{
  if( type == id )
    parms.stdHepID = conjugatedID( parms.stdHepID );
  else if( type != notest )
    throw DescriptorError("Charge conjugate only allowed"
                          " on explicit particle or '?'");
}

int MCDecayFinder::ParticleMatcher::conjugatedID( int id )
{
  if(m_ppSvc)
  {
    const LHCb::ParticleProperty * pp=m_ppSvc->find(LHCb::ParticleID(id));
    if(pp)
    {
      const LHCb::ParticleProperty *anti=pp->antiParticle();
      if(anti) return anti->pid().pid();
      else return id; //assume it has no antiparticle
      
    }
    throw DescriptorError("A particle was not known in the prop service");
    
  }
  //else warning() << "Particle property service not defined, guessing conjugate ID" << endmsg;
      
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
  case -130:    // KS0
  case -221:    // eta
  case -223:    // omega(782)
  case -310:    // K0L
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
    //  Interesting case of CPT violation ;-)
    //  case -310:    // KS0~ <-> KL0
    //    cc_id = 130;
    //    break;
    //  case -130:    // KL0~ <-> KS0
    //    cc_id = 310;
    //    break;
  default:
    break;
  }
  return cc_id;
  
}
bool MCDecayFinder::sanityCheck(const std::string & decay)
{
  //check for more than one lot of '...'
  std::size_t apos = decay.find("...");
  if(apos!=std::string::npos)
  {
    apos++;
    apos = decay.find("...",apos);
    if(apos!=std::string::npos)
	  {
	    warning() << "The decay descriptor " << decay 
                << " has two lots of |...|. This would cause a segfault if I hadn't caught it here." 
                << endmsg;
	    return false;
	  }
    
  }
    //check for mismatched brackets, first make the string with only brackets
  std::string bstring="";
  
  for(std::string::const_iterator itd=decay.begin(); itd != decay.end(); itd++)
  {
    for(std::string::const_iterator itb=m_brackets.begin(); itb != m_brackets.end() && itd != decay.end(); itb++)
	  {
	    if(*itb == *itd)
      {
        bstring += *itb;
        break;
      }
	    
	  }
    
  }
  //check the number of brackets matches up
  if (msgLevel(MSG::VERBOSE)) verbose() << "Checking brackets " << bstring << endmsg;
  if(bstring.size()==0) return true;
  if(bstring.size()%2!=0)
  {
    warning() << "There is an uneven number of brackets in: " << bstring << endmsg;
    return false;
  }
  
    
  //loop until there is no string
  while(bstring.size()>1)
  {
    const unsigned int start=bstring.size();
    
    //loop over possible bracket combinations
    //iterate over n and n+1, to get opening and closing bracket
    for(unsigned int itb=0; bstring.size()>1 && itb < m_brackets.size()-1; itb+=2)
	  {
	    //loop over the string
	    for(unsigned int itd=0; bstring.size()>1 && itd < bstring.size()-1; itd+=1)
      {
        //remove matching brackets
        if(bstring[itd]==m_brackets[itb] && bstring[itd+1]==m_brackets[itb+1])
        {
          if (msgLevel(MSG::VERBOSE)) verbose() << "Removing : " << bstring[itd] << bstring[itd+1] << endmsg;
          bstring.erase(itd,2);
          //reduced loop counter by two, to move to previous location
          if(itd >= 2) itd-=2;
		    else itd=0; 
          itb=0;
          //unless I am at the beginning, then just go to back to zero
        }
      }
	    
	  }
    
    //break if the string is the same size at beginning and at end
    if (start==bstring.size()) break;
  }
  if (msgLevel(MSG::VERBOSE)) verbose() << "Removed matching brackets now: " << bstring << endmsg;
  if(bstring.size()>0)
  {
    warning() << "There are mismatched brackets in this decay descriptor. The non-matching brackets are: " << bstring << endmsg;
    return false;
      }
  //now check for at least one comma between all curly brackets
  
  bstring=decay;
  
  while(bstring.size()!=0)
  {
    const unsigned int start=bstring.size();
    
    //find the first closing bracket }
    
    for(unsigned int iclose=0; bstring.size()!=0 && iclose<bstring.size(); iclose++)
	  {
	    if(bstring[iclose]==m_brackets[1])
      {
        //find the opening bracket for this
		for(unsigned int iopen=iclose-1; true; iopen--)
    {
      if(bstring[iopen]==m_brackets[0]) 
      {
        //check there is at least one comma
        unsigned int comma=iopen+1;
        char acomma=',';
        while(comma<iclose)
			  {
			    if(bstring[comma]==acomma) break;
			    else comma++;
			  }
        if(bstring[comma]!=acomma)
			  {
			    warning() << "every {} must contain at least one alternative. "<< bstring << " does not" << endmsg;
			    return false;
			  }
        else
			  {
			    //OK so far, erase this bracket
			    bstring.erase(iopen, iclose-iopen+1);
			    //start from the beginning of this bracket
			    iclose=iopen-1;
			    //stop looping over iopen, go on to the next closing bracket
			    if (msgLevel(MSG::VERBOSE)) verbose() << "OK so far, moving onto the next curly bracket in " << bstring << endmsg;
			    break;
			  }
      }
      else if(iopen <=0 || iopen>=iclose)
      {
        warning() << "there is something odd with the descriptor: "<< bstring << endmsg;
        return false;
      }
		  
		  }
    
      }
	    
	  }
    
    if (start==bstring.size()) break;
  }
  //find the opening bracket for this
  //check that there is at least one comma
  //replace all with a #
  if (msgLevel(MSG::VERBOSE)) verbose() << "Sanity check passed for decay: " << decay << endmsg;
  
  return true;
  
}
