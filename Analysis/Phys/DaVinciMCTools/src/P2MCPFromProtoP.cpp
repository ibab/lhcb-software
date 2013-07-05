// $Id: P2MCPFromProtoP.cpp,v 1.5 2009-11-10 15:33:42 pkoppenb Exp $
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

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
P2MCPFromProtoP::P2MCPFromProtoP( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : Particle2MCAssociatorBase ( type, name , parent ),
    m_PP2MC()
{
  declareProperty("Locations",m_PP2MC,"Protoparticle locations (without /Event)");
}
//=============================================================================
StatusCode P2MCPFromProtoP::initialize() 
{
  const StatusCode sc = Particle2MCAssociatorBase::initialize();
  if ( sc.isFailure() ) return sc;

  if ( m_PP2MC.empty() )
  {
    if ( context() == "Hlt" || context() == "HLT" )
    {
      m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::HltCharged ) ;
      m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::HltNeutrals ) ;
    } 
    else 
    {
      m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Charged  ) ;
      m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Upstream ) ;
      m_PP2MC.push_back ( "Relations/" + LHCb::ProtoParticleLocation::Neutrals ) ;
    }
    debug() << "Will look into " << m_PP2MC << endmsg ;
  } 
  else
  {
    // Non standard
    info() << "Will look into " << m_PP2MC << endmsg ;
  }

  return sc;
}
//=============================================================================
Particle2MCParticle::ToVector
P2MCPFromProtoP::relatedMCPsImpl(const LHCb::Particle* particle,
                                 const LHCb::MCParticle::ConstVector& mcps) const
{
  Particle2MCParticle::ToVector associations;
  if ( ( particle->isBasicParticle() ) && ( NULL != particle->proto() ) )
  {
    loadTables();
    for ( Tables::const_iterator iTable = i_tables().begin();
          iTable != i_tables().end(); ++iTable )
    {
      const LoKi::Types::TablePP2MC::Range range = (*iTable)->relations(particle->proto());
      for ( LoKi::Types::TablePP2MC::Range::const_iterator rel = range.begin();
            rel != range.end(); ++rel )
      {
        associations.push_back( MCAssociation(rel->to(),rel->weight()) );
      }
    }
  }
  else 
  {
    Warning( "Composite or ProtoParticle-less association not implemented!", 
             StatusCode::SUCCESS, 10 );
  }

  // check if the associated MCPs are in the input container, if not,
  // remove the association!

  return Particle2MCParticle::FilterMCAssociations(associations,mcps);
}
//=============================================================================
void P2MCPFromProtoP::loadTables() const
{
  m_tables.clear();

  for ( Addresses::const_iterator item = m_PP2MC.begin(); item!=m_PP2MC.end(); ++item )
  {
    const std::string& address = *item;
    LoKi::Types::TablePP2MC * table = getIfExists<LoKi::Types::TablePP2MC>(address,false);
    if ( !table ) { table = getIfExists<LoKi::Types::TablePP2MC>(address,true); }
    if (  table ) 
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "Adding table " << address << endmsg;
      m_tables.push_back(table);
    } 
//     else
//     {
//       Error ( " There is no valid data at '" + address + "'" ).ignore() ;
//     }
  }
}
//=============================================================================
// Destructor
//=============================================================================
P2MCPFromProtoP::~P2MCPFromProtoP() {}
//=============================================================================
// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( P2MCPFromProtoP )
//=============================================================================
