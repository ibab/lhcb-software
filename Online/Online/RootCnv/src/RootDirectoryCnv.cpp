// $Id: RootDirectoryCnv.cpp,v 1.1 2010-01-11 17:14:49 frankb Exp $
//------------------------------------------------------------------------------
//
// Implementation of class :  RootDirectoryCnv
//
// Author :                   Markus Frank
//
//------------------------------------------------------------------------------

#include <algorithm>
#include <memory>

// Include files
#include "RootCnvSvc.h"
#include "RootDirectoryCnv.h"
#include "RootDataConnection.h"
#include "RootCnv/RootNTupleDescriptor.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IDataManagerSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/CnvFactory.h"
#include "TBranch.h"
#include "TROOT.h"

// Factory declaration
using namespace Gaudi;

#if 0
PLUGINSVC_FACTORY_WITH_ID( RootDirectoryCnv, 
                           ConverterID(POOL_StorageType,CLID_StatisticsDirectory),
                           IConverter*(long, CLID, ISvcLocator*) );
#endif

// Standard Constructor
RootDirectoryCnv::RootDirectoryCnv (long typ, 
				    const CLID& clid, 
				    ISvcLocator* svc, RootCnvSvc* mgr) 
: RootStatCnv(typ, clid, svc, mgr)
{
}

/// Create transient object from persistent data
StatusCode 
RootDirectoryCnv::createObj(IOpaqueAddress* /* pAddr */,DataObject*& refpObj)  {
  refpObj = new NTuple::Directory();
  return StatusCode::SUCCESS;
}

StatusCode 
RootDirectoryCnv::createRep(DataObject* pObj,IOpaqueAddress*& /* refpAddr */)  {
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
  std::string ident = containerName(pObj->registry());
  std::string path  = fileName(pObj->registry());
  return saveDescription(path, ident, dsc, "", objType());
}

// Fill transient object references
StatusCode RootDirectoryCnv::fillObjRefs(IOpaqueAddress* pAddr, DataObject* pObj)  {
  return updateObjRefs(pAddr, pObj);
}

StatusCode 
RootDirectoryCnv::updateObjRefs(IOpaqueAddress* pAddr, 
                                 DataObject* pObject)
{
  typedef std::vector<RootNTupleDescriptor*> REFS;
  REFS refs;
  StatusCode status = StatusCode(StatusCode::FAILURE,true);
  MsgStream log(msgSvc(), "RootDatabaseCnv");
  if ( pAddr ) {
    IRegistry* pReg = pAddr->registry();
    if ( pReg )  {
      typedef std::vector<IRegistry*> Leaves;
      std::string ident   = pReg->identifier();
      std::string fname   = fileName(pReg);
      std::string cntName = containerName(pReg);
      Leaves leaves;
      RootDataConnection* con = 0;
      status = m_dbMgr->connectDatabase(fname, IDataConnection::READ, &con);
      if ( status.isSuccess() )  {
	TClass* cl = gROOT->GetClass("Gaudi::RootNTupleDescriptor",kTRUE);
	if ( cl ) {
	  TBranch* b = con->getBranch("##Descriptors","GaudiStatisticsDescription",cl);
	  if ( b ) {
	    for(Long64_t n=b->GetEntries(), i=0; i<n; ++i)  {
	      RootNTupleDescriptor* ref=0;
	      b->SetAddress(&ref);
	      int nb = b->GetEntry(i);
	      if ( nb > 1 ) {
		std::string s = ref->container.substr(0,cntName.length());
		// log << MSG::INFO << "Read description:" << ref->container << " " << ident << " " << cntName << endmsg;
		if ( s == cntName )  {
		  if ( ref->container.length() >= cntName.length()+1 )  {
		    if ( ref->container.find('/',cntName.length()+1) == std::string::npos ) {
		      refs.push_back(ref);
		      continue;
		    }
		  }
		}
              }
	      delete ref;
            }
          }
        }
	// log << MSG::ALWAYS << "Got " << refs.size() << " tuple connection(s)....." << endmsg;
        status = m_dataMgr->objectLeaves(pObject, leaves);
        if ( status.isSuccess() )    {
          for(REFS::iterator i = refs.begin(); i != refs.end(); ++i)  {
            REFS::value_type& ref = *i;
            if ( ref )   {
              bool need_to_add = true;
              for(Leaves::iterator j=leaves.begin(); j != leaves.end(); ++j )  {
                std::string curr_leaf = containerName(*j);
                if ( curr_leaf == ref->container )  {
                  need_to_add = false;
                  break;
                }
              }
              if ( need_to_add )  {
                IOpaqueAddress* pA= 0;
                if ( ref->clid == CLID_StatisticsDirectory ||
                     ref->clid == CLID_StatisticsFile      || 
                     ref->clid == CLID_RowWiseTuple        ||
                     ref->clid == CLID_ColumnWiseTuple      )
                {
                  std::string spar[]   = { fname, ref->container};
                  unsigned long ipar[] = { ~0x0, ~0x0 };
                  status = m_dbMgr->createAddress(repSvcType(),
                                                  ref->clid,
                                                  spar,
                                                  ipar,
                                                  pA);
                }
                if ( status.isSuccess() )  {
                  std::string top = topLevel(pReg);
                  std::string leaf_name = top + ref->container.substr(7);
                  status = m_dataMgr->registerAddress(leaf_name, pA);
                  if ( status.isSuccess() )  {
		    log << MSG::DEBUG << "Created address for " << leaf_name << " of type " << ref->clid << endmsg;
                    continue;
                  }
                  makeError("Failed to register leaves to directory:"+ident,false).ignore();
                  break;
                }
                makeError("Failed to create leave address to directory:"+ident,false).ignore();
                break;
              }
            }
            makeError("Failed to add leaves to directory:"+ident,false).ignore();
            break;
          }
        }
      }
      else  {
        return makeError("Failed to access required tuple data description.",false);
      }
    }
  }
  for(REFS::iterator k = refs.begin(); k != refs.end(); ++k)  {
    if ( *k ) delete (*k);
  }
  return status;
}

/// Converter overrides: Update transient object from persistent data
StatusCode 
RootDirectoryCnv::updateObj(IOpaqueAddress* /* pAddr */,
			    DataObject*     /* pObj */)
{  
  return StatusCode::SUCCESS;
}

/// Converter overrides: Update persistent object representation.
StatusCode 
RootDirectoryCnv::updateRep(IOpaqueAddress* /* pAddr */,
			    DataObject*     /* pObj */)
{
  return StatusCode::SUCCESS;
}

/// Converter overrides: Update references of persistent object representation.
StatusCode 
RootDirectoryCnv::updateRepRefs(IOpaqueAddress* /* pAddr */,
				DataObject*     /* pObj */)
{
  return StatusCode::SUCCESS;
}

/// Converter overrides: Fill references of persistent object representation.
StatusCode 
RootDirectoryCnv::fillRepRefs(IOpaqueAddress* /* pAddr */,
			      DataObject*     /* pObj */)
{
  return StatusCode::SUCCESS;
}
