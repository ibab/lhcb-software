// $Id: FlavourTagDeepCloner.cpp,v 1.6 2010-08-11 12:52:52 jpalac Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// LHCb
#include "Event/FlavourTag.h"

//MicroDST
#include "MicroDST/ICloneParticle.h"
// local
#include "FlavourTagDeepCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FlavourTagDeepCloner
//
// 2008-08-08 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FlavourTagDeepCloner::FlavourTagDeepCloner( const std::string& type,
                                            const std::string& name,
                                            const IInterface* parent )
  :
  base_class       ( type, name , parent ),
  m_particleCloner ( NULL )
{
  declareProperty( "ICloneParticle", m_particleClonerName = "ParticleCloner" );
  declareProperty( "CloneTaggerParticles", m_cloneTaggerParticles = true );
}

//=============================================================================

StatusCode FlavourTagDeepCloner::initialize()
{
  const StatusCode sc = base_class::initialize();
  if ( sc.isFailure() ) return sc;

  m_particleCloner = tool<ICloneParticle>( m_particleClonerName, this->parent() );

  return sc;
}

//=============================================================================

LHCb::FlavourTag* FlavourTagDeepCloner::operator() (const LHCb::FlavourTag* tag)
{
  return this->clone(tag);
}

//=============================================================================

LHCb::FlavourTag* FlavourTagDeepCloner::clone( const LHCb::FlavourTag* tag )
{
  // Clone the FT object
  LHCb::FlavourTag* tmp = cloneKeyedContainerItem<BasicFTCopy>(tag);

  // Update and clone the tagged B Particle
  tmp->setTaggedB( getStoredClone<LHCb::Particle>( tag->taggedB() ) );

  // Clone the taggers
  cloneTaggers(tmp);

  // return
  return tmp;
}

//=============================================================================

void FlavourTagDeepCloner::cloneTaggers(LHCb::FlavourTag* tag) const
{
  // Get the vector of taggers
  const std::vector<LHCb::Tagger>& taggers = tag->taggers();

  // Make a new vector for the clones
  std::vector<LHCb::Tagger> clonedTaggers;
  clonedTaggers.reserve( taggers.size() );

  // Clone each tagger
  for ( std::vector<LHCb::Tagger>::const_iterator iTagger = taggers.begin();
        iTagger != taggers.end(); ++iTagger )
  {
    clonedTaggers.push_back( cloneTagger(*iTagger) );
  }

  // update in the FT object
  tag->setTaggers(clonedTaggers);
}

//=============================================================================

LHCb::Tagger FlavourTagDeepCloner::cloneTagger(const LHCb::Tagger& tagger) const
{
  // Clone the tagger object
  LHCb::Tagger tmp(tagger);

  // Should we clone the tagger particles
  if ( m_cloneTaggerParticles )
  {
    // Get the vector of tagger particles
    const SmartRefVector<LHCb::Particle>& taggerParts = tagger.taggerParts();

    // make a new clone vector
    SmartRefVector<LHCb::Particle> clonedTaggerParts;
    clonedTaggerParts.reserve( taggerParts.size() );

    // Clone the particles
    for ( SmartRefVector<LHCb::Particle>::const_iterator iTaggerPart = taggerParts.begin();
          iTaggerPart != taggerParts.end(); ++iTaggerPart )
    {
      clonedTaggerParts.push_back(cloneParticle(*iTaggerPart));
    }

    // Update the particles vector in the tagger object
    tmp.setTaggerParts(clonedTaggerParts);
  }
  else
  {
    // Clear the particle vector
    tmp.clearTaggerParts();
  }

  // return
  return tmp;
}

//=============================================================================
// Destructor
//=============================================================================
FlavourTagDeepCloner::~FlavourTagDeepCloner() {}
//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( FlavourTagDeepCloner )
