// $Id: LinksByKey.cpp,v 1.1.1.1 2004-01-08 12:24:33 ocallot Exp $
// Include files 

#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/LinkManager.h"

// local
#include "Event/LinksByKey.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LinksByKey
//
// 2004-01-06 : Olivier Callot
//-----------------------------------------------------------------------------

//=========================================================================
//  Add a reference for a given key and container link.
//=========================================================================
void LinksByKey::addReference ( int srcKey, int srcLinkID,
                                int destKey, int destLinkID, 
                                double weight ) {


  //== Create the LinkReference, and push it in the vector

  LinkReference temp( srcLinkID, destLinkID, destKey, -1, weight);
  m_linkReference.push_back( temp );
  unsigned int refNum = m_linkReference.size() - 1;

  //== Now get the map entry for this key, if any.

  std::map<int,int>::iterator itM = m_linkMap.find( srcKey );

  if ( m_linkMap.end() == itM ) {  //== New key -> add the entry
    m_linkMap[srcKey] = refNum;
    return;
  }
  
  int prevIndex =  (*itM).second;
  if ( m_increasing ) {
    if ( weight < m_linkReference[prevIndex].weight() ) {
      m_linkReference[refNum].setNextIndex( prevIndex );
      (*itM).second = refNum;
    } else {
      int nextIndex = m_linkReference[prevIndex].nextIndex();
      while ( 0 <= nextIndex ) {
        if ( weight < m_linkReference[nextIndex].weight() ) {
          m_linkReference[prevIndex].setNextIndex( refNum );
          m_linkReference[refNum].setNextIndex( nextIndex );
          break;
        }
        prevIndex = nextIndex;
        nextIndex = m_linkReference[prevIndex].nextIndex();
      }
      m_linkReference[prevIndex].setNextIndex( refNum );
    } 
  } else {
    if ( weight > m_linkReference[prevIndex].weight() ) {
      m_linkReference[refNum].setNextIndex( prevIndex );
      (*itM).second = refNum;
    } else {
      int nextIndex = m_linkReference[prevIndex].nextIndex();
      while ( 0 <= nextIndex ) {
        if ( weight > m_linkReference[nextIndex].weight() ) {
          m_linkReference[prevIndex].setNextIndex( refNum );
          m_linkReference[refNum].setNextIndex( nextIndex );
          break;
        }
        prevIndex = nextIndex;
        nextIndex = m_linkReference[prevIndex].nextIndex();
      }
      m_linkReference[prevIndex].setNextIndex( refNum );
    } 
  }
  
}
//=========================================================================
//  Returns the first reference for the given key
//=========================================================================
bool LinksByKey::firstReference ( int key, 
                                  const DataObject* container, 
                                  LinkReference& reference ) {
  int linkID = -1;           // Case with only a key
  if ( NULL != container ) {
    LinkManager::Link* link = linkMgr()->link( container );
    if ( 0 == link ) {
      return false;
    }
    linkID = link->ID();
  }
  
  std::map<int,int>::iterator itM;
  if ( m_linkMap.end() != ( itM = m_linkMap.find( key ) ) ) {
    reference = m_linkReference[ (*itM).second ];
    while ( linkID != reference.srcLinkID() ) {
      if ( 0 > reference.nextIndex() ) {
        return false;
      }
      reference = m_linkReference[reference.nextIndex()]; 
    }
    return true;
  }
  return false;
}


//=========================================================================
//  Returns the next reference from the specified reference. 
//  returns false if no more
//=========================================================================
bool LinksByKey::nextReference ( LinkReference& reference ) {
  if ( 0 > reference.nextIndex() ) return false;
  
  int linkID = reference.srcLinkID();
  reference = m_linkReference[reference.nextIndex()];
  while ( linkID != reference.srcLinkID() ) {
    if ( 0 > reference.nextIndex() ) {
      return false;
    }
    reference = m_linkReference[reference.nextIndex()]; 
  }
  return true;
}

//=========================================================================
// Returns the first key for which the specified reference exists 
//=========================================================================
int LinksByKey::firstSource ( LinkReference& reference, 
                              std::map<int,int>::const_iterator& iter ) {
  iter = m_linkMap.begin();
  reference.setNextIndex( -1 );  // Indicate to restart at this iter's content
  return nextSource( reference, iter );
}

//=========================================================================
// Returns the next key for which the specified reference exists 
//=========================================================================
int LinksByKey::nextSource ( LinkReference& reference, 
                             std::map<int,int>::const_iterator& iter ) {
  LinkReference temp;
  int refNum = reference.nextIndex();  // next entry
  while ( iter != m_linkMap.end() ) {
    if ( 0 > refNum ) refNum = (*iter).second;  // first of this iter
    while( 0 <= refNum ) {
      temp = m_linkReference[ refNum ];
      if ( temp.linkID()    == reference.linkID() &&
           temp.objectKey() == reference.objectKey() ) {
        reference = temp;              // keep track of where we stopped
        return (*iter).first;
      }
      refNum = temp.nextIndex();
    }
    iter++;
  }
  // indicate that we have finished: Key is no longer the one asked first !
  reference.setObjectKey( -1 );     
  return -1;
}

//=========================================================================
//  
//=========================================================================
int LinksByKey::linkID ( const DataObject* obj ) {
  int id;
  LinkManager::Link* link = linkMgr()->link( obj );
  if ( 0 == link ) {
    id = linkMgr()->addLink( obj->registry()->identifier(), obj );
  } else {
    id = link->ID();
  }
  return id;
}
//=============================================================================
