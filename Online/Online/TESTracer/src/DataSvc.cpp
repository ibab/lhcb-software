// Framework include files
#include "GaudiKernel/DataSvc.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/DataIncident.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/RegistryEntry.h"
#include "ITESTracer.h"

#define CAST_REGENTRY(x,y) dynamic_cast<x>(y)
//#define CAST_REGENTRY(x,y) (x)(y)

typedef DataSvcHelpers::RegistryEntry Reg;
typedef DataObject Obj;
typedef IOpaqueAddress Addr;
typedef DataSvc DS;
typedef StatusCode SC;
using namespace std;

/** IDataManagerSvc: Remove all data objects below the sub tree
 *  identified by its full path name.
 */
SC DS::clearSubTree(const string& sub_tree_path)    {
  Obj* pObject = 0;
  SC status = findObject(sub_tree_path, pObject);
  return status.isSuccess() ? clearSubTree(pObject) : status;
}

/** IDataManagerSvc: Remove all data objects below the sub tree
 *  identified by the object.
 */
SC DS::clearSubTree(Obj* pObject)    {
  if ( checkRoot() )  {
    Reg* entry = CAST_REGENTRY(Reg*,pObject->registry());
    if ( 0 != entry )   {
      Reg* parent = entry->parentEntry();
      if ( 0 != parent )  {
	_TRACE_TES(this,TRACE_CLEAR,entry);
        parent->remove(entry);
        return SUCCESS;
      }
      return INVALID_PARENT;
    }
    return INVALID_OBJECT;
  }
  return INVALID_ROOT;
}

/// IDataManagerSvc: Remove all data objects in the data store.
SC DS::clearStore()   {
  if ( checkRoot() )    {
    _TRACE_TES(this,TRACE_CLEAR_ROOT,m_root);
    m_root->release();
    m_root = 0;
    return SUCCESS;
  }
  return INVALID_ROOT;
}

/// IDataManagerSvc: Analyse by traversing all data objects below the sub tree
SC DS::traverseSubTree(Obj* pObject,IDataStoreAgent* pAgent)  {
  if ( checkRoot() )    {
    Reg* entry = CAST_REGENTRY(Reg*,pObject->registry());
    if ( 0 != entry )   {
      _TRACE_TES(this,TRACE_TRAVERSE,entry);
      return entry->traverseTree(pAgent);
    }
    return INVALID_OBJECT;
  }
  return INVALID_ROOT;
}

/// IDataManagerSvc: Analyse by traversing all data objects in the data store.
SC DS::traverseTree(IDataStoreAgent* pAgent)   {
  if ( checkRoot() )    {
    _TRACE_TES(this,TRACE_TRAVERSE,m_root);
    return m_root->traverseTree(pAgent);
  }
  return INVALID_ROOT;
}

/**
 * Initialize data store for new event by giving new event path and root
 * object. Does not clear the store before reinitializing it. This could
 * lead to errors and should be handle with care. Use setRoot if unsure
 */
SC DS::i_setRoot(const string& root_path, Obj* pRootObj) {
  if ( 0 != pRootObj )  {
    m_root = new Reg(root_path);
    m_root->makeHard(pRootObj);
    m_root->setDataSvc(this);
    _TRACE_TES(this,TRACE_SETROOT,m_root);
    preLoad().ignore();
  }
  return SUCCESS;
}

/**
 * Initialize data store for new event by giving new event path and address
 * of root object. Does not clear the store before reinitializing it. This
 * could lead to errors and should be handle with care. Use setRoot if unsure
 */
SC DS::i_setRoot(const string& root_path,Addr* pRootAddr) {
  if ( 0 != pRootAddr )  {
    m_root = new Reg(root_path);
    m_root->makeHard(pRootAddr);
    m_root->setDataSvc(this);
    _TRACE_TES(this,TRACE_SETROOT,m_root);
    preLoad().ignore();
  }
  return SUCCESS;
}

