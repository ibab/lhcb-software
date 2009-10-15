// $Id: Particle2MCAssociatorBase.cpp,v 1.13 2009-10-15 14:07:30 jpalac Exp $
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
  base_class ( type, name , parent ),
  m_defMCLoc(LHCb::MCParticleLocation::Default)
{
  declareProperty ( "MCParticleDefaultLocation" , m_defMCLoc  ) ;
}
//=============================================================================
const LHCb::MCParticle*
Particle2MCAssociatorBase::relatedMCP(const LHCb::Particle* particle) const
{
  return i_bestMCPWithCheck(i_relatedMCPs(particle,
                                        m_defMCLoc) );
}
//=============================================================================
const LHCb::MCParticle*
Particle2MCAssociatorBase::operator()(const LHCb::Particle* particle) const
{
  return i_bestMCPWithCheck(i_relatedMCPs(particle,
                                        m_defMCLoc) );
}
//=============================================================================
const LHCb::MCParticle*
Particle2MCAssociatorBase::relatedMCP(const LHCb::Particle* particle,
                                      const std::string& mcParticleLocation) const
{
  return i_bestMCPWithCheck( i_relatedMCPs(particle, 
                                           mcParticleLocation)  );
}
//=============================================================================
const LHCb::MCParticle*
Particle2MCAssociatorBase::relatedMCP(const LHCb::Particle* particle,
                                      const LHCb::MCParticle::ConstVector& mcParticles) const
{
  return i_bestMCPWithCheck( i_relatedMCPs( particle, 
                                            mcParticles.begin(), 
                                            mcParticles.end()    )   );
}
//=============================================================================
const LHCb::MCParticle*
Particle2MCAssociatorBase::relatedMCP(const LHCb::Particle* particle,
                                      const LHCb::MCParticle::Container& mcParticles) const
{
  return i_bestMCPWithCheck( i_relatedMCPs( particle, 
                                            mcParticles.begin(), 
                                            mcParticles.end()    )   );
}
//=============================================================================
bool
Particle2MCAssociatorBase::isAssociated(const LHCb::Particle*, 
                                        const LHCb::MCParticle*) const
{
  return false;
}
//=============================================================================
Particle2MCParticle::ToVector 
Particle2MCAssociatorBase::relatedMCPsImpl(const LHCb::Particle* particle,
                                           const LHCb::MCParticle::ConstVector& mcParticles) const 
{
  typedef LHCb::MCParticle::ConstVector::const_iterator Iter;
  Particle2MCParticle::ToVector associations;
  for (Iter iMCP = mcParticles.begin() ; iMCP != mcParticles.end() ; ++iMCP){
    if (isAssociated(particle, *iMCP) ) {
      const double wt = associationWeight(particle, *iMCP);
      associations.push_back( MCAssociation(*iMCP, wt ) );
    }
  }
  return associations;
}
//=============================================================================
Particle2MCParticle::ToVector 
Particle2MCAssociatorBase::relatedMCPs(const LHCb::Particle* particle) const 
{
  return i_relatedMCPs( particle, m_defMCLoc );
}
//=============================================================================
Particle2MCParticle::ToVector 
Particle2MCAssociatorBase::relatedMCPs(const LHCb::Particle* particle,
                                       const std::string& mcParticleLocation) const 
{
  return i_relatedMCPs( particle, mcParticleLocation );
}
//=============================================================================
Particle2MCParticle::ToVector 
Particle2MCAssociatorBase::relatedMCPs(const LHCb::Particle* particle,
                                       const LHCb::MCParticle::ConstVector& mcParticles) const 
{
  return i_relatedMCPs( particle, mcParticles.begin(), mcParticles.end() ); 
}
//=============================================================================
Particle2MCParticle::ToVector 
Particle2MCAssociatorBase::relatedMCPs(const LHCb::Particle* particle,
                                       const LHCb::MCParticle::Container& mcParticles) const 
{
  return i_relatedMCPs( particle, mcParticles.begin(), mcParticles.end() );  
}
//=============================================================================
double
Particle2MCAssociatorBase::associationWeight(const LHCb::Particle*, 
                                             const LHCb::MCParticle*) const
{
  return 0;
}
//=============================================================================
// initialize
//=============================================================================
StatusCode Particle2MCAssociatorBase::initialize() {
  return base_class::initialize();
}
//=============================================================================
// finalize
//=============================================================================
StatusCode Particle2MCAssociatorBase::finalize() {
  return base_class::finalize() ; 
}
//=============================================================================
// Destructor
//=============================================================================
Particle2MCAssociatorBase::~Particle2MCAssociatorBase() {}
//=============================================================================
