// $Id: FlavourTagDeepCloner.cpp,v 1.1 2009-11-17 16:41:27 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// LHCb
#include "Event/FlavourTag.h"

//MicroDST
#include "MicroDST/ICloneTagger.h"

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
  base_class ( type, name , parent )

{
}
//=============================================================================
StatusCode FlavourTagDeepCloner::initialize()
{
  debug() << "==> Initialize" << endmsg;

  return base_class::initialize();

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

  return tmp; 
}
//=============================================================================
// Destructor
//=============================================================================
FlavourTagDeepCloner::~FlavourTagDeepCloner() {} 

//=============================================================================