///  IDataManagerSvc: Register object address with the data store.
SC DS::registerAddress(const string& fullPath,Addr* pAddress)   {
  _TRACE_TES(this,TRACE_REGADDR,0,fullPath);
  if ( fullPath.length() > 0 )    {
    if ( fullPath[0] != SEPARATOR )   {
      return registerAddress(m_root, fullPath, pAddress);
    }
    IRegistry* pRegistry = 0;
    return registerAddress(pRegistry, fullPath, pAddress);
  }
  return INVALID_OBJ_PATH;
}

///  IDataManagerSvc: Register object address with the data store.
SC DS::registerAddress(IRegistry* parentObj,
		       const string& objPath,
		       Addr* pAddress)   {

  if ( checkRoot() )  {
    if ( objPath.length() > 0 )   {
      if ( 0 == parentObj )   {
        if ( objPath[0] != SEPARATOR )    {
          return registerAddress(m_root, objPath, pAddress);
        }
        string::size_type sep = objPath.find(SEPARATOR,1);
        if ( sep != string::npos )    {
          string p_path (objPath, 0, sep);
          if ( p_path == m_rootName )   {
            string o_path (objPath, sep, objPath.length());
            return registerAddress(m_root, o_path, pAddress);
          }
        }
        return INVALID_PARENT;
      }
      if ( objPath[0] != SEPARATOR )    {
        string path;
        path  = SEPARATOR;
        path += objPath;
        return registerAddress(parentObj, path, pAddress);
      }
      Reg* par_entry = CAST_REGENTRY(Reg*,parentObj);
      if ( 0 != par_entry )   {
        string::size_type sep = objPath.rfind(SEPARATOR);
        if ( sep > 0 && sep != string::npos )    {
          string p_path (objPath, 0, sep);
          string o_path (objPath, sep, objPath.length());
          Reg* p_entry = par_entry->findLeaf(p_path);
          // Create default object leafs if the
          // intermediate nodes are not present
          if ( 0 == p_entry && m_forceLeaves )    {
            Obj *pLeaf = createDefaultObject();
            SC sc = registerObject(par_entry->identifier(),
                                           p_path,
                                           pLeaf);
            if ( ! sc.isSuccess() )   {
              delete pLeaf;
            }
            p_entry = par_entry->findLeaf(p_path);
          }
          if ( 0 != p_entry )  {
            return registerAddress(p_entry, o_path, pAddress);
          }
          return INVALID_PARENT;
        }
        SC status = par_entry->add(objPath, pAddress);
        if ( status.isSuccess() )   {
	  _TRACE_TES(this,TRACE_REGADDR,pAddress->registry());
          return status;
        }
        return DOUBL_OBJ_PATH;
      }
      return INVALID_PARENT;
    }
    return INVALID_OBJ_PATH;
  }
  return INVALID_ROOT;
}

///  IDataManagerSvc: Unregister object address from the data store.
SC DS::unregisterAddress(IRegistry* pParent,const string& objPath) {
  if ( checkRoot() )  {
    if ( objPath.length() > 0 )   {
      if ( 0 == pParent )   {
        if ( objPath[0] != SEPARATOR )    {
          return unregisterAddress(m_root, objPath);
        }
        string::size_type sep = objPath.find(SEPARATOR,1);
        if ( sep != string::npos )    {
          string p_path (objPath, 0, sep);
          if ( p_path == m_rootName )   {
            string o_path (objPath, sep, objPath.length());
            return unregisterAddress(m_root, o_path);
          }
        }
        return INVALID_PARENT;
      }
      if ( objPath[0] != SEPARATOR )    {
        string path;
        path  = SEPARATOR;
        path += objPath;
        return unregisterAddress(pParent, path);
      }
      Reg* node_entry = CAST_REGENTRY(Reg*,pParent);
      if ( 0 != node_entry )   {
        Reg* leaf_entry = node_entry->findLeaf(objPath);
        if ( 0 != leaf_entry )    {
          string::size_type sep = objPath.rfind(SEPARATOR);
          if ( sep > 0 && sep != string::npos )    {
            string path = objPath.substr(sep);
            return unregisterAddress(leaf_entry->parent(), path);
          }
          SC status = node_entry->remove(objPath);
          if ( status.isSuccess() )   {
	    _TRACE_TES(this,TRACE_UNREGADDR,leaf_entry);
            return status;
          }
        }
      }
      return INVALID_PARENT;
    }
    return INVALID_OBJ_PATH;
  }
  return INVALID_ROOT;
}

