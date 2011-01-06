// $Id: DataStoreLoadAgent.cpp,v 1.2 2005-03-18 15:47:08 cattanem Exp $
// Include files 

#include <iostream>

#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "DetDesc/IDetectorElement.h"

// local
#include "DetDesc/DataStoreLoadAgent.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DataStoreLoadAgent
//
// 2005-02-24 : Marco CLEMENCIC
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DataStoreLoadAgent::DataStoreLoadAgent(  ):
  m_loadedObjects(0),
  m_maxDepth(0)
{
}
//=============================================================================
// Destructor
//=============================================================================
DataStoreLoadAgent::~DataStoreLoadAgent() {}
//=============================================================================
// Analyze
//=============================================================================
bool DataStoreLoadAgent::analyse (IRegistry *pObject, int level){
  DataObject *tmp;
  
   // I couldn't find a better way to load an object.
  StatusCode sc = pObject->dataSvc()->retrieveObject(pObject->identifier(),tmp);

  if (level > m_maxDepth) m_maxDepth = level;

  if ( sc.isSuccess() ){ // object was loaded successfully
    ++m_loadedObjects; 
  } else { // obj. was not loaded: stop the recursion (there shouldn't be any level downstream)
    return false;
  }
  
  return true;
}
//=============================================================================
