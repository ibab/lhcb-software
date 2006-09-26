// $Id: HltDataStore.cpp,v 1.2 2006-09-26 13:54:51 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

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


