// $Id: FlavourTag.cpp,v 1.1 2002-05-23 23:03:19 gcorti Exp $
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
