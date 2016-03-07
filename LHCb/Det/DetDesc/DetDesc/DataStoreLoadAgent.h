// $Id: DataStoreLoadAgent.h,v 1.1 2005-03-18 15:47:08 cattanem Exp $
#ifndef DATASTORELOADAGENT_H 
#define DATASTORELOADAGENT_H 1

// Include files
#include "GaudiKernel/IDataStoreAgent.h"

/** @class DataStoreLoadAgent DataStoreLoadAgent.h
 *
 *  Simple class that load all the objects od a transient store.  
 *
 *  @author Marco CLEMENCIC
 *  @date   2005-02-24
 */
class DataStoreLoadAgent: public IDataStoreAgent {
public: 
  /// Standard constructor
  DataStoreLoadAgent( ); 

  virtual ~DataStoreLoadAgent( ); ///< Destructor

  virtual bool 	analyse (IRegistry *pObject, int level);

  /**
   * Give the number of objects that were loaded.
   */
  inline int loadedObjects () const { return m_loadedObjects; }

  /**
   * Give the maximum depth reached navigating the tree.
   */
  inline int maxDepth () const { return m_maxDepth; }
  
  /**
   * Reset the counters. Useful when using the same instance many times.
   */
  
  inline void resetCounters() { m_loadedObjects = m_maxDepth = 0; }
  

protected:

private:
  int m_loadedObjects;
  int m_maxDepth;
};
#endif // DATASTORELOADAGENT_H
