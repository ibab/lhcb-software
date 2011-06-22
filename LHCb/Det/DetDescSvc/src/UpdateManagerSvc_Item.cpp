// Include files

#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IRegistry.h"

#include "DetDesc/ValidDataObject.h"
#include "DetDesc/Condition.h"

// local
#include "UpdateManagerSvc.h"

//-----------------------------------------------------------------------------
// Implementation file for class : UpdateManagerSvc::Item
//
// 2006-10-23 : Marco Clemencic
//-----------------------------------------------------------------------------

//=============================================================================
// Destructor
//=============================================================================
UpdateManagerSvc::Item::~Item() {
  for (MembFuncList::const_iterator mi = memFuncs.begin();
       mi != memFuncs.end(); ++mi) {
    delete mi->mf;
    delete mi->items;
  }
  // I'm the owner of the list of user's pointer setters: delete them!
  for (UserPtrList::iterator pi = user_dest_ptrs.begin();
       pi != user_dest_ptrs.end(); ++pi){
    delete pi->first;
  }
}
//=============================================================================
// Main method. Used to update the object and all the used ones.
//=============================================================================
StatusCode UpdateManagerSvc::Item::update(IDataProviderSvc *dp,const Gaudi::Time &when, MsgStream &log, bool inInit){
  if( log.level() <= MSG::DEBUG ) {
    log << MSG::DEBUG << "Updating (Item* " << this << ") " << ptr << " ---BEGIN---";
    if (!path.empty()) log << " " << path;
    log << endmsg;
    if( log.level() <= MSG::VERBOSE )
      log << MSG::VERBOSE << "    initial validity: " << since << " -> " << until << endmsg;
  }
  
  if (updateLock) {
    if( log.level() <= MSG::VERBOSE )
      log << MSG::VERBOSE << "Update lock found: break loop" << endmsg;
    return StatusCode::SUCCESS;
  }
  // check validity
  if (isValid(when)) {
    if( log.level() <= MSG::VERBOSE )
      log << MSG::VERBOSE << "Item valid, not need to update" << endmsg;
    return StatusCode::SUCCESS;
  }
  // prepare for update
  updateLock = true;
  resetIOV();
  StatusCode sc(StatusCode::SUCCESS, true);
  // start real update
  if (ptr == NULL && vdo == NULL && !path.empty()) { // I do not have a VDO ptr (or a void*) but a path: load it

    if( log.level() <= MSG::DEBUG )
      log << MSG::DEBUG << "Retrieve object " << path << " from data provider" << endmsg;

    DataObject  *pObj;
    sc = dp->retrieveObject(path, pObj);

    if (!sc.isSuccess()){
      log << MSG::ERROR << "Cannot retrieve " << path << " from data provider!" << endmsg;
      return sc;
    }

    // Check if the requested condition is in the override list.
    if ( override ) {
      // yes, it is!
      // First I check if I can update in place the object
      ValidDataObject * vdo = dynamic_cast<ValidDataObject *>(pObj);
      if (vdo) { // Good, we can update in place
        vdo->update(*override);
        Condition * cond = dynamic_cast<Condition *>(pObj);
        if (cond) { // A condition needs to be initialized too
          sc = cond->initialize();
          if ( !sc.isSuccess() ) {
            log << MSG::ERROR
                << "Unable to initialize overridden condition at "
                << path << endmsg;
            return sc;
          }
        }
        // to avoid memory leaks, I have to delete the overriding object
        delete override;
        //override->release();
        override = NULL;
      } else { // I cannot update the object, so I replace it.
        //   let's unregister the original object
        sc = dp->unregisterObject(pObj);
        if ( !sc.isSuccess() ) {
          log << MSG::ERROR << "Unable to unregister object at " << path << endmsg;
          return sc;
        }
        //   and delete it
        pObj->release();
        //   Now I can register the user specified one
        pObj = override;
        sc = dp->registerObject(path,pObj);
        if ( !sc.isSuccess() ) {
          log << MSG::ERROR << "Unable to register override object to " << path << endmsg;
          return sc;
        }
        // I do not need to delete the overriding object because now it belongs to the T.S.
      }
    }

    // Set also internal pointers
    sc = setPointers(pObj);
    if ( !sc.isSuccess() ) {
      log << MSG::ERROR << "Failure setting the pointers for object at " << path << endmsg;
      return sc;
    }

    // try to get the path to CondDB folder
    IOpaqueAddress *pAddr = pObj->registry()->address();
    if (pAddr != NULL) {
      if (pAddr->svcType() == CONDDB_StorageType) {
        // it comes from the cond db, so I can find its path
        db_path = pAddr->par()[0];
      }
    }
  }
  if (vdo != NULL && !vdo->isValid(when)){ // I have a VDO ptr and the object is not valid
    // Note: this may happen even if we just retrieved the object
    // (see bug #80076, https://savannah.cern.ch/bugs/?80076)
    if (!inInit) { // during initialization we may have a loaded object that doesn't have a valid
                   // IOV because of the HeartBeat, so it is actually OK and doesn't need to be updated.
      if( log.level() <= MSG::DEBUG )
        log << MSG::DEBUG << "Update object " << path << " from data provider" << endmsg;
      sc = vdo->update(); // only if I didn't load it
      if ( !sc.isSuccess() ) {
        log << MSG::ERROR << "Cannot update " << path << " from data provider!" << endmsg;
        return sc;
      }
    }
    // Set also internal pointers (needed here too, because it is needed in a very special case... a test)
    sc = setPointers(vdo);
    if ( !sc.isSuccess() ) {
      log << MSG::ERROR << "Failure setting the pointers for object at " << path << endmsg;
      return sc;
    }
  }
  if (vdo != NULL) { // it is a valid data object and should be up-to-date: align validity
    // no check because it shouldn't be necessary
    since = vdo->validSince();
    until = vdo->validTill();
  }
  // object internal data are up-to-date, now check what it depends on
  if( log.level() <= MSG::VERBOSE )
    log << MSG::VERBOSE << "Enter dependencies update loop" << endmsg;
  for (MembFuncList::iterator mfIt = memFuncs.begin(); mfIt != memFuncs.end(); ++mfIt){
    if (!mfIt->isValid(when)) { // only if one the children of the member function need an update
      size_t n = mfIt - memFuncs.begin();
      if( log.level() <= MSG::VERBOSE )
        log << MSG::VERBOSE << "Loop over dependencies of m.f. " << n << endmsg;
      mfIt->resetIOV();
      for (ItemList::iterator itemIt = mfIt->items->begin(); itemIt != mfIt->items->end(); ++itemIt){
        sc = (*itemIt)->update(dp, when, log, inInit);
        if (!sc.isSuccess()) return sc;
        // child item updated, update mf's IOV
        if (mfIt->since < (*itemIt)->since) mfIt->since = (*itemIt)->since;
        if (mfIt->until > (*itemIt)->until) mfIt->until = (*itemIt)->until;
      }
      if( log.level() <= MSG::VERBOSE )
        log << MSG::VERBOSE << "Call m.f. " << n << endmsg;
      sc = (*(mfIt->mf))();
      if (!sc.isSuccess()){
        if( log.level() <= MSG::DEBUG )
          log << MSG::DEBUG << "m.f. " << n << " returned a failure" << endmsg;
        return sc;
      }
    }
    // update the overall validity even if the M.F. was not called
    if (since < mfIt->since) since = mfIt->since;
    if (until > mfIt->until) until = mfIt->until;
  }
  updateLock = false;
  if( log.level() <= MSG::DEBUG ) {
    if( log.level() <= MSG::VERBOSE )
      log << MSG::VERBOSE << "    final validity: " << since << " -> " << until << endmsg;
    log << MSG::DEBUG << "Updating (Item* " << this << ") " << ptr << " ---END---";
    if (!path.empty()) log << " " << path;
    log << endmsg;
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
// Adds a child item to the given member function
//=============================================================================
BaseObjectMemberFunction * UpdateManagerSvc::Item::addChild(BaseObjectMemberFunction *thisMF, Item *child) {
  MembFuncList::iterator mfIt = find(thisMF);
  if (mfIt == memFuncs.end()) {
    mfIt = memFuncs.insert(mfIt,MembFunc(thisMF));
  } else {
    if (mfIt->mf != thisMF)
      delete thisMF;
  }
  if (std::find(mfIt->items->begin(),mfIt->items->end(),child) == mfIt->items->end()){
    // it is a new child (not in current m.f. list)
    mfIt->items->push_back(child);

    // intersect M.F. validity with the new child
    if (mfIt->since < child->since) mfIt->since = child->since;
    if (mfIt->until > child->until) mfIt->until = child->until;

    // add the new child to the list of children if not already included
    if (std::find(children.begin(),children.end(),child) == children.end()){
      children.push_back(child);
    }

    // intersect the item validity with the one of the member function
    if (since < mfIt->since) since = mfIt->since;
    if (until > mfIt->until) until = mfIt->until;

  }
  // return the real pointer since thisMF can be deleted
  return mfIt->mf;
}

//=============================================================================
// Clean up the tree
//=============================================================================
void UpdateManagerSvc::Item::purge(MsgStream *log) {
  if ( ! path.empty() ) {
    if( log && (*log).level() <= MSG::DEBUG )
      (*log) << MSG::DEBUG << "Purging " << path << endmsg;
    // Clean up pointers
    const bool force = true;
    setPointers(NULL,force).ignore();
  } else {
    if( log && (*log).level() <= MSG::DEBUG )
      (*log) << MSG::DEBUG << "Purging object at " << ptr << endmsg;
  }

  // I'm not sure I need this
  // invalidate();
  // or just this
  since = 1;
  until = 0;
}
//=============================================================================