/// Register object with the data store.
SC DS::registerObject (const string& fullPath,DataObject* pObject)   {
  _TRACE_TES(this,TRACE_REGOBJ,0,fullPath);
  return registerObject(0, fullPath, pObject);
}

/// Register object with the data store.
SC DS::registerObject(Obj* parentObj,
		      const string& objPath,
		      Obj* pObject)   {
  if ( checkRoot() )  {
    if ( 0 == parentObj )   {
      if ( objPath.length() > 0 )   {
        if ( objPath[0] == SEPARATOR )    {
          string::size_type sep = objPath.find(SEPARATOR,1);
          if ( sep != string::npos )    {
            string p_path (objPath, 0, sep);
            string o_path (objPath, sep, objPath.length());
            return registerObject(p_path, o_path, pObject);
          }
        }
        else  {
          return registerObject(m_rootName, objPath, pObject);
        }
      }
      return INVALID_OBJ_PATH;
    }
    Reg* node_entry = CAST_REGENTRY(Reg*,parentObj->registry());
    if ( 0 != node_entry )   {
      SC status = INVALID_PARENT;
      string::size_type sep = objPath.find(SEPARATOR,1);
      if ( sep != string::npos )    {
        string p_path (objPath, 0, sep);
        string o_path (objPath, sep, objPath.length());
        Reg* par_entry = node_entry->findLeaf(p_path);
        // Create default object leafs if the
        // intermediate nodes are not present
        if ( 0 == par_entry && m_forceLeaves )    {
          Obj *pLeaf = createDefaultObject();
          SC sc = registerObject(parentObj, p_path, pLeaf);
          if ( ! sc.isSuccess() )   {
            delete pLeaf;
          }
          par_entry = node_entry->findLeaf(p_path);
        }
        else if ( 0 != par_entry && par_entry->object() == 0 )  {
          status = retrieveEntry( node_entry, p_path, par_entry);
          if ( !status.isSuccess() && !par_entry->address() && m_forceLeaves )  {
            Obj *pLeaf = createDefaultObject();
            SC sc = registerObject(parentObj, p_path, pLeaf);
            if ( ! sc.isSuccess() )   {
              delete pLeaf;
            }
            par_entry = node_entry->findLeaf(p_path);
          }
        }
        node_entry = par_entry;
        if ( 0 != node_entry )    {
          Obj* obj = node_entry->object();
          if ( 0 != obj )   {
            status = registerObject( obj, o_path, pObject );
          }
        }
      }
      else   {
        Reg* leaf = node_entry->findLeaf(objPath);
        if ( 0 == leaf )    {
          status = node_entry->add( objPath, pObject );
	  _TRACE_TES(this,TRACE_REGOBJ,pObject->registry());
        }
        else    {
          Obj* obj = leaf->object();
          if ( 0 == obj )    {
            if (0 == pObject) {
              MsgStream log(msgSvc(), name());
              log << MSG::ERROR
                  << "registerObject: trying to register null Obj" << endmsg;
              return SC::FAILURE;
            }
            else  {
              pObject->setRegistry(leaf);
            }
            leaf->setAddress(0);
            leaf->setObject(pObject);
	    _TRACE_TES(this,TRACE_REGOBJ, leaf);
            status = SC::SUCCESS;
          }
          else  {
            status = DOUBL_OBJ_PATH;
          }
        }
      }
      return status;
    }
    return INVALID_PARENT;
  }
  return INVALID_ROOT;
}

