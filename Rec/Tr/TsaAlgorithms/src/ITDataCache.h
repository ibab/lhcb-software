// $Id: ITDataCache.h,v 1.2 2007-03-21 14:35:01 cattanem Exp $ 
#ifndef _ITDataCache_H
#define _ITDataCache_H

/** @class ITDataCache ITDataCache.h 
* Helper class to ITBoxProcessor
* Essentially it acts as a cache, making data access faster
* @author M. Needham
**/ 

#include "TsaKernel/STCluster.h"
#include "TsaKernel/Types.h"

class ITDataCache{

public:

  /// constructer
  ITDataCache(unsigned int layer);

  /// destructer
  virtual ~ITDataCache(){}

  /// configure
  void configure(Tsa::STRange range);

  /// empty flag
  bool isEmpty() const;

  /// begin
  Tsa::STClusters::const_iterator begin() const ;

  /// end
  Tsa::STClusters::const_iterator end() const ;
  
  /// layerID
  unsigned int layerID() const;
    
private:
 
  unsigned int m_layer;  
  bool m_empty;

  Tsa::STClusters::const_iterator m_begin;
  Tsa::STClusters::const_iterator m_end;
  
};

inline Tsa::STClusters::const_iterator ITDataCache::begin() const{
  return m_begin;
}

inline Tsa::STClusters::const_iterator ITDataCache::end() const{
  return m_end;
}

inline unsigned int ITDataCache::layerID() const{
  return m_layer;
}

inline bool ITDataCache::isEmpty() const{
  return m_empty;
}

#endif // _ITDataCache

