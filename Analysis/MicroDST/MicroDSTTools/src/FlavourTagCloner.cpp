// $Id: FlavourTagCloner.cpp,v 1.3 2009-07-30 10:03:01 jpalac Exp $
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

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( FlavourTagCloner );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FlavourTagCloner::FlavourTagCloner( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : base_class ( type, name , parent )
{
}
//=============================================================================
LHCb::FlavourTag* FlavourTagCloner::operator() (const LHCb::FlavourTag* tag)
{
  return this->clone(tag);
}
//=============================================================================
LHCb::FlavourTag* FlavourTagCloner::clone( const LHCb::FlavourTag* tag ) 
{

  LHCb::FlavourTag* tmp = 
    cloneKeyedContainerItem<LHCb::FlavourTag, BasicFTCopy>(tag);

  tmp->setTaggedB( getStoredClone<LHCb::Particle>( tag->taggedB() ) );

  return tmp; 
}
//=============================================================================
// Destructor
//=============================================================================
FlavourTagCloner::~FlavourTagCloner() {} 

//=============================================================================