/// Unregister object from the data store.
SC DS::unregisterObject(const string& fullPath)   {
  Obj* pObject = 0;
  SC status = findObject(fullPath, pObject);
  if ( status.isSuccess() )   {
    Reg* pEntry = CAST_REGENTRY(Reg*,pObject->registry());
    if ( 0 != pEntry )    {
      if ( pEntry->isEmpty() )    {
        Reg* pParent = pEntry->parentEntry();
        if ( 0 != pParent )   {
          if ( 0 != pObject )    {
            pObject->addRef();
          }
	  _TRACE_TES(this,TRACE_UNREGOBJ, pEntry);
          pParent->remove(pEntry);
          return SC::SUCCESS;
        }
        return INVALID_PARENT;
      }
      return DIR_NOT_EMPTY;
    }
    return INVALID_ROOT;
  }
  return status;
}

/// Unregister object from the data store.
SC DS::unregisterObject(Obj* pObject)    {
  if ( checkRoot() )  {
    Reg* entry = m_root->findLeaf(pObject);
    if ( 0 != entry )   {
      Reg* parent = entry->parentEntry();
      if ( 0 != parent )    {
        if ( entry->isEmpty() )    {
          if ( 0 != entry->object() )    {
            entry->object()->addRef();
          }
          if ( 0 != parent )  {
	    _TRACE_TES(this,TRACE_UNREGOBJ, entry);
            parent->remove(entry);
          }
          return SUCCESS;
        }
        return INVALID_PARENT;
      }
      return DIR_NOT_EMPTY;
    }
    return INVALID_OBJECT;
  }
  return INVALID_ROOT;
}

/// Unregister object from the data store.
SC DS::unregisterObject (Obj* pParentObj,const string& objectPath) {
  if ( checkRoot() )  {
    try   {
      Reg* parent = CAST_REGENTRY(Reg*,pParentObj->registry());
      if ( 0 != parent )   {
        Reg* entry = parent->findLeaf(objectPath);
        if ( 0 != entry )   {
          if ( entry->isEmpty() )    {
            if ( 0 != entry->object() )    {
              entry->object()->addRef();
            }
	    _TRACE_TES(this,TRACE_UNREGOBJ, entry);
            parent->remove(entry);
            return SUCCESS;
          }
          return DIR_NOT_EMPTY;
        }
        return INVALID_OBJECT;
      }
    }
    catch(...) {
    }
    return INVALID_PARENT;
  }
  return INVALID_ROOT;
}

/** Invoke data fault handling if enabled
  */
Obj* DS::handleDataFault(IRegistry* pReg, const string& path)   {
  if ( m_enableFaultHdlr )  {
    IRegistry* pLeaf = 0;
    if ( pReg && path.length() == 0 )   {
      DataIncident incident(name(), m_faultName, pReg->identifier());
      _TRACE_TES(this,TRACE_DATAFAULT,pReg,path);
      m_incidentSvc->fireIncident(incident);
      { _TRACE_TES(this,pReg->object() ? TRACE_DATAFAULT_OK : TRACE_DATAFAULT_FAIL,pReg,path); }
      return pReg->object();
    }
    else if ( pReg )  {
      string p = pReg->identifier();
      if (path[0] != SEPARATOR ) p += SEPARATOR;
      p += path;
      DataIncident incident(name(), m_faultName, p);
      _TRACE_TES(this,TRACE_DATAFAULT,pReg,path);
      m_incidentSvc->fireIncident(incident);
      pLeaf = m_root->findLeaf(p);
    }
    else  {
      string p = m_root->identifier();
      if (path[0] != SEPARATOR ) p += SEPARATOR;
      p += path;
      DataIncident incident(name(), m_faultName, p);
      _TRACE_TES(this,TRACE_DATAFAULT,pReg,path);
      m_incidentSvc->fireIncident(incident);
      pLeaf = m_root->findLeaf(p);
    }
    if ( pLeaf )  {
      _TRACE_TES(this,pLeaf->object() ? TRACE_DATAFAULT_OK : TRACE_DATAFAULT_FAIL,pReg,path);
      return pLeaf->object();
    }
    _TRACE_TES(this,TRACE_DATAFAULT_FAIL,pReg,path);
  }
  return 0;
}

/** Invoke Persistency service to create transient object from its
 *  persistent representation
 */
