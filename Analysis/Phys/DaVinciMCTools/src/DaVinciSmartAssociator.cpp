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

  if (msgLevel(MSG::VERBOSE)) 
    verbose() << "Performing smart association on " 
              << particle 
              << endmsg;

  Particle2MCParticle::ToVector associatedParts;

  if (!particle) Exception("The smart associator was asked to associate a NULL particle, exiting.").ignore();

  //Now we get the association result based on the particle type
  if (particle->isBasicParticle()){ //if this is a stable
      if (msgLevel(MSG::VERBOSE)) 
        verbose() << "Associating a basic particle with pid = " 
                  << particle->particleID().pid() 
                  << endmsg; 

      associatedParts = m_weightedAssociation->relatedMCPs(particle,mcps);

      if (msgLevel(MSG::VERBOSE)) 
        verbose() << "Associated a basic particle with pid = " 
                  << particle->particleID().pid() 
                  << endmsg;
  } else{ //If composite use BackCat
    if (msgLevel(MSG::VERBOSE)) verbose() << "Associating a composite particle with pid = " 
                                          << particle->particleID().pid() 
                                          << endmsg;
    if ( 0!=m_bkg ) {
      associatedParts.push_back(MCAssociation(m_bkg->origin(particle), 1.)); 
    } else Exception("The BackCat tool could not be found, exiting.").ignore();
    if (msgLevel(MSG::VERBOSE)) verbose() << "Associated a composite particle with pid = " 
                                          << particle->particleID().pid() 
                                          << endmsg;
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
  m_weightedAssociation = tool<IParticle2MCWeightedAssociator>("P2MCPFromProtoP");

  return sc;
}
//=============================================================================
// finalize
//=============================================================================
StatusCode DaVinciSmartAssociator::finalize() {
  return Particle2MCAssociatorBase::finalize() ; 
}
//=============================================================================
