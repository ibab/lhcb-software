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
  declareInterface<IParticle2MCWeightedAssociator>(this);
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

//  ProtoParticle2MCLinker::ToRange associatedParts;
  Particle2MCParticle::ToVector associatedParts;
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