SC DS::loadObject(IConversionSvc* pLoader, IRegistry* pRegistry) {
  SC status = INVALID_OBJ_ADDR;
  Obj* pObject = 0;
  if ( 0 == pLoader )   {         // Precondition: Data loader must be present
    if (handleDataFault(pRegistry) != 0) return  SUCCESS;
    else                                 return  NO_DATA_LOADER;
  }
  if ( 0 == pRegistry )    {      // Precondition: Directory must be valid
    if (handleDataFault(pRegistry) != 0) return  SUCCESS;
    else                                 return  INVALID_OBJ_ADDR;
  }

  MsgStream log( msgSvc(), name() );
  log << MSG::VERBOSE << "Requested object " << pRegistry->identifier() << endmsg;

  if ( m_enableAccessHdlr )  {
    // Fire data access incident
    DataIncident incident(name(), m_accessName, pRegistry->identifier());
    m_incidentSvc->fireIncident(incident);
  }
  if ( m_inhibitPathes.size() > 0 )   {
    const string& ident = pRegistry->identifier();
    vector<string>::iterator inhibit =
      find(m_inhibitPathes.begin(), m_inhibitPathes.end(), ident);
    if ( inhibit != m_inhibitPathes.end() )   {
      return NO_ACCESS;
    }
  }
  Addr* pAddress = pRegistry->address();
  if ( 0 == pAddress )  {         // Precondition:
    return INVALID_OBJ_ADDR;      // Address must be valid
  }
  try   {
    status = pLoader->createObj(pAddress, pObject);  // Call data loader
    if ( status.isSuccess() )    {

      log << MSG::VERBOSE << "Object " << pRegistry->identifier() << " created" << endmsg;

      Reg *pEntry = CAST_REGENTRY(Reg*,pRegistry);
      pEntry->setObject(pObject);

      log << MSG::VERBOSE << "Filling object " << pRegistry->identifier() << endmsg;
      status = pLoader->fillObjRefs(pAddress, pObject);
      _TRACE_TES(this,TRACE_LOAD,pEntry);
    }
  }
  catch( const GaudiException& exc ) {
    if ( handleDataFault(pRegistry) != 0 )  {
      return SUCCESS;
    }
    throw GaudiException("GaudiException in loadObject() " + pRegistry->identifier(),
        name(), SC::FAILURE, exc);
  }
  catch( const exception& x) {
    if ( handleDataFault(pRegistry) != 0 )  {
      return SUCCESS;
    }
    throw GaudiException("exception in loadObject() " + pRegistry->identifier() +
        ": " + System::typeinfoName(typeid(x)) + ", " + x.what(),
      name(), SC::FAILURE);
  }
  catch(...) {
    if ( handleDataFault(pRegistry) != 0 )  {
      return SUCCESS;
    }
    throw GaudiException("UNKN exception in loadObject() " + pRegistry->identifier(),
      name(), SC::FAILURE);
  }
  if ( !status.isSuccess() )  {
    if ( handleDataFault(pRegistry) != 0 )  {
      return SC::SUCCESS;
    }
  }
  if ( status.isSuccess() ) {
    log << MSG::VERBOSE << "Object " << pRegistry->identifier() << " successfully loaded" << endmsg;
  }
  return status;
}

/// Retrieve object identified by its full path from the data store.
SC DS::retrieveObject(const string& fullPath,DataObject*& pObject)   {
  IRegistry* nullDir = 0;
  _TRACE_TES(this,TRACE_RETRIEVE,0, fullPath);
  return retrieveObject(nullDir, fullPath, pObject);
}

/// Retrieve object from data store.
SC DS::retrieveObject(DataObject* parentObj,const string& path,DataObject*& pObject)  {
  IRegistry* pRegistry = (0==parentObj) ? 0 : parentObj->registry();
  _TRACE_TES(this,TRACE_RETRIEVE, pRegistry, path);
  return retrieveObject(pRegistry, path, pObject);
}

/// Retrieve object identified by its directory from the data store.
SC DS::retrieveObject(IRegistry* pRegistry,const string& path,Obj*& pObject)   {
  pObject = 0;
  Reg *result = 0, *parent = CAST_REGENTRY(Reg*,pRegistry);
  SC status = retrieveEntry(parent, path, result);
  if ( status.isSuccess() )   {
    _TRACE_TES(this,TRACE_RETRIEVE,result);
    pObject = result->object();
  }
  return status;
}

