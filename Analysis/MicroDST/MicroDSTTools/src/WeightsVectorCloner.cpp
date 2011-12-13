// $Id: WeightsVectorCloner.cpp,v 1.4 2010-08-11 12:52:52 jpalac Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// from LHCb
#include "Event/WeightsVector.h"

// local
#include "WeightsVectorCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : WeightsVectorCloner
//
// 2008-04-01 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
WeightsVectorCloner::WeightsVectorCloner( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : base_class ( type, name , parent ) { }

//=============================================================================

LHCb::WeightsVector* WeightsVectorCloner::operator() (const LHCb::WeightsVector* track)
{
  return this->clone(track);
}

//=============================================================================

LHCb::WeightsVector* WeightsVectorCloner::clone(const LHCb::WeightsVector* track)
{
  LHCb::WeightsVector* trackClone =
    cloneKeyedContainerItem<BasicWeightsVectorCloner>(track);

  return trackClone;

}

//=============================================================================
// Destructor
//=============================================================================
WeightsVectorCloner::~WeightsVectorCloner() {}

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( WeightsVectorCloner )
