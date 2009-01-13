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
  : GaudiTool ( type, name , parent )
  , m_linkerTool_cPP(NULL)
  , m_linkerTool_nPP(NULL)
  , m_linker_charged(NULL)
  , m_linker_neutral(NULL)
  , m_bkg(NULL)
{
  declareInterface<IDaVinciSmartAssociator>(this);
}
//=============================================================================
// Destructor
//=============================================================================
DaVinciSmartAssociator::~DaVinciSmartAssociator() {} 
//=============================================================================
// Make & return the linker
//============================================================================= 
ProtoParticle2MCLinker::ToRange DaVinciSmartAssociator::associate( LHCb::Particle* particleToBeAssociated ) {
//We associate according to the particle type: protoparticle associators
//are used for neutral and charged stable tracks, otherwise we use BackCat
//for composites. The associator wrapper makes sure the linkers thus created are
//deleted in the correct manner.

  //Clear the array of associated particles
  associatedParts.clear(); 

  //Get the linkers 
  m_linker_charged = (ProtoParticle2MCLinker*) m_linkerTool_cPP->linker(Particle2MCMethod::ChargedPP); 
  m_linker_neutral = (ProtoParticle2MCLinker*) m_linkerTool_nPP->linker(Particle2MCMethod::NeutralPP);
 
  //Now we get the association result based on the particle type
  if (particleToBeAssociated->isBasicParticle()){ //if this is a stable
        verbose() << "About to get the array of matching particles" << endmsg;
        if ( particleToBeAssociated->particleID().pid() == 22 || particleToBeAssociated->particleID().pid() == 111) {
          associatedParts = m_linker_neutral->rangeFrom(particleToBeAssociated->proto()); //for the neutrals
        } else {
          associatedParts = m_linker_charged->rangeFrom(particleToBeAssociated->proto()); //for the charged
        }   
  }else{ //If composite use BackCat
    //First define an instance of the MCAssociation class
    //ProtoParticle2MCLinker::MCAssociation thisCompositeAssociation;
    //thisCompositeAssociation.setTo(m_bkg->origin(particleToBeAssociated)); 
    //thisCompositeAssociation.setWeight(1.);
    associatedParts.push_back(MCAssociation(m_bkg->origin(particleToBeAssociated), 1.));  
  }

  return associatedParts; 
}
//=============================================================================
// initialize
//=============================================================================
StatusCode DaVinciSmartAssociator::initialize() {
  StatusCode sc = GaudiTool::initialize();
  //Init the BackCat instance
  m_bkg = tool<IBackgroundCategory>( "BackgroundCategory", this );
  //And the associator wrappers
  m_linkerTool_cPP = tool<IDaVinciAssociatorsWrapper>("DaVinciAssociatorsWrapper","Wrapper_CAT_cPP",this);
  m_linkerTool_nPP = tool<IDaVinciAssociatorsWrapper>("DaVinciAssociatorsWrapper","Wrapper_CAT_nPP",this);
  return sc;
} 
//=============================================================================
// finalize
//=============================================================================
StatusCode DaVinciSmartAssociator::finalize() {
  return GaudiTool::finalize() ; 
}
