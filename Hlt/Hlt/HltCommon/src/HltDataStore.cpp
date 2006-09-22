// $Id: HltDataStore.cpp,v 1.1 2006-09-22 11:36:07 hernando Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "HltDataStore.h"

//-----------------------------------------------------------------------------
// Implementation file for class : HltDataStore
//
// 2006-07-20 : Jose Angel Hernando Morata
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( HltDataStore );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
HltDataStore::HltDataStore( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IHltDataStore>(this);

}
//=============================================================================
// Destructor
//=============================================================================
HltDataStore::~HltDataStore() {} 

//=============================================================================


