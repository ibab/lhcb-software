// $Id: LinksByKey.cpp,v 1.4 2004-05-18 11:44:18 ocallot Exp $
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
// Resolve the links, loading the containers if needed
//=========================================================================
void LinksByKey::resolveLinks ( IDataProviderSvc* eventSvc ) {
  int linkID = 0;
  LinkManager::Link* link;
  while ( 0 != (link = linkMgr()->link( linkID ) ) ) {
    if ( 0 == link->object() ) {
      SmartDataPtr<DataObject> tmp( eventSvc, link->path() );
      link->setObject( tmp );
    }
    linkID++;
  }
}
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

  int indx;
  if ( !findIndex( srcKey, indx ) ) {
     m_keyIndex.push_back( std::pair<int,int>( 0, 0 ) );
     int iL = m_keyIndex.size() - 1;
     while ( iL > indx ) {
       m_keyIndex[iL] = m_keyIndex[iL-1];
       --iL;
     }
     m_keyIndex[indx] = std::pair<int,int>( srcKey, refNum );
     
     //== Test of proper ordering
     
     //int before = -1;
     //for ( unsigned int kk = 0 ; m_keyIndex.size() > kk ; kk++ ) {
     //  if ( before >= m_keyIndex[kk].first ) {
     //    std::cout << "=== Not sorted Link key " << srcKey
     //              << " index inserted " << indx
     //              << " fault at " << kk
     //              << " before = " << before
     //              << " current " <<  m_keyIndex[kk].first
     //              << std::endl;
     //  }
     //  before = m_keyIndex[kk].first;
     //}

     return;
  }

  int prevIndex = m_keyIndex[indx].second;
  if ( m_increasing ) {
    if ( weight < m_linkReference[prevIndex].weight() ) {
      m_linkReference[refNum].setNextIndex( prevIndex );
      m_keyIndex[indx].second = refNum;
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
      m_keyIndex[indx].second = refNum;
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
  
  int index;
  if ( findIndex( key, index ) ) {
    reference = m_linkReference[ m_keyIndex[index].second ];
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
    std::vector<std::pair<int,int> >::const_iterator& iter ) {
  iter = m_keyIndex.begin();
  reference.setNextIndex( -1 );  // Indicate to restart at this iter's content
  return nextSource( reference, iter );
}

//=========================================================================
// Returns the next key for which the specified reference exists 
//=========================================================================
int LinksByKey::nextSource ( LinkReference& reference, 
    std::vector<std::pair<int,int> >::const_iterator& iter ) {
  LinkReference* temp;
  int refNum = reference.nextIndex();  // next entry
  while ( iter != m_keyIndex.end() ) {
    if ( 0 > refNum ) refNum = (*iter).second;  // first of this iter
    while( 0 <= refNum ) {
      temp = &m_linkReference[ refNum ];
      if ( temp->linkID()    == reference.linkID() &&
           temp->objectKey() == reference.objectKey() ) {
        reference = *temp;              // keep track of where we stopped
        int key = (*iter).first;
        if ( 0 > temp->nextIndex() ) iter++; // If last for entry, go to next
        return key;
      }
      refNum = temp->nextIndex();
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

//=========================================================================
//  Find the index of a given key in m_keyIndex. False if not found.
//=========================================================================
bool LinksByKey::findIndex ( int key, int& index) {

  // binary search
  int iF = 0;
  int iL = m_keyIndex.size() - 1;
  while ( iF <= iL ) {
    int iM = (iF + iL ) / 2;
    int tmpKey = m_keyIndex[iM].first;
    if ( key == tmpKey ) {
      index = iM;
      return true;
    } else if ( key < tmpKey ) {
      iL = iM-1;
    } else {
      iF = iM+1;
    }
  }
  index = iF;
  return false;
}

//=============================================================================
