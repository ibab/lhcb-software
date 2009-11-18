// $Id: FlavourTagDeepCloner.cpp,v 1.3 2009-11-18 07:30:26 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// LHCb
#include "Event/FlavourTag.h"

//MicroDST
#include "MicroDST/ICloneTagger.h"
#include "MicroDST/ICloneParticle.h"
// local
#include "FlavourTagDeepCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FlavourTagDeepCloner
//
// 2008-08-08 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( FlavourTagDeepCloner );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FlavourTagDeepCloner::FlavourTagDeepCloner( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  : 
  base_class ( type, name , parent ),
  m_particleCloner(0),
  m_particleClonerName("ParticleCloner")

{
  declareProperty("ICloneParticle", m_particleClonerName);
}
//=============================================================================
StatusCode FlavourTagDeepCloner::initialize()
{
  debug() << "==> Initialize" << endmsg;

  StatusCode sc = base_class::initialize();
  if (! sc.isSuccess() ) return sc;

  debug() << "Going to get ParticleCloner" << endmsg;
  
  m_particleCloner = tool<ICloneParticle>(m_particleClonerName, 
                                          this->parent() );

  if (m_particleCloner) {
    debug() << "Found ParticleCloner " << m_particleCloner->name() << endmsg;
  } else {
    error() << "Failed to find ParticleCloner" << endmsg;
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
LHCb::FlavourTag* FlavourTagDeepCloner::operator() (const LHCb::FlavourTag* tag)
{
  return this->clone(tag);
}
//=============================================================================
LHCb::FlavourTag* FlavourTagDeepCloner::clone( const LHCb::FlavourTag* tag ) 
{

  LHCb::FlavourTag* tmp = 
    cloneKeyedContainerItem<LHCb::FlavourTag, BasicFTCopy>(tag);

  tmp->setTaggedB( getStoredClone<LHCb::Particle>( tag->taggedB() ) );

  // will have to deal with the taggers by hand, since LHCb::Tagger is
  // not a contained object.
  // loop over taggers, clone tagger particles, clone them, and set them.

  return tmp; 
}
//=============================================================================
// Destructor
//=============================================================================
FlavourTagDeepCloner::~FlavourTagDeepCloner() {} 

//=============================================================================
