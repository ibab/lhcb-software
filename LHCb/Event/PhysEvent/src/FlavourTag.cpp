// $Id: FlavourTag.cpp,v 1.2 2002-11-27 14:27:22 gcorti Exp $
// Include files 

// STD and STL
#include <iostream> // from Gaudi
#include "GaudiKernel/StreamBuffer.h" 

// local
#include "Event/FlavourTag.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FlavourTag
//                                 derived from class KeyedObject
//
// 2002-05-22 : Gloria Corti: necessary for return type of enum 
//-----------------------------------------------------------------------------

//=============================================================================
// Copy constructor
//=============================================================================
FlavourTag::FlavourTag(const FlavourTag& tag)
  : KeyedObject<int>()
  , m_decision(tag.decision()), m_type(tag.type())
  , m_taggedB(tag.taggedB()), m_tagger(tag.tagger())
{
}

//=============================================================================
// Clone 
//=============================================================================
FlavourTag* FlavourTag::clone() const
{
  return new FlavourTag(*this);
}

//=============================================================================
// Assignment operator
//=============================================================================
FlavourTag &FlavourTag::operator=(const FlavourTag& orig) {
  
  // protect against self assignement
  if( this != &orig ) {
    m_taggedB = orig.taggedB();
    m_tagger = orig.tagger();
    m_type = orig.type();
    m_decision = orig.decision();
  }
  return *this;
}

//=============================================================================
// Retrieve The result of the tagging algorithm
//=============================================================================
FlavourTag::TagResult FlavourTag::decision() const 
{
  return m_decision;
}

//=============================================================================
// Retrieve The result of the tagging algorithm (non-const)
//=============================================================================
FlavourTag::TagResult FlavourTag::decision()
{
  return m_decision;
}

//=============================================================================
// Retrieve the kind of tagging algorithm used
//=============================================================================
FlavourTag::TagType FlavourTag::type() const 
{
  return m_type;
}

//=============================================================================
// Retrieve the kind of tagging algorithm used (non-const)
//=============================================================================
FlavourTag::TagType FlavourTag::type()
{
  return m_type;
}
