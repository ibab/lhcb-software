// $Id: FlavourTagCloner.cpp,v 1.4 2010-08-11 12:52:52 jpalac Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// LHCb
#include "Event/FlavourTag.h"

// local
#include "FlavourTagCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FlavourTagCloner
//
// 2008-08-08 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FlavourTagCloner::FlavourTagCloner( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : base_class ( type, name , parent ) 
{ }

//=============================================================================

LHCb::FlavourTag* FlavourTagCloner::operator() (const LHCb::FlavourTag* tag)
{
  return this->clone(tag);
}

//=============================================================================

LHCb::FlavourTag* FlavourTagCloner::clone( const LHCb::FlavourTag* tag )
{
  // Clone the FT object
  LHCb::FlavourTag * tmp = cloneKeyedContainerItem<BasicFTCopy>(tag);

  // Update the Particle SmartRef
  tmp->setTaggedB( getStoredClone<LHCb::Particle>( tag->taggedB() ) );

  // Clear the taggers vector
  tmp->setTaggers( std::vector<LHCb::Tagger>() );

  // return
  return tmp;
}

//=============================================================================
// Destructor
//=============================================================================
FlavourTagCloner::~FlavourTagCloner() {}

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( FlavourTagCloner )
