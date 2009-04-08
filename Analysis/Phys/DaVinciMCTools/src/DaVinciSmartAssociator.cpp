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

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DaVinciSmartAssociator::DaVinciSmartAssociator( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : 
  Particle2MCAssociatorBase( type, name , parent ),
  m_weightedAssociation(0),
  m_bkg(0)
{

}
//=============================================================================
// Destructor
//=============================================================================
DaVinciSmartAssociator::~DaVinciSmartAssociator() {} 
//=============================================================================
// Make & return the linker
//=============================================================================
Particle2MCParticle::ToVector 
DaVinciSmartAssociator::relatedMCPsImpl(const LHCb::Particle* particle,
                                        const LHCb::MCParticle::ConstVector& mcps) const 
{
//We associate according to the particle type: protoparticle associators
//are used for neutral and charged stable tracks, otherwise we use BackCat
//for composites. The associator wrapper makes sure the linkers thus created are
//deleted in the correct manner.

  Particle2MCParticle::ToVector associatedParts;
  //Now we get the association result based on the particle type
  if (particle->isBasicParticle()){ //if this is a stable
      associatedParts = m_weightedAssociation->relatedMCPs(particle,mcps);
  } else{ //If composite use BackCat
    if ( 0!=m_bkg ) {
      const LHCb::MCParticle* assocMCP = m_bkg->origin(particle);
      if (0!=assocMCP ) {
        associatedParts.push_back(MCAssociation(m_bkg->origin(particle), 1.)); 
      }
    }
  }

  // check if the associaited MCPs are in the input container, if not,
  // remove the association!

  return Particle2MCParticle::FilterMCAssociations(associatedParts,
                                                   mcps);
 
}
//=============================================================================
// initialize
//=============================================================================
StatusCode DaVinciSmartAssociator::initialize() {
  StatusCode sc = Particle2MCAssociatorBase::initialize();
  if (sc.isFailure()) return sc;
  //Init the BackCat instance
  m_bkg = tool<IBackgroundCategory>( "BackgroundCategory", this );
  //And the P2MCPFromProtoP instance that will do the associating
  //of charged and neutral "stables" 
  m_weightedAssociation = tool<IParticle2MCWeightedAssociator>("P2MCPFromProtoP", this);

  return sc;
}
//=============================================================================
// finalize
//=============================================================================
StatusCode DaVinciSmartAssociator::finalize() {
  return Particle2MCAssociatorBase::finalize() ; 
}
//=============================================================================
