// $Id: MCMatchObjP2MCRelator.cpp,v 1.2 2009-03-05 11:27:24 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
// LoKi
#include "LoKi/IReporter.h"
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
  return false;
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

  return  LoKi::MCMatch( m_matcher ) ;
}
//=============================================================================
// Destructor
//=============================================================================
MCMatchObjP2MCRelator::~MCMatchObjP2MCRelator() {}
//=============================================================================
