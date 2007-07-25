// $Id: FilterMCParticleArrayByDecay.cpp,v 1.1 2007-07-25 14:36:34 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from LHCb
#include "MCInterfaces/IMCDecayFinder.h"

// local
#include "FilterMCParticleArrayByDecay.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterMCParticleArrayByDecay
//
// 2007-07-25 : Juan Palacios
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( FilterMCParticleArrayByDecay );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FilterMCParticleArrayByDecay::FilterMCParticleArrayByDecay( const std::string& type,
                                                            const std::string& name,
                                                            const IInterface* parent )
  : 
  GaudiTool ( type, name , parent ),
  m_decayFinderName("DecayFinder"),
  m_decayDescriptorName("DecayDescriptor"),
  m_mcParticleLocation(LHCb::MCParticleLocation::Default),
  m_decayFinder(0)
{
  declareInterface<IMCParticleArrayFilter>(this);

  declareProperty("DecayFinderName", m_decayFinderName);
  declareProperty("DecayDescriptorName", m_decayDescriptorName);
  declareProperty("MCParticleLocation", m_mcParticleLocation); 
}
//=============================================================================
StatusCode FilterMCParticleArrayByDecay::initialize() 
{
  m_decayFinder = tool<IMCDecayFinder>("MCDecayFinder", 
                                       m_decayFinderName, this);

  return (0!=m_decayFinder) ? StatusCode::SUCCESS : StatusCode::FAILURE;

}
//=============================================================================
StatusCode FilterMCParticleArrayByDecay::filter( const LHCb::MCParticle::ConstVector& in, 
                                                 LHCb::MCParticle::ConstVector& out) const
{
  LHCb::MCParticle::ConstVector heads;
  findDecayHeads(in, heads);
  findAllDecay(heads, out);
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode FilterMCParticleArrayByDecay::filter( LHCb::MCParticle::ConstVector& mcParts) const
{
  LHCb::MCParticle::ConstVector filteredParts;
  filter( mcParts, filteredParts);
  mcParts=filteredParts;
  return StatusCode::SUCCESS;
}
//=============================================================================
void FilterMCParticleArrayByDecay::findDecayHeads(const LHCb::MCParticle::ConstVector& in, 
                                                  LHCb::MCParticle::ConstVector& heads) const
{
  if (!m_decayFinder->hasDecay(in)) return;
  if ( !heads.empty() ) heads.clear(); // this shouldn't delete the pointers

  // Need to make sure we actually get the original pointer.
  const LHCb::MCParticle* tmp(NULL);
  while ( m_decayFinder->findDecay(in, tmp) ){
    if (NULL!=tmp) {
      LHCb::MCParticle* head= tmp->clone();
      heads.push_back(head);  
    }
  }

}
//=============================================================================
void FilterMCParticleArrayByDecay::findAllDecay(const LHCb::MCParticle::ConstVector& heads, 
                                                LHCb::MCParticle::ConstVector& decay) const
{
  if (!decay.empty()) decay.clear();
  LHCb::MCParticle::Vector tmpDecay;
  LHCb::MCParticle::ConstVector::const_iterator iHead = heads.begin();
  for ( ; iHead!= heads.end(); ++iHead) {
    LHCb::MCParticle*  tmp= (*iHead)->clone();
    tmpDecay.push_back(tmp);
    m_decayFinder->decayMembers(tmp, tmpDecay);
  }
  decay.resize(tmpDecay.size());
  std::copy( tmpDecay.begin(), tmpDecay.end(), decay.begin() );
}

//=============================================================================
StatusCode FilterMCParticleArrayByDecay::finalize() 
{
  toolSvc()->releaseTool(m_decayFinder);
  return GaudiTool::finalize();
}
//=============================================================================
// Destructor
//=============================================================================
FilterMCParticleArrayByDecay::~FilterMCParticleArrayByDecay() {} 

//=============================================================================
