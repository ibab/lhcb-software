// $Id: MCMatchObjP2MCRelator.cpp,v 1.3 2009-03-06 14:24:14 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
// LoKi
#include "LoKi/IReporter.h"
#include "LoKi/MCTrees.h"
// local
#include "MCMatchObjP2MCRelator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MCMatchObjP2MCRelator
//
// 2009-03-04 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MCMatchObjP2MCRelator );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MCMatchObjP2MCRelator::MCMatchObjP2MCRelator( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : 
  Particle2MCAssociatorBase( type, name , parent ),
  m_reporter(0),
  m_matcher(0),
  m_PP2MC()
{

  m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Charged  ) ;
  m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Upstream ) ;
  m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Neutrals ) ;

  declareInterface<IParticle2MCAssociator>(this);

}
//=============================================================================
StatusCode MCMatchObjP2MCRelator::initialize()
{
  StatusCode sc =  Particle2MCAssociatorBase::initialize();
  if (sc.isFailure()) return sc;
  m_reporter = tool<LoKi::IReporter>( "LoKi::Reporter", this ) ;
  return (0!=m_reporter) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}
//=============================================================================
StatusCode MCMatchObjP2MCRelator::finalize()
{
  return Particle2MCAssociatorBase::finalize();
}
//=============================================================================
bool MCMatchObjP2MCRelator::isMatched(const LHCb::Particle* particle, 
                                      const LHCb::MCParticle* mcParticle) const
{
  const bool match = matcher()->match(particle, mcParticle);
  if (match) {
    verbose() << "match Particle PID " << particle->particleID().pid() 
              << " to MCParticle PID " << mcParticle->particleID().pid() 
              << endmsg;
  } else {
    verbose() << "No match for Particle PID " << particle->particleID().pid() 
              << endmsg;
  }
  return match;
}
//=============================================================================
LoKi::MCMatch MCMatchObjP2MCRelator::matcher() const
{

  if (0==m_matcher) {
    // get the reporter 
    // create the new matcher 
    m_matcher = new LoKi::MCMatchObj( "P2MCRelator" , m_reporter ) ;
    // increment the reference counter 
    m_matcher->addRef() ;
  }
  m_matcher->clear();
  // feed it the relations tables
  for (Addresses::const_iterator item = m_PP2MC.begin(); item!=m_PP2MC.end(); ++item) {
    const std::string& address = *item;
    if (exist<LoKi::Types::TablePP2MC>(address) ) {
      LoKi::Types::TablePP2MC table* = get<LoKi::Types::TablePP2MC>(address);
      m_matcher->addMatchInfo(table);
    } else {
      Error ( " There is no valid data at '" + address + "'" ) ; 
    }
  }
  
  return  LoKi::MCMatch( m_matcher ) ;
}
//=============================================================================
LHCb::MCParticle::ConstVector 
MCMatchObjP2MCRelator::sort(const LHCb::MCParticle::ConstVector& mcParticles) const 
{
  return LoKi::MCTrees::buildTrees(mcParticles);
}
//=============================================================================
LHCb::MCParticle::ConstVector 
MCMatchObjP2MCRelator::sort(const LHCb::MCParticle::Container* mcParticles) const 
{
  return LoKi::MCTrees::buildTrees(mcParticles);
}
//=============================================================================
// Destructor
//=============================================================================
MCMatchObjP2MCRelator::~MCMatchObjP2MCRelator() {}
//=============================================================================
