// $Id: MCMatchObjP2MCRelator.cpp,v 1.5 2009-03-10 18:12:06 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
// LoKi
#include "LoKi/IReporter.h"
#include "LoKi/BuildMCTrees.h"
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
  P2MCPBase( type, name , parent ),
  m_reporter(0),
  m_matcher(0),
  m_PP2MC()
{

  m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Charged  ) ;
  m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Upstream ) ;
  m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Neutrals ) ;

  declareInterface<IP2MCP>(this);

}
//=============================================================================
StatusCode MCMatchObjP2MCRelator::initialize()
{
  StatusCode sc =  P2MCPBase::initialize();
  if (sc.isFailure()) return sc;
  m_reporter = tool<LoKi::IReporter>( "LoKi::Reporter", this ) ;
  return (0!=m_reporter) ? StatusCode::SUCCESS : StatusCode::FAILURE;
}
//=============================================================================
StatusCode MCMatchObjP2MCRelator::finalize()
{
  return P2MCPBase::finalize();
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
  addTables(m_matcher);
  
  return  LoKi::MCMatch( m_matcher ) ;
}
//=============================================================================
LHCb::MCParticle::ConstVector 
MCMatchObjP2MCRelator::sort(const LHCb::MCParticle::ConstVector& mcParticles) const 
{
  //  return mcParticles;
//   std::cout << "MCMatchObjP2MCRelator::sort" << std::endl;
  LHCb::MCParticle::ConstVector head = LoKi::MCTrees::buildTrees(mcParticles);
  int size = head.size();
  if ( size > 1) {
    std::cout << "BEWARE! Association from " << size
              << " TREES!!!" << std::endl;
  } else if (size ==1 ){
    std::cout << "ALL'S GOOD! Association from " << size
              << " TREE!!!" << std::endl;
  } else {
    std::cout << "BEWARE! " << size << " Associations " << std::endl;
  }
  
  LHCb::MCParticle::ConstVector output(mcParticles);
  
//   std::cout << "SORTING " << mcParticles.size() << " MCPs " << std::endl;
//   printMCPIDs(mcParticles);
  std::stable_sort( output.begin() , output.end() , MCSortLogic() ) ;
//   std::cout << "SORTED and got " << output.size() << " MCPs" << std::endl;
//   printMCPIDs(output);
  
  return output;
  
//  return LoKi::MCTrees::buildTrees(mcParticles);
}
//=============================================================================
LHCb::MCParticle::ConstVector 
MCMatchObjP2MCRelator::sort(const LHCb::MCParticle::Container* mcParticles) const 
{
  return LoKi::MCTrees::buildTrees(mcParticles);
}
//=============================================================================
void MCMatchObjP2MCRelator::addTables(LoKi::MCMatchObj* matcher) const 
{
  for (Addresses::const_iterator item = m_PP2MC.begin(); item!=m_PP2MC.end(); ++item) {
    const std::string& address = *item;
    if (exist<LoKi::Types::TablePP2MC>(address) ) {
      verbose() << "Adding table " << address << std::endl;
      LoKi::Types::TablePP2MC* table = get<LoKi::Types::TablePP2MC>(address);
      matcher->addMatchInfo(table);
    } else {
      Error ( " There is no valid data at '" + address + "'" ).ignore() ; 
    }
  }
}
//=============================================================================
// Destructor
//=============================================================================
MCMatchObjP2MCRelator::~MCMatchObjP2MCRelator() {}
//=============================================================================
