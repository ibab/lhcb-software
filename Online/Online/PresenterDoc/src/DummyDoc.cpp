// $Id: $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "DummyDoc.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DummyDoc
//
// 2012-03-30 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( DummyDoc )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DummyDoc::DummyDoc( const std::string& type,
                    const std::string& name,
                    const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IPresenterDoc>(this);
}
//=============================================================================
// Destructor
//=============================================================================
DummyDoc::~DummyDoc() {} 


//=========================================================================
//  
//=========================================================================
std::string DummyDoc::document ( std::string& histoName, int binX, int binY ) {
  char tmp[1000];
  sprintf( tmp, "Histo '%s' bin x %d y %d", histoName.c_str(), binX, binY );
  return std::string( tmp );
}
//=============================================================================
