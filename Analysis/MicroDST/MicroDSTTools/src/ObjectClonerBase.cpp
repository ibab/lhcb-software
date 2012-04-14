// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "ObjectClonerBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ObjectClonerBase
//
// 2012-03-30 : Chris Jones
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ObjectClonerBase::ObjectClonerBase( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : MicroDSTTool ( type, name , parent )
{
  declareProperty("TESVetoList",m_tesVetoList);
}

//=============================================================================
// Destructor
//=============================================================================
ObjectClonerBase::~ObjectClonerBase() {} 

//=============================================================================
