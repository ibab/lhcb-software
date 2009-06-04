// $Id: P2MCPFromProtoP.cpp,v 1.3 2009-06-04 14:02:19 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "P2MCPFromProtoP.h"

//-----------------------------------------------------------------------------
// Implementation file for class : P2MCPFromProtoP
//
// 2009-03-31 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( P2MCPFromProtoP );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
P2MCPFromProtoP::P2MCPFromProtoP( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : 
  Particle2MCAssociatorBase ( type, name , parent ),
  m_PP2MC()
{
  m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Charged  ) ;
  m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Upstream ) ;
  m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Neutrals ) ;
  m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::HltCharged ) ;
  m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::HltNeutrals ) ;
}
//=============================================================================
StatusCode P2MCPFromProtoP::initialize() {
  StatusCode sc = Particle2MCAssociatorBase::initialize();
  if (sc.isFailure()) return sc;
  return sc;
}
//=============================================================================
Particle2MCParticle::ToVector 
P2MCPFromProtoP::relatedMCPsImpl(const LHCb::Particle* particle,
                                 const LHCb::MCParticle::ConstVector& mcps) const 
{
  Particle2MCParticle::ToVector associations;
  if ( ( particle->isBasicParticle() ) && ( 0!=particle->proto() )   ) {
    loadTables();
    for (Tables::const_iterator iTable = i_tables().begin();
         iTable != i_tables().end(); 
         ++iTable) {
      LoKi::Types::TablePP2MC::Range range = (*iTable)->relations(particle->proto());
      for (LoKi::Types::TablePP2MC::Range::const_iterator rel = range.begin();
           rel != range.end();
           ++rel) {
        associations.push_back(MCAssociation(rel->to(), rel->weight() ) );
      }
    }
  } else {
    Warning("Composite or ProtoParticle-less association not implemented!", StatusCode::SUCCESS, 10);
  }

  // check if the associaited MCPs are in the input container, if not,
  // remove the association!
  
  return Particle2MCParticle::FilterMCAssociations(associations,
                                                   mcps);
  
}
//=============================================================================
void P2MCPFromProtoP::loadTables() const
{
  m_tables.clear();

  for (Addresses::const_iterator item = m_PP2MC.begin(); item!=m_PP2MC.end(); ++item) {
    const std::string& address = *item;
    if (exist<LoKi::Types::TablePP2MC>(address) ) {
      verbose() << "Adding table " << address << endmsg;
      LoKi::Types::TablePP2MC* table = get<LoKi::Types::TablePP2MC>(address);
      m_tables.push_back(table);
    } else {
      Error ( " There is no valid data at '" + address + "'" ).ignore() ; 
    }
  }
}
//=============================================================================
// Destructor
//=============================================================================
P2MCPFromProtoP::~P2MCPFromProtoP() {} 
//=============================================================================
// finalize
//=============================================================================
StatusCode P2MCPFromProtoP::finalize() {
  return Particle2MCAssociatorBase::finalize() ; 
}
//=============================================================================
