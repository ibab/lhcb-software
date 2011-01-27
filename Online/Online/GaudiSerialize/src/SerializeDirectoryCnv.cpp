// $Id: SerializeDirectoryCnv.cpp,v 1.8 2010/09/27 15:43:53 frankb Exp $
//------------------------------------------------------------------------------
//
// Implementation of class :  SerializeDirectoryCnv
//
// Author :                   Markus Frank
//
//------------------------------------------------------------------------------

// Include files
#include "SerializeDirectoryCnv.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/CnvFactory.h"

// Factory declaration
using namespace Gaudi;
PLUGINSVC_FACTORY_WITH_ID(SerializeDirectoryCnv, 
                          ConverterID(SERIALIZE_StorageType,CLID_StatisticsDirectory),
                          IConverter*(long, CLID, ISvcLocator*) );

// Create transient object from persistent data
StatusCode 
SerializeDirectoryCnv::createObj(IOpaqueAddress* /* pAddr */,DataObject*& refpObj)  {
  refpObj = new NTuple::Directory();
  return StatusCode::SUCCESS;
}

// Converter overrides: Convert the transient object to the requested representation.
StatusCode 
SerializeDirectoryCnv::createRep(DataObject* pObj,IOpaqueAddress*& /* refpAddr */)  {
  std::string dsc;
  if ( objType() == CLID_StatisticsDirectory )  {
    dsc = "Directory containing statistics results.";
  }
  else if ( objType() == CLID_StatisticsFile )  {
    dsc = "File containing statistics results.";
  }
  else  {
    return StatusCode::FAILURE;
  }
  log() << MSG::INFO << "Directory:" << pObj->registry()->identifier() << ": " << dsc << endmsg;
  return StatusCode::SUCCESS;
}

// Converter overrides: Update the references of an updated transient object.
StatusCode 
SerializeDirectoryCnv::updateObjRefs(IOpaqueAddress* pAddr, 
                                DataObject* pObject)
{
  return StatusCode::SUCCESS;
}
