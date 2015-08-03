// $Id: PreloadGeometryTool.cpp,v 1.1 2006-07-27 20:39:15 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/IDataManagerSvc.h" 

// from DetDesc
#include "DetDesc/DataStoreLoadAgent.h"

// local
#include "PreloadGeometryTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PreloadGeometryTool
//
// 2006-07-26 : Marco Cattaneo
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PreloadGeometryTool )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PreloadGeometryTool::PreloadGeometryTool( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IGenericTool>(this);

}
//=============================================================================
// Destructor
//=============================================================================
PreloadGeometryTool::~PreloadGeometryTool() {} 

//=============================================================================
// Excution method
//=============================================================================
void PreloadGeometryTool::execute() 
{
  DataStoreLoadAgent *loadAgent = new DataStoreLoadAgent();
  IDataManagerSvc *dataMgr = svc<IDataManagerSvc>("DetectorDataSvc", true);
  info() << "Preloading detector geometry..." << endmsg;
  dataMgr->traverseTree(loadAgent);
  info() << "Loaded " << loadAgent->loadedObjects() << " objects to depth of "
         << loadAgent->maxDepth() << " levels" << endmsg;
  delete loadAgent;
  return;
}

//=============================================================================
