// $Id: Particle2MCAssociatorBase.cpp,v 1.5 2009-03-06 14:16:48 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "Kernel/Particle2MCAssociatorBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Particle2MCAssociatorBase
//
// 2009-01-30 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Particle2MCAssociatorBase::Particle2MCAssociatorBase( const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent )
  : 
  GaudiTool ( type, name , parent ),
  m_defMCLoc(LHCb::MCParticleLocation::Default)
{
  declareProperty ( "MCParticleDefaultLocation" , m_defMCLoc  ) ;
}
//=============================================================================
Particle2MCParticle::ToVector 
Particle2MCAssociatorBase::associate(const LHCb::Particle* particle) const 
{
  return associate(particle, m_defMCLoc);
}
//=============================================================================
Particle2MCParticle::ToVector 
Particle2MCAssociatorBase::associate(const LHCb::Particle* particle,
                                     const std::string& mcParticleLocation) const 
{
  Particle2MCParticle::ToVector assocVector;
  Particle2MCParticle::LightTable table = this->relatedMCPs(particle,
                                                            mcParticleLocation);
  

  Particle2MCParticle::LightTable::Range r = table.i_relations(particle);
  for (Particle2MCParticle::LightTable::Range::const_iterator i = r.begin();
       i!=r.end(); ++i) {
    MCAssociation myMCAssoc(i->to(), 1.);
    assocVector.push_back(myMCAssoc);
  }
  return assocVector;
}
//=============================================================================
Particle2MCParticle::LightTable 
Particle2MCAssociatorBase::relatedMCPs(const LHCb::Particle* particle) const
{
  return relatedMCPs(particle, m_defMCLoc);
}
//=============================================================================
Particle2MCParticle::LightTable 
Particle2MCAssociatorBase::relatedMCPs(const LHCb::Particle* particle,
                                       const std::string& mcParticleLocation) const
{
  LHCb::MCParticle::Container* mcps = i_MCParticles(mcParticleLocation);
  if (0!=mcps) {
    return i_relatedMCPs(particle,  mcps->begin(), mcps->end());
  } else {
    return Particle2MCParticle::LightTable();
  }   
}
//=============================================================================
Particle2MCParticle::LightTable 
Particle2MCAssociatorBase::relatedMCPs(const LHCb::Particle* particle,
                                       const LHCb::MCParticle::ConstVector& mcParticles) const
{
  return i_relatedMCPs(particle,  mcParticles.begin(), mcParticles.end());
}
//=============================================================================
Particle2MCParticle::LightTable 
Particle2MCAssociatorBase::associations(const LHCb::Particle::ConstVector& particles) const
{
  return i_associations(particles.begin(), particles.end(), m_defMCLoc);
}
//=============================================================================
Particle2MCParticle::LightTable 
Particle2MCAssociatorBase::associations(const LHCb::Particle::ConstVector& particles,
                                        const std::string& mcParticleLocation) const
{
  return i_associations(particles.begin(), particles.end(), mcParticleLocation);
}
//=============================================================================
Particle2MCParticle::LightTable 
Particle2MCAssociatorBase::associations(const LHCb::Particle::ConstVector& particles,
                                        const LHCb::MCParticle::ConstVector& mcParticles) const
{
  return i_associations(particles.begin(),
                        particles.end(),
                        mcParticles.begin(),
                        mcParticles.end());
  
}
//=============================================================================
Particle2MCParticle::LightTable 
Particle2MCAssociatorBase::associations(const LHCb::Particle::Container& particles) const
{
  return i_associations(particles.begin(), particles.end(), m_defMCLoc);
}
//=============================================================================
Particle2MCParticle::LightTable 
Particle2MCAssociatorBase::associations(const LHCb::Particle::Container& particles,
                                        const std::string& mcParticleLocation) const
{
  return i_associations(particles.begin(), particles.end(), mcParticleLocation);  
}
//=============================================================================
Particle2MCParticle::LightTable 
Particle2MCAssociatorBase::associations(const LHCb::Particle::Container& particles,
                                        const LHCb::MCParticle::ConstVector& mcParticles) const
{
  return i_associations(particles.begin(), particles.end(), mcParticles.begin(), mcParticles.end());          
}
//=============================================================================
bool
Particle2MCAssociatorBase::isMatched(const LHCb::Particle* /*particle */, 
                                     const LHCb::MCParticle* /* mcParticle */) const
{
  return false;
}
//=============================================================================
LHCb::MCParticle::ConstVector 
Particle2MCAssociatorBase::sort(const LHCb::MCParticle::ConstVector& mcParticles) const 
{
  return i_sort(mcParticles.begin(), mcParticles.end());
}
//=============================================================================
LHCb::MCParticle::ConstVector 
Particle2MCAssociatorBase::sort(const LHCb::MCParticle::Container* mcParticles) const
{
  return i_sort(mcParticles->begin(), mcParticles->end());
}
//=============================================================================
// initialize
//=============================================================================
StatusCode Particle2MCAssociatorBase::initialize() {
  return GaudiTool::initialize();
}
//=============================================================================
// finalize
//=============================================================================
StatusCode Particle2MCAssociatorBase::finalize() {
  return GaudiTool::finalize() ; 
}
//=============================================================================
// Destructor
//=============================================================================
Particle2MCAssociatorBase::~Particle2MCAssociatorBase() {}
//=============================================================================
