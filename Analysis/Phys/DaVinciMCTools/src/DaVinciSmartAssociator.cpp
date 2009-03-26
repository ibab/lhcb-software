// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "DaVinciSmartAssociator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DaVinciSmartAssociator
//
// 2009-01-13 : Vladimir Gligorov 
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( DaVinciSmartAssociator );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DaVinciSmartAssociator::DaVinciSmartAssociator( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : 
  Particle2MCAssociatorBase( type, name , parent ),
  m_linkerTool_cPP(0),
  m_linkerTool_nPP(0),
  m_bkg(0)
{
  declareInterface<IParticle2MCAssociator>(this);
}
//=============================================================================
// Destructor
//=============================================================================
DaVinciSmartAssociator::~DaVinciSmartAssociator() {} 
//=============================================================================
// Make & return the linker
//============================================================================= 
//============================================================================= 
Particle2MCParticle::ToVector 
DaVinciSmartAssociator::relatedMCPs(const LHCb::Particle* particle) const 
{
//We associate according to the particle type: protoparticle associators
//are used for neutral and charged stable tracks, otherwise we use BackCat
//for composites. The associator wrapper makes sure the linkers thus created are
//deleted in the correct manner.

  ProtoParticle2MCLinker::ToRange associatedParts;
  //Now we get the association result based on the particle type
  if (particle->isBasicParticle()){ //if this is a stable
    ProtoParticle2MCLinker* linker(0);
    verbose() << "About to get the array of matching particles" << endmsg;
    if ( particle->particleID().pid() == 22 || 
         particle->particleID().pid() == 111) {
      linker = dynamic_cast<ProtoParticle2MCLinker*>(m_linkerTool_nPP->linker(Particle2MCMethod::NeutralPP) );
    } else {
      linker = dynamic_cast<ProtoParticle2MCLinker*>(m_linkerTool_cPP->linker(Particle2MCMethod::ChargedPP) ); 
    }
    if (0!=linker) {
      associatedParts = linker->rangeFrom(particle->proto());
    }
    
  } else{ //If composite use BackCat
    if (0!=m_bkg) {
      associatedParts.push_back(MCAssociation(m_bkg->origin(particle), 1.));  
    }
  }

  return associatedParts; 
}
//============================================================================= 
Particle2MCParticle::ToVector 
DaVinciSmartAssociator::relatedMCPs(const LHCb::Particle* particle,
                                    const std::string& mcParticleLocation) const 
{
  if (mcParticleLocation!=LHCb::MCParticleLocation::Default) {
    Warning("associate(const LHCb::Particle*  particle, const std::string& mcParticleLocation) NOT implemented",
            1, StatusCode::SUCCESS).ignore();
    return ProtoParticle2MCLinker::ToRange();
  } else {
    return relatedMCPs(particle);
  }

}
//=============================================================================
bool DaVinciSmartAssociator::isAssociated(const LHCb::Particle* particle, 
                                          const LHCb::MCParticle* mcParticle) const
{
  ///@todo Should implement this one!
  if ( !particle->isBasicParticle() ) {
    return ( 0!= m_bkg                    && 
             0!= m_bkg->origin(particle)  && 
             m_bkg->origin(particle) == mcParticle );
  } else {
    Warning("isAssociated(const LHCb::Particle*  particle, const LHCb::MCParticle) NOT implemented for Basic particles, returninf false",
            1, 
            StatusCode::SUCCESS).ignore();
    return false;
  }
  
}
//=============================================================================
double DaVinciSmartAssociator::associationWeight(const LHCb::Particle* particle,
                                                 const LHCb::MCParticle* mcParticle) const
{
  ///@todo Should implement this one!

  return isAssociated(particle, mcParticle) ? 1. : 0. ;

}
//=============================================================================
// initialize
//=============================================================================
StatusCode DaVinciSmartAssociator::initialize() {
  StatusCode sc = Particle2MCAssociatorBase::initialize();
  if (sc.isFailure()) return sc;
  //Init the BackCat instance
  m_bkg = tool<IBackgroundCategory>( "BackgroundCategory", this );
  //And the associator wrappers
  m_linkerTool_cPP = tool<IDaVinciAssociatorsWrapper>("DaVinciAssociatorsWrapper",
                                                      "Wrapper_CAT_cPP",
                                                      this);
  m_linkerTool_nPP = tool<IDaVinciAssociatorsWrapper>("DaVinciAssociatorsWrapper",
                                                      "Wrapper_CAT_nPP",
                                                      this);
  return sc;
}
//=============================================================================
// finalize
//=============================================================================
StatusCode DaVinciSmartAssociator::finalize() {
  return Particle2MCAssociatorBase::finalize() ; 
}
//=============================================================================