/// Retrieve object identified by its directory from the data store.
SC DS::findObject(IRegistry* pRegistry,const string& path,Obj*& pObject)   {
  pObject = 0;
  IRegistry* pReg = (0==pRegistry) ? m_root : pRegistry;
  Reg* root_entry = CAST_REGENTRY(Reg*, pReg);
  if ( 0 != root_entry )    {
    if ( path.length() > 0 )  {
      pReg = root_entry->find(path);
    }
    if ( 0 == pReg )    {
      return INVALID_OBJ_PATH;
    }
    _TRACE_TES(this,TRACE_FIND,pReg);
    pObject = pReg->object();
  }
  return (0 == pObject) ? OBJ_NOT_LOADED : IDataProviderSvc_NO_ERROR;
}

/// Find object identified by its full path in the data store.
SC DS::findObject(const string& path,Obj*& pObject)   {
  pObject = 0;
  if ( checkRoot() )  {
    if ( path.length() == 0 || path == m_rootName )   {
      pObject = m_root->object();
      _TRACE_TES(this,TRACE_FIND,m_root);
      return (0 == pObject) ? OBJ_NOT_LOADED : IDataProviderSvc_NO_ERROR;
    }
    else if ( path[0] != SEPARATOR )    {
      return findObject(m_rootName, path, pObject);
    }
    return findObject((IRegistry*)0, path, pObject);
  }
  return INVALID_ROOT;
}

// Link object
SC DS::linkObject(IRegistry* from,const string& objPath, Obj* to) {
  if ( checkRoot() )  {
    try   {
      Reg* from_entry = CAST_REGENTRY(Reg*,from);
      if ( 0 != from_entry )    {
        // First check if both objects are already registered to the store
        Reg* to_entry   = m_root->findLeaf(to);
        if ( 0 == to_entry )   {
          return INVALID_OBJECT;
        }
        else  {
          string::size_type sep = objPath.rfind(SEPARATOR);
          if ( sep > 0 && sep != string::npos )    {   // in case the objPath is a sub-directory itself
            Obj* pO = 0;
            string fromPath(objPath, 0, sep);
            SC sc = retrieveObject(from, fromPath, pO);
            if ( sc.isSuccess() )    {
              string toPath(objPath, sep, objPath.length());
              sc = linkObject(pO->registry(), toPath, to);
            }
            return sc;
          }
          // Now register the soft link
          SC status = from_entry->add( objPath, to, true);
	  _TRACE_TES(this,TRACE_LINK,from_entry, objPath);
          return status.isSuccess() ?
            IDataProviderSvc_NO_ERROR : DOUBL_OBJ_PATH;
        }
      }
    }
    catch (...) {
    }
    return INVALID_PARENT;
  }
  return INVALID_ROOT;
}

/// Remove a link to another object.
SC DS::unlinkObject(IRegistry* from, const string& objPath)    {
  if ( checkRoot() )  {
    try   {
      Reg* from_entry = CAST_REGENTRY(Reg*,from);
      if ( 0 != from_entry )    {
        string::size_type sep = objPath.rfind(SEPARATOR);
        if ( sep > 0 && sep != string::npos )    {
	  // in case the objPath is a sub-directory itself
          Obj* pO = 0;
          string fromPath(objPath, 0, sep);
          SC sc = findObject(from, fromPath, pO);
          if ( sc.isSuccess() )    {
            string toPath(objPath, sep, objPath.length());
            sc = unlinkObject(pO->registry(), toPath);
          }
          return sc;
        }
        SC status = from_entry->remove( objPath );
        if ( status.isSuccess() )     {
	  _TRACE_TES(this,TRACE_UNLINK,from_entry, objPath);
          return status;
        }
        return INVALID_OBJ_PATH;
      }
    }
    catch (...)   {
    }
    return INVALID_PARENT;
  }
  return INVALID_ROOT;
}
